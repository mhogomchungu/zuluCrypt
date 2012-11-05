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

static inline char * zuluExit( char * c,struct crypt_device * cd )
{
	if( cd != NULL )
		crypt_free( cd );
	
	return c ;
}

char * zuluCryptEmptySlots( const char * device )
{
	crypt_keyslot_info cki ;
	struct crypt_device * cd;
	int j ;
	int k ;
	char * q ;
	const char * type ;
	
	string_t p = StringVoid ;

	if( crypt_init( &cd,device ) != 0 )
		return zuluExit( NULL,NULL ) ;
	
	if( crypt_load( cd,NULL,NULL ) != 0 )
		return zuluExit( NULL,cd ) ;
		
	type = crypt_get_type( cd ) ;
	
	if( type == NULL )
		return zuluExit( NULL,cd ) ;
	
	k = crypt_keyslot_max( type ) ;
	
	if( k < 0 )
		return zuluExit( NULL,cd ) ;

	p = String( "" ) ;
	
	for( j = 0 ; j < k ; j++ ){
		cki = crypt_keyslot_status( cd,j );
		switch ( cki ){
			case CRYPT_SLOT_INACTIVE   : StringAppend( p,"0" ) ; break ;
			case CRYPT_SLOT_ACTIVE     : StringAppend( p,"1" ) ; break ;
			case CRYPT_SLOT_INVALID    : StringAppend( p,"2" ) ; break ;
			case CRYPT_SLOT_ACTIVE_LAST: StringAppend( p,"3" ) ; break ;
		}		
	}
	
	q = StringDeleteHandle( &p ) ;
	
	return zuluExit( q,cd ) ;
}

