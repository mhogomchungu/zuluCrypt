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

#include "includes.h"

/*
 *  adding the header below because it contains prototype of crypt_get_dir()
 */
#include <libcryptsetup.h> 

/*
 * This function is responsible for creating a mapper name,the mapper name will show up at "/dev/mapper" if the volume
 * is successfully opened.
 * 
 * mapping_name is taken from a path to a volume presented by the user and then "zuluCrypt-XXX-YYYY is added to it to give
 * a mapper name unique to zuluCrypt.
 * 
 * XXX is the UID of the user that run the program,YYYY tells if the volume uses UUID or not."NAAN" pads the volume length
 * to make sure the length of "zuluCrypt-XXX-YYYY" is always the same regardless of what the user provided.
 * 
 * XXX is there for security reason.It makes sure one user can not manage another user's mappers 
 */

/*
 * the mapping_name name is taken from the last segment of volume path,this means that two volumes 
 * with the same name but from two different paths will end up with the same mapping_name making it impossible to open second volume.
 * 
 * this simple hash function will make the above possible by appending hush of full path hopefully to make sure two
 * paths will always be different while using the same mapping_name
 */
string_t hash_path( const char * path )
{
	size_t i = 0 ;
	size_t l = strlen( path ) ;
	uint64_t h = 0 ;
	
	for ( i = 0 ; i < l ; i++ ) 
		h = h + path[ i ] ;	
	
	return StringIntToString( h ) ;
}

string_t create_mapper_name( const char * device,const char * mapping_name,uid_t uid,int i )
{
	string_t z ;
	string_t q ;
	string_t p ;

	if( i == OPEN )
		p = String( "zuluCrypt-" ) ;
	else{
		p = String( crypt_get_dir() ) ;
		StringAppend( p,"/zuluCrypt-" ) ;
	}
	
	q = StringIntToString( uid ) ;	
	StringAppendString( p,q ) ;
	
	if( strncmp( mapping_name,"UUID-",5 ) == 0 )
		StringMultipleAppend( p,"-",mapping_name,"-",'\0' ) ;
	else
		StringMultipleAppend( p,"-NAAN-",mapping_name,"-",'\0' ) ;			
	
	if( strncmp( mapping_name,"UUID-",5 ) == 0 )
		z = hash_path( mapping_name ) ;
	else
		z = hash_path( device ) ;
	
	StringAppendString( p,z ) ;
	
	replace_bash_special_chars( p ) ;
	
	StringDelete( &q ) ;
	StringDelete( &z ) ;

	return p ;
}
