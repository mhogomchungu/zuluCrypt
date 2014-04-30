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
#include "../lib/includes.h"
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libintl.h>
#include <locale.h>

#include <blkid/blkid.h>

#include "udev_support.h"

/*
 * NOTE: This source file was previously named "partition.c"
 */

/*
 * This source file deals with parsing partition list from "/proc/partitions,"/etc/fstab" and "/etc/mtab".
 *
 * For security reasons,a normal user it is not allowed to create volumes in system partitions.
 *
 * A System partition is defined as a partition with an active entries in /etc/fstab and/or /etc/crypttab.
 *
 * This policy is in place to prevent a normal user from attempting to create volumes in internal partitions
 * intentially to destroy other people's data.
 *
 * External,pluggable usb based partitions are not considered to be system partitions and the tool can be used to create volumes in those.
 *
 * Internal partitions are considered part of the system and the policy is in place to make sure a normal user does not
 * perform actions reserved for root user.
 *
 */

/*
 * this function reads a line from a fine, it does what gets() does,it just handles the memory dynamically *
 */

/*
 * major minor  #blocks  name
 *
 8        0   78150744 sda *
 8        1   11566768 sda1
 8        2          1 sda2
 8        5   66581361 sda5
 8       16  312571224 sdb
 8       17    1044193 sdb1
 8       18          1 sdb2
 8       21  311524416 sdb5
 8       32     250879 sdc
 8       33     250608 sdc1
 *
 * above output is the output of "cat /proc/partitions" and below function was build again it.
 * The list of partitions is taken from the 4th field and only sdX and hdY entries are taken
 *
 */

static inline int _allowedDevice( const char * device )
{
	const char * fsType ;
	int st ;
	blkid_probe blkid ;
	int sts ;

	if( StringPrefixEqual( device,"/dev/sr" ) ){
		/*
		 * device is probably a cdrom or dvdrom,allow them
		 */
		return 1 ;
	}

	sts = StringSize( device ) ;

	if( sts == 8 ){
		/*
		 * we will get here with a device with an address of "/dev/XYZ".
		 * This device is either not partitioned or is a root address of a partitioned device
		 * Support it only if it has a recognizable file system.
		 */
		blkid = blkid_new_probe_from_filename( device ) ;
		if( blkid == NULL ){
			return 0 ;
		}else{
			blkid_do_probe( blkid ) ;
			st = blkid_probe_lookup_value( blkid,"TYPE",&fsType,NULL ) ;
			blkid_free_probe( blkid ) ;
			return st == 0 ;
		}
	}else if( sts > 8 ){
		return StringAtLeastOnePrefixMatch( device,"/dev/hd","/dev/sd","/dev/md","/dev/mmc","/dev/loop",NULL ) ;
	}else{
		return 0 ;
	}
}

static stringList_t _zuluCryptAddLVMVolumes( stringList_t stl )
{
	struct dirent * entry ;

	string_t st ;
	string_t xt ;

	DIR * dir = opendir( "/dev/mapper/" ) ;

	if( dir != NULL ){
		st = String( "/dev/mapper/" ) ;
		while( ( entry = readdir( dir ) ) != NULL ){
			if( !StringAtLeastOneMatch_1( entry->d_name,".","..","control",NULL ) ){
				/*
				 * zuluCryptConvertIfPathIsLVM() is defined in ../lib/resolve_paths.c
				 */
				xt = zuluCryptConvertIfPathIsLVM( StringAppendAt( st,12,entry->d_name ) ) ;
				if( StringStartsWith( xt,"/dev/mapper/" ) ){
					StringDelete( &xt ) ;
				}else{
					stl = StringListAppendString_1( stl,&xt ) ;
				}
			}
		}
		StringDelete( &st ) ;
		closedir( dir ) ;
	}

	return stl ;
}

