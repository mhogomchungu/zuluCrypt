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

#include "includes.h"

stringList_t zuluCryptGetMoutedListFromMountInfo( void )
{
	const char * device ;
	const char * mount_point ;
	const char * file_system ;
	const char * mount_options ;

	char * const * entry = NULL ;

	size_t entry_len = 0 ;
	int index ;

	stringList_t tmp ;
	stringList_t stx = StringListVoid;
	stringList_t stl ;

	StringListIterator it  ;
	StringListIterator end ;

	string_t st = StringGetFromVirtualFile( "/proc/self/mountinfo" ) ;

	stl = StringListStringSplit( st,'\n' ) ;

	StringDelete( &st ) ;

	StringListGetIteratorBeginAndEnd( stl,&it,&end ) ;

	while( it != end ){
		tmp = StringListStringSplit( *it,' ' ) ;
		it++ ;
		if( StringListContentAtEqual( tmp,3,"/" ) ){
			index = StringListContains( tmp,"-" ) ;
			if( index != -1 ){
				entry         = StringListStringArray_1( entry,&entry_len,tmp ) ;
				device        = *( entry + index + 2 ) ;
				mount_point   = *( entry + 4 ) ;
				file_system   = *( entry + index + 1 ) ;
				mount_options = *( entry + 5 ) ;
				/*
				 * zuluCryptResolvePath_1() is defined in resolve_paths.c
				 */
				st = zuluCryptResolvePath_1( device ) ;

				StringMultipleAppend( st," ",mount_point," ",file_system," ",mount_options,END ) ;

				stx = StringListAppendString_1( stx,&st ) ;
			}
		}
		StringListDelete( &tmp ) ;
	}

	StringFree( entry ) ;
	StringListDelete( &stl ) ;
	return stx ;
}

stringList_t zuluCryptGetMountInfoList( void )
{
	return zuluCryptGetMoutedListFromMountInfo() ;
}

stringList_t zuluCryptOpenedVolumesList( uid_t uid )
{
	const char * e ;
	const char * c ;
	const char * d ;

	char * f ;
	char * g ;

	StringListIterator it  ;
	StringListIterator end ;

	ssize_t k ;

	string_t q ;
	string_t z ;

	stringList_t stx ;
	stringList_t list = StringListVoid ;
	stringList_t stl = zuluCryptGetMountInfoList() ;

	if( uid ){;}

	StringListGetIteratorBeginAndEnd( stl,&it,&end ) ;

	while( it != end ){
		c = StringContent( *it ) ;
		it++ ;
		if( !StringPrefixMatch( c,"/dev/mapper/zuluCrypt-",22 ) ){
			/*
			 * dont care about other volumes
			 */
			continue ;
		}
		if( StringHasComponent( c,"/run/media/public/" ) ){
			/*
			 * dont show mirror images due to bind mounts
			 */
			continue ;
		}

		stx = StringListSplit( c,' ' ) ;

		e = StringListContentAtFirstPlace( stx ) ;

		k = StringHasComponent_1( e,"-UUID-" ) ;
		if( k != -1 && StringHasNoComponent( e,"-NAAN-" ) ) {
			q = StringListStringAtFirstPlace( stx ) ;
			/*
			 * zuluCryptDecodeMountEntry() is defined in mount_volume.c
			 */
			d = zuluCryptDecodeMountEntry( StringListStringAtSecondPlace( stx ) ) ;

			/*
			 * zuluCryptGetVolumeTypeFromMapperPath() is defined in status.c
			 */
			f = zuluCryptGetVolumeTypeFromMapperPath( StringContent( q ) ) ;
			e = StringSubChar( q,StringLastIndexOfChar( q,'-' ),'\0' ) + k + 6 ;
			z = String( "" ) ;
			StringMultipleAppend( z,"UUID=\"",e,"\"\t",d,"\t",f,END ) ;
			list = StringListAppendString_1( list,&z ) ;
			StringFree( f ) ;
		}else{
			/*
			 * zuluCryptVolumeDeviceName() is defined in status.c
			 */
			g = zuluCryptVolumeDeviceName( e ) ;
			if( g != NULL ){
				d = zuluCryptDecodeMountEntry( StringListStringAtSecondPlace( stx ) ) ;
				/*
				 * zuluCryptGetVolumeTypeFromMapperPath() is defined in status.c
				 */
				f = zuluCryptGetVolumeTypeFromMapperPath( StringListContentAtFirstPlace( stx ) ) ;
				z = String( "" ) ;
				StringMultipleAppend( z,g,"\t",d,"\t",f,END ) ;
				list = StringListAppendString_1( list,&z ) ;
				StringFree( f ) ;
				StringFree( g ) ;
			}
		}

		StringListDelete( &stx ) ;
	}
	StringListDelete( &stl ) ;
	return list ;
}

string_t zuluCryptGetMountEntry( const char * path )
{
	stringList_t stl = zuluCryptGetMountInfoList() ;
	string_t st = zuluCryptGetMountEntry_1( stl,path ) ;
	StringListDelete( &stl ) ;
	return st ;
}

string_t zuluCryptGetMountEntry_1( stringList_t stl,const char * path )
{
	string_t st ;
	string_t xt ;

	if( stl == StringListVoid ){
		return StringVoid ;
	}else{
		/*
		 * zuluCryptResolvePath_1() is defined in resolve_paths.c
		 */
		st = zuluCryptResolvePath_1( path ) ;

		xt = StringListHasStartSequence_1( stl,StringAppend( st," " ) ) ;

		StringDelete( &st ) ;

		return StringCopy( xt ) ;
	}
}

char * zuluCryptGetMountPointFromPath( const char * path )
{
	string_t st = zuluCryptGetMountEntry( path ) ;
	stringList_t stl ;

	if( st == StringVoid ){
		return NULL ;
	}else{
		stl = StringListStringSplit( st,' ' ) ;
		StringDelete( &st ) ;
		if( stl == StringListVoid ){
			return NULL ;
		}else{
			st = StringListCopyStringAtSecondPlace( stl ) ;
			StringListDelete( &stl ) ;
			zuluCryptDecodeMountEntry( st ) ;
			return StringDeleteHandle( &st ) ;
		}
	}
	/*
	 * shouldnt get here
	 */
	return NULL ;
}

int zuluCryptPartitionIsMounted( const char * path )
{
	char * p = zuluCryptGetMountPointFromPath( path ) ;
	if( p == NULL ){
		return 0 ;
	}else{
		free( p ) ;
		return 1 ;
	}
}
