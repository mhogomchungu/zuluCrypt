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

#include "libzuluCryptPluginManager.h"
#include "../process/process.h"
#include "../socket/socket.h"
#include "../string/String.h"
#include "../string/StringManage.h"
#include "../constants.h"
#include "../bin/includes.h"
/*
 * below header file is created at config time.
 */
#include "plugin_path.h"
#include <stdio.h>

size_t zuluCryptGetKeyFromSocket( const char * sockpath,string_t * key,uid_t uid )
{	
	size_t dataLength = 0 ;
	char * buffer ;
	
	socket_t client ;
	
	socket_t server = SocketLocal( sockpath ) ;	
	
	unlink( sockpath ) ;
	
	SocketBind( server ) ;
	
	chown( sockpath,uid,uid ) ;	
	chmod( sockpath,S_IRWXU | S_IRWXG | S_IRWXO ) ;
	
	SocketListen( server ) ;
	
	client = SocketAccept( server ) ;
	
	dataLength = SocketGetData( client,&buffer,INTMAXKEYZISE ) ;	
	
	SocketClose( server ) ;
	SocketClose( client ) ;
	
	SocketDelete( &server ) ;	
	SocketDelete( &client ) ;
	
	if( dataLength > 0 )
		*key = StringInheritWithSize( &buffer,dataLength ) ;
	
	unlink( sockpath ) ;
	
	return dataLength ;	
}

void * zuluCryptPluginManagerOpenConnection( const char * sockpath )
{
	int i ;	
	socket_t client = SocketLocal( sockpath ) ;
		
	for( i = 0 ;  ; i++ ){
		if( SocketConnect( client ) == 0 )
			return ( void * ) client ;
		else if( i == 20 ){
			SocketDelete( &client ) ;
			break ;
		}else
			sleep( 1 ) ;	
	}
	
	return NULL ;	
}

ssize_t zuluCryptPluginManagerSendKey( void * client,const char * key,size_t length )
{
	return client == NULL ? -1 : SocketSendData( ( socket_t )client,key,length ) ;
}

void zuluCryptPluginManagerCloseConnection( void * p )
{	
	if( p != NULL ){
		socket_t client = ( socket_t ) p;
		SocketClose( client ) ;
		SocketDelete( &client ) ;	
	}
}

static string_t zuluCryptGetDeviceUUID( const char * device )
{
	string_t p ;
	blkid_probe blkid ;
	const char * uuid ;
	
	blkid = blkid_new_probe_from_filename( device ) ;
	blkid_do_probe( blkid );
	
	if( blkid_probe_lookup_value( blkid,"UUID",&uuid,NULL ) == 0 ){
		p = String( uuid ) ;
	}else{
		p = String( "Nil" ) ;
	}
	
	blkid_free_probe( blkid );
	
	return p ;		
}

string_t zuluCryptPluginManagerGetKeyFromModule( const char * device,const char * name,uid_t uid,const char * argv )
{
	struct passwd * pass ;	
	
	socket_t server ;
	socket_t client ;
	
	char * buffer ;	
	process_t p ;	
	
	int i ;	
	const char * sockpath ;	
	string_t key   = StringVoid ;	
	string_t mpath = StringVoid ;	
	string_t path  = StringVoid ;
	string_t id    = StringVoid ;
	string_t uuid  = StringVoid ;
	const char * cpath ;	
	struct stat st ;
	pass = getpwuid( uid ) ;
		
	if( pass == NULL )
		return NULL ;	
	
	if( strrchr( name,'/' ) == NULL ){
		/*
		 * ZULUCRYPTpluginPath is set at config time at it equals $prefix/lib(64)/zuluCrypt/
		 */
		mpath = String( ZULUCRYPTpluginPath ) ;	
		StringAppend( mpath,name ) ;
	}else{
		/*
		 * module has a backslash, assume its path to where a module is located
		 */
		mpath = String( name ) ;
	}
	
	cpath = StringContent( mpath ) ;
	
	if( stat( cpath,&st ) != 0 ){
		StringDelete( &mpath ) ;
		return StringVoid ;
	}	
	
	path = String( pass->pw_dir ) ;
	sockpath = StringAppend( path,"/.zuluCrypt-socket/" ) ;
	
	mkdir( sockpath,S_IRWXU | S_IRWXG | S_IRWXO ) ;
	chown( sockpath,uid,uid ) ;
	chmod( sockpath,S_IRWXU ) ;		
	
	id = StringIntToString( getpid() ) ;
	
	sockpath = StringAppendString( path,id ) ;
	
	uuid = zuluCryptGetDeviceUUID( device ) ;

	p = Process( cpath ) ;		
	ProcessSetUser( p,uid ) ;
	/* 
	 * ProcessSetOptionTimeout( p,60,SIGKILL ) ;
	 */

	ProcessSetArgumentList( p,device,StringContent( uuid ),sockpath,CHARMAXKEYZISE,argv,'\0' ) ;	
	ProcessStart( p ) ;		
	
	server = SocketLocal( sockpath ) ;
	
	if( stat( sockpath,&st ) == 0 )
		unlink( sockpath ) ;
	
	SocketBind( server ) ;
	
	chown( sockpath,uid,uid ) ;	
	chmod( sockpath,S_IRWXU | S_IRWXG | S_IRWXO ) ;
	
	SocketListen( server ) ;
	
	client = SocketAccept( server ) ;
	SocketClose( server ) ;
	
	i = SocketGetData( client,&buffer,INTMAXKEYZISE ) ;
	SocketClose( client ) ;
	
	if( i > 0 )
		key = StringInheritWithSize( &buffer,i ) ;
	
	SocketDelete( &server ) ;
	SocketDelete( &client ) ;

	unlink( sockpath ) ;
	
	ProcessExitStatus( p ) ;
	ProcessDelete( &p ) ;
	
	StringMultipleDelete( &mpath,&uuid,&id,&path,'\0' ) ;      
	
	return key ;
}
