/*
 *
 *  Copyright (c) 2011-2015
 *  name : Francis Banyikwa
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

#include <stdio.h>
#include "StringList.h"

/*
 * buffer size will grow exponentially by a multiple of this number
 */
#define FACTOR 2

/*
 * initial buffer size
 */
#define INIT_SIZE 8

struct StringListType
{
	size_t size ;     /* size of the array*/
	size_t length ;   /* size of the buffer of thr array */
	string_t * stp ;  /* pointer to String_t array */
};

struct StringType
{
	char * string ;
	size_t size ;
	size_t length ;
	int owned ;
};

static void _f( void )
{
}

static void ( *_memory_error )( void )  = _f ;

void StringListExitOnMemoryExaustion( void ( *f )( void ) )
{
	_memory_error = f ;
}

static void * _StringListError( void )
{
	_memory_error() ;
	return NULL ;
}

void StringListForEach( stringList_t stl,void (*fct)( string_t ) )
{
	size_t i ;
	size_t j ;
	string_t * q ;
	string_t p ;

	if( stl != StringListVoid ){
		j = stl->size ;
		q = stl->stp ;
		for( i = 0 ; i < j ; i++ ){
			p = *( q + i ) ;
			if( p != StringVoid ){
				fct( p ) ;
			}
		}
	}
}

void StringListForEach_1( stringList_t stl,void (*fct)( string_t,void * ),void * arg )
{
	size_t i ;
	size_t j ;
	string_t * q ;
	string_t p ;

	if( stl != StringListVoid ){
		j = stl->size ;
		q = stl->stp ;
		for( i = 0 ; i < j ; i++ ){
			p = *( q + i ) ;
			if( p != StringVoid ){
				fct( p,arg ) ;
			}
		}
	}
}

void StringListForEachString( stringList_t stl,void (*fct)( const char * ) )
{
	size_t i ;
	size_t j ;
	string_t * q ;
	string_t p ;

	if( stl != StringListVoid ){
		j = stl->size ;
		q = stl->stp ;
		for( i = 0 ; i < j ; i++ ){
			p = *( q + i ) ;
			if( p != StringVoid ){
				fct( p->string ) ;
			}
		}
	}
}

void StringListForEachString_1( stringList_t stl,void (*fct)( const char *,void * ),void * arg )
{
	size_t i ;
	size_t j ;
	string_t * q ;
	string_t p ;

	if( stl != StringListVoid ){
		j = stl->size ;
		q = stl->stp ;
		for( i = 0 ; i < j ; i++ ){
			p = *( q + i ) ;
			if( p!= StringVoid ){
				fct( p->string,arg ) ;
			}
		}
	}
}

static inline string_t * __ExpandMemory( stringList_t stl )
{
	string_t * p ;
	if( stl->size >= stl->length ){
		stl->length = stl->length * FACTOR ;
		p = realloc( stl->stp,sizeof( string_t ) * ( stl->length ) ) ;
		if( p == NULL ){
			return _StringListError() ;
		}else{
			return p ;
		}
	}else{
		return stl->stp ;
	}
}

stringList_t StringList( const char * cstring )
{
	stringList_t stl ;

	if( cstring == NULL ){
		return StringListVoid ;
	}

	stl = malloc( sizeof( struct StringListType ) ) ;

	if( stl == NULL ){
		return _StringListError() ;
	}

	stl->stp = malloc( sizeof( string_t ) * INIT_SIZE ) ;

	if( stl->stp == NULL ){
		free( stl ) ;
		return _StringListError() ;
	}

	stl->stp[0] = String( cstring ) ;

	if( stl->stp[0] == StringVoid ){
		free( stl->stp ) ;
		free( stl ) ;
		return _StringListError() ;
	}else{
		stl->stp[0]->owned = 1 ;
		stl->size = 1 ;
		stl->length = INIT_SIZE ;
		return stl ;
	}
}

