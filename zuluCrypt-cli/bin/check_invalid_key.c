/*
 *
 *  Copyright (c) 2012-2015
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

#include "includes.h"
#include <stdio.h>

void zuluCryptCheckInvalidKey( const char * device )
{
	char * d   ;
	const char * c ;
	int e = 0  ;

	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptEmptySlots() is defined in ../lib/empty_slots.c
	 */
	d = zuluCryptEmptySlots( device ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;

	if( d == NULL ){
		/*
		 * we got here because the volume is either not luks based or the path is invalid
		 */
		;
	}else{
		c = d - 1 ;
		while( *++c ){
			if( *c == '2' ){
				fprintf( stderr,"WARNING: key slot number: %d is corrupted\n",e ) ;
			}
			e++ ;
		}
		free( d ) ;
	}
}
