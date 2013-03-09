 
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

#include <libcryptsetup.h>
/*
 * below header file does not ship with the source code, it is created at configure time 
 */
#include "libmount_header.h"

char * zuluCryptResolveDevRoot( void )
{
	char * dev = NULL ;
	const char * e ;
	int index ;
	stringList_t stl ;
	string_t st = StringGetFromVirtualFile( "/proc/cmdline" ) ;
	if( st == StringVoid ){
		return NULL ;
	}
	stl = StringListStringSplit( st,' ' ) ;
	StringDelete( &st ) ;
	index = StringListHasSequence( stl,"root=/dev/" ) ;
	if( index >= 0 ){
		st = StringListCopyStringAt( stl,index ) ;
		e = StringRemoveString( st,"root=" ) ;
		if( StringPrefixMatch( e,"/dev/disk/by-",13 ) ){
			/*
			 * zuluCryptRealPath() is defined in ./real_path.c
			 */
			dev = zuluCryptRealPath( e ) ;
			StringDelete( &st ) ;
		}else{
			dev = StringDeleteHandle( &st ) ;
		}
	}else{
		index = StringListHasSequence( stl,"root=UUID=" ) ;
		if( index >= 0 ){
			st = StringListCopyStringAt( stl,index ) ;
			StringRemoveString( st,"root=UUID=" ) ;
			/*
			 * zuluCryptDeviceFromUUID() is defined in ./blkid_evaluate_tag.c
			 */
			dev = zuluCryptDeviceFromUUID( StringContent( st ) ) ;
			StringDelete( &st ) ;
		}
	}
	StringListDelete( &stl ) ;
	return dev ;
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
	struct stat str ;
	stringList_t tmp ;
	stringList_t stx = StringListVoid;
	stringList_t stl ;
	StringListIterator it ;
	StringListIterator end;
	string_t n ;
	string_t st = StringGetFromVirtualFile( "/proc/self/mountinfo" ) ;
	if( st == StringVoid ){
		return StringListVoid ;
	}
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
		if( tmp == StringListVoid ){
			continue ;
		}
		if( !StringListContentAtEqual( tmp,3,"/" ) ){
			StringListDelete( &tmp ) ;
			continue ;
		}
		index = StringListContains( tmp,"-" ) ;
		if( index == -1 ){
			StringListDelete( &tmp ) ;
			continue ;
		}
		device        = StringListContentAt( tmp,index+2 ) ;
		mount_point   = StringListContentAt( tmp,4 ) ;
		file_system   = StringListContentAt( tmp,index+1 ) ;
		mount_options = StringListContentAt( tmp,index-1 ) ;
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
			StringAppend( n,device ) ;
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
			StringReset( n ) ;
		}else{
			StringMultipleAppend( st,device," ",mount_point," ",file_system," ",mount_options,END ) ;
		}
		stx = StringListAppendString( stx,st ) ;
		StringReset( st ) ;
		StringListDelete( &tmp ) ;
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
						StringSubChar( n,index,' ' ) ;
						StringReset( *it ) ;
						StringAppendString( *it,n ) ;
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
	const char * c ;
	const char * d ;
	const char * e ;
	char * f ;
	char * g ;
	
	size_t len ;
	size_t j ;
	size_t i ;
	ssize_t k ;
	
	string_t q ; 
	string_t z ;
	string_t p ;
	
	stringList_t stx ;
	stringList_t list = StringListVoid ;
	stringList_t stl = zuluCryptGetMountInfoList() ;
	
	if( stl == StringListVoid ){
		return StringListVoid ;
	}
	
	p = StringIntToString( uid ) ;
	
	e = StringMultiplePrepend( p,"/zuluCrypt-",crypt_get_dir(),END ) ;
	
	len = StringLength( p ) ;
	j = StringListSize( stl )  ;
	
	for( i = 0 ; i < j ; i++ ){
		c = StringListContentAt( stl,i ) ;
		if( StringPrefixMatch( c,e,len ) ){
			if( StringHasComponent( c,"/media/share" ) ){
				/*
				 * dont show mirror images due to bind mounts
				 */
				continue ;
			}
			stx = StringListSplit( c,' ' ) ;
			if( stx == StringListVoid ){
				continue ;
			}
			if( StringPrefixMatch( c + len + 1,"UUID",4 ) ){
				q = StringListStringAt( stx,0 ) ;
				k = StringLastIndexOfChar( q,'-' ) ;
				if( k != -1 ){
					c = StringSubChar( q,k,'\0' ) + len + 6 ;
					/*
					 * zuluCryptDecodeMtabEntry() is defined in mount_volume.c
					 */
					d = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;
					/*
					 * zuluCryptGetVolumeTypeFromMapperPath() is defined in status.c
					 */
					f = zuluCryptGetVolumeTypeFromMapperPath( StringSubChar( q,k,'-' ) ) ;
					z = String( "" ) ;
					StringMultipleAppend( z,"UUID=\"",c,"\"\t",d,"\t",f,END ) ;
					list = StringListAppendString_1( list,&z ) ;
					free( f ) ;
				}
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
	}
	StringListDelete( &stl ) ;
	StringDelete( &p ) ;
	return list ;
}

string_t zuluCryptGetMtabEntry( const char * path )
{
	const char * g ;
	struct stat str ;
	string_t st ;
	string_t entry = StringVoid ;
	stringList_t stl = zuluCryptGetMountInfoList() ;
	ssize_t index = -1 ;
	char * e = NULL ;
	
	if( StringPrefixMatch( path,"/dev/mapper/",12 ) ){
		st = String( path ) ;
		index = StringLastIndexOfChar( st,'-' ) ;
		if( index != -1 ){
			StringSubChar( st,index,'/' ) ;
			g = StringReplaceString( st,"/dev/mapper/","/dev/" ) ;
			if( stat( g,&str ) == 0 ){
				index = StringListHasStartSequence( stl,g ) ;
			}else{
				index = StringListHasStartSequence( stl,path ) ;
			}
		}else{
			index = StringListHasStartSequence( stl,path ) ;
		}
		StringDelete( &st ) ;
	}else if( StringPrefixMatch( path,"/dev/loop",9 ) ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ./create_loop_device.c
		 */
		e = zuluCryptLoopDeviceAddress( path ) ;
		if( e != NULL ){
			index = StringListHasStartSequence( stl,e ) ;
			free( e ) ;
		}
	}else{
		index = StringListHasStartSequence( stl,path ) ;
	}
	
	if( index >= 0 ){
		entry = StringListCopyStringAt( stl,index ) ;
	}
	StringListDelete( &stl ) ;
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

