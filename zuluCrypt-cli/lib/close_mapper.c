/*
 * 
 *  Copyright (c) 2011
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

/*
 * this header is created at config time
 */
#include "truecrypt_support.h"

static int _close_mapper( const char * mapper )
{
#if TRUECRYPT_TCPLAY
	tc_api_opts api_opts ;
	int r = TC_ERR ;
	int j ;
	memset( &api_opts,'\0',sizeof( api_opts ) ) ;
	api_opts.tc_map_name = mapper ;
	tc_api_init( 0 ) ;
	for( j = 0 ; j < 3 ; j++ ) { 
		/*
		 * try multiple types to close the mapper just in case
		 */
		if( tc_api_unmap_volume( &api_opts ) == TC_OK ){
			r = TC_OK ;
			break ;
		}else{
			r = TC_ERR ;
			sleep( 1 ) ;
		}
	}
	tc_api_uninit() ;
	return r == TC_OK ? 0 : 1 ;
#else
	if( mapper ){;}
	return 1 ;
#endif
}

int zuluCryptCloseMapper( const char * mapper )
{
	int j ;
	int st = 1;
	struct crypt_device * cd;
	if( crypt_init_by_name( &cd,mapper ) == 0 ){
		if( crypt_get_type( cd ) == NULL ){
			/*
			 * We will get here if the volume is truecrypt volume opened with tcplay
			 */
			crypt_free( cd ) ;
			return _close_mapper( mapper ) ;
		}
		for( j = 0 ; j < 3 ; j++ ) { 
			/*
			* try multiple types to close the mapper just in case
			*/
			if( crypt_deactivate( cd,mapper ) == 0 ){
				st = 0 ;
				break ;
			}else{
				sleep( 1 ) ;
			}
		}
		crypt_free( cd ) ;
	}else{
		return _close_mapper( mapper ) ;
	}
	
	return st ;
}
