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

typedef struct{

	const char * key ;
	size_t       key_len ;
	int          slot_number ;

}arguments ;

static int zuluExit( int st,struct crypt_device * cd )
{
	crypt_free( cd ) ;
	return st ;
}

static int _remove_key( const char * device,const resolve_path_t * opts )
{
	int slot ;

	struct crypt_device * cd ;

	const arguments * args = opts->args ;

	if( zuluCryptVolumeIsNotLuks( device ) ){

		return 1 ;
	}
	if( crypt_init( &cd,device ) != 0 ){

		return 3 ;
	}
	if( crypt_load( cd,NULL,NULL ) != 0 ){

		return zuluExit( 3,cd ) ;
	}

	slot = crypt_activate_by_passphrase( cd,NULL,CRYPT_ANY_SLOT,args->key,args->key_len,0 ) ;

	if( slot < 0 ){

		return zuluExit( 2,cd ) ;
	}
	if( args->slot_number == CRYPT_ANY_SLOT ){
		/*
		 * Behaves like luksRemoveKey
		 */
		if( crypt_keyslot_destroy( cd,slot ) < 0 ){

			return zuluExit( 2,cd ) ;
		}else{
			return zuluExit( 0,cd ) ;
		}
	}else{
		/*
		 * Behaves like luksKillSlot
		 */
		if( crypt_keyslot_destroy( cd,args->slot_number ) < 0 ){

			return zuluExit( 2,cd ) ;
		}else{
			return zuluExit( 0,cd ) ;
		}
	}
}

int zuluCryptRemoveKey( const char * device ,const char * pass,size_t pass_size )
{
	return zuluCryptRemoveKey_0( device,pass,pass_size,CRYPT_ANY_SLOT ) ;
}

int zuluCryptRemoveKey_0( const char * device ,const char * pass,size_t pass_size,int key_slot )
{
	/*
	 * resolve_path_t is defined in includes.h
	 */
	resolve_path_t opts ;
	arguments args ;

	memset( &opts,'\0',sizeof( opts ) ) ;
	memset( &args,'\0',sizeof( args ) ) ;

	args.key     = pass ;
	args.key_len = pass_size ;

	if( key_slot == -1 ){

		args.slot_number = CRYPT_ANY_SLOT ;
	}else{
		args.slot_number  = key_slot ;
	}

	opts.device       = device ;
	opts.args         = &args  ;
	opts.open_mode    = O_RDWR ;
	opts.error_value  = 2 ;

	/*
	 * zuluCryptResolveDevicePath() is defined in resolve_path.c
	 */
	return zuluCryptResolveDevicePath( _remove_key,&opts ) ;
}
