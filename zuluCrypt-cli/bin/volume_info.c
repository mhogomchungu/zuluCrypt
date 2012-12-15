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

int zuluCryptEXEVolumeInfo( const char * mapper,const char * device,uid_t uid )
{
	char * dev ;
	char * output ;
	int xt ;
	
	string_t p ;

	dev = zuluCryptRealPath( device ) ;
	if( dev == NULL ){
		printf( "ERROR: full device path could not get resolved\n" ) ;
		return 1 ;
	}
		
	/*
	 * This function is defined at "create_mapper_name.c"
	 * 
	 * Explanation for what it does is explained where it is defined.
	 */
	p = zuluCryptCreateMapperName( dev,mapper,uid,CLOSE ) ;
	if( !zuluCryptSecurityGainElevatedPrivileges() ){
		printf( "ERROR: could get elevated privileges,check binary permissions\n" ) ;
		return 3 ;
	}
		
	output = zuluCryptVolumeStatus( StringContent( p ) ) ;
	
	if( output != NULL ){
		printf( "%s\n",output );
		free( output ) ;
		xt = 0 ;
	}else{
		printf( "ERROR: could not get volume info,is the volume opened?\n" ) ;
		xt = 2 ;
	}
	zuluCryptSecurityDropElevatedPrivileges() ;
	StringDelete( &p );
	free( dev ) ;
	return xt ;
}

