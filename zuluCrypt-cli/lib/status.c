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

char * loop_device_address( const char * device )
{
	int fd ;
	char * path ;
	struct loop_info64 l_info ;
	
	fd = open( device , O_RDONLY ) ;
	ioctl( fd, LOOP_GET_STATUS64, &l_info ) ;
	path = realpath( ( char * ) l_info.lo_file_name,NULL ) ;
	close( fd ) ;
	return path ;
}

char * status( const char * mapper )
{		
	#define SIZE 33
	char buffer[ SIZE ] ;
	const char * e ;
	char * path ;
	
	int luks = 0 ;
	int i ;
	int j ;
	int k ;
	
	struct crypt_device * cd;
	struct crypt_active_device cad ;
	
	string_t properties = String( mapper ) ;
	
	crypt_init_by_name( &cd,mapper );
	crypt_get_active_device( NULL,mapper,&cad ) ;
	
	switch( crypt_status( cd, mapper ) ){
		case CRYPT_INACTIVE :
			StringAppend( properties," is inactive.\n" ) ; 	
			goto out ;
		case CRYPT_ACTIVE   : 
			StringAppend( properties," is active.\n" ) ;
			break ;
		case CRYPT_BUSY     : 	
			StringAppend( properties," is active and is in use.\n" ) ;
			break ;
		case CRYPT_INVALID  : 
			StringAppend( properties," is invalid.\n" ) ;	
			goto out ;
	}	
	
	StringAppend( properties," type:   \t" );	
	
	e = crypt_get_type( cd ) ;
	
	if( strcmp( e,"LUKS1" ) == 0 ){
		StringAppend( properties,"luks1" ) ;
		luks = 1 ;
	}else if( strcmp( e,"plain") )
		StringAppend( properties,"plain" ) ;
	
	StringAppend( properties,"\n cipher:\t" );
	StringAppend( properties,crypt_get_cipher_mode( cd ) ) ;
	StringAppend( properties,"\n keysize:\t" );
	StringAppend( properties,StringIntToString( buffer,SIZE,8 * crypt_get_volume_key_size( cd ) ) ) ;
	StringAppend( properties," bits" );
	StringAppend( properties,"\n device:\t" );
	
	e = crypt_get_device_name( cd ) ;
	StringAppend( properties, e ) ;
	
	if( strncmp( e ,"/dev/loop",9 ) == 0 ){
		StringAppend( properties,"\n loop:   \t" );
		path = loop_device_address( e ) ;
		if( path != NULL ){
			StringAppend( properties,path ) ;
			free( path ) ;
		}else
			StringAppend( properties,"NaN" ) ;		
	}
	StringAppend( properties,"\n offset:\t");
	StringAppend( properties,StringIntToString( buffer,SIZE,crypt_get_data_offset( cd ) ) )  ;
	StringAppend( properties," sectors" ) ;	
	StringAppend( properties,"\n size:   \t" );
	StringAppend( properties,StringIntToString( buffer,SIZE,cad.size ) ) ;	
	StringAppend( properties," sectors" );
	StringAppend( properties,"\n mode:   \t");
	
	if( cad.flags == 1 )
		StringAppend( properties,"read only" );
	else
		StringAppend( properties,"read and write" );	
	
	if( luks == 1 ){
		i = 0 ;
		k = crypt_keyslot_max( CRYPT_LUKS1 ) ;
		for( j = 0 ; j < k ; j++){
			switch( crypt_keyslot_status(cd, j) ){
				case CRYPT_SLOT_INACTIVE    :     ; break ;
				case CRYPT_SLOT_ACTIVE_LAST : i++ ; break ;
				case CRYPT_SLOT_ACTIVE      : i++ ; break ;
				case CRYPT_SLOT_INVALID     :     ; break ;				
			}		
		}
		StringAppend( properties,"\n active slots:\t");
		StringAppend( properties,StringIntToString( buffer,SIZE,i));
		StringAppend( properties," / ");
		StringAppend( properties,StringIntToString( buffer,SIZE,k));
	}
	out:
	crypt_free( cd );
	return StringDeleteHandle( &properties ) ;
}

char * volume_device_name( const char * mapper )
{
	struct crypt_device * cd;
	char * path ;
	int i ;
	string_t address ;
	const char * e ;
	
	i = crypt_init_by_name( &cd,mapper );
	
	if( i < 0 ){
		crypt_free( cd ) ;		
		return NULL ;
	}
	
	e = crypt_get_device_name( cd ) ;	
	
	if( strncmp( e ,"/dev/loop",9 ) == 0 ){
		path = loop_device_address( e ) ;
		address = StringInherit( &path ) ;	
	}else
		address = String( e ) ;
	
	crypt_free( cd ) ;
	
	return StringDeleteHandle( &address ) ;
}