StringListIterator StringListBegin( stringList_t stl )
{
	if( stl == StringListVoid ){
		return NULL ;
	}else{
		return stl->stp ;
	}
}

StringListIterator StringListEnd( stringList_t stl )
{
	if( stl == StringListVoid ){
		return NULL ;
	}else{
		return stl->stp + stl->size ;
	}
}

void StringListGetIterators( stringList_t stl,StringListIterator * begin,StringListIterator * end )
{
	if( stl == StringListVoid ){
		*begin = NULL ;
		*end   = NULL ;
	}else{
		*begin = stl->stp;
		*end   = stl->stp + stl->size ;
	}
}

string_t * StringListAssign( stringList_t stl )
{
	string_t * p ;

	if( stl == StringListVoid ){
		return NULL ;
	}

	p = __ExpandMemory( stl ) ;

	if( p == NULL ){
		return _StringListError() ;
	}else{
		stl->stp = p ;
		stl->stp[ stl->size ] = StringVoid ;
		p = &stl->stp[ stl->size ] ;

		stl->size = stl->size + 1 ;

		return p ;
	}
}

string_t * StringListArray( stringList_t * stz,size_t arraySize )
{
	stringList_t stl = malloc( sizeof( struct StringListType ) ) ;

	if( stl == NULL ){
		return _StringListError() ;
	}

	stl->stp = malloc( sizeof( string_t ) * arraySize ) ;

	if( stl->stp == NULL ){
		free( stl ) ;
		return _StringListError() ;
	}

	stl->size   = arraySize ;
	stl->length = arraySize ;

	memset( stl->stp,'\0',sizeof( string_t ) * arraySize ) ;

	*stz = stl ;
	return stl->stp ;
}

string_t StringListAssignString( stringList_t stl,string_t st )
{
	string_t * p ;

	if( stl == StringListVoid ){
		return st ;
	}

	p = __ExpandMemory( stl ) ;

	if( p == NULL ){
		return _StringListError() ;
	}else{
		stl->stp = p ;

		if( st != NULL ){
			st->owned = 1 ;
		}

		stl->stp[ stl->size ] = st ;
		stl->size = stl->size + 1 ;
		return st ;
	}
}

stringList_t StringListInit( void )
{
	stringList_t stl = malloc( sizeof( struct StringListType ) ) ;

	if( stl == NULL ){
		return _StringListError() ;
	}

	stl->stp = malloc( sizeof( string_t ) * INIT_SIZE ) ;

	if( stl->stp == NULL ){
		free( stl ) ;
		return _StringListError() ;
	}else{
		stl->size = 0 ;
		stl->length = INIT_SIZE ;
		return stl ;
	}
}

stringList_t StringListWithSize( char ** c, size_t s,size_t t )
{
	stringList_t stl ;
	if( c == NULL ){
		return StringListVoid ;
	}
	stl = malloc( sizeof( struct StringListType ) ) ;
	if( stl == NULL ){
		return _StringListError() ;
	}
	stl->stp = malloc( sizeof( string_t ) * INIT_SIZE ) ;
	if( stl->stp == NULL ){
		free( stl ) ;
		return _StringListError() ;
	}
	stl->stp[0] = StringInheritWithSize( c,s,t ) ;
	if( stl->stp[0] == StringVoid ){
		free( stl->stp ) ;
		free( stl ) ;
		return _StringListError() ;
	}else{
		stl->stp[0]->owned = 1 ;
		stl->size = 1 ;
		stl->length = INIT_SIZE ;
		return stl ;
	}
}

stringList_t StringListAppendWithSize( stringList_t stl,char ** c, size_t s,size_t l )
{
	string_t * p ;
	string_t q ;
	if( stl == StringListVoid ){
		return StringListWithSize( c,s,l ) ;
	}

	q = StringInheritWithSize( c,s,l ) ;

	if( q == StringVoid ){
		return stl ;
	}
	p = __ExpandMemory( stl ) ;

	if( p == NULL ){
		StringDelete( &q ) ;
		return _StringListError() ;
	}else{
		stl->stp = p ;
		q->owned = 1 ;
		stl->stp[ stl->size ] = q ;
		stl->size = stl->size + 1 ;
		return stl ;
	}
}

