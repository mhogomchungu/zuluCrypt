 
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
 * tcplay data structure
 * 
 */
typedef struct tc_api_opts {
	/* Common fields */
	char		*tc_device;
	char		*tc_passphrase;
	const char	**tc_keyfiles;
	
	/* Fields for mapping */
	char		*tc_map_name;
	int		tc_password_retries;
	int		tc_interactive_prompt;
	unsigned long	tc_prompt_timeout;
	
	/* Fields for creation */
	char		*tc_cipher;
	char		*tc_prf_hash;
	char		*tc_cipher_hidden;
	char		*tc_prf_hash_hidden;
	size_t		tc_size_hidden_in_bytes;
	char		*tc_passphrase_hidden;
	const char	**tc_keyfiles_hidden;
} tc_api_opts;

static inline int zuluExit( int st,void * handle )
{
	dlclose( handle ) ;
	return st ;
}
int zuluCryptCreateTCrypt( const char * dev,const char * pass,size_t pass_size,const char * rng )
{
	struct stat st ;
	tc_api_opts api_opts ;
	void * handle ;
	int r ;
	
	int (*tc_init)( int ) ;
	int (*tc_uninit)( void ) ;
	int (*tc_create) ( tc_api_opts * ) ;
	
	if( pass_size ){;}
	if( rng ){;}
	
	if( stat( ZULUCRYPTtcrypt,&st ) != 0 ){
		return 1 ;
	}
	
	handle = dlopen( ZULUCRYPTtcrypt,RTLD_LAZY ) ;
	
	if( handle == NULL ){
		return 1 ;
	}
	
	tc_init   = dlsym( handle,"tc_api_init" ) ;
	if( tc_init == NULL ){
		puts( "tc_api_init failed" ) ;
		return zuluExit( 1,handle ) ; 
	}
	tc_uninit = dlsym( handle,"tc_api_uninit" ) ;
	if( tc_uninit == NULL ){
		puts( "tc_api_uninit failed " ) ;
		return zuluExit( 1,handle ) ;
	}
	tc_create = dlsym( handle,"tc_api_create_volume" ) ;
	if( tc_create == NULL ){
		puts( "tc_api_create_volume failed" ) ;
		return zuluExit( 1,handle ) ;
	}
	
	memset( &api_opts,'\0',sizeof( api_opts ) ) ;
	
	api_opts.tc_device = ( char * ) dev;
	api_opts.tc_passphrase = ( char * )pass ;
	api_opts.tc_keyfiles = NULL;
	api_opts.tc_keyfiles_hidden = NULL;
	api_opts.tc_size_hidden_in_bytes = 0 ;
	api_opts.tc_passphrase_hidden = NULL ;
	api_opts.tc_cipher = "AES-256-XTS";
	api_opts.tc_cipher_hidden = "SERPENT-256-XTS";
	api_opts.tc_prf_hash = "whirlpool";
	api_opts.tc_prf_hash_hidden = "RIPEMD160";
	
	tc_init( 0 ) ;
	r = tc_create( &api_opts );
	tc_uninit() ;
	
	return zuluExit( r,handle ) ;
}
