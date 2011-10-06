
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

StrHandle * StringCpy(const char * data)
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

const char * StringCat(StrHandle * st ,const char * data) 
{	
	char * a ;
	
	char * b ;
	
	char * c ;
	
	const char * d ;
	
	st->size = st->size + strlen(data) ;
	
	c = a = ( char * ) malloc ( sizeof(char) * ( st->size + 1 ) ) ;
	
	if ( a == NULL )
		return NULL ;
	
	b = st->string ;
	
	while( ( *c++ = *b++ ) != '\0' ) { ; } 
	
	d = data ;
	
	c-- ;
	
	while( ( *c++ = *d++ ) != '\0' ) { ; } 	
	
	free( st->string ) ;
	
	return st->string = a ;
	
}

int StringLength(StrHandle * st)
{
	return st->size ;	
}

const char * StringCont( StrHandle * st)
{
	return st->string ;
}

char * StringContCopy( StrHandle *st )
{
	return StringContCopyLength( st,StringLength( st ) ) ;	
}

char * StringContCopyLength( StrHandle *st,int l )
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

char StringCharAt( StrHandle * st, int p)
{
	return st->string[p] ;
}

const char * StringFrom( StrHandle * st , int p)
{
	return &(st->string[p]) ;
	
}

void StringDelete(StrHandle * st)
{	
	free( st->string ) ;
	free( st ) ;
}

const char * StringCharInsert(StrHandle * st, int x,char s ) 
{
	char c[2] ;
	
	c[0] = s ;
	
	c[1] = '\0' ;
	
	return StringInsert(st, x, c ) ;
}

const char * StringInsert(StrHandle * st, int x, const char * s )
{	

	int i  ;

	st->size = st->size + strlen( s ) ;

	char * c ;
	
	char * d ;
	
	const char * f ;
	
	char * e = st->string;
	
	c = d = ( char * ) malloc ( sizeof( char ) * ( st->size + 1 ) ) ;
	
	if( c == NULL )
		return NULL ;
	
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

const char * StringChop(StrHandle * st, int x) 
{
	char * c ;
	
	char * d ;
	
	const char * f ;
	
	int i ;
	
	st->size = st->size - x ;
	
	c = d = ( char * ) malloc ( sizeof (char ) * ( st->size + 1 ) ) ;
	
	if ( c == NULL )
		return NULL ;
	
	f = st->string ;
	
	i = 0 ;
	
	while( i++ < x )
		*c++ = *f++ ;
	
	*c = '\0' ;
	
	free( st->string ) ;
	
	return st->string = d ;
}

