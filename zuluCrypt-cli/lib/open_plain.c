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

static inline int zuluExit( int st,struct crypt_device * cd )
{
	crypt_free( cd );
	return st ;
}

int _open_plain( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{
	int flags ;
	struct crypt_device * cd ;
	
	struct crypt_params_plain params ;
	
	memset( &params,'\0',sizeof( struct crypt_params_plain ) ) ;
	
	params.hash = "ripemd160";
	params.skip = 0;
	params.offset = 0;
	
	if( zuluCryptPathIsNotValid( device ) )
		return 3 ;
	
	if( strstr( mode,"ro" ) != NULL )
		flags = 1 ;
	else
		flags = 0 ;
	
	if( crypt_init( &cd,device ) != 0 )
		return 2 ;
	
	if( crypt_format( cd,CRYPT_PLAIN,"aes","cbc-essiv:sha256",NULL,NULL,32,&params ) != 0 )
		return zuluExit( 2,cd ) ;
	
	if( crypt_activate_by_passphrase( cd,mapper,CRYPT_ANY_SLOT,pass,pass_size,flags ) < 0 )
		return zuluExit( 2,cd ) ;
	else
		return zuluExit( 0,cd ) ;
}

int zuluCryptOpenPlain( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{
	int lmode ;
	string_t st ;
	int fd ;
	int r ;
	if( strncmp( device,"/dev/",5 ) == 0 ){
		return _open_plain( device,mapper,mode,pass,pass_size ) ;
	}else{
		if( strcmp( mode,"ro" ) == 0 ){
			lmode = O_RDONLY ;
		}else{
			lmode = O_RDWR ;
		}
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in ./create_loop.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( device,lmode,&fd,&st ) ){
			r = _open_plain( device,mapper,mode,pass,pass_size ) ;
			StringDelete( &st ) ;
			close( fd ) ;
			return r ;
		}else{
			return 2 ;
		}
	}
}

