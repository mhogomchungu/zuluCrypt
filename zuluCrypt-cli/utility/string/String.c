
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

#include "String.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <termios.h>
#include <sys/mman.h>

/*
 * a string memory block gows by below factor when it expands
 */
#define FACTOR 1.5

/*
 * when an empty string is initialized,star it with below buffer size
 */
#define STRING_INIT_SIZE 32

struct StringType
{	/*
	 *the size of the string
	 */	
	size_t size ;
	/*
	 * the size of the string buffer
	 */
	size_t length ;
	/*
	 * pointer to the string
	 */
	char * string ; 
	
	/*
	 * contains info if the string is owned by stringlist
	 */
	int owned ;
};

static void ( *_fcn_ )( void )  = NULL ;

void StringExitOnMemoryExaustion( void ( *f )( void ) )
{
	_fcn_ = f ;
}

static string_t _StringError( void )
{
	if( _fcn_ != NULL ){
		( *_fcn_ )() ;
	}
	return StringVoid ;
}

int StringOwned( string_t st )
{
	if( st == StringVoid ){
		return 0 ;
	}else{
		return st->owned ;
	}
}

static inline char * __StringExpandMemory( string_t st,size_t new_size )
{
	char * p ;
	if( new_size >= st->length ) {
		st->length = new_size * FACTOR ; 
		p = ( char * ) realloc( st->string,st->length ) ;
		if( p == NULL ){
			return ( char * ) _StringError() ;
		}else{
			return p ;
		}
	}else{
		return st->string ;
	}
}

int StringLock( string_t st )
{
	if( st != StringVoid ){
		return mlock( ( const void * )st->string,st->size ) ;
	}else{
		return  -1 ;
	}
}

int StringUnlock( string_t st )
{
	if( st != StringVoid ){
		return munlock( ( const void * )st->string,st->size ) ;
	}else{
		return  -1 ;
	}
}

void StringDelete( string_t * st )
{
	string_t s ;
	if( st != NULL ){
		s = *st ;
		if( s != StringVoid ){
			if( s->owned == 0 ){
				*st = StringVoid ;
				free( s->string ) ;
				free( s ) ;
			}
		}
	}
}

void StringClearDelete( string_t * st ) 
{	
	string_t s ;
	char * e ;
	if( st != NULL ){
		s = *st ;
		if( s != StringVoid ){
			if( s->owned == 0 ){
				*st = StringVoid ;
				e = s->string ;
				memset( e,'\0',s->length ) ;
				free( e ) ;
				free( s ) ;
			}
		}
	}
}

void StringMultipleDelete( string_t * xt,... )
{
	string_t * entry ;
	va_list list ;
	string_t st ;
	
	if( xt == NULL ){
		return ;
	}
	st = *xt ;
	if( st != StringVoid ){ 
		if( st->owned == 0 ){
			free( st->string ) ;
			free( st ) ;
			*xt = StringVoid ;
		}
	}
	
	va_start( list,xt ) ;
	
	while( 1 ){
		entry = va_arg( list,string_t * ) ;
		if( entry == ENDDELETE ){
			break ;
		}
		st = *entry ;
		if( st == StringVoid ){
			continue ;
		}
		if( st->owned == 0 ){
			free( st->string ) ;
			free( st ) ;
			*entry = StringVoid ;
		}
	}
	
	va_end( list ) ;
}

char * StringDeleteHandle( string_t * xt )
{
	char * c = NULL ;
	string_t st ;
	if( xt == NULL ){
		return NULL ;
	}
	st = *xt ;
	if( st == StringVoid ){
		return NULL ;
	}
	if( st->owned == 0 ){
		*xt = StringVoid ;
		c = st->string ;
		free( st ) ;
	}else{
		c = ( char * ) malloc( sizeof( char ) * ( st->size + 1 ) ) ;
		if( c == NULL ){
			_StringError() ;
			return NULL ;
		}else{
			memcpy( c,st->string,st->size + 1 ) ;
		}
	}
	
	return c ;
}

string_t StringCopy( string_t st )
{	
	string_t xt ;
	char * c ;
	
	if( st == StringVoid ){
		return StringVoid ;
	}
	
	c = ( char * ) malloc( sizeof( char ) * ( st->size + 1 ) ) ;
	
	if( c == NULL ){
		return _StringError() ;
	}
	
	xt = ( string_t ) malloc ( sizeof( struct StringType ) ) ;
	
	if( xt == NULL ){
		free( c ) ;
		return _StringError() ;
	}
	
	memcpy( c,st->string,st->size + 1 ) ;
	
	xt->size = st->size ;
	xt->length = st->size + 1 ;
	xt->string = c ;
	xt->owned = 0 ;
	
	return xt ;
}

