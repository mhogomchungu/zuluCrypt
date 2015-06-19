
 /*
  *
  *  Copyright (c) 2013
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
#include <sys/syscall.h>
#include <sys/syscall.h>
#include <libcryptsetup.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 * this header is created at config time
 */
#include "truecrypt_support_1.h"

static const int TRUE  = 1 ;
static const int FALSE = 0 ;

static int _create_file_system( const create_tcrypt_t * e )
{
	string_t m = StringVoid ;

	int r ;

	const char * device_mapper ;
	const char * mapper ;

	size_t len ;

	open_struct_t opts ;

	memset( &opts,'\0',sizeof( open_struct_t ) ) ;

	m = String( crypt_get_dir() ) ;
	len = StringLength( m )   ;

	StringAppend( m,"/zuluCrypt-" ) ;
	device_mapper = StringAppendInt( m,syscall( SYS_gettid ) ) ;
	mapper = device_mapper + len + 1 ;

	opts.volume_type = TCRYPT_NORMAL ;
	opts.device = e->device ;
	opts.mapper_name = mapper ;
	opts.m_opts = "rw" ;

	opts.key     = e->passphrase ;
	opts.key_len = e->passphrase_size ;

	if( e->keyfiles != NULL ){
		opts.tcrypt_keyfiles       = e->keyfiles ;
		opts.tcrypt_keyfiles_count = e->keyfiles_number ;
	}

	opts.veraCrypt_volume = e->veraCrypt_volume ;
	opts.iteration_count  = e->iteration_count ;

	/*
	 * zuluCryptOpenTcrypt_1 is defined in open_tcrypt.c
	 */
	r = zuluCryptOpenTcrypt_1( &opts ) ;

	if( r == 0 ){
		/*
		 * zuluCryptCreateFileSystemInAVolume() is defined in create_volume.c
		 */
		if( zuluCryptCreateFileSystemInAVolume( e->fs,device_mapper ) == 0 ){

			r = 0 ;
		}else{
			r = 3 ;
		}
		/*
		 * zuluCryptCloseMapper() is defined in close_mapper.c
		 */
		zuluCryptCloseMapper( device_mapper ) ;
	}

	if( r == 0 && e->hidden_volume_size > 0 ){

		opts.volume_type = TCRYPT_HIDDEN ;

		opts.key     = e->passphrase_h ;
		opts.key_len = e->passphrase_h_size ;

		if( e->keyfiles_h != NULL ){
			opts.tcrypt_keyfiles       = e->keyfiles_h ;
			opts.tcrypt_keyfiles_count = e->keyfiles_h_number ;
		}

		r = zuluCryptOpenTcrypt_1( &opts ) ;

		if( r == 0 ){

			if( zuluCryptCreateFileSystemInAVolume( e->fs_h,device_mapper ) == 0 ){

				r = 0 ;
			}else{
				r = 3 ;
			}

			/*
			 * zuluCryptCloseMapper() is defined in close_mapper.c
			 */
			zuluCryptCloseMapper( device_mapper ) ;
		}
	}

	StringDelete( &m ) ;
	return r ;
}

static string_t _root_device( const char * device,const char ** sys_device )
{
	size_t e ;

	ssize_t r ;

	string_t st = String( device ) ;

	if( StringStartsWithAtLeastOne( st,"/dev/sd","/dev/hd",NULL ) ){
		/*
		 * this path will convert something like: "/dev/sdc12" to "/dev/sdc".
		 * basically,it removes digits from the end of the string to give the root device
		 * required by tcplay's system volume or fde volume
		 */
		*sys_device = StringRemoveDigits( st ) ;
	}else if( StringStartsWith( st,"/dev/mmc" ) ){
		/*
		 * device path will be something like "/dev/mmcblk0p2" and what we want to do
		 * is cut off the string from p to end iwth "/dev/mmcblk0"
		 */
		r = StringIndexOfChar( st,0,'p' ) ;
		if( r != -1 ){
			e = StringLength( st ) - ( size_t )r ;
			*sys_device = StringRemoveRight( st,e ) ;
		}else{
			*sys_device = StringContent( st ) ;
		}
	}else{
		*sys_device = StringContent( st ) ;
	}
	return st ;
}

