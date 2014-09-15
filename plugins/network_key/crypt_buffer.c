
/*
 * copyright: 2014
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
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "crypt_buffer.h"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gcrypt.h>
#pragma GCC diagnostic warning "-Wdeprecated-declarations"

#define KEY_LENGTH 32
#define SALT_SIZE  16
#define IV_SIZE    16
#define LOAD_INFO_SIZE  32
#define HMAC_SIZE 24
#define MAGIC_STRING "TRUE"
#define MAGIC_STRING_LENGTH 4

#define PBKDF2_ITERATIONS 5000

/*
 * This library takes a block of data and returns an encrypted version of the data.
 * This library also has the ability to reverse the above action.
 *
 * A user gives a password of specified size and the library coverts it to a 32 byte key using
 * pbkdf2 with iteration count of 5000 and a hash function of sha2.
 *
 * Data is encrypted using 256 bit AES in CBC mode.
 *
 * The format of the encrypted data:
 * 16 bytes from offset 0 stores pbkdf2 salt.
 * 16 bytes from offset 16 stores AES initialization vector.
 * 4  bytes from offset 32 stores the size of the load given by the user.
 * 4  bytes from offset 36 stores a string "TRUE" that is used to verify encryption key during decryption.
 * 24 bytes from offset 40 are used to store hmac hash value of the clear text.
 * The load the user gave to be stored encrypted starts from offset 64.The load will be padded up to a multiple of 32.
 *
 * Encrypted data starts at offset 32.
 *
 * The size of ciphertext will be ( 64 + 32n ) bytes where n is a number starting from 0.
 */

struct crypt_buffer_ctx_1
{
	char * buffer ;
	char * key ;
	size_t buffer_size ;
	size_t key_size ;
	int    fd ;
	gcry_cipher_hd_t h ;
	gcry_md_hd_t     m ;
} ;

static int _debug( crypt_buffer_ctx ctx,const char * e )
{
#if 0
	if( 0 && ctx ){;}
	puts( e ) ;
#else
	if( 0 && ctx && e ){;}
#endif
	return 0 ;
}

static int _failed( gcry_error_t r )
{
	return r != GPG_ERR_NO_ERROR ;
}

static int _passed( gcry_error_t r )
{
	return r == GPG_ERR_NO_ERROR ;
}

static int _failed_init( int fd,char * key,crypt_buffer_ctx c,gcry_cipher_hd_t h,gcry_md_hd_t m )
{
	close( fd ) ;

	free( key ) ;
	free( c ) ;

	if( h != 0 ){
		gcry_cipher_close( h ) ;
	}
	if( m != 0 ){
		gcry_md_close( m ) ;
	}

	return 0 ;
}

int crypt_buffer_init( crypt_buffer_ctx * ctx,const void * k,size_t key_size )
{
	gcry_error_t r ;

	gcry_cipher_hd_t h = 0 ;
	gcry_md_hd_t m = 0 ;

	crypt_buffer_ctx c ;

	void * key = NULL ;

	int fd = open( "/dev/urandom",O_RDONLY ) ;

	if( fd == -1 ){
		return 0 ;
	}

	if( gcry_control( GCRYCTL_INITIALIZATION_FINISHED_P ) == 0 ){
		gcry_check_version( NULL ) ;
		gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;
	}

	c = malloc( sizeof( struct crypt_buffer_ctx_1 ) ) ;
	if( c == NULL ){
		return _failed_init( fd,key,c,h,m ) ;
	}

	key = malloc( key_size ) ;
	if( key == NULL ){
		return _failed_init( fd,key,c,h,m ) ;
	}

	r = gcry_cipher_open( &h,GCRY_CIPHER_AES256,GCRY_CIPHER_MODE_CBC,0 ) ;

	if( _failed( r ) ){
		return _failed_init( fd,key,c,h,m ) ;
	}

	r = gcry_md_open( &m,GCRY_MD_SHA256,GCRY_MD_FLAG_HMAC ) ;

	if( _failed( r ) ){
		return _failed_init( fd,key,c,h,m ) ;
	}

	r = gcry_md_setkey( m,k,key_size ) ;

	if( _failed( r ) ){
		return _failed_init( fd,key,c,h,m ) ;
	}else{
		memcpy( key,k,key_size ) ;

		c->buffer      = NULL ;
		c->buffer_size = 0 ;
		c->h           = h ;
		c->m           = m ;
		c->key         = key ;
		c->key_size    = key_size ;
		c->fd          = fd ;
		*ctx           = c ;
		return 1 ;
	}
}

