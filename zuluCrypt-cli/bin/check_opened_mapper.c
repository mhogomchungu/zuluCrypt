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
#include "includes.h"
#include <dirent.h>
#include <libcryptsetup.h>

int zuluCryptCheckOpenedMapper( const char * mapper )
{
	const char * c = mapper ;
	char * d ;

	int st = 0 ;

	struct dirent * entry ;

	DIR * dir = opendir( crypt_get_dir() ) ;

	if( dir == NULL ){
		return 2 ;
	}

	while( *++c != '-' ) { ; }
	while( *++c != '-' ) { ; }

	while( ( entry = readdir( dir ) ) != NULL ){

		d = strstr( entry->d_name,"zuluCrypt-" ) ;
		if( d != NULL ){

			while( *++d != '-' ) { ; }
			while( *++d != '-' ) { ; }

			if( strcmp( c,d ) == 0 ){
				st = 1 ;
				break ;
			}
		}
	}

	closedir( dir ) ;
	return st ;
}
