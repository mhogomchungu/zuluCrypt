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

#include "includes.h"

typedef struct{
	const char * device ;
	const char * mountPoint ;
	const char * fileSystem ;
	const char * mountOptions ;
	const char * rootPath ;
}vInfo ;

static int _valid_entry( const vInfo * e )
{
	if( StringAtLeastOnePrefixMatch( e->mountPoint,"/var/run/media/public",
					    "/var/run/media/private",NULL ) ){
		/*
		 * some distributions auto generate these extra mount points and we
		 * ignore them as they confuse us.
		 */
		return 0 ;
	}

	if( StringsAreEqual( e->rootPath,"/" ) || StringsAreEqual( e->fileSystem,"btrfs" ) ){
		/*
		 * we only support bind mount on btrfs.
		 */
		return 1 ;
	}

	return 0 ;
}

static stringList_t _volumeList( string_t ( *function )( const vInfo * ) )
{
	vInfo volumeInfo ;

	char * const * entry = NULL ;

	size_t entry_len = 0 ;
	u_int64_t e ;

	stringList_t tmp ;
	stringList_t stx = StringListVoid ;
	stringList_t stl ;

	StringListIterator it  ;
	StringListIterator end ;

	string_t st = StringGetFromVirtualFile( "/proc/self/mountinfo" ) ;

	stl = StringListStringSplit( st,'\n' ) ;

	StringDelete( &st ) ;

	StringListGetIterators( stl,&it,&end ) ;

	while( it != end ){

		tmp = StringListStringSplit( *it,' ' ) ;

		it++ ;

		StringListStringArray_1( &entry,&entry_len,tmp ) ;

		volumeInfo.device       = *( entry + entry_len - 2 ) ;
		volumeInfo.mountPoint   = *( entry + 4 ) ;
		volumeInfo.fileSystem   = *( entry + entry_len - 3 ) ;
		volumeInfo.mountOptions = *( entry + 5 ) ;
		volumeInfo.rootPath     = *( entry + 3 ) ;

		if( StringsAreEqual( volumeInfo.fileSystem,"fuse.encfs" ) ){

			st = StringListStringAt( tmp,entry_len - 2 ) ;
			StringReset( st ) ;
			e = StringJenkinsOneAtATimeHash( volumeInfo.mountPoint ) ;
			volumeInfo.device = StringAppendInt( st,e ) ;
		}

		if( _valid_entry( &volumeInfo ) ){

			st = function( &volumeInfo ) ;
			stx = StringListAppendString_1( stx,&st ) ;
		}

		StringListDelete( &tmp ) ;
	}

	StringFree( entry ) ;
	StringListDelete( &stl ) ;
	return stx ;
}

static string_t _resolve_path_1( const vInfo * e )
{
	/*
	 * zuluCryptResolvePath_1() is defined in resolve_paths.c
	 */
	string_t st = zuluCryptResolvePath_1( e->device ) ;
	StringMultipleAppend( st," ",e->mountPoint," ",e->fileSystem," ",e->mountOptions,NULL ) ;
	return st ;
}

stringList_t zuluCryptGetMoutedList( void )
{
	return _volumeList( _resolve_path_1 ) ;
}

static string_t _resolve_path_2( const vInfo * e )
{
	/*
	 * zuluCryptResolvePath_2() is defined in resolve_paths.c
	 */
	string_t st = zuluCryptResolvePath_2( e->device ) ;
	StringMultipleAppend( st," ",e->mountPoint," ",e->fileSystem," ",e->mountOptions,NULL ) ;
	return st ;
}

stringList_t zuluCryptGetMoutedList_1( void )
{
	return _volumeList( _resolve_path_2 ) ;
}

static string_t _get_mounted_device_list( const vInfo * e )
{
	return zuluCryptResolvePath_1( e->device ) ;
}

stringList_t zuluCryptGetAListOfMountedVolumes( void )
{
	return _volumeList( _get_mounted_device_list ) ;
}

static int _mounted( ssize_t( *function )( stringList_t,const char * ),string_t st )
{
	stringList_t stl = zuluCryptGetMoutedList() ;

	ssize_t r = function( stl,StringContent( st ) ) ;

	StringListDelete( &stl ) ;

	StringDelete( &st ) ;

	return r != -1 ;
}

int zuluCryptMountPointIsActive( const char * m_point )
{
	return _mounted( StringListHasSequence,String_1( " ",m_point," ",NULL ) ) ;
}

int zuluCryptPartitionIsMounted( const char * path )
{
	return _mounted( StringListHasStartSequence,String_1( path," ",NULL ) ) ;
}

stringList_t zuluCryptOpenedVolumesList( uid_t uid )
{
	const char * e ;
	const char * c ;
	const char * d ;
	const char * t ;

	char * f ;
	char * g ;

	StringListIterator it  ;
	StringListIterator end ;

	ssize_t k ;

	string_t q ;
	string_t z ;

	string_t j ;

	stringList_t stx ;
	stringList_t list = StringListVoid ;
	stringList_t stl = zuluCryptGetMoutedList() ;

	if( uid ){;}

	/*
	 * zuluCryptMapperPrefix() is defined in create_mapper_name.c
	 */
	j = String_1( zuluCryptMapperPrefix(),"/zuluCrypt-",NULL ) ;
	/*
	 * t will probably contain "/dev/mapper/zuluCrypt-"
	 */
	t = StringContent( j ) ;

	StringListGetIterators( stl,&it,&end ) ;

	while( it != end ){

		c = StringContent( *it ) ;

		it++ ;

		if( StringPrefixNotEqual( c,t ) ){

			/*
			 * we only care about zuluCrypt volumes and these volumes that we care about starts with
			 * "/dev/mapper/zuluCrypt-"
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

		if( k != -1 ) {

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
			z = String_1( "UUID=\"",e,"\"\t",d,"\t",f,NULL ) ;
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
				z = String_1( g,"\t",d,"\t",f,NULL ) ;
				list = StringListAppendString_1( list,&z ) ;
				StringFree( f ) ;
				StringFree( g ) ;
			}
		}

		StringListDelete( &stx ) ;
	}
	StringListDelete( &stl ) ;
	StringDelete( &j ) ;
	return list ;
}

string_t zuluCryptGetMountEntry( const char * path )
{
	stringList_t stl = zuluCryptGetMoutedList() ;
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
}
