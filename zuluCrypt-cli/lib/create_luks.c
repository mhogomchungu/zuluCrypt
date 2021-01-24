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
#include <stdio.h>
#include <sys/syscall.h>

#include <libcryptsetup.h>

#include "luks2_support.h"

typedef struct arguments{

	size_t key_len ;
	size_t key_size ;

	const char * key ;
	const char * type ;
	const char * hash ;
	const char * algo ;
	const char * cipher ;
	const char * rng ;
	const char * integrity ;

	void * params ;
	void * pbkdf ;

	u_int64_t iterations ;

	void *( *function )( const struct arguments * ) ;
	int ( *format )( struct crypt_device * ) ;

}arguments ;

static void _debug( int a,const char * b,void * c )
{
	if( c ){}

	if( a == CRYPT_LOG_ERROR ){

		if( b ){

			printf( "log level: %d,log msg: %s",a,b ) ;
		}else{
			printf( "log level: %d,log msg: NULL",a ) ;
		}
	}
}

void zuluCryptPrintLogOutPut( void * cd )
{
	crypt_set_log_callback( cd,_debug,NULL ) ;
}

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

static int _create_luks( const char * device,const resolve_path_t * opts )
{
	struct crypt_device * cd = NULL ;

	const arguments * args = opts->args ;

	if( crypt_init( &cd,device ) != 0 ){

		return 1 ;
	}

	if( args->integrity != NULL ){

		zuluCryptPrintLogOutPut( cd ) ;
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
		if( args->integrity ){

			if( args->format( cd ) == 0 ){

				return zuluExit_1( 0,cd ) ;
			}else{
				return zuluExit_1( 3,cd ) ;
			}
		}else{
			return zuluExit_1( 0,cd ) ;
		}
	}
}

static int _create_luks_0( arguments * args,const char * device,const char * key,
			   size_t key_len,const char * options )
{
	/*
	 * resolve_path_t is defined in includes.h
	 */
	resolve_path_t opts ;

	size_t list_count = 0 ;

	char * const * list = NULL ;

	stringList_t stl ;

	/*
	 * options is structure expected to be in a format of:
	 * rng.algo.cipher.keysize.hash.iteration_count
	 */

	if( StringHasNothing( options ) ){

		stl = StringList( "/dev/urandom" ) ;
	}else{
		stl = StringListSplit( options,'.' ) ;
	}

	StringListStringArray_1( &list,&list_count,stl ) ;

	if( list_count == 1 ){

		args->hash     = "sha256" ;
		args->cipher   = "xts-plain64" ;
		args->algo     = "aes" ;
		args->key_size = 64 ;
		args->rng      = *( list + 0 ) ;

	}else if( list_count >= 5 ){

		if( _not_supported_argument_encountered( list ) ){

			return zuluExit( 1,stl,list ) ;
		}else{
			args->rng      = *( list + 0 ) ;
			args->algo     = *( list + 1 ) ;
			args->cipher   = *( list + 2 ) ;
			args->key_size = ( size_t ) StringConvertToInt( *( list + 3 ) ) / 8 ;
			args->hash     = *( list + 4 ) ;

			if( list_count > 5 ){

				args->iterations = StringConvertToInt( *( list + 5 ) ) ;
			}

			if( list_count > 6 ){

				args->integrity = *( list + 6 ) ;
			}
		}
	}else{
		return zuluExit( 1,stl,list ) ;
	}

	args->key      = key ;
	args->key_len  = key_len ;

	args->params   = args->function( args ) ;

	opts.args         = args ;
	opts.device       = device ;
	opts.open_mode    = O_RDWR ;
	opts.error_value  = 2 ;

	/*
	 * zuluCryptResolveDevicePath() is defined in resolve_path.c
	 */
	return zuluExit( zuluCryptResolveDevicePath( _create_luks,&opts ),stl,list ) ;
}

static void * _luks1( const arguments * args )
{
	struct crypt_params_luks1 * params = args->params ;

	params->hash           = args->hash ;
	params->data_alignment = 4096 ;

	return params ;
}

static int _format_0( struct crypt_device * cd )
{
	if( cd ){}
	return 0 ;
}

