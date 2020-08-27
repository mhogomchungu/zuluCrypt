/*
 *
 *  Copyright (c) 2011-2015
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

#include <mntent.h>
#include <sys/statvfs.h>
#include <blkid/blkid.h>
#include <stdint.h>
#include <stdio.h>
#include <locale.h>
#include <libintl.h>

#include "../zuluCrypt-cli/lib/includes.h"
#include "../zuluCrypt-cli/bin/includes.h"

stringList_t zuluCryptPartitionList( void ) ;

static const char * _fs( const char * e )
{
	if( StringPrefixEqual( e,"fuse" ) ){

		return e + 5 ;
	}else{
		return e ;
	}
}

void zuluMountPartitionProperties( const char * dev,const char * UUID,
				   const char * mapper,const char * m_point,const char * fs )
{
	#define SIZE 64

	const char * g ;
	const char * e ;

	char * m ;

	blkid_probe blkid ;

	struct statvfs vfs ;
	u_int64_t total ;
	u_int64_t used ;
	u_int64_t free_space ;
	u_int32_t block_size ;
	u_int64_t volume_size = 0 ;

	char buff[ SIZE ] ;
	char * buffer = buff ;

	const char * device = NULL ;
	char * device_1 = NULL ;

	if( zuluCryptFUSEVolumeIsSupported( fs ) ){

		if( m_point != NULL ){

			printf( "%s\t%s\t%s\tNil\tNil\tNil\n",dev,m_point,_fs( fs ) ) ;
		}else{
			printf( "%s\tNil\t%s\tNil\tNil\tNil\n",dev,_fs( fs ) ) ;
		}
		return ;
	}

	zuluCryptSecurityGainElevatedPrivileges() ;

	if( StringPrefixEqual( dev,"/dev/" ) ){

		device = dev ;
	}else{
		/*
		 * got a path to an image file,convert it to its associated loop device first
		 * because we dont want to pass image files paths to blkid.
		 */
		/*
		 * zuluCryptGetALoopDeviceAssociatedWithAnImageFile()
		 * is defined in ../zuluCrypt-cli/lib/create_loop_device.c
		 */
		device = device_1 = zuluCryptGetALoopDeviceAssociatedWithAnImageFile( dev ) ;
	}

	if( device == NULL ){

		zuluCryptSecurityDropElevatedPrivileges() ;

		return ;
	}

	blkid = blkid_new_probe_from_filename( device ) ;

	if( blkid == NULL ){

		zuluCryptSecurityDropElevatedPrivileges() ;

		printf( "%s\tNil\tNil\tNil\tNil\tNil\n",device ) ;

		StringFree( device_1 ) ;

		return ;
	}

	if( UUID != NULL ){

		printf( "%s\t",UUID ) ;
	}else{
		if( zuluCryptNoPartitionLoopDevice( dev ) ){
			/*
			 * zuluCryptLoopDeviceAddress_1() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
			 */
			g = zuluCryptLoopDeviceAddress_1( dev ) ;

			if( g != NULL ){
				printf( "%s\t",g ) ;
				StringFree( g ) ;
			}else{
				printf( "%s\t",dev ) ;
			}
		}else{
			printf( "%s\t",dev ) ;
		}
	}

	if( m_point == NULL ){

		printf( "Nil\t" ) ;
	}else{
		printf( "%s\t",m_point ) ;
	}

	if( StringsAreEqual( dev,mapper ) ){

		blkid_do_probe( blkid ) ;

		if( StringPrefixEqual( fs,zuluCryptBitLockerType() ) ){

			printf( "%s\t",fs ) ;
		}else{
			e = zuluCryptVolumeType( blkid,device ) ;

			if( StringPrefixEqual( e,"crypto_LUKS" ) ){

				m = zuluCryptGetVolumeType_1( device ) ;

				if( m ){

					printf( "%s\t",m ) ;

					StringFree( m ) ;
				}else{
					printf( "%s\t",e ) ;
				}
			}else{
				printf( "%s\t",e ) ;
			}
		}

		if( blkid_probe_lookup_value( blkid,"LABEL",&g,NULL ) == 0 ){

			printf( "%s",g ) ;
		}else{
			printf( "Nil" ) ;
		}

		blkid_free_probe( blkid ) ;

		/*
		 * zuluCryptGetVolumeSize() is defined in ../zuluCrypt-cli/lib/blkid_evaluate_tag.c
		 */
		if( m_point == NULL ){

			volume_size = zuluCryptGetVolumeSize( device ) ;
		}
	}else{
		blkid_free_probe( blkid ) ;

		blkid = blkid_new_probe_from_filename( mapper ) ;

		if( blkid == NULL ){

			printf( "Nil\tNil" ) ;
		}else{
			blkid_do_probe( blkid ) ;
			/*
			 * zuluCryptGetVolumeTypeFromMapperPath() is defined in ../zuluCrypt-cli/lib/status.c
			 */
			e = zuluCryptGetVolumeTypeFromMapperPath( mapper ) ;

			if( blkid_probe_lookup_value( blkid,"TYPE",&g,NULL ) == 0 ){

				printf( "%s/%s\t",e,g ) ;
			}else{
				printf( "%s/Nil\t",e ) ;
			}

			if( blkid_probe_lookup_value( blkid,"LABEL",&g,NULL ) == 0 ){

				printf( "%s",g ) ;
			}else{
				printf( "Nil" ) ;
			}

			blkid_free_probe( blkid ) ;
			StringFree( e ) ;

			if( m_point == NULL ){

				volume_size = zuluCryptGetVolumeSize( mapper ) ;
			}
		}
	}

	if( m_point == NULL ){

		zuluCryptFormatSize( volume_size,buffer,SIZE ) ;
		printf( "\t%s\tNil\n",buffer ) ;
	}else{
		if( statvfs( m_point,&vfs ) != 0 ){

			printf( "\tNil\tNil\n" ) ;
		}else{
			block_size = (unsigned int)vfs.f_frsize ;

			total = block_size * vfs.f_blocks ;

			zuluCryptFormatSize( total,buffer,SIZE ) ;
			printf( "\t%s",buffer ) ;

			free_space = block_size * vfs.f_bavail  ;

			used = total - free_space ;

			if( used == total ){

				puts( "\t100%" ) ;
			}else{
				snprintf( buff,SIZE,"%.2f%%",100 * (double)( ( float ) used / ( float ) total ) ) ;
				printf( "\t%s\n",buff ) ;
			}
		}
	}

	StringFree( device_1 ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;
}

