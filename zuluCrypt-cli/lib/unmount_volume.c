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

/*
 * below header file does not ship with the source code, it is created at configure time
 * */
#include "libmount_header.h"

static int entry_found( const char * m_dir,char ** m_point )
{
	string_t st ;
	int h ;
	int i ;
	
	/*
	 * try 5 times on one second intervals to umount the volume.
	 * Trying to unmount more than once seem to be necessary sometimes
	 *  when the opened volume is accessed over samba share. 
	 */
	for( i = 0 ; i < 5 ; i++ ){
		h = umount( m_dir ) ;
		if( h == 0 ){
			break ;
		}else{
			sleep( 1 ) ;
		}
	}
	
	if( h == 0 && m_point != NULL ){
		st = String( m_dir ) ;
		*m_point = StringDeleteHandle( &st ) ;
	}
	
	return h ;
}

int zuluCryptUnmountVolume( const char * map,char ** m_point )
{
	char * m ;
	struct stat st ;
	FILE * f ;
	FILE * g ;
	int h = 3 ;
	int status ;
	size_t map_len = strlen( map ) ;
	
#if USE_NEW_LIBMOUNT_API
	struct libmnt_lock * lock ;
#else
	mnt_lock * lock ;
#endif
	struct mntent * mt ;
	
	if( stat( map,&st ) != 0 )
		return 1 ;
	
	/*
	 * zuluCryptMtabIsAtEtc() is defined in print_mounted_volumes.c
	 * 1 is return if "mtab" is found to be a file located at "/etc/"
	 * 0 is returned otherwise,probably because "mtab" is a soft link to "/proc/mounts"
	 */
	
	if( zuluCryptMtabIsAtEtc() ){
		f = setmntent( "/etc/mtab","r" ) ;
		
		lock = mnt_new_lock( "/etc/mtab~",getpid() ) ;
		status = mnt_lock_file( lock ) ;
		
		if( status != 0 ){
			h = 4 ;
		}else{
			g = setmntent( "/etc/mtab-zC","w" ) ;
			while( ( mt = getmntent( f ) ) != NULL ){
				if( strncmp( mt->mnt_fsname,map,map_len ) == 0 ){
					h = entry_found( mt->mnt_dir,m_point ) ;
				}else{
					addmntent( g,mt ) ;
				}
			}
			
			endmntent( g ) ;
			
			if( h == 0 ){
				rename( "/etc/mtab-zC","/etc/mtab" ) ;
				chown( "/etc/mtab",0,0 ) ;
			}else{
				remove( "/etc/mtab-zC" ) ;
			}
			
			mnt_unlock_file( lock ) ;
		}
		
		endmntent( f ) ;
		mnt_free_lock( lock ) ;
	}else{
		/*
		 * zuluCryptGetMountPointFromPath() is defined in ./print_mounted_volumes.c
		 */
		m = zuluCryptGetMountPointFromPath( map ) ;
		if( m != NULL ){
			h = entry_found( m,m_point ) ;
			free( m ) ;
		}
	}
	
	if( h != 0 && h != 3 && h != 4 )
		h = 2 ;

	return h ;
}
