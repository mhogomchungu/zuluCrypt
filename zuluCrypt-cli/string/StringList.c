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
#define FACTOR 2

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

static string_t * __ExpandMemory( stringList_t stl )
{
	string_t * p = stl->stp ;
	if( stl->size + 1 > stl->length )
	{
		stl->length *= FACTOR ;
		p = realloc( p,sizeof( string_t ) * ( stl->length ) ) ; 		
	}
	return p ;
}

stringList_t StringList( const char * cstring )
{
	stringList_t stl = ( stringList_t ) malloc( sizeof( struct StringListType ) ) ;	
	if( stl == NULL )
		return NULL ;
	
	stl->stp = ( string_t * ) malloc( sizeof( string_t ) * INIT_SIZE ) ;
	
	if( stl->stp == NULL )
	{
		free( stl ) ;
		return NULL ;
	}	
	stl->stp[0] = String( cstring ) ;
	if( stl->stp[0] == NULL )
	{
		free( stl->stp );
		free( stl ) ;
		return NULL ;
	}
	
	stl->size = 1 ;
	stl->length = INIT_SIZE ;
	return stl ;
}

stringList_t StringListString( string_t * st ) 
{
	stringList_t stl = ( stringList_t ) malloc( sizeof( struct StringListType ) ) ;	
	if( stl == NULL )
		return NULL ;
	stl->stp = ( string_t * ) malloc( sizeof( string_t ) * INIT_SIZE ) ;
	if( stl->stp == NULL )
	{
		free( stl ) ;
		return NULL ;
	}	
	stl->stp[0] = *st ;
	*st = NULL ;	
	stl->size = 1 ;
	stl->length = INIT_SIZE ;
	return stl ;
}

stringList_t StringListWithSize( char ** c, size_t s )
{
	stringList_t stl = ( stringList_t ) malloc( sizeof( struct StringListType ) ) ;	
	if( stl == NULL )
		return NULL ;
	stl->stp = ( string_t * ) malloc( sizeof( string_t ) * INIT_SIZE ) ;
	if( stl->stp == NULL )
	{
		free( stl ) ;
		return NULL ;
	}	
	stl->stp[0] = StringInheritWithSize( c,s ) ;
	if( stl->stp[0] == NULL )
	{
		free( stl->stp );
		free( stl ) ;
		return NULL ;
	}
	stl->size = 1 ;
	stl->length = INIT_SIZE ;
	return stl ;
}

stringList_t StringListAppendWithSize( stringList_t stl,char ** c, size_t s )
{
	string_t * p ;
	string_t q ;
	if( stl == NULL )
		return StringListWithSize( c,s ) ;
	q = StringInheritWithSize( c,s ) ;
	if( q == NULL )
		return NULL ;

	p = __ExpandMemory( stl ) ;
	
	if( p == NULL )
	{
		StringDelete( &q ) ;
		return NULL ;
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
		return NULL ;
	*( c + len ) = '\0' ;
	memcpy( c,cstring,len ) ;
	p = StringListAppendWithSize( stl,&c,len ) ;
	if( p == NULL )
	{
		free( c ) ;
		return NULL ;
	}else
		return p ;
}

stringList_t StringListAppendString( stringList_t stl,string_t * st ) 
{
	string_t * p ;
	
	if( stl == NULL )
		return StringListString( st ) ;
	
	p = __ExpandMemory( stl ) ;
	
	if( p == NULL )
		return NULL ;
	
	stl->stp = p ;
	stl->stp[ stl->size ] = *st ;
	*st = NULL ;
	stl->size = stl->size + 1 ;	
	return stl ;
	
}

stringList_t StringListSplit( const char * cstring,char splitter ) 
{
	const char * b = cstring ;
	char * d ;
	char * e ;
	
	size_t sp_len = sizeof( char ) ;
	size_t len ; 
	stringList_t stl = NULL ;
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
					if( stl != NULL )
						StringListDelete( &stl ) ;
					return NULL;
				}
				memcpy( e,b,len ) ;
				*( e + len ) = '\0' ;
				stx = StringListAppendWithSize( stl,&e,len );
				if( stx == NULL ){
					if( stl != NULL )
						StringListDelete( &stl ) ;
					free( e ) ;
					return NULL ;
				}else{
					stl = stx ;
				}
				
			}
			b = d + sp_len ;
		}
	}

	return stl ;
}

stringList_t StringListStringSplit( string_t * st,char splitter ) 
{
	string_t xt = *st ;
	stringList_t stl = StringListSplit( xt->string,splitter ) ;
	if( stl != NULL )
		StringDelete( st ) ;
	return stl ;
}

size_t StringListSize( stringList_t stl )
{
	return stl->size ;
}

const char * StringListContentAt( stringList_t stl,size_t index )
{
	return stl->stp[index]->string  ;	
}

const char * StringListContentAtLast( stringList_t stl ) 
{
	return stl->stp[ stl->size - 1 ]->string  ;
}

string_t StringListStringAtLast( stringList_t stl ) 
{
	return stl->stp[ stl->size - 1 ] ;
}