string_t String( const char * cstring )
{
	size_t size ;
	string_t st ;
	
	if( cstring == NULL ){
		return StringVoid ;
	}
	
	size = strlen( cstring ) ;
	
	st = ( string_t ) malloc ( sizeof( struct StringType ) ) ;
	
	if( st == NULL ){
		return _StringError() ;
	}
	if( size < STRING_INIT_SIZE / 2 ){
		
		st->string = ( char * ) malloc( sizeof( char ) * STRING_INIT_SIZE ) ;
		if( st->string == NULL ){
			return _StringError() ;
		}
		
		memcpy( st->string,cstring,size + 1 ) ;
		st->size = size ;
		st->length = STRING_INIT_SIZE ;
	}else{
		st->string = ( char * ) malloc( sizeof( char ) * ( size + 1 ) );
	
		if( st->string == NULL ){
			free( st ) ;
			return StringVoid ;
		}
		
		memcpy( st->string,cstring,size + 1 ) ;
		st->size = size  ;
		st->length = size + 1 ;
	}
	st->owned = 0 ;
	return st ;
}

void StringReadToBuffer( string_t st,char * buffer,size_t size )
{
	if( buffer != NULL ){
		memcpy( buffer,st->string,size ) ;
	}
}

string_t StringInherit( char ** data )
{	
	size_t l ;
	if( data != NULL ){
		if( *data != NULL ){
			l = strlen( *data ) ;
			return StringInheritWithSize( data,l,l + 1 ) ;
		}else{
			return StringVoid ;
		}
	}else{
		return StringVoid ;
	}
}

void StringPrint( string_t st )
{
	if( st != StringVoid ){
		printf( "%s",st->string ) ;
	}
}

void StringPrintLine( string_t st ) 
{
	if( st != StringVoid ){
		printf( "%s\n",st->string ) ;
	}
}

int StringContains( string_t st,const char * str )
{
	if( st == StringVoid || str == NULL ){
		return 0 ;
	}else{
		return strstr( st->string,str ) != NULL ;
	}
}

string_t StringInheritWithSize( char ** data,size_t size,size_t length )
{
	string_t st ;
	
	if( data == NULL || *data == NULL ){
		return StringVoid ;
	}
	if( length == 0 ){
		return StringVoid ;
	}
	
	st = ( string_t ) malloc ( sizeof( struct StringType ) ) ;
	
	if( st == NULL ){
		return _StringError() ;
	}
	st->owned = 0 ;
	st->size = size ;
	st->length = length ;
	st->string = *data ;
	*data = NULL ;
	return st ;
}

string_t StringWithSize( const char * s,size_t len )
{
	char * c ;
	if( s == NULL ){
		return StringVoid ;
	}
	c = ( char * ) malloc( sizeof( char ) * ( len + 1 ) ) ;
	if( c == NULL ){
		return _StringError() ;
	}
	memcpy( c,s,len ) ;
	*( c + len ) = '\0' ;
	return StringInheritWithSize( &c,len,len + 1 ) ;
}

ssize_t StringIndexOfString( string_t st,size_t p,const char * s )
{
	char * c ;
	
	if( st == StringVoid || s == NULL ){
		return -1 ;
	}
	if( p >= st->size ){
		return -1 ;
	}
	
	c = strstr( st->string + p,s ) ;
	
	if( c == NULL ){
		return -1 ;
	}else{
		return c - st->string ;
	}
}

ssize_t StringLastIndexOfChar( string_t st,char s ) 
{
	char * c ;
	char * d ;
	
	if( st == StringVoid ){
		return -1 ;
	}
	
	d = st->string ;
	c = d + st->size ;
	
	while( --c != d ){
		if( *c == s ){
			return c - d ;
		}
	}
		
	return -1 ;
}

ssize_t StringLastIndexOfString( string_t st,const char * s ) 
{
	ssize_t p = -1 ;
	
	char * c ;
	char * d ;
	char * e ;

	size_t len ;
	
	if( s == NULL ){
		return -1 ;
	}
	
	if( st == StringVoid ){
		return -1 ;
	}
	
	len = strlen( s ) ;
	
	if( len == 0 ){
		return -1 ;
	}
	
	e = d = st->string ;

	while( 1 )
	{
		c = strstr( d,s ) ;
		if( c != NULL ){
			p = c - e ;
			d = d + len ;
		}else{
			break ;
		}
	}
	return p ;
}

ssize_t StringIndexOfChar( string_t st,size_t p,char s ) 
{	
	char * c ;
	char d[ 2 ] ;
	d[ 1 ] = '\0' ;
	d[ 0 ] = s ;

	if( st == StringVoid ){
		return -1 ;
	}
	
	c = strstr( st->string + p,d ) ;
	
	if( c == NULL ){
		return -1 ;
	}else{
		return c - st->string ;
	}
}

const char * StringRemoveLength( string_t st,size_t x ,size_t y ) 
{	
	if( st == StringVoid ){
		return NULL ;
	}
	if( x >= st->size ){
		return st->string ;
	}
	if( x + y >= st->size ){
		y = st->size - x ;
	}
	
	memmove( st->string + x,st->string + x + y,st->size - y - x + 1 ) ;
	
	st->size -= y ;
	
	return st->string ;
}

