
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
#include <stdio.h>

struct StringType
{
	size_t size ;
	char * string ; 
};

string_t String( const char * c )
{
	char * d ;
	
	string_t st = ( string_t ) malloc ( sizeof( struct StringType ) ) ;
	
	if( st == NULL )
		return NULL ;
	
	st->size = strlen( c ) ;
	
	d = st->string = ( char * ) malloc ( sizeof ( char ) * ( st->size + 1 ) ) ;
	
	if ( d == NULL )
	{
		free( st ) ;
		return NULL ;
	}
	
	while( ( *d++ = *c++ ) ) { ; }	
	
	return st ;	
}

void StringReadToBuffer( string_t st,char * buffer, size_t size )
{
	size_t i  ;	
	for ( i = 0 ; i < size ; i++ )
		buffer[i] = st->string[i] ;	
}

string_t StringInherit( char * data )
{
	string_t st = ( string_t ) malloc ( sizeof( struct StringType ) ) ;
	
	if( st == NULL )
		return NULL ;
	
	st->size = strlen( data ) ;
	
	st->string = data ;
	
	return st ;	
}

int StringIndexOfString( string_t st,size_t p, const char * s )
{
	char * c = strstr( st->string + p,s ) ;
	
	if( c == NULL )
		return -1 ;
	else
		return  c - st->string;	
}

int StringLastIndexOfChar( string_t st , char s ) 
{
	char * c = st->string + st->size  ;
	char * d = st->string ;
	
	while( --c != d )
		if ( *c == s )
			return c - d ;
		
	return -1 ;	
}

int StringLastIndexOfString( string_t st ,const char * s ) 
{
	int p = -1 ;
	int j = -1 ;
	
	while( 1 )
	{		
		p = StringIndexOfString( st,p + 1,s ) ;
		
		if( p == -1 )
			return j ;
		else
			j = p ;
	}
}

int StringIndexOfChar( string_t st, size_t p , char s ) 
{	
	char * c = st->string  + p - 1 ;	
	
	while ( *++c )
		if( *c == s ) 
			return c - st->string ;		
			
	return -1 ;
}

const char * StringRemoveString( string_t st,size_t x , size_t y ) 
{	
	char * b = st->string + x ;
	char * c = st->string - 1  ;
	char * d ;
	char * e ;
	size_t f = st->size - y ;
	
	e = d = ( char * ) malloc( sizeof(char) * ( f + 1  ) ) ;

	if( d == NULL )
		return NULL ;
	
	st->size = f ;
	
	while( ++c < b ) { *e++ = *c ; }
	
	c = c + y  ;
	
	while( ( *e++ = *c++ ) ) { ; }
	
	free( st->string ) ;
	
	return st->string = d ;
}

size_t StringLength( string_t st )
{
	return st->size ;	
}

const char * StringContent( string_t st )
{
	return st->string ;
}

char * StringCopy( string_t st )
{
	return StringLengthCopy( st,StringLength( st ) ) ;	
}

char * StringLengthCopy( string_t st,size_t l )
{
	char * c ;
	char * d = st->string - 1 ;
	char * e ;
	char * f = st->string + l ;
	
	e =  c = ( char * )malloc( sizeof( char ) * ( l + 1 ) ) ;
	
	if( e == NULL )
		return NULL ;
	
	while( ++d < f ) { *c++ = *d  ; }
	
	*c = '\0' ;
	
	return e ;	
}

int StringEndsWithString( string_t st , const char *s ) 
{
	size_t j = strlen(s) ;
	size_t i = strncmp(st->string + st->size - j, s, j ) ;
	
	if( i == 0 )
		return 0 ;
	else
		return -1 ;	
}

int StringEndsWithChar( string_t st ,char s )
{
	if ( * ( st->string + st->size -1 ) == s )
		return 0 ;
	else
		return -1 ;
}

char StringCharAt( string_t st, size_t p)
{
	return * ( st->string + p )  ;
}

const char * StringStringAt( string_t st , size_t p)
{
	return st->string + p ;	
}

void StringDelete( string_t st)
{	
	if( st != NULL ) 
	{
		free( st->string ) ;
		free( st ) ;
		st = NULL ;
	}
}

char * StringDeleteHandle( string_t st)
{
	char * c = st->string ;
	free( st ) ;
	return c ;	
}

const char * StringSubChar( string_t st, size_t x, char s )
{	
	* ( st->string + x ) = s ;
	return st->string ; 	
}

const char * StringReplaceCharString( string_t st, char x, const char * y ) 
{
	size_t i ;
	size_t j ;
	size_t k = strlen( y ) ;
	size_t l = st->size ;
	
	for( j = 0 ; j < l ; j++ )
	{		
		for( i = 0 ; i < k ; i++ )
		{		
			if( * ( st->string + j ) == * ( y + i ) )
			{				
				* ( st->string + j ) = x ;
				break ;
			}
		}	
	}
	return st->string ;
}

const char * StringSubString( string_t st, size_t x, const char * s ) 
{
	const char * c = s - 1  ;
	char * d = st->string + x  ;
	
	while( *++c ) { *d++ = *c ; }
	
	return st->string ;
}

const char * StringInsertChar( string_t st, size_t x,char s ) 
{
	char c[2] ;
	c[0] = s ;
	c[1] = '\0' ;
	return StringInsertString( st, x, c ) ;
}

const char * StringPrepend( string_t st ,const  char * s )
{
	return StringInsertString( st,0,s ) ;
}

