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

int zuluMountUMount( ARGS * args )
{
	const char * device    = args->device ;
	uid_t        uid       = args->uid    ;
	const char * mode      = args->m_opts   ;
	int mount_point_option = args->mpo    ;
	char * loop_device ;
	char * m_point = NULL ;
	int status ;
	string_t st = StringVoid ;
	const char * dev = NULL ;
	const char * errorMsg = gettext( "\
ERROR: you can not umount volumes out of \"%s\" since you are not root and do not belong to group \"zulumount\"\n" ) ;
	string_t xt ;
	
	if( 0 && mode && mount_point_option ) {;}
	
	if( StringPrefixEqual( device,"/dev/loop" ) ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ../zuluCrypt-cli/lib/create_loop_devices.c
		 */
		loop_device = zuluCryptLoopDeviceAddress( device ) ;
		if( loop_device == NULL ){
			/*
			 * the error msg is a lie,but its harmless since the user will most likely never see it as
			 * this code path will not be passed.
			 */
			return _zuluExit( 100,StringVoid,m_point,gettext( "ERROR: device does not appear to be mounted" ) ) ;
		}else{
			st = StringInherit( &loop_device ) ;
			dev = StringContent( st ) ;
			/*
			 * zuluCryptGetMountPointFromPath() is defined in defined in ../zuluCrypt-cli/lib/process_mountinfo.c 
			 */
			m_point = zuluCryptGetMountPointFromPath( dev ) ;
			if( m_point == NULL ){
				return _zuluExit( 100,st,m_point,gettext( "ERROR: device does not appear to be mounted" ) ) ;
			}
		}
	}else{
		/*
		 * zuluCryptGetMountPointFromPath() is defined in defined in ../zuluCrypt-cli/lib/process_mountinfo.c 
		*/
		m_point = zuluCryptGetMountPointFromPath( device ) ;
		if( m_point == NULL ){
			return _zuluExit( 100,st,m_point,gettext( "ERROR: device does not appear to be mounted" ) ) ;
		}
	}
	
	/*
	 * zuluCryptSecurityMountPointPrefixMatch() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	if( !zuluCryptSecurityMountPointPrefixMatch( m_point,uid,&xt ) ){
		/*
		 * zuluCryptUserIsAMemberOfAGroup() is defined in ../zuluCrypt-cli/bin/security.c
		 */
		if( !zuluCryptUserIsAMemberOfAGroup( uid,"zulumount" ) ){
			printf( errorMsg,StringContent( xt ) ) ;
			StringDelete( &xt ) ;
			return _zuluExit( 101,st,m_point,NULL ) ;
		}else{
			StringDelete( &xt ) ;
		}
	}else{
		StringDelete( &xt ) ;
	}
	
	StringFree( m_point ) ;
	m_point = NULL ;
	
	/*
	 * zuluCryptBindUnmountVolume() is defined in ../zuluCrypt-cli/bin/bind.c
	 */
	switch( zuluCryptBindUnmountVolume( StringListVoid,device,uid ) ){
		case 3 : return _zuluExit( 107,st,m_point,gettext( "ERROR: shared mount point appear to be busy" ) ) ;
		case 4 : return _zuluExit( 108,st,m_point,gettext( "ERROR: shared mount point appear to belong to a different user" ) ) ;
		case 5 : return _zuluExit( 109,st,m_point,gettext( "ERROR: shared mount point appear to be in an ambiguous state,advice to unmount manually" ) ) ;  
		default: ;
	}
	
	
	/*
	 * zuluCryptSecurityGainElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptUnmountVolume() is defined in ../zuluCrypt-cli/lib/unmount_volume.c
	 */
	status = zuluCryptUnmountVolume( device,&m_point ) ;
	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	if( status == 0 ){
		if( m_point != NULL ){
			zuluCryptSecurityGainElevatedPrivileges() ;
			rmdir( m_point ) ;
			zuluCryptSecurityDropElevatedPrivileges() ;
		}
		return _zuluExit( 0,st,m_point,gettext( "SUCCESS: umount complete successfully" ) ) ;
	}else{
		switch( status ) {
			case 1 : return _zuluExit( 103,st,m_point,gettext( "ERROR: device does not exist" ) ) ;
			case 2 : return _zuluExit( 104,st,m_point,gettext( "ERROR: failed to unmount,the mount point and/or one or more files are in use" ) ) ;
			case 4 : return _zuluExit( 105,st,m_point,gettext( "ERROR: failed to unmount,could not get a lock on /etc/mtab~" ) ) ;
			default: return _zuluExit( 106,st,m_point,gettext( "ERROR: failed to unmount the partition" ) ) ;
		}
	}
}
