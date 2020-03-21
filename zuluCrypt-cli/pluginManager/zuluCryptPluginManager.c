/*
 *
 *  Copyright (c) 2012-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <blkid/blkid.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

#include "libzuluCryptPluginManager.h"
#include "../utility/process/process.h"
#include "../utility/socket/socket.h"
#include "../utility/string/String.h"
#include "../constants.h"
#include "../bin/includes.h"
#include "../bin/libzuluCrypt-exe.h"

#define _ignore_result( x ) if( x ){;}

/*
 * below header file is created at config time.
 */
#include "plugin_path.h"

static void _debug_0( process_t p,ProcessIO std_io )
{
	char * e = NULL ;

	if( std_io == ProcessStdOut ){
		puts( "--------stdout------------" ) ;
	}else{
		puts( "--------stderror----------" ) ;
	}

	while( 1 ){
		ProcessGetOutPut( p,&e,std_io ) ;
		if( e ){
			printf( "%s",e ) ;
			fflush( stdout ) ;
			free( e ) ;
			e = NULL ;
		}else{
			break ;
		}
	}
}

static void _debug( process_t p )
{
	_debug_0( p,ProcessStdOut ) ;
	_debug_0( p,ProcessStdError ) ;
}

void zuluCryptGetKeyFromSocket( const char * sockpath,string_t * key,uid_t uid )
{
	ssize_t e ;
	char * buffer = NULL ;

	socket_t client ;
	socket_t server = SocketLocal( sockpath ) ;

	*key = StringVoid ;

	if( uid ){}

	errno = 0 ;

	if( SocketBind( server ) ){

		if( SocketListen( server ) ){

			client = SocketAccept( server ) ;

			/*
			 * ZULUCRYPT_INT_MAX_KEYSIZE is set in ../constants.h
			 */
			e = SocketGetData_1( client,&buffer,ZULUCRYPT_INT_MAX_KEYSIZE ) ;

			if( e == 0 ){

				*key = StringEmpty() ;

			}else if( e == -1 ){

				perror( "Failed to read data from socket" ) ;
			}else{
				*key = StringInheritWithSize( &buffer,(size_t)e,(size_t)e + 1 ) ;
			}

			SocketClose( &client ) ;
		}else{
			perror( "Socket listen failed" ) ;
		}

		SocketClose( &server ) ;
	}else{
		perror( "Socket bind failed" ) ;
	}
}

void * zuluCryptPluginManagerOpenConnection( const char * sockpath )
{
	int i ;
	socket_t client ;

	for( i = 0 ; i < 10 ; i++ ){

		client = SocketLocal( sockpath ) ;

		if( SocketConnect( &client ) ){

			return client ;
		}else{
			sleep( 1 ) ;
		}
	}

	printf( "Client: Failed to open connection at:%s\n",sockpath ) ;
	perror( "Reason:" ) ;

	return NULL ;
}

ssize_t zuluCryptPluginManagerSendKey( void * client,const char * key,size_t length )
{
	return SocketSendData( client,key,length ) ;
}

void zuluCryptPluginManagerCloseConnection( void * e )
{
	socket_t client = e ;
	SocketClose( &client ) ;
}

static void _create_path( const char * path,uid_t uid )
{
	if( mkdir( path,0700 ) == 0 ){

		_ignore_result( chmod( path,0700 ) )
		_ignore_result( chown( path,uid,uid ) )
	}
}

string_t zuluCryptPluginManagerGetKeyFromModule( const char * device,const char * plugin,
						 const char * uuid,
						 uid_t uid,
						 const struct_opts * opts,
						 const char * run_path,
						 int * r )
{
	process_t p ;
	struct stat st ;

	const char * sockpath ;
	const char * argv  = opts->argv ;
	char * const * env = opts->env ;
	const char * args[ 7 ] ;

	ProcessStructure * str ;

	string_t key   = StringVoid ;
	string_t plugin_path = StringVoid ;
	string_t path  = StringVoid ;

	struct passwd * pass = getpwuid( uid ) ;

	if( pass == NULL ){
		return key ;
	}
	/*
	 * ZULUCRYPTpluginPath is set at config time at it equals $prefix/lib(64)/zuluCrypt/
	 */
	plugin_path = String( ZULUCRYPTpluginPath ) ;

	plugin = plugin + StringLastIndexOfChar_1( plugin,'/' ) + 1 ;

	plugin = StringAppend( plugin_path,plugin ) ;

	if( stat( plugin,&st ) == 0 && S_ISREG( st.st_mode ) ) {

		_create_path( run_path,uid ) ;

		path = String( run_path ) ;
		sockpath = StringAppendInt( path,(u_int64_t)syscall( SYS_gettid ) ) ;

		*( args + 0 ) = plugin ;
		*( args + 1 ) = device ;

		if( uuid != NULL ){
			*( args + 2 ) = uuid ;
		}else{
			*( args + 2 ) = "Nil" ;
		}

		*( args + 3 ) = sockpath ;
		/*
		 * ZULUCRYPT_CHAR_MAX_KEYSIZE is set in ../constants.h
		 */
		*( args + 4 ) = ZULUCRYPT_CHAR_MAX_KEYSIZE ;
		*( args + 5 ) = argv ;
		*( args + 6 ) = NULL ;

		p = Process( NULL,NULL ) ;
		str = ProcessArgumentStructure( p ) ;
		str->args    = ( char * const * )args ;
		str->env     = env ;
		str->user_id = uid ;

		ProcessStart( p ) ;

		zuluCryptGetKeyFromSocket( sockpath,&key,uid ) ;

		*r = ProcessWaitUntilFinished( &p ) ;

		_debug( p ) ;
	}else{
		*r = 1 ;
	}

	StringMultipleDelete( &plugin_path,&path,NULL ) ;

	return key ;
}
