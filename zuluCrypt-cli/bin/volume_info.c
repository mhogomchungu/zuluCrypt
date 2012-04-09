/*
 * 
 *  Copyright (c) 2011
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

int volume_info( const char * mapper,const char * device,uid_t uid )
{
	char * dev ;
	char * output ;	
	int xt ;
	
	string_t p ;

	dev = realpath( device,NULL ) ;
	if( dev == NULL ){
		printf( "ERROR: full device path could not get resolved\n" ) ;
		return 1 ;
	}
		
	/*
	 * This function is defined at "create_mapper_name.c"
	 * 
	 * Explanation for what it does is explained where it is defined.	  * 
	 */
	p = create_mapper_name( dev,mapper,uid,CLOSE ) ;
	
	output = status( StringContent( p ) ) ;
	
	if( output != NULL ){
		printf( "%s\n",output );
		free( output ) ;
		xt = 0 ;
	}else{
		printf( "ERROR: could not get volume info,is the volume opened?\n" ) ;
		xt = 2 ;
	}
	StringDelete( &p );
	free( dev ) ;
	return xt ;
}

