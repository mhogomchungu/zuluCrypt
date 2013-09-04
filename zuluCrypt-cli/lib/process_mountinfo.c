 
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
#include <dirent.h>
#include <sys/stat.h>

/*
 * below header file does not ship with the source code, it is created at configure time 
 */
#include "libmount_header.h"

char * zuluCryptResolveDevRoot( void )
{
	const char * e ;
	char * dev       = NULL ;
	
	string_t st      = StringGetFromVirtualFile( "/proc/cmdline" ) ;
	stringList_t stl = StringListStringSplit( st,' ' ) ;
	StringDelete( &st ) ;
	
	st = StringListHasSequence_1( stl,"root=/dev/" ) ;
	if( st != StringVoid ){
		e = StringRemoveString( st,"root=" ) ;
		if( StringPrefixMatch( e,"/dev/disk/by-",13 ) ){
			/*
			 * zuluCryptRealPath() is defined in ./real_path.c
			 */
			dev = zuluCryptRealPath( e ) ;
		}else{
			dev = StringDeleteHandle( &st ) ;
		}
	}else{
		st = StringListHasSequence_1( stl,"root=UUID=" ) ;
		if( st != StringVoid ){
			/*
			 * zuluCryptDeviceFromUUID() is defined in ./blkid_evaluate_tag.c
			 */
			dev = zuluCryptDeviceFromUUID( StringRemoveString( st,"root=UUID=" ) ) ;
		}
	}
	StringListDelete( &stl ) ;
	return dev ;
}

/*
 * raid path can be in format /dev/mdX or /dev/md/X.
 * We prefer the latter and if given the former,convert it to the latter if possible
 */
string_t zuluCryptResolveMDPath_1( const char * path )
{
	const char * f = "/dev/md/" ;
	DIR * dir = opendir( f ) ;
	string_t st = String( f ) ;
	struct dirent * entry ;
	char * e ;
	int r ;
	if( dir != NULL ){
		while( ( entry = readdir( dir ) ) != NULL ){
			f = entry->d_name ;
			if( StringAtLeastOneMatch_1( f,".","..",NULL ) ){
				;
			}else{
				e = zuluCryptRealPath( StringAppendAt( st,8,f ) ) ;
				if( e != NULL ){
					r = StringsAreEqual( path,e ) ;
					free( e ) ;
					if( r == 1 ){
						closedir( dir ) ;
						return st ;
					}
				}
			}
		}
		closedir( dir ) ;
	}
	
	StringAppendAt( st,0,path ) ;
	return st ;
}

char * zuluCryptResolveMDPath( const char * path )
{
	string_t st = zuluCryptResolveMDPath_1( path ) ;
	return StringDeleteHandle( &st ) ;
}

stringList_t zuluCryptGetMoutedListFromMountInfo( void )
{
	const char * device ;
	const char * mount_point ;
	const char * file_system ;
	const char * mount_options ;
	const char * g ;
	char * dev ;
	int index ;
	char * const * entry = NULL ;
	size_t entry_len = 0 ;
	struct stat str ;
	stringList_t tmp ;
	stringList_t stx = StringListVoid;
	stringList_t stl ;
	StringListIterator it ;
	StringListIterator end;
	string_t n ;
	string_t st = StringGetFromVirtualFile( "/proc/self/mountinfo" ) ;
	
	stl = StringListStringSplit( st,'\n' ) ;
	StringDelete( &st ) ;
	if( stl == StringListVoid ){
		return StringListVoid ;
	}
	it  = StringListBegin( stl ) ;
	end = StringListEnd( stl )   ;
	st = String( "" ) ;
	n  = String( "" ) ;
	for( ; it != end ; it++ ){
		tmp = StringListStringSplit( *it,' ' ) ;
		if( !StringListContentAtEqual( tmp,3,"/" ) ){
			StringListDelete( &tmp ) ;
			continue ;
		}
		index = StringListContains( tmp,"-" ) ;
		if( index == -1 ){
			StringListDelete( &tmp ) ;
			continue ;
		}
		entry         = StringListStringArray_1( entry,&entry_len,tmp ) ;
		device        = entry[ index+2 ] ;
		mount_point   = entry[ 4 ] ;
		file_system   = entry[ index+1 ] ;
		mount_options = entry[ 5 ] ;
		if( StringPrefixMatch( device,"/dev/loop",9 ) ){
			/*
			 * zuluCryptLoopDeviceAddress() is defined in ./create_loop_device.c
			 */
			dev = zuluCryptLoopDeviceAddress( device ) ;
			if( dev == NULL ){
				StringMultipleAppend( st,device," ",mount_point," ",file_system," ",mount_options,END ) ;
			}else{
				StringMultipleAppend( st,dev," ",mount_point," ",file_system," ",mount_options,END ) ;
				free( dev ) ;
			}
		}else if( StringsAreEqual( device,"/dev/root" ) ){
			dev = zuluCryptResolveDevRoot() ;
			if( dev == NULL ){
				StringMultipleAppend( st,device," ",mount_point," ",file_system," ",mount_options,END ) ;
			}else{
				StringMultipleAppend( st,dev," ",mount_point," ",file_system," ",mount_options,END ) ;
				free( dev ) ;
			}
		}else if( StringPrefixMatch( device,"/dev/disk/by-",13 ) ){
			/*
			 * zuluCryptRealPath() is defined in ./real_path.c
			 */
			dev = zuluCryptRealPath( device ) ;
			if( dev == NULL ){
				StringMultipleAppend( st,device," ",mount_point," ",file_system," ",mount_options,END ) ;
			}else{
				StringMultipleAppend( st,dev," ",mount_point," ",file_system," ",mount_options,END ) ;
				free( dev ) ;
			}
		}else if( StringPrefixMatch( device,"/dev/mapper/",12 ) ){
			/*
			 * If the entry is an LVM volume and its in /dev/mapper/abc-def format,then convert it to
			 * /dev/abc/def format as this is the internal format of the tool
			 */
			StringAppendAt( n,0,device ) ;
			index = StringLastIndexOfChar( n,'-' ) ;
			if( index != -1 ){
				StringSubChar( n,index,'/' ) ;
				g = StringReplaceString( n,"/dev/mapper/","/dev/" ) ;
				if( stat( g,&str ) == 0 ){
					StringMultipleAppend( st,g," ",mount_point," ",file_system," ",mount_options,END ) ;
				}else{
					StringMultipleAppend( st,device," ",mount_point," ",file_system," ",mount_options,END ) ;
				}
			}else{
				StringMultipleAppend( st,device," ",mount_point," ",file_system," ",mount_options,END ) ;
			}
		}else if( StringPrefixMatch( device,"/dev/md",7 ) ){
			dev = zuluCryptResolveMDPath( device ) ;
			StringMultipleAppend( st,dev," ",mount_point," ",file_system," ",mount_options,END ) ;
			free( dev ) ;
		}else{
			StringMultipleAppend( st,device," ",mount_point," ",file_system," ",mount_options,END ) ;
		}
		stx = StringListAppendString( stx,st ) ;
		StringReset( st ) ;
		StringListDelete( &tmp ) ;
	}
	
	if( entry != NULL ){
		free( ( void * )entry ) ;
	}
	StringMultipleDelete( &st,&n,END ) ;
	StringListDelete( &stl ) ;
	return stx ;
}