static int _modify_tcrypt_header( const char * device,const resolve_path_t * opts )
{
	tc_api_task task ;

	int r = !TC_OK ;

	const char * sys_device = NULL ;

	const info_t * info = opts->args ;

	string_t st = StringVoid ;

	if( info->device == NULL ){
		return r ;
	}
	if( tc_api_initialize() ){

		if( tc_api_task_initialize( &task,"modify" ) ){

			if( StringsAreEqual( info->opt,"sys" ) ){

				tc_api_task_set( task,"dev",device ) ;
				st = _root_device( device,&sys_device ) ;
				tc_api_task_set( task,"sys",sys_device ) ;

			}else if( StringsAreEqual( info->opt,"fde" ) ){

				st = _root_device( device,&sys_device ) ;

				tc_api_task_set( task,"dev",sys_device ) ;
				tc_api_task_set( task,"fde",TRUE ) ;
			}else{
				tc_api_task_set( task,"dev",device ) ;
			}

			tc_api_task_set( task,"hidden_size_bytes",( u_int64_t )0 ) ;
			tc_api_task_set( task,info->header_source,info->tmp_path ) ;
			tc_api_task_set( task,info->header_key_source,info->header_key ) ;
			tc_api_task_set( task,info->header_new_key_source,info->header_new_key ) ;
			
			tc_api_task_set( task,"weak_keys_and_salt",StringsAreEqual( info->rng,"/dev/urandom" ) ) ;

			r = tc_api_task_do( task ) ;

			tc_api_task_uninit( task ) ;
		}

		tc_api_uninit() ;
	}
	StringDelete( &st ) ;
	return r ;
}

int zuluCryptModifyTcryptHeader( const info_t * info )
{
	/*
	 * resolve_path_t is defined in includes.h
	 */
	resolve_path_t opts ;

	memset( &opts,'\0',sizeof( resolve_path_t ) ) ;

	opts.device      = info->device ;
	opts.args        = info ;
	opts.error_value = 3 ;
	opts.open_mode   = O_RDWR ;

	/*
	 * zuluCryptResolveDevicePath() is defined in resolve_path.c
	 */
	return zuluCryptResolveDevicePath( _modify_tcrypt_header,&opts ) ;
}

struct{
	const char * first ;
	const char * second;
} pair[] = {
	{ "aes"                ,"AES-256-XTS" },
	{ "twofish"            ,"TWOFISH-256-XTS" },
	{ "serpent"            ,"SERPENT-256-XTS" },
	{ "twofish:aes"        ,"TWOFISH-256-XTS,AES-256-XTS" },
	{ "aes:serpent"        ,"AES-256-XTS,SERPENT-256-XTS" },
	{ "serpent:twofish"    ,"SERPENT-256-XTS,TWOFISH-256-XTS" },
	{ "aes:twofish:serpent","AES-256-XTS,TWOFISH-256-XTS,SERPENT-256-XTS" },
	{ "serpent:twofish:aes","SERPENT-256-XTS,TWOFISH-256-XTS,AES-256-XTS" },
	{ NULL                 ,NULL }
} ;

const char * zuluCryptConvertCipher( const char * p )
{
	int i ;

	const char * q ;

	for( i = 0 ; ; i++ ){

		q = pair[ i ].second ;

		if( q == NULL ){

			break ;

		}else if( StringsAreEqual( p,q ) ){

			return pair[ i ].first ;
		}
	}

	return "Nil" ;
}

static const char * _set_cipher_chain( char * const * z )
{
	int i ;

	const char * q ;

	const char * p = *z ;

	for( i = 0 ; ; i++ ){

		q = pair[ i ].first ;

		if( q == NULL ){

			break ;

		}else if( StringsAreEqual( p,q ) ){

			return pair[ i ].second ;
		}
	}

	return NULL ;
}

