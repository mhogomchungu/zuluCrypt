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

#include "includes.h"

int zuluCryptVolumeIsType( const char * device,const char * type )
{
	struct crypt_device * cd;
	int st ;
	if( crypt_init( &cd,device ) != 0 )
		return 0 ;
	st = crypt_load( cd,type,NULL ) ;
	crypt_free( cd );
	return st == 0 ;
}

int zuluCryptGetVolumeType( const char * device )
{
	if( zuluCryptVolumeIsType( device,CRYPT_LUKS1 ) ){
		return 1 ;
	}
#ifdef CRYPT_TCRYPT
	/*
	if( zuluCryptVolumeIsType( device,CRYPT_TCRYPT ) ){
		return 2 ;
	}
	*/
#endif
	return 3 ;
}

