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
#include <fcntl.h>
#include <unistd.h>

#include <libcryptsetup.h>

static int zuluExit( int st,struct crypt_device * cd,stringList_t stl,char * const * options )
{
	StringFree( options ) ;
	StringListDelete( &stl ) ;
	if( cd != NULL ){
		crypt_free( cd ) ;
	}
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

				if( StringAtLeastOneMatch_1( e,"sha1","sha256","sha512","ripemd160","whirlpool",NULL ) ){

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
static int _create_luks( const char * dev,const char * pass,size_t pass_size,const char * opts )
{
	struct crypt_device * cd = NULL ;
	struct crypt_params_luks1 params ;

	const char * rng    = "/dev/urandom" ;
	const char * hash   = "sha1" ;
	const char * cipher = "xts-plain64" ;
	const char * algo   = "aes" ;
	size_t keySize      = 32 ;

	char * const * options = NULL ;
	size_t options_count = 0 ;

	stringList_t stl ;

	if( zuluCryptPathIsNotValid( dev ) ){
		return 4 ;
	}
	if( crypt_init( &cd,dev ) != 0 ){
		return 1 ;
	}
	if( StringsAreEqual( opts,"" ) ){
		stl = StringListSplit( "/dev/urandom.aes.xts-plain64.256.sha1",'.' ) ;
	}else{
		stl = StringListSplit( opts,'.' ) ;
	}

	options = StringListStringArray_1( options,&options_count,stl ) ;

	if( options_count == 1 ){

		rng = *( options + 0 ) ;

	}else if( options_count == 5 ){

		if( _not_supported_argument_encountered( options ) ){

			return zuluExit( 1,cd,stl,options ) ;
		}else{
			rng     = *( options + 0 ) ;
			algo    = *( options + 1 ) ;
			cipher  = *( options + 2 ) ;
			keySize = ( size_t ) StringConvertToInt( *( options + 3 ) ) / 8 ;
			hash    = *( options + 4 ) ;
		}
	}else{
		return zuluExit( 1,cd,stl,options ) ;
	}

	memset( &params,'\0',sizeof( struct crypt_params_luks1 ) ) ;

	params.hash           = hash ;
	params.data_alignment = 4096 ;

	if( StringsAreEqual( rng,"/dev/random" ) ){
		crypt_set_rng_type( cd,CRYPT_RNG_RANDOM ) ;
	}else{
		crypt_set_rng_type( cd,CRYPT_RNG_URANDOM ) ;
	}

	if( crypt_format( cd,CRYPT_LUKS1,algo,cipher,NULL,NULL,keySize,&params ) != 0 ){
		return zuluExit( 2,cd,stl,options ) ;
	}

	if( crypt_keyslot_add_by_volume_key( cd,CRYPT_ANY_SLOT,NULL,keySize,pass,pass_size ) < 0 ){
		return zuluExit( 3,cd,stl,options ) ;
	}else{
		return zuluExit( 0,cd,stl,options ) ;
	}
}

int zuluCryptCreateLuks( const char * dev,const char * pass,size_t pass_size,const char * opts )
{
	string_t st ;
	int fd ;
	int r ;
	if( StringPrefixEqual( dev,"/dev/" ) ){
		return _create_luks( dev,pass,pass_size,opts ) ;
	}else{
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in ./create_loop.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( dev,O_RDWR,&fd,&st ) ){
			r = _create_luks( StringContent( st ),pass,pass_size,opts ) ;
			StringDelete( &st ) ;
			close( fd ) ;
			return r ;
		}else{
			return 2 ;
		}
	}
}
