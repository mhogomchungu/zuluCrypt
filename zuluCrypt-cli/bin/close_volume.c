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

static int st_msg( int st,char * m_point )
{
	switch( st ) {
		case 0 : printf( "SUCCESS: volume closed successfully \n" );
			 if( m_point != NULL ){
				remove( m_point ) ;
				free( m_point ) ;
			 }
			 break ;
		case 1 : printf( "ERROR: close failed, encrypted volume with that name does not exist\n" );      			  break ;			
		case 2 : printf( "ERROR: close failed, the mount point and/or one or more files are in use\n" );			  break ;
		case 3 : printf( "ERROR: close failed, volume does not have an entry in /etc/mtab\n" ) ;        			  break ;
		case 4 : printf( "ERROR: close failed, could not get a lock on /etc/mtab~\n" ) ;  	             			  break ;
		case 5 : printf( "ERROR: close failed, volume is unmounted but could not close mapper,advice to close it manually\n");	  break ;
		case 6 : printf( "ERROR: close failed, could not resolve full path of device\n");					  break ;					
		default: printf( "ERROR: unrecognized error with status number %d encountered\n",st );
	}	
	return st ;
}

int close_opened_volume( const char * device,const char * mapping_name,uid_t uid )
{	
	 int st ;	 
	 string_t p ;
	 char * m_point = NULL ;
	 
	 char * dev = realpath( device,NULL ) ;
	 
	 if( dev == NULL )
		 return st_msg( 6,NULL ) ;
	 /*
	  * This function is defined at "create_mapper_name.c"
	  * 
	  * Explanation for what it does is explained where it is defined.	  * 
	  */
	 p = create_mapper_name( dev,mapping_name,uid,CLOSE ) ;
	 
	 st = close_volume( StringContent( p ),&m_point ) ;	 

	 StringDelete( &p ) ;	
	 free( dev ) ;
	 return st_msg( st,m_point ) ;	
}
