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

#include <stdio.h>
#include "StringList.h"

/*
 * buffer size will grow exponentially by a multiple of this number
 */
#define FACTOR 1.5

/*
 * initial buffer size
 */
#define INIT_SIZE 32

struct StringListType
{
	size_t size ;     /* size of the array*/
	size_t length ;   /* size of the buffer of thr array */
	string_t * stp ;  /* pointer to String_t array */
};

struct StringType
{
	size_t size ;
	size_t length ;
	char * string ; 
};

static void ( *__StringListErrorFunction__ )( void )  = NULL ;

void StringListExitOnMemoryExaustion( void ( *f )( void ) )
{
	__StringListErrorFunction__ = f ;
}

static stringList_t _StringListError( void )
{
	if( __StringListErrorFunction__ != NULL )
		( *__StringListErrorFunction__ )() ;
	
	return StringListVoid ;
}

void StringListForEach( stringList_t stl,void (*fct)( string_t ) ) 
{
	size_t i ;
	size_t j ;
	if( stl == StringListVoid )
		return ;
	j = stl->size ;
	for( i = 0 ; i < j ; i++ )
		fct( stl->stp[ i ] ) ;
}

void StringListForEachString( stringList_t stl,void (*fct)( const char * ) ) 
{
	size_t i ;
	size_t j ;
	if( stl == StringListVoid )
		return ;
	j = stl->size ;
	for( i = 0 ; i < j ; i++ )
		fct( stl->stp[ i ]->string ) ;
}

static inline string_t * __ExpandMemory( stringList_t stl )
{
	string_t * p ;
	if( stl->size >= stl->length )
	{
		stl->length = stl->length * FACTOR ;
		p = ( string_t * ) realloc( stl->stp,sizeof( string_t ) * ( stl->length ) ) ; 
		return p == NULL ? ( string_t * ) _StringListError() : p ;
	}else{
		return stl->stp ;
	}
}

stringList_t StringList( const char * cstring )
{
	stringList_t stl = ( stringList_t ) malloc( sizeof( struct StringListType ) ) ;
	if( stl == NULL )
		return _StringListError() ;
	
	stl->stp = ( string_t * ) malloc( sizeof( string_t ) * INIT_SIZE ) ;
	
	if( stl->stp == NULL )
	{
		free( stl ) ;
		return _StringListError() ;
	}	
	stl->stp[0] = String( cstring ) ;
	if( stl->stp[0] == StringVoid )
	{
		free( stl->stp );
		free( stl ) ;
		return _StringListError() ;
	}
	
	stl->size = 1 ;
	stl->length = INIT_SIZE ;
	return stl ;
}

StringListIterator StringListBegin( stringList_t stl )
{
	return &stl->stp[ 0 ] ;
}

StringListIterator StringListEnd( stringList_t stl ) 
{
	return &stl->stp[ stl->size ] ;
}

string_t * StringListAssign( stringList_t stl ) 
{
	string_t * p ;
	
	if( stl == StringListVoid )
		return StringVoid ;
	
	p = __ExpandMemory( stl ) ;
	
	if( p == StringListVoid )
		return StringVoid ;
	
	stl->stp = p ;
	stl->stp[ stl->size ] = StringVoid ;
	p = &stl->stp[ stl->size ]  ;
	
	stl->size = stl->size + 1 ;
	
	return p ;
}

string_t StringListAssignString( stringList_t stl,string_t st ) 
{
	string_t * p ;
	
	if( stl == StringListVoid )
		return st ;
	
	p = __ExpandMemory( stl ) ;
	
	if( p == StringListVoid )
		return StringListVoid ;
	
	stl->stp = p ;
	stl->stp[ stl->size ] = st;
	stl->size = stl->size + 1 ;
	return st ;
}

stringList_t StringListInit( void )
{	
	stringList_t stl = ( stringList_t ) malloc( sizeof( struct StringListType ) ) ;
	if( stl == NULL )
		return _StringListError() ;
	
	stl->stp = ( string_t * ) malloc( sizeof( string_t ) * INIT_SIZE ) ;
	
	if( stl->stp == NULL )
	{
		free( stl ) ;
		return _StringListError() ;
	}	
	
	stl->size = 0 ;
	stl->length = INIT_SIZE ;
	return stl ;
}

stringList_t StringListString( string_t * st ) 
{
	stringList_t stl = ( stringList_t ) malloc( sizeof( struct StringListType ) ) ;
	if( stl == NULL )
		return _StringListError() ;
	stl->stp = ( string_t * ) malloc( sizeof( string_t ) * INIT_SIZE ) ;
	if( stl->stp == NULL )
	{
		free( stl ) ;
		return _StringListError() ;
	}	
	stl->stp[0] = *st ;
	*st = StringVoid ;
	stl->size = 1 ;
	stl->length = INIT_SIZE ;
	return stl ;
}

