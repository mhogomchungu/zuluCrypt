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

#include "lxqtwallet.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <sys/mman.h>
#include <dirent.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gcrypt.h>
#pragma GCC diagnostic warning "-Wdeprecated-declarations"

/*
 * below string MUST BE 5 bytes long
 */
#define VERSION "2.0.0"
#define VERSION_SIZE 5
/*
 * below string MUST BE 11 bytes long
 */
#define MAGIC_STRING "lxqt_wallet"
#define MAGIC_STRING_SIZE 11
#define MAGIC_STRING_BUFFER_SIZE 16
#define PASSWORD_SIZE 16
#define BLOCK_SIZE 16
#define IV_SIZE 16
#define SALT_SIZE 16

#define PBKDF2_ITERATIONS 10000

#define NODE_HEADER_SIZE ( 2 * sizeof( u_int32_t ) )

#define WALLET_EXTENSION ".lwt"

struct lxqt_wallet_struct{
	char * application_name ;
	char * wallet_name ;
	char key[ PASSWORD_SIZE ] ;
	char salt[ SALT_SIZE ] ;
	char * wallet_data ;
	u_int32_t wallet_data_size ;
	u_int32_t wallet_data_entry_count ;
	int wallet_modified ;
};

/*
 * Encrypted file documentation.
 *
 * A newly created file or an empty one takes 64 bytes.
 *
 * The first 16 bytes are used for pbkdf2 salt.
 * This salt is obtained from "/dev/urandom" and will not change when the wallet is updated.
 *
 * The second 16 bytes are used to store AES Initialization Vector.
 * The IV is initially obtained from "/dev/urandom".
 * The IV is stored unencrypted and will change on every wallet update.
 *
 * Everything from 32nd byte onward is store encrypted.
 *
 * The third 16 bytes are "magic string" bytes.
 * The first 11 bytes are used to store a known data aka "magic string" to be used to check if decryption key is correct or not.
 * The remaining 5 bytes are used to store file version number.
 *
 * The fourth 16 bytes are used to store information about the contents of the load.
 * The first 4 bytes are a u_int32_t data type and are used to store the load size
 * The second 4 bytes are a u_int32_t data type and are used to store the number of entries in the wallet.
 * The remaining 8 bytes are currently unused.
 *
 * The load starts at 64th byte.
 *
 * The file is encrypted using CBC mode of 128 bit AES and hence may be padded to a file size larger than file contents to
 * accomodate CBC mode demanding data sizes that are divisible by 16.
 *
 * Key-Pair entries are stored as singly linked list nodes in an array.
 * Interesting video on why traditional linked lists are bad: http://www.youtube.com/watch?v=YQs6IC-vgmo
 *
 * A node of a linked list has 4 properties.
 * First 4 bytes of the node are a u_int32_t data type and are used to store the size of the key.
 * Second 4 bytes of the node are a u_int32_t data type and used to stores the size of the value.
 * The 8th byte of the node will be the beginning of the key.
 * The 8th byte of the node plus the size of the key will be the beginning of the value.
 *
 * The sum of the two 4 bytes plus the length of the key plus the length of the value will
 * point to the next node in the list.
 */

static char * _wallet_full_path( char * path_buffer,u_int32_t path_buffer_size,const char * wallet_name,const char * application_name ) ;

static void _create_application_wallet_path( const char * application_name ) ;

static gcry_error_t _create_key( const char salt[ SALT_SIZE ],char output_key[ PASSWORD_SIZE ],const char * input_key,u_int32_t input_key_length ) ;

static gcry_error_t _create_temp_key( char * output_key,u_int32_t output_key_size,const char * input_key,u_int32_t input_key_length ) ;

static void _get_iv_from_wallet_header( char iv[ IV_SIZE ],int fd ) ;

static void _get_salt_from_wallet_header( char salt[ SALT_SIZE ],int fd ) ;

static void _get_magic_string_from_header( char magic_string[ MAGIC_STRING_BUFFER_SIZE ],int fd ) ;

static void _get_random_data( char * buffer,size_t buffer_size ) ;

static void _create_magic_string_header( char magic_string[ MAGIC_STRING_BUFFER_SIZE ] ) ;

static int _wallet_is_not_compatible( char version_buffer[ VERSION_SIZE + 1 ] ) ;

char * _lxqt_wallet_get_wallet_data( lxqt_wallet_t wallet )
{
	return wallet->wallet_data ;
}

static inline u_int32_t _get_first_header_component( const char * str )
{
	return *( u_int32_t * ) str ;
}

static inline u_int32_t _get_second_header_component( const char * str )
{
	const char * e = str + sizeof( u_int32_t ) ;
	return *( u_int32_t * ) e ;
}

u_int32_t lxqt_wallet_wallet_size( lxqt_wallet_t wallet )
{
	if( wallet == NULL ){
		return -1 ;
	}else{
		return wallet->wallet_data_size ;
	}
}

