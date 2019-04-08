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
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <libcryptsetup.h>
#include "includes.h"
#include "zuluplay_support.h"

typedef struct{

	const char * mapper ;
	int ( * function )( const char * mapper ) ;

}mapper_closer ;

static int _close_cryptsetup( const char * mapper )
{
	int r = 1 ;

	struct crypt_device * cd ;

	if( crypt_init_by_name( &cd,mapper ) == 0 ){

		r = crypt_deactivate( cd,mapper ) ;

		crypt_free( cd ) ;
	}

	return r ;
}

static int _close_dislocker( const char * mapper )
{
	int r ;

	string_t st = String( mapper ) ;

	const char * m = StringRemoveString( st,"/dislocker-file" ) ;

	/*
	 * doing it this way over calling umount() function is better
	 * because the mount tool cleans up /etc/mtab
	 */
	r = ProcessExecute( ZULUCRYPTumount,m,NULL ) ;

	if( r == 0 ){

		rmdir( m ) ;
	}

	StringDelete( &st ) ;

	return r ;
}

static int _close_mapper( const mapper_closer * m )
{
	int i ;
	int k = 1 ;

	for( i = 0 ; i < 5 ; i++ ){

		k = m->function( m->mapper ) ;

		if( k == 0 ){

			return 0 ;
		}else{
			sleep( 1 ) ;
		}
	}

	fprintf( stderr,"Trouble ahead, failed to remove encryption mapper: %s\n",m->mapper ) ;

	return k ;
}

int zuluCryptCloseMapper( const char * mapper )
{
	mapper_closer m = { mapper,NULL } ;

	/*
	 * zuluCryptTrueCryptOrVeraCryptVolume() is defined in status.c
	 */
	if( zuluCryptTrueCryptOrVeraCryptVolume( m.mapper ) ){

		m.function = tc_api_close_mapper ;

	}else if( zuluCryptBitLockerVolume( m.mapper ) ){

		m.function = _close_dislocker ;
	}else{
		m.function = _close_cryptsetup ;
	}

	return _close_mapper( &m ) ;
}
