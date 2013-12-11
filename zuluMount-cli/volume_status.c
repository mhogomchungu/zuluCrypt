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

static const char * _mapper_path ;
static size_t _mapper_length ;

void zuluMountPartitionProperties( const char * device,const char * UUID,const char * mapper,const char * m_point )
{
	#define SIZE 64

	const char * g ;
	const char * e ;

	blkid_probe blkid ;

	struct statvfs vfs ;
	uint64_t total ;
	uint64_t used ;
	uint64_t free_space ;
	uint32_t block_size ;
	int64_t blkid_device_size = 0 ;

	char buff[ SIZE ] ;
	char * buffer = buff ;
	char * loop_device ;

	zuluCryptSecurityGainElevatedPrivileges() ;

	blkid = blkid_new_probe_from_filename( device ) ;

	if( blkid == NULL ){
		zuluCryptSecurityDropElevatedPrivileges() ;
		printf( "%s\tNil\tNil\tNil\tNil\tNil\n",device ) ;
		return ;
	}

	if( UUID != NULL ){
		printf( "%s\t",UUID ) ;
	}else{
		if( StringPrefixEqual( device,"/dev/loop" ) ){
			/*
			 * zuluCryptLoopDeviceAddress_1() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
			 */
			loop_device = zuluCryptLoopDeviceAddress_1( device ) ;
			if( loop_device != NULL ){
				printf( "%s\t",loop_device ) ;
				free( loop_device ) ;
			}else{
				printf( "%s\t",device ) ;
			}
		}else{
			printf( "%s\t",device ) ;
		}
	}

	if( m_point == NULL ){
		printf( "Nil\t" ) ;
	}else{
		printf( "%s\t",m_point ) ;
	}

	if( StringsAreEqual( device,mapper ) ){

		blkid_do_probe( blkid ) ;

		blkid_device_size = ( int64_t ) blkid_probe_get_size( blkid ) ;

		if( blkid_probe_lookup_value( blkid,"TYPE",&g,NULL ) == 0 ){
			printf( "%s\t",g ) ;
		}else{
			printf( "Nil\t" ) ;
		}

		if( blkid_probe_lookup_value( blkid,"LABEL",&g,NULL ) == 0 ){
			printf( "%s",g ) ;
		}else{
			printf( "Nil" ) ;
		}

		blkid_free_probe( blkid ) ;
	}else{
		blkid_free_probe( blkid ) ;

		blkid = blkid_new_probe_from_filename( mapper ) ;

		if( blkid == NULL ){
			printf( "Nil\tNil" ) ;
		}else{
			blkid_do_probe( blkid ) ;

			blkid_device_size = ( int64_t ) blkid_probe_get_size( blkid ) ;
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
		}
	}

	if( m_point == NULL ){
		if( blkid_device_size == -1 ){
			printf( "\tNil\tNil\n" ) ;
		}else{
			zuluCryptFormatSize( blkid_device_size,buffer,SIZE ) ;
			printf( "\t%s\tNil\n",buffer ) ;
		}
	}else{
		if( statvfs( m_point,&vfs ) != 0 ){
			printf( "\tNil\tNil\n" ) ;
		}else{
			block_size = vfs.f_frsize ;

			total = block_size * vfs.f_blocks ;

			zuluCryptFormatSize( total,buffer,SIZE ) ;
			printf( "\t%s",buffer ) ;

			free_space = block_size * vfs.f_bavail  ;

			used = total - free_space ;

			snprintf( buff,SIZE,"%.2f%%",100 * ( ( float ) used / ( float ) total ) ) ;
			printf( "\t%s\n",buff ) ;
		}
	}

	zuluCryptSecurityDropElevatedPrivileges() ;
}

static void _printUnmountedVolumes( const char * device )
{
	zuluMountPartitionProperties( device,NULL,device,NULL ) ;
}