const char * StringAppend( string_t st ,const char * s ) 
{	
	return StringInsertString( st, st->size, s ) ;
}

const char * StringInsertString( string_t st, size_t x, const char * s )
{
	size_t k = st->size + strlen( s ) ;
	char * c ;
	char * d ;
	char * f = st->string + x ;
	char * e = st->string - 1 ;
	
	c = d = ( char * ) malloc ( sizeof( char ) * ( k + 1 ) ) ;
	
	if( d == NULL )
		return NULL ;
		
	st->size = k ;
	
	s = s - 1 ;
	
	while( ++e < f ) { *c++ = *e ; }	
	
	while( *++s ) { *c++ = *s ; }	
	
	while( ( *c++ = *e++ ) ) { ; }
	
	free( st->string ) ;
	
	return  st->string = d ;
}

char * StringMidString( string_t st , size_t x, size_t y ) 
{
	char * c ;
	char * d ;
	char * e = st->string + x + y ;
	char * f = st->string + x - 1 ;		
	
	c = d = ( char * ) malloc ( sizeof( char ) * ( y + 1 ) ) ;
	
	if( c == NULL )
		return NULL ;
	
	while ( ++f < e ) { *d++ = *f ; }
	
	*d = '\0' ;
	
	return c ;
}

const char * StringRemoveRight(string_t st, size_t x ) 
{
	return StringCrop( st,0,x ) ;
}

const char * StringRemoveLeft(string_t st, size_t x ) 
{
	return StringCrop( st,x,0 ) ;
}

const char * StringCrop( string_t st, size_t x, size_t y ) 
{
	char * c ;
	char * d ;
	char * e ;
	size_t j = 0 ;
	size_t k = st->size - y - x ;
	
	if( k < 0 )
		return NULL ;
	
	c = d = ( char * ) malloc ( sizeof( char ) * ( k + 1 ) ) ;
	
	if( c == NULL )
		return NULL ;
	
	st->size = k ;
	
	e = st->string + x ;
	
	while( j++ < k ) { *c++ = *e++ ; }
	
	*c = '\0' ;
	
	free( st->string ) ;
	
	return st->string = d ;	
}

const char * StringRemoveStringString(string_t st, const char * s ) 
{
	return StringReplaceString( st,s,"" ) ;
}

const char * StringReplaceString( string_t st, const char * x, const char * s ) 
{
	char * c ;
	char * d ;
	char * e = st->string  ;
	char * f ;
	const char * g ;
	size_t i = 0 ;
	size_t j = strlen( s ) ;
	size_t k = strlen( x ) ;
	
	while( ( c = strstr( e, x ) ) != NULL )
	{
		i++ ;
		e = c + k ;		
	}
	
	i = st->size - i * ( k - j ) ;
	
	d = f = ( char * ) malloc( sizeof( char ) * ( i + 1 ) ) ;
	
	if ( d == NULL )
		return NULL ;
	
	st->size = i ;
	
	e = st->string  ;
	
	while( ( c = strstr( e, x ) ) != NULL )
	{		
		while( e != c ) { *d++ = *e++ ; }			
				
		g = s - 1 ;
		
		while ( *++g  ) { *d++ = *g ; }
		
		e = c + k ;
	}
	
	while ( ( *d++ = *e++ ) ) { ; }
	
	free( st->string ) ;
	
	return st->string = f ;	
}

const char * StringReplaceChar( string_t st, char x, char y)
{
	char * c = st->string - 1;
	
	while ( *++c  )
		if( *c == x )
			*c = y ;

	return st->string ;	
}

char * StringIntToString( char * x, size_t y,uint64_t z )
{
	char *c =  x + y - 1  ;
	
	*c-- = '\0' ;	
	
	do{
		*c-- = z % 10 + '0' ;
		z = z / 10 ;		
		
	}while( z != 0 ) ;
	
	return ++c ;
}

int StringCompare( string_t x , string_t y ) 
{
	if( x->size != y->size )
		return 1 ;
	else if( strcmp( x->string, y->string ) != 0 )
		return -1 ;
	else
		return 0 ;	
}

int StringCompareString( string_t x, const char * y )
{
	if( strcmp( x->string, y ) == 0 )
		return 0 ;
	else
		return -1 ;	
}

const char * StringInsertCharString( string_t st, char x, const char * n ) 
{	
	char * d ;
	char * f ;
	size_t count = 0 ;
	size_t i,j ;
	size_t z = st->size ;
	char * c = st->string ;
	size_t k = strlen( n ) ;
	
	for ( i = 0 ; i < z ; i++ )
	{		
		for( j = 0 ; j < k ; j++ )
		{			
			if( c[i] == n[j] )
			{
				count++ ;
				break ;
			}
		}		
	}
	
	f = d = (char * ) malloc(sizeof(char) * ( z + count + 1 ) ) ;	
	
	if( f == NULL )
		return NULL ;
	
	st->size = z + count ;
	
	for ( i = 0 ; i < z ; i++ ){
		
		for( j = 0 ; j < k ; j++ ){
			
			if( c[i] == n[j] ){
				*f++ = x ;
				break ;
			}
		}
		
		*f++ = c[i] ;
	}
	*f = '\0' ;
	
	free( st->string ) ;
	
	return st->string = d ;
}

const char * StringInsertCharChar( string_t st, char x, char y )
{
	char c[2] ;
	c[0] = y ;
	c[1] = '\0' ;
	return StringInsertCharString( st, x, c ) ;
}
