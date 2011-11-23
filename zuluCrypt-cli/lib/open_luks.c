/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

int open_luks( const char * device,const char * mapper,const char * mode,const char * source,const char * pass )
{
	struct stat st ;
	
	struct crypt_device *cd;
	
	uint32_t flags = 0;
	
	int status;
	
	const char * c ;	
	
	if( stat( device, &st ) != 0 )
		return 3 ;
	
	c = strrchr( mapper,'/' );
	
	/*
	 * second argument of "crypt_activate_by_passphrase" and "crypt_activate_by_keyfile"
	 * takes a name, not a path, below code will take the name or the last segment of the path
	 * making it irrelevant what a user passes to this function. 
	 * 
	 * blablabla will be taken as is
	 *\/dev/mapper/blablabla will be taken as blablabla only.These two functions will add they
	 *first part of the address.
	 * 
	 */	
	
	if( c == NULL )
		c = mapper ;
	else
		c++ ;
	
	status = crypt_init( &cd, device ) ;
	
	if ( status != 0 ){
		status = 2;	
		goto out ;
	}
	
	status = crypt_load( cd, CRYPT_LUKS1, NULL ) ;
	
	if ( status != 0 ){
		status = 2;	
		goto out ;
	}
	
	if( strcmp( mode,"ro" ) == 0 )
		flags = 1 ;
	else
		flags = 0 ;
	
	if(strcmp( source,"-p" )==0 ){
		
		status = crypt_activate_by_passphrase( cd,c,CRYPT_ANY_SLOT,pass,strlen( pass ),flags );
	}else{
		if ( stat( pass,&st ) != 0 ){
			status = 4;	
			goto out ;			
		}			
		
		status = crypt_activate_by_keyfile( cd,c,CRYPT_ANY_SLOT,pass,st.st_size,flags ) ;
	}
	
	if( status < 0 )
		status = 1 ;
	else
		status = 0 ;
	
	out:
	
	crypt_free( cd );
	
	return status ;
}

