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
#include "luks2_support.h"

typedef struct arguments{

	const char * key_0 ;
	size_t       key_0_len ;
	const char * key_1 ;
	size_t       key_1_len ;
	const char * options ;
	int ( *set_options )( int *,struct crypt_device *,const struct arguments * ) ;
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

	int key_slot ;

	if( zuluCryptVolumeIsNotLuks( device ) ){

		return 3 ;
	}
	if( crypt_init( &cd,device ) != 0 ){

		return 2 ;
	}
	if( crypt_load( cd,NULL,NULL ) != 0 ){

		return zuluExit( 2,cd ) ;
	}

	args->set_options( &key_slot,cd,args ) ;

	if( crypt_keyslot_add_by_passphrase( cd,
					     key_slot,
					     args->key_0,
					     args->key_0_len,
					     args->key_1,
					     args->key_1_len ) < 0 ){
		return zuluExit( 1,cd ) ;
	}else{
		return zuluExit( 0,cd ) ;
	}
}

int zuluCryptAddKey( const char * device,const char * existingkey,
		     size_t existingkey_size,const char * newkey,size_t newkey_size )
{
	return zuluCryptAddKey_0( device,
				  existingkey,
				  existingkey_size,
				  newkey,
				  newkey_size,
				  "-1.-1.argon2id.-1.-1.-1" ) ;
}

#ifdef CRYPT_LUKS2

static int _set_options( int * key_slot,struct crypt_device * cd,const arguments * args )
{
	stringList_t stl = StringListSplit( args->options,'.' ) ;

	size_t list_count = 0 ;

	char * const * list = NULL ;

	int r ;

	StringListStringArray_1( &list,&list_count,stl ) ;

	const char * pbkdf_type = NULL ;
	const char * max_memory = NULL ;
	const char * max_threads = NULL ;
	const char * time_cost = NULL ;
	const char * iterations = NULL ;
	const char * keySlot = NULL ;

	if( list_count >= 6 ){

		time_cost   = *( list + 0 ) ;
		iterations  = *( list + 1 ) ;
		pbkdf_type  = *( list + 2 ) ;
		max_memory  = *( list + 3 ) ;
		max_threads = *( list + 4 ) ;
		keySlot     = *( list + 5 ) ;
	}

	struct crypt_pbkdf_type pbkdf ;

	memset( &pbkdf,'\0',sizeof( pbkdf ) ) ;

#if SUPPORT_crypt_get_pbkdf_default
	/*
	 * added in cryptsetup 2.0.3
	 */
	memcpy( &pbkdf,crypt_get_pbkdf_default( CRYPT_LUKS2 ),sizeof( struct crypt_pbkdf_type ) ) ;
#else
	pbkdf.type             = CRYPT_KDF_ARGON2I ;
	pbkdf.max_memory_kb    = 1024 ;
	pbkdf.parallel_threads = 4 ;
	pbkdf.hash = "sha256" ;
#endif
	if( StringsAreNotEqual( pbkdf_type,"pbkdf2" ) ){

		if( max_memory && StringsAreNotEqual( max_memory,"-1" ) ){

			pbkdf.max_memory_kb = ( unsigned int ) StringConvertToInt( max_memory ) ;
		}

		if( max_threads && StringsAreNotEqual( max_threads,"-1" ) ){

			pbkdf.parallel_threads = ( unsigned int ) StringConvertToInt( max_threads ) ;
		}
	}else{
		pbkdf.max_memory_kb = 0 ;
		pbkdf.parallel_threads = 0 ;
	}

	if( time_cost && StringsAreNotEqual( time_cost,"-1" ) ){

		pbkdf.time_ms = (unsigned int) StringConvertToInt( time_cost ) ;
	}

	if( iterations && StringsAreNotEqual( iterations,"-1" ) ){

		pbkdf.time_ms = 0 ;
		pbkdf.iterations = (unsigned int) StringConvertToInt( iterations ) ;
		pbkdf.flags |= CRYPT_PBKDF_NO_BENCHMARK ;
	}

	if( pbkdf_type ){

		pbkdf.type = pbkdf_type ;
	}

	if( keySlot && StringsAreNotEqual( keySlot,"-1" ) ){

		*key_slot = (int)StringConvertToInt( keySlot ) ;
	}else{
		*key_slot = CRYPT_ANY_SLOT ;
	}

	r = crypt_set_pbkdf_type( cd,&pbkdf ) ;

	StringListDelete( &stl ) ;
	StringFree( list ) ;

	return r ;
}

#else

static int _set_options( int * key_slot,struct crypt_device * cd, const arguments * args )
{
	if( cd && args ){}

	stringList_t stl = StringListSplit( args->options,'.' ) ;

	size_t list_count = 0 ;

	char * const * list = NULL ;

	StringListStringArray_1( &list,&list_count,stl ) ;

	const char * keySlot ;

	if( list_count >= 6 ){

		keySlot = *( list + 5 ) ;

		if( keySlot && StringsAreNotEqual( keySlot,"-1" ) ){

			*key_slot = (int)StringConvertToInt( keySlot ) ;
		}else{
			*key_slot = CRYPT_ANY_SLOT ;
		}
	}else{
		*key_slot = CRYPT_ANY_SLOT ;
	}

	StringListDelete( &stl ) ;
	StringFree( list ) ;

	return 0 ;
}

#endif

int zuluCryptAddKey_0( const char * device,
		       const char * existingkey,
		       size_t existingkey_size,
		       const char * newkey,
		       size_t newkey_size,
		       const char * options )
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
	args.options      = options ;
	args.set_options  = _set_options ;

	opts.device       = device ;
	opts.args         = &args ;
	opts.open_mode    = O_RDWR ;
	opts.error_value  = 1 ;

	/*
	 * zuluCryptResolveDevicePath() is defined in resolve_path.c
	 */
	return zuluCryptResolveDevicePath( _add_key,&opts ) ;
}
