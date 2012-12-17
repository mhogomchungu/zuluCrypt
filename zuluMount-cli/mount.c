/*
 * 
 *  Copyright (c) 2012
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

int zuluMountMount( const char * device,const char * m_point,
			    const char * m_opts,const char * fs_opts,uid_t uid,
			    int mount_point_option )
{
	int status ;
	int mount_point_from_fstab = mount_point_option ;
	string_t z = StringVoid ;
	char * path = NULL ;
	const char * rm_point ;
	unsigned long m_flags ;
	stringList_t stl = StringListVoid ;
	
	if( mount_point_from_fstab ){;}
	
	/*
	 * zuluCryptMountFlagsAreNotCorrect() is defined in ../zuluCrypt-cli/bin/mount_flags.c
	 */
	if( zuluCryptMountFlagsAreNotCorrect( m_opts,uid,&m_flags ) )
		return _zuluExit( 112,z,path,"ERROR: insuffienct privileges to mount the volume with given mount options" ) ;
	
	/*
	 * zuluCryptPathIsNotValid() is defined in ../zuluCrypt-cli/lib/is_path_valid.c
	 */
	if( zuluCryptPathIsNotValid( device ) )
		return _zuluExit( 101,z,path,"ERROR: invalid path to device" ) ;
	
	/*
	 * zuluCryptPartitionIsMounted is defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c
	 * It checks if a device has an entry in "/etc/mtab" and return 1 if it does and 0 is it doesnt
	 */
	if( zuluCryptPartitionIsMounted( device ) )
		return _zuluExit( 101,z,path,"ERROR: device already mounted" ) ;
	
	/*
	 * zuluCryptGetFstabEntryList() is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	stl = zuluCryptGetFstabEntryList( device ) ;
	
	status = _zuluMountPartitionAccess( device,m_opts,uid,stl ) ;

	if( status == 1 ){
		StringListDelete( &stl ) ;
		return _zuluExit( 102,z,path,"ERROR: \"/etc/fstab\" entry for this partition requires it to be mounted read only" ) ;
	}else if( status == 2 ){
		StringListDelete( &stl ) ;
		return _zuluExit( 103,z,path,"ERROR: \"/etc/fstab\" entry for this partition requires only root user or members of group zulucrypt to mount it" ) ;
	}else if( status == 4 ){
		StringListDelete( &stl ) ;
		return _zuluExit( 112,z,path,"ERROR: insuffienct privilege to access a system partition,only root and zulucrypt group members can do that" ) ;
	}
	
	/*
	 * zuluCryptSecurityCreateMountPoint() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	z = zuluCryptSecurityCreateMountPoint( device,m_point,uid ) ;
	
	if( z == StringVoid )
		return _zuluExit( 107,z,path,"ERROR: could not create mount point path,path already taken" ) ;
	
	rm_point = StringContent( z ) ;
	if( !zuluCryptSecurityGainElevatedPrivileges() )
		return _zuluExit( 112,z,path,"ERROR: could not get elevated privilege,check binary permissions" ) ;
	/*
	 * zuluCryptMountVolume() defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	status = zuluCryptMountVolume( device,rm_point,m_flags,fs_opts,uid ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	if( status == 0 ){
		printf( "SUCCESS: mount complete successfully\nvolume mounted at: %s\n",rm_point ) ;
		return _zuluExit( 0,z,path,NULL ) ;
	}else{
		rmdir( rm_point ) ;
		switch( status ){
			case -1: return _zuluExit( 113,z,path,"ERROR: failed to mount a filesystem,invalid mount option or permission denied" ) ;
			case 1 : return _zuluExit( 108,z,path,"ERROR: failed to mount ntfs file system using ntfs-3g,is ntfs-3g package installed?" ) ;
			case 4 : return _zuluExit( 109,z,path,"ERROR: mount failed,no or unrecognized file system" )	; 
			case 12: return _zuluExit( 110,z,path,"ERROR: mount failed,could not get a lock on /etc/mtab~" ) ;	
			default: return _zuluExit( 111,z,path,"ERROR: failed to mount the partition" ) ;
		}
	}
}