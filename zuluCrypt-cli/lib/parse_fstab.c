 
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

static char * _resolve_path( char * path )
{
	string_t st ;
	if( StringPrefixMatch( path,"/dev/mapper/",12 ) ){
		/*
		 * zuluCryptConvertIfPathIsLVM() is defined in status.c
		 */
		st = zuluCryptConvertIfPathIsLVM( path ) ;
	}else if( StringPrefixMatch( path,"/dev/loop",9 ) ){
		/*
		 * zuluCryptLoopDeviceAddress_2() is defined in create_loop_device.c
		 */
		st = zuluCryptLoopDeviceAddress_2( path ) ;
	}else if( StringPrefixMatch( path,"/dev/md",7 ) ){
		/*
		 * zuluCryptResolveMDPath() is defined in this process_mountinfo.c
		 */
		st = zuluCryptResolveMDPath_1( path ) ;
	}else{
		st = String( path ) ;
	}
	
	StringFree( path ) ;
	
	return StringDeleteHandle( &st ) ;
}

static inline char * _evaluate_tag( const char * tag,const char * entry,blkid_cache * cache )
{
	char * f = NULL ;
	string_t st = String( entry ) ;
	const char * e = StringReplaceChar_1( st,0,' ','\0' ) ;
	
	if( e != NULL ){
		f = blkid_evaluate_tag( tag,e,cache ) ;
	}
	
	StringDelete( &st ) ;
	
	return _resolve_path( f ) ;
}

static inline char * _evaluate_tag_by_id( string_t st )
{
	char * f = NULL ;
	ssize_t index = StringIndexOfChar( st,0,' ' ) ;
	if( index >= 0 ){
		f = zuluCryptRealPath( StringSubChar( st,index,'\0' ) ) ;
		StringSubChar( st,index,' ' ) ;
	}
	return  _resolve_path( f ) ;
}

stringList_t zuluCryptGetFstabList( uid_t uid )
{
	string_t xt = StringGetFromFile( "/etc/fstab" ) ;
	string_t st ;
	
	stringList_t fstabList = StringListVoid ;
	
	StringListIterator it  ;
	StringListIterator end ;
	
	ssize_t index ;
	ssize_t index_1 ;
	
	char * ac ;
	const char * entry ;
	const char * e ;
	const char * f ;
	
	blkid_cache cache = NULL ;
	
	struct stat str ;
	
	if( xt == StringVoid ){
		return StringListVoid ;
	}
	
	if( uid ){;}
	
	fstabList = StringListStringSplit( xt,'\n' ) ;
	
	StringDelete( &xt ) ;
	
	if( fstabList == StringListVoid ){
		return StringListVoid ;
	}
	
	StringListRemoveIfStringStartsWith( fstabList,"#" ) ;
	
	if( StringListSize( fstabList ) < 1 ){
		StringListDelete( &fstabList ) ;
		return StringListVoid ;
	}
	
	if( blkid_get_cache( &cache,NULL ) != 0 ){
		cache = NULL ;
	}
	
	it  = StringListBegin( fstabList ) ;
	end = StringListEnd( fstabList ) ;
	
	while( it != end  ){
		xt = *it ;
		it++ ;
		entry = StringContent( xt ) ;
		if( StringPrefixMatch( entry,"/dev/",5 ) ){
			if( StringPrefixMatch( entry,"/dev/root",9 ) ){
				/*
				 * zuluCryptResolveDevRoot() is defined in ./print_mounted_volumes.c 
				 */
				ac =  zuluCryptResolveDevRoot() ;
				if( ac != NULL ){
					StringReplaceString( xt,"/dev/root",ac ) ;
					free( ac ) ;
				}
			}else if( StringPrefixMatch( entry,"/dev/disk/by",12 ) ){
				ac = _evaluate_tag_by_id( xt ) ;
				if( ac != NULL ){
					index = StringIndexOfChar( xt,0,' ' ) ;
					if( index >= 0 ){
						StringRemoveLeft( xt,index ) ;
						StringPrepend( xt,ac ) ;
					}
					free( ac ) ;
				}
			}else if( StringPrefixMatch( entry,"/dev/mapper/",12 ) ){
				st = StringCopy( xt ) ;
				index_1 = StringIndexOfChar( st,0,' ' ) ;
				if( index_1 >= 0 ){
					
					index = StringLastIndexOfChar( st,'-' ) ;
					
					f = StringSubChar( st,index_1,'\0' ) ;
					
					if( index != -1 ){
						StringSubChar( st,index,'/' ) ;
						e = StringReplaceString( st,"/dev/mapper/","/dev/" ) ;
						if( stat( e,&str ) == 0 ){
							StringSubChar( xt,index,'/' ) ;
							StringReplaceString( xt,"/dev/mapper/","/dev/" ) ;
						}else{
							/*
							 * zuluCryptVolumeDeviceName() is defined in status.c
							 */
							ac = zuluCryptVolumeDeviceName( f ) ;
							
							if( ac != NULL ){
								StringRemoveLeft( xt,index_1 ) ;
								StringPrepend( xt,ac ) ;
								free( ac ) ;
							}
						}
					}else{
						/*
						 * zuluCryptVolumeDeviceName() is defined in status.c
						 */
						ac = zuluCryptVolumeDeviceName( f ) ;
						if( ac != NULL ){
							StringRemoveLeft( xt,index_1 ) ;
							StringPrepend( xt,ac ) ;
							free( ac ) ;
						}
					}
				}
				StringDelete( &st ) ;
			}else if( StringPrefixMatch( entry,"/dev/md",7 ) ){
				/*
				 * zuluCryptResolveMDPath() is defined in process_mountinfo.c
				 */
				index = StringIndexOfChar( xt,0,' ' ) ;
				if( index != -1 ){
					ac =  zuluCryptResolveMDPath( StringSubChar( xt,index,'\0' ) ) ;
					StringSubChar( xt,index,' ' ) ;
					if( ac != NULL ){
						StringRemoveLeft( xt,index ) ;
						StringPrepend( xt,ac ) ;
						free( ac ) ;
					}
				}
			}
		}else if( StringAtLeastOnePrefixMatch( entry,"UUID=","uuid=",NULL ) ){
			entry = StringRemoveString( xt,"\"" ) ;
			ac = _evaluate_tag( "UUID",entry + 5,&cache ) ;
			if( ac != NULL ){
				index = StringIndexOfChar( xt,0,' ' ) ;
				if( index >= 0 ){
					StringRemoveLeft( xt,index ) ;
					StringPrepend( xt,ac ) ;
				}
				free( ac ) ;
			}
		}else if( StringAtLeastOnePrefixMatch( entry,"LABEL=","label=",NULL ) ){
			entry = StringRemoveString( xt,"\"" ) ;
			ac = _evaluate_tag( "LABEL",entry + 6,&cache ) ;
			if( ac != NULL ){
				index = StringIndexOfChar( xt,0,' ' ) ;
				if( index >= 0 ){
					StringRemoveLeft( xt,index ) ;
					StringPrepend( xt,ac ) ;
				}
				free( ac ) ;
			}
		}
	}
	
	if( cache != NULL ){
		blkid_put_cache( cache ) ;
	}
	return fstabList ;
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