stringList_t StringListAppendSize( stringList_t stl,const char * cstring,size_t len )
{
	stringList_t p ;
	char * c ;

	if( cstring == NULL ){
		return stl ;
	}

	c = malloc( sizeof( char ) * ( len + 1 ) ) ;

	if( c == NULL ){
		return _StringListError() ;
	}
	memcpy( c,cstring,len ) ;
	*( c + len ) = '\0' ;
	p = StringListAppendWithSize( stl,&c,len,len + 1 ) ;
	if( p == StringListVoid ){
		free( c ) ;
		return _StringListError() ;
	}else{
		return p ;
	}
}

stringList_t StringListAppendList( stringList_t p,stringList_t q )
{
	size_t j ;
	size_t i ;
	string_t * z ;
	string_t k ;

	if( q == StringListVoid ){
		return p ;
	}
	if( p == StringListVoid ){
		return StringListCopy( q ) ;
	}

	j = StringListSize( q ) ;
	z = q->stp ;

	for( i = 0 ; i < j ; i++ ){
		k = *( z + i ) ;
		if( k != StringVoid ){
			StringListAppendSize( p,k->string,k->size ) ;
		}
	}
	return p ;
}

stringList_t StringListAppendString( stringList_t stl,string_t st )
{
	if( st == StringVoid ){
		return stl ;
	}else{
		return StringListAppendSize( stl,st->string,st->size ) ;
	}
}

stringList_t StringListSplit( const char * cstring,char splitter )
{
	const char * b = cstring ;
	char * d ;
	char * e ;
	char s[ 2 ] ;

	size_t sp_len = sizeof( char ) ;
	size_t len ;

	stringList_t stl = StringListVoid ;
	stringList_t stx ;

	if( cstring == NULL ){
		return StringListVoid ;
	}

	s[ 1 ] = '\0' ;
	s[ 0 ] = splitter ;

	while( 1 ){
		d = strstr( b,s ) ;
		if( d == NULL ){
			if( *b != '\0' ){
				if( b == cstring || *b != splitter ){
					stl = StringListAppend( stl,b ) ;
				}
			}
			break ;
		}else{
			len = d - b ;
			if( len > 0 ){
				e = malloc( sizeof( char ) * ( len + 1 ) ) ;
				if( e == NULL ){
					StringListDelete( &stl ) ;
					return _StringListError() ;
				}
				memcpy( e,b,len ) ;
				*( e + len ) = '\0' ;
				stx = StringListAppendWithSize( stl,&e,len,len + 1 ) ;
				if( stx == StringListVoid ){
					StringListDelete( &stl ) ;
					free( e ) ;
					return _StringListError() ;
				}else{
					stl = stx ;
				}
			}
			b = d + sp_len ;
		}
	}

	return stl ;
}

stringList_t StringListStringSplit( string_t st,char splitter )
{
	if( st == StringVoid ){
		return StringListVoid ;
	}else{
		return StringListSplit( st->string,splitter ) ;
	}
}

size_t StringListSize( stringList_t stl )
{
	if( stl == StringListVoid ){
		return 0 ;
	}else{
		return stl->size ;
	}
}

const char * StringListContentAt( stringList_t stl,size_t index )
{
	if( stl == StringListVoid ){
		return NULL ;
	}else{
		if( index >= stl->size ){
			return NULL ;
		}else{
			return stl->stp[ index ]->string ;
		}
	}
}

int StringListContentAtEqual( stringList_t stl,size_t index,const char * cstring )
{
	if( stl == StringListVoid || cstring == NULL ){
		return 0 ;
	}else{
		if( index >= stl->size ){
			return 0 ;
		}else{
			return strcmp( stl->stp[ index ]->string,cstring ) == 0 ;
		}
	}
}

