/*
 * copyright: 2013
 * name : mhogo mchungu
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef LXQTWALLET_H
#define LXQTWALLET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
	
typedef struct lxqt_wallet_struct * lxqt_wallet_t ;

/*
 * error values
 */
typedef enum{
	lxqt_wallet_no_error = 0,
	lxqt_wallet_wrong_password,
	lxqt_wallet_wallet_exists,
	lxqt_wallet_gcry_cipher_open_failed,
	lxqt_wallet_gcry_cipher_setkey_failed,
	lxqt_wallet_gcry_cipher_setiv_failed,
	lxqt_wallet_gcry_cipher_encrypt_failed,
	lxqt_wallet_gcry_cipher_decrypt_failed,
	lxqt_wallet_failed_to_open_file,
	lxqt_wallet_failed_to_allocate_memory,
	lxqt_wallet_invalid_argument,
	lxqt_wallet_incompatible_wallet,
	lxqt_wallet_failed_to_create_key_hash,
	lxqt_wallet_libgcrypt_version_mismatch
}lxqt_wallet_error;

/*
 * key can not be NULL,
 * a NULL key_value or a key_value_length of size 0 will be taken as an empty key_value
 * 
 * If the key is a NULL terminated string,add the NULL character as part of the key by adding one to strlen( key ) in key_size argument
 * and account for the extra character when searching for the key value or when retrieving the key through its value.
 */
lxqt_wallet_error lxqt_wallet_add_key( lxqt_wallet_t,const char * key,u_int32_t key_size,const char * key_value,u_int32_t key_value_length ) ;

/*
 * open "wallet_name" wallet of application "application_name" using a password of size password_length.
 *
 * The rest of the API except lxqt_wallet_create() are undefined if this function returns a non zero number
 */
lxqt_wallet_error lxqt_wallet_open( lxqt_wallet_t *,const char * password,u_int32_t password_length,
		      const char * wallet_name,const char * application_name ) ;

/*
 * create a new wallet named "wallet_name" owned by application "application_name" using a password "password" of size "password_length".
 */
lxqt_wallet_error lxqt_wallet_create( const char * password,u_int32_t password_length,const char * wallet_name,const char * application_name ) ;

/*
 * returns a value of a key.
 * caller of this function is responsible for the returned buffer and should free() it when done with it
 * NULL is returned if the key could not be found
 */
void lxqt_wallet_read_key_value( lxqt_wallet_t,const char * key,u_int32_t key_size,char ** value,u_int32_t * value_size ) ;

/*
 * returns 1 if a wallet has a key and 0 otherwise 
 */
int lxqt_wallet_wallet_has_key( lxqt_wallet_t,const char * key,u_int32_t key_size ) ;

/*
 * returns 1 if wallet has value and 0 otherwise.
 * if key argument is not NULL,the  key will be returned through it.
 * if key_size is not NULL,the size size will be returned through it. 
 * It is the responsibility of the caller to free() the returned key memory when done with it
 */
int lxqt_wallet_wallet_has_value( lxqt_wallet_t,char ** key,u_int32_t * key_size,const char * value,u_int32_t value_size ) ;

/*
 * give a list of all wallets that belong to a program
 * Returned value is a NULL terminated array of strings with names of program wallets.
 * size argument will contain the number of wallets managed by the program
 * The caller of this function is expected to free() the returned value.
 * The caller of this function is expected to free() all individual entries in the returned array
 */
char ** lxqt_wallet_wallet_list( const char * application_name,int * size ) ;

/*
 * delete a key.
 */
lxqt_wallet_error lxqt_wallet_delete_key( lxqt_wallet_t,const char * key,u_int32_t key_size ) ;

/*
 * delete a wallet named "wallet_name" of an application named "application_name" exists
 */
lxqt_wallet_error lxqt_wallet_delete_wallet( const char * wallet_name,const char * application_name ) ;

/*
 * close a wallet handle.
 */
lxqt_wallet_error lxqt_wallet_close( lxqt_wallet_t * ) ;

/*
 * Check if a wallet named "wallet_name" of an application named "application_name" exists
 * returns 0 if the wallet exist
 */
int lxqt_wallet_exists( const char * wallet_name,const char * application_name ) ;

