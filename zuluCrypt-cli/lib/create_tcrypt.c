 
 /*
  * 
  *  Copyright (c) 2013
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
#include <dlfcn.h>

/*
 * this header is created at config time
 */
#include "build_tcplay.h"

int zuluCryptCreateTCrypt( const char * dev,const char * pass,size_t pass_size,const char * rng )
{
#ifdef FOUND_TCPLAY	
	
	tc_api_opts api_opts ;
	int r ;
	
	if( pass_size ){;}
	if( rng ){;}
		
	memset( &api_opts,'\0',sizeof( api_opts ) ) ;
	
	api_opts.tc_device = ( char * ) dev;
	api_opts.tc_passphrase = ( char * )pass ;
	api_opts.tc_cipher = "AES-256-XTS";
	api_opts.tc_cipher_hidden = "SERPENT-256-XTS";
	api_opts.tc_prf_hash = "whirlpool";
	api_opts.tc_prf_hash_hidden = "RIPEMD160";
	
	tc_api_init( 0 ) ;
	r = tc_api_create_volume( &api_opts );
	tc_api_uninit() ;
		
	return r ;
#else
	if( dev ) {;}
	if( pass ){;}
	if( pass_size ){;}
	if( rng ){;}
	return 1 ;
#endif	
	
}