static const char * _set_hash( char * const * q )
{
	const char * e = *q ;

	if( StringsAreEqual( e,"ripemd160" ) ){

		return "RIPEMD160" ;

	}else if( StringsAreEqual( e,"whirlpool" ) ){

		/*
		 * zuluCryptWhirlpoolIsSupported() is defined in include.h
		 */
		if( zuluCryptWhirlpoolIsSupported() ){

			return "whirlpool" ;
		}else{
			return NULL ;
		}

	}else if( StringsAreEqual( e,"sha512" ) ){

		return "SHA512" ;
	}else{
		return NULL ;
	}
}

static int _zuluExit( int r,char * const * options,stringList_t stl )
{
	StringFree( options ) ;
	StringListDelete( &stl ) ;
	return r ;
}

int zuluCryptVeraCryptPIM( int e )
{
	if( e > 0 ){
		return 15000 + ( e * 1000 ) ;
	}else{
		return 0 ;
	}
}

/*
 * e->encryption_options will contain a structure in the following format:
 * rng.algorithm.cipher mode.key size in bits.hash function
 *
 * different components of the structure are separated by "." character
 * multiple algorithms are separated by ":" character.
 *
 * example structures:
 * /dev/urandom.aes.xts-plain64.256.ripemd160
 * /dev/urandom.aes:twofish:serpent.xts-plain64.256.ripemd160
 *
 * key size field and cipher mode field are currently not in use
 */
static int _create_tcrypt_volume( const char * device,const resolve_path_t * opts )
{
	tc_api_task task ;
	int r = !TC_OK ;
	size_t i ;
	size_t k ;
	const char * const * z ;

	const char * cipher_chain ;
	const char * hash ;
	const char * rng ;

	char * const * options = NULL ;
	size_t options_count = 0 ;

	stringList_t stl ;

	const create_tcrypt_t * e = opts->args ;

	if( e->veraCrypt_volume && !VERACRYPT_CREATE ){

		return !TC_OK ;
	}

	if( StringHasNothing( e->encryption_options ) ){

		stl = StringList( "/dev/urandom" ) ;
	}else{
		stl = StringListSplit( e->encryption_options,'.' ) ;
	}

	StringListStringArray_1( &options,&options_count,stl ) ;

	if( options_count == 1 ){

		cipher_chain = "AES-256-XTS" ;
		rng          = *( options + 0 ) ;

		if( e->veraCrypt_volume ){

			hash = "SHA512" ;
		}else{
			hash = "RIPEMD160" ;
		}

	}else if( options_count == 5 ){

		cipher_chain = _set_cipher_chain( options + 1 ) ;
		hash         = _set_hash( options + 4 ) ;
		rng          = *( options + 0 ) ;

		if( cipher_chain == NULL || hash == NULL ){
			return _zuluExit( !TC_OK,options,stl ) ;
		}
	}else{
		return _zuluExit( !TC_OK,options,stl ) ;
	}

	if( tc_api_initialize() ){

		if( tc_api_task_initialize( &task,"create" ) ){

			tc_api_task_set( task,"veracrypt_mode",e->veraCrypt_volume ) ;
			tc_api_task_set( task,"iteration_count",zuluCryptVeraCryptPIM( e->iteration_count ) ) ;
			tc_api_task_set( task,"dev",device ) ;
			tc_api_task_set( task,"secure_erase",FALSE ) ;
			tc_api_task_set( task,"prf_algo",hash ) ;
			tc_api_task_set( task,"cipher_chain",cipher_chain ) ;
			tc_api_task_set( task,"passphrase",e->passphrase ) ;
			tc_api_task_set( task,"weak_keys_and_salt",StringsAreEqual( rng,"/dev/urandom" ) ) ;

			z = e->keyfiles ;
			k = e->keyfiles_number ;

			for( i = 0 ; i < k ; i++ ){
				tc_api_task_set( task,"keyfiles",*( z + i ) ) ;
			}

			if( e->hidden_volume_size > 0 ){

				tc_api_task_set( task,"hidden_size_bytes",e->hidden_volume_size ) ;
				tc_api_task_set( task,"h_prf_algo",hash ) ;
				tc_api_task_set( task,"h_cipher_chain",cipher_chain ) ;
				tc_api_task_set( task,"h_passphrase",e->passphrase_h ) ;

				z = e->keyfiles_h ;
				k = e->keyfiles_h_number ;

				for( i = 0 ; i < k ; i++ ){
					tc_api_task_set( task,"h_keyfiles",*( z + i ) ) ;
				}
			}

			r = tc_api_task_do( task ) ;

			tc_api_task_uninit( task ) ;

			if( r == TC_OK ){
				r = _create_file_system( e ) ;
			}
		}

		tc_api_uninit() ;
	}

	return _zuluExit( r,options,stl ) ;
}