static stringList_t _zuluCryptAddMDRAIDVolumes( stringList_t stl )
{
	DIR * dir = opendir( "/dev/md/" ) ;
	struct dirent * entry ;
	char * e ;
	const char * f ;
	string_t st = StringVoid ;

	if( dir != NULL ){
		while( ( entry = readdir( dir ) ) != NULL ){
			f = entry->d_name ;
			if( !StringAtLeastOneMatch_1( f,".","..","md-device-map",NULL ) ){
				st = String( "/dev/md/" ) ;
				e = zuluCryptRealPath( StringAppend( st,f ) ) ;
				if( e != NULL ){
					StringListRemoveString( stl,e ) ;
					free( e ) ;
				}
				stl = StringListAppendString_1( stl,&st ) ;
			}
		}
		closedir( dir ) ;
	}

	return stl ;
}

/*
 * It is possible for a btrfs volume to cover multiple volumes and this routine
 * keeps only the first one seen and removes the rest.
 */
static stringList_t _remove_btfs_multiple_devices( stringList_t stl )
{
	stringList_t stx = StringListVoid ;
	stringList_t stz = StringListVoid ;

	StringListIterator it  ;
	StringListIterator end ;

	string_t st ;

	const char * e = NULL ;

	blkid_probe blkid ;

	StringListGetIteratorBeginAndEnd( stl,&it,&end ) ;

	zuluCryptSecurityGainElevatedPrivileges() ;

	while( it != end ){
		st = *it ;
		it++ ;
		blkid = blkid_new_probe_from_filename( StringContent( st ) ) ;
		if( blkid != NULL ){
			blkid_do_probe( blkid ) ;
			blkid_probe_lookup_value( blkid,"TYPE",&e,NULL ) ;
			if( StringsAreEqual( e,"btrfs" ) ){
				blkid_probe_lookup_value( blkid,"UUID",&e,NULL ) ;
				if( StringListHasNoEntry( stx,e ) ){
					/*
					 * we got a btrfs volume with UUID we do not know about,
					 * This will be the only device with this btrfs UUID we support and
					 * all device operations must happen through this device and this device only.
					 */
					stz = StringListAppendString( stz,st ) ;
					stx = StringListAppend( stx,e ) ;
				}else{
					/*
					 * we already know this UUID and this device is not supported.Any operation on this
					 * device should fail.
					 */
				}
			}else{
				/*
				 * not a btrfs volume
				 */
				stz = StringListAppendString( stz,st ) ;
			}
			blkid_free_probe( blkid ) ;
		}else{
			stz = StringListAppendString( stz,st ) ;
		}
	}

	zuluCryptSecurityDropElevatedPrivileges() ;

	StringListMultipleDelete( &stl,&stx,NULL ) ;
	return stz ;
}

static stringList_t _zuluCryptVolumeList_0( int resolve_loop_devices )
{
	const char * device ;

	const char * e ;

	ssize_t index ;

	StringListIterator it ;
	StringListIterator end ;

	stringList_t stz   = StringListVoid ;
	stringList_t stl   = StringListVoid ;
	stringList_t stl_1 = StringListVoid ;

	string_t st = StringGetFromVirtualFile( "/proc/partitions" ) ;
	string_t st_1 = String( "/dev/" ) ;

	stl = StringListStringSplit( st,'\n' ) ;

	StringDelete( &st ) ;

	if( stl == StringListVoid ){
		return StringListVoid ;
	}

	StringListGetIteratorBeginAndEnd( stl,&it,&end ) ;

	/*
	 * skip the first entry
	 */
	it++ ;

	zuluCryptSecurityGainElevatedPrivileges() ;
	while( it != end ){
		st = *it ;
		it++ ;
		index = StringLastIndexOfChar( st,' ' ) ;
		if( index != -1 ){
			e = StringContent( st ) + index + 1 ;
			device = StringAppendAt( st_1,5,e ) ;
			if( _allowedDevice( device ) ){
				if( StringPrefixEqual( device,"/dev/loop" ) ){
					/*
					 * Here we only keep one loop device if the volume file has
					 * more than one loop device
					 */
					e = zuluCryptLoopDeviceAddress_1( device ) ;
					if( StringListHasNoEntry( stz,e ) ){
						if( resolve_loop_devices ){
							stl_1 = StringListAppend( stl_1,e ) ;
						}else{
							stl_1 = StringListAppend( stl_1,device ) ;
						}
						stz = StringListAppend( stz,e ) ;
					}
					StringFree( e ) ;
				}else{
					stl_1 = StringListAppendIfAbsent( stl_1,device ) ;
				}
			}
		}
	}
	zuluCryptSecurityDropElevatedPrivileges() ;
	StringListMultipleDelete( &stl,&stz,NULL ) ;
	StringDelete( &st_1 ) ;
	return _zuluCryptAddLVMVolumes( _zuluCryptAddMDRAIDVolumes( stl_1 ) ) ;
}