static void _print_device_properties( string_t entry,const char * mapper_path,size_t mapper_length,uid_t uid )
{
	char * x ;

	const char * q ;
	const char * e ;
	const char * f ;

	const char * fs ;

	ssize_t index ;

	string_t st = StringVoid ;
	string_t xt ;

	stringList_t stx = StringListStringSplit( entry,' ' ) ;

	if( stx == StringListVoid ){

		return ;
	}

	fs = StringListContentAtThirdPlace( stx ) ;

	q = StringListContentAtFirstPlace( stx ) ;

	if( StringPrefixMatch( q,mapper_path,mapper_length ) ){
		/*
		 * zuluCryptSecurityGainElevatedPrivileges() and zuluCryptSecurityDropElevatedPrivileges()
		 * are defined in ../zuluCrypt-cli/bin/security.c
		 */
		zuluCryptSecurityGainElevatedPrivileges() ;
		/*
		 * zuluCryptVolumeDeviceName() is defined in ../zuluCrypt-cli/lib/status.c
		 */
		x = zuluCryptVolumeDeviceName( q ) ;

		zuluCryptSecurityDropElevatedPrivileges() ;

		if( x != NULL ){

			index = StringHasComponent_1( q,"-UUID-" ) ;

			if( index != -1 ){

				st = String( q ) ;
				StringRemoveLeft( st,(size_t)index + 6 ) ;
				e = StringPrepend( st,"UUID=\"" ) ;
				index = StringLastIndexOfChar( st,'-' ) ;

				if( index >= 0 ){

					StringSubChar( st,(size_t)index,'\"' ) ;
					e = StringSubChar( st,(size_t)index+1,'\0' ) ;
				}
			}else{
				e = x ;
			}

			StringReplace( entry,e ) ;
			/*
			 * zuluCryptDecodeMountEntry() is defined in ../zuluCrypt-cli/lib/mount_volume.c
			 * it decodes space,tab,new line and backslash characters since they are written differently in "/etc/mtab"
			 */
			f = zuluCryptDecodeMountEntry( StringListStringAtSecondPlace( stx ) ) ;

			zuluMountPartitionProperties( x,e,q,f,fs ) ;

			StringFree( x ) ;
			StringDelete( &st ) ;
		}else{
			StringReplaceChar_1( entry,0,' ','\0' ) ;
		}

	}else if( zuluCryptBitLockerVolume( q ) ){

		st = zuluCryptBitLockerResolveMapperPath( q,uid ) ;

		e = zuluCryptDecodeMountEntry( st ) ;
		f = zuluCryptDecodeMountEntry( StringListStringAtSecondPlace( stx ) ) ;

		StringReplace( entry,e ) ;

		zuluCryptSecurityGainElevatedPrivileges() ;

		xt = zuluCryptGetFileSystemFromDevice( q ) ;

		if( xt != StringVoid ){

			StringMultiplePrepend( xt,"/",zuluCryptBitLockerType(),NULL ) ;

			zuluMountPartitionProperties( e,NULL,e,f,StringContent( xt ) ) ;
		}else{
			zuluMountPartitionProperties( e,NULL,e,f,zuluCryptBitLockerType() ) ;
		}

		zuluCryptSecurityDropElevatedPrivileges() ;

		StringMultipleDelete( &st,&xt,NULL ) ;
	}else{
		StringReplaceChar_1( entry,0,' ','\0' ) ;

		e = zuluCryptDecodeMountEntry( StringListStringAtFirstPlace( stx ) ) ;
		f = zuluCryptDecodeMountEntry( StringListStringAtSecondPlace( stx ) ) ;

		zuluMountPartitionProperties( e,NULL,e,f,fs ) ;
	}

	zuluCryptSecurityDropElevatedPrivileges() ;
	StringListDelete( &stx ) ;
}

