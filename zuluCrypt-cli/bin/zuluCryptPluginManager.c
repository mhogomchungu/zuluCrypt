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
#include "includes.h"
#include <unistd.h>
#include <pwd.h>

#include "../process/process.h"
#include "../socket/socket.h"

void zuluCryptPluginManager( const char * sockpath,int size,const char * buffer )
{
	socket_t p ;
	socket_t q = Socket( "local" ) ;
	
	SocketSetHostAddress( q,sockpath ) ;
	
	SocketBind( q ) ;
	SocketListen( q ) ;
	
	p = SocketAccept( q ) ;
	
	SocketSendData( p,buffer,size ) ;
	
	SocketClose( p ) ;	
	SocketClose( q ) ;	
	
	SocketDelete( &p ) ;	
	SocketDelete( &q ) ;
} 

string_t zuluCryptPluginManagerGetKeyFromModule( const char * name,uid_t uid )
{
	struct passwd * pass ;	
	socket_t s ;
	char buffer[ 1025 ] ;	
	process_t p ;	
	string_t key = NULL ;	
	int i ;	
	const char * sockpath ;	
	string_t mpath ;	
	string_t  spath ;	
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
		mpath = String( "/etc/zuluCrypt/modules/" ) ;	
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
		
	spath = StringIntToString( getpid() ) ;
	sockpath = StringMultiplePrepend( spath,"/.zuluCrypt-",pass->pw_dir,'\0' ) ;
	
	if( stat( sockpath,&st ) == 0 )
		unlink( sockpath ) ;
	
	p = Process( cpath ) ;		
	ProcessSetUser( p,uid ) ;
	ProcessSetOptionTimeout( p,30,SIGKILL ) ;
	ProcessSetArgumentList( p,sockpath,"1024",'\0' ) ;	
	ProcessStart( p ) ;		
	
	s = Socket( "local" ) ;
	
	SocketSetHostAddress( s,sockpath ) ;
	
	for( i = 0 ; ; i++ ){
		if( SocketConnect( s ) == 0 ){
			i = SocketGetData( s,buffer,1024 ) ;
			if( i > 0 ){
				buffer[ i ] = '\0' ;
				key = String( buffer ) ;
			}			
			SocketClose( s ) ;			
			break ;
		}else if( i == 20 ){			
			ProcessKill( p ) ;
			break ;
		}else{
			sleep( 1 ) ;			
		}
	}
	
	StringMultipleDelete( &mpath,&spath,'\0' ) ;	
	
	ProcessExitStatus( p ) ;
	unlink( sockpath ) ;	
	SocketDelete( &s ) ;	
	ProcessDelete( &p ) ;
	
	return key ;
}