void StringClear( string_t st )
{
	if( st != StringVoid ){
		memset( st->string,'\0',st->size ) ;
		st->size = 0 ;
	}
}

void StringReset( string_t st )
{
	if( st != StringVoid ){
		st->string[ 0 ] = '\0' ;
		st->size = 0 ;
	}
}

const char * StringRemoveRight( string_t st,size_t x ) 
{
	if( x >= st->size ){
		st->string[ 0 ] = '\0' ;
		st->size = 0 ;
	}else{
		st->size = st->size - x ;
		*( st->string + st->size ) = '\0' ;
	}
	
	return st->string ;
}

const char * StringRemoveLeft( string_t st,size_t x ) 
{
	return StringRemoveLength( st,0,x ) ;
}

const char * StringCrop( string_t st,size_t x,size_t y ) 
{
	ssize_t s ;
	if( st == StringVoid ){
		return NULL ;
	}
	if( x >= st->size ){
		x = st->size - 1 ;
	}
	if( y >= st->size ){
		y = st->size - 1 ;
	}
	
	memmove( st->string,st->string + x,st->size - x + 1 ) ;
	
	s = st->size - x - y ;
	if( s < 0  ){
		st->size = 0 ;
	}else{
		st->size = s ;
	}
	*( st->string + st->size ) = '\0';
	
	return st->string ;
}

ssize_t StringLength( string_t st )
{	
	if( st == StringVoid ){
		return -1 ;
	}else{
		return st->size ;
	}
}

const char * StringContent( string_t st )
{
	if( st == StringVoid ){
		return NULL ;
	}else{
		return st->string ;
	}
}

const char ** StringPointer( string_t st ) 
{
	if( st == StringVoid ){
		return NULL ;
	}else{
		return ( const char ** )&st->string ;
	}
}

char * StringCopy_1( string_t st )
{
	if( st == StringVoid ){
		return NULL ;
	}else{
		return StringCopy_3( st,st->size ) ;
	}
}

char * StringCopy_2( const char * str )
{
	char * c ;
	size_t len ;
	if( str == NULL ){
		c = ( char * )malloc( sizeof( char ) ) ;
		if( c == NULL ){
			return ( char * ) _StringError() ;
		}else{
			c[ 0 ] = '\0' ;
			return c ;
		}
	}else{
		len = strlen( str ) ;
		c = ( char * )malloc( sizeof( char ) * ( len + 1 ) ) ;
		if( c == NULL ){
			return ( char * ) _StringError() ;
		}else{
			memcpy( c,str,len + 1 ) ;
			return c ;
		}
	}
}

char * StringCopy_3( string_t st,size_t l )
{
	char * c ;
	if( st == StringVoid ){
		c = ( char * )malloc( sizeof( char ) ) ;
		if( c == NULL ){
			return ( char * ) _StringError() ;
		}else{
			c[ 0 ] = '\0' ;
			return c ;
		}
	}else{
		c = ( char * )malloc( sizeof( char ) * ( l + 1 ) ) ;
		if( c == NULL ){
			return ( char * ) _StringError() ;
		}else{
			memcpy( c,st->string,l ) ;
			*( c + l ) = '\0' ;
			return c ;
		}
	}
}

int StringEndsWith( string_t st,const char * s ) 
{
	size_t j ;
		
	if( st == StringVoid || s == NULL ){
		return 0 ;
	}else{
		j = strlen( s ) ;
		return strncmp( st->string + st->size - j,s,j ) == 0 ;
	}
}

int StringStartsWith( string_t st,const char * s ) 
{
	if( st == StringVoid || s == NULL ){
		return 0 ;
	}else{
		return strncmp( st->string,s,strlen( s ) ) == 0 ;
	}
}

int StringEndsWithChar( string_t st,char s )
{	
	if( st == StringVoid ){
		return 0 ;
	}else{
		return st->string[ st->size - 1 ] == s ;
	}
}

char StringCharAt( string_t st,size_t p )
{
	if( st == StringVoid ){
		return '\0' ;
	}
	if( p >= st->size ){
		return '\0' ;
	}
	return * ( st->string + p )  ;
}

char StringCharAtLast( string_t st )
{
	if( st == StringVoid ){
		return '\0' ;
	}
	if( st->size == 0 ){
		return '\0' ;
	}
	return * ( st->string + st->size - 1 )  ;
}

const char * StringStringAt( string_t st,size_t p )
{	
	if( st == StringVoid ){
		return NULL ;
	}
	if( p >= st->size ){
		return NULL ;
	}
	return st->string + p ;
}

const char * StringSubChar( string_t st,size_t x,char s )
{		
	if( st == StringVoid ){
		return NULL ;
	}
	if( x >= st->size ){
		return st->string ;
	}
	st->string[ x ] = s ;
	return st->string ;
}