const char * StringListContentAtLast( stringList_t stl )
{
	if( stl == StringListVoid ){
		return NULL ;
	}else{
		if( stl->size < 1 ){
			return NULL ;
		}else{
			return stl->stp[ stl->size - 1 ]->string ;
		}
	}
}

string_t StringListStringAtLast( stringList_t stl )
{
	if( stl == StringListVoid ){
		return StringVoid ;
	}else{
		if( stl->size < 1 ){
			return StringVoid ;
		}else{
			return stl->stp[ stl->size - 1 ] ;
		}
	}
}

stringList_t StringListInsertAt( stringList_t stl,const char * cstring,size_t index )
{
	string_t * p ;
	string_t q ;
	size_t size = sizeof( string_t ) ;

	if( stl == StringListVoid ){
		return StringListVoid ;
	}
	if( cstring == NULL ){
		return stl ;
	}
	if( index > stl->size ){
		return stl ;
	}

	q = String( cstring ) ;

	if( q == StringVoid ){
		return _StringListError() ;
	}

	p = __ExpandMemory( stl ) ;

	if( p == NULL ){
		StringDelete( &q ) ;
		return _StringListError() ;
	}else{
		stl->stp = p ;
		memmove( stl->stp + index + 1,stl->stp + index,size * ( stl->size - index ) ) ;
		q->owned = 1 ;
		stl->stp[index] = q ;
		stl->size = stl->size + 1 ;
		return stl ;
	}
}

static int _StringListStringInsertAt( stringList_t stl,string_t * st,size_t index )
{
	string_t * p ;
	size_t size = sizeof( string_t ) ;

	if( stl == StringListVoid ){
		return 0 ;
	}
	if( index > stl->size ){
		return 0 ;
	}

	p = __ExpandMemory( stl ) ;

	if( p == NULL ){
		_StringListError() ;
		return 0 ;
	}else{
		stl->stp = p ;
		memmove( stl->stp + index + 1,stl->stp + index,size * ( stl->size - index ) ) ;
		stl->stp[index] = *st ;
		if( stl->stp[index] != StringVoid ){
			stl->stp[index]->owned = 1 ;
		}
		stl->size = stl->size + 1 ;
		return 1 ;
	}
}

static int _StringListString( string_t * st,stringList_t * r )
{
	stringList_t stl = malloc( sizeof( struct StringListType ) ) ;

	if( stl == NULL ){
		_StringListError() ;
		return 0 ;
	}
	stl->stp = malloc( sizeof( string_t ) * INIT_SIZE ) ;
	if( stl->stp == NULL ){
		free( stl ) ;
		_StringListError() ;
		return 0 ;
	}
	if( *st != StringVoid ){
		stl->stp[ 0 ] = *st ;
		stl->stp[ 0 ]->owned = 1 ;
	}else{
		stl->stp[ 0 ] = StringVoid ;
	}

	stl->size = 1 ;
	stl->length = INIT_SIZE ;

	*r = stl ;
	return 1 ;
}

stringList_t StringListString( string_t * st )
{
	stringList_t stl = StringListVoid ;

	if( st && *st ){
		if( _StringListString( st,&stl ) ){
			*st = StringVoid ;
		}
	}
	return stl ;
}

void StringListAppendString_1( stringList_t * stl,string_t * st )
{
	if( st && *st && stl ){
		if( *stl == StringListVoid ){
			if( _StringListString( st,stl ) ){
				*st = StringVoid ;
			}
		}else{
			if( _StringListStringInsertAt( *stl,st,( *stl )->size ) ){
				*st = StringVoid ;
			}
		}
	}
}

stringList_t StringListStringInsertAt( stringList_t stl,string_t * st,size_t index )
{
	if( st && *st ){
		if( _StringListStringInsertAt( stl,st,index ) ){
			*st = StringVoid ;
		}
	}
	return stl ;
}

