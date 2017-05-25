/*
 *
 *  Copyright (c) 2011-2015
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
#include <libcryptsetup.h>
#include <fcntl.h>
#include <unistd.h>

static char * zuluExit( char * c,struct crypt_device * cd )
{
	if( cd != NULL ){
		crypt_free( cd ) ;
	}
	return c ;
}

static char * _empty_slots( const char * device,const resolve_path_t * opts )
{
	struct crypt_device * cd;

	int j ;
	int k ;
	const char * type ;

	string_t p ;

	if( opts ){;}

	if( crypt_init( &cd,device ) != 0 ){

		return zuluExit( NULL,NULL ) ;
	}

	zuluCryptDisableMetadataLocking( cd ) ;

	if( crypt_load( cd,NULL,NULL ) != 0 ){

		return zuluExit( NULL,cd ) ;
	}

	type = crypt_get_type( cd ) ;

	if( type == NULL ){

		return zuluExit( NULL,cd ) ;
	}

	k = crypt_keyslot_max( type ) ;

	if( k < 0 ){

		return zuluExit( NULL,cd ) ;
	}

	p = StringEmpty() ;

	for( j = 0 ; j < k ; j++ ){

		switch( crypt_keyslot_status( cd,j ) ){

			case CRYPT_SLOT_INACTIVE   : StringAppend( p,"0" ) ; break ;
			case CRYPT_SLOT_ACTIVE     : StringAppend( p,"1" ) ; break ;
			case CRYPT_SLOT_INVALID    : StringAppend( p,"2" ) ; break ;
			case CRYPT_SLOT_ACTIVE_LAST: StringAppend( p,"3" ) ; break ;
			default : ;
		}
	}

	return zuluExit( StringDeleteHandle( &p ),cd ) ;
}

char * zuluCryptEmptySlots( const char * device )
{
	/*
	 * resolve_path_t is defined in includes.h
	 */
	resolve_path_t opts ;

	memset( &opts,'\0',sizeof( opts ) ) ;

	opts.device        = device ;
	opts.open_mode     = O_RDONLY ;
	opts.error_value_1 = NULL ;

	/*
	 * zuluCryptResolveDevicePath_1() is defined in resolve_path.c
	 */
	return zuluCryptResolveDevicePath_1( _empty_slots,&opts ) ;
}