void crypt_buffer_uninit( crypt_buffer_ctx * ctx )
{
	crypt_buffer_ctx t ;

	if( ctx != NULL && *ctx != NULL ){
		t = *ctx ;
		*ctx = NULL ;
		gcry_cipher_close( t->h ) ;
		gcry_md_close( t->m ) ;
		close( t->fd ) ;
		free( t->key ) ;
		free( t->buffer ) ;
		free( t ) ;
	}
}

static int _get_random_data( crypt_buffer_ctx ctx,char * buffer,size_t buffer_size )
{
	ssize_t e = buffer_size ;
	ssize_t z = read( ctx->fd,buffer,buffer_size ) ;
	return z != e ;
}

static gcry_error_t _create_key( const char * salt,size_t salt_size,const char * input_key,
				 size_t input_key_length,char * output_key,size_t output_key_size )
{
	return gcry_kdf_derive( input_key,input_key_length,GCRY_KDF_PBKDF2,GCRY_MD_SHA256,
				salt,salt_size,PBKDF2_ITERATIONS,output_key_size,output_key ) ;
}

static int _set_gcrypt_handle( crypt_buffer_ctx ctx,const char * salt,size_t salt_size,const char * iv,size_t iv_size )
{
	char key[ KEY_LENGTH ] ;

	gcry_error_t r ;

	gcry_cipher_hd_t handle = ctx->h ;

	r = gcry_cipher_reset( handle ) ;

	if( _passed( r ) ){
		r = _create_key( salt,salt_size,ctx->key,ctx->key_size,key,KEY_LENGTH ) ;
		if( _passed( r ) ){
			r = gcry_cipher_setkey( handle,key,KEY_LENGTH ) ;
			if( _passed( r ) ){
				r = gcry_cipher_setiv( handle,iv,iv_size ) ;
				if( _passed( r ) ){
					return 1 ;
				}else{
					return _debug( ctx,"failed to set iv" ) ;
				}
			}else{
				return _debug( ctx,"failed to set key" ) ;
			}
		}else{
			return _debug( ctx,"failed to create key" ) ;
		}
	}else{
		return _debug( ctx,"failed to reset handle" ) ;
	}
}

static char * _expand_buffer( crypt_buffer_ctx h,size_t z )
{
	char * e = NULL ;

	if( h == NULL ){
		return 0 ;
	}

	if( h->buffer_size < z ){

		e = realloc( h->buffer,z ) ;

		if( e != NULL ){
			h->buffer = e ;
			h->buffer_size = z ;
			return e ;
		}else{
			return NULL ;
		}
	}else{
		return h->buffer ;
	}
}

static unsigned char * _create_hmac( crypt_buffer_ctx ctx,const void * buffer,u_int32_t buffer_size )
{
	gcry_md_reset( ctx->m ) ;

	gcry_md_write( ctx->m,buffer,buffer_size ) ;

	return gcry_md_read( ctx->m,0 ) ;
}

int crypt_buffer_encrypt( crypt_buffer_ctx ctx,const void * buffer,u_int32_t buffer_size,crypt_buffer_result * r )
{
	char buff[ SALT_SIZE + IV_SIZE ] ;

	gcry_error_t z ;

	size_t len ;
	size_t k = buffer_size ;

	char * e ;
	unsigned char * f ;
	const char * salt = buff ;
	const char * iv   = buff + SALT_SIZE ;

	if( _get_random_data( ctx,buff,SALT_SIZE + IV_SIZE ) ){
		return _debug( ctx,"failed to get random data" ) ;
	}

	/*
	 * make sure the block buffer we are going to encrypt is a multiple of 32
	 */
	while( k % 32 != 0 ){
		k += 1 ;
	}

	e = _expand_buffer( ctx,k + SALT_SIZE + IV_SIZE + LOAD_INFO_SIZE ) ;

	if( e == NULL ){
		return _debug( ctx,"faile to expand memory buffer" ) ;
	}

	f = _create_hmac( ctx,buffer,buffer_size ) ;

	if( f == NULL ){
		return _debug( ctx,"failed to create hmac hash" ) ;
	}

	if( _set_gcrypt_handle( ctx,salt,SALT_SIZE,iv,IV_SIZE ) ){

		len = SALT_SIZE + IV_SIZE ;
		/*
		 * 16 bytes from offset 0 contains pbkdf2 salt.
		 * 16 bytes from offset 16 contains AES initialization vector.
		 * The sum of the above makes the first 32 byte block.
		 * This block is stored in clear text.
		 */
		memcpy( e,buff,len ) ;
		/*
		 * 4 bytes from offset 32 stores the size of the clear text we are going to encrypt
		 */
		memcpy( e + len,&buffer_size,sizeof( u_int32_t ) ) ;
		/*
		 * 4 bytes at offset 36 stores "TRUE" bytes to be used to verify decryption key
		 */
		memcpy( e + len + sizeof( u_int32_t ),MAGIC_STRING,MAGIC_STRING_LENGTH ) ;
		/*
		 * 24 bytes from offset 40 contains hmac digest of the cleartext
		 *
		 * These 24 bytes plus the 4 above and the 4 above it makes the second 32 byte block.
		 * Ciphertext starts with this block.
		 */
		memcpy( e + len + sizeof( u_int32_t ) + sizeof( u_int32_t ) ,f,HMAC_SIZE ) ;
		/*
		 * User data to encrypt starts at the 64th bytes.
		 * The 64th bytes marks the end of the header and the beginning of the load.
		 */
		memcpy( e + len + LOAD_INFO_SIZE,buffer,buffer_size ) ;

		/*
		 * Encryption starts at offset 32
		 */
		z = gcry_cipher_encrypt( ctx->h,e + len,LOAD_INFO_SIZE + k,NULL,0 ) ;

		if( _passed( z ) ){
			r->buffer = ctx->buffer ;
			/*
			 * SALT_SIZE + IV_SIZE + LOAD_INFO_SIZE will equal 64.
			 * k will equal the size of the data we were asked to encrypt rounded up to a multiple of 32
			 */
			r->length = k + SALT_SIZE + IV_SIZE + LOAD_INFO_SIZE ;
			return 1 ;
		}else{
			return _debug( ctx,"failed to encrypt data" ) ;
		}
	}else{
		return _debug( ctx,"failed to create encryption handle" ) ;
	}
}