int zuluCryptCreateLuks( const char * device,const char * key,size_t key_len,const char * options )
{
	struct crypt_params_luks1 params ;

	arguments args ;

	memset( &args,'\0',sizeof( args ) ) ;
	memset( &params,'\0',sizeof( params ) ) ;

	args.params   = &params ;
	args.function = _luks1 ;
	args.type     = CRYPT_LUKS1 ;
	args.format   = _format_0 ;

	return _create_luks_0( &args,device,key,key_len,options ) ;
}

#ifdef CRYPT_LUKS2

static int _tools_wipe_progress( uint64_t size,uint64_t offset,void * usrptr )
{
	int * progress = usrptr ;

	int x = ( int )( offset * 100 / size ) ;

	if( x > *progress ){

		printf( "%s complete: %d\n","%",x ) ;

		*progress = x ;
	}

	return 0 ;
}

static int _format( struct crypt_device * cd )
{
	int progress = 0 ;

	int i ;

	string_t s = String_1( crypt_get_dir(),"/",NULL ) ;

	string_t m = String( "zuluCrypt-wipe-volume-" ) ;

	const char * mapper = StringAppendInt( m,(u_int64_t)syscall( SYS_gettid ) ) ;

	int r = crypt_activate_by_volume_key( cd,
					      mapper,
					      NULL,
					      0,
					      CRYPT_ACTIVATE_PRIVATE | CRYPT_ACTIVATE_NO_JOURNAL ) ;
	if( r < 0 ){

		return 3 ;
	}

	printf( "----Starting to wipe an integrity device----\n" ) ;

	crypt_wipe( cd,
		    StringAppendString( s,m ),
		    CRYPT_WIPE_ZERO,
		    0,
		    0,
		    (size_t)crypt_get_sector_size( cd ) * 512,
		    0,
		    _tools_wipe_progress,
		    &progress ) ;

	printf( "----Finish wiping an integrity device----\n" ) ;

	for( i = 0 ; i < 3 ; i++ ){

		if( crypt_deactivate( cd,mapper ) == 0 ){

			break ;
		}else{
			sleep( 1 ) ;
		}
	}

	StringDelete( &m ) ;
	StringDelete( &s ) ;

	return 0 ;
}

void zuluCryptDisableMetadataLocking( void )
{
	crypt_metadata_locking( NULL,0 ) ;
}

static void * _luks2( const arguments * args )
{
	struct crypt_pbkdf_type * pbkdf  = args->pbkdf ;

	struct crypt_params_luks2 * params = args->params ;

	params->sector_size     = 512 ;
	params->integrity       = args->integrity ;

#if SUPPORT_crypt_get_pbkdf_default
	/*
	 * added in cryptsetup 2.0.3
	 */
	memcpy( pbkdf,crypt_get_pbkdf_default( CRYPT_LUKS2 ),sizeof( struct crypt_pbkdf_type ) ) ;
	pbkdf->hash             = args->hash ;
#else
	pbkdf->type             = CRYPT_KDF_ARGON2I ;
	pbkdf->hash             = args->hash ;
	pbkdf->time_ms          = 800 ;
	pbkdf->max_memory_kb    = 1024 ;
	pbkdf->parallel_threads = 4 ;
#endif
	params->pbkdf           = pbkdf ;

	return params ;
}

int zuluCryptCreateLuks2( const char * device,const char * key,size_t key_len,const char * options )
{
	struct crypt_params_luks2 params ;
	struct crypt_pbkdf_type pbkdf ;

	arguments args ;

	memset( &args,'\0',sizeof( args ) ) ;
	memset( &params,'\0',sizeof( params ) ) ;
	memset( &pbkdf,'\0',sizeof( pbkdf ) ) ;

	args.pbkdf    = &pbkdf ;
	args.params   = &params ;
	args.function = _luks2 ;
	args.type     = CRYPT_LUKS2 ;
	args.format   = _format ;

	return _create_luks_0( &args,device,key,key_len,options ) ;
}

#else

void zuluCryptDisableMetadataLocking( void )
{
}

int zuluCryptCreateLuks2( const char * device,const char * pass,size_t pass_size,const char * options )
{
	if( 0 && device && pass && pass_size && options ){;}
	return 2 ;
}

#endif
