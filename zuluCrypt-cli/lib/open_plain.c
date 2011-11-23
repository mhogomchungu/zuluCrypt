
#include "includes.h"

int open_plain( const char * device,const char * mapper,const char * mode,const char * source,const char * pass,const char * cipher )
{
	int status ;
	
	int flags ;
	
	const char * c ;
	
	struct crypt_device * cd;
	
	struct crypt_params_plain params = {
		.hash = "ripemd160",
		.skip = 0,
		.offset = 0,
	};
	
	struct stat st;
	
	if( stat( device,&st ) != 0 )
		return 3 ;
	
	c = strrchr( mapper,'/' );
	
	if( c == NULL )
		c = mapper ;
	else
		c++ ;
	
	if( strcmp( mode,"ro" ) == 0 )
		flags = 1 ;
	else
		flags = 0 ;
	
	status = crypt_init( &cd, device ) ;	
	
	if( status != 0 ){
		status = 2 ;
		goto out ;
	}
	
	status = crypt_format( cd,CRYPT_PLAIN,"aes",cipher,NULL,NULL,32,&params );
	
	if( status != 0 ){
		status = 2 ;
		goto out ;
	}
	
	if( strcmp( source,"-p" ) == 0 ){
		
		status = crypt_activate_by_passphrase( cd,c,CRYPT_ANY_SLOT,pass,strlen( pass ),flags );
	}else{			
		if( stat( pass,&st ) != 0 ){
			status = 4;
			goto out ;
		}		
		status = crypt_activate_by_keyfile( cd,c,CRYPT_ANY_SLOT,pass,st.st_size,flags ) ;
	}
	
	if ( status < 0 )
		status = 2 ;
	else
		status = 0 ;
	
	out:
	crypt_free( cd );
	
	return status ;
}