static int _starts_with_digits_only( string_t st )
{
	StringIterator it  ;
	StringIterator end ;

	char e ;

	StringGetIterators( st,&it,&end ) ;

	while( it != end ){

		e = *it ;

		it++ ;

		if( e == ' ' ){

			return 1 ;
		}
		if( !( e >= '0' && e <= '9' ) ){

			return 0 ;
		}
	}

	return 1 ;
}

static int _normal_mounted_volume( string_t st )
{
	if( StringStartsWithAtLeastOne( st,"/proc","/sys","/dev ",NULL ) ){

		return 0 ;
	}
	if( StringsAreEqual_2( st,"/dev" ) ){

		return 0 ;
	}
	if( StringStartsWith( st,"/" ) ){

		return 1 ;
	}
	if( _starts_with_digits_only( st ) )
	{
		return 1 ;
	}
	return 0 ;
}

int zuluMountPrintVolumesProperties( uid_t uid )
{
	stringList_t stl ;
	stringList_t stz ;

	StringListIterator it  ;
	StringListIterator end ;

	string_t st ;

	const char * e ;
	const char * z ;

	size_t l ;

	/*
	 * zuluCryptGetMoutedList() is  defined in ../zuluCrypt-cli/lib/process_mountinfo.c
	 * run StringListPrintList( stl ) to get a list of entries being worked on
	 */
	stl = zuluCryptGetMoutedList() ;

	/*
	 * zuluCryptGetAListOfAllPartitions() is defined in ../zuluCrypt-cli/bin/volumes.c
	 */
	stz = zuluCryptGetAListOfAllVolumes() ;

	if( uid ){}
	/*
	 * zuluCryptMapperPrefix() is defined in ../zuluCrypt-cli/lib/create_mapper_name.c
	 * it should return something like "/dev/mapper"
	 */
	z = zuluCryptMapperPrefix() ;
	l = StringSize( z ) ;

	StringListGetIterators( stl,&it,&end ) ;
	/*
	 * print a list of mounted volumes
	 */
	while( it != end ){

		st = *it ;

		it++ ;

		if( _normal_mounted_volume( st ) ){

			_print_device_properties( st,z,l,uid ) ;

			if( StringStartsWith( st,"UUID=" ) ){

				e = StringRemoveString( st,"\"" ) ;
				/*
				 * zuluCryptEvaluateDeviceTags() ../zuluCrypt-cli/bin/path_access.c
				 */
				e = zuluCryptEvaluateDeviceTags( "UUID",e + 5 ) ;

				StringListRemoveIfPresent( stz,e ) ;

				StringFree( e ) ;
			}else{
				zuluCryptDecodeMountEntry( st ) ;
				StringListRemoveIfPresent_1( stz,st ) ;
			}
		}else{
			zuluCryptDecodeMountEntry( st ) ;

			StringReplaceChar_1( st,0,' ','\0' ) ;

			StringListRemoveIfPresent_1( stz,st ) ;
		}
	}

	StringListGetIterators( stz,&it,&end ) ;
	/*
	 * print a list of not mounted volumes
	 */
	while( it != end ){

		e = StringContent( *it ) ;

		it++ ;

		zuluMountPartitionProperties( e,NULL,e,NULL,NULL ) ;
	}

	StringListMultipleDelete( &stl,&stz,NULL ) ;

	return 0 ;
}

