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
#include <fcntl.h>
#include <unistd.h>

#include <libcryptsetup.h>

typedef struct{

	size_t       data_alignment ;
	size_t       key_len ;
	size_t       key_size ;
	const char * key ;
	const char * type ;
	const char * hash ;
	const char * algo ;
	const char * cipher ;
	const char * rng ;
	void * params ;
	u_int64_t    iterations ;

}arguments ;

static int zuluExit( int st,stringList_t stl,char * const * options )
{
	StringFree( options ) ;

	StringListDelete( &stl ) ;

	return st ;
}

static int zuluExit_1( int st,struct crypt_device * cd )
{
	crypt_free( cd ) ;

	return st ;
}

static int _not_supported_argument_encountered( char * const * options )
{
	const char * e = *( options + 1 ) ;

	/*
	 * supported algorithms are: aes,serpent and twofish
	 */
	if( StringAtLeastOneMatch_1( e,"aes","serpent","twofish",NULL ) ){

		e = *( options + 2 ) ;
		/*
		 * supported ciphers are: xts-plain64 and cbc-essiv:sha256
		 */
		if( StringAtLeastOneMatch_1( e,"xts-plain64","cbc-essiv:sha256",NULL ) ){

			e = *( options + 3 ) ;

			/*
			 * supported keysizes in bits are: 256 and 512
			 */
			if( StringAtLeastOneMatch_1( e,"256","512",NULL ) ){

				e = *( options + 4 ) ;

				/*
				 * supported hashes are: sha1,sha256,sha512,ripemd160 and whirlpool
				 */

				if( StringsAreEqual( e,"whirlpool" ) ){
					/*
					 * zuluCryptWhirlpoolIsSupported() is defined in include.h
					 */
					if( zuluCryptWhirlpoolIsSupported() ){

						return 0 ;
					}else{
						return 1 ;
					}
				}
				if( StringAtLeastOneMatch_1( e,"sha1","sha256","sha512","ripemd160",NULL ) ){

					return 0 ;
				}
			}
		}
	}

	return 1 ;
}

/*
 * opts is structure expected to be in a format of:
 * rng.mode.cipher.keysize.hash
 */
static int _create_luks( const char * device,const resolve_path_t * opts )
{
	struct crypt_device * cd = NULL ;

	const arguments * args = opts->args ;

	if( crypt_init( &cd,device ) != 0 ){

		return 1 ;
	}

	if( StringsAreEqual( args->rng,"/dev/random" ) ){

		crypt_set_rng_type( cd,CRYPT_RNG_RANDOM ) ;
	}else{
		crypt_set_rng_type( cd,CRYPT_RNG_URANDOM ) ;
	}

	if( args->iterations != 0 ){

		crypt_set_iteration_time( cd,args->iterations ) ;
	}

	if( crypt_format( cd,args->type,args->algo,args->cipher,NULL,NULL,
			  args->key_size,args->params ) != 0 ){

		return zuluExit_1( 2,cd ) ;
	}

	if( crypt_keyslot_add_by_volume_key( cd,CRYPT_ANY_SLOT,NULL,args->key_size,
		args->key,args->key_len ) < 0 ){
		return zuluExit_1( 3,cd ) ;
	}else{
		return zuluExit_1( 0,cd ) ;
	}
}

static int _create_luks_0( void *( *function )( void *,const arguments * args ),
			   void * params,
			   const char * device,
			   const char * pass,
			   size_t pass_size,
			   const char * options,
			   const char * type )
{
	/*
	 * resolve_path_t is defined in includes.h
	 */
	resolve_path_t opts ;
	arguments args ;

	size_t list_count = 0 ;

	char * const * list = NULL ;

	stringList_t stl ;

	memset( &opts,'\0',sizeof( opts ) ) ;
	memset( &args,'\0',sizeof( args ) ) ;

	if( StringHasNothing( options ) ){

		stl = StringList( "/dev/urandom" ) ;
	}else{
		stl = StringListSplit( options,'.' ) ;
	}

	StringListStringArray_1( &list,&list_count,stl ) ;

	if( list_count == 1 ){

		args.hash     = "sha256" ;
		args.cipher   = "xts-plain64" ;
		args.algo     = "aes" ;
		args.key_size = 32 ;
		args.rng      = *( list + 0 ) ;

	}else if( list_count >= 5 ){

		if( _not_supported_argument_encountered( list ) ){

			return zuluExit( 1,stl,list ) ;
		}else{
			args.rng      = *( list + 0 ) ;
			args.algo     = *( list + 1 ) ;
			args.cipher   = *( list + 2 ) ;
			args.key_size = ( size_t ) StringConvertToInt( *( list + 3 ) ) / 8 ;
			args.hash     = *( list + 4 ) ;

			if( list_count > 5 ){

				args.iterations = StringConvertToInt( *( list + 5 ) )  ;
			}
		}
	}else{
		return zuluExit( 1,stl,list ) ;
	}

	args.key          = pass ;
	args.key_len      = pass_size ;
	args.type         = type ;
	args.data_alignment = 4096 ;
	args.params       = function( params,&args ) ;

	opts.device       = device ;
	opts.args         = &args ;
	opts.open_mode    = O_RDWR ;
	opts.error_value  = 2 ;

	/*
	 * zuluCryptResolveDevicePath() is defined in resolve_path.c
	 */
	return zuluExit( zuluCryptResolveDevicePath( _create_luks,&opts ),stl,list ) ;
}

static void * _set_luks_1_options( void * p,const arguments * args )
{
	struct crypt_params_luks1 * params = p ;

	params->hash           = args->hash ;
	params->data_alignment = args->data_alignment ;

	return params ;
}

int zuluCryptCreateLuks( const char * device,const char * pass,size_t pass_size,const char * options )
{
	struct crypt_params_luks1 params ;

	memset( &params,'\0',sizeof( struct crypt_params_luks1 ) ) ;

	return _create_luks_0( _set_luks_1_options,&params,device,pass,pass_size,options,CRYPT_LUKS1 ) ;
}

#ifdef CRYPT_LUKS2

static void * _set_luks_2_options( void * p,const arguments * args )
{
	struct crypt_params_luks2 * params = p ;

	struct crypt_pbkdf_type * pbkdf = ( struct crypt_pbkdf_type * ) &params->pbkdf ;

	params->data_alignment = args->data_alignment ;

	/*
	 * It seems to be necessary to set these and we are going with current cryptsetup default
	 */
	pbkdf->type             = "argon2" ;
	pbkdf->hash             = args->hash ;
	pbkdf->time_ms          = 800 ;
	pbkdf->memory_kb        = 1024 ;
	pbkdf->parallel_threads = 4 ;

	return params ;
}

int zuluCryptCreateLuks2( const char * device,const char * pass,size_t pass_size,const char * options )
{
	struct crypt_params_luks2 params ;

	memset( &params,'\0',sizeof( struct crypt_params_luks2 ) ) ;

	return _create_luks_0( _set_luks_2_options,&params,device,pass,pass_size,options,CRYPT_LUKS2 ) ;
}

#else

int zuluCryptCreateLuks2( const char * device,const char * pass,size_t pass_size,const char * options )
{
	if( 0 && device && pass && pass_size && options ){;}
	return 2 ;
}

#endif
