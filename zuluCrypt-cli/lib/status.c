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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <blkid/blkid.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <stdio.h>
#include <libcryptsetup.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * 64 byte buffer is more than enough because the API that will produce the largest number is crypt_get_data_offset()
 * and it produces a uint64_t number and this type has a maximum digit count is 19.
 */
#define SIZE 64


/*
 * zuluCryptLoopDeviceAddress() is moved to create_loop.c
 */

char * zuluCryptGetMountPointFromPath( const char * path ) ;

static void convert( char * buffer,int buffer_size,const char * s,double y,double z )
{
	snprintf( buffer,buffer_size,"%.1f %s",y/z,s ) ;
}

void zuluCryptFormatSize( u_int64_t n,char * buffer,size_t buffer_size )
{
	const char * z = StringIntToString_1( buffer,buffer_size,n ) ;
	double number = ( double )n ;

	switch( StringSize( z ) ){
	case 0 :
	case 1 : case 2 : case 3 :
		 snprintf( buffer,buffer_size,"%d B",( int )n ) ;
		 break ;
	case 4 : case 5 : case 6 :
		 convert( buffer,buffer_size,"KB",number,1024 ) ;
		 break ; ;
	case 7 : case 8 : case 9 :
		 convert( buffer,buffer_size,"MB",number,1024 * 1024 ) ;
		 break ;
	case 10: case 11 : case 12 :
		 convert( buffer,buffer_size,"GB",number,1024 * 1024 * 1024 ) ;
		 break ;
	case 13: case 14 : case 15 :
		 convert( buffer,buffer_size,"TB",number,1.0 * 1024 * 1024 * 1024 * 1024 ) ;
		 break ;
	default:
		 convert( buffer,buffer_size,"TB",number,1.0 * 1024 * 1024 * 1024 * 1024 ) ;
		 break ;
	}
}

char * zuluCryptGetUUIDFromMapper( const char * mapper )
{
	string_t uuid ;
	struct crypt_device * cd;
	const char * id ;
	const char * type ;
	const char * e = " UUID:   \t\"Nil\"" ;

	if( crypt_init_by_name( &cd,mapper ) < 0 ){
		uuid = String( e ) ;
	}else{
		type = crypt_get_type( cd ) ;
		if( StringHasNoComponent( type,"LUKS" ) ){
			uuid = String( e ) ;
		}else{
			id = crypt_get_uuid( cd ) ;
			if( id == NULL ){
				uuid = String( e ) ;
			}else{
				uuid = String( "" ) ;
				StringMultipleAppend( uuid," UUID:   \t\"",id,"\"",END ) ;
			}
		}
		crypt_free( cd ) ;
	}
	return StringDeleteHandle( &uuid ) ;
}

void zuluCryptFileSystemProperties( string_t p,const char * mapper,const char * m_point )
{
	const char * e ;
	blkid_probe blkid ;
	struct statvfs vfs ;
	u_int64_t total ;
	u_int64_t used ;
	u_int64_t unused ;
	u_int64_t block_size ;
	char buff[ SIZE ] ;
	char * buffer = buff ;
	string_t q ;
	ssize_t index ;
	struct stat statstr ;

	blkid = blkid_new_probe_from_filename( mapper ) ;

	if( blkid == NULL ){
		return ;
	}

	blkid_do_probe( blkid ) ;

	if( blkid_probe_lookup_value( blkid,"TYPE",&e,NULL ) == 0 ){
		StringMultipleAppend( p,"\n file system:\t",e,END ) ;
	}else{
		StringAppend( p,"\n file system:\tNil" ) ;
	}

	blkid_free_probe( blkid ) ;

	if( statvfs( m_point,&vfs ) != 0 ){
		return ;
	}

	block_size = vfs.f_frsize ;
	total = block_size * vfs.f_blocks  ;
	unused =  block_size * vfs.f_bavail  ;

	used = total - unused ;

	zuluCryptFormatSize( total,buffer,SIZE ) ;
	StringMultipleAppend( p,"\n total space:\t",buffer,END ) ;

	zuluCryptFormatSize( used,buffer,SIZE ) ;
	StringMultipleAppend( p,"\n used space:\t",buffer,END ) ;

	zuluCryptFormatSize( unused,buffer,SIZE ) ;
	StringMultipleAppend( p,"\n free space:\t",buffer,END ) ;

	if( used == total ){
		StringAppend( p,"\n used%:   \t100%\n" ) ;
	}else{
		snprintf( buff,SIZE,"%.2f%%",100 * ( ( float ) used / ( float ) total ) ) ;
		StringMultipleAppend( p,"\n used%:   \t",buff,"\n",END ) ;
	}

	buffer = zuluCryptGetUUIDFromMapper( mapper ) ;
	StringAppend( p,buffer ) ;

	StringFree( buffer ) ;

	StringMultipleAppend( p,"\n mount point1:\t",m_point,END ) ;

	q = String( m_point ) ;
	index = StringLastIndexOfChar( q,'/' ) ;
	if( index == -1 ){
		StringAppend( p,"\n mount point2:\tNil" ) ;
	}else{
		StringRemoveLeft( q,index ) ;
		e = StringPrepend( q,"/run/media/public" ) ;
		if( stat( e,&statstr ) == 0 ){
			StringMultipleAppend( p,"\n mount point2:\t",e,END ) ;
		}else{
			StringAppend( p,"\n mount point2:\tNil" ) ;
		}
	}

	StringDelete( &q ) ;
}

