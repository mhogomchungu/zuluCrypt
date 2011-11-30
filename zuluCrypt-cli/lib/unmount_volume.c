/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

#define USE_UNSTABLE_LIBMOUNT_API 1
#include <mount/mount.h>

int entry_found( const char * map, const char * map_m_point, char ** m_point )
{
	StrHandle * p ;
	
	int h = umount( map_m_point ) ;
	
	if( h == 0 ){
		
		close_mapper( map ) ;
		
		if( m_point != NULL ){
		
			p = String( map_m_point ) ;
				
			*m_point = StringDeleteHandle( p ) ;				
		}
	}		
	return h ;
}

int unmount_volume( const char * map, char ** m_point )
{
	struct stat st ;		
	
	FILE * f ;	
	
	FILE * g ;	
	
	int h = 3 ;
	
	int status ;
	
	int map_len = strlen( map ) ;
	
	char path[ 16 ] ;
	
	mnt_lock * lock ;
	
	struct mntent * mt ;
	
	if ( stat( map , &st ) != 0 )
		return 1 ;		
	
	realpath( "/etc/mtab", path ) ;
	
	/*
	 *go theough /etc/mtab to find out the mount point,umount works with mount points, not device addresses. 
	*/
	f = setmntent( path ,"r" ) ;
	
	if( strncmp( path, "/proc/",6 ) == 0 ){
		/*
		 *.  /etc/mtab is a symbolic link to /proc/mounts, dont modify it to remove the entry since
		 *   umount command does it.
		 */		
		while( ( mt = getmntent( f ) ) != NULL ){
			
			if( strncmp( mt->mnt_fsname,map, map_len ) == 0 ){
				
				h = entry_found( mt->mnt_fsname, mt->mnt_dir, m_point ) ;
				
				endmntent( f ) ;
				
				break ;
			}		
		}			
	}else if ( strncmp( path, "/etc/",5 ) == 0 ) {		
		
		lock = mnt_new_lock( "/etc/mtab~", getpid() ) ;
		
		status = mnt_lock_file( lock ) ;	
		
		if( status != 0 ){
			
			endmntent( f ) ;
			
			h = 4 ;
		}else{		
			g = setmntent( "/etc/mtab-zC","w" ) ;
	
			while( ( mt = getmntent( f ) ) != NULL ){
		
				if( strncmp( mt->mnt_fsname,map, map_len ) == 0 ){
			
					h = entry_found( mt->mnt_fsname, mt->mnt_dir, m_point ) ;				
				}else			
					addmntent( g, mt ) ;			
			}

			endmntent( f ) ;
			
			endmntent( g ) ;		
		
			if( h == 0 ){			
				rename( "/etc/mtab-zC", "/etc/mtab" ) ;
				chown( "/etc/mtab", 0,0 ) ;
			}else
				remove( "/etc/mtab-zC" ) ;		
		
			mnt_unlock_file( lock ) ;
		}		
		
		mnt_free_lock( lock ) ;
	}	
	
	if( h != 0 && h != 3 && h != 4 )
		h = 2 ;

	return h ;
}
