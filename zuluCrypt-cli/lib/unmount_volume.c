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
#include "../libmount_header.h"

static int entry_found( const char * map, const char * m_dir, char ** m_point )
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
		if( h == 0 )
			break ;
		sleep( 1 ) ;
	}	
	
	if( h == 0 && m_point != NULL ){
		st = String( m_dir ) ;
		*m_point = StringDeleteHandle( &st ) ;
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
	char * path ;
#if USE_NEW_LIBMOUNT_API
	struct libmnt_lock * lock ;
#else
	mnt_lock * lock ;
#endif
	struct mntent * mt ;
	
	if( stat( map,&st ) != 0 )
		return 1 ;		
	
	path = realpath( "/etc/mtab",NULL ) ;	

	if( path == NULL )
		return h ;
	
	f = setmntent( path,"r" ) ;
	
	if( strncmp( path,"/proc/",6 ) == 0 ){
		/*
		 *.  /etc/mtab is a symbolic link to /proc/mounts, dont modify it to remove the entry since
		 *   umount command does it.
		 */		
		while( ( mt = getmntent( f ) ) != NULL ){
			if( strncmp( mt->mnt_fsname,map,map_len ) == 0 ){
				h = entry_found( mt->mnt_fsname,mt->mnt_dir,m_point ) ;
				endmntent( f ) ;
				break ;
			}		
		}			
	}else if( strncmp( path,"/etc/",5 ) == 0 ) {	
		/*
		 * .  /etc/mtab is a reguar file,manually edit it to remove the unmounted volume.
		 */
		lock = mnt_new_lock( "/etc/mtab~",getpid() ) ;
		status = mnt_lock_file( lock ) ;	
		
		if( status != 0 ){
			endmntent( f ) ;
			h = 4 ;
		}else{		
			g = setmntent( "/etc/mtab-zC","w" ) ;
			while( ( mt = getmntent( f ) ) != NULL ){
				if( strncmp( mt->mnt_fsname,map,map_len ) == 0 ){
					h = entry_found( mt->mnt_fsname,mt->mnt_dir,m_point ) ;				
				}else			
					addmntent( g,mt ) ;			
			}
			endmntent( f ) ;
			endmntent( g ) ;		
		
			if( h == 0 ){			
				rename( "/etc/mtab-zC","/etc/mtab" ) ;
				chown( "/etc/mtab",0,0 ) ;
			}else
				remove( "/etc/mtab-zC" ) ;		
		
			mnt_unlock_file( lock ) ;
		}	
		mnt_free_lock( lock ) ;
	}	
	if( h != 0 && h != 3 && h != 4 )
		h = 2 ;

	free( path ) ;
	return h ;
}
