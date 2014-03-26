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
#include <libcryptsetup.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * This header file is created by the build process and it checks if the version of cryptsetup >= 1.4.0
 */
#include "luks_external_header.h"

static inline int zuluExit( int st,struct crypt_device * cd )
{
	crypt_free( cd ) ;
	return st ;
}

static inline int zuluExit_1( int r,struct crypt_device * cd,string_t st )
{
	crypt_free( cd ) ;
	/*
	 * zuluCryptDeleteFile() is defined in open_path_security.c
	 */
	zuluCryptDeleteFile( StringContent( st ) ) ;
	StringDelete( &st ) ;
	return r ;
}

static int _open_luks( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{
	struct crypt_device * cd ;
	uint32_t flags ;
	int st ;

	if( zuluCryptPathIsNotValid( device ) ){
		return 3 ;
	}
	if( crypt_init( &cd,device ) != 0 ){
		return 2 ;
	}
	if( crypt_load( cd,NULL,NULL ) != 0 ){
		return zuluExit( 2,cd ) ;
	}
	if( StringHasComponent( mode,"ro" ) ){
		flags = CRYPT_ACTIVATE_READONLY ;
	}else{
		flags = CRYPT_ACTIVATE_ALLOW_DISCARDS ;
	}

	st = crypt_activate_by_passphrase( cd,mapper,CRYPT_ANY_SLOT,pass,pass_size,flags ) ;

	if( st >= 0 ){
		return zuluExit( 0,cd ) ;
	}else if( st == -1 ){
		return zuluExit( 1,cd ) ;
	}else{
		return zuluExit( 2,cd ) ;
	}
}

int zuluCryptOpenLuks( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{
	int lmode ;
	string_t st ;
	int fd ;
	int r ;
	if( StringPrefixMatch( device,"/dev/",5 ) ){
		return _open_luks( device,mapper,mode,pass,pass_size ) ;
	}else{
		if( StringHasComponent( mode,"ro" ) ){
			lmode = O_RDONLY ;
		}else{
			lmode = O_RDWR ;
		}
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in ./create_loop.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( device,lmode,&fd,&st ) ){
			r = _open_luks( StringContent( st ),mapper,mode,pass,pass_size ) ;
			StringDelete( &st ) ;
			close( fd ) ;
			return r ;
		}else{
			return 2 ;
		}
	}
}

#if LUKS_EXTERNAL_HEADER
static int _open_luks_1( const char * device,const open_struct_t * opts )
{
	size_t key_len ;

	string_t st ;

	struct crypt_device * cd ;
	uint32_t flags ;

	int r ;

	const char * key ;
	const char * luks_header_file ;
	const char * luks_header_file_contents ;

	size_t luks_header_file_size ;

	if( opts->key_len > 3145728 ){
		/*
		 * a 3MB structure is a structure that is simply above expectations.
		 */
		return 1 ;
	}

	/*
	 * key variable is expected to hold a structure made up of 4 components.
	 * first  component at offset 0 is a u_int32_t structure holding the size of the passphrase
	 * Second component at offset 4 is a u_int32_t structure holding the size of the contents of luks header
	 * third  component at offset 8 is the passphrase to unlock the LUKS volume.
	 * last   component is at offset that marks the end of the third component.Where this offset will be depends on the length of the passphrase
	 */
	memcpy( &key_len,opts->key,sizeof( u_int32_t ) ) ;
	key = opts->key + sizeof( u_int32_t ) + sizeof( u_int32_t ) ;

	memcpy( &luks_header_file_size,opts->key + sizeof( u_int32_t ),sizeof( u_int32_t ) ) ;
	luks_header_file_contents = opts->key + sizeof( u_int32_t ) + sizeof( u_int32_t ) + key_len  ;

	if( luks_header_file_size < 1048576 ){
		/*
		 * luks header backup can not be less than 1MB
		 */
		return 1 ;
	}
	/*
	 * zuluCryptCreateKeyFile() is defined in open_tcrypt.c
	 */
	st = zuluCryptCreateKeyFile( luks_header_file_contents,luks_header_file_size,"luks_header_file-" ) ;
	luks_header_file = StringContent( st ) ;

	if( crypt_init( &cd,luks_header_file ) != 0 ){
		return zuluExit_1( 1,cd,st ) ;
	}
	if( crypt_load( cd,NULL,NULL ) != 0 ){
		return zuluExit_1( 1,cd,st ) ;
	}
	if( crypt_set_data_device( cd,device ) != 0 ){
		return zuluExit_1( 1,cd,st ) ;
	}
	if( StringHasComponent( opts->m_opts,"ro" ) ){
		flags = CRYPT_ACTIVATE_READONLY ;
	}else{
		flags = CRYPT_ACTIVATE_ALLOW_DISCARDS ;
	}

	r = crypt_activate_by_passphrase( cd,opts->mapper_name,CRYPT_ANY_SLOT,key,key_len,flags ) ;

	if( r == 0 ){
		return zuluExit_1( 0,cd,st ) ;
	}else{
		return zuluExit_1( 1,cd,st ) ;
	}
}

int zuluCryptOpenLuks_1( const open_struct_t * opts )
{
	int lmode ;
	string_t st ;
	int fd ;
	int r ;
	if( StringPrefixMatch( opts->device,"/dev/",5 ) ){
		return _open_luks_1( opts->device,opts ) ;
	}else{
		if( StringHasComponent( opts->m_opts,"ro" ) ){
			lmode = O_RDONLY ;
		}else{
			lmode = O_RDWR ;
		}
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in ./create_loop.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( opts->device,lmode,&fd,&st ) ){
			r = _open_luks_1( StringContent( st ),opts ) ;
			StringDelete( &st ) ;
			close( fd ) ;
			return r ;
		}else{
			return 1 ;
		}
	}
}

#else
int zuluCryptOpenLuks_1( const open_struct_t * opts )
{
	if( opts ){;}
	return 1 ;
}
#endif