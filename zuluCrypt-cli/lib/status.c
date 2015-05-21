/*
 *
 *  Copyright (c) 2011
 *  name : Francis Banyikwa
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

#define TYPE 3

#define UUID 4

/*
 * zuluCryptLoopDeviceAddress() is moved to create_loop.c
 */

char * zuluCryptGetMountPointFromPath( const char * path ) ;

static void convert( char * buffer,int buffer_size,const char * s,u_int64_t y,u_int64_t z )
{
	snprintf( buffer,buffer_size,"%.1f %s",( double )y/z,s ) ;
}

void zuluCryptFormatSize( u_int64_t number,char * buffer,size_t buffer_size )
{
	const char * z = StringIntToString_1( buffer,buffer_size,number ) ;

	switch( StringSize( z ) ){
	case 0 :
	case 1 : case 2 : case 3 :
		 snprintf( buffer,buffer_size,"%d B",( int )number ) ;
		 break ;
	case 4 : case 5 : case 6 :
		 convert( buffer,buffer_size,"KB",number,1024 ) ;
		 break ;
	case 7 : case 8 : case 9 :
		 convert( buffer,buffer_size,"MB",number,1024 * 1024 ) ;
		 break ;
	case 10: case 11 : case 12 :
		 convert( buffer,buffer_size,"GB",number,1024 * 1024 * 1024 ) ;
		 break ;
	case 13: case 14 : case 15 :
		 convert( buffer,buffer_size,"TB",number,1024.0 * 1024 * 1024 * 1024 ) ;
		 break ;
	default:
		 convert( buffer,buffer_size,"TB",number,1024.0 * 1024 * 1024 * 1024 ) ;
		 break ;
	}
}

/*
 * given a path of something like "/dev/mapper/zuluCrypt-500-NAAN-header-image-file.img-2244846319",
 * this routine will look for its corresponding entry in "/dev/disk/by-id/" and we will find
 * "dm-uuid-CRYPT-LUKS1-LUKS_UUID-zuluCrypt-500-NAAN-header-image-file.img-2244846319" and will
 * return desired component of the path when separated by "-" character.
 *
 */

string_t _get_mapper_property_from_udev( const char * mapper,size_t position )
{
	DIR * dir = opendir( "/dev/disk/by-id/" ) ;
	struct dirent * e ;

	const char * f = mapper + StringSize( crypt_get_dir() ) + 1 ;

	stringList_t stl ;

	string_t st = StringVoid ;

	if( dir != NULL ){

		while( ( e = readdir( dir ) ) != NULL ){

			if( StringStartsAndEndsWith( e->d_name,"dm-uuid-CRYPT-LUKS",f ) ){

				stl = StringListSplit( e->d_name,'-' ) ;

				st = StringListCopyStringAt( stl,position ) ;

				StringListDelete( &stl ) ;

				break ;
			}
		}

		closedir( dir ) ;
	}

	return st ;
}

static char * _get_uuid_from_udev( const char * mapper )
{
	string_t st = _get_mapper_property_from_udev( mapper,UUID ) ;
	return StringDeleteHandle( &st ) ;
}

static char * _get_type_from_udev( const char * mapper )
{
	string_t st = _get_mapper_property_from_udev( mapper,TYPE ) ;

	if( st == StringVoid ){

		return StringCopy_2( "Nil" ) ;
	}else{
		StringPrepend( st,"crypto_" ) ;

		return StringDeleteHandle( &st ) ;
	}
}

static string_t _get_type_from_udev_1( const char * mapper )
{
	string_t st = _get_mapper_property_from_udev( mapper,TYPE ) ;

	if( st == StringVoid ){

		/*
		 * failed to discover volume type,assume its luks1
		 */
		return String( "luks1" ) ;
	}else{
		StringToLowerCase( st ) ;

		return st ;
	}
}

