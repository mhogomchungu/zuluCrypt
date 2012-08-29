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
#include "../constants.h"

/*
 * below header file is created at config time.
 */
#include "plugin_path.h"

/*
 * The below number is the cryptsetup default amount of maximum bytes to read from a keyfile.
 */
#define INTMAXKEYZISE  8192000
#define CHARMAXKEYZISE "8192000"

typedef struct {
	socket_t server ;
	socket_t connected ;
} socketStruct ;

void * zuluCryptPluginManagerStartConnection( const char * sockpath )
{
	socketStruct * sc = ( socketStruct * ) malloc( sizeof( socketStruct ) ) ;
	
	sc->server = SocketLocal( sockpath ) ;
	
	SocketBind( sc->server ) ;
	SocketListen( sc->server ) ;
	
	sc->connected = SocketAccept( sc->server ) ;
	
	return sc ;	
}

int zuluCryptPluginManagerSendKey( void * p,const char * key,int length )
{
	socketStruct * sc = ( socketStruct * ) p ;
	
	return SocketSendData( sc->connected,key,length ) ;
}

void zuluCryptPluginManagerCloseConnection( void * p )
{
	socketStruct * sc = ( socketStruct * ) p ;
	const char * sockpath = SocketAddress( sc->server ) ;
	
	SocketClose( sc->connected ) ;
	SocketClose( sc->server ) ;
	
	unlink( sockpath ) ;
	
	SocketDelete( &sc->connected ) ;
	SocketDelete( &sc->server ) ;
	
	free( sc ) ;	
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

string_t zuluCryptPluginManagerGetKeyFromModule( const char * device,const char * name,uid_t uid )
{
	struct passwd * pass ;	
	socket_t s ;
	char * buffer ;	
	process_t p ;	
	string_t key = NULL ;	
	int i ;	
	const char * sockpath ;	
	string_t mpath ;	
	string_t path ;
	string_t id ;
	string_t uuid ;
	const char * cpath ;	
	struct stat st ;
	pass = getpwuid( uid ) ;
	
	if( pass == NULL )
		return NULL ;	
	
	if( strrchr( name,'/' ) == NULL ){
		/*
		 * module name does not contain a backslash, assume its a module name and go look for
		 * it in /etc/zuluCrypt/modules
		 */
		
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
		return NULL ;
	}	
		
	id = StringIntToString( getpid() ) ;
	
	path = String( pass->pw_dir ) ;
	sockpath = StringAppend( path,"/.zuluCrypt-socket/" ) ;
	
	mkdir( sockpath,S_IRWXU | S_IRWXG | S_IRWXO ) ;
	chmod( sockpath,S_IRWXU | S_IRWXG | S_IRWXO ) ;
	
	sockpath = StringAppendString( path,id ) ;

	if( stat( sockpath,&st ) == 0 )
		unlink( sockpath ) ;
	
	uuid = zuluCryptGetDeviceUUID( device ) ;

	p = Process( cpath ) ;		
	ProcessSetUser( p,uid ) ;
	/* 
	 * ProcessSetOptionTimeout( p,60,SIGKILL ) ;
	 */
	ProcessSetArgumentList( p,device,StringContent( uuid ),sockpath,CHARMAXKEYZISE,'\0' ) ;	
	ProcessStart( p ) ;		
	
	s = SocketLocal( sockpath ) ;
	
	for( i = 0 ; ; i++ ){
		if( SocketConnect( s ) == 0 ){
			i = SocketGetData( s,&buffer,INTMAXKEYZISE ) ;
			if( i > 0 )
				key = StringInheritWithSize( &buffer,i ) ;
			SocketClose( s ) ;			
			break ;
		}else if( i == 30 ){			
			ProcessKill( p ) ;
			break ;
		}else{
			sleep( 1 ) ;			
		}
	}
	
	StringMultipleDelete( &mpath,&uuid,&id,&path,'\0' ) ;	
	
	ProcessExitStatus( p ) ;
	
	SocketDelete( &s ) ;	
	ProcessDelete( &p ) ;
	
	return key ;
}

/*
 * Currently unsed function.
 */
void zuluCryptPluginManager( const char * sockpath,int size,const char * buffer )
{
	socket_t p ;
	socket_t q = SocketLocal( sockpath ) ;
	
	SocketBind( q ) ;
	SocketListen( q ) ;
	
	p = SocketAccept( q ) ;
	
	SocketSendData( p,buffer,size ) ;
	
	SocketClose( p ) ;	
	SocketClose( q ) ;	
	
	unlink( sockpath ) ;
	
	SocketDelete( &p ) ;	
	SocketDelete( &q ) ;
} 
