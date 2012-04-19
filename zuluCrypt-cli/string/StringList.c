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

stringList_t StringList( const char * cstring )
{
	stringList_t stl = ( stringList_t ) malloc( sizeof( struct StringListType ) ) ;	
	if( stl == NULL )
		return NULL ;
	stl->stp = ( string_t * ) malloc ( sizeof ( struct StringType ) ) ;
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
	return stl ;
}

stringList_t StringListString( string_t * st ) 
{
	stringList_t stl = ( stringList_t ) malloc( sizeof( struct StringListType ) ) ;	
	if( stl == NULL )
		return NULL ;
	stl->stp = ( string_t * ) malloc ( sizeof ( struct StringType ) ) ;
	if( stl->stp == NULL )
	{
		free( stl ) ;
		return NULL ;
	}	
	stl->stp[0] = *st ;
	*st = NULL ;
	if( stl->stp[0] == NULL )
	{
		free( stl->stp );
		free( stl ) ;
		return NULL ;
	}
	stl->size = 1 ;
	return stl ;
}

stringList_t StringListWithSize( char ** c, size_t s )
{
	stringList_t stl = ( stringList_t ) malloc( sizeof( struct StringListType ) ) ;	
	if( stl == NULL )
		return NULL ;
	stl->stp = ( string_t * ) malloc ( sizeof ( struct StringType ) ) ;
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
	p = realloc( stl->stp,sizeof( struct StringType ) * ( stl->size + 1 ) ) ;
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
	
	p = realloc( stl->stp,sizeof( struct StringType ) * ( stl->size + 1 ) ) ;
	
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
	char s[ 2 ] ;
	s[ 1 ] = '\0' ;
	s[ 0 ] = splitter ;
	
	size_t sp_len = sizeof( char ) ;
	size_t len ; 
	stringList_t stl = NULL ;

	if( strstr( b,s ) == NULL )
		return StringList( cstring ) ;
	
	while( ( d = strstr( b,s ) ) != NULL )
	{
		len = d - b ;
		if( len > 0 ){
			e = ( char * ) malloc( sizeof( char ) * ( len + 1 ) ) ;
			if( e == NULL )
				return NULL;
			memcpy( e,b,len ) ;
			*( e + len ) = '\0' ;
			stl = StringListAppendWithSize( stl,&e,len );
			if( stl == NULL )
			{
				free( e ) ;
				return NULL ;
			}
		}
		b = d + sp_len ;
	}
	return stl ;
}

stringList_t StringListStringSplit( string_t * st,char splitter ) 
{
	string_t xt = *st ;
	stringList_t stl = StringListSplit( xt->string,splitter ) ;	
	StringDelete( st ) ;
	return stl ;
}

size_t StringListSize( stringList_t stl )
{
	return stl->size ;
}

const char * StringListContentAt( stringList_t stl,size_t index )
{
	return StringContent( stl->stp[index] ) ;	
}

stringList_t StringListInsertAt( stringList_t stl,const char * cstring,size_t index ) 
{
	string_t * p ;
	string_t q ;
	size_t size = sizeof( struct StringType ) ;	
	q = String( cstring ) ;
	if( q == NULL )
		return NULL ;
	p = realloc( stl->stp, size * ( stl->size + 1 ) ) ;
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
	size_t size = sizeof( struct StringType ) ;
	
	if( stl == NULL )
		return StringListString( st ) ;		

	p = realloc( stl->stp, size * ( stl->size + 1 ) ) ;
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
	size_t size = sizeof( struct StringType ) ;	
	
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
	p = realloc( stl->stp, size * ( stl->size + 1 ) ) ;
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
	stl->stp = realloc( stl->stp,sizeof( struct StringType ) * ( stl->size + 1 ) ) ;
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
	size_t size = sizeof( struct StringType ) ;	
	StringDelete( &stl->stp[index] ) ;		
	memmove( stl->stp + index,stl->stp + index + 1,size * ( stl->size - 1 - index ) ) ;	
	stl->stp = realloc( stl->stp,size * ( stl->size - 1 ) ) ;	
	stl->size = stl->size - 1 ;
	return stl ;
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
	stx->stp = ( string_t * ) malloc( sizeof( struct StringType ) * j ) ;
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
