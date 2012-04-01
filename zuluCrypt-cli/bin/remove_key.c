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

static int status_msg( int st )
{
	switch ( st ){
		case 0 : printf( "SUCCESS: key removed successfully\n" );										break ;
		case 2 : printf( "ERROR: there is no key in the volume that match the presented key\n" ) ;						break ;
		case 3 : printf( "ERROR: could not open device\n" ) ;											break ;  
		case 5 : printf( "ERROR: keyfile does not exist\n" ) ;											break ;
		case 6 : printf( "ERROR: one or more required argument(s) for this operation is missing\n" ) ;						break ;
		case 7 : printf( "ERROR: could not get enough memory to open the key file\n" ) ;							break ;
		case 10: printf( "ERROR: device does not exist\n" );											break ;	
		case 11: printf( "WARNING: there is only one key in the volume left and all data in the volume will be lost if you continue.\n" );
			 printf( "if you want to continue,rerun the command with -k option\n" ) ;							break;
		case 12: printf( "ERROR: insufficient privilege to open volume for writing\n" ) ;							break ;
		case 13: printf( "ERROR: insufficient privilege to open key file for reading\n" );							break ;	
		default :printf( "ERROR: unrecognized error with status number %d encountered\n",st );
	}		
	return st ;
}

static int status_msg_1( int st,const char * device )
{
	printf( "ERROR: device \"%s\" is not a luks device",device ) ;
	return st ;
}

int removekey( const struct_opts * opts,uid_t uid ) 
{
	int i                    = opts->interactive_passphrase ;
	int k                    = opts->dont_ask_confirmation ;
	const char * device      = opts->device ;
	const char * keyType     = opts->key_source ;
	const char * keytoremove = opts->key ;
	
	string_t pass;
	int status = 0 ;
	
	/*
	 * This function is defined at "is_path_valid.c"
	 * It makes sure the path exists and the user has atleast reading access to the path.
	 * 
	 * The importance of the function is explained where it is defined.
	 */
	switch( can_open_path_for_writing( device,uid ) ){
		case 2 : return status_msg( 10 ); break ;
		case 1 : return status_msg( 12 ); break ;		
	}
	
	if( check_empty_slot( device ) )
		if( k != 1 )
			return status_msg( 11 ) ;
	
	if ( i == 1 ){
		printf( "Enter the passphrase of the key you want to delete: " ) ;
		pass = get_passphrase() ;
		printf( "\n" ) ;	
		status = remove_key( device,StringContent( pass ),StringLength( pass ) ) ;
		StringDelete( &pass ) ;
	}else{
		if( keyType == NULL || keytoremove == NULL )
			return status_msg( 6 ) ;
		
		if( strcmp( keyType, "-f" ) == 0 ){	
			/*
			 * function is defined at security.c"
			 */
			switch( get_pass_from_file( keytoremove,uid,&pass ) ){
				case 1 : return status_msg( 5 )  ; 
				case 2 : return status_msg( 7 )  ;
				case 4 : return status_msg( 13 ) ;				
			}
			status = remove_key( device,StringContent( pass ),StringLength( pass ) ) ;
			StringDelete( &pass ) ;
		}else if( strcmp( keyType, "-p" ) == 0 ) {
			
			status = remove_key( device,keytoremove,strlen( keytoremove ) ) ;		
		}
	}
	
	if( status == 1 )
		return status_msg_1( status,device ) ;
	else
		return status_msg( status ) ; 
}

 