static void Stringsrcs__( string_t st,char x,const char * y,size_t p )
{
	size_t i ;
	size_t j ;
	size_t k ;
	size_t l ;
	char * c ;
	
	if( st == StringVoid ){
		return  ;
	}
	if( y == NULL ){
		return ;
	}
	if( p >= st->size ){
		return ;
	}
	
	c= st->string ;
	l = st->size ;
	k = strlen( y ) ;
	
	for( j = p ; j < l ; j++ ){
		for( i = 0 ; i < k ; i++ ){
			if( * ( c + j ) == * ( y + i ) ){
				* ( c + j ) = x ;
				break ;
			}
		}
	}
}

const char * StringReplaceCharStringPos( string_t st,char x,const char * y,size_t p ) 
{
	Stringsrcs__( st,x,y,p ) ;
	return st->string ;
}

const char * StringReplaceCharString( string_t st,char x,const char * y ) 
{
	return StringReplaceCharStringPos( st,x,y,0 )  ;
}

const char * StringSubString( string_t st, size_t x,const char * s ) 
{
	size_t k ;
	if( st == StringVoid ){
		return NULL ;
	}
	if( x >= st->size || s == NULL ){
		return st->string ;
	}
	k = strlen( s ) ;
	if( x + k >= st->size ){
		return NULL ;
	}
	memcpy( st->string + x,s,k );
	return st->string ;
}

const char * StringInsertChar( string_t st,size_t x,char s ) 
{
	char c[2] ;
	c[0] = s ;
	c[1] = '\0' ;
	return StringInsertString( st,x,c ) ;
}

const char * StringAppendAt( string_t st,size_t x,const char * s ) 
{
	size_t len ;
	char * c   ; 
	
	if( st == StringVoid ){
		return NULL ;
	}
	if( x > st->size || s == NULL ){
		return st->string ;
	}
	
	len = strlen( s ) ;
	c = __StringExpandMemory( st,st->size + len ) ;
	
	if( c != NULL ){
		memcpy( c + x,s,len + 1 ) ;
		st->string = c ;
		st->size = x + len ;
	}
	
	return c ;
}

const char * StringPrepend( string_t st,const char * s )
{
	char * c ;
	size_t len ;
	
	if( st == StringVoid ){
		return NULL ;
	}
	if( s == NULL ){
		return st->string ;
	}
	
	len = strlen( s ) ;
	
	c = __StringExpandMemory( st,st->size + len ) ;
	
	if( c != NULL ){
		st->string = c ;
		memmove( st->string + len,st->string,st->size + 1 ) ;
		memcpy( st->string,s,len ) ;
		st->size += len ;
	}
	return c ;
}

const char * StringPrependString( string_t st,string_t xt ) 
{
	return StringPrepend( st,xt->string ) ;
}

const char * StringPrependChar( string_t st,char c )
{
	char s[ 2 ] ;
	s[ 1 ] = '\0' ;
	s[ 0 ] = c ;
	return StringPrepend( st,s ) ;
}

const char * StringAppend( string_t st,const char * s ) 
{
	char * c ;
	size_t len ;
	
	if( st == StringVoid ){
		return NULL ;
	}
	if( s == NULL ){
		return st->string ;
	}
	
	len = strlen( s ) ;

	c = __StringExpandMemory( st,st->size + len ) ;
	
	if( c != NULL ){
		st->string = c ;
		memcpy( st->string + st->size,s,len + 1 ) ;
		st->size += len ;
	}
	return c ;
}

int StringAtLeastOneStartsWith( string_t st,... )
{
	int r = 0 ;
	const char * entry ;
	va_list list ;
	va_start( list,st ) ;
	
	while( 1 ){
		entry = va_arg( list,const char * ) ;
		if( entry == NULL ){
			break ;
		}else{
			if( strncmp( st->string,entry,strlen( entry ) ) == 0 ){
				r = 1 ;
				break ;
			}
		}
	}
	
	va_end( list ) ;
	return r ;
}

const char * StringMultipleAppend( string_t st,... ) 
{
	const char * entry ;
	va_list list ;
	va_start( list,st ) ;
	
	while( 1 ){
		entry = va_arg( list,const char * ) ;
		if( entry == END ){
			break ;
		}
		StringAppend( st,entry ) ;
	}
	
	va_end( list ) ;
	return st->string ;
}

const char * StringAppendMultipleString( string_t st,... ) 
{
	string_t entry ;
	va_list list ;
	va_start( list,st ) ;
	
	while( 1 ){
		entry = va_arg( list,string_t ) ;
		if( entry == NULL ){
			break ;
		}
		StringAppend( st,entry->string ) ;
	}
	
	va_end( list ) ;
	return st->string ;
}

const char * StringMultiplePrepend( string_t st,... )  
{
	const char * entry ;
	va_list list ;
	va_start( list,st ) ;
	
	while( 1 ){
		entry = va_arg( list,const char * ) ;
		if( entry == END ){
			break ;
		}
		StringPrepend( st,entry ) ;
	}
	
	va_end( list ) ;
	return st->string ;
}

