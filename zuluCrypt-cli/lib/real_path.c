/*
 *
 *  Copyright (c) 2012-2015
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

#include <limits.h> /* PATH_MAX */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "includes.h"
#include <stdlib.h>

char * zuluCryptRealPath( const char * path )
{
	string_t st ;
	string_t xt ;

	char * e = realpath( path,NULL ) ;

	if( StringPrefixEqual( e,"/dev/dm-" ) ){

		st = String_1( "/sys/block/",e + 5,"/dm/name",NULL ) ;

		xt = StringGetFromVirtualFile( StringContent( st ) ) ;

		StringRemoveRight( xt,1 ) ;

		StringPrepend( xt,"/dev/mapper/" ) ;

		StringFree( e ) ;

		StringDelete( &st ) ;

		return StringDeleteHandle( &xt ) ;
	}else{
		return e ;
	}
}

int zuluCryptPathStartsWith( const char * path,const char * start )
{
	int st = 0 ;
	char * p = zuluCryptRealPath( path ) ;
	if( p != NULL ){
		st = StringPrefixEqual( p,start ) ;
		StringFree( p ) ;
	}
	return st ;
}

int zuluCryptPathDoesNotStartsWith( const char * path,const char * start )
{
	int st = 0;
	char * p = zuluCryptRealPath( path ) ;
	if( p != NULL ){
		st = StringPrefixEqual( p,start ) ;
		StringFree( p ) ;
	}
	return st == 0 ;
}

int zuluCryptPathDidNotChange( const char * path )
{
	char * p = zuluCryptRealPath( path ) ;
	int st = 0 ;
	if( p != NULL ){
		st = StringsAreEqual( path,p ) ;
		StringFree( p ) ;
	}
	return st ;
}

int zuluCryptPathDeviceIsBlockDevice( const char * device )
{
	if( device ){;}
	return 0 ;
}
