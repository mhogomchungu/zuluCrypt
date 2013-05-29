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

static int _zuluMountPartitionAccess( const char * device,const char * m_opts,uid_t uid )
{
	/*
	 * this function is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	/*
	 * MOUNTOPTIONS constant is defined in ../zuluCrypt-cli/lib/includes.h
	 */
	
	int ro      ;
	int nouser  ;
	int defaulT ;
	int user    ;
	int users   ;
	int system_partition ;
	int st = 1  ;
	/*
	 * zuluCryptGetFstabEntryList() is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	stringList_t stl ; 
	string_t p ;
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	stl = zuluCryptGetFstabEntryList( device,uid ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	if( stl != StringListVoid ){
		if( StringListSize( stl ) != 6 ){
			StringListDelete( &stl ) ;
			return 3 ;
		}
	}

	p = StringListStringAt( stl,MOUNTOPTIONS ) ;
	
	/*
	 * zuluCryptPartitionIsSystemPartition() is defined in ../zuluCrypt-cli/bin/partition.c
	 */
	system_partition = zuluCryptPartitionIsSystemPartition( device,uid ) ;

	if( system_partition ){
		/*
		 * zuluCryptUserIsAMemberOfAGroup() is defined in ../zuluCrypt/bin/security.c
		 */
		if( zuluCryptUserIsAMemberOfAGroup( uid,"zulumount" ) ){
			system_partition = 0 ;
		}
	}
	
	if( p == StringVoid ){
		/*
		 * partition does not have an entry in fstab
		 */
		if( system_partition ){
			/*
			 * partition is system partition
			 */
			if( uid == 0 ){
				/*
				 * cant say no to root
				 */
				st = 0 ;
			}else{
				/*
				* system partition with no entry in fstab,refuse to mount this one
				*/
				st = 1 ;
			}
		}else{
			/*
			 * no entry in fstab,not a system partition,mount this one
			 */
			st = 0 ;
		}
	}else{
		/*
		 * has an entry in fstab
		 */
		ro      = StringContains( p,"ro" ) ;
		nouser  = StringContains( p,"nouser" ) ;
		defaulT = StringContains( p,"defaults" ) ;
		users   = StringContains( p,"users" );
		user    = StringContains( p,"user" ) ;
		
		if( ro && StringHasComponent( m_opts,"rw" ) ){
			/*
			 * respect the option for the partition to be mounted read only
			 */
			st = 2 ;
		}else if( uid == 0 ){
			/*
			 * user is root,mount it
			 */
			st = 0 ;
		}else{
			if( nouser ){
				/*
				 * normal user is not allowed to mount it
				 */
				st = 1 ;
			}else if( user || users ){
				/*
				 * the partition has option to allow normal user to mount it,mount it
				 */
				st = 0 ;
			}else if( defaulT ){
				/*
				 * zuluCryptUserIsAMemberOfAGroup() is defined in ../zuluCrypt/bin/security.c
				 */
				if( zuluCryptUserIsAMemberOfAGroup( uid,"zulumount" ) ){
					/*
					 * user is a member is zulumount group,mount it 
					 */
					st = 0 ;
				}else{
					st = 1 ;
				}
			}else{
				/*
				 * remaining options go there
				 */
				/*
				 * zuluCryptUserIsAMemberOfAGroup() is defined in ../zuluCrypt/bin/security.c
				 */
				if( zuluCryptUserIsAMemberOfAGroup( uid,"zulumount" ) ){
					/*
					 * user is a member is zulumount group,mount it 
					 */
					st = 0 ;
				}else{
					st = 1 ;
				}
			}
		}
	}

	StringListDelete( &stl ) ;
	return st ;
}

