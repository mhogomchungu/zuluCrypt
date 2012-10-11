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

#include <unistd.h>

/*
 * For reasons currently unknown to me,the mapper path soft link does not always get deleted
 * do it manually when that happens
 */
static inline int unlink_mapper( const char * mapper )
{
	unlink( mapper ) ;
	return 0 ;
}

int zuluCryptCloseMapper( const char * mapper )
{
	int j ;
	/*
	 * For reasons currently unknown to me, the mapper fail to close sometimes so give it some room when it happens
	 */
	for( j = 0 ; j < 10 ; j++ ) { 
		if( crypt_deactivate( NULL,mapper ) == 0 ){			
			return unlink_mapper( mapper ) ;
		}else{
			sleep( 1 ) ;
		}	
	}
	
	return 1 ;
}