u_int32_t lxqt_wallet_wallet_entry_count( lxqt_wallet_t wallet )
{
	if( wallet == NULL ){
		return -1 ;
	}else{
		return wallet->wallet_data_entry_count ;
	}
}

static lxqt_wallet_error _exit_create( lxqt_wallet_error r,gcry_cipher_hd_t gcry_cipher_handle )
{
	if( gcry_cipher_handle != 0 ){
		gcry_cipher_close( gcry_cipher_handle ) ;
	}
	return r ;
}

lxqt_wallet_error lxqt_wallet_create( const char * password,u_int32_t password_length,
				      const char * wallet_name,const char * application_name )
{
	int fd ;
	char path[ PATH_MAX ] ;
	char iv[ IV_SIZE ] ;
	char magic_string[ MAGIC_STRING_BUFFER_SIZE ] ;
	char key[ PASSWORD_SIZE ] ;
	char salt[ SALT_SIZE ] ;
	char block[ BLOCK_SIZE ] ;

	gcry_error_t r ;

	gcry_cipher_hd_t gcry_cipher_handle = 0 ;

	if( password == NULL || wallet_name == NULL || application_name == NULL ){
		return _exit_create( lxqt_wallet_invalid_argument,gcry_cipher_handle ) ;
	}
	if( lxqt_wallet_exists( wallet_name,application_name ) == 0 ){
		return _exit_create( lxqt_wallet_wallet_exists,gcry_cipher_handle ) ;
	}

	gcry_check_version( NULL ) ;

	gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;

	r = gcry_cipher_open( &gcry_cipher_handle,GCRY_CIPHER_AES128,GCRY_CIPHER_MODE_CBC,0 ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _exit_create( lxqt_wallet_gcry_cipher_open_failed,gcry_cipher_handle ) ;
	}

	_get_random_data( salt,SALT_SIZE ) ;

	r = _create_key( salt,key,password,password_length ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _exit_create( lxqt_wallet_failed_to_create_key_hash,gcry_cipher_handle ) ;
	}

	r = gcry_cipher_setkey( gcry_cipher_handle,key,PASSWORD_SIZE ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _exit_create( lxqt_wallet_gcry_cipher_setkey_failed,gcry_cipher_handle ) ;
	}

	_get_random_data( iv,IV_SIZE ) ;

	r = gcry_cipher_setiv( gcry_cipher_handle,iv,IV_SIZE ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _exit_create( lxqt_wallet_gcry_cipher_setiv_failed,gcry_cipher_handle ) ;
	}

	_create_application_wallet_path( application_name ) ;

	_wallet_full_path( path,PATH_MAX,wallet_name,application_name ) ;

	fd = open( path,O_WRONLY|O_CREAT,0600 ) ;

	if( fd == -1 ){
		return _exit_create( lxqt_wallet_failed_to_open_file,gcry_cipher_handle ) ;
	}

	/*
	 * first 16 bytes are for PBKDF2 salt
	 */
	write( fd,salt,SALT_SIZE ) ;
	/*
	 * second 16 bytes are for AES IV
	 */
	write( fd,iv,IV_SIZE ) ;

	_create_magic_string_header( magic_string ) ;

	r = gcry_cipher_encrypt( gcry_cipher_handle,magic_string,MAGIC_STRING_BUFFER_SIZE,NULL,0 ) ;

	memset( block,'\0',BLOCK_SIZE ) ;
	gcry_cipher_encrypt( gcry_cipher_handle,block,BLOCK_SIZE,NULL,0 ) ;

	gcry_cipher_close( gcry_cipher_handle ) ;

	if( r != GPG_ERR_NO_ERROR ){
		close( fd ) ;
		unlink( path ) ;
		return lxqt_wallet_gcry_cipher_encrypt_failed ;
	}else{
		/*
		 * third 16 bytes are for the magic string
		 */
		write( fd,magic_string,MAGIC_STRING_BUFFER_SIZE ) ;
		/*
		 * fourth 16 bytes block
		 */
		write( fd,block,BLOCK_SIZE ) ;

		close( fd ) ;
		return lxqt_wallet_no_error ;
	}
}

lxqt_wallet_error lxqt_wallet_change_wallet_password( lxqt_wallet_t wallet,const char * new_key,u_int32_t new_key_size )
{
	char key[ PASSWORD_SIZE ] ;
	gcry_error_t r ;

	if( wallet == NULL || new_key == NULL ){
		return lxqt_wallet_invalid_argument ;
	}else{
		r = _create_key( wallet->salt,key,new_key,new_key_size ) ;
		if( r != GPG_ERR_NO_ERROR ){
			return lxqt_wallet_failed_to_create_key_hash ;
		}else{
			memcpy( wallet->key,key,PASSWORD_SIZE ) ;
			wallet->wallet_modified = 1 ;
			return lxqt_wallet_no_error ;
		}
	}
}

