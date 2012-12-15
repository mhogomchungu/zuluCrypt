 
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
	if(     strcmp( fs,"ntfs" ) == 0  || 
		strcmp( fs,"vfat" ) == 0  || 
		strcmp( fs,"fat" ) == 0   ||
		strcmp( fs,"msdos" ) == 0 ||
		strcmp( fs,"umsdos" ) == 0 ||
		strcmp( fs,"affs" ) == 0 || 
		strcmp( fs,"hfs" ) == 0  ||
		strcmp( fs,"iso9660" ) == 0 ||
		strcmp( fs,"udf" ) == 0  ||
		strcmp( fs,"ext2" ) == 0 ||
		strcmp( fs,"ext3" ) == 0 ||
		strcmp( fs,"ext4" ) == 0 ||
		strcmp( fs,"reiserfs" ) == 0 ||
		strcmp( fs,"reiser4" ) == 0 ||
		strcmp( fs,"btrfs" ) == 0 ) 
		{
			return 1 ;
		}else{
			return 0 ;
		}
}

static inline int _userIsAllowed( uid_t uid,const char * fs )
{
	string_t p = String( "zulucrypt-" ) ;
	StringAppend( p,fs ) ;
	/*
	 * zuluCryptUserIsAMemberOfAGroup() is defined in ../bin/security.c
	 */
	if( uid == 0 || zuluCryptUserIsAMemberOfAGroup( uid,StringContent( p ) ) ){
		
		StringDelete( &p ) ;
		return 1 ;
	}else{
		StringDelete( &p ) ;
		return 0 ;
	}
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
	if( fs == NULL || fs_opts == NULL )
		return 0 ;
	
	stl = StringListSplit( fs_opts,',' ) ;
	
	if( stl == StringListVoid )
		return 0 ;
	if( strcmp( fs,"ext2" ) == 0 || strcmp( fs,"ext3" ) == 0 || strcmp( fs,"ext4" ) == 0 ){
		return allowed_extX( stl ) ;
	}
	if( strcmp( fs,"vfat" ) == 0 || strcmp( fs,"fat" ) == 0 || strcmp( fs,"msdos" ) == 0 || strcmp( fs,"msudos" ) == 0 ){
		return allowed_vfat( stl ) ;
	}
	if( strcmp( fs,"ntfs" ) == 0 ){
		return allowed_vfat( stl ) ;
	}
	if( strcmp( fs,"iso9660" ) == 0 ){
		return allowed_vfat( stl ) ;
	}
	if( strcmp( fs,"udf" ) == 0 ){
		return allowed_vfat( stl ) ;
	}
	if( strcmp( fs,"affs" ) == 0 ){
		return allowed_affs( stl ) ;
	}
	if( strcmp( fs,"hfs" ) == 0 ){
		return allowed_hfs( stl ) ;
	}
	if( strcmp( fs,"iso9660" ) == 0 ){
		return allowed_iso9660( stl ) ;
	}	
	if( strcmp( fs,"btrfs" ) == 0 ){
		return allowed_iso9660( stl ) ;
	}
	if( strcmp( fs,"reiserfs" ) == 0 ){
		return allowed_iso9660( stl ) ;
	}
	if( strcmp( fs,"reiser4" ) == 0 ){
		return allowed_iso9660( stl ) ;
	}
	return 1 ;
}

int zuluCryptMountHasNotAllowedFileSystemOptions( uid_t uid,const char * fs_opts,string_t s )
{
	const char * fs = StringContent( s ) ;
	
	if( fs == NULL )
		return 0 ;
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
