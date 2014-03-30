/*
 *
 *  Copyright (c) 2013
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

#include "includes.h"
#include "../lib/includes.h"

#include <blkid/blkid.h>
#include <sys/types.h>
#include <sys/stat.h>

stringList_t zuluCryptGetFstabList( uid_t uid )
{
	string_t xt = StringGetFromFile( "/etc/fstab" ) ;

	stringList_t stl = StringListVoid ;

	StringListIterator it  ;
	StringListIterator end ;

	ssize_t index ;

	char * ac ;
	const char * entry ;

	blkid_cache cache = NULL ;

	if( uid ){;}

	stl = StringListStringSplit( xt,'\n' ) ;

	StringDelete( &xt ) ;

	StringListRemoveIfStringStartsWith( stl,"#" ) ;
	StringListRemoveIfStringStartsWith( stl,"//" ) ;

	if( StringListSize( stl ) < 1 ){
		StringListDelete( &stl ) ;
		return StringListVoid ;
	}

	if( blkid_get_cache( &cache,NULL ) != 0 ){
		cache = NULL ;
	}

	StringListGetIteratorBeginAndEnd( stl,&it,&end ) ;

	while( it != end  ){
		xt = *it ;
		it++ ;
		index = StringIndexOfChar( xt,0,' ' ) ;
		if( index != -1 ){
			entry = StringSubChar( xt,index,'\0' ) ;
			if( StringPrefixMatch( entry,"/dev/",5 ) ){
				/*
				 * zuluCryptResolvePath() is defined in resolve_paths.c
				 */
				ac = zuluCryptResolvePath( entry ) ;
				StringSubChar( xt,index,' ' ) ;
				if( ac != NULL ){
					StringRemoveLeft( xt,index ) ;
					StringPrepend( xt,ac ) ;
					free( ac ) ;
				}
			}else if( StringAtLeastOnePrefixMatch( entry,"UUID=","uuid=",NULL ) ){
				entry = StringRemoveString( xt,"\"" ) ;
				ac = blkid_evaluate_tag( "UUID",entry + 5,&cache ) ;
				StringSubChar( xt,index,' ' ) ;
				if( ac != NULL ){
					StringRemoveLeft( xt,index ) ;
					StringPrepend( xt,ac ) ;
					free( ac ) ;
				}
			}else if( StringAtLeastOnePrefixMatch( entry,"LABEL=","label=",NULL ) ){
				entry = StringRemoveString( xt,"\"" ) ;
				ac = blkid_evaluate_tag( "LABEL",entry + 6,&cache ) ;
				StringSubChar( xt,index,' ' ) ;
				if( ac != NULL ){
					StringRemoveLeft( xt,index ) ;
					StringPrepend( xt,ac ) ;
					free( ac ) ;
				}
			}else{
				entry = StringSubChar( xt,index,' ' ) ;
			}
		}
	}

	if( cache != NULL ){
		blkid_put_cache( cache ) ;
	}
	return stl ;
}

string_t zuluCryptGetFstabEntry( const char * device,uid_t uid )
{
	string_t st = StringVoid ;
	string_t xt = String( device ) ;
	stringList_t stl = zuluCryptGetFstabList( uid ) ;
	ssize_t index = StringListHasStartSequence( stl,StringAppend( xt," " ) ) ;
	if( index >= 0 ){
		st = StringListCopyStringAt( stl,index ) ;
	}
	StringDelete( &xt ) ;
	StringListDelete( &stl ) ;
	return st ;
}

string_t zuluCryptGetMountOptionsFromFstab( const char * device,int pos,uid_t uid )
{
	stringList_t stl ;
	string_t st = zuluCryptGetFstabEntry( device,uid ) ;
	if( st != StringVoid ){
		stl = StringListStringSplit( st,' ' ) ;
		StringDelete( &st ) ;
		st = StringListCopyStringAt( stl,pos ) ;
		StringListDelete( &stl ) ;
	}
	return st ;
}

stringList_t zuluCryptGetFstabEntryList( const char * device,uid_t uid )
{
	stringList_t stl = StringListVoid;
	string_t st = zuluCryptGetFstabEntry( device,uid ) ;
	if( st != StringVoid ){
		stl = StringListStringSplit( st,' ' ) ;
		StringDelete( &st ) ;
	}
	return stl ;
}
