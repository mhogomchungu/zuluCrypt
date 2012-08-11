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
#include <sys/statvfs.h>

/*
 * 64 byte buffer is more than enough because the API that will produce the largest number is crypt_get_data_offset()
 * and it produces a uint64_t number and this type has a maximum digit count is 19. 
 */
#define SIZE 64

char * zuluCryptGetMountPointFromPath( const char * path ) ;

static void format_size_1( char * buffer,int x,const char * z )
{
	buffer[ x ] = buffer[ x - 1 ] ; 
	buffer[ x - 1 ] = '.' ;
	strcpy( buffer + x + 1,z ) ; 
}

void zuluCryptFormatSize( char * buffer,const char * buff )
{
	strcpy( buffer,buff ) ;
	
	switch( strlen( buff ) ){
		case 0 : 
		case 1 :  
		case 2 : 
		case 3 : strcat( buffer," B" )             ; break ;
		case 4 : format_size_1( buffer,2," KB" ) ; break ; 
		case 5 : format_size_1( buffer,3," KB" ) ; break ;
		case 6 : format_size_1( buffer,4," KB" ) ; break ;
		case 7 : format_size_1( buffer,2," MB" ) ; break ; 
		case 8 : format_size_1( buffer,3," MB" ) ; break ;
		case 9 : format_size_1( buffer,4," MB" ) ; break ;
		case 10: format_size_1( buffer,2," GB" ) ; break ;
		case 11: format_size_1( buffer,3," GB" ) ; break ;
		case 12: format_size_1( buffer,4," GB" ) ; break ;
		case 13: format_size_1( buffer,2," TB" ) ; break ;
		case 14: format_size_1( buffer,3," TB" ) ; break ;
		case 15: format_size_1( buffer,4," TB" ) ; break ;			
	}	
}

static void file_system_properties( string_t p,const char * mapper,const char * m_point )
{
	const char * e ;	
	blkid_probe blkid ;	
	struct statvfs vfs ;
	uint64_t total ;
	uint64_t used ;
	uint64_t free ;
	uint32_t block_size ;
	char buff[ SIZE ] ;	
	char * buffer = buff ;
	char format[ SIZE ] ;
	
	blkid = blkid_new_probe_from_filename( mapper ) ;
	blkid_do_probe( blkid );
	
	if( blkid_probe_lookup_value( blkid,"TYPE",&e,NULL ) == 0 )
		StringMultipleAppend( p,"\n file system:\t",e,'\0' ) ;
	else
		StringAppend( p,"\n file system:\tNil" ) ;
	
	blkid_free_probe( blkid );	
	
	if( statvfs( m_point,&vfs ) != 0 )
		return ;
	
	block_size = vfs.f_frsize ;
	total = block_size * vfs.f_blocks  ;
	free =  block_size * vfs.f_bavail  ;
	
	used = total - free ;
	
	e = StringIntToString_1( buffer,SIZE,total ) ;
	zuluCryptFormatSize( format,e ) ;	
	StringMultipleAppend( p,"\n total space:\t",format,'\0' ) ;
	
	e = StringIntToString_1( buffer,SIZE,used )  ;
	zuluCryptFormatSize( format,e ) ;
	StringMultipleAppend( p,"\n used space:\t",format,'\0' ) ;
	
	e = StringIntToString_1( buffer,SIZE,free ) ;
	zuluCryptFormatSize( format,e ) ;
	StringMultipleAppend( p,"\n free space:\t",format,'\0' ) ;
	
	snprintf( buff,SIZE,"%.2f%%",100 * ( ( float ) used / ( float ) total ) ) ;
	StringMultipleAppend( p,"\n used%:   \t",buff,'\0' ) ;
	
	StringMultipleAppend( p,"\n mount point:\t",m_point,'\0' ) ;
}

static char * zuluCryptLoopDeviceAddress( const char * device )
{
	int fd ;
	char * path ;
	struct loop_info64 l_info ;
	
	memset( &l_info,'\0',sizeof( struct loop_info64 ) ) ;
	
	fd = open( device,O_RDONLY ) ;
	ioctl( fd,LOOP_GET_STATUS64,&l_info ) ;
	path = realpath( ( char * ) l_info.lo_file_name,NULL ) ;
	close( fd ) ;
	return path ;
}

