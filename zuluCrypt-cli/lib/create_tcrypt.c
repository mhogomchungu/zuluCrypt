
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

typedef struct{
	const char * device ;
	const char * key ;
	int          key_source ;
	const char * key_source_1 ;
	const char * key_h ;
	int          key_source_h ;
	const char * key_source_h_1 ;
	int          weak_keys_and_salt ;
	u_int64_t    hidden_volume_size ;
}tcrypt_t ;

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
static int _create_volume( const tcrypt_t * info )
{
	int r = !TC_OK ;

	tc_api_task task ;

	if( tc_api_init( 0 ) == TC_OK ){
		task = tc_api_task_init( "create" ) ;
		if( task != NULL ){

			tc_api_task_set( task,"dev",info->device ) ;
			tc_api_task_set( task,"secure_erase",(int)0 ) ;
			tc_api_task_set( task,"prf_algo","RIPEMD160" ) ;
			tc_api_task_set( task,"cipher_chain","AES-256-XTS" ) ;
			tc_api_task_set( task,info->key_source_1,info->key ) ;

			if( info->weak_keys_and_salt ){
				tc_api_task_set( task,"weak_keys_and_salt",1 ) ;
			}

			if( info->hidden_volume_size > 0 ){

				tc_api_task_set( task,"hidden",(int)1 ) ;
				tc_api_task_set( task,"hidden_size_bytes",info->hidden_volume_size ) ;
				tc_api_task_set( task,"h_prf_algo","RIPEMD160" ) ;
				tc_api_task_set( task,"h_cipher_chain","AES-256-XTS" ) ;
				tc_api_task_set( task,info->key_source_h_1,info->key_h ) ;
			}

			r = tc_api_task_do( task ) ;

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
static int _create_volume( const tcrypt_t * info )
{
	int r = !TC_OK ;

	tc_api_opts api_opts ;

	const char * keyfiles[ 2 ] = { NULL,NULL } ;
	const char * keyfiles_h[ 2 ] = { NULL,NULL } ;

	if( tc_api_init( 0 ) == TC_OK ){

		memset( &api_opts,'\0',sizeof( api_opts ) ) ;

		api_opts.tc_device          = info->device ;
		api_opts.tc_cipher          = "AES-256-XTS";
		api_opts.tc_prf_hash        = "RIPEMD160"  ;
		api_opts.tc_no_secure_erase = 1 ;

		if( info->weak_keys_and_salt ){
			api_opts.tc_use_weak_keys = 1 ;
		}

		if( info->key_source == TCRYPT_PASSPHRASE ){
			api_opts.tc_passphrase = info->key ;
		}else{
			keyfiles[ 0 ] = info->key ;
			api_opts.tc_keyfiles = keyfiles ;
		}

		if( info->hidden_volume_size > 0 ){

			api_opts.tc_size_hidden_in_bytes = info->hidden_volume_size ;
			api_opts.tc_cipher_hidden    = "AES-256-XTS";
			api_opts.tc_prf_hash_hidden  = "RIPEMD160"  ;

			if( info->key_source_h == TCRYPT_PASSPHRASE ){
				api_opts.tc_passphrase_hidden = info->key_h ;
			}else{
				keyfiles_h[ 0 ] = info->key_h ;
				api_opts.tc_keyfiles_hidden = keyfiles_h ;
			}
		}

		r = tc_api_create_volume( &api_opts ) ;

		tc_api_uninit() ;
	}

	return r ;
}

#endif

static int _create_tcrypt_volume( const char * device,const char * file_system,
				  const char * rng,const char * key,size_t key_len,
				  int key_source,u_int64_t hidden_volume_size,
				  const char * file_system_h,const char * key_h,size_t key_len_h,int key_source_h )
{
	string_t st = StringVoid ;
	string_t xt = StringVoid ;

	tcrypt_t info ;

	int r = 3 ;

	if( zuluCryptPathIsNotValid( device ) ){
		return 1 ;
	}

	memset( &info,'\0',sizeof( tcrypt_t ) ) ;

	info.device = device ;
	info.key_source = key_source ;
	info.key_source_h = key_source_h ;
	info.hidden_volume_size = hidden_volume_size ;

	if( StringPrefixMatch( rng,"/dev/urandom",12 ) ){
		info.weak_keys_and_salt = 1 ;
	}

	if( info.key_source == TCRYPT_PASSPHRASE ){
		info.key = key ;
		info.key_source_1 = "passphrase" ;
	}else{
		/*
		 * zuluCryptCreateKeyFile() is defined in open_tcrypt.c
		 */
		st = zuluCryptCreateKeyFile( key,key_len,"create_tcrypt-1-" ) ;
		info.key = StringContent( st ) ;
		info.key_source = TCRYPT_KEYFILE_FILE ;
		info.key_source_1 = "keyfiles" ;
	}

	if( info.hidden_volume_size > 0 ){

		if( info.key_source_h == TCRYPT_PASSPHRASE ){
			info.key_h = key_h ;
			info.key_source_h_1 = "h_passphrase" ;
		}else{
			xt = zuluCryptCreateKeyFile( key_h,key_len_h,"create_tcrypt-2-" ) ;
			info.key_h = StringContent( xt ) ;
			info.key_source_h = TCRYPT_KEYFILE_FILE ;
			info.key_source_h_1 = "h_keyfiles" ;
		}
	}

	if( _create_volume( &info ) == TC_OK ){
		r = _create_file_system( device,file_system,info.key_source,info.key,key_len,TCRYPT_NORMAL ) ;
		if( info.hidden_volume_size > 0 && r == 0 ){
			r = _create_file_system( device,file_system_h,info.key_source_h,info.key_h,key_len_h,TCRYPT_HIDDEN ) ;
		}
	}

	/*
	 * zuluCryptDeleteFile() is defined in file_path_security.c
	 */
	if( st != StringVoid ){
		zuluCryptDeleteFile( StringContent( st ) ) ;
		StringDelete( &st ) ;
	}
	if( xt != StringVoid ){
		zuluCryptDeleteFile( StringContent( xt ) ) ;
		StringDelete( &xt ) ;
	}

	return r ;
}

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