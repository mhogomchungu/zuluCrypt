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

/*
 * This header file is created by the build process and it checks if the version of cryptsetup >= 1.4.0
 */
#include "luks_external_header.h"

static int zuluExit( int st,struct crypt_device * cd )
{
	crypt_free( cd ) ;
	return st ;
}

static int zuluExit_1( int r,struct crypt_device * cd,stringList_t stl )
{
	crypt_free( cd ) ;

	/*
	 * zuluCryptDeleteFile_1() is defined in open_path_security.c
	 */

	zuluCryptDeleteFile_1( StringListStringAtFirstPlace( stl ) ) ;

	StringListDelete( &stl ) ;

	return r ;
}

static int _open_luks_2( const char * device,const resolve_path_t * opt )
{
	struct crypt_device * cd ;
	uint32_t flags ;
	int st ;

	/*
	 * open_struct_t is defined in includes.h
	 */
	const open_struct_t * opts = opt->args ;

	if( zuluCryptPathIsNotValid( device ) ){

		return 3 ;
	}
	if( crypt_init( &cd,device ) != 0 ){

		return 2 ;
	}
	if( crypt_load( cd,NULL,NULL ) != 0 ){

		return zuluExit( 2,cd ) ;
	}
	if( opt->open_mode == O_RDONLY ){

		flags = CRYPT_ACTIVATE_READONLY ;
	}else{
		flags = CRYPT_ACTIVATE_ALLOW_DISCARDS ;
	}

	st = crypt_activate_by_passphrase( cd,opts->mapper_name,CRYPT_ANY_SLOT,
					   opts->key,opts->key_len,flags ) ;

	if( st >= 0 ){

		return zuluExit( 0,cd ) ;

	}else if( st == -1 ){

		return zuluExit( 1,cd ) ;
	}else{
		return zuluExit( 2,cd ) ;
	}
}

#if LUKS_EXTERNAL_HEADER
/*
 * This functionality is enabled with cryptsetup >= 1.4.0
 */

