/*
 *
 *  Copyright (c) 2014
 *  name : Francis Banyikwa
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

static string_t zuluExit( DIR * dir,string_t st )
{
	if( dir != NULL ){

		closedir( dir ) ;
	}

	return st ;
}

/*
 * raid path can be in format /dev/mdX or /dev/md/X.
 * We prefer the latter and if given the former,convert it to the latter if possible
 */
string_t zuluCryptResolveMDPath_1( const char * path )
{
	struct dirent * entry ;

	char * e ;

	const char * f = "/dev/md/" ;

	DIR * dir = opendir( f ) ;

	string_t st = String( f ) ;

	if( dir != NULL ){

		while( ( entry = readdir( dir ) ) != NULL ){

			f = entry->d_name ;

			if( !StringAtLeastOneMatch_1( f,".","..",NULL ) ){

				e = zuluCryptRealPath( StringAppendAt( st,8,f ) ) ;

				if( StringsAreEqual( path,e ) ){

					StringFree( e ) ;

					return zuluExit( dir,st ) ;
				}else{
					StringFree( e ) ;
				}
			}
		}
	}

	StringReplace( st,path ) ;

	return zuluExit( dir,st ) ;
}

char * zuluCryptResolveMDPath( const char * path )
{
	string_t st = zuluCryptResolveMDPath_1( path ) ;
	return StringDeleteHandle( &st ) ;
}

/*
 * An assumption is made here that the path is an LVM path if "path"
 * is in /dev/mapper/abc-def format and there exist a path at /dev/abc/def.
 *
 * Info in lvm path structures can be found here:
 * https://www.redhat.com/archives/linux-lvm/2014-January/msg00014.html
 */
string_t zuluCryptConvertIfPathIsLVM( const char * path )
{
	const char * e ;
	const char ** z ;

	struct stat st ;

	StringIterator a ;
	StringIterator b ;
	StringIterator c ;
	StringIterator d ;

	string_t q = String( path ) ;

	StringGetIterators( q,&c,&d ) ;

	for( c = c + 3 ; c < d ; c++ ){

		a = c - 2 ;
		b = c - 1 ;

		if( *a != '-' && *b == '-' && *c != '-' ){
			/*
			 * found a place with a single dash,replace the dash with a slash
			 */
			*b = '/' ;
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
				 * The path appear to be an LVM path since
				 * "/dev/mapper/ABC-DEF" input path has a corresponding
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

static char * _convert_if_path_is_lvm( const char * path )
{
	string_t st = zuluCryptConvertIfPathIsLVM( path ) ;
	return StringDeleteHandle( &st ) ;
}

/*
 * dm path is a path like "/dev/dm-5".
 * this routine will transform the path to /dev/abc/def if the path is
 * an lvm path or to /dev/mapper/xyz if the volume is any other device manager volume.
 */
char * zuluCryptResolveDMPath( const char * path )
{
	char * e = zuluCryptRealPath( path ) ;

	char * f = _convert_if_path_is_lvm( e ) ;

	StringFree( e ) ;

	return f ;
}

char * zuluCryptResolvePath( const char * path )
{
	char * e ;
	char * f ;

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

				f = _convert_if_path_is_lvm( e ) ;
				StringFree( e ) ;

				return f ;
			}else{
				return e ;
			}
		}
	}else if( StringPrefixEqual( path,"/dev/mapper/" ) ){

		return _convert_if_path_is_lvm( path ) ;

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
