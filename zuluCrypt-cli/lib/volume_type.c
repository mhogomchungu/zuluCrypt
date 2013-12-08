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

#ifdef CRYPT_TCRYPT
int zuluCryptGetVolumeType( const char * device,const char * pass,size_t pass_size )
{
	/*
	 * zuluCryptVolumeIsLuks()   is defined in is_luks.c
	 * zuluCryptVolumeIsTcrypt() is defined in open_tcrypt.c
	 */
	if( zuluCryptVolumeIsLuks( device ) ){
		return 1 ;
	}else if( zuluCryptVolumeIsTcrypt( device,pass,pass_size ) ){
		return 2 ;
	}else{
		return 3 ;
	}
}
#else
int zuluCryptGetVolumeType( const char * device,const char * pass __attribute__((unused)),size_t pass_size __attribute__((unused)) )
{
	/*
	 * zuluCryptVolumeIsLuks() is defined in is_luks.c
	 */
	if( zuluCryptVolumeIsLuks( device ) ){
		return 1 ;
	}else{
		return 3 ;
	}
}
#endif

