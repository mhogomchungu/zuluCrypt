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

static int zuluExit( int st,struct crypt_device * cd )
{
	crypt_free( cd );
	return st ;
}

static int _remove_key( const char * device ,const char * pass,size_t pass_size )
{
	int slot ;
	struct crypt_device * cd;
	
	if( zuluCryptVolumeIsNotLuks( device ) ){
		return 1 ;      
	}
	if( crypt_init( &cd,device ) != 0 ){
		return 3 ;
	}
	if( crypt_load( cd,NULL,NULL ) != 0 ){
		return zuluExit( 3,cd ) ;
	}
	
	slot = crypt_activate_by_passphrase( cd,NULL,CRYPT_ANY_SLOT,pass,pass_size,0 );
	
	if ( slot < 0 ){
		return zuluExit( 2,cd ) ;
	}
	if( crypt_keyslot_destroy( cd,slot ) < 0 ){
		return zuluExit( 2,cd ) ;
	}else{
		return zuluExit( 0,cd ) ;
	}
}

int zuluCryptRemoveKey( const char * device ,const char * pass,size_t pass_size )
{       
	string_t st ;
	int fd ;
	int r ;
	
	if( StringPrefixMatch( device,"/dev/",5 ) ){
		return _remove_key( device,pass,pass_size ) ;
	}else{
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in ./create_loop.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( device,O_RDWR,&fd,&st ) ){
			r = _remove_key( StringContent( st ),pass,pass_size ) ;
			StringDelete( &st ) ;
			close( fd ) ;
			return r ;
		}else{
			return 2 ;
		}
	}
}