/*
 * returns a path to where the wallet file is stored.
 * on return path_buffer will contain something like "/home/$USER/.local/application_name/wallets"
 */
void lxqt_wallet_application_wallet_path( char * path_buffer,u_int32_t path_buffer_size,const char * application_name ) ;

/*
 * returns number of bytes keys in the volume consume
 */
int lxqt_wallet_wallet_size( lxqt_wallet_t ) ;

/*
 * returns the number of elements in the wallet
 */
int lxqt_wallet_wallet_entry_count( lxqt_wallet_t ) ;

typedef struct{
	char * key ;
	u_int32_t key_size ;
	char * key_value ;
	u_int32_t key_value_size ;
}lxqt_wallet_key_values_t ;

/*
 * get a list of all key-values in the wallet to make it easy to iterate over them.
 * 
 * the retured structure must be free()ed by the caller.
 * the "key" and "key_value" members must be free()ed by the caller.
 * 
 * see "print" example in the documentation program below for an example of how to read all keys and their respective values
 */
lxqt_wallet_key_values_t * lxqt_wallet_read_all_key_values( lxqt_wallet_t ) ;

/*
 * get a list of all keys in the wallet.The key_value member will not be filled and should not be free()ed
 */
lxqt_wallet_key_values_t * lxqt_wallet_read_all_keys( lxqt_wallet_t ) ;

/*
 * change the wallet password
 */
lxqt_wallet_error lxqt_wallet_change_wallet_password( lxqt_wallet_t,const char * new_password,u_int32_t new_password_size ) ;

/*
 * undocumented API
 */
char * _lxqt_wallet_get_wallet_data( lxqt_wallet_t wallet ) ;

/*
 * below is a complete program that tests the library functionality,it may be useful as a
 * source of how the use the API
 */
#if 0

#include "lxqtwallet.h"
#include <string.h>

/*
 * This source file shows how the library can be used 
 */

static const char * wallet_name    = "wallet_name" ;
static const char * application_name = "application_name" ;

#define stringsAreEqual( x,y ) strcmp( x,y ) == 0

