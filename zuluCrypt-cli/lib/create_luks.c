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

static int free_crypt( int st,struct crypt_device * cd )
{
	crypt_free( cd );
	return st ;
}

int create_luks( const char * dev,const char * pass,size_t pass_size,const char * rng )
{
	struct crypt_device * cd;
	
	struct crypt_params_luks1 params = {
		.hash = "sha1",
		.data_alignment = 4096,
	};
		
	if( is_path_valid( dev ) != 0 )
		return 4 ;
	
	if( crypt_init( &cd,dev ) != 0 )
		return 1 ;
	
	if( strcmp( rng,"/dev/random" ) == 0 )
		crypt_set_rng_type( cd,CRYPT_RNG_RANDOM );
	else 
		crypt_set_rng_type( cd,CRYPT_RNG_URANDOM );
	
	if( crypt_format( cd,CRYPT_LUKS1,"aes","cbc-essiv:sha256",NULL,NULL,32,&params ) != 0 )
		return free_crypt( 2,cd ) ;
	
	if( crypt_keyslot_add_by_volume_key( cd,CRYPT_ANY_SLOT,NULL,32,pass,pass_size ) < 0 )
		return free_crypt( 3,cd ) ;
	else
		return free_crypt( 0,cd ) ;
}