stringList_t StringListInsertAtSize( stringList_t stl,const char * cstring,size_t len,size_t index )
{
	char * c ;
	string_t * p ;
	string_t q ;
	size_t size = sizeof( string_t ) ;

	if( stl == StringListVoid || cstring == NULL ){
		return stl ;
	}
	if( index > stl->size ){
		return stl ;
	}

	c = malloc( sizeof( char ) * ( len + 1 ) ) ;

	if( c == NULL ){
		return _StringListError() ;
	}

	memcpy( c,cstring,len );
	*( c + len ) = '\0' ;
	q = StringInheritWithSize( &c,len,len + 1 ) ;

	if( q == StringVoid ){
		free( c ) ;
		return _StringListError() ;
	}

	p = __ExpandMemory( stl ) ;

	if( p == NULL ){
		StringDelete( &q ) ;
		return _StringListError() ;
	}else{
		stl->stp = p ;
		memmove( stl->stp + index + 1,stl->stp + index,size * ( stl->size - index ) ) ;
		q->owned = 1 ;
		stl->stp[index] = q ;
		stl->size = stl->size + 1 ;
		return stl ;
	}
}

stringList_t StringListPrependSize( stringList_t stl,const char * cstring,size_t len )
{
	char * c ;
	stringList_t p ;
	if( cstring == NULL ){
		return stl ;
	}
	if( stl == StringListVoid ){
		c = malloc( sizeof( char ) * ( len + 1 ) ) ;
		if( c == NULL ){
			return _StringListError() ;
		}
		memcpy( c,cstring,len ) ;
		*( c + len ) = '\0' ;
		p = StringListWithSize( &c,len,len + 1 ) ;
		if( p == StringListVoid ){
			free( c ) ;
			return _StringListError() ;
		}else{
			return p ;
		}
	}else{
		return StringListInsertAtSize( stl,cstring,len,0 ) ;
	}
}

stringList_t StringListPrepend( stringList_t stl,const char * cstring )
{
	if( stl == StringListVoid ){
		return StringList( cstring ) ;
	}else{
		return StringListInsertAt( stl,cstring,0 ) ;
	}
}

stringList_t StringListAppend( stringList_t stl,const char * cstring )
{
	string_t q ;

	if( stl == StringListVoid ){
		return StringList( cstring ) ;
	}

	if( cstring == NULL ){
		return stl ;
	}

	q = String( cstring ) ;

	if( q == StringVoid ){
		return _StringListError() ;
	}

	stl->stp = __ExpandMemory( stl ) ;

	if( stl->stp == NULL ){
		StringDelete( &q ) ;
		return _StringListError() ;
	}else{
		q->owned = 1 ;
		stl->stp[ stl->size ] = q ;
		stl->size = stl->size + 1 ;
		return stl ;
	}
}

stringList_t StringListAppendIfAbsent( stringList_t stl,const char * cstring )
{
	if( stl == StringListVoid ){
		return StringList( cstring ) ;
	}else{
		if( StringListContains( stl,cstring ) == -1 ){
			StringListAppend( stl,cstring ) ;
		}
		return stl ;
	}
}

ssize_t StringListContains( stringList_t stl,const char * cstring )
{
	size_t i  ;
	size_t size ;
	string_t * k ;
	string_t e ;

	if( stl == StringListVoid || cstring == NULL ){
		return -1 ;
	}

	size = stl->size ;

	k = stl->stp ;

	for( i = 0 ; i < size ; i++ ){
		e = *( k + i ) ;
		if( e != StringVoid ){
			if( strcmp( e->string,cstring ) == 0 ){
				return i ;
			}
		}
	}
	return -1 ;
}

ssize_t StringListHasSequence( stringList_t stl,const char * str )
{
	size_t i  ;
	size_t size ;
	string_t * k ;
	string_t e ;

	if( stl == StringListVoid || str == NULL ){
		return -1 ;
	}

	size = stl->size ;
	k = stl->stp ;

	for( i = 0 ; i < size ; i++ ){
		e = *( k + i ) ;
		if( e != StringVoid ){
			if( strstr( e->string,str ) != NULL ){
				return i ;
			}
		}
	}

	return -1 ;
}

