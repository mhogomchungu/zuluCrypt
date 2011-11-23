
#include "includes.h"

/*
 * #;\"',\\`:!*?&$@(){}[]><|%~^ \n
 * 
 * The above line is made up of most if not all bash special characters.
 * 
 * The above line of text appears in a bunch of places for the following reasons.
 * 
 *1. popen uses a shell and those characters in the above line triggers bash intelligence and hence
 *   all of their occurances are preceded by a backslash to escape them. All user inputs that goes to popen
 *   are escaped to stop bash from going crazy when an inputs with interesting character combination is used.
 * 
 *2. cryptsetup seem to create two mappers when any of the characters are used in the mapper address. The bogus
 *   mapper has the character in its name and the one used by crypsetup has a '_' as a substitute for them and this confuses
 *   zuluCrypt-gui. For consistency reason, all of them are substituted by '_' before they are passed to cryptsetup
 *   so that it can create only one mapper.
 */

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


