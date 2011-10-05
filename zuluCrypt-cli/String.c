
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
		return NULL ;
	
	while( ( *d++ = *c++ ) != '\0' ) { ; }	
	
	return str ;	
}

const char * StringCat(StrHandle * st ,const char * data) 
{	
	st->size = st->size + strlen(data) ;
	
	char * a = ( char * ) malloc ( sizeof(char) * ( st->size + 1 ) ) ;
	
	if ( a == NULL )
		return NULL ;
	
	char * b ;
	
	char * c ;
	
	const char * d ;
	
	b = st->string ;
	
	c = a ;
	
	while( ( *c++ = *b++ ) != '\0' ) { ; } 
	
	d = data ;
	
	c-- ;
	
	while( ( *c++ = *d++ ) != '\0' ) { ; } 	
	
	free( st->string ) ;
	
	st->string = a ;
	
	return a ;	
}

int StringLength(StrHandle * st)
{
	return st->size ;	
}

const char * StringCont( StrHandle * st)
{
	const char *c = st->string ;
	return  c ;
}

char * StringContCopy( StrHandle *st )
{
	char *c ;
	char *d = st->string;
	char *e ;
	
	e =  c = ( char * )malloc( sizeof( char ) * st->size ) ;
	
	while( ( *c++ = *d++ ) != '\0' ) { ; }
	
	return e ;	
}

char StringCharAt( StrHandle * st, int p)
{
	return st->string[p] ;
}

const char * StringFrom( StrHandle * st , int p)
{
	const char *c ;
	c = &(st->string[p]) ;
	return c ;
}

void StringDelete(StrHandle * st)
{	
	free( st->string ) ;
	free( st ) ;
}