string_t StringListHasSequence_1( stringList_t stl,const char * str )
{
	ssize_t e = StringListHasSequence( stl,str ) ;

	if( e == -1 ){
		return StringVoid ;
	}else{
		return *( stl->stp + e ) ;
	}
}

ssize_t StringListHasStartSequence( stringList_t stl,const char * str )
{
	size_t i  ;
	size_t size ;
	string_t * k ;
	string_t e ;
	size_t len  ;

	if( stl == StringListVoid || str == NULL ){
		return -1 ;
	}

	len = strlen( str ) ;
	size = stl->size ;
	k = stl->stp ;

	for( i = 0 ; i < size ; i++ ){
		e = *( k + i ) ;
		if( e != StringVoid ){
			if( strncmp( e->string,str,len ) == 0 ){
				return i ;
			}
		}
	}

	return -1 ;
}

string_t StringListHasStartSequence_1( stringList_t stl,const char * str )
{
	ssize_t e = StringListHasStartSequence( stl,str ) ;

	if( e == -1 ){
		return StringVoid ;
	}else{
		return *( stl->stp + e ) ;
	}
}

const char ** StringListStringArray( stringList_t stl )
{
	size_t i ;
	size_t j ;
	const char ** q ;
	string_t * p ;
	string_t z ;

	if( stl == StringListVoid ){
		q = malloc( sizeof( char * ) ) ;
		if( q == NULL ){
			return _StringListError() ;
		}else{
			*q = NULL ;
		}
	}else{
		j = stl->size ;
		p = stl->stp ;
		q = malloc( sizeof( char * ) * ( j + 1 ) ) ;
		if( q == NULL ){
			return _StringListError() ;
		}
		*( q + j ) = NULL ;
		for( i = 0 ; i < j ; i++ ){
			z = *( p + i ) ;
			if( z != StringVoid ){
				*( q + i ) = z->string ;
			}else{
				*( q + i ) = NULL ;
			}
		}
	}
	return q ;
}

void StringListStringArray_1( char * const ** buffer,size_t * size ,stringList_t stl )
{
	size_t i ;
	size_t j ;
	string_t * p ;
	string_t z ;

	char ** e ;

	if( buffer == NULL || size == NULL || stl == StringListVoid ){
		return ;
	}

	e = ( char ** ) *buffer ;

	j = stl->size ;
	p = stl->stp ;

	if( *size < j ){
		e = realloc( e,sizeof( char * ) * ( j + 1 ) ) ;
		if( e == NULL ){
			_StringListError() ;
			*size = 0 ;
		}else{
			*size = j ;
			*( e + j ) = NULL ;
		}
	}

	for( i = 0 ; i < j ; i++ ){
		z = *( p + i ) ;
		if( z != StringVoid ){
			*( e + i ) = z->string ;
		}else{
			*( e + i ) = NULL ;
		}
	}

	*buffer = e ;
}

size_t StringListRemoveIfStringStartsWith( stringList_t stl,const char * str )
{
	string_t z ;
	string_t * it  ;
	string_t * end ;
	string_t * result  ;
	size_t count = 0 ;
	size_t len ;

	if( stl == StringListVoid || str == NULL ){
		return 0 ;
	}

	it     = stl->stp ;
	end    = it + stl->size ;
	result = it ;
	count  = 0 ;
	len    = strlen( str ) ;

	while( it != end ){
		z = *it ;
		it++ ;
		if( z != StringVoid ){
			if( strncmp( z->string,str,len ) == 0 ){
				free( z->string ) ;
				free( z ) ;
				count++ ;
				stl->size-- ;
			}else{
				*result = z ;
				result++ ;
			}
		}else{
			*result = z ;
			result++ ;
		}
	}

	return count ;
}

