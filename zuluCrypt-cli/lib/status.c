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
#include <dirent.h>

/*
 * 64 byte buffer is more than enough because the API that will produce the largest number is crypt_get_data_offset()
 * and it produces a uint64_t number and this type has a maximum digit count is 19. 
 */
#define SIZE 64


/*
 * zuluCryptLoopDeviceAddress() is moved to create_loop.c
 */

char * zuluCryptGetMountPointFromPath( const char * path ) ;

char * zuluCryptVolumeDeviceName( const char * mapper ) ;

static void zuluCryptFormatSize_1( char * buffer,int x,const char * z )
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
		case 3 : strcat( buffer," B" )          	 ; break ;
		case 4 : zuluCryptFormatSize_1( buffer,2," KB" ) ; break ; 
		case 5 : zuluCryptFormatSize_1( buffer,3," KB" ) ; break ;
		case 6 : zuluCryptFormatSize_1( buffer,4," KB" ) ; break ;
		case 7 : zuluCryptFormatSize_1( buffer,2," MB" ) ; break ; 
		case 8 : zuluCryptFormatSize_1( buffer,3," MB" ) ; break ;
		case 9 : zuluCryptFormatSize_1( buffer,4," MB" ) ; break ;
		case 10: zuluCryptFormatSize_1( buffer,2," GB" ) ; break ;
		case 11: zuluCryptFormatSize_1( buffer,3," GB" ) ; break ;
		case 12: zuluCryptFormatSize_1( buffer,4," GB" ) ; break ;
		case 13: zuluCryptFormatSize_1( buffer,2," TB" ) ; break ;
		case 14: zuluCryptFormatSize_1( buffer,3," TB" ) ; break ;
		case 15: zuluCryptFormatSize_1( buffer,4," TB" ) ; break ;
	}	
}

char * zuluCryptGetUUIDFromMapper( const char * mapper )
{
	string_t p ;
	blkid_probe blkid ;
	const char * uuid ;
	
	char * device = zuluCryptVolumeDeviceName( mapper ) ;
	
	blkid = blkid_new_probe_from_filename( device ) ;
	
	if( blkid == NULL ){
		p = String( " UUID:   \t\"Nil\"" ) ;
		return StringDeleteHandle( &p ) ;
	}
		
	blkid_do_probe( blkid );
	
	if( blkid_probe_lookup_value( blkid,"UUID",&uuid,NULL ) == 0 ){
		p = String( "" ) ;
		StringMultipleAppend( p," UUID:   \t\"",uuid,"\"",END ) ;
	}else{
		p = String( " UUID:   \t\"Nil\"" ) ;
	}
	
	blkid_free_probe( blkid );
	
	free( device ) ;
	
	return StringDeleteHandle( &p ) ;
}

static void zuluCryptFileSystemProperties( string_t p,const char * mapper,const char * m_point )
{
	const char * e ;
	blkid_probe blkid ;
	struct statvfs vfs ;
	uint64_t total ;
	uint64_t used ;
	uint64_t unused ;
	uint64_t block_size ;
	char buff[ SIZE ] ;
	char * buffer = buff ;
	char format[ SIZE ] ;
	
	blkid = blkid_new_probe_from_filename( mapper ) ;
	
	if( blkid == NULL )
		return ;
	
	blkid_do_probe( blkid );
	
	if( blkid_probe_lookup_value( blkid,"TYPE",&e,NULL ) == 0 )
		StringMultipleAppend( p,"\n file system:\t",e,END ) ;
	else
		StringAppend( p,"\n file system:\tNil" ) ;
	
	blkid_free_probe( blkid );
	
	if( statvfs( m_point,&vfs ) != 0 )
		return ;
	
	block_size = vfs.f_frsize ;
	total = block_size * vfs.f_blocks  ;
	unused =  block_size * vfs.f_bavail  ;
	
	used = total - unused ;
	
	e = StringIntToString_1( buffer,SIZE,total ) ;
	zuluCryptFormatSize( format,e ) ;
	StringMultipleAppend( p,"\n total space:\t",format,END ) ;
	
	e = StringIntToString_1( buffer,SIZE,used )  ;
	zuluCryptFormatSize( format,e ) ;
	StringMultipleAppend( p,"\n used space:\t",format,END ) ;
	
	e = StringIntToString_1( buffer,SIZE,unused ) ;
	zuluCryptFormatSize( format,e ) ;
	StringMultipleAppend( p,"\n free space:\t",format,END ) ;
	
	snprintf( buff,SIZE,"%.2f%%",100 * ( ( float ) used / ( float ) total ) ) ;
	StringMultipleAppend( p,"\n used%:   \t",buff,"\n",END ) ;
		
	buffer = zuluCryptGetUUIDFromMapper( mapper ) ;
	StringAppend( p,buffer );
	
	free( buffer ) ;
	
	StringMultipleAppend( p,"\n mount point:\t",m_point,END ) ;
}

