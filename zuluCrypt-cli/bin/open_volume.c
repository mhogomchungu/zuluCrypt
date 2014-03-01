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
#include <sys/types.h>
#include <string.h>
#include <locale.h>
#include <stdio.h>
#include <libintl.h>
#include <sys/stat.h>
#include <unistd.h>

static int _open_volume( const open_struct_t * ) ;

static char * _device_path( const char * device )
{
	char * path ;

	if( StringPrefixEqual( device,"/dev/loop" ) ){
		zuluCryptSecurityGainElevatedPrivileges() ;
		/*
		 * zuluCryptLoopDeviceAddress_1() is defined in ../zuluCrypt-cli/create_loop_device.c
		 */
		path = zuluCryptLoopDeviceAddress_1( device ) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
		if( path == NULL ){
			return StringCopy_2( device ) ;
		}else{
			return path ;
		}
	}else{
		return StringCopy_2( device ) ;
	}
}

static void _printResult( const char * device,const char * m_point )
{
	char * e ;

	zuluCryptSecurityGainElevatedPrivileges() ;

	/*
	 * zuluCryptGetVolumeTypeFromMapperPath() is defined in ../lib/status.c
	 */
	e = zuluCryptGetVolumeTypeFromMapperPath( device ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	if( StringHasComponent( e,"LUKS" ) ){
		printf( gettext( "SUCCESS: %s volume opened successfully\n" ),"luks" ) ;
	}else if( StringHasComponent( e,"PLAIN" ) ){
		printf( gettext( "SUCCESS: %s volume opened successfully\n" ),"plain" ) ;
	}else if( StringHasComponent( e,"TCRYPT" ) ){
		printf( gettext( "SUCCESS: %s volume opened successfully\n" ),"tcrypt" ) ;
	}else{
		printf( gettext( "SUCCESS: volume opened successfully\n" ) ) ;
	}

	free( e ) ;
	if( m_point != NULL ){
		printf( gettext( "volume mounted at: %s\n" ),m_point ) ;
	}
}

static int zuluExit( int st,const char * device,const char * m_point,stringList_t stl )
{
	switch( st ){
		case 0 : _printResult( device,m_point ) ;											break ;
		case 1 : printf( gettext( "ERROR: failed to mount ntfs/exfat file system using ntfs-3g,is ntfs-3g/exfat package installed?\n" ) ) 	;break ;
		case 2 : printf( gettext( "ERROR: there seem to be an open volume accociated with given address\n" ) ) ;			break ;
		case 3 : printf( gettext( "ERROR: no file or device exist on given path\n" ) ) ;						break ;
		case 4 : printf( gettext( "ERROR: volume could not be opened with the presented key\n" ) ) ;					break ;
		case 5 : printf( gettext( "ERROR: insufficient privilege to mount the device with given options\n" ) ) ;			break ;
		case 6 : printf( gettext( "ERROR: insufficient privilege to open device in read write mode or device does not exist\n" ) ) ;	break ;
		case 7 : printf( gettext( "ERROR: only root user can perform this operation\n" ) ) ;						break ;
		case 8 : printf( gettext( "ERROR: -O and -m options can not be used together\n" ) ) ;						break ;
		case 9 : printf( gettext( "ERROR: could not create mount point, invalid path or path already taken\n" ) ) ;			break ;
		case 10: printf( gettext( "ERROR: shared mount point path aleady taken\n" ) ) ;							break ;
		case 11: printf( gettext( "ERROR: there seem to be an opened mapper associated with the device\n" ) ) ;				break ;
		case 12: printf( gettext( "ERROR: could not get a passphrase from the module\n" ) ) ;						break ;
		case 13: printf( gettext( "ERROR: could not get passphrase in sikey_lent mode\n" ) ); 						break ;
		case 14: printf( gettext( "ERROR: insufficient memory to hold passphrase\n" ) ) ;						break ;
		case 15: printf( gettext( "ERROR: one or more required argument(s) for this operation is missing\n" ) ) ;			break ;
		case 16: printf( gettext( "ERROR: invalid path to key file\n" ) ) ;								break ;
		case 17: printf( gettext( "ERROR: could not get enought memory to hold the key file\n" ) ) ;					break ;
		case 18: printf( gettext( "ERROR: insufficient privilege to open key file for reading\n" ) );					break ;
		case 19: printf( gettext( "ERROR: could not get a passphrase through a local socket\n" ) );					break ;
		case 20: printf( gettext( "ERROR: failed to mount a filesystem:invalid/unsupported mount option or unsupported file system encountered\n" ) );break ;
		case 21: printf( gettext( "ERROR: could not create a lock on /etc/mtab\n" ) ) ;							break ;
		case 22: printf( gettext( "ERROR: insufficient privilege to open a system volume\n" ) ) ;					break ;
		default: printf( gettext( "ERROR: unrecognized error with status number %d encountered\n" ),st ) ;
	}

	zuluCryptSecurityUnlockMemory( stl ) ;
	StringListClearDelete( &stl ) ;

	return st ;
}

/*
 * open_volume function below can be devided into two, the first part is before the mount point folder is created and the
 * other part is after. This function is called after the mount point is created to see if it the mount point folder
 * should be removed first before calling the above function.The above function is called directly when "open_volume"
 * function is to be exited before the mount point is created. *
 */
static int zuluExit_1( int st,const struct_opts * opts,const char * device,const char * m_point,stringList_t stl )
{
	if( opts->open_mount && st != 0 ){
		zuluCryptSecurityGainElevatedPrivileges() ;
		rmdir( m_point ) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
	}
	return zuluExit( st,device,m_point,stl ) ;
}

int zuluCryptEXEOpenVolume( const struct_opts * opts,const char * mapping_name,uid_t uid )
{
	int share                = opts->share ;
	int open_mount           = opts->open_mount ;
	const char * device      = opts->device ;
	const char * mount_point = opts->mount_point ;
	const char * m_opts      = opts->m_opts ;
	const char * source      = opts->key_source ;
	const char * pass        = opts->key ;
	const char * plugin_path = opts->plugin_path ;
	const char * fs_opts     = opts->fs_opts ;
	const char * offset      = opts->offset ;
	/*
	 * Below is a form of memory management.All strings are collected in a stringlist object to easily delete them
	 * when the function returns.This allows for the function to have multiple exit points without risks of leaking
	 * memory from manually examining each exit point to make sure all strings are deleted or go with multiple goto
	 * code deleting blocks to take into account different exit points.
	 */
	stringList_t stl ;
	string_t * stringArray = StringListArray( &stl,6 ) ;
	string_t * passphrase  =  &stringArray[ 0 ] ;
	string_t * m_name      =  &stringArray[ 1 ] ;
	string_t * data        =  &stringArray[ 2 ] ;
	string_t * m_point     =  &stringArray[ 3 ] ;
	string_t * mapper      =  &stringArray[ 4 ] ;
	string_t * mapper_path =  &stringArray[ 5 ] ;

	const char * key = NULL ;
	const char * mapper_name ;
	const char * e ;

	size_t key_len = 0 ;
	int st = 0 ;

	unsigned long m_flags ;

	int key_source = TCRYPT_PASSPHRASE ;

	const char * uuid ;
	char * device_path ;

	/*
	 * open_struct_t is declared in ../lib/include.h
	 */
	open_struct_t open_struct ;

	struct stat statstr ;

	/*
	 * zuluCryptPartitionIsSystemPartition() is defined in volumes.c
	 */
	if( zuluCryptPartitionIsSystemPartition( device,uid ) ){
		if( !zuluCryptUserIsAMemberOfAGroup( uid,"zulucrypt" ) ){
			return zuluExit( 22,device,mount_point,stl ) ; ;
		}
	}
	if( m_opts == NULL ){
		m_opts = "rw" ;
	}
	/*
	 * zuluCryptMountFlagsAreNotCorrect() is defined in ./mount_flags.c
	 */
	if( zuluCryptMountFlagsAreNotCorrect( m_opts,uid,&m_flags ) ){
		return zuluExit( 5,device,mount_point,stl ) ;
	}
	if( StringHasComponent( m_opts,"rw" ) ){
		/*
		 * zuluCryptSecurityDeviceIsWritable() is defined in path_access.c
		 */
		st = zuluCryptCanOpenPathForWriting( device,uid ) ;
	}else{
		/*
		 * zuluCryptSecurityDeviceIsReadable() is defined in path_access.c
		 */
		st = zuluCryptCanOpenPathForReading( device,uid ) ;
	}

	/*
	 * 1-permissions denied
	 * 2-invalid path
	 * 3-shenanigans
	 * 4-common error
	 */
	switch( st ){
		case 0 :  break ;
		case 1 :  return zuluExit( 6,device,mount_point,stl ) ;
		case 2 :  return zuluExit( 6,device,mount_point,stl ) ;
		case 3 :  return zuluExit( 6,device,mount_point,stl ) ;
		case 4 :  return zuluExit( 6,device,mount_point,stl ) ;
		default:  return zuluExit( 6,device,mount_point,stl ) ;
	}

	if( open_mount ){
		/*
		* zuluCryptCreateMountPoint() is defined in create_mount_point.c
		*/
		*m_point = zuluCryptCreateMountPoint( device,mount_point,m_opts,uid ) ;
		mount_point = StringContent( *m_point ) ;
		if( mount_point == NULL ){
			return zuluExit( 9,device,mount_point,stl ) ;
		}
	}else{
		if( uid != 0 ){
			return zuluExit( 7,device,mount_point,stl ) ;
		}
		if( mount_point != NULL ){
			return zuluExit( 8,device,mount_point,stl ) ;
		}
	}

	if( share ){
		/*
		 * zuluCryptBindSharedMountPointPathTaken() is defined in bind.c
		 */
		if( zuluCryptBindSharedMountPointPathTaken( *m_point ) ){
			return zuluExit_1( 10,opts,device,mount_point,stl ) ;
		}
	}

	/*
	 * ZULUCRYPTshortMapperPath is set in ../constants.h
	 * zuluCryptCreateMapperName() is defined at ../lib/create_mapper_name.c
	 */
	*m_name = zuluCryptCreateMapperName( device,mapping_name,uid,ZULUCRYPTshortMapperPath ) ;

	*mapper = StringCopy( *m_name ) ;
	mapper_name = StringContent( *m_name ) ;

	*mapper_path = String( zuluCryptMapperPrefix() ) ;
	e = StringMultipleAppend( *mapper_path,"/",mapper_name,END ) ;

	if( stat( e,&statstr ) == 0 ){
		return zuluExit_1( 11,opts,device,mount_point,stl ) ;
	}

	if( plugin_path != NULL ){
		/*
		 * zuluCryptUUIDFromPath() is defined in path_access.c
		 */
		uuid = zuluCryptUUIDFromPath( device ) ;

		device_path = _device_path( device ) ;
		/*
		 * zuluCryptPluginManagerGetKeyFromModule is defined in ../pluginManager/zuluCryptPluginManager.c
		 */
		*passphrase = zuluCryptPluginManagerGetKeyFromModule( device_path,plugin_path,uuid,uid,opts ) ;

		StringFree( device_path ) ;
		StringFree( uuid ) ;

		if( *passphrase == StringVoid ){
			return zuluExit_1( 12,opts,device,mount_point,stl ) ;
		}
		key_len = StringLength( *passphrase ) ;
		if( key_len == 0 ){
			/*
			 * TODO
			 * A plugin just gave us an empty password,currently,we cant tell if its because
			 * a user of the plugin sent an empty password or the plugin was cancelled and a user
			 * sent no password.Look into trying to tell the two apart.
			 */
			;
		}
		key = StringContent( *passphrase ) ;
		zuluCryptSecurityLockMemory_1( *passphrase ) ;
	}else if( source == NULL ){
		printf( gettext( "Enter passphrase: " ) ) ;
		/*
		 * ZULUCRYPT_KEY_MAX_SIZE is set in ../constants.h
		 */
		switch( StringSilentlyGetFromTerminal_1( passphrase,ZULUCRYPT_KEY_MAX_SIZE ) ){
			case 1 : return zuluExit_1( 13,opts,device,mount_point,stl ) ;
			case 2 : return zuluExit_1( 14,opts,device,mount_point,stl ) ;
		}
		printf( "\n" ) ;
		key = StringContent( *passphrase ) ;
		key_len = StringLength( *passphrase ) ;
		zuluCryptSecurityLockMemory_1( *passphrase ) ;
	}else{
		if( source == NULL || pass == NULL ){
			return zuluExit_1( 15,opts,device,mount_point,stl ) ;
		}
		if( StringsAreEqual( source,"-p" ) ){
			key = pass ;
			key_len = StringSize( pass ) ;
		}else if( StringsAreEqual( source,"-f" ) ){
			if( StringHasNoComponent( pass,"/.zuluCrypt-socket" ) ){
				key_source = TCRYPT_KEYFILE ;
			}
			/*
			 * function is defined at "path_access.c"
			 */
			switch( zuluCryptGetPassFromFile( pass,uid,data ) ){
				case 1 : return zuluExit_1( 16,opts,device,mount_point,stl ) ;
				case 2 : return zuluExit_1( 17,opts,device,mount_point,stl ) ;
				case 4 : return zuluExit_1( 18,opts,device,mount_point,stl ) ;
				case 5 : return zuluExit_1( 19,opts,device,mount_point,stl ) ;
			}
			key = StringContent( *data ) ;
			key_len = StringLength( *data ) ;
			zuluCryptSecurityLockMemory_1( *data ) ;
		}
	}

	memset( &open_struct,'\0',sizeof( open_struct_t ) ) ;

	open_struct.device      = device ;
	open_struct.offset      = offset ;
	open_struct.mapper_name = mapper_name ;
	open_struct.key         = key ;
	open_struct.key_len     = key_len ;
	open_struct.key_source  = key_source ;
	open_struct.m_point     = mount_point ;
	open_struct.fs_opts     = fs_opts ;
	open_struct.key_len     = key_len ;
	open_struct.uid         = uid ;
	open_struct.m_opts      = m_opts ;
	open_struct.m_flags     = m_flags ;

	zuluCryptSecurityGainElevatedPrivileges() ;

	st = _open_volume( &open_struct ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	/*
	 * below two return values comes from ../lib/mount_volume.c
	 */
	if( st == -1 ){
		st = 20 ;
	}
	if( st == 12 ){
		st = 21 ;
	}
	if( st == 8 || st == 3 ){
		st = 3 ;
	}

	device = StringMultiplePrepend( *mapper,"/",zuluCryptMapperPrefix(),END ) ;

	if( st == 0 && share ){
		/*
		 * user wish to share the mount point bind the mount point to a publicly accessed path at /run/media/public/
		 */
		/*
		 * zuluCryptBindMountVolume() is defined in ../zuluCrypt-cli/bin/bind.c
		 */
		zuluCryptBindMountVolume( device,*m_point,m_flags ) ;
	}

	/*
	 * zuluCryptCheckInvalidKey() is defined in check_invalid_key.c
	 */
	zuluCryptCheckInvalidKey( opts->device ) ;
	return zuluExit_1( st,opts,device,mount_point,stl ) ;
}

static int _open_volume( const open_struct_t * opts )
{
	int st ;
	if( opts->offset == NULL ){
		/*
		 * zuluCryptOpenVolume() is defined in ../lib/open_volume.c
		 */
		st = zuluCryptOpenVolume_1( opts ) ;

		if( st == 4 ){
			/*
			 * failed to open a LUKS or PLAIN volume.
			 */
			/*
			 * zuluCryptVolumeIsNotLuks() is defined in ../lib/is_luks.c
			 */
			if( zuluCryptVolumeIsNotLuks( opts->device ) ){
				/*
				 * The volume is not LUKS,its either PLAIN or TRUECRYPT,we already failed to open it as PLAIN
				 * so it must be TRUECRYPT or the key is wrong.
				 */

				/*
				 * try to open is a normal TRUECRYPT volume.
				 */
				st = zuluCryptOpenTcrypt_1( opts ) ;

				if( st == 15 || st == 0 || st == -1 ){
					;
				}else{
					st = 4 ;
				}
			}
		}
	}else{
		/*
		 * zuluCryptOpenPlainWithOffset() is defined in ../lib/open_volume.c
		 */
		st = zuluCryptOpenPlainWithOffset( opts ) ;
	}
	return st ;
}