const char * StringPrependMultipleString( string_t st,... )  
{
	string_t entry ;
	va_list list ;
	va_start( list,st ) ;
	
	while( 1 ){
		entry = va_arg( list,string_t ) ;
		if( entry == NULL ){
			break ;
		}
		StringPrepend( st,entry->string ) ;
	}
	
	va_end( list ) ;
	return st->string ;
}

const char * StringAppendString( string_t st,string_t xt )  
{
	return StringAppend( st,xt->string ) ;
}

const char * StringAppendChar( string_t st,char c )
{
	char * d = __StringExpandMemory( st,st->size + 1 ) ;
	
	if( d != NULL ){
		st->string[ st->size ] = c ;
		st->string[ st->size + 1 ] = '\0' ;
		st->size += 1 ;
	}
	
	return d ;
}

const char * StringInsertString( string_t st,size_t x,const char * s )
{
	char * c ;
	size_t len ;
	
	if( s == NULL ){
		return NULL ;
	}
	
	len = strlen( s ) ;
	
	c = __StringExpandMemory( st,len ) ;
	
	if( c != NULL ){
		st->string = c ;
		memmove( st->string + len + x,st->string + x,st->size - x + 1 ) ;
		memcpy( st->string + x,s,len ) ;
		st->size += len ;
	}
	return c ;
}

string_t StringMidString( string_t st,size_t x,size_t y ) 
{
	char * c ;
	
	c = ( char * ) malloc ( sizeof( char ) * ( y + 1 ) ) ;
	if( c == NULL ){
		return _StringError() ;
	}
	strncpy( c,st->string + x, y ) ;
	
	*( c + y ) = '\0' ;

	return StringInheritWithSize( &c,y,y + 1 ) ;
}

static char * StringRS__( string_t st,const char * x,const char * s,size_t p )
{
	char * c ;
	char * d ;
	char * e ;

	size_t j  ;
	size_t k  ;
	size_t len ;
	
	if( st == StringVoid ){
		return NULL ;
	}
		
	if( x == NULL || s == NULL || p >= st->size ){
		return st->string ;
	}
	
	d = st->string ;
	e = st->string + p ;
	
	j = strlen( s ) ;
	k = strlen( x ) ;
	
	if( j == k ){
		while( ( c = strstr( e,x ) ) != NULL ){
			memcpy( c,s,j ) ;
			e = e + j ;
		}
	}else if( j > k  ){
		while( ( c = strstr( e,x ) ) != NULL ){
			len = c - st->string ;
			d = __StringExpandMemory( st,st->size + j ) ;
			if( d != NULL ){	
				st->string = d ;
				c = st->string + len ;
				memmove( c + j,c + k,st->size - ( c - st->string ) + 1 ) ;
				memcpy( c,s,j ) ;
				st->size = st->size + j - k ;
				e = st->string + len + j ;
			}
		}
	}else if( k > j ){
		while( ( c = strstr( e,x ) ) != NULL ){
			len = c - st->string ; 
			memmove( c + j,c + k,st->size - ( c - st->string + k ) + 1 ) ;
			memcpy( c,s,j ) ;
			if( d != NULL ){
				st->string = d ;
				st->size = st->size +j - k  ;
				e = st->string + len ;
			}
		}
	}
	
	return d ;
}

const char * StringReplaceStringPos( string_t st,const char * x,const char * s,size_t p ) 
{
	return StringRS__( st,x,s,p ) ;
}

const char * StringReplaceString( string_t st,const char * x,const char * s ) 
{
	return StringReplaceStringPos( st,x,s,0 ) ;
}

const char * StringRemoveStringPos( string_t st,const char * s,size_t p ) 
{
	return StringRS__( st,s,"",p ) ;
}

const char * StringRemoveString( string_t st,const char * s )
{
	return StringRemoveStringPos( st,s,0 ) ;
}

static char * StringCRC__( string_t st, char x,char y,size_t p )
{
	char * c ;
	if( st == StringVoid ){
		return NULL ;
	}
	if( p >= st->size ){
		return st->string ;
	}
	c = st->string - 1 + p ;
	while ( *++c  ){
		if( *c == x ){
			*c = y ;
		}
	}
	return st->string ;
}


const char * StringReplaceCharPos( string_t st,char x,char y,size_t p )
{	
	return StringCRC__( st,x,y,p ) ;
}

const char * StringReplaceChar_1( string_t st,size_t index,char x,char y ) 
{
	char * e ;
	if( st == StringVoid ){
		return NULL ;
	}
	if( index >= st->size ){
		return NULL ;
	}
	e = strchr( st->string + index,x ) ;
	if( e != NULL ){
		*e = y ;
		return st->string ;
	}else{
		return NULL ;
	}
}

const char * StringReplaceChar( string_t st,char x,char y )
{
	return StringReplaceCharPos( st,x,y,0 ) ;
}

/*
 *  2^64 has a maximum of 19 digits,64 byte buffer is more that enough
 */
