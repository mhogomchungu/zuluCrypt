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
static int check_empty_slot( const char * device )
{
	int status = 0 ;
	char * c = empty_slots( device ) ;
	char * d  ;
	
	if( c == NULL )
		return 1 ;
	d = c - 1 ;
	while( *++d ){
		if( *d == '3' ){
			status = 3 ;
			break ;
		}
	}	
	free( c ) ;
	return status ;
}

int removekey( const struct_opts * opts ) 
{
	int i                    = opts->interactive_passphrase ;
	int k                    = opts->dont_ask_confirmation ;
	const char * device      = opts->device ;
	const char * keyType     = opts->key_source ;
	const char * keytoremove = opts->key ;
	
	string_t pass;
	int status = 0 ;
	
	if( check_empty_slot( device ) ){
		if( k != 1 ){
			status = 11 ;
			goto out ;		
		}		
	}
	if ( is_path_valid( device ) == 1 ){
		status = 10 ;
		goto out ;
	}	
	if ( i == 1 ){
		printf( "Enter the passphrase of the key you want to delete: " ) ;
		pass = get_passphrase() ;
		printf( "\n" ) ;	
		status = remove_key( device,StringContent( pass ),StringLength( pass ) ) ;
		StringDelete( &pass ) ;
	}else{
		if( keyType == NULL || keytoremove == NULL ){
			status = 6 ;
			goto out ;
		}
		if( strcmp( keyType, "-f" ) == 0 ){	
			switch( StringGetFromFile_1( &pass,keytoremove ) ){
				case 1 : status = 5 ; goto out ; 
				case 3 : status = 7 ; goto out ;
			}
			status = remove_key( device,StringContent( pass ),StringLength( pass ) ) ;
			StringDelete( &pass ) ;
		}else if( strcmp( keyType, "-p" ) == 0 ) {
			
			status = remove_key( device,keytoremove,strlen( keytoremove ) ) ;		
		}
	}
	out:
	switch ( status ){
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
		case 6 : printf( "ERROR: one or more required argument(s) for this operation is missing\n" ) ;
		break ;
		case 7 : printf( "ERROR: could not get enough memory to open the key file\n" ) ;
		break ;
		case 10 : printf( "ERROR: device does not exist\n" );
		break ;	
		case 11 :printf( "WARNING: there is only one key in the volume left and all data in the volume will be lost if you continue.\n" );
			 printf( "if you want to continue,rerun the command with -k option\n" ) ;
		break;
		default :
			;		
	}		
	return status ;	
}

 