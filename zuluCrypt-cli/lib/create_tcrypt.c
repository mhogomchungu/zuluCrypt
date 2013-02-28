 
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

/*
 * this header is created at config time
 */
#include "truecrypt_support_1.h"

int zuluCryptCreateTCrypt( const char * dev,const char * pass,size_t pass_size,const char * rng )
{
#if TRUECRYPT_CREATE
	tc_api_opts api_opts ;
	int r ;
	
	if( pass_size ){;}
		
	memset( &api_opts,'\0',sizeof( api_opts ) ) ;
	
	api_opts.tc_device          = dev;
	api_opts.tc_passphrase      = pass ;
	api_opts.tc_cipher          = "AES-256-XTS";
	api_opts.tc_prf_hash        = "RIPEMD160"  ;
	api_opts.tc_no_secure_erase = 1 ;
	
	if( StringPrefixMatch( rng,"/dev/urandom",12 ) ){
		api_opts.tc_use_weak_keys = 1 ;
	}
	if( tc_api_init( 0 ) == TC_OK ){
		r = tc_api_create_volume( &api_opts );
		tc_api_uninit() ;
		return r == TC_OK ? 0 : 1 ;
	}else{
		return 1 ;
	}
#else
	if( dev ) {;}
	if( pass ){;}
	if( pass_size ){;}
	if( rng ){;}
	return 1 ;
#endif	
	
}