stringList_t StringListInsertAt( stringList_t stl,const char * cstring,size_t index ) 
{
	string_t * p ;
	string_t q ;
	size_t size = sizeof( string_t ) ;	
	q = String( cstring ) ;
	if( q == NULL )
		return NULL ;
	
	p = __ExpandMemory( stl ) ;
	
	if( p == NULL )
	{
		StringDelete( &q ) ;
		return NULL ;
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
	
	if( stl == NULL )
		return StringListString( st ) ;		

	p = __ExpandMemory( stl ) ;
	
	if( p == NULL )
	{
		return NULL ;
	}
	stl->stp = p ;
	memmove( stl->stp + index + 1,stl->stp + index,size * ( stl->size - index ) ) ;	
	stl->stp[index] = *st ;	
	stl->size = stl->size + 1 ;
	*st = NULL ;
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
	
	c = ( char * ) malloc( sizeof( char ) * ( len + 1 ) ) ;
	if( c == NULL )
		return NULL ;
	memcpy( c,cstring,len );
	*( c + len ) = '\0' ;
	q = StringInheritWithSize( &c,len ) ;
	if( q == NULL )
	{
		free( c ) ;
		return NULL ;
	}
	
	p = __ExpandMemory( stl ) ;
	
	if( p == NULL )
	{
		StringDelete( &q ) ;
		return NULL ;
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
	if( stl == NULL ){
		c = ( char * ) malloc( sizeof( char ) * ( len + 1 ) ) ;
		if( c == NULL )
			return NULL ;
		memcpy( c,cstring,len ) ;
		*( c + len ) = '\0' ;
		p = StringListWithSize( &c,len ) ;
		if( p == NULL )
		{
			free( c ) ;
			return NULL ;
		}else
			return p ;
	}
	else
		return StringListInsertAtSize( stl,cstring,len,0 ) ;
}

stringList_t StringListPrepend( stringList_t stl,const char * cstring ) 
{
	if( stl == NULL )
		return StringList( cstring ) ;
	else
		return StringListInsertAt( stl,cstring,0 ) ;
}

stringList_t StringListAppend( stringList_t stl,const char * cstring ) 
{
	string_t q ;
	if( stl == NULL )
		return StringList( cstring ) ;
	q = String( cstring ) ;
	if( q == NULL )
		return NULL ;
	
	stl->stp = __ExpandMemory( stl ) ;
	
	if( stl->stp == NULL )
	{
		StringDelete( &q ) ;
		return NULL ;
	}
	
	stl->stp[ stl->size ] = q ;	
	stl->size = stl->size + 1 ;
	
	return stl ;	
}

ssize_t StringListContains( stringList_t stl,const char * cstring )
{
	ssize_t index  ;
	size_t size = stl->size ;
	for( index = 0 ; index < size ; index++ )	
		if( StringCompareString( stl->stp[index],cstring ) == 0 )
			return index ;	
	return -1 ;
}

stringList_t StringListRemoveAt( stringList_t stl, size_t index ) 
{
	size_t size = sizeof( string_t ) ;	
	StringDelete( &stl->stp[index] ) ;		
	memmove( stl->stp + index,stl->stp + index + 1,size * ( stl->size - 1 - index ) ) ;	
	stl->size = stl->size - 1 ;
	return stl ;
}

string_t StringListDetachAt( stringList_t stl, size_t index ) 
{
	string_t st = stl->stp[index] ;
	size_t size = sizeof( string_t ) ;	
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
	return stl->stp[index] ;
}

void StringListDelete( stringList_t * stl ) 
{
	size_t index ;	
	stringList_t stx = *stl ;
	size_t size  = stx->size ;
	*stl = NULL ;	
	for( index = 0 ; index < size ; index++ )
		StringDelete( &stx->stp[index] ) ;
	free( stx->stp ) ;	
	free( stx );	
}

stringList_t StringListCopy( stringList_t stl ) 
{
	size_t i ;
	size_t j = stl->size ;
	stringList_t stx = ( stringList_t ) malloc( sizeof( struct StringListType ) ) ;
	if( stx == NULL )
		return NULL ;
	stx->stp = ( string_t * ) malloc( sizeof( string_t ) * j ) ;
	if( stx->stp == NULL )
	{
		free( stx ) ;
		return NULL ;
	}
	stx->size = stl->size ;	
	for( i = 0 ; i < j ; i++)
		stx->stp[i] = StringWithSize( stl->stp[i]->string,stl->stp[i]->size ) ;
	return stx ;
}

stringList_t StringListSwap( stringList_t stl, size_t x,size_t y ) 
{
	string_t p = stl->stp[ x ] ;
	stl->stp[ x ] = stl->stp[ y ] ;
	stl->stp[ y ] = p ;
	return stl ;
}

void StringListPrintAt( stringList_t stl,size_t index )
{
	printf("%s",stl->stp[ index ]->string ) ;
}

void StringListPrintLineAt( stringList_t stl,size_t index ) 
{
	printf("%s\n",stl->stp[ index ]->string ) ;	
}
