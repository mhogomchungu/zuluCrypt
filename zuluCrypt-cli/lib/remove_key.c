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

int remove_key( const char * device ,const char * pass )
{       
	int status ;
	
	struct crypt_device * cd;
	
	if( is_luks( device ) == 1 )
		return 1 ;      
	
	status = crypt_init( &cd,device ) ;
	
	if( status != 0 ){
		status =  3 ;
		goto out ;
	}
	
	status = crypt_load( cd, CRYPT_LUKS1, NULL ) ;
	
	if( status != 0 ){
		status =  3 ;
		goto out ;
	}
	
	status =  crypt_activate_by_passphrase( cd,NULL,CRYPT_ANY_SLOT,pass,strlen( pass ),0 );
	
	if ( status < 0 ){
		status = 2 ;
		goto out ;
	}
	
	status = crypt_keyslot_destroy( cd,status ) ;
	
	if ( status < 0 )
		status = 2 ;    
	else
		status = 0 ;
	
	out:
	crypt_free( cd );               
	
	return status ;
}

