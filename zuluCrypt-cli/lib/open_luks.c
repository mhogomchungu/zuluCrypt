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
