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

#include "includes.h"
#include "../lib/includes.h"
#include <sys/types.h>
#include <string.h>
#include <locale.h>
#include <stdio.h>
#include <libintl.h>
#include <sys/stat.h>
#include <unistd.h>

stringList_t zuluCryptCreateKeyFiles( const char * const * list,int s )
{
	#define buffer_size 32
	char buffer[ buffer_size ] ;
	const char * e ;

	int i ;

	stringList_t stz = StringListVoid ;

	string_t xt ;
	string_t zt ;

	for( i = 0 ; list[ i ] != NULL ; i++ ){

		e = *( list + i ) ;

		zuluCryptSecurityDropElevatedPrivileges() ;
		/*
		 * TrueCrypt only uses the first 1MB of keyfile.
		 */
		if( StringGetFromFile_3( &xt,e,0,1048576 ) == 0 ){

			zuluCryptSecurityGainElevatedPrivileges() ;

			e = StringIntToString_1( buffer,buffer_size,(u_int64_t)i + (u_int64_t)s ) ;
			/*
			 * zuluCryptCreateKeyFile_1() is defined in ../lib/open_tcrypt.c
			 */
			zt = zuluCryptCreateKeyFile_1( xt,e ) ;

			StringDelete( &xt ) ;

			StringListAppendString_1( &stz,&zt ) ;
		}
	}

	zuluCryptSecurityDropElevatedPrivileges() ;
	return stz ;
}

void zuluCryptDeleteKeyFiles( stringList_t stl )
{
	StringListIterator it  ;
	StringListIterator end ;

	StringListGetIterators( stl,&it,&end ) ;

	zuluCryptSecurityGainElevatedPrivileges() ;

	while( it != end ){
		/*
		 * zuluCryptDeleteFile_1() is defined in ../lib/file_path_security.c
		 */
		zuluCryptDeleteFile_1( *it ) ;
		it++ ;
	}

	zuluCryptSecurityDropElevatedPrivileges() ;
}

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

	}else if( StringHasComponent( e,"VCRYPT" ) ){

		printf( gettext( "SUCCESS: %s volume opened successfully\n" ),"vcrypt" ) ;

	}else if( StringHasComponent( e,"BITLK" ) ){

		printf( gettext( "SUCCESS: %s volume opened successfully\n" ),"bitlocker" ) ;
	}else{
		printf( gettext( "SUCCESS: volume opened successfully\n" ) ) ;
	}

	StringFree( e ) ;

	if( m_point != NULL ){

		printf( gettext( "volume mounted at: %s\n" ),m_point ) ;
	}
}