char * zuluCryptGetUUIDFromMapper( const char * mapper )
{
	string_t uuid ;
	struct crypt_device * cd ;

	const char * id ;
	const char * e = " UUID:   \t\"Nil\"" ;

	char * f ;

	if( crypt_init_by_name( &cd,mapper ) < 0 ){

		uuid = String( e ) ;
	}else{
		id = crypt_get_uuid( cd ) ;

		if( id == NULL ){

			/*
			 * Either not a LUKS volume or a LUKS volume but with a detached header.
			 * consult udev to see if it can sort this volume out.
			 */

			f = _get_uuid_from_udev( mapper ) ;

			if( f == NULL ){

				uuid = String( e ) ;
			}else{
				uuid = String_1( " UUID:   \t\"",f,"\"",NULL ) ;
				StringFree( f ) ;
			}

		}else{
			uuid = String_1( " UUID:   \t\"",id,"\"",NULL ) ;
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

		StringMultipleAppend( p,"\n file system:\t",e,NULL ) ;
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
	StringMultipleAppend( p,"\n total space:\t",buffer,NULL ) ;

	zuluCryptFormatSize( used,buffer,SIZE ) ;
	StringMultipleAppend( p,"\n used space:\t",buffer,NULL ) ;

	zuluCryptFormatSize( unused,buffer,SIZE ) ;
	StringMultipleAppend( p,"\n free space:\t",buffer,NULL ) ;

	if( used == total ){

		StringAppend( p,"\n used%:   \t100%\n" ) ;

	}else if( used == 0 ){

		StringAppend( p,"\n used%:   \t0%\n" ) ;
	}else{
		snprintf( buff,SIZE,"%.2f%%",100 * ( ( float ) used / ( float ) total ) ) ;
		StringMultipleAppend( p,"\n used%:   \t",buff,"\n",NULL ) ;
	}

	buffer = zuluCryptGetUUIDFromMapper( mapper ) ;
	StringAppend( p,buffer ) ;

	StringFree( buffer ) ;

	StringMultipleAppend( p,"\n mount point1:\t",m_point,NULL ) ;

	q = String( m_point ) ;
	index = StringLastIndexOfChar( q,'/' ) ;

	if( index == -1 ){

		StringAppend( p,"\n mount point2:\tNil" ) ;
	}else{
		StringRemoveLeft( q,index ) ;
		e = StringPrepend( q,"/run/media/public" ) ;

		if( stat( e,&statstr ) == 0 ){

			StringMultipleAppend( p,"\n mount point2:\t",e,NULL ) ;
		}else{
			StringAppend( p,"\n mount point2:\tNil" ) ;
		}
	}

	StringDelete( &q ) ;
}

/*
 * This function needs an explanation.
 * a VeraCrypt mapper will be in a format of: /dev/mapper/zuluCrypt-500-VERA-xxx-yyy.
 * other mappers will be in a format of:      /dev/mapper/zuluCrypt-500-NAAN-xxx-yyy or
 * in a format of:                            /dev/mapper/zuluCrypt-500-UUID-xxx-yyy
 *
 * the "500" is the UID of the user that unlocked the volume.
 *
 * to check if a volume is a VeraCrypt volume,we first strip the digits to end up with
 * something like "/dev/mapper/zuluCrypt--VERA-" and then we check if the volume is a
 * VeraCrypt volume by comparing it with "/dev/mapper/zuluCrypt--VERA-"
 */
static int _veraCrypt_volume( const char * mapper )
{
	int r ;

	/*
	 * st will hold a string like: /dev/mapper/zuluCrypt-500-VERA-xxx-yyy
	 */
	string_t st = String( mapper ) ;

	/*
	 * xt will hold a string like: /dev/mapper/zuluCrypt--VERA-
	 */
	string_t xt = String_1( crypt_get_dir(),"/zuluCrypt--VERA-",NULL ) ;

	/*
	 * strip the digits from st to end up with a string like: /dev/mapper/zuluCrypt--VERA-xxx-yyy
	 */
	StringRemoveDigits( st ) ;

	/*
	 * check if st(/dev/mapper/zuluCrypt--VERA-xxx-yyy) starts with xt(/dev/mapper/zuluCrypt--VERA-)
	 */
	r = StringStartsWith_1( st,xt ) ;

	StringMultipleDelete( &st,&xt,NULL ) ;

	return r ;
}

char * zuluCryptGetVolumeTypeFromMapperPath( const char * mapper )
{
	struct crypt_device * cd ;
	const char * type ;
	char * r ;
	string_t st ;

	if( StringPrefixNotEqual( mapper,crypt_get_dir() ) ){

		return StringCopy_2( "Nil" ) ;
	}

	if( _veraCrypt_volume( mapper ) ){

		return StringCopy_2( "crypto_VCRYPT" ) ;
	}

	if( crypt_init_by_name( &cd,mapper ) < 0 ){

		return StringCopy_2( "Nil" ) ;
	}

	type = crypt_get_type( cd ) ;

	if( type == NULL ){

		if( StringHasComponent( mapper,"veracrypt" ) ){

			r = StringCopy_2( "crypto_VCRYPT" ) ;

		}else if( StringHasComponent( mapper,"truecrypt" ) ){

			r = StringCopy_2( "crypto_TCRYPT" ) ;

		}else{
			r = _get_type_from_udev( mapper ) ;
		}
	}else{
		st = String_1( "crypto_",type,NULL ) ;
		r = StringDeleteHandle( &st ) ;
	}

	crypt_free( cd ) ;
	return r ;
}

static char * zuluExit( string_t st,struct crypt_device * cd )
{
	crypt_free( cd ) ;
	return StringDeleteHandle( &st ) ;
}

static char * _volume_status( const char * mapper )
{
	char buff[ SIZE ] ;
	char * buffer = buff ;
	const char * z ;
	const char * type ;
	const char * device_name ;
	char * path ;
	int i ;
	int j ;
	int k ;

	struct crypt_device * cd ;
	struct crypt_active_device cad ;

	string_t p = StringVoid ;
	string_t q ;

	if( crypt_init_by_name( &cd,mapper ) != 0 ){

		return NULL ;
	}
	if( crypt_get_active_device( NULL,mapper,&cad ) != 0 ){

		return zuluExit( p,cd ) ;
	}

	device_name = crypt_get_device_name( cd ) ;

	if( device_name == NULL ){

		return zuluExit( p,cd ) ;
	}

	p = String( mapper ) ;

	switch( crypt_status( cd,mapper ) ){
	case CRYPT_INACTIVE :
		StringAppend( p," is inactive.\n" ) ;
		return zuluExit( p,cd ) ;
	case CRYPT_INVALID  :
		StringAppend( p," is invalid.\n" ) ;
		return zuluExit( p,cd ) ;
	case CRYPT_ACTIVE   :
		StringAppend( p," is active.\n" ) ;
		break ;
	case CRYPT_BUSY     :
		StringAppend( p," is active and is in use.\n" ) ;
		break ;
	default :
		StringAppend( p," is invalid.\n" ) ;
		return zuluExit( p,cd ) ;
	}

	StringAppend( p," type:   \t" ) ;

	type = crypt_get_type( cd ) ;

	if( type != NULL ){

		if( StringsAreEqual( type,"TCRYPT" ) ){

			if( _veraCrypt_volume( mapper ) ){

				StringAppend( p,"vcrypt" ) ;
			}else{
				StringAppend( p,"tcrypt" ) ;
			}
		}else{
			q = String( type ) ;
			StringAppend( p,StringToLowerCase( q ) ) ;
			StringDelete( &q ) ;
		}

	}else{
		q = _get_type_from_udev_1( mapper ) ;

		StringAppendString( p,q ) ;

		StringDelete( &q ) ;
	}

	z = crypt_get_cipher( cd ) ;

	if( z != NULL ){

		StringMultipleAppend( p,"\n cipher:\t",z,"-",NULL ) ;
	}else{
		StringAppend( p,"\n cipher:\tNil-" ) ;
	}

	z = crypt_get_cipher_mode( cd ) ;

	if( z != NULL ){

		StringAppend( p,z ) ;
	}else{
		StringAppend( p,"Nil" ) ;
	}

	z = StringIntToString_1( buffer,SIZE,8 * crypt_get_volume_key_size( cd ) ) ;
	StringMultipleAppend( p,"\n keysize:\t",z," bits\n device:\t",NULL ) ;

	if( StringPrefixEqual( device_name,"/dev/loop" ) ){

		StringMultipleAppend( p,device_name,"\n loop:   \t",NULL ) ;
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
		StringMultipleAppend( p,z,"\n loop:   \tNil",NULL ) ;
		StringFree( z ) ;
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

	k = crypt_keyslot_max( type ) ;

	if( k > 0 ){

		i = 0 ;

		for( j = 0 ; j < k ; j++ ){

			switch( crypt_keyslot_status( cd,j ) ){

				case CRYPT_SLOT_ACTIVE_LAST : i++ ; break ;
				case CRYPT_SLOT_ACTIVE      : i++ ; break ;
				default : ;
			}
		}

		StringMultipleAppend( p,"\n active slots:\t",StringIntToString_1( buffer,SIZE,i ),NULL ) ;

		StringMultipleAppend( p," / ",StringIntToString_1( buffer,SIZE,k ),NULL ) ;
	}else{
		StringAppend( p,"\n active slots:\tNil" ) ;
	}

	/*
	 * zuluCryptGetMountPointFromPath() is defined in ./process_mountinfo.c
	 */
	path = zuluCryptGetMountPointFromPath( mapper ) ;

	if( path != NULL ){

		zuluCryptFileSystemProperties( p,mapper,path ) ;
		StringFree( path ) ;
	}

	return zuluExit( p,cd ) ;
}

char * zuluCryptVolumeStatus( const char * mapper )
{
	ssize_t r ;

	char * s ;

	char * e = _volume_status( mapper ) ;

	if( e == NULL ){

		/*
		 * we failed,lets try again assuming its a VeraCrypt volume by just substituting
		 * "NAAN" with "VERA"
		 */
		r = StringHasComponent_1( mapper,"-NAAN-" ) ;

		if( r != -1 ){

			s = StringCopy_2( mapper ) ;

			*( s + r + 1 ) = 'V' ;
			*( s + r + 2 ) = 'E' ;
			*( s + r + 3 ) = 'R' ;
			*( s + r + 4 ) = 'A' ;

			e = _volume_status( s ) ;

			StringFree( s ) ;
		}
	}

	return e ;
}

char * zuluCryptVolumeDeviceName( const char * mapper )
{
	struct crypt_device * cd ;
	const char * e = crypt_get_dir() ;
	char * f = NULL ;

	if( StringPrefixEqual( mapper,e ) && crypt_init_by_name( &cd,mapper ) == 0 ){

		e = crypt_get_device_name( cd ) ;

		if( e != NULL ){
			/*
			 * zuluCryptResolvePath_3() is defined in resolve_path.c
			*/
			f = zuluCryptResolvePath_3( e ) ;
		}
		crypt_free( cd ) ;
	}
	return f ;
}
