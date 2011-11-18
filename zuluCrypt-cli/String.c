
/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

StrHandle * String(const char * data)
{
	const char *c = data ;
	
	char *d ;
	
	StrHandle *str = ( StrHandle * ) malloc ( sizeof( StrHandle ) ) ;
	
	if( str == NULL )
		return NULL ;
	
	str->size = strlen( data ) ;
	
	d = str-> string = ( char * ) malloc ( sizeof ( char ) * ( str->size + 1 ) ) ;
	
	if ( str->string == NULL )
	{
		free( str ) ;
		return NULL ;
	}
	
	while( ( *d++ = *c++ ) != '\0' ) { ; }	
	
	return str ;	
}

void StringReadToBuffer( StrHandle * st,char *buffer, int size)
{
	int i  ;	
	for ( i = 0 ; i < size ; i++ )
		buffer[i] = st->string[i] ;	
}

const char * StringAppend(StrHandle * st ,const char * data) 
{	
	char * a ;
	
	char * b ;
	
	char * c ;
	
	const char * d ;
	
	st->size = st->size + strlen(data) ;
	
	c = a = ( char * ) malloc ( sizeof(char) * ( st->size + 1 ) ) ;
	
	if ( a == NULL )
	{	
		st->size = st->size - strlen(data) ;
		return NULL ;
	}
	
	b = st->string ;
	
	while( ( *c++ = *b++ ) != '\0' ) { ; } 
	
	d = data ;
	
	c-- ;
	
	while( ( *c++ = *d++ ) != '\0' ) { ; } 	
	
	free( st->string ) ;
	
	return st->string = a ;	
}

StrHandle * StringInherit( char * data )
{
	StrHandle *str = ( StrHandle * ) malloc ( sizeof( StrHandle ) ) ;
	
	if( str == NULL )
		return NULL ;
	
	str->size = strlen( data ) ;
	
	str->string = data ;
	
	return str ;	
}

int StringIndexOfString( StrHandle * st,int p, const char * s )
{
	char *c = strstr(st->string + p,s) ;
	
	if( c == NULL )
		return -1 ;
	else
		return  c - st->string;	
}

int StringLastIndexOfChar(StrHandle * st , char s) 
{
	char *c = st->string + st->size ;
	
	while( 1 )
	{
		
		if( st->string == c )
			return -1 ;
		else if ( *c == s )
			return c - st->string ;
		
		c-- ;		
	}	
}

int StringLastIndexOfString(StrHandle * st ,const char *s) 
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

int StringIndexOfChar( StrHandle * st, int p , char s) 
{	
	char * c = st->string  + p - 1 ;	
	
	while ( 1 )
	{
		++c ;
		if( *c == '\0') 
			return -1 ;
		else if( *c == s ) 
			return c - st->string ;		
	}	
}

const char * StringRemoveString(StrHandle *st,int x , int y) 
{
	char *c = st->string  ;
	
	char *d ;
	
	char * e ;
	
	int i ;
	
	st->size = st->size - y ;
		
	e = d = ( char * ) malloc( sizeof(char) * ( st->size + 1  ) ) ;

	if( d == NULL )
	{
		st->size = st->size + y ;
		return NULL ;
	}
	
	i = 0 ;
	
	while( i++ < x )
		*e++ = *c++ ;
	
	c = c + y  ;
	
	while( ( *e++ = *c++ ) != '\0' ) { ; }
	
	free( st->string ) ;
	
	return st->string = d ;
}

int StringLength(StrHandle * st)
{
	return st->size ;	
}

const char * StringContent( StrHandle * st)
{
	return st->string ;
}

char * StringCopy( StrHandle *st )
{
	return StringLengthCopy( st,StringLength( st ) ) ;	
}

char * StringLengthCopy( StrHandle *st,int l )
{
	int i = 0 ;
	
	char *c ;
	
	char *d = st->string;
	
	char *e ;
	
	e =  c = ( char * )malloc( sizeof( char ) * ( l + 1 ) ) ;
	
	if( e == NULL )
		return NULL ;
	
	while( i++ < l )
		*c++ = *d++ ;
	
	*c = '\0' ;
	
	return e ;	
}

int StringEndsWithString(StrHandle * st , const char *s)
{
	int j = strlen(s) ;
	
	int i = strncmp(st->string + st->size - j, s, j ) ;
	
	if( i == 0 )
		return 0 ;
	else
		return -1 ;	
}