static int _open_luks_1( const char * device,const resolve_path_t * opt )
{
	u_int32_t key_len ;
	u_int32_t flags ;
	u_int32_t luks_header_file_size ;
	u_int32_t buffer_size ;

	stringList_t stl ;

	string_t * xt_0 = StringListArray( &stl,4 ) ;
	string_t * xt_1 = xt_0 + 1 ;
	string_t * xt_2 = xt_0 + 2 ;
	string_t * xt_3 = xt_0 + 3 ;

	const char * cipher ;
	const char * mode ;

	struct crypt_device * cd = NULL ;

	struct crypt_params_plain plain = { NULL,256,256,0 } ;

	/*
	 * open_struct_t is defined in includes.h
	 */
	const open_struct_t * opts = opt->args ;

	const size_t e = sizeof( u_int32_t ) ;

	char * key_0 ;
	const char * key ;
	const char * luks_header_file ;
	const char * luks_header_file_contents ;

	size_t size ;

	buffer_size = opts->key_len ;

	if( buffer_size < 1048576 + 8 || buffer_size > 8192000 ){
		/*
		 * the structure is expected to be atleast 1MB + 8 bytes
		 */
		/*
		 * cryptsetup has an 8MB limit somewhere i cant remember
		 */
		return 1 ;
	}
	
	/*
	 * opts->key variable is expected to hold a structure made up of 4 components.
	 * first  component at offset 0 is a u_int32_t structure holding the size of the passphrase
	 * Second component at offset 4 is a u_int32_t structure holding the size of the contents of luks header
	 * third  component at offset 8 is the passphrase to unlock the LUKS volume.
	 * last   component is at offset that marks the end of the third component.Where this offset will be depends on the length of the passphrase
	 */

	memcpy( &key_len,opts->key,e ) ;
	key = opts->key + e + e ;

	memcpy( &luks_header_file_size,opts->key + e,e ) ;
	luks_header_file_contents = opts->key + e + e + key_len  ;

	if( key_len + luks_header_file_size + e + e != buffer_size ){
		/*
		 * malformed structure detected
		 */
		return 1 ;
	}
	if( luks_header_file_size < 1048576 || luks_header_file_size > 3145728 ){
		/*
		 * luks header backup or detached header is expected to be greater than 1MB but less than 2MB,we check
		 * against 3MB to be generous.
		 */
		return 1 ;
	}
	/*
	 * zuluCryptCreateKeyFile() is defined in open_tcrypt.c
	 */
	*xt_0 = zuluCryptCreateKeyFile( luks_header_file_contents,luks_header_file_size,"luks_header_file-" ) ;
	luks_header_file = StringContent( *xt_0 ) ;

	if( crypt_init( &cd,luks_header_file ) != 0 ){

		return zuluExit_1( 1,cd,stl ) ;
	}
	if( crypt_load( cd,NULL,NULL ) != 0 ){

		return zuluExit_1( 1,cd,stl ) ;
	}

	if( opt->open_mode == O_RDONLY ){

		flags = CRYPT_ACTIVATE_READONLY ;
	}else{
		flags = CRYPT_ACTIVATE_ALLOW_DISCARDS ;
	}

	if( opts->luks_detached_header ){

		if( crypt_set_data_device( cd,device ) != 0 ){

			return zuluExit_1( 1,cd,stl ) ;
		}

		if( crypt_activate_by_passphrase( cd,opts->mapper_name,
						  CRYPT_ANY_SLOT,key,key_len,flags ) < 0 ){

			return zuluExit_1( 1,cd,stl ) ;
		}else{
			return zuluExit_1( 0,cd,stl ) ;
		}

	}else if( opts->general_detached_header ){

		size = crypt_get_volume_key_size( cd ) ;

		*xt_1 = StringBuffer( size ) ;

		key_0 = ( char * ) StringContent( *xt_1 ) ;

		if( crypt_volume_key_get( cd,CRYPT_ANY_SLOT,key_0,&size,key,key_len ) < 0 ){

			return zuluExit_1( 1,cd,stl ) ;
		}

		*xt_2 = String( crypt_get_cipher( cd ) ) ;
		*xt_3 = String( crypt_get_cipher_mode( cd ) ) ;

		crypt_free( cd ) ;

		if( crypt_init( &cd,device ) != 0 ){

			return zuluExit_1( 1,cd,stl ) ;
		}

		cipher = StringContent( *xt_2 ) ;
		mode   = StringContent( *xt_3 ) ;

		if( crypt_format( cd,CRYPT_PLAIN,cipher,mode,NULL,NULL,size,&plain ) != 0 ){

			return zuluExit_1( 1,cd,stl ) ;
		}

		if( crypt_activate_by_volume_key( cd,opts->mapper_name,key_0,size,flags ) != 0 ){

			return zuluExit_1( 1,cd,stl ) ;
		}else{
			return zuluExit_1( 0,cd,stl ) ;
		}
	}else{
		return zuluExit_1( 1,cd,stl ) ;
	}
}

#else

static int _open_luks_1( const char * device,const resolve_path_t * opts )
{
	if( 0 && device && opts ){;}
	return 1 ;
}

#endif

int zuluCryptOpenLuks( const char * device,const char * mapper,
		       const char * mode,const char * key,size_t key_len )
{
	open_struct_t opts ;

	memset( &opts,'\0',sizeof( open_struct_t ) ) ;

	opts.device      = device ;
	opts.mapper_name = mapper ;
	opts.m_opts      = mode   ;
	opts.key         = key    ;
	opts.key_len     = key_len;

	return zuluCryptOpenLuks_2( &opts ) ;
}

int zuluCryptOpenLuks_1( const open_struct_t * opts )
{
	/*
	 * zuluCryptResolveDevicePath_0() is defined in resolve_path.c
	 */
	return zuluCryptResolveDevicePath_0( _open_luks_1,opts,2 ) ;
}

int zuluCryptOpenLuks_2( const open_struct_t * opts )
{
	/*
	 * zuluCryptResolveDevicePath_0() is defined in resolve_path.c
	 */
	return zuluCryptResolveDevicePath_0( _open_luks_2,opts,2 ) ;
}
