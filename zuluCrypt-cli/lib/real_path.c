 
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

char * zuluCryptRealPath( const char * path )
{
	return realpath( path,NULL ) ;
}

int zuluCryptPathStartsWith( const char * path,const char * start )
{
	int st ;
	char * p = realpath( path,NULL ) ;
	if( p == NULL )
		return 0 ;
	st = strcmp( p,start ) ;
	free( p ) ;
	return st == 0 ;
}

int zuluCryptPathDoesNotStartsWith( const char * path,const char * start )
{
	int st ;
	char * p = realpath( path,NULL ) ;
	if( p == NULL )
		return 0 ;
	st = strcmp( p,start ) ;
	free( p ) ;
	return st != 0 ;
}

int zuluCryptPathDidNotChange( const char * path )
{
	char * p = realpath( path,NULL ) ;
	int st = 0 ;
	if( p == NULL ){
		return 0 ;
	}else{
		st = strcmp( path,p ) ;
		free( p ) ;
	}
	return st == 0 ;
}

int zuluCryptPathDeviceIsBlockDevice( const char * device )
{
	struct stat st ;
	if( strncmp( device,"/dev/shm/",9 ) == 0 )
		return 0 ;
	stat( device,&st ) ;
	return S_ISBLK( st.st_mode ) ;
}