static void  _zuluMountprintAListOfMountedVolumes( string_t st,uid_t uid )
{
	const char * e ;
	const char * f ;

	/*
	 * zuluCryptMapperPrefix() is defined in ../zuluCrypt-cli/lib/create_mapper_name.c
	 * mapper_prefix will probably contain "/dev/mapper/"
	 */
	const char * mapper_prefix = zuluCryptMapperPrefix() ;

	string_t q ;

	if( StringStartsWith( st,mapper_prefix ) ){
		/*
		 * we will get here is the path starts with "/dev/mapper/".
		 * This path could be an LVM path or an encrypted mapper path
		 */
		/*
		 * zuluCryptConvertIfPathIsLVM() is defined in ../zuluCrypt-cli/lib/status.c
		 */
		q = zuluCryptConvertIfPathIsLVM( StringContent( st ) ) ;

		if( StringStartsWith( q,mapper_prefix ) ){
			/*
			 * volume is probably an encrypted one
			 */
			if( StringContains( q,"-NAAN-" ) ){

				zuluCryptSecurityGainElevatedPrivileges() ;
				/*
				 * zuluCryptVolumeDeviceName() is defined in ../zuluCrypt-cli/lib/status.c
				 */
				f = zuluCryptVolumeDeviceName( StringContent( q ) ) ;

				zuluCryptSecurityDropElevatedPrivileges() ;

				if( f != NULL ){

					puts( f ) ;
					StringFree( f ) ;
				}
			}else if( StringContains( q,"-UUID-" ) ){

				StringReplaceString( q,"-UUID-","-UUID=\"" ) ;

				e = StringAppend( q,"\"" ) ;
				e = e + StringHasComponent_1( e,"UUID=" ) ;

				puts( e ) ;
			}else{
				/*
				 * Still assuming its an encrypted volume opened and outside of zuluCrypt/zuluMount
				 */
				zuluCryptSecurityGainElevatedPrivileges() ;

				f = zuluCryptVolumeDeviceName( StringContent( q ) ) ;

				zuluCryptSecurityDropElevatedPrivileges() ;

				if( f != NULL ){

					puts( f ) ;
					StringFree( f ) ;
				}else{
					/*
					 * not exactly sure what this is,just print it and let the user sort it out
					 */
					zuluCryptDecodeMountEntry( q ) ;
					StringPrintLine( q ) ;
				}
			}
		}else{
			/*
			 * the volume is probably an LVM volume
			 */
			zuluCryptDecodeMountEntry( q ) ;
			StringPrintLine( q ) ;
		}

		StringDelete( &q ) ;

	}else if( zuluCryptBitLockerVolume( StringContent( st ) ) ){

		q = zuluCryptBitLockerResolveMapperPath( StringContent( st ),uid ) ;

		StringPrintLine( q ) ;

		StringDelete( &q ) ;
	}else{
		zuluCryptDecodeMountEntry( st ) ;
		StringPrintLine( st ) ;
	}
}