#define BUFFSIZE 64
#define BUFFLIMIT 63
static inline char * _intToString( char * buffer,u_int64_t z )
{
	int i = BUFFLIMIT ;
	do{
		i-- ;
		buffer[ i ] = z % 10 + '0' ;
		z = z / 10 ;
	}while( z != 0 && i != 0 ) ;
	return buffer + i ;
}

string_t StringIntToString( u_int64_t z )
{
	char buffer[ BUFFSIZE ] = { '\0' };
	return String( _intToString( buffer,z ) );
}

const char * StringAppendInt( string_t st,u_int64_t z ) 
{
	char buffer[ BUFFSIZE ] = { '\0' };
	return StringAppend( st,_intToString( buffer,z ) ) ;
}

const char * StringSubStringWithInt( string_t st,const char * str,u_int64_t z ) 
{
	char buffer[ BUFFSIZE ] = { '\0' };
	return StringReplaceString( st,str,_intToString( buffer,z ) ) ;
}

char * StringIntToString_1( char * x,size_t y,u_int64_t z )
{
	char * c ;
	if( x == NULL ){
		return NULL ;
	}
	c =  x + y - 1  ;
	*c-- = '\0' ;
	do{
		*c-- = z % 10 + '0' ;
		z = z / 10 ;
	}while( z != 0 ) ;
	return ++c ;
}

int StringEqualString( string_t x,string_t y ) 
{
	if( x == StringVoid || y == StringVoid || x->size != y->size ){
		return 0 ;
	}else{
		return strcmp( x->string,y->string ) == 0 ;
	}
}

int StringEqual( string_t x,const char * y )
{
	if( x == StringVoid ){
		return 0 ;
	}else{
		return strcmp( x->string,y ) == 0 ;
	}
}

int StringAtLeastOneMatch( string_t st,... )
{
	va_list list ;
	int r = 0   ;
	const char * e   ;
	const char * f   ;
	
	if( st == StringVoid ){
		return 0 ;
	}
	f = st->string ;
	va_start( list,st ) ;
	while( 1 ){
		e = va_arg( list,const char * ) ;
		if( e == NULL ){
			break ;
		}else if( strcmp( f,e ) == 0 ){
			r = 1 ;
			break ;
		}
	}
	
	va_end( list ) ;
	return r ;
}

int StringAtLeastOneMatch_1( const char * x,... )
{
	va_list list ;
	int r = 0 ;
	const char * e ;
	
	va_start( list,x ) ;
	
	if( x == NULL ){
		return 0 ;
	}
	while( 1 ){
		e = va_arg( list,const char * ) ;
		if( e == NULL ){
			break ;
		}else if( strcmp( x,e ) == 0 ){
			r = 1 ;
			break ;
		}
	}
	
	va_end( list ) ;
	return r ;
}

int StringAtLeastOnePrefixMatch( const char * x,... )
{
	va_list list ;
	int r = 0 ;
	const char * e ;
	
	va_start( list,x ) ;
	
	if( x == NULL ){
		return 0 ;
	}
	while( 1 ){
		e = va_arg( list,const char * ) ;
		if( e == NULL ){
			break ;
		}else if( strncmp( x,e,strlen( e ) ) == 0 ){
			r = 1 ;
			break ;
		}
	}
	
	va_end( list ) ;
	return r ;
}

static char * StringICS__( string_t st,char x,const char * s,size_t p )
{
	const char * d ;
	char * e ;
	char * f  ;
	size_t pos ;
	
	if( st == StringVoid ){
		return NULL ;
	}
	if( p >= st->size || s == NULL ){
		return st->string ;
	}
	d = s - 1 ;
	while( *++d ){
		f = st->string - 1 + p ;
		while( *++f ){
			if( *d == *f ){
				pos = f - st->string ;
				e = __StringExpandMemory( st,st->size + 2 ) ;
				if( e != NULL ){
					st->string = e ;
					memmove( e + pos + 1,e + pos,st->size - pos + 1 ) ;
					*( e + pos ) = x ;
					f = e + pos + 1 ;
					st->size++ ;
				}
			}
		}
	}
	return st->string ;
}

const char * StringInsertCharStringPos( string_t st,char x,const char * s,size_t p ) 
{	
	return StringICS__( st,x,s,p ) ;
}

const char * StringInsertCharString( string_t st,char x,const char * s ) 
{
 	return StringInsertCharStringPos( st,x,s,0 ) ;
}

const char * StringInsertCharChar( string_t st,char x,char y )
{
	char c[2] ;
	c[0] = y ;
	c[1] = '\0' ;
	return StringInsertCharString( st,x,c ) ;
}

string_t StringGetFromTerminal( void ) 
{
	int c ;
	const char * d ;
	string_t p = String( "" ) ;
	
	while( 1 ){
		c = getchar() ;
		if( c == '\n' || c == EOF ){
			break ;
		}else{
			d = StringAppendChar( p,( char )c ) ;
			if( d == NULL ){
				StringDelete( &p ) ;
				return StringVoid ;
			}
		}
	}
	
	return p ;
}

