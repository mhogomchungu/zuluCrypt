
#include "includes.h"

int remove_key( const char * device ,const char * pass )
{       
	int status ;
	
	struct crypt_device * cd;
	
	if( is_luks( device ) == 1 )
		return 1 ;      
	
	status = crypt_init( &cd,device ) ;
	
	if( status != 0 ){
		status =  3 ;
		goto out ;
	}
	
	status = crypt_load( cd, CRYPT_LUKS1, NULL ) ;
	
	if( status != 0 ){
		status =  3 ;
		goto out ;
	}
	
	status =  crypt_activate_by_passphrase( cd,NULL,CRYPT_ANY_SLOT,pass,strlen( pass ),0 );
	
	if ( status < 0 ){
		status = 2 ;
		goto out ;
	}
	
	status = crypt_keyslot_destroy( cd,status ) ;
	
	if ( status < 0 )
		status = 2 ;    
	else
		status = 0 ;
	
	out:
	crypt_free( cd );               
	
	return status ;
}