stringList_t StringListWithSize( char ** c, size_t s )
{
	stringList_t stl = ( stringList_t ) malloc( sizeof( struct StringListType ) ) ;	
	if( stl == NULL )
		return _StringListError() ;
	stl->stp = ( string_t * ) malloc( sizeof( string_t ) * INIT_SIZE ) ;
	if( stl->stp == NULL )
	{
		free( stl ) ;
		return _StringListError() ;
	}	
	stl->stp[0] = StringInheritWithSize( c,s ) ;
	if( stl->stp[0] == StringVoid )
	{
		free( stl->stp );
		free( stl ) ;
		return _StringListError() ;
	}
	stl->size = 1 ;
	stl->length = INIT_SIZE ;
	return stl ;
}

stringList_t StringListAppendWithSize( stringList_t stl,char ** c, size_t s )
{
	string_t * p ;
	string_t q ;
	if( stl == StringListVoid )
		return StringListWithSize( c,s ) ;
	q = StringInheritWithSize( c,s ) ;
	if( q == StringVoid )
		return StringListVoid ;

	p = __ExpandMemory( stl ) ;
	
	if( p == StringListVoid )
	{
		StringDelete( &q ) ;
		return _StringListError() ;
	}
	stl->stp = p ;
	stl->stp[ stl->size ] = q ;
	stl->size = stl->size + 1 ;
	return stl ;
}

stringList_t StringListAppendSize( stringList_t stl,const char * cstring,size_t len ) 
{
	stringList_t p ;
	char * c = ( char * )malloc( sizeof( char ) * ( len + 1 ) ) ;
	if( c == NULL )
		return StringListVoid ;
	*( c + len ) = '\0' ;
	memcpy( c,cstring,len ) ;
	p = StringListAppendWithSize( stl,&c,len ) ;
	if( p == StringVoid )
	{
		free( c ) ;
		return _StringListError() ;
	}else
		return p ;
}

stringList_t StringListAppendList( stringList_t p,stringList_t q )
{
	size_t j ;
	size_t i ;
	string_t * z ;
	
	if( q == StringListVoid )
		return p ;
	
	if( p == StringListVoid )
		return StringListCopy( q ) ;
	
	j = StringListSize( q ) ;
	z = q->stp ;
	
	for( i = 0 ; i < j ; i++ )
		StringListAppendSize( p,z[i]->string,z[i]->size ) ;
	return p ;
}

stringList_t StringListAppendString( stringList_t stl,string_t st ) 
{
	return st == StringVoid ? stl : StringListAppendSize( stl,st->string,st->size ) ;
}