int StringEndsWithChar(StrHandle * st ,char s)
{
	if ( * ( st->string + st->size -1 ) == s )
		return 0 ;
	else
		return -1 ;
}

char StringCharAt( StrHandle * st, int p)
{
	return st->string[p] ;
}

const char * StringStringAt( StrHandle * st , int p)
{
	return st->string + p ;	
}

void StringDelete(StrHandle * st)
{	
	if( st != NULL ) 
	{
		free( st->string ) ;
		free( st ) ;
		st = NULL ;
	}
}

char * StringDeleteHandle( StrHandle *st)
{
	char * c = st->string ;
	free( st ) ;
	return c ;	
}

const char * StringSubChar(StrHandle * st, int x, char s )
{	
	* ( st->string + x ) = s ;
	return st->string ; 	
}

const char * StringSubString(StrHandle * st, int x, const char * s ) 
{
	const char * c = s - 1  ;
	char *d = st->string + x  ;
	
	while( *++c  != '\0' ) 
		*d++ = *c ;
	
	return st->string ;
}

const char * StringInsertCharAt(StrHandle * st, int x,char s ) 
{
	char c[2] ;
	
	c[0] = s ;
	
	c[1] = '\0' ;
	
	return StringInsertStringAt(st, x, c ) ;
}

const char * StringInsertStringAt(StrHandle * st, int x, const char * s )
{	

	int i  ;

	st->size = st->size + strlen( s ) ;

	char * c ;
	
	char * d ;
	
	const char * f ;
	
	char * e = st->string;
	
	c = d = ( char * ) malloc ( sizeof( char ) * ( st->size + 1 ) ) ;
	
	if( c == NULL )
	{
		st->size = st->size - strlen( s ) ;
		return NULL ;
	}
	i = 0 ;
	
	while( i++ < x )
		*c++ = *e++ ;	

	f = s ;
	
	while ( ( *c++ = *f++ ) != '\0' ) { ; }
	
	c-- ;	
	
	while ( ( *c++ = *e++ ) != '\0' ) { ; }
	
	free( st->string ) ;
	
	return  st->string = d ;
}

char * StringMidString( StrHandle *st , int x, int y ) 
{
	int i = 0 ;
	
	char * d ;
	
	char * f ;
	
	char * c = ( char * ) malloc ( sizeof( char ) * ( y + 1 ) ) ;
	
	if( c == NULL )
		return NULL ;
	
	d = c ;
	
	f = st->string + x;
	
	while ( i++ < y )
		*d++ = *f++ ;
	
	*d = '\0' ;
	
	return c ;
}

const char * StringRemoveRight(StrHandle * st, int x) 
{
	char * c ;
	
	char * d ;
	
	char * f ;	
	
	char * e ;
	
	c = d = ( char * ) malloc ( sizeof (char ) * ( x + 1 ) ) ;
	
	if ( c == NULL )		
		return NULL ;		

	st->size = x ;
	
	f = st->string - 1 ;
	
	e = st->string + x ;
	
	while( ++f < e )
		*c++ = *f ;
	
	*c = '\0' ;
	
	free( st->string ) ;
	
	return st->string = d ;
}

const char * StringRemoveLeft(StrHandle * st, int x) 
{
	char * c ;
	
	char * d ;
	
	char * f ;
	
	st->size = st->size - x ;
	
	c = d = ( char * ) malloc ( sizeof (char ) * ( st->size + 1 ) ) ;
	
	if ( c == NULL )
	{
		st->size = st->size + x ;
		return NULL ;
	}
	
	f = st->string + x ;
	
	while( ( *c++ = *f++ ) != 0 ) { ; }
	
	free( st->string ) ;
	
	return st->string = d ;	
}

const char * StringReplaceString( StrHandle *st, const char *x, const char *y) 
{
	int i = 0 ;
	
	while( ( i = StringIndexOfString(st,i,x) ) != -1 )
	{		
		StringRemoveString(st,i,strlen(x));
		StringInsertStringAt(st,i,y) ;		
	}	
	return st->string ;	
}

const char * StringReplaceChar( StrHandle *st, char x, char y)
{
	char *c = st->string - 1;
	
	while ( *++c != '\0' )
		if( *c == x )
			*c = y ;

	return st->string ;	
}

char * intToString(char * x, int y,int z)
{
	char *c =  x + y  ;
	
	*c-- = '\0' ;	
	
	do{
		*c-- = z % 10 + '0' ;
		
		z = z / 10 ;		
		
	}while( z != 0 ) ;
	
	return ++c ;
}