size_t StringListRemoveIfPresent_1( stringList_t stl,string_t st )
{
	return StringListRemoveIfPresent( stl,st->string ) ;
}

size_t StringListRemoveIfPresent( stringList_t stl,const char * str )
{
	string_t z ;
	string_t * it  ;
	string_t * end ;
	string_t * result  ;
	size_t count = 0 ;

	if( stl == StringListVoid || str == NULL ){
		return 0 ;
	}

	it     = stl->stp ;
	end    = it + stl->size ;
	result = it ;
	count  = 0 ;

	while( it != end ){
		z = *it ;
		it++ ;
		if( z != StringVoid ){
			if( strcmp( z->string,str ) == 0 ){
				free( z->string ) ;
				free( z ) ;
				count++ ;
				stl->size-- ;
			}else{
				*result = z ;
				result++ ;
			}
		}else{
			*result = z ;
			result++ ;
		}
	}

	return count ;
}


size_t StringListRemoveIfStringContains( stringList_t stl,const char * str )
{
	string_t z ;
	string_t * it  ;
	string_t * end ;
	string_t * result  ;
	size_t count = 0 ;

	if( stl == StringListVoid || str == NULL ){
		return 0 ;
	}

	it     = stl->stp ;
	end    = it + stl->size ;
	result = it ;
	count  = 0 ;

	while( it != end ){
		z = *it ;
		it++ ;
		if( z != StringVoid ){
			if( strstr( z->string,str ) != NULL ){
				free( z->string ) ;
				free( z ) ;
				count++ ;
				stl->size-- ;
			}else{
				*result = z ;
				result++ ;
			}
		}else{
			*result = z ;
			result++ ;
		}
	}

	return count ;
}

stringList_t StringListRemoveAt( stringList_t stl, size_t index )
{
	size_t size ;

	if( stl == StringListVoid ){
		return StringListVoid ;
	}
	if( index >= stl->size ){
		return stl ;
	}
	size = sizeof( string_t ) ;
	if( stl->stp[index] != StringVoid ){
		stl->stp[index]->owned = 0 ;
	}
	StringDelete( &stl->stp[index] ) ;
	memmove( stl->stp + index,stl->stp + index + 1,size * ( stl->size - 1 - index ) ) ;
	stl->size = stl->size - 1 ;
	return stl ;
}

void StringListRemoveAt_1( stringList_t stl,StringListIterator at,StringListIterator * end )
{
	StringListRemoveAt( stl,at - stl->stp ) ;
	*end = *end - 1 ;
}

string_t StringListDetachAt( stringList_t stl, size_t index )
{
	string_t st;
	size_t  size ;

	if( stl == StringListVoid ){
		return StringVoid ;
	}
	if( index >= stl->size ){
		return StringVoid ;
	}

	st = stl->stp[index] ;
	size = sizeof( string_t ) ;

	memmove( stl->stp + index,stl->stp + index + 1,size * ( stl->size - 1 - index ) ) ;
	stl->size = stl->size - 1 ;

	if( st != StringVoid ){
		st->owned = 0 ;
	}

	return st ;
}

ssize_t StringListRemoveString( stringList_t stl,const char * cstring )
{
	ssize_t index = StringListContains( stl,cstring ) ;
	if( index == -1 ){
		return -1 ;
	}else{
		StringListRemoveAt( stl,index ) ;
		return index ;
	}
}

string_t StringListStringAt( stringList_t stl,size_t index )
{
	if( stl == StringListVoid ){
		return StringVoid ;
	}else{
		if( index >= stl->size ){
			return StringVoid ;
		}else{
			return stl->stp[index] ;
		}
	}
}

void StringListDelete( stringList_t * stl )
{
	size_t i ;
	stringList_t stx ;
	size_t size ;
	string_t * k ;
	string_t e ;

	if( stl == NULL ){
		return ;
	}
	if( *stl == StringListVoid ){
		return ;
	}

	stx = *stl ;
	size  = stx->size ;
	*stl = StringListVoid ;

	k = stx->stp ;

	for( i = 0 ; i < size ; i++ ){
		e = *( k + i ) ;
		if( e != StringVoid ){
			free( e->string ) ;
			free( e ) ;
		}
	}
	free( stx->stp ) ;
	free( stx );
}

