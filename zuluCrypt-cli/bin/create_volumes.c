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
 */

ssize_t check_partition( const char * device ) ;

int create_volumes( const struct_opts * opts  )
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

	if( is_path_valid( device ) == 1 ){
		st = 1 ;
		goto out ;
	}	
	stat( device, &xt ) ;	
	if( strncmp( device,"/dev/",5 ) != 0 && xt.st_size < 3145728 ){
		st = 9 ;
		goto out ;		
	}
	if( check_partition( device ) != -1 ){		
		st = 10 ;
		goto out ;		
	}		
	if( is_path_valid( ZULUCRYPTmkfs ) == -1 ){		
		st = 11 ;
		goto out ;		
	}	
	if( conf == -1 ){			
		printf( "\nThis operation will destroy all data in a device at: \"%s\"\n",device ) ;
		printf("Are you sure you want to proceed?\n" ) ;
		printf( "Type \"Y\" and press enter if you want to process: " ) ;
		confirm = getchar() ;
		while( getchar() != '\n' ){ ; } /* clear the keyboard buffer */
			
		if( confirm != 'Y' ){
			st = 12 ;
			goto out ;
		}
	}
	if( i == 1 ){
		if( fs == NULL || type == NULL || rng == NULL ){
			st = 4 ;
			goto out ;			
		}
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
		if( fs == NULL || type == NULL || pass == NULL || rng == NULL || keyType == NULL ){
			st = 4 ;
			goto out ;			
		}
		if( strcmp( keyType,"-p" ) == 0 ) 			
			st = create_volume( device,fs,type,pass,strlen( pass ),rng ) ;			
		else if( strcmp( keyType, "-f" ) == 0 ) {
			switch( StringGetFromFile_1( &content,pass ) ){
				case 1 : st = 8 ; goto out ; 
				case 3 : st = 6 ; goto out ;
			}
			st = create_volume( device,fs,type,StringContent( content ),StringLength( content ),rng ) ;					
			StringDelete( &content ) ;				
		}else{
			st = 2 ;			
		}
	}	
	out:	
	switch ( st ){
		case 0 : printf( "SUCCESS: volume created successfully\n" ) ;
		break  ;
		case 1 : printf( "ERROR: invalid path to a file or device\n" ) ;
		break  ;
		case 2 : printf( "ERROR: wrong option type\n" );
		break  ;
		case 3 : printf( "ERROR: could not create an encrypted volume in a file or device\n" );
		break  ;	
		case 4 : printf( "ERROR: one or more required argument(s) for this operation is missing\n" );
		break  ;
		case 5 : printf( "ERROR: wrong choice, exiting\n" );
		break  ;
		case 7 : printf( "ERROR: passphrases do not match\n" ) ;
		break  ;	
		case 8 : printf( "ERROR: invalid path to key file\n" ) ;
		break  ;
		case 9 : printf( "ERROR: container file must be bigger than 3MB\n" ) ;
		break  ;
		case 10: printf( "ERROR: creating volumes on system partitions is not allowed.\n" );
		printf( "System partitions have active entries in /etc/fstab and /etc/crypttab\n" ) ;
		break  ;
		case 11 : printf( "ERROR: %s not found \n",ZULUCRYPTmkfs ) ;
		break  ;
		case 6 : printf( "ERROR: couldnt get enought memory to hold the key file\n" ) ;
		break  ;
		case 12 : printf( "ERROR: user chose not to proceed\n" ) ;
				break  ;
		default:
			;
	}
	return st ;
}

