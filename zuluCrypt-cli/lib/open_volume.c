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

static int zuluExit( int x,string_t * p )
{
	StringDelete( p ) ;	
	return x ;
}

int zuluCryptOpenVolume( const char * dev,const char * map,const char * m_point,uid_t id,const char * mode,const char * pass,size_t pass_size ) 
{
	int h ;
	string_t p ;
	string_t * z = &p ;
	const char * mapper ;
	char * m ;
	
	if( zuluCryptIsPathValid( dev ) != 0 )		 
		return 3 ;
	
	p = String( crypt_get_dir() ) ;
	
	mapper = StringMultipleAppend( p,"/",map,'\0' ) ;

	if( zuluCryptIsPathValid( mapper ) == 0 )
		return zuluExit( 2,z ) ;	

	if( zuluCryptVolumeIsLuks( dev ) == 0 )
		h = zuluCryptOpenLuks( dev,map,mode,pass,pass_size ) ;
	else
		h = zuluCryptOpenPlain( dev,map,mode,pass,pass_size ) ;
		
	switch( h ){
		case 1 : return zuluExit( 4,z ) ;
		case 2 : return zuluExit( 8,z ) ; 
		case 3 : return zuluExit( 3,z ) ;	 
	}

	m = realpath( mapper,NULL ) ;
	if( m != NULL ){
		chown( m,0,0 ) ;
		chmod( m,S_IRWXU ) ;
		free( m ) ;
	}
	
	if( m_point != NULL ){	
		h = zuluCryptMountVolume( mapper,m_point,mode,id ) ;	
	
		if( h != 0 )
			if( zuluCryptCloseMapper( map ) != 0 )
				h = 15 ;
	}
	
	return zuluExit( h,z ) ;
}

