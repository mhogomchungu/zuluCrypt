/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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