string_t StringGetFromTerminal_1( size_t s ) 
{
	int c ;
	const char * d ;
	string_t p = String( "" ) ;
	
	while( 1 ){
		if( s == 0 ){
			/*
			 * we already got the requested number of characters,now clear the buffer
			 * by reading until we get a newline or EOF character
			 */
			while( 1 ){
				c = getchar() ;
				if( c == '\n' || c == EOF ){
					break ;
				}
			}
			
			return p ;
		}
		
		c = getchar() ;
		
		if( c == '\n' || c == EOF ){
			break ;
		}else{
			s-- ;
			d = StringAppendChar( p,( char )c ) ;
			if( d == NULL ){
				StringDelete( &p ) ;
				return StringVoid ;
			}
		}
	}
	
	return p ;
}

static inline int __terminalEchoOff( struct termios * old,struct termios * new )
{	
	if( tcgetattr ( 1,old ) != 0 ){
		return 1 ;
	}
	
	*new = *old;
	new->c_lflag &= ~ECHO;
	
	if( tcsetattr ( 1,TCSAFLUSH,new ) != 0 ){
		return 1 ;
	}else{
		return 0 ;
	}
}

int StringSilentlyGetFromTerminal( string_t * q ) 
{
	string_t p ;
	struct termios old ;
	struct termios new ;
	
	if( __terminalEchoOff( &old,&new ) == 1 ){
		return 1 ;
	}
	
	p = StringGetFromTerminal() ;
	
	if( p == StringVoid ){
		return 2 ;
	}
	
	tcsetattr( 1,TCSAFLUSH,&old );
	
	*q = p ;
	return 0 ;
}

int StringSilentlyGetFromTerminal_1( string_t * q,size_t s ) 
{
	string_t p ;
	struct termios old ;
	struct termios new ;
	
	if( __terminalEchoOff( &old,&new ) == 1 ){
		return 1 ;
	}
	p = StringGetFromTerminal_1( s ) ;
	if( p == StringVoid ){
		return 2 ;
	}
	tcsetattr ( 1,TCSAFLUSH,&old );
	
	*q = p ;
	return 0 ;
}

string_t StringRandomString( size_t size )
{
	string_t s = StringVoid ;
	char * e ;
	char c ;
	int f ;
	size_t g = 0 ;
	
	if( size < 1 ){
		return s ;
	}
	
	e = ( char * ) malloc( sizeof( char ) * ( size + 1 ) ) ;
	
	if( e == NULL ){
		return s ;
	}
	
	f = open( "/dev/urandom",O_RDONLY ) ;
	
	if( f == -1 ){
		free( e ) ;
		return s ;
	}
		
	while( g < size ){
		read( f,&c,1 ) ;
		if( c >= ' ' && c <= '~' ){
			*( e + g ) = c ;
			g++ ;
		}
	}
		
	*( e + size ) = '\0' ;
		
	close( f ) ;
	
	s = StringInheritWithSize( &e,size,size + 1 ) ;
	
	if( s == StringVoid ){
		free( e ) ;
	}
	return s ;
}

u_int32_t StringJenkinsOneAtATimeHash( const char * key )
{
	size_t l ;
	u_int32_t hash = 0;
	u_int32_t i ;
	
	if( key != NULL ){
		l = strlen( key ) ;
		for( i = 0 ; i < l ; i++ ){
			hash += key[ i ];
			hash += ( hash << 10 );
			hash ^= ( hash >> 6 );
		}
		hash += ( hash << 3 );
		hash ^= ( hash >> 11 );
		hash += ( hash << 15 );
	}
	return hash;
}

u_int32_t StringJenkinsOneAtATimeHash_1( string_t st ) 
{
	size_t l ;
	u_int32_t hash = 0;
	u_int32_t i ;
	const char * e ;
	
	if( st != StringVoid ){
		e = st->string ;
		l = st->size ;
		for( i = 0 ; i < l ; i++ ){
			hash += e[ i ];
			hash += ( hash << 10 );
			hash ^= ( hash >> 6 );
		}
		hash += ( hash << 3 );
		hash ^= ( hash >> 11 );
		hash += ( hash << 15 );
	}
	return hash ;
}

int StringGetFromFile_1( string_t * str,const char * path ) 
{
	struct stat st ;
	if( stat( path,&st ) != 0 ){
		return 1 ;
	}else{
		return StringGetFromFile_3( str,path,0,st.st_size ) ;
	}
}