static lxqt_wallet_error _exit_open( lxqt_wallet_error st,
				     struct lxqt_wallet_struct * w,gcry_cipher_hd_t gcry_cipher_handle,int fd )
{
	if( gcry_cipher_handle != 0 ){
		gcry_cipher_close( gcry_cipher_handle ) ;
	}
	if( fd != -1 ){
		close( fd ) ;
	}
	if( w != NULL ){
		if( w->wallet_name != NULL ){
			free( w->wallet_name ) ;
		}
		if( w->application_name != NULL ){
			free( w->application_name ) ;
		}
		free( w ) ;
	}
	return st ;
}

lxqt_wallet_error lxqt_wallet_open( lxqt_wallet_t * wallet,const char * password,u_int32_t password_length,
		      const char * wallet_name,const char * application_name )
{
	struct stat st ;
	u_int32_t len ;

	gcry_error_t r ;
	gcry_cipher_hd_t gcry_cipher_handle = 0 ;

	char iv[ IV_SIZE ] ;
	char path[ PATH_MAX ] ;
	char magic_string[ MAGIC_STRING_BUFFER_SIZE ] ;
	char version_buffer[ VERSION_SIZE + 1 ] ;
	char load_header[ BLOCK_SIZE ] ;

	char * e ;

	int fd;

	struct lxqt_wallet_struct * w ;

	if( wallet_name == NULL || application_name == NULL || wallet == NULL ){
		return lxqt_wallet_invalid_argument ;
	}

	_wallet_full_path( path,PATH_MAX,wallet_name,application_name ) ;

	fd = open( path,O_RDONLY ) ;
	if( fd == -1 ){
		return _exit_open( lxqt_wallet_failed_to_open_file,NULL,gcry_cipher_handle,fd ) ;
	}

	gcry_check_version( NULL ) ;

	gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;

	*wallet = NULL ;

	w = malloc( sizeof( struct lxqt_wallet_struct ) ) ;

	if( w == NULL ){
		return _exit_open( lxqt_wallet_failed_to_allocate_memory,NULL,gcry_cipher_handle,fd ) ;
	}

	memset( w,'\0',sizeof( struct lxqt_wallet_struct ) ) ;

	len = strlen( wallet_name ) ;

	w->wallet_name = malloc( sizeof( char ) * ( len + 1 ) ) ;

	if( w->wallet_name == NULL ){
		return _exit_open( lxqt_wallet_failed_to_allocate_memory,w,gcry_cipher_handle,fd ) ;
	}else{
		memcpy( w->wallet_name,wallet_name,len + 1 ) ;
	}

	len = strlen( application_name ) ;

	w->application_name = malloc( sizeof( char ) * ( len + 1 ) ) ;

	if( w->application_name == NULL ){
		return _exit_open( lxqt_wallet_failed_to_allocate_memory,w,gcry_cipher_handle,fd ) ;
	}else{
		memcpy( w->application_name,application_name,len + 1 ) ;
	}

	r = gcry_cipher_open( &gcry_cipher_handle,GCRY_CIPHER_AES128,GCRY_CIPHER_MODE_CBC,0 ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _exit_open( lxqt_wallet_gcry_cipher_open_failed,w,gcry_cipher_handle,fd ) ;
	}

	_get_salt_from_wallet_header( w->salt,fd ) ;

	r = _create_key( w->salt,w->key,password,password_length ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _exit_open( lxqt_wallet_failed_to_create_key_hash,w,gcry_cipher_handle,fd ) ;
	}

	r = gcry_cipher_setkey( gcry_cipher_handle,w->key,PASSWORD_SIZE ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _exit_open( lxqt_wallet_gcry_cipher_setkey_failed,w,gcry_cipher_handle,fd ) ;
	}

	_get_iv_from_wallet_header( iv,fd ) ;

	r = gcry_cipher_setiv( gcry_cipher_handle,iv,IV_SIZE ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _exit_open( lxqt_wallet_gcry_cipher_setiv_failed,w,gcry_cipher_handle,fd ) ;
	}

	_get_magic_string_from_header( magic_string,fd ) ;

	r =  gcry_cipher_decrypt( gcry_cipher_handle,magic_string,MAGIC_STRING_BUFFER_SIZE,NULL,0 ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _exit_open( lxqt_wallet_gcry_cipher_decrypt_failed,w,gcry_cipher_handle,fd ) ;
	}

	if( memcmp( magic_string,MAGIC_STRING,MAGIC_STRING_SIZE ) == 0 ){
		/*
		 * correct password was given and wallet is opened
		 */
		memcpy( version_buffer,magic_string + MAGIC_STRING_SIZE,VERSION_SIZE ) ;
		version_buffer[ VERSION_SIZE ] = '\0' ;

		if( _wallet_is_not_compatible( version_buffer ) ){
			return _exit_open( lxqt_wallet_incompatible_wallet,w,gcry_cipher_handle,fd ) ;
		}else{
			fstat( fd,&st ) ;

			len = st.st_size - ( SALT_SIZE + IV_SIZE + MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE ) ;

			if( len <= 0 ){
				/*
				 * empty wallet
				 */
				*wallet = w ;
				return _exit_open( lxqt_wallet_no_error,NULL,gcry_cipher_handle,fd ) ;
			}else{
				lseek( fd,SALT_SIZE + IV_SIZE + MAGIC_STRING_BUFFER_SIZE,SEEK_SET ) ;
				read( fd,load_header,BLOCK_SIZE ) ;

				gcry_cipher_decrypt( gcry_cipher_handle,load_header,BLOCK_SIZE,NULL,0 ) ;

				w->wallet_data_size        = _get_first_header_component( load_header ) ;
				w->wallet_data_entry_count = _get_second_header_component( load_header ) ;

				e = malloc( len ) ;

				if( e != NULL ){
					mlock( e,len ) ;
					read( fd,e,len ) ;
					gcry_cipher_decrypt( gcry_cipher_handle,e,len,NULL,0 ) ;
					w->wallet_data = e ;
					*wallet = w ;
					return _exit_open( lxqt_wallet_no_error,NULL,gcry_cipher_handle,fd ) ;
				}else{
					return _exit_open( lxqt_wallet_failed_to_allocate_memory,w,gcry_cipher_handle,fd ) ;
				}
			}
		}
	}else{
		return _exit_open( lxqt_wallet_wrong_password,w,gcry_cipher_handle,fd ) ;
	}
}

