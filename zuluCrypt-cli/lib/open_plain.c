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

typedef struct{

	const char * rng ;
	const char * algo ;
	const char * cipher ;
	const char * keySize ;
	const char * hash ;
	const char * offset ;
}args ;

static int zuluExit( int st,struct crypt_device * cd )
{
	crypt_free( cd ) ;
	return st ;
}

static const char * _remove_letters( char * e )
{
	char * m = e ;

	while( *e ){

		if( !( *e >= '0' && *e <= '9' ) ){

			*e = '\0' ;
			break ;
		}

		e++ ;
	}

	return m ;
}

static u_int64_t _offset( const char * offset )
{
	char * e ;

	u_int64_t r = 0 ;

	size_t s = StringSize( offset ) ;

	if( s == 0 ){

		return 0 ;
	}else{
		e = StringCopy_2( offset ) ;

		if( StringEndsWithAtLeastOne( e,"m","MB","mb","Mb","M",NULL ) ){

			r = StringConvertToInt( _remove_letters( e ) ) ;
			r = 2 * 1024 * r ;

		}else if( StringEndsWithAtLeastOne( e,"g","GB","gb","Gb","G",NULL ) ){

			r = StringConvertToInt( _remove_letters( e ) ) ;
			r = 2 * 1024 * 1024 * r ;

		}else if( StringEndsWithAtLeastOne( e,"t","TB","tb","Tb","T",NULL ) ){

			r = StringConvertToInt( _remove_letters( e ) ) ;
			r = (u_int64_t)( 1.0 * 2 * 1024 * 1024 * 1024 * r ) ;

		}else if( StringEndsWithAtLeastOne( e,"k","KB","kb","Kb","K",NULL ) ){

			r = StringConvertToInt( _remove_letters( e ) ) ;
			r = 2 * r ;

		}else if( StringEndsWithAtLeastOne( e,"b","B",NULL ) ){

			r = StringConvertToInt( _remove_letters( e ) ) ;

			if( r < 512 ){
				r = 0 ;
			}else{
				r = r / 512 ;
			}
		}else {
			r = StringConvertToInt( offset ) ;
		}

		StringFree( e ) ;
	}

	return r ;
}

static int _open_plain( const char * device,const resolve_path_t * opts )
{
	uint32_t flags ;

	struct crypt_device * cd ;
	struct crypt_params_plain params ;

	size_t size ;
	/*
	 * open_struct_t is defined in includes.h
	 */
	const open_struct_t * opt = opts->args ;

	const args * e = opt->variables ;

	memset( &params,'\0',sizeof( struct crypt_params_plain ) ) ;

	params.hash = e->hash ;

	if( zuluCryptPathIsNotValid( device ) ){

		return 3 ;
	}
	if( crypt_init( &cd,device ) != 0 ){

		return 2 ;
	}

	params.offset = _offset( e->offset ) ;

	if( opts->open_mode == O_RDONLY ){

		flags = CRYPT_ACTIVATE_READONLY ;
	}else{
		flags = CRYPT_ACTIVATE_ALLOW_DISCARDS ;
	}

	size = ( size_t ) StringConvertToInt( e->keySize ) / 8 ;

	if( crypt_format( cd,CRYPT_PLAIN,e->algo,e->cipher,NULL,NULL,size,&params ) != 0 ){

		return zuluExit( 2,cd ) ;
	}
	if( crypt_activate_by_passphrase( cd,opt->mapper_name,CRYPT_ANY_SLOT,
		opt->key,opt->key_len,flags ) < 0 ){

		return zuluExit( 2,cd ) ;
	}else{
		return zuluExit( 0,cd ) ;
	}
}

int zuluCryptOpenPlain_2( const char * device,const char * mapper,
			  const char * mode,const char * key,size_t key_len,
			  const char * options )
{
	open_struct_t opt ;

	stringList_t stl ;

	int r ;

	char * const * e = NULL ;
	size_t n = 0 ;

	args s = { "/dev/urandom","aes","cbc-essiv:sha256","256","ripemd160","0" } ;

	memset( &opt,'\0',sizeof( open_struct_t ) ) ;

	opt.device      = device ;
	opt.mapper_name = mapper ;
	opt.key         = key ;
	opt.key_len     = key_len ;
	opt.m_opts      = mode ;
	opt.variables   = &s ;

	stl = StringListSplit( options,'.' ) ;

	StringListStringArray_1( &e,&n,stl ) ;

	if( n == 6 ){

		s.rng     = *( e + 0 ) ;
		s.algo    = *( e + 1 ) ;
		s.cipher  = *( e + 2 ) ;
		s.keySize = *( e + 3 ) ;
		s.hash    = *( e + 4 ) ;
		s.offset  = *( e + 5 ) ;

	}else if( n == 5 ){

		s.algo    = *( e + 0 ) ;
		s.cipher  = *( e + 1 ) ;
		s.keySize = *( e + 2 ) ;
		s.hash    = *( e + 3 ) ;
		s.offset  = *( e + 4 ) ;
	}

	/*
	 * zuluCryptResolveDevicePath_0() is defined in resolve_path.c
	 */
	r = zuluCryptResolveDevicePath_0( _open_plain,&opt,2 ) ;

	StringListDelete( &stl ) ;

	StringFree( e ) ;

	return r ;
}

int zuluCryptOpenPlain( const char * device,const char * mapper,
			const char * mode,const char * key,size_t key_len )
{
	return zuluCryptOpenPlain_2( device,
				     mapper,
				     mode,
				     key,
				     key_len,
				     "/dev/urandom.aes.cbc-essiv:sha256.256.ripemd160.0" ) ;
}

int zuluCryptOpenPlain_1( const open_struct_t * opt )
{
	return zuluCryptOpenPlain_2( opt->device,
				     opt->mapper_name,
				     opt->m_opts,
				     opt->key,
				     opt->key_len,
				     opt->plain_dm_properties ) ;
}
