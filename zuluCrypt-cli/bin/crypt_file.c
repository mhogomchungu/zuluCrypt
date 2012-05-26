/*
 * 
 *  Copyright (c) 2012
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
#include "../zuluCrypt.h"

#define DECRYPT 1
#define ENCRYPT 0

static int msg( int st )
{
	switch( st ){
		case 0 : printf( "SUCCESS: encrypted file created successfully\n")				; break ;
		case 1 : printf( "SUCCESS: decrypted file created successfully\n" )  				; break ;
		case 2 : printf( "ERROR: could not open key file for reading\n" )  				; break ;
		case 3 : printf( "ERROR: missing key source\n")							; break ;
		case 4 : printf( "ERROR: could not open encryption routines\n")					; break ;
		case 5 : printf( "ERROR: file or folder already exist at destination address\n")		; break ; 
		case 6 : printf( "ERROR: invalid path to source\n")						; break ;
		case 7 : printf( "ERROR: could not resolve path to destination file\n")				; break ;
		case 8 : printf( "ERROR: passphrases do not match\n")						; break ;
		case 9 : printf( "ERROR: required argument is missing\n")					; break ;
		case 10: printf( "ERROR: insufficient privilege to create destination file\n")			; break ;
		case 11: printf( "ERROR: wrong passphrase\n")							; break ;
	}	
	return st ;
}

static int crypt_opt( const struct_opts * opts,const char * mapper,uid_t uid,int opt )
{
	string_t q ;
	string_t p ;

	int st ;
	
	const char * source	= opts->device ;
	const char * dest  	= opts->mode ;
	const char * passphrase = opts->key ;
	const char * type 	= opts->key_source ;
	int i 			= opts->interactive_passphrase ;
	
	if( dest == NULL )
		return msg( 9 ) ;
	
	if( source == NULL )
		return msg( 9 ) ;
	
	if( is_path_valid( dest ) == 0 )
		return msg( 5 ) ;
	
	if( can_open_path_for_writing( dest,uid ) == 1 )
		return msg( 10 ) ;		
	
	if( is_path_valid( source ) != 0 )
		return msg( 6 ) ;
	
	if( i == 1 ){
		printf( "Enter passphrase: " ) ;			
		p = get_passphrase();			
		printf( "\nRe enter passphrase: " ) ;			
		q = get_passphrase();				
		printf( "\n" ) ;			
		if( StringCompare( p,q ) != 0 ){
			StringDelete( &p ) ;
			StringDelete( &q ) ;
			return msg( 8 ) ; 
		}else{
			StringDelete( &q ) ;
		}
	}else{
		if( type == NULL )
			return msg( 9 ) ;
		if( strcmp( type,"-p" ) == 0 ){
			p = String( passphrase ) ;
		}else if( strcmp( type,"-f" ) == 0 ){		
			p = StringGetFromFile( passphrase ) ;		
			if( p == NULL )
				return msg( 2 ) ;		
		}else{
			return msg( 3 ) ;
		}
	}		
	
	if( opt == ENCRYPT )
		st = encrypt_file( source,dest,StringContent( p ),StringLength( p ) ) ;
	else
		st = decrypt_file( source,dest,StringContent( p ),StringLength( p ) ) ;	
	
	StringDelete( &p ) ;
	
	switch( st ){
		case 1 : return msg( 4 ) ;
		case 2 : return msg( 11 ) ;
	}
	
	chmod( dest,S_IRUSR | S_IWUSR ) ;
	chown( dest,uid,uid ) ;
	
	if( opt == 1 )
		return msg( 1 ) ;
	else
		return msg( 0 ) ;
}

int file_decrypt( const struct_opts * opts,const char * mapper,uid_t uid ) 
{
	return crypt_opt( opts,mapper,uid,DECRYPT ) ;
}

int file_encrypt( const struct_opts * opts,const char * mapper,uid_t uid )
{
	return crypt_opt( opts,mapper,uid,ENCRYPT ) ;		
}
