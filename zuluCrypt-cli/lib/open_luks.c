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
	if( opts->luks_detached_header ){
#if LUKS_EXTERNAL_HEADER
		if( crypt_init( &cd,opts->luks_detached_header ) != 0 ){

			return 1 ;
		}

		if( crypt_load( cd,NULL,NULL ) != 0 ){

			return zuluExit( 2,cd ) ;
		}

		if( crypt_set_data_device( cd,device ) != 0 ){

			return zuluExit( 1,cd ) ;
		}
#else
		return 1 ;
#endif
	}else{
		if( crypt_init( &cd,device ) != 0 ){

			return 2 ;
		}

		if( crypt_load( cd,NULL,NULL ) != 0 ){

			return zuluExit( 2,cd ) ;
		}
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
	return zuluCryptOpenLuks_2( opts ) ;
}

int zuluCryptOpenLuks_2( const open_struct_t * opts )
{
	/*
	 * zuluCryptResolveDevicePath_0() is defined in resolve_path.c
	 */
	return zuluCryptResolveDevicePath_0( _open_luks_2,opts,2 ) ;
}