int zuluMountMount( ARGS * args )
{
	const char * device    = args->device  ;
	const char * m_point   = args->m_point ;
	const char * m_opts    = args->m_opts  ;
	const char * fs_opts   = args->fs_opts ;
	uid_t        uid       = args->uid     ;
	int share              = args->share   ;
	
	int mount_point_option = args->mpo;
	int status ;
	int mount_point_from_fstab = mount_point_option ;
	string_t z = StringVoid ;
	char * path = NULL ;
	const char * rm_point ;
	unsigned long m_flags ;
	const char * dev = device ;
	const char * msg ="\
ERROR: insuffienct privilege to manage a system volume.\nnecessary privileges can be acquired by:\n\
1. adding an entry for the volume in fstab with \"user\" mount option\n\
2. add yourself to \"zulumount\" group";
	
	if( mount_point_from_fstab ){;}
	
	if( StringPrefixEqual( device,"/dev/loop" ) ){
		/*
		 * zuluCryptLoopDeviceAddress_1() is defined in ../zuluCrypt-cli/lib/create_loop_devices.c
		 */
		path = zuluCryptLoopDeviceAddress_1( device ) ;
		if( path == NULL ){
			return _zuluExit( 112,z,path,"ERROR: could not resolve path to device" ) ;
		}else{
			dev = path ;
		}
	}
	
	/*
	 * zuluCryptMountFlagsAreNotCorrect() is defined in ../zuluCrypt-cli/bin/mount_flags.c
	 */
	if( zuluCryptMountFlagsAreNotCorrect( m_opts,uid,&m_flags ) ){
		return _zuluExit( 100,z,path,"ERROR: insuffienct privileges to mount the volume with given mount options" ) ;
	}
	/*
	 * zuluCryptPartitionIsMounted is defined in ../zuluCrypt-cli/lib/process_mountinfo.c
	 */
	if( zuluCryptPartitionIsMounted( dev ) ){
		return _zuluExit( 102,z,path,"ERROR: device already mounted" ) ;
	}
	
	status = _zuluMountPartitionAccess( dev,m_opts,uid ) ;
	
	switch( status ){
		case 0 : break ;
		case 1 : return _zuluExit( 103,z,path,msg ) ;
		case 2 : return _zuluExit( 104,z,path,"ERROR: \"/etc/fstab\" entry for this volume requires it to be mounted read only" ) ;
		case 3 : return _zuluExit( 113,z,path,"ERROR: \"/etc/fstab\" entry for this volume is malformed" ) ;
		default: return _zuluExit( 105,z,path,"ERROR: \"/etc/fstab\" entry for this volume does not allow you to mount it" ) ;
	}
	
	/*
	 * zuluCryptSecurityCreateMountPoint() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	z = zuluCryptSecurityCreateMountPoint( device,m_point,uid ) ;
	
	if( z == StringVoid ){
		return _zuluExit( 106,z,path,"ERROR: could not create mount point path,path already taken" ) ;
	}
	
	rm_point = StringContent( z ) ;
	
	if( share ){
		/*
		 * zuluCryptBindSharedMountPointPathTaken() is defined in ../zuluCrypt-cli/bin/bind.c
		 */
		if( zuluCryptBindSharedMountPointPathTaken( z ) ){
			zuluCryptSecurityGainElevatedPrivileges() ;
			rmdir( rm_point ) ;
			zuluCryptSecurityDropElevatedPrivileges() ;
			return _zuluExit( 113,z,path,"ERROR: shared mount point path aleady taken" ) ;
		}
	}
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptMountVolume() defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	status = zuluCryptMountVolume( device,rm_point,m_flags,fs_opts,uid ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	if( status == 0 ){
		if( share ){
			/*
			 * user wish to share the mount point publicly, bind the mount point to a publicly accessed path of /run/share
			 */
			/*
			 * zuluCryptBindMountVolume() is defined in ../zuluCrypt-cli/bin/bind.c
			 */
			zuluCryptBindMountVolume( device,z,m_flags ) ;
		}
		printf( "SUCCESS: mount complete successfully\nvolume mounted at: %s\n",rm_point ) ;
		return _zuluExit( 0,z,path,NULL ) ;
	}else{
		zuluCryptSecurityGainElevatedPrivileges() ;
		rmdir( rm_point ) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
		switch( status ){
			case -1: return _zuluExit( 108,z,path,"ERROR: failed to mount a filesystem:invalid/unsupported mount option or unsupported file system encountered" ) ;
			case 1 : return _zuluExit( 109,z,path,"ERROR: failed to mount ntfs file system using ntfs-3g,is ntfs-3g package installed?" ) ;
			case 4 : return _zuluExit( 110,z,path,"ERROR: mount failed,no or unrecognized file system" )	; 
			case 12: return _zuluExit( 111,z,path,"ERROR: mount failed,could not get a lock on /etc/mtab~" ) ;	
			default: return _zuluExit( 112,z,path,"ERROR: failed to mount the partition" ) ;
		}
	}
}
