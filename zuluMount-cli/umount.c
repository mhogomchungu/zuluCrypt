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

int zuluMountUMount( const char * device,uid_t uid,const char * mode,int mount_point_option )
{
	char * loop_device ;
	char * m_point = NULL ;
	int status ;
	string_t st = StringVoid ;
	const char * dev = NULL ;
	if( mode ) {;}
	if( mount_point_option ) {;}
	
	if( strncmp( device,"/dev/loop",9 ) == 0 ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ../zuluCrypt-cli/lib/create_loop_devices.c
		 */
		loop_device = zuluCryptLoopDeviceAddress( device ) ;
		if( loop_device == NULL ){
			return _zuluExit( 100,StringVoid,m_point,"ERROR: device does not appear to be mounted" ) ;
		}else{
			st = StringInherit( &loop_device ) ;
			dev = StringContent( st ) ;
			/*
			 * zuluCryptGetMountPointFromPath() is defined in defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c 
			 */
			m_point = zuluCryptGetMountPointFromPath( dev ) ;
			if( m_point == NULL ){
				return _zuluExit( 100,st,m_point,"ERROR: device does not appear to be mounted" ) ;
			}
		}
	}else{
		/*
		* zuluCryptGetMountPointFromPath() is defined in defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c 
		*/
		m_point = zuluCryptGetMountPointFromPath( device ) ;
		if( m_point == NULL )
			return _zuluExit( 100,st,m_point,"ERROR: device does not appear to be mounted" ) ;
	}
	
	/*
	 * zuluCryptSecurityMountPointPrefixMatch() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	if( !zuluCryptSecurityMountPointPrefixMatch( m_point,uid ) ){
		if( uid != 0 ){
			return _zuluExit( 101,st,m_point,"ERROR: you can only unmount volumes you have mounted" ) ;
		}
	}
	
	free( m_point ) ;
	m_point = NULL ;
	
	/*
	 * zuluCryptSecurityGainElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	if( !zuluCryptSecurityGainElevatedPrivileges() )
		return _zuluExit( 102,st,m_point,"ERROR: could not get elevated privilege,check binary permissions" ) ;
	/*
	 * zuluCryptUnmountVolume() is defined in ../zuluCrypt-cli/lib/unmount_volume.c
	 */
		
	status = zuluCryptUnmountVolume( device,&m_point ) ;
	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	
	if( status == 0 ){
		if( m_point != NULL ){
			rmdir( m_point ) ;
		}
		zuluCryptSecurityDropElevatedPrivileges() ;
		return _zuluExit( 0,st,m_point,"SUCCESS: umount complete successfully" ) ;
	}else{
		zuluCryptSecurityDropElevatedPrivileges() ;
		switch( status ) {
			case 1 : return _zuluExit( 103,st,m_point,"ERROR: device does not exist" )  ;
			case 2 : return _zuluExit( 104,st,m_point,"ERROR: failed to unmount,the mount point and/or one or more files are in use" );
			case 4 : return _zuluExit( 105,st,m_point,"ERROR: failed to unmount,could not get a lock on /etc/mtab~" ) ;
			default: return _zuluExit( 106,st,m_point,"ERROR: failed to unmount the partition" ) ;
		}
	}
}
