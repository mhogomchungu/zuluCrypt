
#include "includes.h"

int is_luks( const char * dev )
{		
	int r;
	
	struct crypt_device * cd;
	
	r = crypt_init( &cd, dev ) ;	
	
	if( r == 0 )
		r = crypt_load( cd, CRYPT_LUKS1, NULL );	
	
	crypt_free( cd );	
	
	if( r == 0 )
		return 0 ;
	else 
		return 1 ;
}

