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
static uint64_t hash_path( const char * path )
{
	size_t i = 0 ;
	size_t l = strlen( path ) ;
	uint64_t h = 0 ;
	
	for ( i = 0 ; i < l ; i++ ) 
		h = h + path[ i ] ;
	return h ;
}

string_t zuluCryptCreateMapperName( const char * device,const char * mapping_name,uid_t uid,int i )
{	
	string_t p ;
	uint64_t z ;
	char * e ;
	/*
	 * ZULUCRYPTshortMapperPath is set in ../constants.h
	 */
	if( i == ZULUCRYPTshortMapperPath ){
		p = String( "zuluCrypt-" ) ;
	}else{
		p = String( crypt_get_dir() ) ;
		StringAppend( p,"/zuluCrypt-" ) ;
	}
	
	StringAppendInt( p,uid ) ;
	
	if( StringPrefixMatch( mapping_name,"UUID-",5 ) ){
		StringMultipleAppend( p,"-",mapping_name,"-",END ) ;
		z = hash_path( mapping_name ) ;
	}else{
		StringMultipleAppend( p,"-NAAN-",mapping_name,"-",END ) ;
		if( StringPrefixMatch( device,"/dev/loop",9 ) ){
			/*
			* zuluCryptLoopDeviceAddress() is defined in ./create_loop_device.c
			*/
			e = zuluCryptLoopDeviceAddress( device ) ;
			if( e != NULL ){
				z = hash_path( e ) ;
				free( e ) ;
			}else{
				z = hash_path( device ) ;
			}
		}else{
			z = hash_path( device ) ;
		}
	}
	
	StringAppendInt( p,z ) ;
	
	/*
	 * cryptsetup 1.4.1 and previous have a bug and its triggered when the mapper has one or more bash
	 * special characters, this functions substitute bash special characters for an underscore to
	 * work around the bug. 
	 */
	StringReplaceCharString( p,'_',BASH_SPECIAL_CHARS ) ;
	return p ;
}
