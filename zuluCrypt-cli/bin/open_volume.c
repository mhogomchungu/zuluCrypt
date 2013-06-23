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

static int _open_tcrypt( const char * device,const char * mapper_name,const char * key,
			 size_t key_len,const char * key_source,const char * key_origin,
			 int volume_type,const char * m_point,uid_t uid,
			 unsigned long m_flags,const char * fs_opts ) ;

static void _printResult( const char * device,const char * m_point,uid_t uid,const char * mapping_name )
{
	char * e ;
	
	if( uid ){;}
	if( mapping_name ){;}
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	/*
	 * zuluCryptGetVolumeTypeFromMapperPath() is defined in ../lib/status.c
	 */
	e = zuluCryptGetVolumeTypeFromMapperPath( device ) ;
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	if( StringHasComponent( e,"LUKS" ) ){
		printf( "SUCCESS: luks volume opened successfully\n" ) ;
	}else if( StringHasComponent( e,"PLAIN" ) ){
		printf( "SUCCESS: plain volume opened successfully\n" ) ;
	}else if( StringHasComponent( e,"TCRYPT" ) ){
		printf( "SUCCESS: tcrypt volume opened successfully\n" ) ;
	}else{
		printf( "SUCCESS: volume opened successfully\n" ) ;
	}
	
	free( e ) ;
	if( m_point != NULL ){
		printf( "volume mounted at: %s\n",m_point ) ;
	}
}

