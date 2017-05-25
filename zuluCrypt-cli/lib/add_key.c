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

	const char * key_0 ;
	size_t       key_0_len ;
	const char * key_1 ;
	size_t       key_1_len ;

}arguments ;

static int zuluExit( int st,struct crypt_device * cd )
{
	crypt_free( cd ) ;
	return st ;
}

static int _add_key( const char * device,const resolve_path_t * opts )
{
	struct crypt_device * cd ;

	const arguments * args = opts->args ;

	if( zuluCryptVolumeIsNotLuks( device ) ){

		return 3 ;
	}
	if( crypt_init( &cd,device ) != 0 ){

		return 2 ;
	}

	zuluCryptDisableMetadataLocking( cd ) ;

	if( crypt_load( cd,NULL,NULL ) != 0 ){

		return zuluExit( 2,cd ) ;
	}
	if( crypt_keyslot_add_by_passphrase( cd,CRYPT_ANY_SLOT,args->key_0,

		args->key_0_len,args->key_1,args->key_1_len ) < 0 ){
		return zuluExit( 1,cd ) ;
	}else{
		return zuluExit( 0,cd ) ;
	}
}

int zuluCryptAddKey( const char * device,const char * existingkey,
		     size_t existingkey_size,const char * newkey,size_t newkey_size )
{
	/*
	 * resolve_path_t is defined in includes.h
	 */
	resolve_path_t opts ;
	arguments args ;

	memset( &opts,'\0',sizeof( opts ) ) ;
	memset( &args,'\0',sizeof( args ) ) ;

	args.key_0        = existingkey ;
	args.key_0_len    = existingkey_size ;
	args.key_1        = newkey ;
	args.key_1_len    = newkey_size ;

	opts.device       = device ;
	opts.args         = &args ;
	opts.open_mode    = O_RDWR ;
	opts.error_value  = 1 ;

	/*
	 * zuluCryptResolveDevicePath() is defined in resolve_path.c
	 */
	return zuluCryptResolveDevicePath( _add_key,&opts ) ;
}
