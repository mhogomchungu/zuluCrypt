/*
 *
 *  Copyright (c) 2014
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
#include <unistd.h>

static char * _zuluCryptResolveDevRoot( void )
{
	const char * e ;
	char * dev ;
	string_t st      = StringGetFromVirtualFile( "/proc/cmdline" ) ;
	stringList_t stl = StringListStringSplit( st,' ' ) ;
	StringDelete( &st ) ;

	st = StringListHasSequence_1( stl,"root=/dev/" ) ;
	if( st != StringVoid ){
		e = StringContent( st ) + 5 ;
		dev = zuluCryptResolvePath( e ) ;
	}else{
		st = StringListHasSequence_1( stl,"root=UUID=" ) ;
		if( st != StringVoid ){
			/*
			 * zuluCryptDeviceFromUUID() is defined in ./blkid_evaluate_tag.c
			 */
			e = StringContent( st ) + 10 ;
			dev = zuluCryptDeviceFromUUID( e ) ;
		}else{
			dev = NULL ;
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
	int r = 0 ;
	if( dir != NULL ){
		while( ( entry = readdir( dir ) ) != NULL ){
			f = entry->d_name ;
			if( !StringAtLeastOneMatch_1( f,".","..",NULL ) ){
				e = zuluCryptRealPath( StringAppendAt( st,8,f ) ) ;
				r = StringsAreEqual( path,e ) ;
				StringFree( e ) ;
				if( r == 1 ){
					break ;
				}
			}
		}
		closedir( dir ) ;
	}

	if( r == 1 ){
		return st ;
	}else{
		StringReplace( st,path ) ;
		return st ;
	}
}

char * zuluCryptResolveMDPath( const char * path )
{
	string_t st = zuluCryptResolveMDPath_1( path ) ;
	return StringDeleteHandle( &st ) ;
}

/*
 * dm path is a path like "/dev/dm-5".
 * These paths should not be used as they are for dm kernel infrastructure internal use only.
 * We are just handling them because there is buggy code out there that forces them on us.
 *
 * When we get one,we try to convert it to its appropriate paths.
 */
char * zuluCryptResolveDMPath( const char * path )
{
	char dm_path[ PATH_MAX + 1 ] ;
	DIR * dir = opendir( "/dev/mapper/" ) ;
	string_t st ;
	string_t xt ;
	struct dirent * entry ;
	const char * e ;
	const char * z ;
	ssize_t index ;
	char * dev = NULL ;
	if( dir != NULL ){
		st = String( "/dev/mapper/" ) ;
		while( ( entry = readdir( dir ) ) != NULL ){
			e = entry->d_name ;
			if( !StringAtLeastOneMatch_1( e,".","..","control",NULL ) ){
				z = StringAppendAt( st,12,e ) ;
				index = readlink( z,dm_path,PATH_MAX ) ;
				if( index != -1 ){
					dm_path[ index ] = '\0' ;
					/*
					 * path will have something like "/dev/dm-5",skip forward to only "dm-5"
					 * dm_path will have something like "../dm-5",skip forward to only "dm-5"
					 */
					if( StringsAreEqual( path + 5,dm_path + 3 ) ){
						xt = zuluCryptConvertIfPathIsLVM( z ) ;
						dev = StringDeleteHandle( &xt ) ;
						break ;
					}
				}
			}
		}
		StringDelete( &st ) ;
		closedir( dir ) ;
	}

	if( dev == NULL ){
		/*
		 * failed to resolve the path,just return original path
		 */
		return StringCopy_2( path ) ;
	}else{
		return dev ;
	}
}

/*
 * An assumption is made here that the path is an LVM path if "path" is in /dev/mapper/abc-def format
 * and there exist a path at /dev/abc/def.
 *
 * Info in lvm path structures can be found here: https://www.redhat.com/archives/linux-lvm/2014-January/msg00014.html
 */
string_t zuluCryptConvertIfPathIsLVM( const char * path )
{
	const char * e ;
	const char ** z ;

	struct stat st ;

	char * c ;
	char * d ;
	char * k ;

	StringIterator it  ;
	StringIterator end ;

	string_t q = String( path ) ;

	StringGetIteratorBeginAndEnd( q,&it,&end ) ;

	/*
	 * jumpt to the third character
	 */
	it = it + 3 ;

	while( it < end ){
		d = it ;
		it++ ;
		c = d - 1 ;
		k = d - 2 ;
		if( *k != '-' && *c == '-' && *d != '-' ){
			/*
			 * found a place with a single dash,replace the dash with a slash
			 */
			*c = '/' ;
			/*
			 * replace double dashes if present.
			 */
			z = StringPointer( q ) ;
			while( StringHasComponent( *z,"--" ) ){
				StringReplaceString( q,"--","-" ) ;
			}

			e = StringReplaceString( q,"/dev/mapper/","/dev/" ) ;

			if( stat( e,&st ) == 0 ){
				/*
				 * The path appear to be an LVM path since "/dev/mapper/ABC-DEF" input path has a corresponding
				 * "/dev/ABC/DEF" path
				 */
			}else{
				/*
				 * Not an LVM volume,replace the string to its original
				 */
				StringReplace( q,path ) ;
			}
			break ;
		}
	}

	return q ;
}

char * zuluCryptResolvePath( const char * path )
{
	char * e ;
	string_t st ;
	if( StringsAreEqual( path,"/dev/root" ) ){

		e = _zuluCryptResolveDevRoot() ;
		if( e == NULL ){
			return StringCopy_2( path ) ;
		}else{
			return e ;
		}
	}else if( StringPrefixEqual( path,"/dev/disk/by-" ) ){
		/*
		 * zuluCryptRealPath() is defined in real_path.c
		 */
		e = zuluCryptRealPath( path ) ;
		if( e == NULL ){
			return StringCopy_2( path ) ;
		}else{
			if( StringPrefixEqual( e,"/dev/mapper/" ) ){
				st = zuluCryptConvertIfPathIsLVM( e ) ;
				StringFree( e ) ;
				return StringDeleteHandle( &st ) ;
			}else{
				return e ;
			}
		}
	}else if( StringPrefixEqual( path,"/dev/mapper/" ) ){

		st = zuluCryptConvertIfPathIsLVM( path ) ;
		return StringDeleteHandle( &st ) ;

	}else if( StringPrefixEqual( path,"/dev/md" ) ){

		return zuluCryptResolveMDPath( path ) ;

	}else if( StringPrefixEqual( path,"/dev/dm-" ) ){

		return zuluCryptResolveDMPath( path ) ;

	}else if( StringPrefixEqual( path,"/dev/loop" ) ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in create_loop_device.c
		 */
		return zuluCryptLoopDeviceAddress( path ) ;
	}else{
		return StringCopy_2( path ) ;
	}
}

string_t zuluCryptResolvePath_1( const char * path )
{
	char * e = zuluCryptResolvePath( path ) ;
	return StringInherit( &e ) ;
}

string_t zuluCryptResolvePath_2( const char * path )
{
	if( StringPrefixEqual( path,"/dev/loop" ) ){
		return String( path ) ;
	}else{
		return zuluCryptResolvePath_1( path ) ;
	}
}

char * zuluCryptResolvePath_3( const char * path )
{
	if( StringPrefixEqual( path,"/dev/loop" ) ){
		/*
		 * zuluCryptLoopDeviceAddress_1() is defined in create_loop_device.c
		 */
		return zuluCryptLoopDeviceAddress_1( path ) ;
	}else{
		return zuluCryptResolvePath( path ) ;
	}
}
