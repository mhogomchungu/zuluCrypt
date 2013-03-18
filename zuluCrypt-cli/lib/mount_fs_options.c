 
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

#include "../bin/includes.h"

int zulucryptFileSystemIsSupported( const char * fs )
{
	if(     StringsAreEqual( fs,"xfs" )   || 
		StringsAreEqual( fs,"ntfs" )  || 
		StringsAreEqual( fs,"vfat" )  || 
		StringsAreEqual( fs,"fat" )   ||
		StringsAreEqual( fs,"msdos" ) ||
		StringsAreEqual( fs,"umsdos" )||
		StringsAreEqual( fs,"affs" )  || 
		StringsAreEqual( fs,"hfs" )   ||
		StringsAreEqual( fs,"iso9660" )||
		StringsAreEqual( fs,"udf" )    ||
		StringsAreEqual( fs,"ext2" )   ||
		StringsAreEqual( fs,"ext3" )   ||
		StringsAreEqual( fs,"ext4" )   ||
		StringsAreEqual( fs,"reiserfs" )  ||
		StringsAreEqual( fs,"reiser4" )   ||
		StringsAreEqual( fs,"btrfs" ) ) 
		{
			return 1 ;
		}else{
			return 0 ;
		}
}

static inline int _userIsAllowed( uid_t uid,const char * fs )
{	
	if( uid == 0 ){
		return 1 ;
	}
	if( fs ){;}
	/*
	 * we are supposed to check here is a user is allowed to mount a file system not in the list above,
	 * we dont do this check for now. 
	 */
	return 0 ;
}

static inline int allowed_vfat( stringList_t stl )
{
	int st ;
	ssize_t index ;
	index = StringListHasSequence( stl,"uid=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"gid=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"shortname=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"dmask=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"umask=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"fmask=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"utf8=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"iocharset=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	
	st = StringListSize( stl ) ;
	StringListDelete( &stl ) ;
	return st > 0 ;
}

static inline int allowed_ntfs( stringList_t stl )
{
	int st ;
	ssize_t index ;
	index = StringListHasSequence( stl,"umask=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"dmask=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"fmask=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"locale=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"norecover=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"ignore_case=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"windows_names=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"compression=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"nocompression=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	st = StringListSize( stl ) ;
	StringListDelete( &stl ) ;
	return st > 0 ;
}

static inline int allowed_iso9660( stringList_t stl )
{
	int st ;
	ssize_t index ;
	index = StringListHasSequence( stl,"norock" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"nojoliet" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"iocharset=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"mode=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"dmode=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	st = StringListSize( stl ) ;
	StringListDelete( &stl ) ;
	return st > 0 ;
}

static inline int allowed_udf( stringList_t stl )
{
	int st ;
	ssize_t index ;
	index = StringListHasSequence( stl,"iocharset=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	index = StringListHasSequence( stl,"umask=" ) ;
	if( index >= 0 )
		StringListRemoveAt( stl,index ) ;
	st = StringListSize( stl ) ;
	StringListDelete( &stl ) ;
	return st > 0 ;
}

static inline int allowed_affs( stringList_t stl )
{
	StringListDelete( &stl ) ;
	return 1 ;
}

static inline int allowed_hfs( stringList_t stl )
{
	StringListDelete( &stl ) ;
	return 1 ;
}

static inline int allowed_extX( stringList_t stl )
{
	StringListDelete( &stl ) ;
	return 1 ;
}

static inline int allowed_reiserfs( stringList_t stl )
{
	StringListDelete( &stl ) ;
	return 1 ;
}

static inline int allowed_reiser4( stringList_t stl )
{
	StringListDelete( &stl ) ;
	return 1 ;
}

static inline int allowed_btrfs( stringList_t stl )
{
	StringListDelete( &stl ) ;
	return 1 ;
}

static inline int _option_contain_not_allowed( const char * fs,const char * fs_opts )
{
	stringList_t stl ;
	if( fs == NULL || fs_opts == NULL ){
		return 0 ;
	}
	
	stl = StringListSplit( fs_opts,',' ) ;
	
	if( stl == StringListVoid )
		return 0 ;
	if( StringsAreEqual( fs,"ext2" ) || StringsAreEqual( fs,"ext3" ) || StringsAreEqual( fs,"ext4" ) ){
		return allowed_extX( stl ) ;
	}
	if( StringsAreEqual( fs,"vfat" ) || StringsAreEqual( fs,"fat" ) || StringsAreEqual( fs,"msdos" ) || StringsAreEqual( fs,"msudos" ) ){
		return allowed_vfat( stl ) ;
	}
	if( StringsAreEqual( fs,"ntfs" ) ){
		return allowed_ntfs( stl ) ;
	}
	if( StringsAreEqual( fs,"udf" ) ){
		return allowed_udf( stl ) ;
	}
	if( StringsAreEqual( fs,"affs" ) ){
		return allowed_affs( stl ) ;
	}
	if( StringsAreEqual( fs,"hfs" ) ){
		return allowed_hfs( stl ) ;
	}
	if( StringsAreEqual( fs,"iso9660" ) ){
		return allowed_iso9660( stl ) ;
	}	
	if( StringsAreEqual( fs,"btrfs" ) ){
		return allowed_btrfs( stl ) ;
	}
	if( StringsAreEqual( fs,"reiserfs" ) ){
		return allowed_reiserfs( stl ) ;
	}
	if( StringsAreEqual( fs,"reiser4" ) ){
		return allowed_reiser4( stl ) ;
	}
	return 1 ;
}

int zuluCryptMountHasNotAllowedFileSystemOptions( uid_t uid,const char * fs_opts,string_t s )
{
	const char * fs = StringContent( s ) ;
	
	if( fs == NULL ){
		return 0 ;
	}
	if( !zulucryptFileSystemIsSupported( fs ) ){
		if( _userIsAllowed( uid,fs ) ){
			return 0 ;
		}else{
			return 1 ;
		}
	}
	if( _option_contain_not_allowed( fs,fs_opts ) ){
		if( _userIsAllowed( uid,fs ) ){
			return 0 ;
		}else{
			return 1 ;
		}
	}
	
	return 0 ;
}
