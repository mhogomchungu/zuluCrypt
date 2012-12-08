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
#include <sys/syscall.h>

static inline int zuluExit( int st,struct crypt_device * cd )
{
	crypt_free( cd );
	return st ;
}

/*
 * 1 is returned if a volume is a truecrypt volume.
 * 0 is returned if a volume is not a truecrypt volume or functionality is not supported
 */
int zuluCryptVolumeIsTcrypt( const char * device,const char * key,size_t key_len )
{
#ifdef CRYPT_TCRYPT
	struct crypt_device * cd = NULL;
	struct crypt_params_tcrypt params ;
	
	memset( &params,'\0',sizeof( struct crypt_params_tcrypt ) ) ;
	
	params.passphrase      = key ;
	params.passphrase_size = key_len ;
	params.flags           = CRYPT_TCRYPT_LEGACY_MODES ;
	
	if( crypt_init( &cd,device ) < 0 )
		return 0 ;
	
	if( crypt_load( cd,CRYPT_TCRYPT,&params ) == 0 ){
		return zuluExit( 1,cd ) ;
	}else{
		return zuluExit( 0,cd ) ;
	}
#else
	return 0 ;
#endif
}

/*
 * 0 is returned if a volume was successfully opened.
 * 1 is returned if a volume was not successfully opened or functionality is not supported 
 */
int zuluCryptOpenTcrypt( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{
#ifdef CRYPT_TCRYPT
	uint32_t flags = 0 ;

	struct crypt_device * cd = NULL;
	struct crypt_params_tcrypt params ;
	
	memset( &params,'\0',sizeof( struct crypt_params_tcrypt ) ) ;
	
	params.passphrase = pass ;
	params.passphrase_size  = pass_size ;
	params.flags = CRYPT_TCRYPT_LEGACY_MODES ;
	
	if( strstr( mode,"ro" ) != NULL )
		flags |= CRYPT_ACTIVATE_READONLY;
	
	if( crypt_init( &cd,device ) < 0 )
		return 1 ;
	if( crypt_load( cd,CRYPT_TCRYPT,&params ) < 0 )
		return zuluExit( 1,cd ) ;
	if( crypt_activate_by_volume_key( cd,mapper,NULL,0,flags ) == 0 ){
		return zuluExit( 0,cd ) ;
	}else{
		return zuluExit( 1,cd ) ;
	}
#else
	return 1 ;
#endif
}