/*
 * The password is assumed to be correct if the 4 bytes from offset 36 equal "TRUE"
 */
static int _password_is_correct( const char * buffer )
{
	return memcmp( buffer + sizeof( u_int32_t ),MAGIC_STRING,MAGIC_STRING_LENGTH ) == 0 ;
}

static u_int32_t _get_data_length( const char * buffer )
{
	u_int32_t l ;
	memcpy( &l,buffer,sizeof( u_int32_t ) ) ;
	return l ;
}

static int _hmac_passed( crypt_buffer_ctx ctx,const char * e,u_int32_t buffer_size )
{
	/*
	 * calculate the hmac hash of the decrypted data.
	 */
	unsigned char * f = _create_hmac( ctx,e + LOAD_INFO_SIZE,buffer_size ) ;
	/*
	 * compare the hmac of the decrypted data against the stored hmac at offset 8 of the decrypted data.
	 * This offset 8 is actually offset 40 since the decryption skipped the first 32 bytes of the salt and IV.
	 */
	return memcmp( f,e + sizeof( u_int32_t ) + sizeof( u_int32_t ),HMAC_SIZE ) == 0 ;
}

int crypt_buffer_decrypt( crypt_buffer_ctx ctx,const void * buffer,u_int32_t buffer_size,crypt_buffer_result * r )
{
	gcry_error_t z ;

	char * e ;

	const char * buff = buffer ;
	const char * salt = buff ;
	const char * iv   = buff + SALT_SIZE ;

	size_t len = buffer_size - ( SALT_SIZE + IV_SIZE ) ;
	ssize_t l  = buffer_size - ( SALT_SIZE + IV_SIZE + LOAD_INFO_SIZE ) ;

	if( l < 0 ){
		return _debug( ctx,"inconsistent data size detected" ) ;
	}

	e = _expand_buffer( ctx,buffer_size ) ;

	if( e == NULL ){
		return _debug( ctx,"failed to expand internal buffer" ) ;
	}

	if( _set_gcrypt_handle( ctx,salt,SALT_SIZE,iv,IV_SIZE ) ){

		/*
		 * Skip to offset 32 and start decryption from there.Thats because the first 32 bytes
		 * holds salt and IV and are stored unencrypted.
		 */
		z = gcry_cipher_decrypt( ctx->h,e,len,buff + SALT_SIZE + IV_SIZE,len ) ;

		if( _passed( z ) ){

			if( _password_is_correct( e ) ){

				len = _get_data_length( e ) ;

				if( len <= buffer_size - ( SALT_SIZE + IV_SIZE + LOAD_INFO_SIZE ) ){

					if( _hmac_passed( ctx,e,len ) ){

						r->buffer = e + LOAD_INFO_SIZE ;
						r->length = len ;

						return 1 ;
					}else{
						return _debug( ctx,"hmac check failed" ) ;
					}
				}else{
					return _debug( ctx,"inconsistency detected on stored data size" ) ;
				}
			}else{
				return _debug( ctx,"password check failed" ) ;
			}
		}else{
			return _debug( ctx,"decryption routine failed 1" ) ;
		}
	}else{
		return _debug( ctx,"decryption routine failed 2" ) ;
	}
}