int zuluMountprintAListOfMountedVolumes( uid_t uid )
{
	/*
	 * zuluCryptGetAListOfMountedVolumes() is defined in ../zuluCrypt-cli/lib/process_mountinfo.c
	 */
	stringList_t stz = zuluCryptGetAListOfMountedVolumes() ;
	stringList_t stx = StringListVoid ;

	string_t st ;

	StringListIterator it  ;
	StringListIterator end ;

	const char * f ;
	/*
	 * remove duplicates caused by bind mounts and other entries we dont care about
	 */
	StringListGetIterators( stz,&it,&end ) ;

	while( it != end ){

		st = *it ;

		it++ ;

		if( _normal_mounted_volume( st ) ){

			f = StringContent( st ) ;

			if( StringListHasNoEntry( stx,f ) ){

				/*
				 * Only print one entry if there are more due to bind mounts
				 */
				_zuluMountprintAListOfMountedVolumes( st,uid ) ;
				stx = StringListAppend( stx,f ) ;
			}
		}
	}

	StringListMultipleDelete( &stz,&stx,NULL ) ;
	return 0 ;
}

int zuluMountPrintDeviceProperties( const char * device,const char * UUID,uid_t uid )
{
	string_t p ;
	string_t q ;
	string_t z = StringVoid ;
	string_t f = StringVoid ;

	char * dev = NULL ;
	const char * device_1 ;
	const char * e ;

	/*
	 * zuluCryptMapperPrefix() is defined in ../zuluCrypt-cli/lib/create_mapper_name.c
	 * mapper_prefix will probably contain "/dev/mapper/"
	 */
	const char * mapper_prefix = zuluCryptMapperPrefix() ;

	size_t mapper_length = StringSize( mapper_prefix ) ;

	StringListIterator it  ;
	StringListIterator end ;

	/*
	 * zuluCryptGetMoutedList() is defined in ../zuluCrypt-cli/lib/process_mountinfo.c
	 */
	stringList_t stl = zuluCryptGetMoutedList() ;

	device_1 = device ;

	if( UUID == NULL ){

		if( StringPrefixEqual( device,"/dev/loop" ) ){

			/*
			 * zuluCryptLoopDeviceAddress() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
			 */

			if( zuluCryptMultiPartitionLoopDevice( device ) ){

				dev = StringCopy_2( device ) ;
			}else{
				dev = zuluCryptLoopDeviceAddress( device ) ;
			}

			device_1 = dev ;

			z = String( dev ) ;
			e = zuluCryptDecodeMountEntry( z ) ;
			/*
			 * ZULUCRYPTlongMapperPath is set in ../zuluCrypt-cli/constants.h
			 * zuluCryptCreateMapperName() is defined at ../zuluCrypt-cli/lib/create_mapper_name.c
			 */
			e = e + StringLastIndexOfChar_1( e,'/' ) + 1 ;
			q = zuluCryptCreateMapperName( StringContent( z ),e,uid,ZULUCRYPTlongMapperPath ) ;
		}else{
			/*
			 * ZULUCRYPTlongMapperPath is set in ../zuluCrypt-cli/constants.h
			 * zuluCryptCreateMapperName() is defined at ../zuluCrypt-cli/lib/create_mapper_name.c
			 */
			e = device + StringLastIndexOfChar_1( device,'/' ) + 1 ;
			q = zuluCryptCreateMapperName( device,e,uid,ZULUCRYPTlongMapperPath ) ;
		}
	}else{
		p = String( UUID ) ;
		StringRemoveString( p,"\"" ) ;
		e = StringReplaceString( p,"UUID=","UUID-" ) ;
		q = zuluCryptCreateMapperName( device,e,uid,ZULUCRYPTlongMapperPath ) ;
		StringDelete( &p ) ;
	}

	e = StringContent( q ) ;

	p = StringListHasStartSequence_1( stl,e ) ;

	if( p != StringVoid ){

		/*
		 * mounted and encrypted volume opened by this user
		 */
		_print_device_properties( p,mapper_prefix,mapper_length,uid ) ;
	}else{
		/*
		 * We will get if:
		 * 1. The volume is not mounted.
		 * 2. The volume is mounted,encrypted and opened by a different user
		 * 3. The volume is mouted and unencrypted
		 */
		f = String( device_1 ) ;
		p = StringListHasStartSequence_1( stl,StringAppend( f," " ) ) ;

		StringDelete( &f ) ;

		if( p != StringVoid ){
			/*
			 * volume is unencrypted and mounted by any user
			 */
			_print_device_properties( p,mapper_prefix,mapper_length,uid ) ;
		}else{
			/*
			 * We will get here is:
			 * 1. The volume is not mounted
			 * 2. The volume is encrypted and mounted by a different user
			 */
			StringListGetIterators( stl,&it,&end ) ;

			zuluCryptSecurityGainElevatedPrivileges() ;

			while( it != end ){

				p = *it ;

				it++ ;

				if( StringStartsWith( p,mapper_prefix ) ){

					e = StringReplaceChar_1( p,0,' ','\0' ) ;
					/*
					 * zuluCryptVolumeDeviceName() is defined in ../zuluCrypt-cli/lib/status.c
					 */
					e = zuluCryptVolumeDeviceName( e ) ;

					if( StringsAreEqual( e,device ) ){

						f = p ;
						StringReplaceChar_1( p,0,'\0',' ' ) ;

						StringFree( e ) ;

						break ;
					}else{
						StringFree( e ) ;
					}
				}
			}

			zuluCryptSecurityDropElevatedPrivileges() ;

			if( f != StringVoid ){

				/*
				 * The volume is encrypted and mounted by any user,probably a different user
				 * since volumes mounted by this user are already checked.
				 */
				_print_device_properties( f,mapper_prefix,mapper_length,uid ) ;
			}else{
				/*
				 * volume is not mounted
				 */
				zuluMountPartitionProperties( device,NULL,device,NULL,NULL ) ;
			}
		}
	}

	StringListDelete( &stl ) ;
	StringFree( dev ) ;

	StringDelete( &z ) ;
	StringDelete( &q ) ;

	return 0 ;
}