char * zuluCryptGetVolumeTypeFromMapperPath( const char * mapper )
{
	struct crypt_device * cd;
	const char * type ;
	char * volType = NULL ;
	
	type = crypt_get_dir() ;
	
	if( strncmp( mapper,type,strlen( type ) != 0 ) ){
		return strdup( "Nil" ) ;
	}
	
	if( crypt_init_by_name( &cd,mapper ) < 0 ){
		return strdup( "Nil" ) ;
	}
	
	type = crypt_get_type( cd ) ;
	
	if( type == NULL ){
		volType = strdup( "Nil" ) ;
	}else{
		if( strstr( type,"LUKS" ) != NULL ){
			volType = strdup( "crypto_LUKS" ) ;
		}else if( strstr( type,"PLAIN" ) != NULL ){
			volType = strdup( "crypto_PLAIN" ) ;
		}else if( strstr( type,"TCRYPT" ) != NULL ){
			volType = strdup( "crypto_TCRYPT" ) ;
		}else{
			volType = strdup( "Nil" ) ;
		}
	}
	
	crypt_free( cd ) ;
	return volType ;
}

char * zuluCryptVolumeStatus( const char * mapper )
{	
	char buff[ SIZE ] ;
	char * buffer = buff ;
	const char * z ;
	const char * type ;
	const char * device_name ;
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
	
	device_name = crypt_get_device_name( cd ) ;
	if( device_name == NULL ){
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
	
	if( type == NULL ){
		StringAppend( p,"Nil" ) ;
	}else{
		if( strncmp( type,"LUKS",4 ) == 0 ){
			luks = 1 ;
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
		}else if( strcmp( type,"PLAIN" ) == 0 ){
			StringAppend( p,"plain" ) ;
		}else if( strcmp( type,"TCRYPT" ) == 0 ){
			StringAppend( p,"tcrypt" ) ;
		}else{
			StringAppend( p,"Nil" ) ;
		}
	}
	
	StringMultipleAppend( p,"\n cipher:\t",crypt_get_cipher_mode( cd ),END );
	
	z = StringIntToString_1( buffer,SIZE,8 * crypt_get_volume_key_size( cd ) ) ;
	StringMultipleAppend( p,"\n keysize:\t",z," bits",END );

	StringMultipleAppend( p,"\n device:\t",device_name,END );
		
	if( strncmp( device_name,"/dev/loop",9 ) == 0 ){
		StringAppend( p,"\n loop:   \t" );
		path = zuluCryptLoopDeviceAddress( device_name ) ;
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
	StringMultipleAppend( p,"\n offset:\t",z," sectors",NULL );
	
	if( cad.flags == 1 )
		StringAppend( p,"\n mode:   \tread only" );
	else
		StringAppend( p,"\n mode:   \tread and write" );
	
	if( luks == 1 ){
		i = 0 ;
		k = crypt_keyslot_max( type ) ;
		for( j = 0 ; j < k ; j++ ){
			switch( crypt_keyslot_status( cd,j ) ){
				case CRYPT_SLOT_INACTIVE    :     ; break ;
				case CRYPT_SLOT_ACTIVE_LAST : i++ ; break ;
				case CRYPT_SLOT_ACTIVE      : i++ ; break ;
				case CRYPT_SLOT_INVALID     :     ; break ;
			}
		}
		
		StringMultipleAppend( p,"\n active slots:\t",StringIntToString_1( buffer,SIZE,i ),END );
		
		StringMultipleAppend( p," / ",StringIntToString_1( buffer,SIZE,k ),END );
	}else{
		StringAppend( p,"\n active slots:\tNil" );
	}
	
	/*
	 * zuluCryptGetMountPointFromPath() is defined in ./print_mounted_volumes.c
	 */
	path = zuluCryptGetMountPointFromPath( mapper ) ;
	if( path != NULL ){
		zuluCryptFileSystemProperties( p,mapper,path ) ; 
		free( path ) ;
	}
	
	crypt_free( cd );
	return StringDeleteHandle( &p ) ;
}

static char * zuluCryptVolumeDeviceName_1( const char * mapper )
{
	ssize_t len ;
	string_t st = StringVoid ;
	DIR * dir ;
	struct dirent * entry ;
	const char * path ;
	string_t xt ;
	#define MAPPER_SIZE 32
	char buffer[ MAPPER_SIZE ] ;
	char * result = NULL ;

	len = readlink( mapper,buffer,MAPPER_SIZE - 1 ) ;
	
	if( len < 0 ){
		return NULL ;
	}else{
		buffer[ len ] = '\0' ;
	}
	
	st = String( buffer ) ;
	StringReplaceString( st,"../","/sys/block/" ) ;
	path = StringAppend( st,"/slaves" ) ;
	
	dir = opendir( path ) ;
	if( dir == NULL ){
		StringDelete( &st ) ;
		return NULL ;
	}
	
	while( ( entry = readdir( dir ) ) != NULL ){
		path = entry->d_name ;
		if( strcmp( path,"." ) == 0 )
			continue ;
		if( strcmp( path,".." ) == 0 )
			continue ;
		if( strstr( path,"loop" ) != NULL ){
			path = StringMultipleAppend( st,"/",path,"/loop/backing_file",END ) ;
			xt = StringGetFromVirtualFile( path ) ;
			StringDelete( &st ) ;
			StringRemoveRight( xt,1 ) ;
			result = StringDeleteHandle( &xt ) ;
		}else{
			StringReset( st ) ;
			StringMultipleAppend( st,"/dev/",path,END ) ;
			result = StringDeleteHandle( &st ) ;
		}
	}
	
	closedir( dir ) ;
	return result ;
}

char * zuluCryptVolumeDeviceName( const char * mapper )
{
	struct crypt_device * cd;
	char * path ;
	string_t address = StringVoid ;
	const char * e ;
	
	e = crypt_get_dir() ;
	
	if( e == NULL )
		return NULL ;
	
	if( strncmp( mapper,e,strlen( e ) != 0 ) ){
		return NULL ;
	}
	if( crypt_init_by_name( &cd,mapper ) < 0 ){
		/*
		 * just in case crypt_init_by_name() fail for some reason.
		 */
		return zuluCryptVolumeDeviceName_1( mapper ) ;
	}
	
	e = crypt_get_device_name( cd ) ;
	
	if( e != NULL ){
		if( strncmp( e,"/dev/loop",9 ) == 0 ){
			/*
			 * zuluCryptLoopDeviceAddress() is defined in create_loop.c
			 */
			path = zuluCryptLoopDeviceAddress( e ) ;
			if( path != NULL ){
				address = StringInherit( &path ) ;
			}else{
				address = String( e ) ;
			}
		}else{
			address = String( e ) ;
		}
	}else{
		;
	}
	
	crypt_free( cd ) ;
	
	return StringDeleteHandle( &address ) ;
}
