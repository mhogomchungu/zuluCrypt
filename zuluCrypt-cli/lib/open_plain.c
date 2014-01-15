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

static inline int zuluExit( int st,struct crypt_device * cd )
{
	crypt_free( cd ) ;
	return st ;
}

static u_int64_t _offset( const char * offset )
{
	char * e ;
	size_t s = StringSize( offset ) ;
	char f = *( offset + s - 1 ) ;
	u_int64_t r = 0 ;

	if( !( f >= '0' && f <= '9' ) ){
		/*
		 * The argument ends with a non digit number,assume the argument is not in offsets but
		 * in bytes and convert it to offsets
		 */
		e = StringCopy_2( offset ) ;
		*( e + s - 1 ) = '\0' ;
		r = StringConvertToInt( e ) ;
		if( f == 'b' || f == 'B' ) {
			if( r < 512 ){
				r = 0 ;
			}else{
				r = r / 512 ;
			}
		}else if( f == 'k' || f == 'K' ){
			r = 2 * r ;
		}else if( f == 'm' || f == 'M' ){
			r = 2 * 1024 * r ;
		}else if( f == 'g' || f == 'G' ){
			r = 2 * 1024 * 1024 * r ;
		}else if( f == 't' || f == 'T' ){
			r = 1.0 * 2 * 1024 * 1024 * 1024 * r ;
		}else{
			r = 0 ;
		}
		StringFree( e ) ;
	}else{
		r = StringConvertToInt( offset ) ;
	}

	return r ;
}

static int _open_plain( const char * device,const char * offset,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{
	uint32_t flags ;

	struct crypt_device * cd ;
	struct crypt_params_plain params ;

	memset( &params,'\0',sizeof( struct crypt_params_plain ) ) ;

	params.hash = "ripemd160" ;

	if( zuluCryptPathIsNotValid( device ) ){
		return 3 ;
	}
	if( offset != NULL ){
		params.offset = _offset( offset ) ;
	}
	if( StringHasComponent( mode,"ro" ) ){
		flags = CRYPT_ACTIVATE_READONLY ;
	}else{
		flags = CRYPT_ACTIVATE_ALLOW_DISCARDS ;
	}
	if( crypt_init( &cd,device ) != 0 ){
		return 2 ;
	}
	if( crypt_format( cd,CRYPT_PLAIN,"aes","cbc-essiv:sha256",NULL,NULL,32,&params ) != 0 ){
		return zuluExit( 2,cd ) ;
	}
	if( crypt_activate_by_passphrase( cd,mapper,CRYPT_ANY_SLOT,pass,pass_size,flags ) < 0 ){
		return zuluExit( 2,cd ) ;
	}else{
		return zuluExit( 0,cd ) ;
	}
}

int zuluCryptOpenPlain_1( const char * device,const char * offset,const char * mapper,
			  const char * mode,const char * pass,size_t pass_size,int x,int y )
{
	int lmode ;
	string_t st ;
	int fd ;
	int r ;

	if( x && y ){;}

	if( StringPrefixMatch( device,"/dev/",5 ) ){
		return _open_plain( device,offset,mapper,mode,pass,pass_size ) ;
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
			r = _open_plain( device,offset,mapper,mode,pass,pass_size ) ;
			StringDelete( &st ) ;
			close( fd ) ;
			return r ;
		}else{
			return 2 ;
		}
	}
}

int zuluCryptOpenPlain( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{
	return zuluCryptOpenPlain_1( device,NULL,mapper,mode,pass,pass_size,0,0 ) ;
}
