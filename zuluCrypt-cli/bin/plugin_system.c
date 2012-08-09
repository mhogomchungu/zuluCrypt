/*
 * 
 *  Copyright (c) 2011
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

string_t GetKeyFromModule( const char * name,uid_t uid )
{
	struct passwd * pass ;	
	socket_t s ;
	char buffer[ 1024 ] ;	
	process_t p ;	
	string_t key = NULL ;	
	int i ;	
	const char * sockpath ;	
	string_t mpath ;	
	string_t  spath ;	
	const char * cpath ;	
		
	pass = getpwuid( uid ) ;
	
	if( pass == NULL )
		return NULL ;
	
	mpath = String( "/etc/zuluCrypt/modules/" ) ;
	
	cpath = StringAppend( mpath,name ) ;
	
	if( is_path_valid( cpath ) != 0 ){
		StringDelete( &mpath ) ;
		return NULL ;
	}	
		
	spath = StringIntToString( getpid() ) ;
	sockpath = StringMultiplePrepend( spath,"/.zuluCrypt-",pass->pw_dir,'\0' ) ;
	
	unlink( sockpath ) ;
	
	p = Process( cpath ) ;		
	ProcessSetUser( p,uid ) ;	
	ProcessSetArgumentList( p,sockpath,"1024",'\0' ) ;	
	ProcessStart( p ) ;		
	
	s = Socket( "local" ) ;
	
	SocketSetHostAddress( s,sockpath ) ;
	
	for( i = 0 ; ; i++ ){
		if( SocketConnect( s ) == 0 ){			
			SocketGetData( s,buffer,1024 ) ;
			key = String( buffer ) ;
			break ;			
		}else if( i == 20 ){			
			ProcessKill( p ) ;
			break ;
		}else{
			sleep( 1 ) ;			
		}
	}
	
	unlink( sockpath ) ;
		
	StringMultipleDelete( &mpath,&spath,'\0' ) ;
	
	ProcessDelete( &p ) ;

	SocketDelete( &s ) ;
	
	return key ;
}

string_t GetKeyFromModule_1( const char * name )
{	
	/*
	char * key = NULL ;	
	char * ( *f )( void ) ;	
	void * handle ;
	const char * cpath ;
	
	string_t pass = NULL ;
	string_t path = String( "/etc/zuluCrypt/modules/" ) ;
		
	cpath = StringMultipleAppend( path,name,".so",'\0' ) ;
	
	handle = ( void * ) dlopen( cpath,RTLD_LAZY ) ;
	
	if( handle != NULL ){
		f = ( char * ( * )() ) dlsym( handle,"zuluCryptGetKeyFromModule" ) ;	
		if( f != NULL ){
			key = ( *f )() ;		
			if( key != NULL ){
				pass = StringInherit( &key ) ;
			}
		}		
		dlclose( handle ) ;			
	}	
	
	StringDelete( &path ) ;
	
	return pass ;
	*/
	return NULL ;
}
 
