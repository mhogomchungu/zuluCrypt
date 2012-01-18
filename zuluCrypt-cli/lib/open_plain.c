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

int open_plain( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size,const char * cipher )
{
	int status ;
	int flags ;
	struct crypt_device * cd;
	
	struct crypt_params_plain params = {
		.hash = "ripemd160",
		.skip = 0,
		.offset = 0,
	};
	
	if( is_path_valid( device ) == -1 )
		return 3 ;

	if( strcmp( mode,"ro" ) == 0 )
		flags = 1 ;
	else
		flags = 0 ;
	
	status = crypt_init( &cd, device ) ;	
	
	if( status != 0 ){
		status = 2 ;
		goto out ;
	}
	status = crypt_format( cd,CRYPT_PLAIN,"aes",cipher,NULL,NULL,32,&params );
	
	if( status != 0 ){
		status = 2 ;
		goto out ;
	}
	status = crypt_activate_by_passphrase( cd,mapper,CRYPT_ANY_SLOT,pass,pass_size,flags );
	
	if ( status < 0 )
		status = 2 ;
	else
		status = 0 ;
	out:
	crypt_free( cd );
	return status ;
}

