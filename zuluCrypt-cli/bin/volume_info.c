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
#include <stdio.h>
#include <libintl.h>

int zuluCryptEXEVolumeInfo( const char * mapper,const char * device,uid_t uid )
{
	char * output ;
	int xt ;

	/*
	 * ZULUCRYPTlongMapperPath is set in ../constants.h
	 * zuluCryptCreateMapperName() is defined at ../lib/create_mapper_name.c
	 */
	string_t p = zuluCryptCreateMapperName( device,mapper,uid,ZULUCRYPTlongMapperPath ) ;

	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 *zuluCryptVolumeStatus() is defined in ../lib/status.c
	 */
	output = zuluCryptVolumeStatus( StringContent( p ) ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	if( output != NULL ){
		printf( "%s\n",output ) ;
		free( output ) ;
		xt = 0 ;
	}else{
		printf( gettext( "ERROR: could not get volume properties,volume is not open or was opened by a different user\n" ) ) ;
		xt = 2 ;
	}
	StringDelete( &p ) ;
	return xt ;
}