int StringGetFromFile_3( string_t * str,const char * path,size_t offset,size_t length ) 
{
	int fd ;
	char * c ;
	ssize_t size ;
	
	struct stat xt ;
	
	*str = StringVoid ;
	if( path == NULL ){
		return 1 ;
	}
	if( stat( path,&xt ) != 0 ){
		return 1 ;
	}
	if( ( fd = open( path,O_RDONLY ) ) == -1 ){
		return 2 ;	
	}
	if( lseek( fd,offset,SEEK_SET ) == -1 ){
		close( fd ) ;
		return 2 ;
	}
	
	c = ( char * ) malloc( sizeof( char ) * ( length + 1 ) ) ; 
	
	if( c == NULL ) {
		close( fd ) ;
		_StringError() ;
		return 3 ;
	}
	
	size = read( fd,c,length ) ;
	
	if( size <= 0 ){
		free( c ) ;
		close( fd ) ;
		return 2 ;
	}
	
	close( fd ) ;
	
	*( c + length ) = '\0' ;
	
	*str = StringInheritWithSize( &c,( size_t )size,length + 1 ) ;
	
	if( *str == StringVoid ){
		free( c ) ;
		return 3 ;
	}else{
		return 0 ;
	}
}

int StringGetFromFileLocked( string_t * str,const char * path,size_t offset,size_t length ) 
{
	int fd ;
	char * c ;
	ssize_t size ;
	
	struct stat xt ;
	
	*str = StringVoid ;
	if( path == NULL ){
		return 1 ;
	}
	if( stat( path,&xt ) != 0 ){
		return 1 ;
	}
	if( ( fd = open( path,O_RDONLY ) ) == -1 ){
		return 2 ;	
	}
	if( lseek( fd,offset,SEEK_SET ) == -1 ){
		close( fd ) ;
		return 2 ;
	}
	if( length == 0 ){
		length = xt.st_size ;
	}
	
	c = ( char * ) malloc( sizeof( char ) * ( length + 1 ) ) ; 
	
	if( c == NULL ) {
		close( fd ) ;
		_StringError() ;
		return 3 ;
	}
	
	mlock( c,length + 1 ) ;
	
	size = read( fd,c,length ) ;
	
	if( size <= 0 ){
		munlock( c,length + 1 ) ;
		free( c ) ;
		close( fd ) ;
		return 2 ;
	}
	
	close( fd ) ;
	
	*( c + length ) = '\0' ;
	
	*str = StringInheritWithSize( &c,( size_t )size,length + 1 ) ;
	
	if( *str == StringVoid ){
		free( c ) ;
		return 3 ;
	}else{
		return 0 ;
	}
}

string_t StringGetFromFile_2( const char * path,int * status ) 
{
	string_t st = NULL ;
	struct stat xt ;
	if( stat( path,&xt ) != 0 ){
		*status = 1 ;
		return StringVoid ;
	}else{
		*status = StringGetFromFile_3( &st,path,0,xt.st_size ) ;
		return st ; 
	}
}

string_t StringGetFromFile( const char * path )
{
	string_t st = StringVoid ;
	StringGetFromFile_1( &st,path ) ;
	return st ;
}

void StringWriteToFile( string_t st,const char * path,int mode ) 
{
	int fd ;

	if( st == StringVoid ){
		return ;
	}
	
	if( mode == 1 ){
		fd = open( path,O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ) ;
	}else{
		fd = open( path,O_WRONLY | O_CREAT | O_APPEND,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ) ;
	}
	
	if( fd == -1 ){
		return ;
	}
	
	write( fd,st->string,st->size ) ;
	
	close( fd ) ;
	
	chown( path,getuid(),getgid() ) ;
	chmod( path,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ) ;
}

static inline string_t _freeBuffer( char * buffer,int fd )
{
	close( fd ) ;
	if( buffer != NULL ){
		free( buffer );
		return _StringError() ;
	}else{
		return _StringError() ;
	}
}

string_t StringGetFromVirtualFile( const char * path ) 
{
	#define SIZE 64
	
	char * buffer = NULL ;
	char * e ;
	
	size_t size = SIZE ;
	size_t size_1 = 0 ;
	size_t strLen ;
	size_t bufferLen ;
	ssize_t j ;
	
	int fd = open( path,O_RDONLY ) ;
	if( fd == -1 ){
		buffer = ( char * )malloc( sizeof( char ) ) ;
		if( buffer == NULL ){
			return _StringError() ;
		}else{
			buffer[0] = '\0' ;
			return StringInheritWithSize( &buffer,0,1 ) ;
		}
	}
	
	while( 1 ){
		e = ( char * )realloc( buffer,size ) ;
		if( e == NULL ){
			return _freeBuffer( buffer,fd ) ;
		}else{
			buffer = e ;
			j = read( fd,buffer + size_1,SIZE ) ;
			if( j < SIZE ){
				if( j <= 0 ){
					e = realloc( buffer,size_1 + 1 ) ;
					if( e != NULL ){
						buffer           = e ;
						buffer[ size_1 ] = '\0' ;
						strLen           = size_1 ;
						bufferLen        = size_1 + 1 ;
					}else{
						return _freeBuffer( buffer,fd ) ;
					}
				}else{
					buffer[ size_1 + j ] = '\0' ;
					strLen               = size_1 + j ;
					bufferLen            = size ;
				}
				break ;
			}else{
				size   += SIZE ;
				size_1 += j ;
			}
		}
	}
	
	close( fd ) ;
	
	return StringInheritWithSize( &buffer,strLen,bufferLen ) ;
}