static int zuluExit( int st,const char * device,const char * m_point,stringList_t * stl )
{
	switch( st ){

		case 0 : _printResult( device,m_point ) ;											break ;
		case 1 : printf( gettext( "ERROR: Failed to mount ntfs/exfat file system using ntfs-3g,is ntfs-3g/exfat package installed?\n" ) ) 	;break ;
		case 2 : printf( gettext( "ERROR: There seem to be an open volume accociated with given address\n" ) ) ;			break ;
		case 3 : printf( gettext( "ERROR: No file or device exist on given path\n" ) ) ;						break ;
		case 4 : printf( gettext( "ERROR: Volume could not be opened with the presented key\n" ) ) ;					break ;
		case 5 : printf( gettext( "ERROR: Insufficient privilege to mount the device with given options\n" ) ) ;			break ;
		case 6 : printf( gettext( "ERROR: Insufficient privilege to open device in read write mode or device does not exist\n" ) ) ;	break ;
		case 7 : printf( gettext( "ERROR: Only root user can perform this operation\n" ) ) ;						break ;
		case 8 : printf( gettext( "ERROR: -O and -m options can not be used together\n" ) ) ;						break ;
		case 9 : printf( gettext( "ERROR: Could not create mount point, invalid path or path already taken\n" ) ) ;			break ;
		case 10: printf( gettext( "ERROR: Shared mount point path aleady taken\n" ) ) ;							break ;
		case 11: printf( gettext( "ERROR: There seem to be an opened mapper associated with the device\n" ) ) ;				break ;
		case 12: printf( gettext( "ERROR: Could not get a passphrase from the module\n" ) ) ;						break ;
		case 13: printf( gettext( "ERROR: Could not get passphrase in silent mode\n" ) ); 						break ;
		case 14: printf( gettext( "ERROR: Insufficient memory to hold passphrase\n" ) ) ;						break ;
		case 15: printf( gettext( "ERROR: One or more required argument(s) for this operation is missing\n" ) ) ;			break ;
		case 16: printf( gettext( "ERROR: Invalid path to key file\n" ) ) ;								break ;
		case 17: printf( gettext( "ERROR: Could not get enought memory to hold the key file\n" ) ) ;					break ;
		case 18: printf( gettext( "ERROR: Insufficient privilege to open key file for reading\n" ) );					break ;
		case 19: printf( gettext( "ERROR: Could not get a passphrase through a local socket\n" ) );					break ;
		case 20: printf( gettext( "ERROR: Failed to mount a filesystem:invalid/unsupported mount option or unsupported file system encountered\n" ) );break ;
		case 21: printf( gettext( "ERROR: Could not create a lock on /etc/mtab\n" ) ) ;							break ;
		case 22: printf( gettext( "ERROR: Insufficient privilege to open a system volume\n" ) ) ;					break ;
		default: printf( gettext( "ERROR: Unrecognized error with status number %d encountered\n" ),st ) ;
	}

	zuluCryptSecurityUnlockMemory( *stl ) ;
	StringListClearDelete( stl ) ;

	return st ;
}

/*
 * open_volume function below can be devided into two, the first part is before the mount point folder is created and the
 * other part is after. This function is called after the mount point is created to see if it the mount point folder
 * should be removed first before calling the above function.The above function is called directly when "open_volume"
 * function is to be exited before the mount point is created. *
 */
static int zuluExit_1( int st,const struct_opts * opts,const char * device,const char * m_point,stringList_t * stl )
{
	if( opts->open_mount && st != 0 ){

		zuluCryptSecurityGainElevatedPrivileges() ;

		rmdir( m_point ) ;

		zuluCryptSecurityDropElevatedPrivileges() ;
	}
	return zuluExit( st,device,m_point,stl ) ;
}