int zuluMountUnEncryptedVolumeStatus( const char * device,const char * fs,const char * device1 )
{
	char * e ;
	char * z ;

	stringList_t stl ;

	string_t p ;
	string_t q ;

	/*
	 * zuluCryptGetMountEntry() is defined in ../zuluCrypt/cli/lib/process_mountinfo.c
	 */
	if( device1 != NULL ){

		p = zuluCryptGetMountEntry( device1 ) ;
	}else{
		p = zuluCryptGetMountEntry( device ) ;
	}

	stl = StringListStringSplit( p,' ' ) ;

	StringDelete( &p ) ;

	if( fs != NULL ){

		p = String_1( "\n type:   \t",fs,"\n cipher:   \tNil\n keysize:   \tNil\n offset:    \tNil\n",NULL ) ;
	}else{
		p = String( "\n type:   \tNil\n cipher:   \tNil\n keysize:   \tNil\n offset:    \tNil\n" ) ;
	}

	if( zuluCryptNoPartitionLoopDevice( device ) ){
		/*
		 * zuluCryptLoopDeviceAddress_1() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
		 */
		e = zuluCryptLoopDeviceAddress_1( device ) ;
		/*
		 * zuluCryptGetLoopDeviceAddress() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
		 */
		z = zuluCryptGetLoopDeviceAddress( device ) ;

		if( e != NULL && z != NULL ){

			StringMultipleAppend( p," device:   \t",z,"\n loop:   \t",e,NULL ) ;
		}else{
			StringMultipleAppend( p," device:   \t","Nil","\n loop:   \tNil",NULL ) ;
		}

		StringFree( e ) ;
		StringFree( z ) ;
	}else{
		StringMultipleAppend( p," device:   \t",device," \n loop:   \tNil",NULL ) ;
	}

	if( StringHasAtLeastOneComponent( StringListStringAt( stl,3 ),"ro,",",ro,",",ro",NULL ) ){

		StringAppend( p," \n mode:   \tread only\n active slots:\tNil" ) ;
	}else{
		StringAppend( p," \n mode:   \tread and write\n active slots:\tNil" ) ;
	}

	zuluCryptSecurityGainElevatedPrivileges() ;

	q = StringListStringAtSecondPlace( stl ) ;
	/*
	 * zuluCryptFileSystemProperties() is defined in ../zuluCrypt-cli/lib/status.c
	 * zuluCryptDecodeMountEntry() is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */

	if( device1 != NULL ){

		zuluCryptFileSystemProperties( p,device1,zuluCryptDecodeMountEntry( q ) ) ;
	}else{
		zuluCryptFileSystemProperties( p,device,zuluCryptDecodeMountEntry( q ) ) ;
	}


	zuluCryptSecurityDropElevatedPrivileges() ;

	/*
	 * zuluCryptSecurityUUIDFromPath() is defined in ../zuluCrypt/cli/bin/path_access.c
	 */
	e = zuluCryptUUIDFromPath( device ) ;

	if( e != NULL ){

		q = String( "" ) ;
		StringReplaceString( p,"\"Nil\"",StringMultipleAppend( q,"\"",e,"\"",NULL ) ) ;
		StringFree( e ) ;
		StringDelete( &q ) ;
	}

	StringPrintLine( p ) ;

	StringDelete( &p ) ;

	StringListDelete( &stl ) ;

	return 0 ;
}

