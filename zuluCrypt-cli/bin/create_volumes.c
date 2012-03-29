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

/*
 * defined in partitions.c 
 * The function is explained where it is defined
 */
ssize_t check_partition( const char * device ) ;

static int status_msg( int st )
{
	switch ( st ){
		case 0 : printf( "SUCCESS: volume created successfully\n" ) ;					break  ;
		case 1 : printf( "ERROR: invalid path to a file or device\n" ) ;				break  ;
		case 2 : printf( "ERROR: wrong option type\n" );						break  ;
		case 3 : printf( "ERROR: could not create an encrypted volume in a file or device\n" );		break  ;	
		case 4 : printf( "ERROR: one or more required argument(s) for this operation is missing\n" );	break  ;
		case 5 : printf( "ERROR: wrong choice, exiting\n" );						break  ;
		case 6 : printf( "ERROR: couldnt get enought memory to hold the key file\n" ) ;			break  ;				
		case 7 : printf( "ERROR: passphrases do not match\n" ) ;					break  ;	
		case 8 : printf( "ERROR: invalid path to key file\n" ) ;					break  ;
		case 9 : printf( "ERROR: container file must be bigger than 3MB\n" ) ;				break  ;
		case 10: printf( "ERROR: insufficient privilege to create a volume on a system partition.\n" );
			 printf( "A system partition is a partition with an active entry in \"/etc/fstab\" " ) ;	
			 printf( "and \"/etc/crypttab.\"\nRerun the tool from root's accout to proceed.\n" ) ;
														break  ;
		case 11: printf( "ERROR: %s not found \n",ZULUCRYPTmkfs ) ;					break  ;
		case 12: printf( "ERROR: user chose not to proceed\n" ) ;					break  ;
		case 13: printf( "ERROR: insufficient privilege to search for volume path\n" ) ;		break  ;
		default: printf( "ERROR: unrecognized error with status number %d encountered\n",st );
	}
	return st ;
}
int create_volumes( const struct_opts * opts,uid_t uid  )
{
	int i                = opts->interactive_passphrase ;
	int conf             = opts->dont_ask_confirmation ;
	const char * device  = opts->device ;
	const char * fs      = opts->fs ;
	const char * type    = opts->type ;
	const char * keyType = opts->key_source ;
	const char * pass    = opts->key ;
	const char * rng     = opts->rng ;
	
	string_t pass_1 ;
	string_t pass_2 ;
	string_t content ;
	
	int st  ;
	struct stat xt ;
	char confirm ;
	
	/*
	 * This function is defined at "is_path_valid.c"
	 * It makes sure the path exists and the user has atleast reading access to the path.
	 * 
	 * The importance of the function is explained where it is defined.
	 */
	switch( is_path_valid_by_euid( device,uid ) ){
		case 1 : return status_msg( 1 ) ; break ;
		case 2 : return status_msg( 13 ); break ;		
	}
	
	stat( device, &xt ) ;	
	if( strncmp( device,"/dev/",5 ) != 0 && xt.st_size < 3145728 )
		return status_msg( 9 ) ;
	
	/*
	 * Only root user can create volumes in system partitions.
	 * System partitions are defined as partitions with active entried in "/etc/fstab" and "/etc/crypttab"
	 * 
	 * Active entries are entries not commented out.	 * 
	 */
	if( uid != 0 )
		if( check_partition( device ) != -1 )
			return status_msg( 10 ) ;
	
	/*
	 * ZULUCRYPTmkfs is defined at "../executables.h"
	 * File systems are cureated not through file systems APIs but through mkfs.xxx executables started using exec call.
	 * 	 */
	if( is_path_valid( ZULUCRYPTmkfs ) == -1 )
		return status_msg( 11 ) ;
	
	if( conf == -1 ){			
		printf( "\nThis operation will destroy all data in a device at: \"%s\"\n",device ) ;
		printf("Are you sure you want to proceed?\n" ) ;
		printf( "Type \"Y\" and press enter if you want to process: " ) ;
		confirm = getchar() ;
		while( getchar() != '\n' ){ ; } /* clear the keyboard buffer */
			
		if( confirm != 'Y' )
			return status_msg( 12 ) ;
	}
	if( i == 1 ){
		/*
		 * Make sure the user has provided all required options
		 */
		if( fs == NULL || type == NULL || rng == NULL )
			return status_msg( 4 ) ;
		
		printf( "Enter passphrase: " ) ;			
		pass_1 = get_passphrase(  );			
		printf( "\nRe enter passphrase: " ) ;			
		pass_2 = get_passphrase(  );				
		printf( "\n" ) ;			
		if(  StringCompare( pass_1,pass_2 ) != 0  ){				
			st = 7 ;
		}else{				
			st = create_volume( device,fs,type,StringContent( pass_1 ),StringLength( pass_1 ),rng ) ;
		}
		StringDelete( &pass_1 ) ;
		StringDelete( &pass_2 ) ;				
	}else{	
		/*
		 * Make sure the user has provided all required options
		 */
		if( fs == NULL || type == NULL || pass == NULL || rng == NULL || keyType == NULL )
			return status_msg( 4 ) ;
		
		/*
		 * "-p" options means a user has provided the passphrase
		 * "-f" option means a user has provided a path to where the passphrase is stored, StringGetFromFile_1 family
		 *  if functions are used to read files.One is used here to do that.		 * 
		 */
		if( strcmp( keyType,"-p" ) == 0 ) 			
			st = create_volume( device,fs,type,pass,strlen( pass ),rng ) ;			
		else if( strcmp( keyType, "-f" ) == 0 ) {
			switch( StringGetFromFile_1( &content,pass ) ){
				case 1 : return status_msg( 8 ) ; 
				case 3 : return status_msg( 6 ) ;
			}
			st = create_volume( device,fs,type,StringContent( content ),StringLength( content ),rng ) ;					
			StringDelete( &content ) ;				
		}else{
			st = 2 ;			
		}
	}
	
	return status_msg( st ) ;
}