void lxqt_wallet_read_key_value( lxqt_wallet_t wallet,const char * key,u_int32_t key_size,char ** value,u_int32_t * value_size )
{
	const char * e ;
	const char * z ;

	char * r ;

	u_int32_t k = 0 ;
	u_int32_t i = 0 ;

	u_int32_t key_len ;
	u_int32_t key_value_len ;

	if( key == NULL || wallet == NULL || value_size == NULL ){
		;
	}else{
		e = wallet->wallet_data ;
		z = e ;
		k = wallet->wallet_data_size ;

		while( i < k ){

			key_len       = _get_first_header_component( e ) ;
			key_value_len = _get_second_header_component( e ) ;

			if( key_len == key_size && memcmp( key,e + NODE_HEADER_SIZE,key_size ) == 0 ){
				r = malloc( key_value_len + 1 ) ;
				if( r != NULL ){
					memcpy( r,e + NODE_HEADER_SIZE + key_len,key_value_len ) ;
					*( r + key_value_len ) = '\0' ;
					*value = r ;
					*value_size = key_value_len ;
				}
				break ;
			}else{
				i = i + NODE_HEADER_SIZE + key_len + key_value_len ;
				e = z + i ;
			}
		}
	}
}

int lxqt_wallet_wallet_has_key( lxqt_wallet_t wallet,const char * key,u_int32_t key_size )
{
	const char * e ;
	const char * z ;

	u_int32_t k = 0 ;
	u_int32_t i = 0 ;

	u_int32_t key_len ;
	u_int32_t key_value_len ;

	if( key == NULL || wallet == NULL ){
		return 0 ;
	}else{
		e = wallet->wallet_data ;
		z = e ;
		k = wallet->wallet_data_size ;

		while( i < k ){

			key_len       = _get_first_header_component( e ) ;
			key_value_len = _get_second_header_component( e ) ;

			if( key_len == key_size && memcmp( key,e + NODE_HEADER_SIZE,key_size ) == 0 ){
				return 1 ;
			}else{
				i = i + NODE_HEADER_SIZE + key_len + key_value_len ;
				e = z + i ;
			}
		}

		return 0 ;
	}
}

int lxqt_wallet_wallet_has_value( lxqt_wallet_t wallet,char ** key,u_int32_t * key_size,const char * value,u_int32_t value_size )
{
	const char * e ;
	const char * z ;

	char * r ;

	u_int32_t k = 0 ;
	u_int32_t i = 0 ;

	u_int32_t key_len ;
	u_int32_t key_value_len ;

	if( key == NULL || wallet == NULL ){
		return 0 ;
	}else{
		e = wallet->wallet_data ;
		z = e ;
		k = wallet->wallet_data_size ;

		while( i < k ){

			key_len       = _get_first_header_component( e ) ;
			key_value_len = _get_second_header_component( e ) ;

			if( key_value_len == value_size && memcmp( value,e + NODE_HEADER_SIZE + key_len,value_size ) == 0 ){
				if( key != NULL ){
					r = malloc( key_len + 1 ) ;
					if( r != NULL ){
						r = malloc( key_len + 1 ) ;
						memcpy( r,e + NODE_HEADER_SIZE,key_len ) ;
						*( r + key_len ) = '\0' ;
						*key = r ;
					}
				}
				if( key_size != NULL ){
					*key_size = key_len ;
				}

				return 1 ;
			}else{
				i = i + NODE_HEADER_SIZE + key_len + key_value_len ;
				e = z + i ;
			}
		}

		return 0 ;
	}
}

