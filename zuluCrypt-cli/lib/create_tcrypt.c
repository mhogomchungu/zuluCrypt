
 /*
  *
  *  Copyright (c) 2013
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

#if TRUECRYPT_CREATE

static int _create_file_system( const char * device,const char * fs,int key_source,
				const char * key,size_t key_len,int volume_type )
{
	string_t m = StringVoid ;

	int r ;

	const char * device_mapper ;
	const char * mapper ;

	size_t len ;

	m = String( crypt_get_dir() ) ;
	len = StringLength( m )   ;

	StringAppend( m,"/zuluCrypt-" ) ;
	device_mapper = StringAppendInt( m,syscall( SYS_gettid ) ) ;
	mapper = device_mapper + len + 1 ;

	/*
	 * zuluCryptOpenTcrypt() is defined in open_tcrypt.c
	 */
	if( zuluCryptOpenTcrypt( device,mapper,key,key_len,key_source,volume_type,NULL,0,0,NULL ) == 0 ){
		/*
		 * zuluCryptCreateFileSystemInAVolume() is defined in create_volume.c
		 */
		if( zuluCryptCreateFileSystemInAVolume( fs,device_mapper ) == 0 ){
			r = 0 ;
		}else{
			r = 3 ;
		}
		/*
		 * zuluCryptCloseMapper() is defined in close_mapper.c
		 */
		zuluCryptCloseMapper( device_mapper ) ;
	}else{
		r = 3 ;
	}

	StringDelete( &m ) ;
	return r ;
}

#if TCPLAY_NEW_API

/*
 * tcplay >= 1.2.0
 */
static int _create_tcrypt_volume( const char * device,const char * file_system,
				  const char * rng,const char * key,size_t key_len,
				  int key_source,u_int64_t hidden_volume_size,
				  const char * file_system_h,const char * key_h,size_t key_len_h,int key_source_h )
{
	string_t st = StringVoid ;
	string_t xt = StringVoid ;

	tc_api_task task ;

	int r = 3 ;

	const char * keyfile = NULL ;
	const char * keyfile_h = NULL ;

	const char * pass = NULL ;
	const char * pass_h = NULL ;

	if( zuluCryptPathIsNotValid( device ) ){
		return 1 ;
	}

	if( tc_api_init( 0 ) == TC_OK ){
		task = tc_api_task_init( "create" ) ;
		if( task != NULL ){

			if( StringPrefixMatch( rng,"/dev/urandom",12 ) ){
				tc_api_task_set( task,"weak_keys_and_salt",1 ) ;
			}

			tc_api_task_set( task,"dev",device ) ;
			tc_api_task_set( task,"secure_erase",0 ) ;
			tc_api_task_set( task,"prf_algo","RIPEMD160" ) ;
			tc_api_task_set( task,"cipher_chain","AES-256-XTS" ) ;

			if( key_source == TCRYPT_PASSPHRASE ){
				tc_api_task_set( task,"passphrase",key ) ;
				pass = key ;
			}else{
				key_source = TCRYPT_KEYFILE_FILE ;
				/*
				 * zuluCryptCreateKeyFile() is defined in open_tcrypt.c
				 */
				st = zuluCryptCreateKeyFile( key,key_len,"/run/zuluCrypt/create_tcrypt-1-" ) ;
				pass = StringContent( st ) ;
				keyfile = pass ;
				tc_api_task_set( task,"keyfiles",keyfile ) ;
			}

			if( hidden_volume_size > 0 ){

				tc_api_task_set( task,"hidden",1 ) ;
				tc_api_task_set( task,"hidden_size_bytes",hidden_volume_size ) ;
				tc_api_task_set( task,"h_prf_algo","RIPEMD160" ) ;
				tc_api_task_set( task,"h_cipher_chain","AES-256-XTS" ) ;

				if( key_source_h == TCRYPT_PASSPHRASE ){
					pass_h = key_h ;
					tc_api_task_set( task,"h_passphrase",key_h ) ;
				}else{
					key_source_h = TCRYPT_KEYFILE_FILE ;
					xt = zuluCryptCreateKeyFile( key,key_len,"/run/zuluCrypt/create_tcrypt-2-" ) ;
					pass_h = StringContent( xt ) ;
					keyfile_h = pass_h ;
					tc_api_task_set( task,"h_keyfiles",keyfile_h ) ;
				}
			}

			if( tc_api_task_do( task ) == TC_OK ){
				r = _create_file_system( device,file_system,key_source,pass,key_len,TCRYPT_NORMAL ) ;
				if( hidden_volume_size > 0 && r == 0 ){
					r = _create_file_system( device,file_system_h,key_source_h,pass_h,key_len_h,TCRYPT_HIDDEN ) ;
				}
			}

			/*
			 * zuluCryptDeleteFile() is defined in file_path_security.c
			 */
			if( keyfile != NULL ){
				zuluCryptDeleteFile( keyfile ) ;
				StringDelete( &st ) ;
			}
			if( keyfile_h != NULL ){
				zuluCryptDeleteFile( keyfile_h ) ;
				StringDelete( &xt ) ;
			}

			tc_api_task_uninit( task ) ;
		}

		tc_api_uninit() ;
	}

	return r ;
}

