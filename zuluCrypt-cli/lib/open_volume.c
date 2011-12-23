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

int open_volume( const char * dev,const char * map,const char * m_point,uid_t id,const char * mode,const char * pass,const char * source ) 
{
	struct stat st ;
	int h ;
	int luks;
	
	if( stat( dev , &st ) != 0 ){		 
		return 3 ;
	}
	if( strcmp( source,"-f" ) == 0 )
		if( stat( pass, &st ) != 0 )
			return 6 ;
		
	if( stat( map,&st ) == 0 )
		return 2 ;	
		
	luks = is_luks( dev ) ;		
		
	if( luks == 0 )
		h = open_luks( dev,map,mode,source,pass ) ;
	else
		h = open_plain( dev,map,mode,source,pass,"cbc-essiv:sha256" ) ;
		
	switch ( h ){
		case 1 : return 4 ;
		case 2 : return 8 ; 
		case 3 : return 3 ;	 
	}
		
	h = mount_volume( map,m_point,mode,id ) ;	
		
	if( h == 4 && luks == 1 ){
		/*
		 * try to reopen a plain volume in legacy/compatibility mode
		 */
		open_plain( dev,map,mode,source,pass,"cbc-plain" ) ;
		h = mount_volume( map,m_point,mode,id ) ;
	}
	return h ;		
}

