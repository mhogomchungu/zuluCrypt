/*
 *
 *  Copyright (c) 2012
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

int zuluMountCryptoUMount( ARGS * args )
{
	const char * device    = args->device ;
	const char * UUID      = args->uuid   ;
	uid_t        uid       = args->uid    ;
	int mount_point_option = args->mpo    ;

	const char * mapping_name ;
	char * path = NULL ;

	int st  ;

	string_t str = StringVoid ;

	if( mount_point_option ){;}

	if( UUID == NULL ){
		if( StringPrefixEqual( device,"/dev/loop" ) ){
			/*
			* zuluCryptLoopDeviceAddress_1() is defined in ../zuluCrypt-cli/create_loop_device.c
			*/
			path = zuluCryptLoopDeviceAddress_1( device ) ;
			if( path == NULL ){
				return 20 ;
			}else{
				device = path ;
			}
		}

		mapping_name = device + StringLastIndexOfChar_1( device,'/' ) + 1 ;
	}else{
		str = String( UUID ) ;
		StringRemoveString( str,"\"" ) ;
		mapping_name = StringSubChar( str,4,'-' ) ;
	}

	/*
	 * zuluCryptEXECloseVolume() is defined in ../zuluCrypt-cli/bin/close_volume.c
	 */
	st = zuluCryptEXECloseVolume( device,mapping_name,uid ) ;

	StringDelete( &str ) ;

	StringFree( path ) ;

	return st ;
}
