
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
static void __debug( process_t p )
{
	char * e = NULL ;
	puts( "--------stdout------------" ) ;
	ProcessGetOutPut( p,&e,STDOUT ) ;
	if( e ){
		puts( e ) ;
		fflush( stdout ) ;
		free( e ) ;
	}
	puts( "--------stderror----------" ) ;
	e = NULL ;
	ProcessGetOutPut( p,&e,STDERROR ) ;
	if( e ){
		puts( e ) ;
		fflush( stdout ) ;
		free( e ) ;
	}
}
#endif

size_t zuluCryptGetKeyFromSocket( const char * sockpath,string_t * key,uid_t uid )
{	
	size_t dataLength = 0 ;
	char * buffer ;
	
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
					*key = StringInheritWithSize( &buffer,dataLength ) ;
					SocketClose( &client ) ;
				}
			}
		}
		SocketClose( &server ) ;
	}
	return dataLength ;
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
	return client == NULL ? -1 : SocketSendData( ( socket_t )client,key,length ) ;
}

void zuluCryptPluginManagerCloseConnection( void * p )
{	
	socket_t client = ( socket_t ) p;
	if( p != NULL ){
		SocketClose( &client ) ;
	}
}

static inline string_t zuluCryptGetDeviceUUID( const char * device,uid_t uid )
{
	string_t p ;
	blkid_probe blkid ;
	const char * uuid ;
	
	seteuid( 0 ) ;
	
	blkid = blkid_new_probe_from_filename( device ) ;
	
	if( blkid == NULL ){
		p = String( "Nil" ) ;
	}else{
		blkid_do_probe( blkid );
	
		if( blkid_probe_lookup_value( blkid,"UUID",&uuid,NULL ) == 0 ){
			p = String( uuid ) ;
		}else{
			p = String( "Nil" ) ;
		}
	
		blkid_free_probe( blkid );
	}
	
	seteuid( uid ) ;
	return p ;
}

static inline int pluginIsGpG( const char * plugin_path )
{
	char * path = realpath( plugin_path,NULL ) ;
	int st = strcmp( path,ZULUCRYPTpluginPath"gpg" ) ;
	free( path ) ;
	return st == 0 ;
}

string_t zuluCryptPluginManagerGetKeyFromModule( const char * device,const char * name,uid_t uid,const struct_opts * opts )
{	
	process_t p ;
	struct stat st ;
	
	const char * sockpath ;
	const char * pluginPath ;
	const char * argv = opts->argv ;
	char * const * env = opts->env ;
	const char * args[ 7 ] ;
	
	ProcessStructure * str ;
	
	string_t key   = StringVoid ;
	string_t plugin_path = StringVoid ;
	string_t path  = StringVoid ;
	string_t uuid  = StringVoid ;

	struct passwd * pass = getpwuid( uid ) ;
		
	if( pass == NULL ){
		return key ;
	}
	/*
	 * ZULUCRYPTpluginPath is set at config time at it equals $prefix/lib(64)/zuluCrypt/
	 */
	plugin_path = String( ZULUCRYPTpluginPath ) ;
	
	pluginPath = strrchr( name,'/' ) ;
	
	if( pluginPath == NULL ){
		pluginPath = StringAppend( plugin_path,name ) ;
	}else{
		pluginPath = StringAppend( plugin_path,pluginPath + 1 ) ;
	}

	if( stat( pluginPath,&st ) == 0 && S_ISREG( st.st_mode ) ) {
		path = String( pass->pw_dir ) ;
		sockpath = StringAppend( path,"/.zuluCrypt-socket/" ) ;
	
		mkdir( sockpath,S_IRWXU | S_IRWXG | S_IRWXO ) ;
		chown( sockpath,uid,uid ) ;
		chmod( sockpath,S_IRWXU ) ;
	
		sockpath = StringAppendInt( path,syscall( SYS_gettid ) ) ;
		
		uuid = zuluCryptGetDeviceUUID( device,uid ) ;
		
		p = Process( pluginPath ) ;
		str = ProcessArgumentStructure( p ) ;
		args[0] = pluginPath ;
		args[1] = device ;
		args[2] = StringContent( uuid ) ;
		args[3] = sockpath ;
		/*
		 * ZULUCRYPT_CHAR_MAX_KEYSIZE is set in ../constants.h
		 */
		args[4] = ZULUCRYPT_CHAR_MAX_KEYSIZE ;
		args[5] = argv ;
		args[6] = NULL ;
		
		str->args    = ( char * const * )args ;
		str->env     = env ;
		str->user_id = uid ;
		
		ProcessStart( p ) ;
	
		zuluCryptGetKeyFromSocket( sockpath,&key,uid ) ;
		/*
		 * for reasons currently unknown to me,the gpg plugin doesnt always exit,it hangs consuming massive amount of cpu circles.
		 * we terminate it here by sending it a sigterm after it is done sending its key to make sure it exits.
		 */
		if( pluginIsGpG( pluginPath ) ){
			ProcessTerminate( p ) ;
		}
		
		#if zuluCryptPluginManagerDebug
			__debug( p ) ;
		#endif
			
		ProcessDelete( &p ) ;
	}
	
	StringMultipleDelete( &plugin_path,&uuid,&path,END ) ;      
	
	return key ;
}
