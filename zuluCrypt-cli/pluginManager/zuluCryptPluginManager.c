/*
 *
 *  Copyright (c) 2012
 *  name : mhogo mchungu
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

#include "libzuluCryptPluginManager.h"
#include "../utility/process/process.h"
#include "../utility/socket/socket.h"
#include "../utility/string/String.h"
#include "../constants.h"
#include "../bin/includes.h"
#include "../bin/libzuluCrypt-exe.h"

/*
 * below header file is created at config time.
 */
#include "plugin_path.h"
#include <stdio.h>

#define zuluCryptPluginManagerDebug 0

#if zuluCryptPluginManagerDebug
static void _debug( process_t p )
{
	char * e = NULL ;
	puts( "--------stdout------------" ) ;
	ProcessGetOutPut( p,&e,ProcessStdOut ) ;
	if( e ){
		puts( e ) ;
		fflush( stdout ) ;
		free( e ) ;
	}
	puts( "--------stderror----------" ) ;
	e = NULL ;
	ProcessGetOutPut( p,&e,ProcessStdError ) ;
	if( e ){
		puts( e ) ;
		fflush( stdout ) ;
		free( e ) ;
	}
}
#else
static void _debug( process_t p )
{
	if( p ){;}
}
#endif

size_t zuluCryptGetKeyFromSocket( const char * sockpath,string_t * key,uid_t uid )
{
	ssize_t dataLength = -1 ;
	char * buffer = NULL ;

	socket_t client ;
	socket_t server = SocketLocal( sockpath ) ;

	if( server ){
		if( SocketBind( server ) ){
			chown( sockpath,uid,uid ) ;
			chmod( sockpath,S_IRWXU | S_IRWXG | S_IRWXO ) ;
			if( SocketListen( server ) ){
				client = SocketAcceptWithTimeOut( server,10 ) ;
				if( client ){
					/*
					 * ZULUCRYPT_INT_MAX_KEYSIZE is set in ../constants.h
					 */
					dataLength = SocketGetData_1( client,&buffer,ZULUCRYPT_INT_MAX_KEYSIZE ) ;
					if( dataLength != -1 ){
						*key = StringInheritWithSize( &buffer,dataLength,dataLength + 1 ) ;
					}else{
						*key = StringEmpty() ;
					}
					SocketClose( &client ) ;
				}
			}
		}
		SocketClose( &server ) ;
	}
	return 0 ;
}

void * zuluCryptPluginManagerOpenConnection( const char * sockpath )
{
	int i ;
	socket_t client ;
	for( i = 0 ; i < 10 ; i++ ){
		client = SocketLocal( sockpath ) ;
		if( SocketConnect( &client ) ){
			return ( void * ) client ;
		}else{
			sleep( 1 ) ;
		}
	}
	return NULL ;
}

ssize_t zuluCryptPluginManagerSendKey( void * client,const char * key,size_t length )
{
	if( client == NULL ){
		return -1 ;
	}else{
		return SocketSendData( ( socket_t )client,key,length ) ;
	}
}

void zuluCryptPluginManagerCloseConnection( void * p )
{
	socket_t client = ( socket_t ) p ;
	if( p != NULL ){
		SocketClose( &client ) ;
	}
}

string_t zuluCryptPluginManagerGetKeyFromModule( const char * device,const char * plugin,
						 const char * uuid,uid_t uid,const struct_opts * opts )
{
	process_t p ;
	struct stat st ;

	const char * sockpath ;
	const char * argv = opts->argv ;
	const char * const * env = opts->env ;
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
		path = String( pass->pw_dir ) ;
		sockpath = StringAppend( path,"/.zuluCrypt-socket/" ) ;

		mkdir( sockpath,S_IRWXU | S_IRWXG | S_IRWXO ) ;
		chown( sockpath,uid,uid ) ;
		chmod( sockpath,S_IRWXU ) ;

		sockpath = StringAppendInt( path,syscall( SYS_gettid ) ) ;

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

		p = Process( "" ) ;
		str = ProcessArgumentStructure( p ) ;
		str->args    = args ;
		str->env     = env ;
		str->user_id = uid ;

		ProcessStart( p ) ;

		zuluCryptGetKeyFromSocket( sockpath,&key,uid ) ;

		_debug( p ) ;

		ProcessDelete( &p ) ;
	}

	StringMultipleDelete( &plugin_path,&path,END ) ;

	return key ;
}
