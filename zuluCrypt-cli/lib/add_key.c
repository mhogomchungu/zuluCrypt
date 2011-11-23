
#include "includes.h"

int add_key( const char * device,const char * existingkey,const char * newkey )
{
	int status ;
	
	struct crypt_device * cd;
	
	if( is_luks( device ) == 1 ){		
		status =  3 ;
		goto out ;
	}
		
	status = crypt_init( &cd,device )  ;
	
	if( status != 0 ){		
		status =  2 ;
		goto out ;
	}
	
	status = crypt_load( cd, CRYPT_LUKS1, NULL ) ;
	
	if( status != 0 ){		
		status =  2 ;
		goto out ;
	}
	
	status = crypt_keyslot_add_by_passphrase( cd,CRYPT_ANY_SLOT,existingkey,strlen( existingkey ),newkey,strlen( newkey ) ) ;
						   
	if ( status < 0 )
		status =  1 ;
	else
		status = 0 ;	
	
	out:
	
	crypt_free( cd );
	
	return status ;
}
