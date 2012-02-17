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
	stl->size = 1 ;

	stl->stp[0] = String( cstring ) ;
	
	if( stl->stp[0] == NULL )
	{
		free( stl->stp ) ;
		free( stl ) ;
		return NULL ;
	}
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
	stl->size = 1 ;
	
	stl->stp[0] = StringInheritWithSize( c,s ) ;
	
	if( stl->stp[0] == NULL )
	{
		free( stl->stp ) ;
		free( stl ) ;
		return NULL ;
	}
	return stl ;
	
}

stringList_t StringListAppendWithSize( stringList_t stl,char ** c, size_t s )
{
	if( stl == NULL )
		return StringListWithSize( c,s ) ;
	string_t * p = realloc( stl->stp,sizeof( struct StringType ) * ( stl->size + 1 ) ) ;	
	if( p == NULL )
		return NULL ;		
	stl->stp = p ;	
	stl->stp[ stl->size ] = StringInheritWithSize( c,s ) ;	
	stl->size = stl->size + 1 ;	
	return stl ;
}

stringList_t StringListAppendSize( stringList_t stl,const char * cstring,size_t len ) 
{
	char * c = ( char * )malloc( sizeof( char ) * ( len + 1 ) ) ;
	*( c + len ) = '\0' ;
	memcpy( c,cstring,len ) ;
	return StringListAppendWithSize( stl,&c,len ) ;
}

stringList_t StringListSplit( const char * cstring,const char * splitter ) 
{
	const char * b = cstring ;
	const char * c = splitter ;	
	char * d ;
	char * e ;
	
	size_t sp_len = strlen( splitter ) ;
	size_t len ; 
	stringList_t stl = NULL ;

	while( 1 )
	{
		d = strstr( b,c ) ;
		if( d == NULL )
		{			
			stl = StringListAppend( stl,b ) ;
			break ;
		}else{
			len = d - b ;
			e = ( char * ) malloc( sizeof( char ) * ( len + 1 ) ) ;
			memcpy( e,b,len ) ;
			*( e + len ) = '\0' ;
			stl = StringListAppendWithSize( stl,&e,len );
			b = d + sp_len ;
		}
	}
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
	size_t size = sizeof( struct StringType ) ;	
	p = realloc( stl->stp, size * ( stl->size + 1 ) ) ;	
	if( p == NULL )
		return NULL ;		
	stl->stp = p ;	
	memmove( stl->stp + index + 1,stl->stp + index,size * ( stl->size - index ) ) ;	
	stl->stp[index] = String( cstring ) ;	
	stl->size = stl->size + 1 ;	
	return stl ;	
}

stringList_t StringListInsertAtSize( stringList_t stl,const char * cstring,size_t len,size_t index ) 
{
	string_t * p ; 	
	size_t size = sizeof( struct StringType ) ;	
	char * c = ( char * ) malloc( sizeof( char ) * ( len + 1 ) ) ;
	memcpy( c,cstring,len );
	*( c + len ) = '\0' ;
	p = realloc( stl->stp, size * ( stl->size + 1 ) ) ;	
	if( p == NULL )
		return NULL ;		
	stl->stp = p ;	
	memmove( stl->stp + index + 1,stl->stp + index,size * ( stl->size - index ) ) ;	
	stl->stp[index] = StringInheritWithSize( &c,len ) ;	
	stl->size = stl->size + 1 ;	
	return stl ;	
}

stringList_t StringListPrependSize( stringList_t stl,const char * cstring,size_t len ) 
{
	char * c ;
	if( stl == NULL ){
		c = ( char * ) malloc( sizeof( char ) * ( len + 1 ) ) ;
		memcpy( c,cstring,len ) ;
		*( c + len ) = '\0' ;
		return StringListWithSize( &c,len ) ;
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
	if( stl == NULL )
		return StringList( cstring ) ;
	string_t * p = realloc( stl->stp,sizeof( struct StringType ) * ( stl->size + 1 ) ) ;	
	if( p == NULL )
		return NULL ;		
	stl->stp = p ;	
	stl->stp[ stl->size ] = String( cstring ) ;	
	stl->size = stl->size + 1 ;	
	return stl ;	
}

ssize_t StringListContains( stringList_t stl,const char * cstring )
{
	ssize_t index  ;
	for( index = 0 ; index < stl->size ; index++ )	
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
	size_t size  ;	
	stringList_t stx = *stl ;	
	*stl = NULL ;	
	size = stx->size ;	
	
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
		stx->stp[i] = StringCopy( stl->stp[i] ) ;
	return stx ;
}
