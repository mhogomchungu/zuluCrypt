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

int removekey(  int argn , char * device, char * keyType, char * keytoremove  )
{
	string_t * pass;
	int status = 0 ;
	struct stat st ;	
	char * c ;
	
	if (  stat(  device,&st  ) != 0  ){
		status = 10 ;
		goto out ;
	}	
	if (  argn == 3  ){
		printf( "Enter the passphrase of the key you want to delete: " ) ;
		pass = get_passphrase(  ) ;
		printf( "\n" ) ;	
		status = remove_key(  device,StringContent( pass ) ) ;
		StringDelete( pass ) ;
	}else if (  argn == 5  ){
		if(  strcmp( keyType, "-f" ) == 0  ){			
			switch( read_file( &c,keytoremove ) ){
				case 1 : status = 5 ; goto out ; 
				case 2 : status = 7 ; goto out ; 
			}	
			status = remove_key( device,c ) ;
			free( c ) ;
		}else if(  strcmp( keyType, "-p" ) == 0  ) {
			
			status = remove_key( device,keytoremove ) ;		
		}
	}else
		status = 6 ;
	out:
	switch (  status  ){
		case 0 : printf( "SUCCESS: key removed successfully\n" );
		break ;
		case 1 : printf( "ERROR: device \"%s\" is not a luks device",device ) ;
		break ;
		case 2 : printf( "ERROR: there is no key in the volume that match the presented key\n" ) ;
		break ;
		case 3 : printf( "ERROR: could not open device\n" ) ;
		break ;  
		case 5 : printf( "ERROR: keyfile does not exist\n" ) ;
		break ;
		case 6 : printf( "ERROR: Wrong number of arguments\n" ) ;
		break ;
		case 7 : printf( "ERROR: insuffucient system memory, quiting\n" ) ;
		break ;
		case 10 : printf( "ERROR: device does not exist\n" );
		break ;		
		default :
			;		
	}		
	return status ;	
}

 