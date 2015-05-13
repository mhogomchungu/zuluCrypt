/*
 *
 *  Copyright (c) 2011
 *  name : Francis Banyikwa
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

#include <libcryptsetup.h>
#include <unistd.h>

int zuluCryptCloseMapper( const char * mapper )
{
	int j ;

	int r = 1 ;

	struct crypt_device * cd ;

	if( crypt_init_by_name( &cd,mapper ) == 0 ){

		for( j = 0 ; j < 3 ; j++ ) {

			/*
			 * try multiple times to close the mapper just in case
			 */

			r = crypt_deactivate( cd,mapper ) ;

			if( r == 0 ){

				break ;
			}else{
				sleep( 1 ) ;
			}
		}

		crypt_free( cd ) ;
	}else{
		/*
		 * we shouldnt get here
		 */
		;
	}

	return r ;
}
