 
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

#include "../bin/includes.h"

static int _fileSystemIsSupported( const char * fs )
{
	string_t           st  = StringGetFromVirtualFile( "/proc/filesystems" ) ;
	stringList_t       stl = StringListStringSplit( st,'\n' ) ;
	StringListIterator it  = StringListBegin( stl ) ;
	StringListIterator end = StringListEnd( stl ) ;
	string_t xt ;
	int r = 0 ;
	
	while( it != end ){
		xt = *it ;
		it++ ;
		if( !StringStartsWith( xt,"nodev" ) ){
			if( StringContains( xt,fs ) ){
				r = 1 ;
				break ;
			}
		}
	}
	
	StringDelete( &st ) ;
	StringListDelete( &stl ) ;
	return r ;
}

int zulucryptFileSystemIsSupported( const char * fs )
{
	const char * f[] = { "xfs","ntfs","vfat","msdos","umsdos","affs","hfs","iso9660",
		"udf","ext2","ext3","ext4","reiserfs","reiser4","btrfs","squashfs",NULL } ;
	
	const char ** e = f ;
	
	if( fs == NULL ){
		return 0 ;
	}else{
		while( 1 ){
			if( *e == NULL ){
				return _fileSystemIsSupported( fs );
			}else if( strcmp( fs,*e ) == 0 ){
				return 1 ;
			}else{
				e++ ;
			}
		}
	}
}

static inline int _check_options( const char ** e,stringList_t stl )
{	
	while( *e != NULL ){
		StringListRemoveIfStringContains( stl,*e ) ;
		e++ ;
	}
	return StringListSize( stl ) > 0 ;
}

static inline int allowed_vfat( stringList_t stl )
{
	/*
	 * is it ok to allow one user to mount a volume with another user as owner of files and folders?
	 */
	const char * f[] = { "uid=","gid=","shortname=","dmask=","umask=","fmask=","utf8","iocharset=",NULL } ;
	return _check_options( f,stl ) ;
}

static inline int allowed_ntfs( stringList_t stl )
{
	const char * f[] = { "umask=","dmask=","fmask=","dmask=","locale=","norecover",
		"ignore_case","windows_names","compression","nocompression",NULL } ;
	return _check_options( f,stl ) ;
}

static inline int allowed_iso9660( stringList_t stl )
{
	const char * f[] = { "norock","nojoliet","fmask=","iocharset=","mode=","dmode=",NULL } ;
	return _check_options( f,stl ) ;
}

static inline int allowed_udf( stringList_t stl )
{	
	const char * f[] = { "iocharset=","umask=",NULL } ;
	return _check_options( f,stl ) ;
}

static inline int _option_contain_not_allowed( const char * fs,const char * fs_opts )
{
	stringList_t stl = StringListSplit( fs_opts,',' ) ;
	
	int r = 1 ;
	
	if( stl != StringListVoid ){
		if( StringHasComponent( fs,"fat" ) || StringHasComponent( fs,"dos" ) ){
			r = allowed_vfat( stl ) ;
		}else if( StringsAreEqual( fs,"ntfs" ) ){
			r = allowed_ntfs( stl ) ;
		}else if( StringsAreEqual( fs,"udf" ) ){
			r = allowed_udf( stl ) ;
		}else if( StringsAreEqual( fs,"iso9660" ) ){
			r = allowed_iso9660( stl ) ;
		}else{
			r = 1 ;
		}
		StringListDelete( &stl ) ;
	}else{
		r = 1 ;
	}
	
	return r ;
}

static inline int _userIsAllowed( uid_t uid,const char * fs )
{	
	if( fs ){;}
	
	if( uid == 0 ){
		/*
		 * cant say No to root
		 */
		return 1 ;
	}else{
		/*
		 * user is not root,we are supposed to use other means to check if a user is allowed to useful
		 * non default fs options or not.By policy,the way to do it is through unix group system but the
		 * functionality is off for now 
		 */
		return 0 ;
	}
}

int zuluCryptMountHasNotAllowedFileSystemOptions( uid_t uid,const char * fs_opts,string_t s )
{
	const char * fs = StringContent( s ) ;
	
	if( fs == NULL ){
		/*
		 * cant mount a volume with no file system,shouldnt even get here
		 */
		return 1 ;
	}
	
	if( zulucryptFileSystemIsSupported( fs ) ){
		/*
		 * file system is supported
		 */
		if( fs_opts == NULL ){
			/*
			 * file system is supported and user did not change default fs option.
			 * return early with success
			 */
			return 0 ;
		}
		
		if( _option_contain_not_allowed( fs,fs_opts ) ){
			/*
			 * file system options are not supported,only privileged users should be allowed to mount
			 */
			if( _userIsAllowed( uid,fs ) ){
				/*
				 * user is allowed to use non default fs options
				 */
				return 0 ;
			}else{
				/*
				 * user not allowed to use non default fs options
				 */
				return 1 ;
			}
		}else{
			/*
			 * supported file system with default options,allow it
			 */
			return 0 ;
		}
	}else{
		/*
		 * not supported file system 
		 */
		if( _userIsAllowed( uid,fs ) ){
			/*
			 * user is allowed to use a not supported file system.
			 * We cant check for supported fs options in a non supported file system so just return
			 * with success.
			 */
			return 0 ;
		}else{
			/*
			 * user not allowed to use the file system,return early with error since we cant support
			 * file system options in a not supported file system
			 */
			return 1 ;
		}
	}
}
