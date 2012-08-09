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

#include "includes.h"

string_t GetKeyFromModule( const char * name )
{	
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
}
 