stringList_t zuluCryptGetMoutedListFromMounts( void )
{
	ssize_t index ;
	char * dev ;
	const char * g ;
	string_t n ;
	struct stat str ;
	StringListIterator it ;
	StringListIterator end;
	string_t q = StringGetFromVirtualFile( "/proc/self/mounts" ) ;
	stringList_t stl = StringListStringSplit( q,'\n' ) ;
	StringDelete( &q ) ;
	if( stl == StringListVoid ){
		return StringListVoid ;
	}
	it  = StringListBegin( stl ) ;
	end = StringListEnd( stl ) ;
	for( ; it != end ;it++ ){
		q = *it ;
		if( StringStartsWith( q,"/dev/root" ) ){
			dev = zuluCryptResolveDevRoot() ;
			if( dev != NULL ){
				StringReplaceString( q,"/dev/root",dev ) ;
				free( dev ) ;
			}
		}else if( StringStartsWith( q,"/dev/disk/by-" ) ){
			index = StringIndexOfChar( q,0,' ' ) ;
			if( index != -1 ){
				dev = zuluCryptRealPath( StringSubChar( q,index,'\0' ) ) ;
				StringSubChar( q,index,' ' ) ;
				if( dev != NULL ){
					StringRemoveLeft( q,index ) ;
					StringMultiplePrepend( q," ",dev,END ) ;
					free( dev ) ;
				}
			}
		}else if( StringStartsWith( q,"/dev/loop" ) ){
			index = StringIndexOfChar( q,0,' ' ) ;
			if( index != -1 ){
				dev = zuluCryptLoopDeviceAddress( StringSubChar( q,index,'\0' ) ) ;
				StringSubChar( q,index,' ' ) ;
				if( dev != NULL ){
					StringRemoveLeft( q,index ) ;
					StringMultiplePrepend( q," ",dev,END ) ;
					free( dev ) ;
				}
			}
		}else if( StringStartsWith( q,"/dev/mapper/" ) ){
			n = StringCopy( *it ) ;
			index = StringLastIndexOfChar( n,'-' ) ;
			if( index != -1 ){
				StringSubChar( n,index,'/' ) ;
				StringReplaceString( n,"/dev/mapper/","/dev/" ) ;
				index = StringIndexOfChar( n,index,' ' ) ;
				if( index != -1 ){
					g = StringSubChar( n,index,'\0' ) ;
					if( stat( g,&str ) == 0 ){
						g = StringSubChar( n,index,' ' ) ;
						StringAppendAt( *it,0,g ) ;
					}else{
						;
					}
				}else{
					;
				}
			}else{
				;
			}
			StringDelete( &n ) ;
		}else if( StringStartsWith( q,"/dev/md" ) ){
			index = StringIndexOfChar( q,0,' ' ) ;
			if( index != -1 ){
				dev = zuluCryptResolveMDPath( StringSubChar( q,index,'\0' ) ) ;
				StringSubChar( q,index,' ' ) ;
				if( dev != NULL ){
					StringRemoveLeft( q,index ) ;
					StringMultiplePrepend( q," ",dev,END ) ;
					free( dev ) ;
				}
			}
		}
	}
	
	return stl ;
}