static void _printDeviceProperties( string_t entry,void * s )
{
	char * x ;

	const char * q ;
	const char * e ;
	const char * f ;

	ssize_t index ;

	string_t st = StringVoid ;

	stringList_t stz = ( stringList_t ) s ;
	stringList_t stx = StringListStringSplit( entry,' ' ) ;

	if( stx == StringListVoid ){
		return ;
	}

	q = StringListContentAt( stx,0 ) ;

	if( StringPrefixMatch( q,_mapper_path,_mapper_length ) ){
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
				StringRemoveLeft( st,index + 6 ) ;
				e = StringPrepend( st,"UUID=\"" ) ;
				index = StringLastIndexOfChar( st,'-' ) ;
				if( index >= 0 ){
					StringSubChar( st,index,'\"' ) ;
					e = StringSubChar( st,index+1,'\0' ) ;
				}
			}else{
				e = x ;
			}

			StringListRemoveString( stz,x ) ;
			/*
			 * zuluCryptDecodeMountEntry() is defined in ../zuluCrypt-cli/lib/mount_volume.c
			 * it decodes space,tab,new line and backslash characters since they are written differently in "/etc/mtab"
			 */
			f = zuluCryptDecodeMountEntry( StringListStringAt( stx,1 ) ) ;
			zuluMountPartitionProperties( x,e,q,f ) ;
			free( x ) ;
			StringDelete( &st ) ;
		}
	}else{
		StringListRemoveString( stz,q ) ;

		e = zuluCryptDecodeMountEntry( StringListStringAt( stx,0 ) ) ;
		f = zuluCryptDecodeMountEntry( StringListStringAt( stx,1 ) ) ;

		zuluMountPartitionProperties( e,NULL,e,f ) ;
	}

	zuluCryptSecurityDropElevatedPrivileges() ;
	StringListDelete( &stx ) ;
}

void zuluMountPrintDeviceProperties_1( string_t entry,uid_t uid )
{
	if( uid ){;}
	_mapper_path = zuluCryptMapperPrefix() ;
	_mapper_length = StringSize( _mapper_path ) ;
	_printDeviceProperties( entry,( void * )StringListVoid ) ;
}

/*
 * This function takes contents of "/etc/mtab" and "/proc/partitions" and compare them.
 * It first print information about partitions with entries in "/etc/mtab" and then
 * the remaining entries net effect being it prints information about partitions that
 * are mounted first and then print information about partitions that are not mounted.
 */
int zuluMountPrintMountedVolumes( uid_t uid )
{
	stringList_t stl ;
	stringList_t stz ;

	if( uid ){;}
	/*
	 * zuluCryptGetMountInfoList() is  defined in ../zuluCrypt-cli/lib/process_mountinfo.c
	 */
	stl = zuluCryptGetMountInfoList() ;

	if( stl == StringListVoid ){
		return 1;
	}
	/*
	 * zuluCryptPartitionList() is defined in ../zuluCrypt-cli/partitions.c
	 * It returns partition list read from /proc/partitions"
	 */
	stz = zuluCryptPartitionList() ;

	if( stz == StringListVoid ){
		StringListDelete( &stl ) ;
		return 1;
	}

	/*
	 * zuluCryptMapperPrefix() is defined in ../zuluCrypt-cli/lib/create_mapper_name.c
	 * it should return something like "/dev/mapper"
	 */
	_mapper_path = zuluCryptMapperPrefix() ;
	_mapper_length = StringSize( _mapper_path ) ;
	/*
	 * print a list of mounted partitions
	 */
	StringListForEach_1( stl,_printDeviceProperties,( void * )stz ) ;

	/*
	 * print a list of unmounted partitions
	 */
	StringListForEachString( stz,_printUnmountedVolumes ) ;

	StringListMultipleDelete( &stl,&stz,ENDDELETE ) ;

	return 0 ;
}