lxqt_wallet_error lxqt_wallet_add_key( lxqt_wallet_t wallet,const char * key,u_int32_t key_size,
				       const char * value,u_int32_t key_value_length )
{
	char * e ;
	char * f ;

	u_int32_t len ;

	if( key == NULL || wallet == NULL ){
		return lxqt_wallet_invalid_argument ;
	}else{
		if( key_size == 0 ){
			return lxqt_wallet_invalid_argument ;
		}else{
			if( value == NULL || key_value_length == 0 ){
				key_value_length = 0 ;
				value = "" ;
			}

			len = NODE_HEADER_SIZE + key_size + key_value_length ;
			f = realloc( wallet->wallet_data,wallet->wallet_data_size + len ) ;

			if( f != NULL ){

				mlock( f,wallet->wallet_data_size + len ) ;
				e = f + wallet->wallet_data_size ;

				memcpy( e,&key_size,sizeof( u_int32_t ) ) ;
				memcpy( e + sizeof( u_int32_t ),&key_value_length,sizeof( u_int32_t ) ) ;
				memcpy( e + NODE_HEADER_SIZE,key,key_size ) ;
				memcpy( e + NODE_HEADER_SIZE + key_size,value,key_value_length ) ;

				wallet->wallet_data_size += len ;
				wallet->wallet_modified = 1 ;
				wallet->wallet_data = f ;
				wallet->wallet_data_entry_count++ ;

				return lxqt_wallet_no_error ;
			}else{
				return lxqt_wallet_failed_to_allocate_memory ;
			}
		}
	}
}

lxqt_wallet_key_values_t * lxqt_wallet_read_all_keys( lxqt_wallet_t wallet )
{
	const char * e ;
	const char * z ;

	u_int32_t k = 0 ;
	u_int32_t i = 0 ;
	u_int32_t q = 0 ;

	u_int32_t key_len ;
	u_int32_t key_value_len ;

	lxqt_wallet_key_values_t * entries ;

	if( wallet == NULL ){
		return NULL ;
	}else{
		entries = malloc( sizeof( lxqt_wallet_key_values_t ) * wallet->wallet_data_entry_count );
		if( entries == NULL ){
			return NULL ;
		}else{
			e = wallet->wallet_data ;
			z = e ;
			k = wallet->wallet_data_entry_count ;

			memset( entries,'\0',sizeof( lxqt_wallet_key_values_t ) * wallet->wallet_data_entry_count ) ;

			while( q < k ){

				key_len       = _get_first_header_component( e ) ;
				key_value_len = _get_second_header_component( e ) ;

				entries[ q ].key = malloc( key_len + 1 ) ;
				if( entries[ q ].key != NULL ){
					memcpy( entries[ q ].key,e + NODE_HEADER_SIZE,key_len ) ;
					entries[ q ].key[ key_len ] = '\0' ;
					entries[ q ].key_size = key_len ;
				}

				i = i + NODE_HEADER_SIZE + key_len + key_value_len ;
				e = z + i ;
				q++ ;
			}

			return entries ;
		}
	}
}

lxqt_wallet_key_values_t * lxqt_wallet_read_all_key_values( lxqt_wallet_t wallet )
{
	const char * e ;
	const char * z ;

	u_int32_t k = 0 ;
	u_int32_t i = 0 ;
	u_int32_t q = 0 ;

	u_int32_t key_len ;
	u_int32_t key_value_len ;

	lxqt_wallet_key_values_t * entries ;

	if( wallet == NULL ){
		return NULL ;
	}else{
		entries = malloc( sizeof( lxqt_wallet_key_values_t ) * wallet->wallet_data_entry_count );
		if( entries == NULL ){
			return NULL ;
		}else{
			e = wallet->wallet_data ;
			z = e ;
			k = wallet->wallet_data_entry_count ;

			while( q < k ){

				key_len       = _get_first_header_component( e ) ;
				key_value_len = _get_second_header_component( e ) ;

				entries[ q ].key = malloc( key_len + 1 ) ;

				if( entries[ q ].key != NULL ){

					memcpy( entries[ q ].key,e + NODE_HEADER_SIZE,key_len ) ;
					entries[ q ].key[ key_len ] = '\0' ;
					entries[ q ].key_size = key_len ;

					entries[ q ].key_value = malloc( key_value_len + 1 ) ;
					if( entries[ q ].key_value != NULL ){
						memcpy( entries[ q ].key_value,e + NODE_HEADER_SIZE + key_len,key_value_len ) ;
						entries[ q ].key_value[ key_value_len ] = '\0' ;
						entries[ q ].key_value_size = key_value_len ;
					}else{
						free( entries[ q ].key ) ;
						memset( &entries[ q ],'\0',sizeof( lxqt_wallet_key_values_t ) ) ;
					}
				}else{
					memset( &entries[ q ],'\0',sizeof( lxqt_wallet_key_values_t ) ) ;
				}

				i = i + NODE_HEADER_SIZE + key_len + key_value_len ;
				e = z + i ;
				q++ ;
			}

			return entries ;
		}
	}
}

