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
#include "../lib/includes.h"
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libintl.h>
#include <locale.h>

#define DECRYPT 1
#define ENCRYPT 0

static int zuluExit( int st )
{
	switch( st ){
		case 0 : printf( gettext( "SUCCESS: encrypted file created successfully\n" ) )				; break ;
		case 1 : printf( gettext( "SUCCESS: decrypted file created successfully\n" ) ) 				; break ;
		case 2 : printf( gettext( "ERROR: could not open key file for reading\n" ) )   				; break ;
		case 3 : printf( gettext( "ERROR: missing key source\n" ) )						; break ;
		case 4 : printf( gettext( "ERROR: could not open encryption routines\n" ) )				; break ;
		case 5 : printf( gettext( "ERROR: file or folder already exist at destination address\n" ) )		; break ;
		case 6 : printf( gettext( "ERROR: invalid path to source\n" ) )						; break ;
		case 7 : printf( gettext( "ERROR: could not resolve path to destination file\n" ) )			; break ;
		case 8 : printf( gettext( "ERROR: passphrases do not match\n" )	)					; break ;
		case 9 : printf( gettext( "ERROR: destination path is missing\n" ) )					; break ;
		case 10: printf( gettext( "ERROR: insufficient privilege to create destination file\n" ) )		; break ;
		case 11: printf( gettext( "ERROR: presented key did not match the encryption key\n" ) )			; break ;
		case 12: printf( gettext( "ERROR: can not get passphrase in silent mode\n"  ) )				; break ;
		case 13: printf( gettext( "ERROR: insufficient memory to hold passphrase\n" ) )				; break ;
		case 14: printf( gettext( "ERROR: source path is missing\n" ) )						; break ;
		case 15: printf( gettext( "ERROR: insufficient privilege to open source file for reading\n" ) )		; break ;
		case 16: printf( gettext( "INFORMATION: functionality currently disabled\n" ) ) ;			; break ;
	}
	return st ;
}

static int crypt_opt( const struct_opts * opts,uid_t uid,int opt )
{
	string_t q = StringVoid ;
	string_t p = StringVoid ;

	int st ;

	const char * source	= opts->device ;
	const char * dest  	= opts->m_opts ;
	const char * passphrase = opts->key ;
	const char * type 	= opts->key_source ;

	return zuluExit( 16 ) ;

	if( dest == NULL ){
		return zuluExit( 9 ) ;
	}
	if( source == NULL ){
		return zuluExit( 14 ) ;
	}
	/*
	 * zuluCryptPathStartsWith() is defined in real_path.c
	 */
	if( zuluCryptPathStartsWith( dest,"/dev/" ) ){
		return zuluExit( 10 ) ;
	}
	if( zuluCryptPathStartsWith( source,"/dev/" ) ){
		return zuluExit( 15 ) ;
	}
	/*
	 * zuluCryptPathIsValid() is defined in ../lib/is_path_valid.c
	 */
	if( zuluCryptPathIsValid( dest ) ){
		return zuluExit( 5 ) ;
	}
	/*
	 * zuluCryptPathIsNotValid() is defined in ../lib/is_path_valid.c
	 */
	if( zuluCryptPathIsNotValid( source ) ){
		return zuluExit( 6 ) ;
	}

	/*
	 * below two functions are defined in path_access.c
	 */
	if( zuluCryptCanOpenPathForWriting( dest,uid ) == 1 ){
		return zuluExit( 10 ) ;
	}
	if( zuluCryptCanOpenPathForReading( source,uid ) == 1 ){
		return zuluExit( 15 ) ;
	}
	if( type == NULL ){

		printf( gettext( "Enter passphrase: " ) ) ;
		/*
		 * ZULUCRYPT_KEY_MAX_SIZE is set in ../constants.h
		 */
		switch( StringSilentlyGetFromTerminal_1( &p,ZULUCRYPT_KEY_MAX_SIZE ) ){
			case 1 : return zuluExit( 12 ) ;
			case 2 : return zuluExit( 13 ) ;
		}

		printf( gettext( "\nRe enter passphrase: " ) ) ;
		switch( StringSilentlyGetFromTerminal_1( &q,ZULUCRYPT_KEY_MAX_SIZE ) ){
			case 1 : StringClearDelete( &p ) ;
				 return zuluExit( 12 ) ;
			case 2 : StringClearDelete( &p ) ;
				 return zuluExit( 13 ) ;
		}

		printf( "\n" ) ;

		if( !StringEqualString( p,q ) ){
			StringClearDelete( &p ) ;
			StringClearDelete( &q ) ;
			return zuluExit( 8 ) ;
		}else{
			StringDelete( &q ) ;
		}
	}else{
		if( type == NULL ){
			return zuluExit( 9 ) ;
		}
		if( StringsAreEqual( type,"-p" ) ){
			p = String( passphrase ) ;
		}else if( StringsAreEqual( type,"-f" ) ){
			p = StringGetFromFile( passphrase ) ;
			if( p == NULL ){
				return zuluExit( 2 ) ;
			}
		}else{
			return zuluExit( 3 ) ;
		}
	}

	if( opt == ENCRYPT ){
		/*
		 * zuluCryptEncryptFile() is defined in ./crypt_file.c
		 */
		st = zuluCryptEncryptFile( source,dest,StringContent( p ),StringLength( p ) ) ;
	}else{
		/*
		 * zuluCryptDecryptFile() is defined in ./crypt_file.c
		 */
		st = zuluCryptDecryptFile( source,dest,StringContent( p ),StringLength( p ) ) ;
	}

	StringClearDelete( &p ) ;

	switch( st ){
		case 1 : return zuluExit( 4 ) ;
		case 2 : return zuluExit( 11 ) ;
	}

	chmod( dest,S_IRUSR | S_IWUSR ) ;
	chown( dest,uid,uid ) ;

	if( opt == 1 ){
		return zuluExit( 1 ) ;
	}else{
		return zuluExit( 0 ) ;
	}
}

int zuluCryptExeFileDecrypt( const struct_opts * opts,uid_t uid )
{
	return crypt_opt( opts,uid,DECRYPT ) ;
}

int zuluCryptExeFileEncrypt( const struct_opts * opts,uid_t uid )
{
	 return crypt_opt( opts,uid,ENCRYPT ) ;
}
