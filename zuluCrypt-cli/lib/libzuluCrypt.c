
#include "includes.h"

char * version(void)
{
	return VERSION_STRING ;
}

/*
 * If output is of size N, size must be at most N-1,null is stored in the last slot
 */
void execute( const char * command,char * output,int size )
{		
	FILE *f ;
	
	int i,c  ;
	
	f = popen( command, "r" ) ;
	
	if ( output != NULL  ){
		for ( i = 0 ; i < size ; i++ ){
			if (  ( c = getc(f) ) != EOF ) 
				output[i] = ( char ) c ;
			else{
				break ;
			}			
		}
		output[i] = '\0' ;
	}	
	pclose( f );
}

int close_volume( const char * map ) 
{		
	int i = unmount_volume( map ) ;
	
	if( i != 0 )
		return i ;	
	
	close_mapper( map ) ;		
	
	return 0 ;
}


