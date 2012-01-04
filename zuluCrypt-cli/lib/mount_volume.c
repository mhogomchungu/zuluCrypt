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

#include <sys/mount.h>
#include <mntent.h>
#include <blkid/blkid.h>
#include <stdlib.h>
#include <unistd.h>

#define USE_UNSTABLE_LIBMOUNT_API 1
#include <mount/mount.h>

int mount_volume( const char * mapper,const char * m_point,const char * mode,uid_t id )
{
	string_t * fs ;
	string_t * options ;
	unsigned long mountflags = 0 ;
	struct mntent mt  ;
	blkid_probe blkid ;
	char uid[ 5 ] ;
	char path[ 16 ] ;
	int h ;
	int status ;
	const char * cf ;	
	FILE * f ;
	mnt_lock * m_lock ;
	
	blkid = blkid_new_probe_from_filename( mapper ) ;
	blkid_do_probe( blkid );
	h = blkid_probe_lookup_value( blkid , "TYPE", &cf, NULL ) ;		
	
	if( h != 0 ){
		blkid_free_probe( blkid );
		close_mapper( mapper ) ; 
		return 4 ;
	}	
	fs = String( cf ) ;
	
	blkid_free_probe( blkid );
	
	if ( strcmp( mode, "ro" ) == 0 )
		mountflags = MS_RDONLY ;
		
	if( strcmp( StringContent( fs ), "vfat" ) == 0 ){
		options = String( "dmask=007,uid=" ) ;
		StringAppend( options ,StringIntToString( uid, 5, id ) ) ;
		StringAppend( options , ",gid=" ) ;
		StringAppend( options ,StringIntToString( uid, 5, id ) );
		h = mount( mapper, m_point,StringContent( fs ),mountflags,StringContent( options ) ) ;	
		StringPrepend( options ,"," ) ;
		StringPrepend( options , mode ) ;
	}else{		
		options = String( mode ) ;
		h = mount( mapper, m_point,StringContent( fs ),mountflags,NULL) ;
	}	
	if( h != 0 ){
		close_mapper( mapper ) ; 
		h = 4 ;		
	}else{		
		realpath( "/etc/mtab", path ) ;
		if( strncmp( path, "/etc/",5 ) == 0 ){
			/* "/etc/mtab" is not a symbolic link to /proc/mounts, add an entry to it since 
			 * mount command doesnt
			 */		
			m_lock = mnt_new_lock( "/etc/mtab~", getpid() ) ;
			status = mnt_lock_file( m_lock ) ;	
			if( status != 0 ){
				umount( m_point ) ;	
				close_mapper( mapper ) ;
				h = 12 ;
			}else{			
				f = setmntent( "/etc/mtab","a" ) ;	
				mt.mnt_fsname = ( char * ) mapper ;
				mt.mnt_dir =    ( char * ) m_point ;
				mt.mnt_type =   ( char * ) StringContent( fs ) ;	
				mt.mnt_opts =   ( char * ) StringContent( options ) ;
				mt.mnt_freq =   0 ;
				mt.mnt_passno = 0 ;
				addmntent( f, &mt ) ;	
				endmntent( f ) ;
				mnt_unlock_file( m_lock ) ;
				mnt_free_lock( m_lock ) ;
			}
		}
	}
	StringDelete( fs ) ;
	StringDelete( options ) ;
	return h ;
}

