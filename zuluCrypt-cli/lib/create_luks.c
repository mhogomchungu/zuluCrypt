
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

