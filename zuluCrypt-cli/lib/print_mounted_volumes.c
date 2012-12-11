 
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

/*
 * defined at ../lib/status.c 
 */
char * zuluCryptVolumeDeviceName( const char * ) ;

char * zuluCryptDeviceFromUUID( const char * uuid ) ;

int zuluCryptMtabIsAtEtc( void )
{	
	const char * mpath = "/etc/mtab" ;
	char * path = realpath( mpath,NULL ) ;
	int st ;
	if( path == NULL ){
		return 0 ;
	}else{
		st = strcmp( path,mpath ) ;
		free( path ) ;
		return st == 0 ;
	}
}

const char * zuluCryptDecodeMtabEntry( string_t st )
{
	StringReplaceString( st,"\\012","\n" ) ;
	StringReplaceString( st,"\\040"," " ) ;
	StringReplaceString( st,"\\134","\\" ) ;
	return StringReplaceString( st,"\\011","\\t" ) ;
}

char * zuluCryptResolveDevRoot( void )
{
	char * dev ;
	int index ;
	stringList_t stl ;
	string_t st = StringGetFromVirtualFile( "/proc/cmdline" ) ;
	string_t xt ;
	if( st == StringVoid )
		return NULL ;
	stl = StringListStringSplit( st,' ' ) ;
	StringDelete( &st ) ;
	index = StringListHasSequence( stl,"root=/dev/" ) ;
	if( index >= 0 ){
		st = StringListCopyStringAt( stl,index ) ;
		StringRemoveString( st,"root=" ) ;
	}else{
		index = StringListHasSequence( stl,"root=UUID=" ) ;
		if( index >= 0 ){
			xt = StringListCopyStringAt( stl,index ) ;
			StringRemoveString( xt,"root=UUID=" ) ;
			/*
			 * zuluCryptDeviceFromUUID() is defined in ./blkid_evaluate_tag.c
			 */
			dev = zuluCryptDeviceFromUUID( StringContent( xt ) ) ;
			StringDelete( &xt ) ;
			st = StringInherit( &dev ) ;
		}
	}
	StringListDelete( &stl ) ;
	return StringDeleteHandle( &st ) ;
}

stringList_t zuluCryptGetMoutedListFromMountInfo( void )
{
	const char * device ;
	const char * mount_point ;
	const char * file_system ;
	const char * mount_options ;
	char * dev ;
	int index ;
	stringList_t tmp ;
	stringList_t stx = StringListVoid;
	stringList_t stl ;
	StringListIterator it ;
	StringListIterator end;
	string_t st = StringGetFromVirtualFile( "/proc/self/mountinfo" ) ;
	if( st == StringVoid )
		return StringListVoid ;
	stl = StringListStringSplit( st,'\n' ) ;
	StringDelete( &st ) ;
	if( stl == StringListVoid )
		return StringListVoid ;
	it  = StringListBegin( stl ) ;
	end = StringListEnd( stl )   ;
	st = String( "" ) ;
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
		mount_options = StringListContentAt( tmp,index+3 ) ;
		if( strncmp( device,"/dev/loop",9 ) == 0 ){	
			/*
			 * zuluCryptLoopDeviceAddress() is defined in ./status.c
			 */
			dev = zuluCryptLoopDeviceAddress( device ) ;
			if( dev == NULL ){
				StringMultipleAppend( st,device," ",mount_point," ",file_system," ",mount_options,END ) ;
			}else{
				StringMultipleAppend( st,dev," ",mount_point," ",file_system," ",mount_options,END ) ;
				free( dev ) ;
			}
		}else if( strcmp( device,"/dev/root" ) == 0 ){
			dev = zuluCryptResolveDevRoot() ;
			if( dev == NULL ){
				StringMultipleAppend( st,device," ",mount_point," ",file_system," ",mount_options,END ) ;
			}else{
				StringMultipleAppend( st,dev," ",mount_point," ",file_system," ",mount_options,END ) ;
				free( dev ) ;
			}
		}else if( strncmp( device,"/dev/disk/by-",13 ) == 0 ){
			dev = realpath( device,NULL ) ;
			if( dev == NULL ){
				StringMultipleAppend( st,device," ",mount_point," ",file_system," ",mount_options,END ) ;
			}else{
				StringMultipleAppend( st,dev," ",mount_point," ",file_system," ",mount_options,END ) ;
				free( dev ) ;
			}
		}else{
			StringMultipleAppend( st,device," ",mount_point," ",file_system," ",mount_options,END ) ;
		}
		stx = StringListAppendString( stx,st ) ;
		StringReset( st ) ;
		StringListDelete( &tmp ) ;
	}
	
	StringDelete( &st ) ;
	StringListDelete( &stl ) ;
	return stx ;
}

