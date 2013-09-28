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

#include <libsecret-1/libsecret/secret.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * Its a bit tricky to use stable part of libsecret API to add our secrets the way that will agree with out API that is consistent with
 * libsecret backend,kwallet backend and the internal back end.
 *
 * The biggest problem is how to get information you do not know about in the wallet.For our usecase,how to get a list of all keys
 * in the wallet.KeyID schema is used to solve this problem.
 *
 * This problem is solved by having two schemas.
 *
 * Schema "s" holds attributes of the wallet.These attributes are:
 * 1. wallet size
 * 2. key and their associated key values
 *
 * "lxqt_wallet_open" is another attributes that is used to try to write to the wallet to check if its open or not
 *
 * Schema "keyID" is used to store a list of keys in the wallet in a way that make it easy to retrieve them.When a key is added
 * in the wallet,a smallest number is seached to attach it to the volume.Seaching is necessary to reuse numbers vacated by removed keys
 */

#define BUFFER_SIZE 32

#define CAST_SCHEMA( x ) ( const SecretSchema * )x

static int _number_of_entries_in_the_wallet( const SecretSchema * s )
{
	int i ;

	char * c = secret_password_lookup_sync( s,NULL,NULL,"string","lxqt_wallet_size",NULL ) ;
	if( c == NULL ){
		return 0 ;
	}else{
		i = atoi( c ) ;
		free( c ) ;
		return i ;
	}
}

/*
 * This function is used to check if a wallet is open or not and force prompt to open it if it is not open.
 *
 * write to the wallet and the operation will succeed if the wallet is open.
 * If the wallet is not open,the operation will block while a user is prompted for a key to unlock it.
 * If the user fail to unlock it,the operation will fail.
 */
int lxqt_secret_service_wallet_is_open( const void * s )
{
	const SecretSchema * e = CAST_SCHEMA( s ) ;
	return secret_password_store_sync( e,"default",e->name,
					   "lxqt_wallet_open",NULL,NULL,"string","lxqt_wallet_open",NULL ) ;
}

char * lxqt_secret_service_get_value( const char * key,const void * s )
{
	return secret_password_lookup_sync( ( SecretSchema * )s,NULL,NULL,"string",key,NULL ) ;
}

void * lxqt_secret_service_create_schema( const char * schemaName,const char * type )
{
	SecretSchema * s = malloc( sizeof( SecretSchema ) ) ;

	memset( s,'\0',sizeof( SecretSchema ) ) ;

	s->name  = schemaName ;
	s->flags = SECRET_SCHEMA_NONE ;

	s->attributes[0].name = type ;

	if( strcmp( type,"string" ) == 0 ){
		s->attributes[0].type = SECRET_SCHEMA_ATTRIBUTE_STRING ;
	}else{
		s->attributes[0].type = SECRET_SCHEMA_ATTRIBUTE_INTEGER ;
	}

	s->attributes[1].name = "NULL" ;
	s->attributes[1].type = 0 ;

	return s ;
}

gboolean lxqt_secret_service_password_store_sync( const char * key,const char * value,const void * p,const void * q )
{
	const SecretSchema * keyValues = CAST_SCHEMA( p ) ;
	const SecretSchema * keyID     = CAST_SCHEMA( q ) ;

	int i = 0 ;
	char * c ;
	int j ;

	const char * walletLabel = keyValues->name ;
	char wallet_size[ BUFFER_SIZE ] ;

	if( !lxqt_secret_service_wallet_is_open( keyValues ) ){
		return 0 ;
	}

	c = secret_password_lookup_sync( keyValues,NULL,NULL,"string","lxqt_wallet_size",NULL ) ;

	if( c == NULL ){
		secret_password_store_sync( keyValues,"default",walletLabel,"1",NULL,NULL,"string","lxqt_wallet_size",NULL ) ;
		secret_password_store_sync( keyID,"default",walletLabel,key,NULL,NULL,"integer",0,NULL ) ;
	}else{
		j = atoi( c ) + 1 ;
		free( c ) ;

		snprintf( wallet_size,BUFFER_SIZE,"%d",j ) ;

		secret_password_store_sync( keyValues,"default",walletLabel,wallet_size,NULL,NULL,"string","lxqt_wallet_size",NULL ) ;

		while( i < j ){
			c = secret_password_lookup_sync( keyID,NULL,NULL,"integer",i,NULL ) ;
			if( c == NULL ){
				secret_password_store_sync( keyID,"default",walletLabel,key,NULL,NULL,"integer",i,NULL ) ;
				break ;
			}else{
				i++ ;
				free( c ) ;
			}
		}
	}

	return secret_password_store_sync( keyValues,"default",walletLabel,value,NULL,NULL,"string",key,NULL ) ;
}

gboolean lxqt_secret_service_clear_sync( const char * key,const void * p,const void * q )
{
	const SecretSchema * keyValues = CAST_SCHEMA( p ) ;
	const SecretSchema * keyID     = CAST_SCHEMA( q ) ;

	int i = 0 ;
	int k = 0 ;

	char * c ;
	char wallet_size[ BUFFER_SIZE ] ;
	const char * walletLabel = keyValues->name ;

	int j = _number_of_entries_in_the_wallet( keyValues ) ;

	if( !lxqt_secret_service_wallet_is_open( keyValues ) ){
		return 0 ;
	}

	while( i <= j ){
		c = secret_password_lookup_sync( keyID,NULL,NULL,"integer",k,NULL ) ;
		if( c != NULL ){
			if( strcmp( c,key ) == 0 ){
				free( c ) ;
				secret_password_clear_sync( keyID,NULL,NULL,"integer",k,NULL ) ;
				c = secret_password_lookup_sync( keyValues,NULL,NULL,"string","lxqt_wallet_size",NULL ) ;
				snprintf( wallet_size,BUFFER_SIZE,"%d",atoi( c ) - 1 ) ;
				free( c ) ;
				secret_password_store_sync( keyValues,"default",walletLabel,wallet_size,NULL,NULL,"string","lxqt_wallet_size",NULL ) ;
				break ;
			}else{
				i++ ;
				k++ ;
				free( c ) ;
			}
		}else{
			k++ ;
		}
	}

	return secret_password_clear_sync( keyValues,NULL,NULL,"string",key,NULL ) ;
}

char ** lxqt_secret_get_all_keys( const void * p,const void * q,int * count )
{
	const SecretSchema * keyValues = CAST_SCHEMA( p ) ;
	const SecretSchema * keyID     = CAST_SCHEMA( q ) ;

	int k = 0 ;
	int i = 0 ;
	int j ;

	char ** c = NULL  ;
	char * e  = NULL ;

	*count = 0 ;

	if( lxqt_secret_service_wallet_is_open( keyValues ) ){
		j = _number_of_entries_in_the_wallet( keyValues ) ;
		c = malloc( sizeof( char * ) * j ) ;
		if( c != NULL ){
			while( i < j ){
				e = secret_password_lookup_sync( keyID,NULL,NULL,"integer",k,NULL ) ;
				if( e != NULL ){
					c[ i ] =  e ;
					*count += 1 ;
					i++ ;
					k++ ;
				}else{
					k++ ;
				}
			}
		}
	}

	return c ;
}

int lxqt_secret_service_wallet_size( const void * s )
{
	return _number_of_entries_in_the_wallet( CAST_SCHEMA( s ) ) ;
}