char * zuluCryptGetVolumeTypeFromMapperPath( const char * mapper )
{
	struct crypt_device * cd;
	const char * type ;
	char * r ;
	const char * nil = "Nil" ;

	if( !StringPrefixEqual( mapper,crypt_get_dir() ) ){
		return StringCopy_2( nil ) ;
	}

	if( crypt_init_by_name( &cd,mapper ) < 0 ){
		return StringCopy_2( nil ) ;
	}

	type = crypt_get_type( cd ) ;

	if( type == NULL ){
		r = StringCopy_2( nil ) ;
	}else{
		if( StringHasComponent( type,"LUKS" ) ){
			r = StringCopy_2( "crypto_LUKS" ) ;
		}else if( StringHasComponent( type,"PLAIN" ) ){
			r = StringCopy_2( "crypto_PLAIN" ) ;
		}else if( StringHasComponent( type,"TCRYPT" ) ){
			r = StringCopy_2( "crypto_TCRYPT" ) ;
		}else{
			r = StringCopy_2( nil ) ;
		}
	}

	crypt_free( cd ) ;
	return r ;
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

	struct crypt_device * cd ;
	struct crypt_active_device cad ;

	string_t p ;
	string_t q ;

	if( crypt_init_by_name( &cd,mapper ) != 0 ){
		return NULL ;
	}
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
			crypt_free( cd ) ;
			return StringDeleteHandle( &p ) ;
		case CRYPT_ACTIVE   :
			StringAppend( p," is active.\n" ) ;
			break ;
		case CRYPT_BUSY     :
			StringAppend( p," is active and is in use.\n" ) ;
			break ;
		case CRYPT_INVALID  :
			StringAppend( p," is invalid.\n" ) ;
			crypt_free( cd ) ;
			return StringDeleteHandle( &p ) ;
		default :
			StringAppend( p," is invalid.\n" ) ;
			crypt_free( cd ) ;
			return StringDeleteHandle( &p ) ;
	}

	StringAppend( p," type:   \t" ) ;

	type = crypt_get_type( cd ) ;

	if( type == NULL ){
		StringAppend( p,"Nil" ) ;
	}else{
		if( StringPrefixMatch( type,"LUKS",4 ) ){
			luks = 1 ;
			q = String( type ) ;
			StringReplaceString( q,"LUKS","luks" ) ;
			StringAppendString( p,q ) ;
			StringDelete( &q ) ;
		}else if( StringsAreEqual( type,"PLAIN" ) ){
			StringAppend( p,"plain" ) ;
		}else if( StringsAreEqual( type,"TCRYPT" ) ){
			StringAppend( p,"tcrypt" ) ;
		}else{
			StringAppend( p,"Nil" ) ;
		}
	}

	StringMultipleAppend( p,"\n cipher:\t",crypt_get_cipher( cd ),"-",crypt_get_cipher_mode( cd ),END ) ;

	z = StringIntToString_1( buffer,SIZE,8 * crypt_get_volume_key_size( cd ) ) ;
	StringMultipleAppend( p,"\n keysize:\t",z," bits\n device:\t",END ) ;

	if( StringPrefixMatch( device_name,"/dev/loop",9 ) ){
		StringMultipleAppend( p,device_name,"\n loop:   \t",END ) ;
		path = zuluCryptLoopDeviceAddress_1( device_name ) ;
		if( path != NULL ){
			StringAppend( p,path ) ;
			StringFree( path ) ;
		}else{
			StringAppend( p,"Nil" ) ;
		}
	}else{
		/*
		 * zuluCryptResolvePath() is defined in resolve_path.c
		 */
		z = zuluCryptResolvePath( device_name ) ;
		StringAppend( p,z ) ;
		StringFree( z ) ;
		StringAppend( p,"\n loop:   \tNil" ) ;
	}

	z = StringIntToString_1( buffer,SIZE,crypt_get_data_offset( cd ) ) ;
	StringMultipleAppend( p,"\n offset:\t",z," sectors",NULL ) ;

	zuluCryptFormatSize( crypt_get_data_offset( cd ) * 512,buffer,SIZE ) ;
	StringMultipleAppend( p," / ",buffer,NULL ) ;

	if( cad.flags == 1 ){
		StringAppend( p,"\n mode:   \tread only" ) ;
	}else{
		StringAppend( p,"\n mode:   \tread and write" ) ;
	}

	if( luks == 1 ){
		i = 0 ;
		k = crypt_keyslot_max( type ) ;
		for( j = 0 ; j < k ; j++ ){
			switch( crypt_keyslot_status( cd,j ) ){
				case CRYPT_SLOT_ACTIVE_LAST : i++ ; break ;
				case CRYPT_SLOT_ACTIVE      : i++ ; break ;
				default : ;
			}
		}

		StringMultipleAppend( p,"\n active slots:\t",StringIntToString_1( buffer,SIZE,i ),END ) ;

		StringMultipleAppend( p," / ",StringIntToString_1( buffer,SIZE,k ),END ) ;
	}else{
		StringAppend( p,"\n active slots:\tNil" ) ;
	}

	/*
	 * zuluCryptGetMountPointFromPath() is defined in ./process_mountinfo.c
	 */
	path = zuluCryptGetMountPointFromPath( mapper ) ;
	if( path != NULL ){
		zuluCryptFileSystemProperties( p,mapper,path ) ;
		free( path ) ;
	}

	crypt_free( cd ) ;
	return StringDeleteHandle( &p ) ;
}

char * zuluCryptVolumeDeviceName( const char * mapper )
{
	struct crypt_device * cd ;
	const char * e = crypt_get_dir() ;
	char * f = NULL ;
	if( StringPrefixEqual( mapper,e ) ){
		if( crypt_init_by_name( &cd,mapper ) == 0 ){
			e = crypt_get_device_name( cd ) ;
			if( e != NULL ){
				/*
				* zuluCryptResolvePath() is defined in resolve_path.c
				*/
				f = zuluCryptResolvePath( e ) ;
			}
			crypt_free( cd ) ;
		}
	}
	return f ;
}