static int zuluExit( int st,const char * device,const char * m_point,stringList_t stl,uid_t uid,const char * mapping_name )
{
	switch( st ){
		case 0 : _printResult( device,m_point,uid,mapping_name ) ;								break ;
		case 1 : printf( "ERROR: failed to mount ntfs file system using ntfs-3g,is ntfs-3g package installed?\n" ) ;		break ;
		case 2 : printf( "ERROR: there seem to be an open volume accociated with given address\n" );				break ;
		case 3 : printf( "ERROR: no file or device exist on given path\n" ) ; 							break ;
		case 4 : printf( "ERROR: volume could not be opened with the presented key\n" );					break ;
		case 5 : printf( "ERROR: insufficient privilege to mount the device with given options\n" ) ;				break ;
		case 6 : printf( "ERROR: insufficient privilege to open device\n" );							break ;
		case 7 : printf( "ERROR: only root user can perform this operation\n" ) ;						break ;
		case 8 : printf( "ERROR: -O and -m options can not be used together\n" );						break ;
		case 9 : printf( "ERROR: could not create mount point, invalid path or path already taken\n" ) ;			break ;
		case 10: printf( "ERROR: shared mount point path aleady taken\n" ) ;							break ;
		case 11: printf( "ERROR: there seem to be an opened mapper associated with the device\n" ) ;				break ;
		case 12: printf( "ERROR: could not get a passphrase from the module\n" ) ;						break ;
		case 13: printf( "ERROR: could not get passphrase in silent mode\n" );							break ;
		case 14: printf( "ERROR: insufficient memory to hold passphrase\n" );							break ;
		case 15: printf( "ERROR: one or more required argument(s) for this operation is missing\n" );				break ;
		case 16: printf( "ERROR: invalid path to key file\n" );									break ;
		case 17: printf( "ERROR: could not get enought memory to hold the key file\n" );					break ;
		case 18: printf( "ERROR: insufficient privilege to open key file for reading\n" );					break ;
		case 19: printf( "ERROR: could not get a passphrase through a local socket\n" );					break ;
		case 20: printf( "ERROR: failed to mount a filesystem:invalid/unsupported mount option or unsupported file system encountered\n" ) ;break ;
		case 21: printf( "ERROR: could not create a lock on /etc/mtab\n" ) ;							break ;
		default: printf( "ERROR: unrecognized error with status number %d encountered\n",st );
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
static int zuluExit_1( int st,const struct_opts * opts,const char * device,const char * cpoint,stringList_t stl,uid_t uid,const char * mapping_name )
{
	if( opts->open_no_mount == -1 && st != 0 ){
		zuluCryptSecurityGainElevatedPrivileges() ;
		rmdir( cpoint ) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
	}
	return zuluExit( st,device,cpoint,stl,uid,mapping_name ) ;
}

int zuluCryptEXEOpenVolume( const struct_opts * opts,const char * mapping_name,uid_t uid )
{
	int share                = opts->share ;
	int nmp                  = opts->open_no_mount ;
	const char * device      = opts->device ;
	const char * mount_point = opts->mount_point ;
	const char * m_opts      = opts->m_opts ;
	const char * source      = opts->key_source ;
	const char * pass        = opts->key ;
	const char * plugin_path = opts->plugin_path ;
	const char * fs_opts     = opts->fs_opts ;
	
	/*
	 * Below is a form of memory management.All strings are collected in a stringlist object to easily delete them
	 * when the function returns.This allows for the function to have multiple exit points without risks of leaking
	 * memory from manually examining each exit point to make sure all strings are deleted or go with multiple goto
	 * code deleting blocks to take into account different exit points. 
	 */
	stringList_t stl ;
	string_t * stringArray = StringListArray( &stl,6 ) ;
	string_t * passphrase =  &stringArray[ 0 ];
	string_t * m_name     =  &stringArray[ 1 ];
	string_t * data       =  &stringArray[ 2 ]; 
	string_t * m_point    =  &stringArray[ 3 ];
	string_t * mapper     =  &stringArray[ 4 ];
	string_t * mapper_path=  &stringArray[ 5 ];
	
	const char * cpass = NULL;
	const char * mapper_name ;
	const char * e ;
	const char * cpoint = NULL ;
	
	size_t len = 0 ;
	int st = 0 ;

	unsigned long m_flags ;
	
	const char * uuid ;
	
	struct stat statstr ;
	
	/*
	 * zuluCryptMountFlagsAreNotCorrect() is defined in ./mount_flags.c
	 */
	if( zuluCryptMountFlagsAreNotCorrect( m_opts,uid,&m_flags ) ){
		return zuluExit( 5,device,cpoint,stl,uid,mapping_name ) ; 
	}
	if( StringHasComponent( m_opts,"rw" ) ){
		/*
		 * zuluCryptSecurityDeviceIsWritable() is defined in security.c
		 */
		st = zuluCryptSecurityDeviceIsWritable( device,uid ) ;
	}else{
		/*
		 * zuluCryptSecurityDeviceIsReadable() is defined in security.c
		 */
		st = zuluCryptSecurityDeviceIsReadable( device,uid ) ;
	}
	
	/*
	 * 1-permissions denied
	 * 2-invalid path
	 * 3-shenanigans
	 * 4-common error 
	 */
	switch( st ){
		case 0 :  break ;
		case 1 :  return zuluExit( 6,device,cpoint,stl,uid,mapping_name ) ;
		case 2 :  return zuluExit( 6,device,cpoint,stl,uid,mapping_name ) ;
		case 3 :  return zuluExit( 6,device,cpoint,stl,uid,mapping_name ) ;
		case 4 :  return zuluExit( 6,device,cpoint,stl,uid,mapping_name ) ;
		default:  return zuluExit( 6,device,cpoint,stl,uid,mapping_name ) ;
	}
	
	if( nmp == 1 ){
		if( uid != 0 ){
			return zuluExit( 7,device,cpoint,stl,uid,mapping_name ) ;
		}
		if( mount_point != NULL ){
			return zuluExit( 8,device,cpoint,stl,uid,mapping_name ) ;
		}
	}
	
	if( nmp == -1 ){
		/*
		* zuluCryptSecurityCreateMountPoint() is defined in security.c
		*/
		*m_point = zuluCryptSecurityCreateMountPoint( device,mount_point,uid ) ;
		cpoint = StringContent( *m_point ) ;
		if( cpoint == NULL ){
			return zuluExit( 9,device,cpoint,stl,uid,mapping_name ) ;
		}
	}
	
	if( share ){
		/*
		 * zuluCryptBindSharedMountPointPathTaken() is defined in bind.c
		 */
		if( zuluCryptBindSharedMountPointPathTaken( *m_point ) ){
			return zuluExit_1( 10,opts,device,cpoint,stl,uid,mapping_name ) ;
		}
	}
	
	/*
	 * ZULUCRYPTshortMapperPath is set in ../constants.h
	 * zuluCryptCreateMapperName() is defined at ../lib/create_mapper_name.c
	 */
	*m_name = zuluCryptCreateMapperName( device,mapping_name,uid,ZULUCRYPTshortMapperPath ) ;

	*mapper = StringCopy( *m_name ) ;
	mapper_name = StringContent( *m_name ) ;
	
	*mapper_path = String( crypt_get_dir() ) ;
	e = StringMultipleAppend( *mapper_path,"/",mapper_name,END ) ;
	
	if( stat( e,&statstr ) == 0 ){
		return zuluExit_1( 11,opts,device,cpoint,stl,uid,mapping_name ) ;
	}
	
	if( plugin_path != NULL ){
		
		uuid = zuluCryptSecurityUUIDFromPath( device ) ;
		/*
		 * zuluCryptPluginManagerGetKeyFromModule is defined in ../pluginManager/zuluCryptPluginManager.c
		 */
		*passphrase = zuluCryptPluginManagerGetKeyFromModule( device,plugin_path,uuid,uid,opts ) ;
		
		StringFree( uuid ) ;
		
		if( *passphrase == StringVoid ){
			return zuluExit_1( 12,opts,device,cpoint,stl,uid,mapping_name ) ;
		}
		cpass = StringContent( *passphrase ) ;
		len = StringLength( *passphrase ) ;
		zuluCryptSecurityLockMemory_1( *passphrase ) ;
	}else if( source == NULL ){
		printf( "Enter passphrase: " ) ;
		/*
		 * ZULUCRYPT_KEY_MAX_SIZE is set in ../constants.h
		 */
		switch( StringSilentlyGetFromTerminal_1( passphrase,ZULUCRYPT_KEY_MAX_SIZE ) ){
			case 1 : return zuluExit_1( 13,opts,device,cpoint,stl,uid,mapping_name ) ;
			case 2 : return zuluExit_1( 14,opts,device,cpoint,stl,uid,mapping_name ) ;
		}
		printf( "\n" ) ;
		cpass = StringContent( *passphrase ) ;
		len = StringLength( *passphrase ) ;
		zuluCryptSecurityLockMemory_1( *passphrase ) ;
	}else{
		if( source == NULL || pass == NULL ){
			return zuluExit_1( 15,opts,device,cpoint,stl,uid,mapping_name ) ;
		}
		if( StringsAreEqual( source,"-p" ) ){
			cpass = pass ;
			len = StringSize( pass ) ;
		}else if( StringsAreEqual( source,"-f" ) ){
			/*
			 * function is defined at "security.c"
			 */
			switch( zuluCryptSecurityGetPassFromFile( pass,uid,data ) ){
				case 1 : return zuluExit_1( 16,opts,device,cpoint,stl,uid,mapping_name ) ;
				case 2 : return zuluExit_1( 17,opts,device,cpoint,stl,uid,mapping_name ) ; 
				case 4 : return zuluExit_1( 18,opts,device,cpoint,stl,uid,mapping_name ) ;
				case 5 : return zuluExit_1( 19,opts,device,cpoint,stl,uid,mapping_name ) ;
			}
			cpass = StringContent( *data ) ;
			len = StringLength( *data ) ;
			zuluCryptSecurityLockMemory_1( *data ) ;
		}
	}
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptOpenVolume() is defined in ../lib/open_volume.c
	 */
	st = zuluCryptOpenVolume( device,mapper_name,cpoint,uid,m_flags,fs_opts,cpass,len ) ;

	if( st == 4 ){
		/*
		 * failed to open a LUKS or PLAIN volume.
		 */
		/*
		 * zuluCryptVolumeIsNotLuks() is defined in ../lib/is_luks.c
		 */
		if( zuluCryptVolumeIsNotLuks( device ) ){
			/*
			 * The volume is not LUKS,its either PLAIN or TRUECRYPT,we already failed to open it as PLAIN
			 * so it must be TRUECRYPT or the key is wrong.
			 */
			
			/*
			 * try to open is a normal TRUECRYPT volume.
			 */
			st = _open_tcrypt( device,mapper_name,cpass,len,source,pass,TCRYPT_NORMAL,cpoint,uid,m_flags,fs_opts ) ;
			if( st == 4 ){
				/*
				* The attempt failed,retry to open it as a hidden TRUECRYPT volume.
				*/
				st = _open_tcrypt( device,mapper_name,cpass,len,source,pass,TCRYPT_HIDDEN,cpoint,uid,m_flags,fs_opts ) ;
			}
		}
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
	
	zuluCryptSecurityDropElevatedPrivileges();
	
	device = StringMultiplePrepend( *mapper,"/",crypt_get_dir(),END ) ;
	
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
	return zuluExit_1( st,opts,device,cpoint,stl,uid,mapping_name );
}

static int _open_tcrypt( const char * device,const char * mapper_name,const char * key,
			 size_t key_len,const char * key_source,const char * key_origin,
			 int volume_type,const char * m_point,uid_t uid,
			 unsigned long m_flags,const char * fs_opts )
{
	int st ;
	if( StringsAreEqual( key_source,"-f" ) ){
		if( StringHasComponent( key_origin,"/.zuluCrypt-socket/" ) ){
			/*
			 * zuluCryptOpenTcrypt() is defined in ../lib/open_tcrypt.c
			 */
			st = zuluCryptOpenTcrypt( device,mapper_name,key,key_len,TCRYPT_PASSPHRASE,volume_type,m_point,uid,m_flags,fs_opts ) ;
		}else{
			st = zuluCryptOpenTcrypt( device,mapper_name,key,key_len,TCRYPT_KEYFILE,volume_type,m_point,uid,m_flags,fs_opts ) ;
		}
	}else{
		st = zuluCryptOpenTcrypt( device,mapper_name,key,key_len,TCRYPT_PASSPHRASE,volume_type,m_point,uid,m_flags,fs_opts ) ;
	}
	
	if( st == 15 || st == 0 || st == -1 ){
		;
	}else{
		st = 4 ;
	}
	
	return st ;
}