stringList_t zuluCryptVolumeList( void )
{
	return _zuluCryptVolumeList_0( 0 ) ;
}

stringList_t zuluCryptGetAListOfAllVolumes( void )
{
	return _remove_btfs_multiple_devices( _zuluCryptVolumeList_0( 1 ) ) ;
}

int zuluCryptDeviceIsSupported( const char * device,uid_t uid )
{
	stringList_t stl ;
	int r ;

	if( StringPrefixEqual( device,"/dev/loop" ) ){
		return 1 ;
	}else{
		stl = zuluCryptPartitions( ZULUCRYPTallPartitions,uid ) ;
		r = StringListHasEntry( stl,device ) ;
		StringListDelete( &stl ) ;
		if( r == 1 ){
			return 1 ;
		}else{
			/*
			 * zuluCryptUserIsAMemberOfAGroup() is defined in security.c
			 */
			return zuluCryptUserIsAMemberOfAGroup( uid,"zulucrypt" ) ;
		}
	}
}

static int _zuluCryptCheckSYSifDeviceIsSystem( const char * device )
{
	/*
	 * UDEV_SUPPORT is set at configure time by "-DUDEVSUPPORT=true" option,the option being absent equals "-DUDEVSUPPORT=false"
	 * To set the option, configure with "-DUDEVSUPPORT=true"
	 */
#if UDEV_SUPPORT
	/*
	 * udev support is enabled
	 */
	int r ;
	size_t e ;
	ssize_t k ;
	string_t xt ;
	string_t st ;
	char dev[ PATH_MAX + 1 ] ;

	const char * path ;

	if( StringPrefixNotEqual( device,"/dev/" ) ){
		/*
		 * udev doesnt work with path to image files so return early
		 */
		return 0 ;
	}

	if( StringPrefixEqual( device,"/dev/loop" ) ){
		/*
		 * udev thinks all loop devices are system devices and we disagree and hence we return early
		 */
		return 0 ;
	}

	path = realpath( device,dev ) ;

	if( path != NULL ){
		st = String( path ) ;
	}else{
		st = String( device ) ;
	}

	if( StringStartsWithAtLeastOne( st,"/dev/sd","/dev/hd",NULL ) ){
		/*
		 * this path will convert something like: "/dev/sdc12" to "/dev/sdc"
		 */
		StringRemoveDigits( st ) ;
	}else if( StringStartsWith( st,"/dev/mmc" ) ){
		/*
		 * device path will be something like "/dev/mmcblk0p2" and what we want to do
		 * is cut off the string from p to end iwth "/dev/mmcblk0"
		 */
		k = StringIndexOfChar( st,0,'p' ) ;
		if( k != -1 ){
			e = StringLength( st ) - ( size_t )k ;
			StringRemoveRight( st,e ) ;
		}
	}

	StringReplaceString( st,"/dev/","/sys/block/" ) ;
	path = StringAppend( st,"/removable" ) ;
	/*
	 * path will be something like "/sys/block/sda/removable"
	 */
	xt = StringGetFromVirtualFile( path ) ;
	StringDelete( &st ) ;
	if( xt == StringVoid ){
		return 0 ;
	}else{
		r = StringEqual( xt,"0\n" ) ;
		StringDelete( &xt ) ;
		return r ;
	}
#else
	if( device ){;}
	/*
	 * udev support is disabled
	 */
	return 0 ;
#endif
}