#else
/*
 * tcplay >= 1.0.0 < 1.2.0
 */
static int _create_tcrypt_volume( const char * device,const char * file_system,
				  const char * rng,const char * key,size_t key_len,
				  int key_source,u_int64_t hidden_volume_size,
				  const char * file_system_h,const char * key_h,size_t key_len_h,int key_source_h )
{
	string_t st = StringVoid ;
	string_t xt = StringVoid ;

	tc_api_opts api_opts ;

	int r = 3 ;

	const char * keyfiles[ 2 ] = { NULL,NULL } ;
	const char * keyfiles_h[ 2 ] = { NULL,NULL } ;

	const char * pass = NULL ;
	const char * pass_h = NULL ;

	if( zuluCryptPathIsNotValid( device ) ){
		return 1 ;
	}

	if( tc_api_init( 0 ) == TC_OK ){

		memset( &api_opts,'\0',sizeof( api_opts ) ) ;

		api_opts.tc_device          = device ;
		api_opts.tc_cipher          = "AES-256-XTS";
		api_opts.tc_prf_hash        = "RIPEMD160"  ;
		api_opts.tc_no_secure_erase = 1 ;

		if( StringPrefixMatch( rng,"/dev/urandom",12 ) ){
			api_opts.tc_use_weak_keys = 1 ;
		}

		if( key_source == TCRYPT_PASSPHRASE ){
			api_opts.tc_passphrase = key ;
			pass = key ;
		}else{
			key_source = TCRYPT_KEYFILE_FILE ;
			st = zuluCryptCreateKeyFile( key,key_len,"/run/zuluCrypt/create_tcrypt-1-" ) ;
			pass = StringContent( st ) ;
			keyfiles[ 0 ] = pass ;
			api_opts.tc_keyfiles = keyfiles ;
		}

		if( hidden_volume_size > 0 ){

			api_opts.tc_size_hidden_in_bytes = hidden_volume_size ;
			api_opts.tc_cipher_hidden    = "AES-256-XTS";
			api_opts.tc_prf_hash_hidden  = "RIPEMD160"  ;

			if( key_source_h == TCRYPT_PASSPHRASE ){
				api_opts.tc_passphrase_hidden = key_h ;
				pass_h = key_h ;
			}else{
				key_source_h = TCRYPT_KEYFILE_FILE ;
				xt = zuluCryptCreateKeyFile( key,key_len,"/run/zuluCrypt/create_tcrypt-2-" ) ;
				pass_h = StringContent( xt ) ;
				keyfiles_h[ 0 ] = pass_h ;
				api_opts.tc_keyfiles_hidden = keyfiles_h ;
			}
		}

		if( tc_api_create_volume( &api_opts ) == TC_OK ){
			r = _create_file_system( device,file_system,key_source,pass,key_len,TCRYPT_NORMAL ) ;
			if( hidden_volume_size > 0 && r == 0 ){
				r = _create_file_system( device,file_system_h,key_source_h,pass_h,key_len_h,TCRYPT_HIDDEN ) ;
			}
		}

		/*
		 * zuluCryptDeleteFile() is defined in file_path_security.c
		 */
		if( keyfiles[ 0 ] != NULL ){
			zuluCryptDeleteFile( keyfiles[ 0 ] ) ;
			StringDelete( &st ) ;
		}
		if( keyfiles_h[ 0 ] != NULL ){
			zuluCryptDeleteFile( keyfiles_h[ 0 ] ) ;
			StringDelete( &xt ) ;
		}

		tc_api_uninit() ;
	}

	return r ;
}

#endif

int zuluCryptCreateTCrypt( const char * device,const char * file_system,const char * rng,
			   const char * key,size_t key_len,int key_source,
			   u_int64_t hidden_volume_size,
			   const char * file_system_h,const char * key_h,size_t key_len_h,int key_source_h )
{
	int fd ;
	string_t q = StringVoid ;
	int r ;
	if( StringPrefixMatch( device,"/dev/",5 ) ){
		r = _create_tcrypt_volume( device,file_system,rng,key,key_len,key_source,
					   hidden_volume_size,file_system_h,key_h,key_len_h,key_source_h ) ;
	}else{
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in create_loop_device.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( device,O_RDWR,&fd,&q ) ){
			device = StringContent( q ) ;
			r = _create_tcrypt_volume( device,file_system,rng,key,key_len,key_source,
						   hidden_volume_size,file_system_h,key_h,key_len_h,key_source_h ) ;
			close( fd ) ;
			StringDelete( &q ) ;
		}else{
			r = 3 ;
		}
	}

	return r ;
}

#else
/*
 * tcplay < 1.0.0 or tcplay not found
 */
int zuluCryptCreateTCrypt( const char * device,const char * file_system,const char * rng,
			   const char * key,size_t key_len,int key_source,
			   u_int64_t hidden_volume_size,const char * file_system_h,
			   const char * key_h,size_t key_len_h,int key_source_h )
{
	if( 0 && device && file_system && rng && key_len && key_source && key
		&& hidden_volume_size && key_len_h && key_h && key_source_h && file_system_h ){;}
	return 3 ;
}
#endif