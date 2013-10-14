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

#include <mntent.h>
#include <sys/mount.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

static inline int _unmount_volume( const char * m_dir )
{
	int h ;
	int i ;
	
	/*
	 * try 5 times on one second intervals to umount the volume.
	 * Trying to unmount more than once seem to be necessary sometimes
	 */
	for( i = 0 ; i < 5 ; i++ ){
		h = umount( m_dir ) ;
		if( h == 0 ){
			break ;
		}else{
			sleep( 1 ) ;
		}
	}
	
	return h ;
}

static inline int _unmount_volume_1( const char * m_dir )
{
	process_t p ;
	
	int h ;
	int i ;
	
	/*
	 * try 5 times on one second intervals to umount the volume.
	 * Trying to unmount more than once seem to be necessary sometimes
	 */
	for( i = 0 ; i < 5 ; i++ ){
		p = Process( ZULUCRYPTumount ) ;
		ProcessSetArgumentList( p,m_dir,ENDLIST ) ;
		ProcessStart( p ) ;
		h = ProcessExitStatus( p ) ;
		ProcessDelete( &p ) ;
		if( h == 0 ){
			break ;
		}else{
			sleep( 1 ) ;
		}
	}
	
	return h ;
}

int zuluCryptUnmountVolume( const char * device,char ** m_point )
{
	char * m ;
	int h = 3 ;
	
	char * loop_path = NULL ;
	
	string_t fs ;
	
	if( StringPrefixMatch( device,"/dev/loop",9 ) ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ./create_loop_device.c
		 */
		loop_path = zuluCryptLoopDeviceAddress( device ) ;
		if( loop_path != NULL ){
			device = loop_path ;
		}
	}
	
	/*
	 * zuluCryptGetMountPointFromPath() is defined in ./process_mountinfo.c
	 */
	m = zuluCryptGetMountPointFromPath( device ) ;
	
	if( m != NULL ){
		/*
		 * zuluCryptGetFileSystemFromDevice() is defined in ./mount_volume.c
		 */
		fs = zuluCryptGetFileSystemFromDevice( device ) ;
		
		if( StringEqual( fs,"ntfs" ) ){
			/*
			 * This is a workaround for ntfs file system.
			 * In my system,the "mount" command seems to ignore the "-n" option and mtab
			 * is getting updated and we unmount using "umount" command to let it
			 * update mtab since we currently do not support mtab.
			 */
			h = _unmount_volume_1( m ) ;
		}else{
			h = _unmount_volume( m ) ;
		}
		
		StringDelete( &fs ) ;
		
		if( h == 0 ){
			if( m_point != NULL ){
				*m_point = m ;
			}else{
				StringFree( m ) ;
			}
		}else{
			StringFree( m ) ;
		}
	}
	
	if( h != 0 && h != 3 && h != 4 ){
		h = 2 ;
	}
	
	StringFree( loop_path ) ;
	
	return h ;
}
