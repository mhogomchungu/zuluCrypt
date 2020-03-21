/*
 *
 *  Copyright (c) 2011-2015
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

#include <libcryptsetup.h>
#include <fcntl.h>
#include <unistd.h>

static int _is_luks( const char * dev,const resolve_path_t * opts )
{
	struct crypt_device * cd ;
	int st ;

	if( opts ){}

	if( crypt_init( &cd,dev ) != 0 ){

		return 0 ;
	}

	st = crypt_load( cd,NULL,NULL ) ;

	crypt_free( cd ) ;

	return st == 0 ;
}

int zuluCryptVolumeIsLuks( const char * device )
{
	/*
	 * resolve_path_t is defined in includes.h
	 */
	resolve_path_t opts ;

	memset( &opts,'\0',sizeof( opts ) ) ;

	opts.device       = device ;
	opts.error_value  = 0 ;

	/*
	 * zuluCryptResolveDevicePath() is defined in resolve_path.c
	 */
	return zuluCryptResolveDevicePath( _is_luks,&opts ) ;
}

int zuluCryptVolumeIsNotLuks( const char * dev )
{
	return !zuluCryptVolumeIsLuks( dev ) ;
}
