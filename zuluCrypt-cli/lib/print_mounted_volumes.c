 
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
	/*
	 * /proc/mounts is broken when volumes mounted with "-bind" options are used.Because of this,we try to avoid "/proc/mounts"
	 * and we use either "/etc/mtab" file only when it is not a symbolic link( if it is a link,its probably to "/proc/mounts" )
	 * or "/proc/self/mountinfo" 
	 */
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
			 * zuluCryptDeviceFromUUID() is defined in ../bin/partitions.c
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
		if( tmp == StringListVoid )
			break ;
		if( StringListContentAtEqual( tmp,3,"/" ) ){
			index = StringListContains( tmp,"-" ) ;
			if( index != -1 ){
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
				}else{
					if( strncmp( device,"/dev/disk/by-",13 ) != 0 ){
						StringMultipleAppend( st,device," ",mount_point," ",file_system," ",mount_options,END ) ;
					}else{
						dev = realpath( device,NULL ) ;
						StringMultipleAppend( st,dev," ",mount_point," ",file_system," ",mount_options,END ) ;
						free( dev ) ;
					}
				}
				stx = StringListAppendString( stx,st ) ;
				StringClear( st ) ;
			}
		}
		StringListDelete( &tmp ) ;
	}
	
	StringDelete( &st ) ;
	StringListDelete( &stl ) ;
	return stx ;
}

stringList_t zuluCryptGetMtabList( void )
{
#if USE_NEW_LIBMOUNT_API
	struct libmnt_lock * m_lock ;
#else
	mnt_lock * m_lock ;
#endif
	char * dev ;
	int index ;
	string_t q       = StringVoid     ;
	stringList_t stl = StringListVoid ;

	if( !zuluCryptMtabIsAtEtc() ){
		stl = zuluCryptGetMoutedListFromMountInfo() ;
		if( stl == StringListVoid ){
			/*
			 * couldnt get the list from "/proc/self/mountinfo,probably because the kernel is too old
			 * and doesnt support it,reluctantly, use,"/proc/mounts".
			 */
			q = StringGetFromVirtualFile( "/proc/mounts" ) ;
			stl = StringListStringSplit( q,'\n' ) ;
			StringDelete( &q ) ;
		}
	}else{
		m_lock = mnt_new_lock( "/etc/mtab~",getpid() ) ;
		if( mnt_lock_file( m_lock ) == 0 ){
			q = StringGetFromFile( "/etc/mtab" ) ;
			mnt_unlock_file( m_lock ) ;
		}
		mnt_free_lock( m_lock ) ;
		if( q != StringVoid ){
			stl = StringListStringSplit( q,'\n' ) ;
			StringDelete( &q ) ;
		}
	}

	index = StringListHasSequence( stl,"/dev/root" ) ;
	if( index >= 0 ){
		q = StringListStringAt( stl,index ) ;
		if( StringStartsWith( q,"/dev/root" ) ){
			dev = zuluCryptResolveDevRoot() ;
			StringReplaceString( q,"/dev/root",dev ) ;
			free( dev ) ;
		}
	}
	
	return stl ; 
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