int main( int argc,char * argv[] )
{
	lxqt_wallet_t wallet ;
	lxqt_wallet_error r = lxqt_wallet_no_error ;
	
	char * e = NULL ;
	const char * f ;
	const char * z ;
	const char * command ;
	char ** p ;
	
	lxqt_wallet_key_values_t * values ;
	
	int i ;
	int k ;
	u_int32_t q ;
	
	if( argc  < 2 ){
		printf( "wrong number of arguments\n" ) ;
		return lxqt_wallet_invalid_argument ;
	}
	
	command = argv[ 1 ] ;
	
	if( stringsAreEqual( command,"create" ) ){
		/*
		 * create a new wallet
		 * additional arguments: password
		 * eg ./wallet create xxx 
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_create( f,strlen( f ),wallet_name,application_name ) ;
		}
	}else if( stringsAreEqual( command,"add" ) ){
		/*
		 * add an entry to the wallet
		 * additional arguments: password key key_value
		 * eg ./wallet add xxx alicia abc
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),wallet_name,application_name ) ;
		}
				
		if( r != lxqt_wallet_no_error ){
			if( r == lxqt_wallet_wrong_password ){
				puts( "wrong password" ) ;
			}else{
				puts( "general error has occured" ) ;
			}			
		}else{
			f = argv[ 3 ] ;
			z = argv[ 4 ] ;
			r = lxqt_wallet_add_key( wallet,f,strlen( f ) + 1,z,strlen( z )) ; 
			
			lxqt_wallet_close( &wallet ) ;
		}
	}else if( stringsAreEqual( command,"read" ) ){
		/*
		 * read an value in a wallet through its key
		 * additional arguments: password key key_value
		 * eg ./wallet read xxx alicia
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),wallet_name,application_name ) ;
		}
		
		if( r != lxqt_wallet_no_error ){
			if( r == lxqt_wallet_wrong_password ){
				puts( "wrong password" ) ;
			}else{
				puts( "general error has occured" ) ;
			}
		}else{
			f = argv[ 3 ] ;
			e = NULL ;
			lxqt_wallet_read_key_value( wallet,f,strlen( f ) + 1,&e,&q ) ;
			
			if( e ){
				printf( "key=%s:value=%s\n",f,e ) ;
				free( e ) ;
			}else{
				printf( "key=%s:value=(NULL)\n",f ) ;
			}
			
			lxqt_wallet_close( &wallet ) ;
		}
	}else if( stringsAreEqual( command,"print" ) ){
		/*
		 * print all entries in the wallet
		 * additional arguments: password
		 * eg ./wallet print xxx 
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),wallet_name,application_name ) ;
		}

		if( r == lxqt_wallet_no_error ){
			
			k = lxqt_wallet_wallet_entry_count( wallet ) ;
			values = lxqt_wallet_read_all_key_values( wallet ) ;
			if( values != NULL ){
				i = 0 ;
				while( i < k ){
					printf( "key=%s\tkey value=\"%s\"\n",values[ i ].key,values[ i ].key_value ) ;
					free( values[ i ].key ) ;
					free( values[ i ].key_value ) ;
					i++ ;
				}
				free( values ) ;
			}
			lxqt_wallet_close( &wallet ) ;
		}else{
			if( r == lxqt_wallet_wrong_password ){
				puts( "wrong password" ) ;
			}else{
				puts( "general error has occured" ) ;
			}
		}
	}else if( stringsAreEqual( command,"delete" ) ){
		/*
		 * delete a key from a wallet
		 * additional arguments: password key
		 * eg ./wallet delete xxx alicia
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),wallet_name,application_name ) ;
		}
		
		if( r == lxqt_wallet_no_error ){
			f = argv[ 3 ] ;
			r = lxqt_wallet_delete_key( wallet,f,strlen( f ) + 1 ) ;
			lxqt_wallet_close( &wallet ) ;
		}else{
			if( r == lxqt_wallet_wrong_password ){
				puts( "wrong password" ) ;
			}else{
				puts( "general error has occured" ) ;
			}
		}
	}else if( stringsAreEqual( command,"change" ) ){
		/*
		 * replace wallet key 
		 * additional arguments: old_password new_password
		 * eg ./wallet replace xxx zzz
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),wallet_name,application_name ) ;
		}
		
		if( r == lxqt_wallet_no_error ){
			f = argv[ 3 ] ;
			r = lxqt_wallet_change_wallet_password( wallet,f,strlen( f ) ) ;
			lxqt_wallet_close( &wallet ) ;
		}else{
			if( r == lxqt_wallet_wrong_password ){
				puts( "wrong password" ) ;
			}else{
				puts( "general error has occured" ) ;
			}
		}
	}else if( stringsAreEqual( command,"value" ) ){
		/*
		 * returns a key with a particular value 
		 * additional arguments: password value
		 * eg ./wallet value xxx zzz
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),wallet_name,application_name ) ;
		}
		
		if( r == lxqt_wallet_no_error ){
			f = argv[ 3 ] ;
			r = lxqt_wallet_wallet_has_value( wallet,&e,&q,f,strlen( f ) ) ;
			if( e != NULL ){
				printf( "key=\"%s\"\nkey size=%d\n",e,q ) ;
				free( e ) ;
			}else{
				printf( "key=\"\"\n" ) ;
			}
			lxqt_wallet_close( &wallet ) ;
		}else{
			if( r == lxqt_wallet_wrong_password ){
				puts( "wrong password" ) ;
			}else{
				puts( "general error has occured" ) ;
			}
		}
	}else if( stringsAreEqual( command,"list" ) ){
		/*
		 * returns a list of wallets managed by a program
		 * eg ./wallet value xxx zzz
		 */
		p = lxqt_wallet_wallet_list( application_name,&k ) ;
		if( p != NULL ){
			printf( "application's wallets are: " ) ;
			for( i = 0 ; i < k ; i++ ){
				printf( "%s,",p[ i ] ) ;
				free( p[ i ] ) ;
			}
			printf( "\n" ) ;
			free( p ) ;
		}else{
			printf( "application has no wallets\n" ) ;
		}
	}else{
		puts( "unknown option" ) ;
	}
	return r ;
}

#endif

#ifdef __cplusplus
}
#endif

#endif
