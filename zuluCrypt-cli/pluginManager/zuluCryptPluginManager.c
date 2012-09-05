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


int zuluCryptGetKeyFromSocket( const char * path,string_t * key )
{
	socket_t client = SocketLocal( path ) ;
	
	int i ;
	size_t dataLength ;
	char * buffer ;
	
	for( i = 0 ;  ; i++ ){		
		if( i == 10 )
			return 5 ;
		if( SocketConnect( client ) == 0 )			
			break ;		
	}	
	
	dataLength = SocketGetData( client,&buffer,INTMAXKEYZISE ) ;	
	
	if( dataLength == 0 )
		return 5 ;
	
	*key = StringInheritWithSize( &buffer,dataLength ) ;
	
	return 0 ;	
}

void * zuluCryptPluginManagerOpenConnection( const char * sockpath )
{
	/*
	 * socketPair_t structure is more or less equivalent to vector<socket_t>
	 * It makes it easier to move around with two  socket_t objects
	 */
	socketPair_t sp = SocketPair() ;
	
	socket_t server = SocketLocal( sockpath ) ;
	socket_t client ;
	
	SocketPairSet( sp,server,0 ) ;
	
	SocketBind( server ) ;
	SocketListen( server ) ;
	
	client = SocketAccept( server ) ;
	
	SocketPairSet( sp,client,1 ) ;
	
	return ( void * ) sp ;	
}

int zuluCryptPluginManagerSendKey( void * p,const char * key,size_t length )
{
	socketPair_t sp = ( socketPair_t ) p ;
	socket_t client = SocketPairSecond( sp ) ;
	return SocketSendData( client,key,length ) ;
}

void zuluCryptPluginManagerCloseConnection( void * p )
{
	socketPair_t sp = ( socketPair_t ) p ;
	socket_t server = SocketPairFirst( sp ) ;
	socket_t client = SocketPairSecond( sp ) ;
	
	const char * sockpath = SocketAddress( server ) ;
	
	SocketClose( server ) ;
	SocketClose( client ) ;
	
	unlink( sockpath ) ;
	
	SocketPairDelete( &sp ) ;	
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

static string_t zuluCryptGetCmdArgumentList( char ** argv )
{
	string_t p = StringVoid ;
	size_t len ;	
	
	if( argv == NULL ){
		p = String( "-none" ) ;
	}else{
		p = String( "" ) ;
		
		while( *argv ){
			StringMultipleAppend( p,*argv," ",'\0' ) ;
			argv++ ;			
		}
		
		len = StringLength( p ) ;
		StringSubChar( p,len - 1,'\0' ) ;
	}
	
	return p ;
}

string_t zuluCryptPluginManagerGetKeyFromModule( const char * device,const char * name,uid_t uid,char ** argv )
{
	struct passwd * pass ;	
	socket_t s ;
	char * buffer ;	
	process_t p ;	
	
	int i ;	
	const char * sockpath ;	
	string_t key   = StringVoid ;	
	string_t cmd   = StringVoid ;
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
		
	id = StringIntToString( getpid() ) ;
	
	path = String( pass->pw_dir ) ;
	sockpath = StringAppend( path,"/.zuluCrypt-socket/" ) ;
	
	mkdir( sockpath,S_IRWXU | S_IRWXG | S_IRWXO ) ;
	chown( sockpath,uid,uid ) ;
	chmod( sockpath,S_IRWXU ) ;
	
	sockpath = StringAppendString( path,id ) ;

	if( stat( sockpath,&st ) == 0 )
		unlink( sockpath ) ;
	
	uuid = zuluCryptGetDeviceUUID( device ) ;

	p = Process( cpath ) ;		
	ProcessSetUser( p,uid ) ;
	/* 
	 * ProcessSetOptionTimeout( p,60,SIGKILL ) ;
	 */
	cmd = zuluCryptGetCmdArgumentList( argv ) ;
	ProcessSetArgumentList( p,device,StringContent( uuid ),sockpath,CHARMAXKEYZISE,StringContent( cmd ),'\0' ) ;	
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
	
	ProcessExitStatus( p ) ;
	
	StringMultipleDelete( &mpath,&uuid,&id,&path,&cmd,'\0' ) ;      
	
	SocketDelete( &s ) ;	
	ProcessDelete( &p ) ;
	
	return key ;
}
