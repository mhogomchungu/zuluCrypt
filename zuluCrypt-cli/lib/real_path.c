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

#include <limits.h> /* PATH_MAX */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "includes.h"

#include "canonicalize/canonicalize.h"

/*
static char * _private_real_path( const char * path )
{
	if( path ){;}
	return NULL ;
}
*/
/*
 * canonicalize_path() is defined in canonicalize/canonicalize.c
 */
char * zuluCryptRealPath( const char * path )
{
	return canonicalize_path( path ) ;
}

int zuluCryptPathStartsWith( const char * path,const char * start )
{
	int st = 0 ;
	char * p = canonicalize_path( path ) ;
	if( p != NULL ){
		st = StringPrefixEqual( p,start ) ;
		free( p ) ;
	}
	return st ;
}

int zuluCryptPathDoesNotStartsWith( const char * path,const char * start )
{
	int st = 0;
	char * p = canonicalize_path( path ) ;
	if( p != NULL ){
		st = StringPrefixEqual( p,start ) ;
		free( p ) ;
	}
	return st == 0 ;
}

int zuluCryptPathDidNotChange( const char * path )
{
	char * p = canonicalize_path( path ) ;
	int st = 0 ;
	if( p != NULL ){
		st = StringsAreEqual( path,p ) ;
		free( p ) ;
	}
	return st ;
}

int zuluCryptPathDeviceIsBlockDevice( const char * device )
{
	if( device ){;}
	return 0 ;
}