lxqt_wallet_error lxqt_wallet_delete_key( lxqt_wallet_t wallet,const char * key,u_int32_t key_size )
{
	char * e ;
	char * z ;

	u_int32_t k = 0 ;
	u_int32_t i = 0 ;

	u_int32_t key_len ;
	u_int32_t key_value_len ;

	u_int32_t block_size ;

	if( key == NULL || wallet == NULL ){
		return lxqt_wallet_invalid_argument ;
	}else{
		e = wallet->wallet_data ;
		z = e ;
		k = wallet->wallet_data_size ;

		while( i < k ){

			key_len       = _get_first_header_component( e ) ;
			key_value_len = _get_second_header_component( e ) ;

			if( key_len == key_size && memcmp( key,e + NODE_HEADER_SIZE,key_size ) == 0 ){

				if( wallet->wallet_data_entry_count == 1 ){
					free( wallet->wallet_data ) ;
					wallet->wallet_data_size = 0 ;
					wallet->wallet_modified = 1 ;
					wallet->wallet_data = NULL ;
					wallet->wallet_data_entry_count = 0 ;
				}else{
					block_size = NODE_HEADER_SIZE + key_len + key_value_len ;

					memmove( e,e + block_size,wallet->wallet_data_size - ( i + block_size ) ) ;

					wallet->wallet_data_size -= block_size ;
					wallet->wallet_modified = 1 ;
					wallet->wallet_data_entry_count-- ;
				}

				break ;
			}else{
				i = i + NODE_HEADER_SIZE + key_len + key_value_len ;
				e = z + i ;
			}
		}
	}

	return lxqt_wallet_no_error ;
}

lxqt_wallet_error lxqt_wallet_delete_wallet( const char * wallet_name,const char * application_name )
{
	char path[ PATH_MAX ] ;
	_wallet_full_path( path,PATH_MAX,wallet_name,application_name ) ;
	unlink( path ) ;
	return lxqt_wallet_no_error ;
}

static lxqt_wallet_error _close_exit( lxqt_wallet_error err,lxqt_wallet_t * w,gcry_cipher_hd_t gcry_cipher_handle )
{
	lxqt_wallet_t wallet = *w ;
	*w = NULL ;

	if( gcry_cipher_handle != 0 ){
		gcry_cipher_close( gcry_cipher_handle ) ;
	}

	if( wallet->wallet_data_size > 0 ){
		memset( wallet->wallet_data,'\0',wallet->wallet_data_size ) ;
		munlock( wallet->wallet_data,wallet->wallet_data_size ) ;
		free( wallet->wallet_data ) ;
	}
	free( wallet->wallet_name ) ;
	free( wallet->application_name ) ;
	free( wallet ) ;
	return err ;
}

