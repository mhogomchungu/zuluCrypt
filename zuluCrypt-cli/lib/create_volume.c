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

int create_volume( const char * dev,const char * fs,const char * type,const char * pass,size_t pass_size,const char * rng )
{
	string_t q ;
	int status ;
	
	if ( is_path_valid( dev ) == -1 )
		return 1 ;
		
	if( strcmp( type,"luks" ) == 0 )
		if(  strcmp( rng,"/dev/random" ) != 0 )
			if( strcmp( rng,"/dev/urandom" ) != 0 )
				return 2 ;
			
	if( strcmp( fs,"ext3" ) != 0 )
		if( strcmp( fs,"ext4" ) != 0 )
			if( strcmp( fs,"vfat" ) != 0 )
				return 2 ;
				
	if( strcmp( type,"luks" )  == 0 ){
		status = create_luks( dev,pass,pass_size,rng ) ;	
		if( status != 0 )
			return 3 ;
		status = open_luks( dev,"zuluCrypt-create-new","rw",pass,pass_size ) ;
		if( status != 0 )
			return 3 ;
	}else if( strcmp( type,"plain") == 0 ){
		status = open_plain( dev,"zuluCrypt-create-new","rw",pass,pass_size,"cbc-essiv:sha256" ) ;
		if( status != 0 )
			return 3 ;		
	}else{
		return 2 ;
	}		
	q = String( ZULUCRYPTmkfs );
	
	StringAppend( q , " -t ") ;
	StringAppend( q , fs ) ;
	
	if( strcmp( fs,"vfat") == 0 )
		StringAppend( q , " " ) ;
	else
		StringAppend( q , " -m 1 " ) ;
				
	StringAppend( q , "/dev/mapper/zuluCrypt-create-new" ) ;
	StringAppend( q , " 1>/dev/null 2>&1" ) ;
	execute( StringContent( q ),NULL,0 ) ;
	close_mapper( "/dev/mapper/zuluCrypt-create-new" );	
	StringDelete( q ) ;
	return 0 ;	
}

