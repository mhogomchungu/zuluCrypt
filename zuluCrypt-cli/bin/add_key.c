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
		if( *d == '0' ){
			status = 2 ;
			break ;
		}
	}	
	free( c ) ;
	return status ;
}

int addkey( const struct_opts * opts )
{
	int i                    = opts->interactive_passphrase ;
	const char * device      = opts->device ;
	const char * keyType1    = opts->existing_key_source ;
	const char * existingKey = opts->existing_key ;
	const char * keyType2    = opts->new_key_source ;
	const char * newKey      = opts->new_key ;
	
	string_t presentKey ;
	string_t newKey_1 ;
	string_t newKey_2 ;
	string_t ek = NULL ;
	string_t nk = NULL ;
	
	const char * key1 = NULL ;
	const char * key2 = NULL ;
	
	size_t len1 = 0 ;
	size_t len2 = 0 ;
	int status = 0 ;

	if( is_path_valid( device ) == 1 ){		
		status = 4 ;
		goto out ;
	}
	
	if( is_luks( device ) == 1 ){
		status = 3 ;
		goto out ;
	}
	
	switch( check_empty_slot( device ) ){
		case 0 : status = 10 ; goto out ;
		case 1 : status = 2  ; goto out ;
	}
	
	if ( i == 1 ){		
		printf( "Enter an existing passphrase: " ) ;		
		presentKey = get_passphrase( ) ;		
		printf( "\n" ) ;		
		printf( "Enter the new passphrase: " ) ;		
		newKey_1 = get_passphrase( ) ;		
		printf( "\n" ) ;		
		printf( "Re enter the new passphrase: " ) ;		
		newKey_2 = get_passphrase( ) ;		
		printf( "\n" ) ;
		
		if( StringCompare( newKey_1 , newKey_2 ) != 0 )			
			status = 7 ;
		else{	
			key1 = StringContent( presentKey ) ;
			len1 = StringLength( presentKey ) ;
			key2 = StringContent( newKey_1 ) ;
			len2 = StringLength( newKey_1 ) ;
			status = add_key( device,key1,len1,key2,len2 );			
		}
		StringDelete( &presentKey ) ;			
		StringDelete( &newKey_1 ) ;	
		StringDelete( &newKey_2 ) ;
		
	}else{		
		if( keyType1 == NULL || keyType2 == NULL || newKey == NULL || existingKey == NULL ){
			status = 6 ;
			goto out ;
		}
		if ( strcmp( keyType1, "-f" ) == 0 ){	
			switch( StringGetFromFile_1( &ek,existingKey ) ){
				case 1 : status = 8 ; goto out ; 
				case 3 : status = 9 ; goto out ;
			}
			key1 = StringContent( ek ) ;
			len1 = StringLength( ek ) ;
		}		
		if ( strcmp( keyType2, "-f" ) == 0 ){	
			switch( StringGetFromFile_1( &nk,newKey ) ){
				case 1 : status = 8 ; goto out ; 
				case 3 : status = 9 ; goto out ;
			}
			key2 = StringContent( nk ) ;
			len2 = StringLength( nk ) ;
		}		
		if ( strcmp( keyType1,"-f" ) == 0 && strcmp( keyType2,"-f" ) == 0 ){
			status = add_key( device,key1,len1,key2,len2 ) ;			
			StringDelete( &nk ) ;
			StringDelete( &ek ) ;
		}else if ( strcmp( keyType1,"-p" ) == 0 && strcmp( keyType2,"-p" ) == 0 ){
			key1 = existingKey ;
			len1 = strlen( existingKey ) ;
			key2 = newKey ;
			len2 = strlen( newKey ) ;
			status = add_key( device,key1,len1,key2,len2 ) ;			
		}else if ( strcmp( keyType1,"-p" ) == 0 && strcmp( keyType2,"-f" ) == 0 ){
			key1 = existingKey ;
			len1 = strlen( existingKey ) ;
			status = add_key( device,key1,len1,key2,len2 ) ;			
			StringDelete( &nk ) ;
		}else if ( strcmp( keyType1,"-f" ) == 0 && strcmp( keyType2,"-p" ) == 0 ){			
			key2 = newKey ;
			len2 = strlen( newKey ) ;
			status = add_key( device,key1,len1,key2,len2 ) ;
			StringDelete( &ek ) ;
		}else{			
			status = 5 ;
		}
	}	
	out:	
	switch ( status ){
		case 0 : printf( "SUCCESS: key added successfully\n" );
		break ;		
		case 1 : printf( "ERROR: presented key does not match any key in the volume\n" ) ;
		break ;
		case 2 : printf( "ERROR: could not open luks device, quiting\n" ) ;
		break ;
		case 3 : printf( "ERROR: device \"%s\" is not a luks device\n",device ) ;
		break ;
		case 4 : printf( "ERROR: device does not exist\n" ) ;
		break ;
		case 5 : printf( "ERROR: wrong arguments\n" ) ;
		break ;
		case 6 : printf( "ERROR: one or more required argument(s) for this operation is missing\n" ) ;
		break ;			
		case 7 : printf( "ERROR: new passphrases do not match\n" ) ;
		break ;
		case 8 : printf( "ERROR: one or both keyfile(s) does not exist\n" ) ;
		break ;  
		case 9 : printf( "ERROR: couldnt get enought memory to hold the key file\n" ) ;
		break ;
		case 10 : printf( "ERROR: all key slots are occupied, can not add any more keys\n" ) ;
		break ;
		default :
			;		
	}
	return status ;
}
