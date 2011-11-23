
#include "includes.h"

int close_mapper( const char * mapper )
{
	int i = crypt_deactivate( NULL, mapper );
	
	if ( i != 0 ){
		/*
		 * I have on occassion seen a mapper not getting closed when running zuluCrypt-test
		 * Assuming its due to slow computers like mine and hence adding two seconds delay
		 * before attempting to redeactive when it fail the first time. 
		 */
		sleep( 2 ) ;
		
		i =crypt_deactivate( NULL, mapper );
	}
	
	if( i == 0 )
		return 0;
	else
		return 2 ;	
}


