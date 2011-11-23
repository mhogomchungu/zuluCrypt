/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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
	int i = crypt_deactivate( NULL, mapper );
	
	if ( i != 0 ){
		/*
		 * I have on occassion seen a mapper not getting closed when running zuluCrypt-test
		 * Assuming its due to slow computers like mine and hence adding two seconds delay
		 * before attempting to redeactive when it fail the first time. 
		 */
		sleep( 2 ) ;
		
		i =crypt_deactivate( NULL, mapper );
	}
	
	if( i == 0 )
		return 0;
	else
		return 2 ;	
}


