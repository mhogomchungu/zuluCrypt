/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

int create_luks( const char * dev,const char * pass,const char * rng )
{
	int status ;
	
	struct crypt_device *cd;
	
	struct stat st;
	
	struct crypt_params_luks1 params = {
		.hash = "sha1",
		.data_alignment = 4096,
	};	
	
	if( stat( dev,&st ) != 0 )
		return 4 ;
	
	status =  crypt_init( &cd,dev ) ;
	
	if ( status != 0 ){
		status = 1 ;
		goto out ;
	}
	
	if( strcmp( rng,"/dev/random" ) == 0 )
		crypt_set_rng_type( cd, CRYPT_RNG_RANDOM );
	else 
		crypt_set_rng_type( cd, CRYPT_RNG_URANDOM );
	
	status = crypt_format( cd,CRYPT_LUKS1,"aes","cbc-essiv:sha256",NULL,NULL,32,&params );	
	
	if ( status != 0 ){
		status = 2 ;
		goto out ;
	}
	
	status = crypt_keyslot_add_by_volume_key( cd,CRYPT_ANY_SLOT,NULL,32,pass,strlen( pass ) );
	
	if ( status < 0 )
		status = 3 ;
	else	
		status = 0 ;
	
	out:
	
	crypt_free( cd );
	
	return status ;
}