lxqt_wallet_error lxqt_wallet_close( lxqt_wallet_t * w )
{
	gcry_cipher_hd_t gcry_cipher_handle ;
	int fd ;
	char iv[ IV_SIZE ] ;
	char path[ PATH_MAX ] ;
	char path_1[ PATH_MAX ] ;
	char magic_string[ MAGIC_STRING_BUFFER_SIZE ] ;
	char load_header[ BLOCK_SIZE ] ;

	lxqt_wallet_t wallet ;

	u_int32_t k ;
	char * e ;

	gcry_error_t r ;

	if( w == NULL || *w == NULL ){
		return lxqt_wallet_invalid_argument ;
	}

	wallet = *w ;

	if( wallet->wallet_modified == 0 ){
		return _close_exit( lxqt_wallet_no_error,w,0 ) ;
	}

	gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;

	r = gcry_cipher_open( &gcry_cipher_handle,GCRY_CIPHER_AES128,GCRY_CIPHER_MODE_CBC,0 ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _close_exit( lxqt_wallet_gcry_cipher_open_failed,w,0 ) ;
	}

	r = gcry_cipher_setkey( gcry_cipher_handle,wallet->key,PASSWORD_SIZE ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _close_exit( lxqt_wallet_gcry_cipher_setkey_failed,w,gcry_cipher_handle ) ;
	}

	_get_random_data( iv,IV_SIZE ) ;

	r = gcry_cipher_setiv( gcry_cipher_handle,iv,IV_SIZE ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _close_exit( lxqt_wallet_gcry_cipher_setiv_failed,w,gcry_cipher_handle ) ;
	}

	_wallet_full_path( path,PATH_MAX,wallet->wallet_name,wallet->application_name ) ;

	snprintf( path_1,PATH_MAX,"%s.tmp",path ) ;

	fd = open( path_1,O_WRONLY|O_CREAT,0600 ) ;

	if( fd == -1 ){
		return _close_exit( lxqt_wallet_gcry_cipher_open_failed,w,gcry_cipher_handle ) ;
	}

	write( fd,wallet->salt,SALT_SIZE ) ;
	write( fd,iv,IV_SIZE ) ;

	_create_magic_string_header( magic_string ) ;

	r = gcry_cipher_encrypt( gcry_cipher_handle,magic_string,MAGIC_STRING_BUFFER_SIZE,NULL,0 ) ;

	if( r != GPG_ERR_NO_ERROR ){
		unlink( path_1 ) ;
		close( fd ) ;
		return _close_exit( lxqt_wallet_gcry_cipher_encrypt_failed,w,gcry_cipher_handle ) ;
	}

	write( fd,magic_string,MAGIC_STRING_BUFFER_SIZE ) ;

	memcpy( load_header,&wallet->wallet_data_size,sizeof( u_int32_t ) ) ;
	memcpy( load_header + sizeof( u_int32_t ),&wallet->wallet_data_entry_count,sizeof( u_int32_t ) ) ;

	r = gcry_cipher_encrypt( gcry_cipher_handle,load_header,BLOCK_SIZE,NULL,0 ) ;

	write( fd,load_header,BLOCK_SIZE ) ;

	k = wallet->wallet_data_size ;

	if( k == 0 ){
		r = gcry_cipher_encrypt( gcry_cipher_handle,wallet->wallet_data,k,NULL,0 ) ;
		if( r != GPG_ERR_NO_ERROR ){
			unlink( path_1 ) ;
			close( fd ) ;
			return _close_exit( lxqt_wallet_gcry_cipher_encrypt_failed,w,gcry_cipher_handle ) ;
		}else{
			write( fd,wallet->wallet_data,k ) ;
			close( fd ) ;
			rename( path_1,path ) ;
			return _close_exit( lxqt_wallet_no_error,w,gcry_cipher_handle ) ;
		}
	}else{
		while( k % 16 != 0 ){
			k++ ;
		}

		e = realloc( wallet->wallet_data,k ) ;

		if( e != NULL ){
			wallet->wallet_data = e ;
			r = gcry_cipher_encrypt( gcry_cipher_handle,wallet->wallet_data,k,NULL,0 ) ;
			if( r != GPG_ERR_NO_ERROR ){
				unlink( path_1 ) ;
				close( fd ) ;
				return _close_exit( lxqt_wallet_gcry_cipher_encrypt_failed,w,gcry_cipher_handle ) ;
			}else{
				write( fd,wallet->wallet_data,k ) ;
				close( fd ) ;
				rename( path_1,path ) ;
				return _close_exit( lxqt_wallet_no_error,w,gcry_cipher_handle ) ;
			}
		}else{
			unlink( path_1 ) ;
			return _close_exit( lxqt_wallet_failed_to_allocate_memory,w,gcry_cipher_handle ) ;
		}
	}
}

char ** lxqt_wallet_wallet_list( const char * application_name,int * size )
{
	char path[ PATH_MAX ] ;
	char ** result = NULL ;
	char ** result_1 ;
	char * e ;

	int count = 0 ;
	u_int32_t len ;
	struct dirent * entry ;
	DIR * dir ;

	if( application_name == NULL || size == NULL ){
		return NULL ;
	}

	lxqt_wallet_application_wallet_path( path,PATH_MAX,application_name ) ;

	dir = opendir( path ) ;

	if( dir == NULL ){
		return NULL ;
	}

	while( ( entry = readdir( dir ) ) != NULL ){
		if( strcmp( entry->d_name,"." ) == 0 || strcmp( entry->d_name,".." ) == 0 ){
			 continue ;
		}

		len = strlen( entry->d_name ) - strlen( WALLET_EXTENSION ) ;
		if( len > 0 ){
			result_1 = realloc( result,sizeof( char * ) * ( count + 1 ) ) ;
			if( result_1 != NULL ){
				e = malloc( len + 1 ) ;
				if( e!= NULL ){
					memcpy( e,entry->d_name,len ) ;
					*( e + len ) = '\0' ;
					result_1[ count ] = e ;
					result = result_1 ;
					count++ ;
				}
			}
		}
	}

	*size = count ;
	closedir( dir ) ;

	return result ;
}

int lxqt_wallet_exists( const char * wallet_name,const char * application_name )
{
	struct stat st ;
	char path[ PATH_MAX ] ;
	if( wallet_name == NULL || application_name == NULL ){
		return lxqt_wallet_invalid_argument ;
	}else{
		_wallet_full_path( path,PATH_MAX,wallet_name,application_name ) ;
		return stat( path,&st ) ;
	}
}