int zuluCryptCreateTCryptVolume( const create_tcrypt_t * tcrypt )
{
	/*
	 * resolve_path_t is defined in includes.h
	 */
	resolve_path_t opts ;

	memset( &opts,'\0',sizeof( resolve_path_t ) ) ;

	opts.device      = tcrypt->device ;
	opts.args        = tcrypt ;
	opts.error_value = 3 ;
	opts.open_mode   = O_RDWR ;

	/*
	 * zuluCryptResolveDevicePath() is defined in resolve_path.c
	 */
	return zuluCryptResolveDevicePath( _create_tcrypt_volume,&opts ) ;
}

int zuluCryptCreateTCrypt( const char * device,const char * file_system,const char * rng,
			   const char * key,size_t key_len,int key_source,
			   u_int64_t hidden_volume_size,
			   const char * file_system_h,const char * key_h,size_t key_len_h,int key_source_h )
{
	string_t st = StringVoid ;
	string_t xt = StringVoid ;

	const char * keyFile ;
	const char * keyFile_h ;

	int r ;

	create_tcrypt_t tcrypt ;

	memset( &tcrypt,'\0',sizeof( create_tcrypt_t ) ) ;

	tcrypt.device = device ;
	tcrypt.fs = file_system ;
	tcrypt.fs_h = file_system_h ;
	tcrypt.hidden_volume_size = hidden_volume_size ;
	tcrypt.encryption_options = rng ;

	if( key_source == TCRYPT_PASSPHRASE ){
		tcrypt.passphrase = key ;
	}else{
		/*
		 * zuluCryptCreateKeyFile() is defined in open_tcrypt.c
		 */
		st = zuluCryptCreateKeyFile( key,key_len,"create_tcrypt-1-" ) ;
		keyFile = StringContent( st ) ;
		tcrypt.keyfiles = &keyFile ;
		tcrypt.keyfiles_number = 1 ;
	}

	if( tcrypt.hidden_volume_size > 0 ){
		if( key_source_h == TCRYPT_PASSPHRASE ){
			tcrypt.passphrase_h = key_h ;
		}else{
			xt = zuluCryptCreateKeyFile( key_h,key_len_h,"create_tcrypt-2-" ) ;
			keyFile_h = StringContent( xt ) ;
			tcrypt.keyfiles_h = &keyFile_h ;
			tcrypt.keyfiles_h_number = 1 ;
		}
	}

	r = zuluCryptCreateTCryptVolume( &tcrypt ) ;

	/*
	 * zuluCryptDeleteFile_1() is defined in file_path_security.c
	 */
	if( st != StringVoid ){
		zuluCryptDeleteFile_1( st ) ;
		StringDelete( &st ) ;
	}
	if( xt != StringVoid ){
		zuluCryptDeleteFile_1( xt ) ;
		StringDelete( &xt ) ;
	}

	return r ;
}