static int _open_volume( const open_struct_t * volume )
{
	int r ;
	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptOpenVolume_2() is defined in ../lib/open_volume.c
	 */
	r = zuluCryptOpenVolume_2( volume ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	return r ;
}

void zuluCryptTrueCryptVeraCryptVolumeInfo( const char * type,tvcrypt * e )
{
	stringList_t stl = StringListSplit( type,'.' ) ;

	size_t p = StringListSize( stl ) ;

	const char * q ;

	memset( e,'\0',sizeof( tvcrypt ) ) ;

	if( p > 0 ){

		e->type = StringListCopyStringAtFirstPlace( stl ) ;

		if( p > 1 ){

			q = StringListContentAtSecondPlace( stl ) ;

			e->iteration_count = ( int )StringConvertToInt( q ) ;
		}
	}

	StringListDelete( &stl ) ;
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
	const char * const * tcrypt_keyfiles = opts->tcrypt_multiple_keyfiles ;
	/*
	 * Below is a form of memory management.All strings are collected in a stringlist object to easily delete them
	 * when the function returns.This allows for the function to have multiple exit points without risks of leaking
	 * memory from manually examining each exit point to make sure all strings are deleted or go with multiple goto
	 * code deleting blocks to take into account different exit points.
	 */
	stringList_t stl = StringListVoid ;
	string_t * stringArray = StringListArray( &stl,8 ) ;
	string_t * passphrase  =  &stringArray[ 0 ] ;
	string_t * m_name      =  &stringArray[ 1 ] ;
	string_t * data        =  &stringArray[ 2 ] ;
	string_t * m_point     =  &stringArray[ 3 ] ;
	string_t * mapper      =  &stringArray[ 4 ] ;
	string_t * mapper_path =  &stringArray[ 5 ] ;
	string_t * offset      =  &stringArray[ 6 ] ;
	string_t * bitlk       =  &stringArray[ 7 ] ;

	const char * key = NULL ;
	const char * mapper_name ;
	const char * path_mapper ;

	size_t key_len = 0 ;
	int st = 0 ;

	int bitlockerVolume ;

	stringList_t stz ;
	tvcrypt v_info ;

	unsigned long m_flags ;

	int tcrypt_keyfile  = 0 ;

	const char * uuid ;
	char * device_path ;

	/*
	 * open_struct_t is declared in ../lib/include.h
	 */
	open_struct_t volume ;

	struct stat statstr ;
#if 0
	/*
	 * zuluCryptVolumeIsInSystemVolumeList() is defined in volumes.c
	 */
	if( zuluCryptVolumeIsInSystemVolumeList( device ) ){
		/*
		 * check permissions only if volume is explicity mentioned as system.
		 * This is an exception to avoid some udev bad behaviors on udev enabled build
		 */
		if( !zuluCryptUserIsAMemberOfAGroup( uid,"zulucrypt" ) ){
			return zuluExit( 22,device,mount_point,stl ) ;
		}
	}
#endif
	if( m_opts == NULL ){

		m_opts = "rw" ;
	}
	/*
	 * zuluCryptMountFlagsAreNotCorrect() is defined in ./mount_flags.c
	 */

	zuluCryptMountFlagsAreNotCorrect( m_opts,uid,&m_flags ) ;

	if( zuluCryptMountFlagsAreNotCorrect( fs_opts,uid,&m_flags ) ){

		return zuluExit( 5,device,mount_point,&stl ) ;
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
		case 1 :  return zuluExit( 6,device,mount_point,&stl ) ;
		case 2 :  return zuluExit( 6,device,mount_point,&stl ) ;
		case 3 :  return zuluExit( 6,device,mount_point,&stl ) ;
		case 4 :  return zuluExit( 6,device,mount_point,&stl ) ;
		default:  return zuluExit( 6,device,mount_point,&stl ) ;
	}

	if( open_mount ){
		/*
		* zuluCryptCreateMountPoint() is defined in create_mount_point.c
		*/
		*m_point = zuluCryptCreateMountPoint( device,mount_point,m_opts,uid ) ;

		mount_point = StringContent( *m_point ) ;

		if( mount_point == NULL ){

			return zuluExit( 9,device,mount_point,&stl ) ;
		}
	}else{
		if( uid != 0 ){

			return zuluExit( 7,device,mount_point,&stl ) ;
		}
		if( mount_point != NULL ){

			return zuluExit( 8,device,mount_point,&stl ) ;
		}
	}

	if( share ){
		/*
		 * zuluCryptBindSharedMountPointPathTaken() is defined in bind.c
		 */
		if( zuluCryptBindSharedMountPointPathTaken( *m_point ) ){

			return zuluExit_1( 10,opts,device,mount_point,&stl ) ;
		}
	}

	zuluCryptSecurityGainElevatedPrivileges() ;

	bitlockerVolume = zuluCryptDeviceHasAgivenFileSystem( device,zuluCryptBitLockerType() ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	if( bitlockerVolume && zuluCryptUseDislockerBitLocker( opts->use_cryptsetup_for_bitlocker ) ){

		*mapper_path = zuluCryptBitLockerMapperPath( uid ) ;

		zuluCryptSecurityGainElevatedPrivileges() ;

		*m_name = zuluCryptBitLockerMapperName( device ) ;

		mapper_name = StringContent( *m_name ) ;
		path_mapper = StringContent( *mapper_path ) ;

		zuluCryptCreateMountPointPrefix( uid ) ;

		zuluCryptSecurityDropElevatedPrivileges() ;
	}else{
		/*
		 * zuluCryptCreateMapperName() is defined in ../lib/create_mapper_name.c
		 */
		*m_name = zuluCryptCreateMapperName( device,mapping_name,uid,ZULUCRYPTshortMapperPath ) ;

		*mapper = StringCopy( *m_name ) ;
		mapper_name = StringContent( *m_name ) ;

		*mapper_path = String( zuluCryptMapperPrefix() ) ;

		path_mapper = StringMultipleAppend( *mapper_path,"/",mapper_name,NULL ) ;

		if( stat( path_mapper,&statstr ) == 0 ){

			return zuluExit_1( 11,opts,device,mount_point,&stl ) ;
		}
	}

	if( plugin_path != NULL ){

		/*
		 * zuluCryptPrepareSocketPath() is defined in path_access.c
		 */
		zuluCryptPrepareSocketPath( uid ) ;

		/*
		 * zuluCryptUUIDFromPath() is defined in path_access.c
		 */
		uuid = zuluCryptUUIDFromPath( device ) ;

		device_path = _device_path( device ) ;
		/*
		 * zuluCryptPluginManagerGetKeyFromModule is defined in ../pluginManager/zuluCryptPluginManager.c
		 */
		*passphrase = zuluCryptPluginManagerGetKeyFromModule( device_path,
								      plugin_path,
								      uuid,
								      uid,
								      opts,
								      zuluCryptRunTimePath(),
								      &st ) ;

		StringFree( device_path ) ;
		StringFree( uuid ) ;

		if( st != 0 || *passphrase == StringVoid ){

			return zuluExit_1( 12,opts,device,mount_point,&stl ) ;
		}

		key_len = StringLength( *passphrase ) ;
		key = StringContent( *passphrase ) ;

		zuluCryptSecurityLockMemory_1( *passphrase ) ;

	}else if( source == NULL && tcrypt_keyfiles[ 0 ] == NULL ){

		printf( gettext( "Enter passphrase: " ) ) ;
		/*
		 * ZULUCRYPT_KEY_MAX_SIZE is set in ../constants.h
		 */
		switch( StringSilentlyGetFromTerminal_1( passphrase,ZULUCRYPT_KEY_MAX_SIZE ) ){

			case 1 : return zuluExit_1( 13,opts,device,mount_point,&stl ) ;
			case 2 : return zuluExit_1( 14,opts,device,mount_point,&stl ) ;
		}

		printf( "\n" ) ;
		key = StringContent( *passphrase ) ;
		key_len = StringLength( *passphrase ) ;

		zuluCryptSecurityLockMemory_1( *passphrase ) ;
	}else{
		if( source == NULL || pass == NULL ){

			if( tcrypt_keyfiles == NULL ){

				return zuluExit_1( 15,opts,device,mount_point,&stl ) ;
			}
		}
		if( StringsAreEqual( source,"-p" ) ){

			key = pass ;
			key_len = StringSize( pass ) ;

		}else if( StringsAreEqual( source,"-f" ) ){

			if( StringHasNoComponent( pass,zuluCryptRunTimePath() ) ){

				tcrypt_keyfile = 1 ;
			}
			/*
			 * function is defined at "path_access.c"
			 */
			switch( zuluCryptGetPassFromFile( NULL,pass,uid,data ) ){

				case 1 : return zuluExit_1( 16,opts,device,mount_point,&stl ) ;
				case 2 : return zuluExit_1( 17,opts,device,mount_point,&stl ) ;
				case 4 : return zuluExit_1( 18,opts,device,mount_point,&stl ) ;
				case 5 : return zuluExit_1( 19,opts,device,mount_point,&stl ) ;
			}

			if( *data == StringVoid ){

				return zuluExit_1( 12,opts,device,mount_point,&stl ) ;
			}

			key = StringContent( *data ) ;
			key_len = StringLength( *data ) ;

			zuluCryptSecurityLockMemory_1( *data ) ;
		}
	}

	memset( &volume,'\0',sizeof( open_struct_t ) ) ;

	if( key != NULL ){

		volume.key     = key ;
		volume.key_len = key_len ;
	}else{
		volume.key     = "" ;
		volume.key_len = 0 ;
	}

	volume.device      = device ;
	volume.mapper_name = mapper_name ;
	volume.mapper_path = path_mapper ;
	volume.m_point     = mount_point ;
	volume.fs_opts     = fs_opts ;
	volume.uid         = uid ;
	volume.m_opts      = m_opts ;
	volume.m_flags     = m_flags ;
	volume.bitlocker_volume = bitlockerVolume ;
	volume.luks_detached_header = opts->luks_external_header ;
	volume.use_cryptsetup_for_bitlocker = opts->use_cryptsetup_for_bitlocker ;

	/*
	 * zuluCryptTrueCryptVeraCryptVolumeInfo() is defined in this source file.
	 */
	zuluCryptTrueCryptVeraCryptVolumeInfo( opts->type,&v_info ) ;

	volume.iteration_count = v_info.iteration_count ;

	if( StringAtLeastOneMatch( v_info.type,"tcrypt","truecrypt",NULL ) ){

		volume.trueCrypt_volume = 1 ;

	}else if( StringAtLeastOneMatch( v_info.type,"vcrypt","veracrypt","vera",NULL ) ){

		volume.veraCrypt_volume = 1 ;

	}else if( StringAtLeastOneMatch( v_info.type,"vcrypt-sys","veracrypt-sys","vera-sys",NULL ) ){

		volume.veraCrypt_volume = 1 ;
		volume.system_volume = 1 ;

	}else if( StringAtLeastOneMatch( v_info.type,"vcrypt-hbk","veracrypt-hbk","vera-hbk",NULL ) ){

		volume.veraCrypt_volume = 1 ;
		volume.use_backup_header = 1 ;

	}else if( StringAtLeastOneMatch( v_info.type,"vcrypt-sys-hbk","veracrypt-sys-hbk","vera-sys-hbk",NULL ) ){

		volume.veraCrypt_volume = 1 ;
		volume.system_volume = 1 ;
		volume.use_backup_header = 1 ;
	}

	if( !volume.veraCrypt_volume && !volume.trueCrypt_volume ){

		if( opts->offset != NULL ){

			*offset = String( "/dev/urandom.aes.cbc-essiv:sha256.256.ripemd160." ) ;
			volume.plain_dm_properties = StringAppend( *offset,opts->offset ) ;
		}else{
			if( StringsAreEqual( opts->type,"plain" ) ){

				volume.plain_dm_properties = "aes.cbc-essiv:sha256.256.sha256.0" ;

			}else if( StringsAreNotEqual( opts->type,"luks" ) ){

				volume.plain_dm_properties = opts->type ;
			}
		}
	}

	StringDelete( &v_info.type ) ;

	if( plugin_path != NULL ){

		plugin_path = plugin_path + StringLastIndexOfChar_1( plugin_path,'/' ) + 1 ;
	}

	if( tcrypt_keyfile ){

		volume.key_source = TCRYPT_KEYFILE ;
	}

	if( tcrypt_keyfiles[ 0 ] != NULL ){

		/*
		 * Here, we take a list of keyfiles supplied by the user and then copy them to a safe
		 * location at "/run/zuluCrypt" and then we pass these safe copies to cryptsetup.
		 *
		 * The idea is not to let cryptsetup, a privileged process handle user managed files.
		 */

		stz = zuluCryptCreateKeyFiles( tcrypt_keyfiles,0 ) ;

		volume.tcrypt_keyfiles_count = StringListSize( stz ) ;
		volume.tcrypt_keyfiles       = StringListStringArray( stz ) ;

		st = _open_volume( &volume ) ;

		zuluCryptDeleteKeyFiles( stz ) ;

		StringFree( volume.tcrypt_keyfiles ) ;
		StringListDelete( &stz ) ;
	}else{
		st = _open_volume( &volume ) ;
	}
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

	if( volume.bitlocker_volume ){

		if( zuluCryptUseCryptsetupBitLocker( opts->use_cryptsetup_for_bitlocker ) ){

			device = StringMultiplePrepend( *mapper,"/",zuluCryptMapperPrefix(),NULL ) ;
		}else{
			*bitlk = zuluCryptBitLockerFullMapperPath( uid,device ) ;

			device = StringContent( *bitlk ) ;
		}
	}else{
		device = StringMultiplePrepend( *mapper,"/",zuluCryptMapperPrefix(),NULL ) ;
	}

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
	return zuluExit_1( st,opts,device,mount_point,&stl ) ;
}
