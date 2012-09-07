/*
 * 
 *  Copyright (c) 2012
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

#include "StringManage.h"

struct StringManage_t
{
	size_t index ;
	size_t size ;
	string_t * stp ;
};

int StringManageMaxSize( stringManage_t stm ) 
{
	return stm->size ;
}

ssize_t StringManageSize( stringManage_t stm ) 
{
	if( stm == StringManageVoid )
		return -1 ;
	else
		return stm->index ;
}

string_t StringManageStringAt( stringManage_t stm,size_t index )
{
	if( stm == StringManageVoid )
		return StringVoid ;
	if( index < 0 || index >= stm->size )
		return StringVoid ;
	
	return stm->stp[ index ] ;
}

stringManage_t StringManage( size_t size ) 
{	
	stringManage_t stm = ( stringManage_t ) malloc( sizeof( struct StringManage_t ) ) ;
	
	if( stm == NULL )
		return StringManageVoid ;
	
	stm->stp = ( string_t * ) malloc( sizeof( string_t ) * size ) ;
	
	if( stm->stp == NULL ){
		free( stm ) ;
		return StringManageVoid ;
	}
	
	stm->index = 0 ;	
	stm->size = size ;
	
	return stm ;	
}

string_t * StringManageAssign( stringManage_t stm ) 
{
	string_t * p ;
	
	if( stm->index == stm->size )
		return StringVoid ;
	
	p =  &stm->stp[ stm->index ] ;
	*p = StringVoid ;
	stm->index = stm->index + 1 ;
	return p ;
}

void StringManageDelete( stringManage_t stm ) 
{
	size_t i ;
	size_t j ;
	
	j = stm->index ;
	
	if( j > 0 )
		for( i = 0 ; i < j ; i++ )
			StringDelete( &stm->stp[ i ] ) ;		
	
	free( stm->stp ) ;
	free( stm ) ;
}

void StringManageClearDelete( stringManage_t stm ) 
{
	size_t i ;
	size_t j ;
	
	j = stm->index ;
	
	if( j > 0 )
		for( i = 0 ; i < j ; i++ )
			StringClearDelete( &stm->stp[ i ] ) ;		
		
	free( stm->stp ) ;
	free( stm ) ;
}

string_t * StringManageString( void ) 
{
	string_t * st = ( string_t * )malloc( sizeof( string_t ) ) ;	
	*st = StringVoid ;
	return st ;
}

void StringManageStringDelete( string_t * st ) 
{	
	StringDelete( st ) ;
	free( st ) ;
}