stringList_t zuluCryptPartitions( int option,uid_t uid )
{
	const char * device ;
	const char * e ;

	stringList_t non_system = StringListVoid ;
	stringList_t system     = StringListVoid ;

	string_t st ;

	stringList_t p ;
	stringList_t stl = zuluCryptVolumeList() ;

	StringListIterator start ;
	StringListIterator it  ;
	StringListIterator end ;

	if( stl == StringListVoid ){
		return StringListVoid ;
	}
	if( option == ZULUCRYPTallPartitions ){
		return _remove_btfs_multiple_devices( stl ) ;
	}

	non_system = stl ;

	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptGetFstabList() is defined in ../lib/parse_fstab.c
	 */
	stl = zuluCryptGetFstabList( uid ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;

	StringListGetIteratorBeginAndEnd( stl,&it,&end ) ;
	/*
	 * gather an initial list of system and non system partitions by comparing entries in "/etc/fstab" and "/proc/partitions"
	 * fstab entries makes an initial list of system partitions.
	 * the difference btw list in "/proc/partitions" and "/etc/fstab" makes an initial list of non system partitions.
	 */
	while( it != end ){
		st = *it ;
		it++ ;
		if( StringStartsWith( st,"/" ) ){
			device = StringReplaceChar_1( st,0,' ','\0' ) ;
			system = StringListAppend( system,device ) ;
			StringListRemoveString( non_system,device ) ;
		}
	}

	StringListDelete( &stl ) ;

	/*
	 * read entried from "crypttab" and then add them to "system" if absent in that list and remove them from "non system" if present
	 * in that list
	 */
	p = zuluCryptGetPartitionFromCrypttab() ;
	if( p != StringListVoid ){
		StringListGetIteratorBeginAndEnd( p,&it,&end ) ;
		while( it != end ){
			device = StringContent( *it ) ;
			it++ ;
			StringListAppendIfAbsent( system,device ) ;
			StringListRemoveIfPresent( non_system,device ) ;
		}
		StringListDelete( &p ) ;
	}

	/*
	 * read entried from "zuluCrypt-system" and then add them to "system" if absent in that list and remove them from "non system" if present
	 * in that list
	 */
	p = zuluCryptGetPartitionFromConfigFile( "/etc/zuluCrypt-system" ) ;
	if( p == StringListVoid ){
		/*
		 * This is the new path since zuluCrypt 4.6.9
		 */
		p = zuluCryptGetPartitionFromConfigFile( "/etc/zuluCrypt/system_volumes.list" ) ;
	}
	if( p != StringListVoid ){
		StringListGetIteratorBeginAndEnd( p,&it,&end ) ;
		while( it != end ){
			device = StringContent( *it ) ;
			it++ ;
			StringListAppendIfAbsent( system,device ) ;
			StringListRemoveIfPresent( non_system,device ) ;
		}
		StringListDelete( &p ) ;
	}

	/*
	 * At this point:
	 * "system" contains system devices gathered from fstab,zuluCrypt-system and crypttab
	 * "non_system" contains non system devices gathered from /proc/partitions minus system partitions.
	 */

	it = StringListBegin( non_system ) ;
	start = it ;

	/*
	 * now we consult udev if enabled and we move partition in the "non system" list to "system" list if udev think they are system
	 */
	while( it != StringListEnd( non_system ) ){
		e = StringContent( *it ) ;
		if( _zuluCryptCheckSYSifDeviceIsSystem( e ) ){
			StringListAppendIfAbsent( system,e ) ;
			StringListRemoveAt( non_system,it - start ) ;
		}else{
			it++ ;
		}
	}

	/*
	 * Now we read from a config file that contains devices that are not to be considered system and remove them from
	 * the system list if present in that list and add them to non system list if absent in that list
	 */
	p = zuluCryptGetPartitionFromConfigFile( "/etc/zuluCrypt-nonsystem" ) ;
	if( p == StringListVoid ){
		/*
		 * This is the new path since zuluCrypt 4.6.9
		 */
		p = zuluCryptGetPartitionFromConfigFile( "/etc/zuluCrypt/nonsystem_volumes.list" ) ;
	}
	if( p != StringListVoid ){
		StringListGetIteratorBeginAndEnd( p,&it,&end ) ;
		while( it != end ){
			device = StringContent( *it ) ;
			it++ ;
			StringListRemoveString( system,device ) ;
			StringListAppendIfAbsent( non_system,device ) ;
		}
		StringListDelete( &p ) ;
	}

	if( option == ZULUCRYPTsystemPartitions ){
		StringListDelete( &non_system ) ;
		return _remove_btfs_multiple_devices( system ) ;
	}else{
		StringListDelete( &system ) ;
		return _remove_btfs_multiple_devices( non_system ) ;
	}
}

int zuluCryptVolumeIsInSystemVolumeList( const char * device )
{
	stringList_t p = zuluCryptGetPartitionFromConfigFile( "/etc/zuluCrypt/system_volumes.list" ) ;
	int r = StringListHasEntry( p,device ) ;
	StringListDelete( &p ) ;
	return r ;
}

u_int64_t zuluCryptGetVolumeSize( const char * device )
{
	stringList_t stl = StringListVoid ;

	StringListIterator it  ;
	StringListIterator end ;

	string_t xt ;

	const char * e = NULL ;

	u_int64_t r = 0 ;

	blkid_probe blkid = blkid_new_probe_from_filename( device ) ;

	if( blkid == NULL ){
		return 0 ;
	}
	blkid_do_probe( blkid ) ;
	blkid_probe_lookup_value( blkid,"TYPE",&e,NULL ) ;

	if( StringsAreNotEqual( e,"btrfs" ) ){
		r = blkid_probe_get_size( blkid ) ;
		blkid_free_probe( blkid ) ;
		return r ;
	}else{
		/*
		 * we got a btrfs volume,this device could be one among a bunch of devices that makes the btfs volume.
		 * iterate through all known devices and add their sizes to this device if they are a part of the same
		 * btrfs volume.
		 */
		if( blkid_probe_lookup_value( blkid,"UUID",&e,NULL ) == 0 ){
			xt = String( e ) ;
		}else{
			xt = StringVoid ;
		}

		blkid_free_probe( blkid ) ;

		if( xt == StringVoid ){
			return 0 ;
		}else{
			/*
			 * zuluCryptVolumeList() is defined in this source file
			 */
			stl = zuluCryptVolumeList() ;
			zuluCryptSecurityGainElevatedPrivileges() ;

			StringListGetIteratorBeginAndEnd( stl,&it,&end ) ;

			while( it != end ){
				blkid = blkid_new_probe_from_filename( StringContent( *it ) ) ;
				it++ ;
				if( blkid != NULL ){
					blkid_do_probe( blkid ) ;
					if( blkid_probe_lookup_value( blkid,"UUID",&e,NULL ) == 0 ){
						if( StringEqual( xt,e ) ){
							r += blkid_probe_get_size( blkid ) ;
						}
					}
					blkid_free_probe( blkid ) ;
				}
			}
			StringDelete( &xt ) ;
			StringListDelete( &stl ) ;
			return r ;
		}
	}
}

void zuluCryptPrintPartitionProperties( const char * device )
{
	#define SIZE 64
	char buffer[ SIZE ] ;

	const char * e ;
	u_int64_t size ;
	blkid_probe blkid ;

	zuluCryptSecurityGainElevatedPrivileges() ;

	if( StringPrefixEqual( device,"/dev/loop" ) ){
		/*
		 * zuluCryptLoopDeviceAddress_1() is defined in ../lib/create_loop_device.c
		 */
		e = zuluCryptLoopDeviceAddress_1( device ) ;
		if( e != NULL ){
			printf( "%s\t",e ) ;
			StringFree( e ) ;
		}else{
			printf( "%s\t",device ) ;
		}
	}else{
		printf( "%s\t",device ) ;
	}

	size = zuluCryptGetVolumeSize( device ) ;

	blkid = blkid_new_probe_from_filename( device ) ;

	if( blkid == NULL ){
		printf( "Nil\tNil\tNil\tNil\n" ) ;
	}else{
		/*
		 * zuluCryptFormatSize() is defined in ../lib/status.c
		 */
		zuluCryptFormatSize( size,buffer,SIZE ) ;

		printf( "%s\t",buffer ) ;

		blkid_do_probe( blkid ) ;

		if( blkid_probe_lookup_value( blkid,"LABEL",&e,NULL ) == 0 ){
			printf( "%s\t",e ) ;
		}else{
			printf( "Nil\t" ) ;
		}
		if( blkid_probe_lookup_value( blkid,"TYPE",&e,NULL ) == 0 ){
			printf( "%s\t",e ) ;
		}else{
			printf( "Nil\t" ) ;
		}
		if( blkid_probe_lookup_value( blkid,"UUID",&e,NULL ) == 0 ){
			printf( "%s\n",e ) ;
		}else{
			printf( "Nil\n" ) ;
		}
		blkid_free_probe( blkid ) ;
	}

	zuluCryptSecurityDropElevatedPrivileges() ;
}

static void _zuluCryptPrintUnMountedPartitionProperties( stringList_t stl )
{
	/*
	 * zuluCryptGetMoutedListFromMountInfo() is defined in ../lib/process_mountinfo.c
	 */
	stringList_t stx = zuluCryptGetMoutedListFromMountInfo() ;

	StringListIterator it  ;
	StringListIterator end ;

	string_t st ;

	StringListGetIteratorBeginAndEnd( stl,&it,&end ) ;

	while( it != end ){
		st = *it ;
		it++ ;
		if( StringListHasStartSequence( stx,StringAppend( st," " ) ) == -1 ){
			zuluCryptPrintPartitionProperties( StringRemoveRight( st,1 ) ) ;
		}
	}

	StringListDelete( &stx ) ;
}

static void _print_list( stringList_t stl )
{
	const char * e ;
	char * z ;

	StringListIterator it  ;
	StringListIterator end ;

	StringListGetIteratorBeginAndEnd( stl,&it,&end ) ;

	while( it != end ){
		e = StringContent( *it ) ;
		it++ ;
		if( StringPrefixEqual( e,"/dev/loop" ) ){
			/*
			 * zuluCryptLoopDeviceAddress_1() is defined in ../lib/create_loop_device.c
			 */
			z = zuluCryptLoopDeviceAddress_1( e ) ;
			if( z != NULL ){
				puts( z ) ;
				StringFree( z ) ;
			}else{
				puts( e ) ;
			}
		}else{
			puts( e ) ;
		}
	}
}

int zuluCryptPrintPartitions( int option,int info,uid_t uid )
{
	stringList_t stl = StringListVoid ;

	switch( option ){
		case 1 : stl = zuluCryptPartitions( ZULUCRYPTallPartitions,uid )       ;break ;
		case 2 : stl = zuluCryptPartitions( ZULUCRYPTsystemPartitions,uid )    ;break ;
		case 3 : stl = zuluCryptPartitions( ZULUCRYPTnonSystemPartitions,uid ) ;break ;
	}

	if( stl != StringListVoid ){
		switch( info ){
			case 1 : StringListForEachString( stl,zuluCryptPrintPartitionProperties )   ; break ;
			case 2 : _zuluCryptPrintUnMountedPartitionProperties( stl ) 	   	    ; break ;
			default: _print_list( stl ) ;
		}
		StringListDelete( &stl ) ;
	}

	return 0 ;
}

static stringList_t _eval_path( string_t path,stringList_t stl_1 )
{
	string_t st ;
	const char * e ;
	char * ac ;
	if( StringStartsWith( path,"/" ) ){
		/*
		 * zuluCryptResolvePath_1() is defined in resolve_paths.c
		 */
		st = zuluCryptResolvePath_1( StringContent( path ) ) ;

		if( st != StringVoid ){
			stl_1 = StringListAppendString_1( stl_1,&st ) ;
		}
	}else if( StringStartsWith( path,"UUID=" ) ){
		/*
		 * check above did not find '/' character and we are in this block assuming the line uses UUID
		 */
		e = StringRemoveString( path,"\"" ) ;
		/*
		 * zuluCryptEvaluateDeviceTags() is defined in path_access.c
		 */
		ac = zuluCryptEvaluateDeviceTags( "UUID",e + 5 ) ;
		if( ac != NULL ){
			stl_1 = StringListAppend( stl_1,ac ) ;
			StringFree( ac ) ;
		}
	}
	return stl_1 ;
}

/*
 * this function will parse /etc/crypttab to see if it has any entries to be used as system partition.
 *
 * sample example of the file content this function was build on.
 *

 * secret /dev/sda15 none
 * secret_1 UUID=d2d210b8-0b1f-419f-9172-9d509ea9af0c none
 *
 */
stringList_t zuluCryptGetPartitionFromCrypttab( void )
{
	stringList_t stl   = StringListVoid ;
	stringList_t stl_1 = StringListVoid ;
	stringList_t stz ;

	string_t st  ;

	StringListIterator it  ;
	StringListIterator end ;

	st = StringGetFromFile( "/etc/crypttab" ) ;

	stl = StringListStringSplit( st,'\n' ) ;

	StringDelete( &st ) ;

	StringListGetIteratorBeginAndEnd( stl,&it,&end ) ;

	while( it != end ){
		st = *it ;
		it++ ;
		if( !StringStartsWith( st,"#" ) ){
			stz = StringListStringSplit( st,' ' ) ;
			st = StringListStringAtSecondPlace( stz ) ;
			stl_1 = _eval_path( st,stl_1 ) ;
			StringListDelete( &stz ) ;
		}
	}

	StringListDelete( &stl ) ;
	return stl_1 ;
}

stringList_t zuluCryptGetPartitionFromConfigFile( const char * path )
{
	StringListIterator it  ;
	StringListIterator end ;

	stringList_t stl ;
	stringList_t stl_1 = StringListVoid ;

	string_t st = StringVoid ;

	zuluCryptSecurityGainElevatedPrivileges() ;
	st = StringGetFromFile( path ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;

	stl = StringListStringSplit( st,'\n' ) ;

	StringDelete( &st ) ;

	StringListGetIteratorBeginAndEnd( stl,&it,&end ) ;

	while( it != end ){
		stl_1 = _eval_path( *it,stl_1 ) ;
		it++ ;
	}

	StringListDelete( &stl ) ;

	return stl_1 ;
}

int _zuluCryptPartitionIsSystemPartition( const char * dev,uid_t uid )
{
	stringList_t stl ;
	int r = 0 ;
	stl = zuluCryptPartitions( ZULUCRYPTsystemPartitions,uid ) ;
	if( stl != StringListVoid ){
		r = StringListHasEntry( stl,dev ) ;
		StringListDelete( &stl ) ;
	}
	return r ;
}

int zuluCryptPartitionIsSystemPartition( const char * device,uid_t uid )
{
	char * dev ;
	int r ;
	if( StringPrefixEqual( device,"/dev/loop" ) ){
		/*
		 * zuluCryptLoopDeviceAddress_1() is defined in ../lib/create_loop_device.c
		 */
		dev = zuluCryptLoopDeviceAddress_1( device ) ;
		if( dev == NULL ){
			return 0 ;
		}else{
			r = _zuluCryptPartitionIsSystemPartition( dev,uid ) ;
			StringFree( dev ) ;
			return r ;
		}
	}else{
		return _zuluCryptPartitionIsSystemPartition( device,uid ) ;
	}
}
