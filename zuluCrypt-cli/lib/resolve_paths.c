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
 * When we one,we try to convert them to their appropriate paths.
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

string_t zuluCryptConvertIfPathIsLVM( const char * path )
{
	/*
	 * An assumption is made here that the path is an LVM path if "path" is in /dev/mapper/abc-def format
	 * and there exist a path at /dev/abc/def format.
	 *
	 * handle double dashes as explained here: https://www.redhat.com/archives/linux-lvm/2014-January/msg00014.html
	 */
	ssize_t index ;

	size_t i ;
	size_t j ;

	const char * e ;
	const char ** z ;

	struct stat st ;

	char c ;
	char d ;
	char k ;

	string_t q = String( path ) ;

	index = StringIndexOfChar( q,0,'-' ) ;
	if( index == -1 ){
		/*
		 * no dash character means its not an lvm volume
		 */
		return q ;
	}else{
		if( StringContains( q,"--" ) ){
			/*
			 * path may look like "/dev/mapper/mariusvg-my--own--lv"
			 */
			z = StringPointer( q ) ;
			e = *z ;
			j = StringLength( q ) ;

			for( i = 1 ; i < j ; i++ ){
				c = *( e + i ) ;
				d = *( e + i + 1 ) ;
				k = *( e + i - 1 ) ;
				if( k != '-' && c == '-' && d != '-' ){
					/*
					 * found a place with a single dash,replace the dash with a slash and then
					 * replace all occurances of double dashes to single dashes
					 */
					StringSubChar( q,i,'/' ) ;
					while( StringHasComponent( *z,"--" ) ){
						StringReplaceString( q,"--","-" ) ;
					}
					break ;
				}
			}
			/*
			 * path will now look like "/dev/mapper/mariusvg/my-own-lv"
			 */
		}else{
			/*
			 * no double dashes,it could be a simple LVM path
			 */
			StringSubChar( q,index,'/' ) ;
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

		return q ;
	}
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
	}else if( StringPrefixMatch( path,"/dev/disk/by-",13 ) ){
		/*
		 * zuluCryptRealPath() is defined in real_path.c
		 */
		e = zuluCryptRealPath( path ) ;
		if( e == NULL ){
			return StringCopy_2( path ) ;
		}else{
			if( StringPrefixMatch( e,"/dev/mapper/",12 ) ){
				st = zuluCryptConvertIfPathIsLVM( e ) ;
				StringFree( e ) ;
				return StringDeleteHandle( &st ) ;
			}else{
				return e ;
			}
		}
	}else if( StringPrefixMatch( path,"/dev/mapper/",12 ) ){

		st = zuluCryptConvertIfPathIsLVM( path ) ;
		return StringDeleteHandle( &st ) ;

	}else if( StringPrefixMatch( path,"/dev/md",7 ) ){

		return zuluCryptResolveMDPath( path ) ;

	}else if( StringPrefixMatch( path,"/dev/dm-",8 ) ){

		return zuluCryptResolveDMPath( path ) ;

	}else if( StringPrefixMatch( path,"/dev/loop",9 ) ){
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
	if( StringPrefixMatch( path,"/dev/loop",9 ) ){
		return String( path ) ;
	}else{
		return zuluCryptResolvePath_1( path ) ;
	}
}