stringList_t zuluCryptGetMountInfoList( void )
{
	stringList_t stl = zuluCryptGetMoutedListFromMountInfo() ;
	
	if( stl == StringListVoid ){
		return zuluCryptGetMoutedListFromMounts() ;
	}else{
		return stl ; 
	}
}

stringList_t zuluCryptOpenedVolumesList( uid_t uid )
{
	const char * e ;
	const char * c ;
	const char * d ;
	char * f ;
	char * g ;
	
	size_t j ;
	size_t i ;
	
	ssize_t k ;
	
	string_t q ;
	string_t z ;
	
	stringList_t stx ;
	stringList_t list = StringListVoid ;
	stringList_t stl = zuluCryptGetMountInfoList() ;
	
	if( stl == StringListVoid ){
		return StringListVoid ;
	}
	
	if( uid ){;}
	
	j = StringListSize( stl )  ;
	
	for( i = 0 ; i < j ; i++ ){
		c = StringListContentAt( stl,i ) ;
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
		
		if( stx == StringListVoid ){
			continue ;
		}
		
		k = StringHasComponent_1( c,"-UUID-" ) ;
		if( k != -1 && StringHasNoComponent( c,"-NAAN-" ) ) {
			q = StringListStringAt( stx,0 ) ;
			/*
			 * zuluCryptDecodeMtabEntry() is defined in mount_volume.c
			 */
			d = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;
			
			/*
			 * zuluCryptGetVolumeTypeFromMapperPath() is defined in status.c
			 */
			f = zuluCryptGetVolumeTypeFromMapperPath( StringContent( q ) ) ;
			e = StringSubChar( q,StringLastIndexOfChar( q,'-' ),'\0' ) + k + 6 ;
			z = String( "" ) ;
			StringMultipleAppend( z,"UUID=\"",e,"\"\t",d,"\t",f,END ) ;
			list = StringListAppendString_1( list,&z ) ;
			free( f ) ;
		}else{
			g = zuluCryptVolumeDeviceName( StringListContentAt( stx,0 ) ) ;
			if( g != NULL ){
				d = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;
				/*
				 * zuluCryptGetVolumeTypeFromMapperPath() is defined in status.c
				 */
				f = zuluCryptGetVolumeTypeFromMapperPath( StringListContentAt( stx,0 ) ) ;
				z = String( "" ) ;
				StringMultipleAppend( z,g,"\t",d,"\t",f,END ) ;
				list = StringListAppendString_1( list,&z ) ;
				free( f ) ;
				free( g ) ;
			}
		}
		
		StringListDelete( &stx ) ;
	}
	StringListDelete( &stl ) ;
	return list ;
}

string_t zuluCryptGetMtabEntry( const char * path )
{
	stringList_t stl = zuluCryptGetMountInfoList() ;
	string_t st = zuluCryptGetMtabEntry_1( stl,path ) ;
	StringListDelete( &stl ) ;
	return st ;
}

string_t zuluCryptGetMtabEntry_1( stringList_t stl,const char * path )
{
	string_t st ;
	string_t entry = StringVoid ;
	ssize_t index ;
	char * e ;
	if( stl != StringListVoid ){
		if( StringPrefixMatch( path,"/dev/mapper/",12 ) ){
			/*
			* zuluCryptConvertIfPathIsLVM() is defined in status.c
			*/
			st = zuluCryptConvertIfPathIsLVM( path ) ;
		}else if( StringPrefixMatch( path,"/dev/loop",9 ) ){
			/*
			* zuluCryptLoopDeviceAddress_2() is defined in ./create_loop_device.c
			*/
			st = zuluCryptLoopDeviceAddress_2( path ) ;
		}else if( StringPrefixMatch( path,"/dev/md",7 ) ){
			/*
			 * zuluCryptLoopDeviceAddress_2() is defined in ./create_loop_device.c
			 */
			e = zuluCryptResolveMDPath( path ) ;
			st = StringInherit( &e ) ;
		}else{
			st = String( path ) ;
		}
	
		index = StringListHasStartSequence( stl,StringAppend( st," " ) ) ;
		StringDelete( &st ) ;
	
		if( index >= 0 ){
			entry = StringListCopyStringAt( stl,index ) ;
		}
	}
	return entry ;
}

char * zuluCryptGetMountPointFromPath( const char * path )
{
	string_t st = zuluCryptGetMtabEntry( path ) ;
	stringList_t stl ;
	
	if( st == StringVoid ){
		return NULL ;
	}else{
		stl = StringListStringSplit( st,' ' ) ;
		StringDelete( &st ) ;
		if( stl == StringListVoid ){
			return NULL ;
		}else{
			st = StringListCopyStringAt( stl,1 ) ;
			StringListDelete( &stl ) ;
			zuluCryptDecodeMtabEntry( st ) ;
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