stringList_t zuluCryptGetMoutedListFromMounts( void )
{
	ssize_t index ;
	char * dev ;
	StringListIterator it ;
	StringListIterator end;
	string_t q = StringGetFromVirtualFile( "/proc/self/mounts" ) ;
	stringList_t stl = StringListStringSplit( q,'\n' ) ;
	StringDelete( &q ) ;
	if( stl == StringListVoid )
		return StringListVoid ;
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
				dev = realpath( StringSubChar( q,index,'\0' ),NULL ) ;
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
		}
	}
	
	return stl ;
}

stringList_t zuluCryptGetMtabList( void )
{
	stringList_t stl = zuluCryptGetMoutedListFromMountInfo() ;
	
	if( stl == StringListVoid ){
		stl = zuluCryptGetMoutedListFromMounts() ;
		if( stl == StringListVoid ){
			return StringListVoid ;
		}
	}
	
	return stl ; 
}

static void print( uid_t uid,stringList_t stl )
{
	const char * c ;
	const char * d ;
	const char * e ;
	char * f ;
	
	size_t len ;
	size_t j ;
	size_t i ;
	ssize_t k ;
	
	stringList_t stx ;
	
	string_t q ; 
	string_t p = StringIntToString( uid ) ;
	
	e = StringMultiplePrepend( p,"/zuluCrypt-",crypt_get_dir(),END ) ;
	
	len = StringLength( p ) ;
	j = StringListSize( stl )  ;
	
	for( i = 0 ; i < j ; i++ ){
		c = StringListContentAt( stl,i ) ;
		if( strncmp( c,e,len ) == 0 ){
			stx = StringListSplit( c,' ' ) ;
			if( stx == StringListVoid )
				continue ;
			if( strncmp( c + len + 1,"UUID",4 ) == 0 ){
				q = StringListStringAt( stx,0 ) ;
				k = StringLastIndexOfChar( q,'-' ) ;
				if( k != -1 ){
					c = StringSubChar( q,k,'\0' ) + len + 6 ;
					d = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;
					printf( "UUID=\"%s\"\t%s\n",c,d ) ;
				}
			}else{
				f = zuluCryptVolumeDeviceName( StringListContentAt( stx,0 ) ) ;
				if( f != NULL ){
					d = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;
					printf( "%s\t%s\n",f,d ) ;
					free( f ) ;
				}
			}
			
			StringListDelete( &stx ) ;
		}
	}
	
	StringDelete( &p ) ;
}

int zuluCryptPrintOpenedVolumes( uid_t uid )
{
	stringList_t stl = zuluCryptGetMtabList() ;
	if( stl == StringListVoid )
		return 1 ;
	print( uid,stl ) ;
	StringListDelete( &stl ) ;
	return 0 ;
}

string_t zuluCryptGetMtabEntry( const char * path )
{
	string_t entry = StringVoid ;
	stringList_t stl = zuluCryptGetMtabList() ;

	StringListIterator it  ;
	StringListIterator end ;
	
	if( stl == StringListVoid )
		return StringVoid ;
	
	it  = StringListBegin( stl ) ;
	end = StringListEnd( stl ) ;
	
	for( ; it != end ; it++ ){
		if( StringStartsWith( *it,path ) ){
			entry = StringCopy( *it ) ;
			break ;
		}
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