void lxqt_wallet_application_wallet_path( char * path,u_int32_t path_buffer_size,const char * application_name )
{
	struct passwd * pass = getpwuid( getuid() ) ;
	snprintf( path,path_buffer_size,"%s/.config/lxqt/wallets/%s/",pass->pw_dir,application_name ) ;
}

char * _wallet_full_path( char * path_buffer,u_int32_t path_buffer_size,const char * wallet_name,const char * application_name )
{
	char path_1[ PATH_MAX ] ;
	lxqt_wallet_application_wallet_path( path_1,PATH_MAX,application_name ) ;
	snprintf( path_buffer,path_buffer_size,"%s/%s%s",path_1,wallet_name,WALLET_EXTENSION ) ;
	return path_buffer ;
}

static void _create_application_wallet_path( const char * application_name )
{
	char path[ PATH_MAX ] ;
	char * e ;

	lxqt_wallet_application_wallet_path( path,PATH_MAX,application_name ) ;

	for( e = path + 1 ; *e != '\0' ; e++ ){
		if( *e == '/' ){
			*e = '\0' ;
			mkdir( path,0755 ) ;
			*e = '/' ;
		}
	}
}

static gcry_error_t _create_temp_key( char * output_key,u_int32_t output_key_size,const char * input_key,u_int32_t input_key_length )
{
	gcry_md_hd_t md ;
	unsigned char * digest ;
	
	gcry_error_t r = gcry_md_open( &md,GCRY_MD_SHA256,GCRY_MD_FLAG_SECURE ) ;
	
	if( r == GPG_ERR_NO_ERROR ){
		gcry_md_write( md,input_key,input_key_length ) ;
		gcry_md_final( md ) ;
		digest = gcry_md_read( md,0 ) ;
		if( digest == NULL ){
			r = !GPG_ERR_NO_ERROR ;
		}else{
			memcpy( output_key,digest,output_key_size ) ;
		}
		gcry_md_close( md ) ;
	}else{
		;
	}
	
	return r ;
}

/*
 * gcry_kdf_derive() doesnt seem to work with empty passphrases,to work around it,we create a temporary passphrases
 * based on provided passphrase and then feed the temporary key to gcry_kdf_derive()
 */
static gcry_error_t _create_key( const char salt[ SALT_SIZE ],
				 char output_key[ PASSWORD_SIZE ],const char * input_key,u_int32_t input_key_length )
{
	#define TEMP_KEY_SIZE 32
	char temp_key[ TEMP_KEY_SIZE ] ;
	gcry_error_t r = _create_temp_key( temp_key,TEMP_KEY_SIZE,input_key,input_key_length ) ;
	
	if( r == GPG_ERR_NO_ERROR){
		return gcry_kdf_derive( temp_key,TEMP_KEY_SIZE,GCRY_KDF_PBKDF2,GCRY_MD_SHA256,
				salt,SALT_SIZE,PBKDF2_ITERATIONS,PASSWORD_SIZE,output_key ) ;
	}else{
		return r ;
	}
}

static void _get_iv_from_wallet_header( char iv[ IV_SIZE ],int fd )
{
	lseek( fd,SALT_SIZE,SEEK_SET ) ;
	read( fd,iv,IV_SIZE ) ;
}

static void _get_salt_from_wallet_header( char salt[ SALT_SIZE ],int fd )
{
	lseek( fd,0,SEEK_SET ) ;
	read( fd,salt,SALT_SIZE ) ;
}

static void _get_magic_string_from_header( char magic_string[ MAGIC_STRING_BUFFER_SIZE ],int fd )
{
	lseek( fd,IV_SIZE + SALT_SIZE,SEEK_SET ) ;
	read( fd,magic_string,MAGIC_STRING_BUFFER_SIZE ) ;
}

static void _get_random_data( char * buffer,size_t buffer_size )
{
	int fd ;
	fd = open( "/dev/urandom",O_RDONLY ) ;
	if( fd != -1 ){
		read( fd,buffer,buffer_size ) ;
		close( fd ) ;
	}else{
		gcry_create_nonce( buffer,buffer_size ) ;
	}
}

static void _create_magic_string_header( char magic_string[ MAGIC_STRING_BUFFER_SIZE ] )
{
	/*
	 * write 11 bytes of magic string
	 */
	memcpy( magic_string,MAGIC_STRING,MAGIC_STRING_SIZE ) ;
	/*
	 * write version information in the remaining 5 bytes of the 16 byte buffer
	 */
	memcpy( magic_string + MAGIC_STRING_SIZE,VERSION,VERSION_SIZE ) ;
}

static int _wallet_is_not_compatible( char version_buffer[ VERSION_SIZE + 1 ] )
{
	if( version_buffer ){;}
	return 0 ;
}