int zuluMountprintAListOfMountedVolumes( void )
{
	/*
	 * This function may print the same device more than once if there exists a normal mount and atleast
	 * a single bind mount.This behavior is expected and is desired since the list given here must match
	 * the list given with "zuluMount-cli -l". zuluMount-gui will go crazy if the two lists do not match
	 */

	/*
	 * zuluCryptGetMountInfoList() is defined in ../zuluCrypt-cli/lib/process_mountinfo.c
	 */
	stringList_t stz = zuluCryptGetMountInfoList() ;

	StringListIterator it  = StringListBegin( stz ) ;
	StringListIterator end = StringListEnd( stz ) ;

	string_t st ;
	string_t q ;

	const char * e ;
	const char * f ;
	/*
	 * zuluCryptMapperPrefix() is defined in ../zuluCrypt-cli/lib/create_mapper_name.c
	 * mapper_prefix will probably contain "/dev/mapper/"
	 */
	const char * mapper_prefix = zuluCryptMapperPrefix() ;

	while( it != end ){
		st = *it ;
		it++ ;

		if( !StringStartsWith( st,"/" ) || StringAtLeastOneStartsWith( st,"/proc","/sys","/dev ",NULL ) ){
			continue ;
		}

		e = StringReplaceChar_1( st,0,' ','\0' ) ;

		if( StringPrefixEqual( e,mapper_prefix ) ){
			/*
			 * zuluCryptConvertIfPathIsLVM() is defined in ../zuluCrypt-cli/lib/status.c
			 */
			q = zuluCryptConvertIfPathIsLVM( e ) ;

			if( StringStartsWith( q,mapper_prefix ) ){
				/*
				 * volume is probably an encrypted one
				 */
				if( StringContains( q,"-NAAN-" ) ){
					zuluCryptSecurityGainElevatedPrivileges() ;
					/*
					 * zuluCryptVolumeDeviceName() is defined in ../zuluCrypt-cli/lib/status.c
					 */
					f = zuluCryptVolumeDeviceName( e ) ;
					zuluCryptSecurityDropElevatedPrivileges() ;
					if( f != NULL ){
						puts( f ) ;
						StringFree( f ) ;
					}
				}else if( StringContains( q,"-UUID-" ) ){
					StringReplaceString( q,"-UUID-","-UUID=\"" ) ;
					e = StringAppend( st,"\"" ) ;
					e = e + StringHasComponent_1( e,"UUID=" ) ;
					puts( e ) ;
				}else{
					/*
					 * Still assuming its an encrypted volume opened and outside of zuluCrypt/zuluMount
					 */
					zuluCryptSecurityGainElevatedPrivileges() ;
					f = zuluCryptVolumeDeviceName( e ) ;
					zuluCryptSecurityDropElevatedPrivileges() ;
					if( f != NULL ){
						puts( f ) ;
						StringFree( f ) ;
					}else{
						/*
						 * not exactly sure what this is,just print it and let the user sort it out
						 */
						zuluCryptDecodeMountEntry( st ) ;
						StringPrintLine( st ) ;
					}
				}
			}else{
				/*
				 * the volume is probably an LVM volume
				 */
				zuluCryptDecodeMountEntry( st ) ;
				StringPrintLine( st ) ;
			}

			StringDelete( &q ) ;
		}else{
			zuluCryptDecodeMountEntry( st ) ;
			StringPrintLine( st ) ;
		}
	}

	StringListDelete( &stz ) ;

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

	StringListIterator it  ;
	StringListIterator end ;

	/*
	 * zuluCryptGetMoutedListFromMountInfo() is defined in ../zuluCrypt-cli/lib/process_mountinfo.c
	 */
	stringList_t stl = zuluCryptGetMoutedListFromMountInfo() ;

	device_1 = device ;

	if( UUID == NULL ){
		if( StringPrefixEqual( device,"/dev/loop" ) ){
			/*
			 * zuluCryptLoopDeviceAddress() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
			 */
			device = dev = zuluCryptLoopDeviceAddress( device ) ;

			z = String( dev ) ;
			device_1 = StringReplaceString( z,"\\040"," " ) ;
			/*
			 * ZULUCRYPTlongMapperPath is set in ../zuluCrypt-cli/constants.h
			 * zuluCryptCreateMapperName() is defined at ../zuluCrypt-cli/lib/create_mapper_name.c
			 */
			q = zuluCryptCreateMapperName( device_1,strrchr( device_1,'/' ) + 1,uid,ZULUCRYPTlongMapperPath ) ;
		}else{
			/*
			 * ZULUCRYPTlongMapperPath is set in ../zuluCrypt-cli/constants.h
			 * zuluCryptCreateMapperName() is defined at ../zuluCrypt-cli/lib/create_mapper_name.c
			 */
			q = zuluCryptCreateMapperName( device,strrchr( device,'/' ) + 1,uid,ZULUCRYPTlongMapperPath ) ;
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
		zuluMountPrintDeviceProperties_1( p,uid ) ;
	}else{
		/*
		 * We will get if:
		 * 1. The volume is not mounted.
		 * 2. The volume is mounted,encrypted and opened by a different user
		 * 3. The volume is mouted and unencrypted
		 */
		f = String( device ) ;
		p = StringListHasStartSequence_1( stl,StringAppend( f," " ) ) ;
		StringDelete( &f ) ;
		if( p != StringVoid ){
			/*
			 * volume is unencrypted and mounted by any user
			 */
			zuluMountPrintDeviceProperties_1( p,uid ) ;
		}else{
			/*
			 * We will get here is:
			 * 1. The volume is not mounted
			 * 2. The volume is encrypted and mounted by a different user
			 */
			it  = StringListBegin( stl ) ;
			end = StringListEnd( stl ) ;
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

					if( StringsAreEqual( e,device_1 ) ){
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
				 * The volume is encrypted and mounted by a any user,probably a different user
				 * since this user condition is above
				 */
				zuluMountPrintDeviceProperties_1( f,uid ) ;
			}else{
				/*
				 * volume is not mounted
				 */
				zuluMountPartitionProperties( device_1,NULL,device_1,NULL ) ;
			}
		}
	}

	StringListDelete( &stl ) ;
	StringFree( dev ) ;

	StringDelete( &z ) ;
	StringDelete( &q ) ;

	return 0 ;
}

int zuluMountUnEncryptedVolumeStatus( const char * device )
{
	char * e ;
	char * z ;

	stringList_t stl ;

	string_t p ;
	string_t q ;

	/*
	 * zuluCryptGetMountEntry() is defined in ../zuluCrypt/cli/lib/process_mountinfo.c
	 */
	p = zuluCryptGetMountEntry( device ) ;

	stl = StringListStringSplit( p,' ' ) ;

	StringDelete( &p ) ;

	p = String( "\n type:   \tNil\n cipher:   \tNil\n keysize:   \tNil\n" ) ;

	if( StringPrefixEqual( device,"/dev/loop" ) ){
		/*
		 * zuluCryptLoopDeviceAddress_1() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
		 */
		e = zuluCryptLoopDeviceAddress_1( device ) ;
		/*
		 * zuluCryptGetLoopDeviceAddress() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
		 */
		z = zuluCryptGetLoopDeviceAddress( device ) ;
		if( e != NULL && z != NULL ){
			StringMultipleAppend( p," device:   \t",z,"\n loop:   \t",e,"\n offset:    \tNil",NULL ) ;
		}else{
			StringMultipleAppend( p," device:   \t","Nil","\n loop:   \t","Nil","\n offset:    \tNil",NULL ) ;
		}
		StringFree( e ) ;
		StringFree( z ) ;
	}else{
		StringMultipleAppend( p," device:   \t",device," \n loop:   \tNil\n offset:    \tNil",NULL ) ;
	}

	if( StringHasAtLeastOneComponent( StringListStringAt( stl,3 ),"ro,",",ro,",",ro",NULL ) ){
		StringAppend( p," \n mode:   \tread only\n active slots:\tNil" ) ;
	}else{
		StringAppend( p," \n mode:   \tread and write\n active slots:\tNil" ) ;
	}

	zuluCryptSecurityGainElevatedPrivileges() ;

	q = StringListStringAt( stl,1 ) ;
	/*
	 * zuluCryptFileSystemProperties() is defined in ../zuluCrypt-cli/lib/status.c
	 * zuluCryptDecodeMountEntry() is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	zuluCryptFileSystemProperties( p,device,zuluCryptDecodeMountEntry( q ) ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	/*
	 * zuluCryptSecurityUUIDFromPath() is defined in ../zuluCrypt/cli/bin/path_access.c
	 */
	e = zuluCryptUUIDFromPath( device ) ;

	if( e != NULL ){
		q = String( "" ) ;
		StringReplaceString( p,"\"Nil\"",StringMultipleAppend( q,"\"",e,"\"",NULL ) ) ;
		free( e ) ;
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
				st = zuluCryptEXEVolumeInfo( strrchr( dev,'/' ) + 1,dev,uid ) ;
				free( dev ) ;
			}else{
				printf( gettext( "ERROR: could not get volume properties,volume is not open or was opened by a different user\n" ) ) ;
				st = 1 ;
			}
		}else{
			st = zuluCryptEXEVolumeInfo( strrchr( device,'/' ) + 1,device,uid ) ;
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
				free( dev ) ;
			}else{
				printf( gettext( "ERROR: could not get volume properties,volume is not open or was opened by a different user" ) ) ;
				st = 1 ;
			}
		}else{
			st = zuluCryptEXEVolumeInfo( e,device,uid ) ;
		}
		StringDelete( &p ) ;
	}

	return st ;
}
