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

int zuluMountCryptoUMount( const char * device,uid_t uid,__attribute__((unused)) int mount_point_option )
{
	const char * mapping_name ;
	const char * e = strrchr( device,'/' ) ;
	/*
	 * if( is_luks( device ) == 1 && check_if_partition_is_system_partition( device ) == 1 && uid != 0 )
	 *	return _zuluExit( 200,NULL,NULL,"ERROR: insuffienct privilege to operate on a system partition" ) ;
	 */
	if( e == NULL)
		mapping_name = device ;
	else
		mapping_name = e + 1 ;
	
	/*
	 * zuluCryptEXECloseVolume() is defined in ../zuluCrypt-cli/bin/close_volume.c
	 */
	return zuluCryptEXECloseVolume( device,mapping_name,uid ) ;
}