int zuluMountVolumeStatus( const char * device,const char * UUID,uid_t uid )
{
	char * dev = NULL ;
	int st ;
	string_t p ;
	const char * e ;

	if( UUID == NULL ){

		if( StringPrefixEqual( device,"/dev/loop" ) ){

			/*
			 * zuluCryptLoopDeviceAddress_1() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
			 */
			dev = zuluCryptLoopDeviceAddress_1( device ) ;

			if( dev != NULL ){

				e = dev + StringLastIndexOfChar_1( dev,'/' ) + 1 ;
				st = zuluCryptEXEVolumeInfo( e,dev,uid ) ;
				free( dev ) ;
			}else{
				printf( gettext( "ERROR: Could not get volume properties,volume is not open or was opened by a different user\n" ) ) ;
				st = 1 ;
			}
		}else{
			e = device + StringLastIndexOfChar_1( device,'/' ) + 1 ;
			st = zuluCryptEXEVolumeInfo( e,device,uid ) ;
		}
	}else{
		p = String( UUID ) ;
		StringRemoveString( p,"\"" ) ;
		e = StringSubChar( p,4,'-' ) ;

		if( StringPrefixEqual( device,"/dev/loop" ) ){

			/*
			 * zuluCryptLoopDeviceAddress_1() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
			 */
			dev = zuluCryptLoopDeviceAddress_1( device ) ;

			if( dev != NULL ){

				st = zuluCryptEXEVolumeInfo( e,dev,uid ) ;
				StringFree( dev ) ;
			}else{
				printf( gettext( "ERROR: Could not get volume properties,volume is not open or was opened by a different user" ) ) ;
				st = 1 ;
			}
		}else{
			st = zuluCryptEXEVolumeInfo( e,device,uid ) ;
		}
		StringDelete( &p ) ;
	}

	return st ;
}

int zuluMountPrintBitLockerProperties( const char * device,uid_t uid )
{
	string_t xt ;
	string_t mt ;

	int r ;

	const char * e ;

	if( zuluCryptNoPartitionLoopDevice( device ) ){

		xt = zuluCryptLoopDeviceAddress_2( device ) ;
		mt = StringCopy( xt ) ;

		e = zuluCryptBitLockerCreateMapperPath( xt,uid ) ;

		r = zuluMountUnEncryptedVolumeStatus( StringContent( mt ),"bitlocker",e ) ;

		StringDelete( &mt ) ;
	}else{
		xt = String( device ) ;

		e = zuluCryptBitLockerCreateMapperPath( xt,uid ) ;

		r = zuluMountUnEncryptedVolumeStatus( device,"bitlocker",e ) ;
	}

	StringDelete( &xt ) ;

	return r ;
}