char * zuluCryptVolumeStatus( const char * mapper )
{	
	char buff[ SIZE ] ;	
	char * buffer = buff ;
	const char * z ;
	const char * e ;
	const char * type ;
	char * path ;
	int luks = 0 ;
	int i ;
	int j ;
	int k ;	
	
	struct crypt_device * cd;
	struct crypt_active_device cad ;
	
	string_t p ;
	
	if( crypt_init_by_name( &cd,mapper ) != 0 )
		return NULL ;
	
	if( crypt_get_active_device( NULL,mapper,&cad ) != 0 ){
		crypt_free( cd ) ;
		return NULL ;
	}
	
	p = String( mapper ) ;
	
	switch( crypt_status( cd,mapper ) ){
		case CRYPT_INACTIVE :
			StringAppend( p," is inactive.\n" ) ; 	
			crypt_free( cd );
			return StringDeleteHandle( &p ) ;
		case CRYPT_ACTIVE   : 
			StringAppend( p," is active.\n" ) ;
			break ;
		case CRYPT_BUSY     : 	
			StringAppend( p," is active and is in use.\n" ) ;
			break ;
		case CRYPT_INVALID  : 
			StringAppend( p," is invalid.\n" ) ;	
			crypt_free( cd );
			return StringDeleteHandle( &p ) ;		
	}	
	
	StringAppend( p," type:   \t" );	
	type = crypt_get_type( cd ) ;
	
	if( strncmp( type,"LUKS",4 ) == 0 ){
		if( strcmp( type,"LUKS1" ) == 0 )
			StringAppend( p,"luks1" ) ;
		else{
			/*
			 * future versions of luks will go here.
			 * "LUKS" in capital letters sticks out when displaying volume properties in the GUI.
			 *  Thats why they are converted to small letters in this conditional block.
			 */
			;
		}
		luks = 1 ;
	}else if( strcmp( type,"plain") )
		StringAppend( p,"plain" ) ;
	
	StringMultipleAppend( p,"\n cipher:\t",crypt_get_cipher_mode( cd ),'\0' );
	
	z = StringIntToString_1( buffer,SIZE,8 * crypt_get_volume_key_size( cd ) ) ;
	StringMultipleAppend( p,"\n keysize:\t",z," bits",'\0' );

	e = crypt_get_device_name( cd ) ;	
	StringMultipleAppend( p,"\n device:\t",e,'\0' );	
		
	if( strncmp( e,"/dev/loop",9 ) == 0 ){
		StringAppend( p,"\n loop:   \t" );
		path = zuluCryptLoopDeviceAddress( e ) ;		
		if( path != NULL ){
			StringAppend( p,path ) ;
			free( path ) ;
		}else{
			StringAppend( p,"Nil" ) ;
		}
	}else{
		StringAppend( p,"\n loop:   \tNil" ) ;
	}		
	
	z = StringIntToString_1( buffer,SIZE,crypt_get_data_offset( cd ) ) ;
	StringMultipleAppend( p,"\n offset:\t",z," sectors",'\0' );
	
	z = StringIntToString_1( buffer,SIZE,cad.size ) ;
	StringMultipleAppend( p,"\n size:   \t",z," sectors",'\0' );
	
	if( cad.flags == 1 )
		StringAppend( p,"\n mode:   \tread only" );
	else
		StringAppend( p,"\n mode:   \tread and write" );	
	
	if( luks == 1 ){
		i = 0 ;
		k = crypt_keyslot_max( type ) ;
		for( j = 0 ; j < k ; j++){
			switch( crypt_keyslot_status(cd, j) ){
				case CRYPT_SLOT_INACTIVE    :     ; break ;
				case CRYPT_SLOT_ACTIVE_LAST : i++ ; break ;
				case CRYPT_SLOT_ACTIVE      : i++ ; break ;
				case CRYPT_SLOT_INVALID     :     ; break ;				
			}		
		}
		
		StringMultipleAppend( p,"\n active slots:\t",StringIntToString_1( buffer,SIZE,i ),'\0' );
		
		StringMultipleAppend( p," / ",StringIntToString_1( buffer,SIZE,k ),'\0' );
	}else{
		StringAppend( p,"\n active slots:\tNil" );
	}
	
	/*
	 * defined in ../bin/check_mounted_volumes.c
	 * The function returns a mount point path given a path representing a device 
	 */
	path = zuluCryptGetMountPointFromPath( mapper ) ;
	if( path != NULL ){
		file_system_properties( p,mapper,path ) ; 
		free( path ) ;
	}
	
	crypt_free( cd );
	return StringDeleteHandle( &p ) ;
}

char * zuluCryptVolumeDeviceName( const char * mapper )
{
	struct crypt_device * cd;
	char * path ;
	string_t address ;
	const char * e ;
	
	if( strncmp( mapper,crypt_get_dir(),strlen( crypt_get_dir() ) != 0 ) )
		return NULL ;
	
	if( crypt_init_by_name( &cd,mapper ) < 0 )
		return NULL ;
	
	e = crypt_get_device_name( cd ) ;	
	
	if( strncmp( e,"/dev/loop",9 ) == 0 ){
		path = zuluCryptLoopDeviceAddress( e ) ;
		if( path != NULL )
			address = StringInherit( &path ) ;
		else
			address = String( "Nil" ) ;
	}else
		address = String( e ) ;
	
	crypt_free( cd ) ;
	
	return StringDeleteHandle( &address ) ;
}
