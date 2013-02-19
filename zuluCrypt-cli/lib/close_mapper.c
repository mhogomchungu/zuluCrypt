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

#include "includes.h"
#include <dirent.h>
#include <unistd.h>

/*
 * truecrypt volumes have create multiple mappers,when a mapper is unmapped,
 * go through /dev/mapper and remove if there are more mappers associated with the removed one
 */
static void _remove_remaining( const char * mapper,size_t len,const char * d )
{
	struct dirent * entry ;
	DIR * dir = opendir( d ) ;
	if( dir != NULL ){
		while( ( entry = readdir( dir ) ) != NULL ){
			if( StringPrefixMatch( mapper,entry->d_name,len ) ){
				crypt_deactivate( NULL,entry->d_name ) ;
			}
		}
		closedir( dir ) ;
	}
}

int zuluCryptCloseMapper( const char * mapper )
{
	int j ;
	const char * d = crypt_get_dir() ;
	size_t len = StringSize( crypt_get_dir() ) ;
	size_t len1 = StringSize( mapper ) ;
	for( j = 0 ; j < 3 ; j++ ) { 
		/*
		 * try multiple types to close the mapper just in case its hang up on something
		 */
		if( crypt_deactivate( NULL,mapper ) == 0 ){
			_remove_remaining( mapper + len + 1,len1 - ( len + 1 ),d ) ;
			return 0 ;
		}else{
			sleep( 1 ) ;
		}
	}
	
	return 1 ;
}