stringList_t StringListSplit( const char * cstring,char splitter ) 
{
	const char * b = cstring ;
	char * d ;
	char * e ;
	
	size_t sp_len = sizeof( char ) ;
	size_t len ; 
	stringList_t stl = StringListVoid ;
	stringList_t stx ;

	char s[ 2 ] ;
	s[ 1 ] = '\0' ;
	s[ 0 ] = splitter ;
	
	while( 1 ){
		
		d = strstr( b,s ) ;
		
		if( d == NULL ){
			if( *b != '\0' )
				if( b == cstring || *b != splitter )
					stl = StringListAppend( stl,b ) ;
			 break ;
		}else{
			len = d - b ;
			if( len > 0 ){
				e = ( char * ) malloc( sizeof( char ) * ( len + 1 ) ) ;
				if( e == NULL ){
					if( stl != StringListVoid )
						StringListDelete( &stl ) ;
					return StringListVoid;
				}
				memcpy( e,b,len ) ;
				*( e + len ) = '\0' ;
				stx = StringListAppendWithSize( stl,&e,len );
				if( stx == StringListVoid ){
					if( stl != StringListVoid )
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
	return st == StringVoid ? StringListVoid : StringListSplit( st->string,splitter ) ;
}

size_t StringListSize( stringList_t stl )
{
	return stl == StringListVoid ? 0 : stl->size ;
}

const char * StringListContentAt( stringList_t stl,size_t index )
{
	return index >= stl->size ? NULL : stl->stp[ index ]->string  ;
}

int StringListContentAtEqual( stringList_t stl,size_t index,const char * cstring )
{
	return index >= stl->size ? -1 : strcmp( stl->stp[ index ]->string,cstring ) ;
}

const char * StringListContentAtLast( stringList_t stl ) 
{
	return stl->size < 1 ? NULL : stl->stp[ stl->size - 1 ]->string  ;
}

string_t StringListStringAtLast( stringList_t stl ) 
{
	return stl->size < 1 ? StringVoid : stl->stp[ stl->size - 1 ] ;
}

stringList_t StringListInsertAt( stringList_t stl,const char * cstring,size_t index ) 
{
	string_t * p ;
	string_t q ;
	size_t size = sizeof( string_t ) ;
	
	if( index >= stl->size )
		return stl ;
	
	q = String( cstring ) ;
	if( q == StringVoid )
		return StringListVoid ;
	
	p = __ExpandMemory( stl ) ;
	
	if( p == StringListVoid )
	{
		StringDelete( &q ) ;
		return _StringListError() ;
	}
	stl->stp = p ;
	memmove( stl->stp + index + 1,stl->stp + index,size * ( stl->size - index ) ) ;
	stl->stp[index] = q ;
	stl->size = stl->size + 1 ;
	return stl ;	
}

stringList_t StringListStringInsertAt( stringList_t stl,string_t * st,size_t index ) 
{
	string_t * p ;
	size_t size = sizeof( string_t ) ;
	
	if( index >= stl->size )
		return stl ;
	
	p = __ExpandMemory( stl ) ;
	
	if( p == StringListVoid )
		return _StringListError() ;
	
	stl->stp = p ;
	memmove( stl->stp + index + 1,stl->stp + index,size * ( stl->size - index ) ) ;
	stl->stp[index] = *st ;
	stl->size = stl->size + 1 ;
	*st = StringListVoid ;
	return stl ;
}

stringList_t StringListPrependString( stringList_t stl,string_t * st )
{
	return StringListStringInsertAt( stl,st,0 ) ;
}

stringList_t StringListInsertAtSize( stringList_t stl,const char * cstring,size_t len,size_t index ) 
{
	char * c ;
	string_t * p ;
	string_t q ;
	size_t size = sizeof( string_t ) ;
	
	if( index >= stl->size )
		return stl ;
	
	c = ( char * ) malloc( sizeof( char ) * ( len + 1 ) ) ;
	if( c == NULL )
		return StringListVoid ;
	memcpy( c,cstring,len );
	*( c + len ) = '\0' ;
	q = StringInheritWithSize( &c,len ) ;
	if( q == StringVoid )
	{
		free( c ) ;
		return _StringListError() ;
	}
	
	p = __ExpandMemory( stl ) ;
	
	if( p == StringListVoid )
	{
		StringDelete( &q ) ;
		return StringListVoid ;
	}
	stl->stp = p ;
	memmove( stl->stp + index + 1,stl->stp + index,size * ( stl->size - index ) ) ;
	stl->stp[index] = q ; 
	stl->size = stl->size + 1 ;
	return stl ;	
}

stringList_t StringListPrependSize( stringList_t stl,const char * cstring,size_t len ) 
{
	char * c ;
	stringList_t p ;
	if( stl == StringListVoid ){
		c = ( char * ) malloc( sizeof( char ) * ( len + 1 ) ) ;
		if( c == NULL )
			return _StringListError() ;
		memcpy( c,cstring,len ) ;
		*( c + len ) = '\0' ;
		p = StringListWithSize( &c,len ) ;
		if( p == StringListVoid )
		{
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
	return stl == StringListVoid ? StringList( cstring ) : StringListInsertAt( stl,cstring,0 ) ;
}

stringList_t StringListAppend( stringList_t stl,const char * cstring ) 
{
	string_t q ;
	if( stl == StringListVoid )
		return StringList( cstring ) ;
	q = String( cstring ) ;
	if( q == StringVoid )
		return StringListVoid ;
	
	stl->stp = __ExpandMemory( stl ) ;
	
	if( stl->stp == StringListVoid )
	{
		StringDelete( &q ) ;
		return _StringListError() ;
	}
	
	stl->stp[ stl->size ] = q ;
	stl->size = stl->size + 1 ;
	
	return stl ;
}

ssize_t StringListContains( stringList_t stl,const char * cstring )
{
	size_t index  ;
	size_t size ;
	string_t * ind ;
	
	if( stl == StringListVoid )
		return -1 ;
	size = stl->size ;
	
	if( size == 0 )
		return -1 ;
	
	ind = stl->stp ;
	for( index = 0 ; index < size ; index++ )
		if( strcmp( ind[index]->string,cstring ) == 0 )
			return index ;
	return -1 ;
}

ssize_t StringListHasSequence( stringList_t stl,const char * str ) 
{	
	size_t index  ;
	size_t size ;
	string_t * ind ;
	
	if( stl == StringListVoid )
		return -1 ;
	size = stl->size ;
	
	if( size == 0 )
		return -1 ;
	ind = stl->stp ;
	
	for( index = 0 ; index < size ; index++ )
		if( strstr( ind[index]->string,str ) != NULL )
			return index ;
	return -1 ;
}

stringList_t StringListRemoveAt( stringList_t stl, size_t index ) 
{
	size_t size ;
	
	if( index >= stl->size )
		return StringListVoid ;
	
	size = sizeof( string_t ) ;
	StringDelete( &stl->stp[index] ) ;
	memmove( stl->stp + index,stl->stp + index + 1,size * ( stl->size - 1 - index ) ) ;
	stl->size = stl->size - 1 ;
	return stl ;
}

string_t StringListDetachAt( stringList_t stl, size_t index ) 
{
	string_t st;
	size_t  size ;
	
	if( index >= stl->size )
		return StringVoid ;
	
	st = stl->stp[index] ;
	size = sizeof( string_t ) ;
	
	memmove( stl->stp + index,stl->stp + index + 1,size * ( stl->size - 1 - index ) ) ;
	stl->size = stl->size - 1 ;
	return st ;
}

ssize_t StringListRemoveString( stringList_t stl,const char * cstring ) 
{
	ssize_t index = StringListContains( stl,cstring ) ;
	if( index == -1 )
		return -1 ;
	else{
		StringListRemoveAt( stl,index ) ;
		return index ;
	}
}

string_t StringListStringAt( stringList_t stl,size_t index ) 
{
	return index >= stl->size ? StringVoid : stl->stp[index] ;
}

void StringListDelete( stringList_t * stl ) 
{
	size_t index ;
	stringList_t stx ;
	size_t size ;
	string_t * entry ;
	string_t ent ;
	
	if( *stl == StringListVoid )
		return ;
	
	stx = *stl ;
	size  = stx->size ;
	*stl = StringListVoid ;
	
	entry = stx->stp ;
	
	if( size > 0 ){
		for( index = 0 ; index < size ; index++ ){
			ent = entry[ index ] ;
			if( ent != StringVoid ){
				free( ent->string ) ;
				free( ent ) ;
			}
		}
	}
	free( stx->stp ) ;
	free( stx );
}

void StringListClearDelete( stringList_t * stl ) 
{
	size_t index ;
	stringList_t stx ;
	size_t size ;
	string_t * entry ;
	string_t ent ;
	
	if( *stl == StringListVoid )
		return ;
	
	stx = *stl ;
	size  = stx->size ;
	*stl = StringListVoid ;
	
	entry = stx->stp ;
	
	if( size > 0 ){
		for( index = 0 ; index < size ; index++ ){
			ent = entry[ index ] ;
			
			if( ent != StringVoid ){ 
				memset( ent->string,'\0',ent->length ) ;
				free( ent->string ) ;
				free( ent ) ;
			}
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
		
		if( entry == END )
			break ;
		
		StringListDelete( entry ) ;
	}
	
	va_end( list ) ;
}

stringList_t StringListCopy( stringList_t stl ) 
{
	size_t i ;
	size_t j ;
	stringList_t stx ;
	if( stl == StringListVoid )
		return StringListVoid ;
	j = stl->size ;
	stx = ( stringList_t ) malloc( sizeof( struct StringListType ) ) ;
	if( stx == NULL )
		return _StringListError() ;
	stx->stp = ( string_t * ) malloc( sizeof( string_t ) * j ) ;
	if( stx->stp == NULL )
	{
		free( stx ) ;
		return _StringListError() ;
	}
	stx->size = stl->size ;
	for( i = 0 ; i < j ; i++)
		stx->stp[i] = StringWithSize( stl->stp[i]->string,stl->stp[i]->size ) ;
	return stx ;
}

string_t StringListCopyStringAt( stringList_t stl,size_t pos ) 
{
	if( stl == StringListVoid )
		return StringVoid ;
	if( pos >= stl->size )
		return StringVoid ;
	return StringCopy( stl->stp[ pos ] ) ;
}

stringList_t StringListSwap( stringList_t stl, size_t x,size_t y ) 
{
	string_t p ;
	if( stl == StringListVoid )
		return StringListVoid ;
	if( x >= stl->size )
		return StringListVoid ;
	if( y >= stl->size )
		return StringListVoid ;
	
	p = stl->stp[ x ] ;
	stl->stp[ x ] = stl->stp[ y ] ;
	stl->stp[ y ] = p ;
	return stl ;
}

void StringListPrintAt( stringList_t stl,size_t index )
{
	if( stl != StringListVoid )
		if( index < stl->size )
			printf("%s",stl->stp[ index ]->string ) ;
}

void StringListPrintLineAt( stringList_t stl,size_t index ) 
{
	if( stl != StringListVoid )
		if( index < stl->size )
			printf("%s\n",stl->stp[ index ]->string ) ;
}

void StringListPrintList( stringList_t stl )
{
	size_t i ;
	size_t j ;
	
	if( stl == StringListVoid )
		return ;
	j = stl->size ;
	for( i = 0 ; i < j ; i++ )
		printf("%s\n",stl->stp[ i ]->string ) ;
}