void StringListClearDelete( stringList_t * stl )
{
	size_t i ;
	stringList_t stx ;
	size_t size ;
	string_t * k ;
	string_t e ;

	if( stl == NULL ){
		return ;
	}
	if( *stl == StringListVoid ){
		return ;
	}

	stx = *stl ;
	size  = stx->size ;
	*stl = StringListVoid ;

	k = stx->stp ;

	for( i = 0 ; i < size ; i++ ){
		e = *( k + i ) ;
		if( e != StringVoid ){
			memset( e->string,'\0',e->length ) ;
			free( e->string ) ;
			free( e ) ;
		}
	}
	free( stx->stp ) ;
	free( stx );
}

void StringListMultipleDelete( stringList_t * stl,... )
{
	stringList_t * entry ;
	va_list list ;
	va_start( list,stl ) ;

	StringListDelete( stl ) ;

	while( 1 ){
		entry = va_arg( list,stringList_t * ) ;

		if( entry == NULL ){
			break ;
		}
		StringListDelete( entry ) ;
	}

	va_end( list ) ;
}

stringList_t StringListCopy( stringList_t stl )
{
	size_t i ;
	size_t j ;

	string_t * st ;
	string_t * xt ;

	string_t st_1 ;

	stringList_t stx ;
	if( stl == StringListVoid ){
		return StringListVoid ;
	}
	j = stl->size ;
	stx = malloc( sizeof( struct StringListType ) ) ;
	if( stx == NULL ){
		return _StringListError() ;
	}
	stx->stp = malloc( sizeof( string_t ) * j ) ;
	if( stx->stp == NULL ){
		free( stx ) ;
		return _StringListError() ;
	}
	stx->size = stl->size ;
	xt = stx->stp ;
	st = stl->stp ;
	for( i = 0 ; i < j ; i++ ){
		st_1 = *( st + i ) ;
		if( st_1 == StringVoid ){
			xt[ i ] = StringVoid ;
		}else{
			xt[ i ] = StringWithSize( st_1->string,st_1->size ) ;
			xt[ i ]->owned = 1 ;
		}
	}
	return stx ;
}

string_t StringListCopyStringAt( stringList_t stl,size_t pos )
{
	if( stl == StringListVoid ){
		return StringVoid ;
	}
	if( pos >= stl->size ){
		return StringVoid ;
	}
	return StringCopy( *( stl->stp + pos ) ) ;
}

stringList_t StringListSwap( stringList_t stl, size_t x,size_t y )
{
	string_t p ;

	if( stl == StringListVoid || x >= stl->size || y >= stl->size ){
		return stl ;
	}else{
		p = stl->stp[ x ] ;
		stl->stp[ x ] = stl->stp[ y ] ;
		stl->stp[ y ] = p ;

		return stl ;
	}
}

void StringListPrintAt( stringList_t stl,size_t index )
{
	if( stl != StringListVoid ){
		if( index < stl->size ){
			printf("%s",stl->stp[ index ]->string ) ;
		}
	}
}

void StringListPrintLineAt( stringList_t stl,size_t index )
{
	if( stl != StringListVoid ){
		if( index < stl->size ){
			printf("%s\n",stl->stp[ index ]->string ) ;
		}
	}
}

void StringListPrintList( stringList_t stl )
{
	size_t i ;
	size_t j ;
	string_t * p ;
	string_t q ;

	if( stl == StringListVoid ){
		return ;
	}

	j = stl->size ;
	p = stl->stp ;

	for( i = 0 ; i < j ; i++ ){
		q = *( p + i ) ;
		if( q != StringVoid ){
			printf("%s\n",q->string ) ;
		}
	}
}

