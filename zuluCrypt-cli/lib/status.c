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

#include <sys/ioctl.h>
#include <linux/loop.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <blkid/blkid.h>

char * status( const char * mapper )
{		
	#define SIZE 32
	char buffer[ SIZE + 1 ] ;
	const char * e ;
	char path[ 512 ] ;
	int fd ;
	struct loop_info64 l_info ;
	struct crypt_device * cd1 = NULL;
	struct crypt_device * cd;
	crypt_status_info csi ;
	struct crypt_active_device cad ;
	StrHandle * p ;
	
	crypt_init_by_name( &cd,mapper );
	crypt_get_active_device( cd1,mapper,&cad ) ;
	csi = crypt_status( cd, mapper );
	
	p = String( mapper ) ;
	
	switch( csi ){
		case CRYPT_INACTIVE :
			StringAppend( p," is inactive.\n" ) ; 	
			goto out ;
		case CRYPT_ACTIVE   : 
			StringAppend( p," is active.\n" ) ;
			break ;
		case CRYPT_BUSY     : 	
			StringAppend( p," is active and is in use.\n" ) ;
			break ;
		case CRYPT_INVALID  : 
			StringAppend( p," is invalid.\n" ) ;	
			goto out ;
	}	
	
	StringAppend( p," type:      " );
	e = crypt_get_type( cd ) ;
	
	if( strcmp( e,"LUKS1" ) == 0 )
		StringAppend( p,"luks1" ) ;
	else if( strcmp( e,"plain") )
		StringAppend( p,"plain" ) ;
	
	StringAppend( p,"\n cipher:    " );
	StringAppend( p,crypt_get_cipher_mode( cd ) ) ;
	StringAppend( p,"\n keysize:   " );
	StringAppend( p,StringIntToString( buffer,SIZE,8 * crypt_get_volume_key_size( cd ) ) ) ;
	StringAppend( p," bits" );
	StringAppend( p,"\n device:    " );
	e = crypt_get_device_name( cd ) ;
	StringAppend( p, e ) ;
	
	if( strncmp( e ,"/dev/loop",9 ) == 0 ){
		fd = open( e , O_RDONLY ) ;
		ioctl( fd, LOOP_GET_STATUS64, &l_info ) ;
		StringAppend( p,"\n loop:      " );
		realpath( ( char * ) l_info.lo_file_name, path ) ;
		StringAppend( p, path ) ;
		close( fd ) ;
	}
	
	StringAppend( p,"\n offset:    ");
	StringAppend( p,StringIntToString( buffer,SIZE,crypt_get_data_offset( cd ) ) )  ;
	StringAppend( p," sectors" ) ;	
	StringAppend( p,"\n size:      " );
	StringAppend( p,StringIntToString( buffer,SIZE,cad.size ) ) ;	
	StringAppend( p," sectors" );
	StringAppend( p,"\n mode:      " );
	
	if( cad.flags == 1 )
		StringAppend( p,"readonly" );
	else
		StringAppend( p,"read/write" );	
	
	out:
	crypt_free( cd );
	crypt_free( cd1 );
	return StringDeleteHandle( p ) ;
}

char * volume_device_name( const char * mapper )
{
	struct crypt_device * cd;
	char path[ 512 ] ;
	int i ;
	StrHandle * p ;
	const char * e ;
	struct loop_info64 l_info ;
	
	i = crypt_init_by_name( &cd,mapper );
	
	if( i < 0 )
		return NULL ;
	
	e = crypt_get_device_name( cd ) ;	
	
	if( strncmp( e ,"/dev/loop",9 ) == 0 ){
		i = open( e , O_RDONLY ) ;
		ioctl( i, LOOP_GET_STATUS64, &l_info ) ;
		close( i ) ;
		realpath( ( char * ) l_info.lo_file_name, path ) ;		
		p = String( path ) ;		
	}else
		p = String( e ) ;
	
	return StringDeleteHandle( p ) ;
}
