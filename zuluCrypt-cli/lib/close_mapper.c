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

int close_mapper( const char * mapper )
{
	struct stat st ;
	
	int j ;
	int i ;
	/*
	 * For reasons currently unknown to me, the mapper fail to close sometimes so give it some room when it happens
	 */
	for( j = 0 ; j < 10 ; j++ ) { 
		if( crypt_deactivate( NULL,mapper ) == 0 ){
			/*
			 * For reasons currently unknown to me,the mapper path soft link does not always get deleted
			 */			
			for( i = 0 ; i < 10 ; i++ ){
				sleep( 1 ) ;
				if( stat( mapper,&st ) == 0 ){
					remove( mapper ) ;
				}else{
					break ;
				}
			}			

			return 0 ;
		}
		
		sleep( 1 );
	}
	
	return 1 ;
}
