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
#include <unistd.h>
#include <sys/wait.h>

/*
 * below header file does not ship with the source code, it is created at configure time
 * */
#include "../libmount_header.h"

int fopt( const char * st,const char * mapper,const char * fs,const char * m_point,const char * mode,unsigned long mountflags, string_t * p,uid_t id )
{
	int h ;
	char uid_s[ 5 ] ;
	char * uid = StringIntToString( uid_s,5,id ) ;		
	string_t opt = String( st ) ;
	StringAppend( opt,uid ) ;
	StringAppend( opt,",gid=" ) ;
	StringAppend( opt,uid );
	h = mount( mapper,m_point,fs,mountflags,StringContent( opt ) ) ;
	StringPrepend( opt,"," ) ;
	StringPrepend( opt,mode ) ;
	*p = opt ;
	return h ;
}

int mount_ntfs( const char * mapper,const char * m_point,const char * mode,uid_t id )
{
	pid_t pid ;	
	char uid_s[ 5 ] ;
	char * uid = StringIntToString( uid_s,5,id ) ;	
	string_t opt ;
	const char * copt ;
	int status ;
	
	pid = fork() ;	
	if( pid == -1 )
		return 1 ;
	if( pid == 0 ){
		close(1);
		close(2);
		opt = String( "-o dmask=077,umask=077," ) ;
		StringAppend( opt,mode ) ;
		StringAppend( opt,",uid=" ) ;
		StringAppend( opt,uid ) ;
		StringAppend( opt,",gid=" ) ;
		StringAppend( opt,uid ) ;
		copt = StringContent( opt ) ;
		execl( ZULUCRYPTmount,"mount","-t","ntfs-3g",copt,mapper,m_point,( char * )0 ) ;
	}
	waitpid( pid,&status,0 ) ;
	return status ;	
}

int mount_mapper( const char * mapper,const char * m_point,const char * mode,uid_t id, const char * fs, string_t * options )
{
	unsigned long mountflags = 0 ;
	int h ;

	string_t opt ;
	
	if( strcmp( mode,"ro" ) == 0 )
		mountflags = MS_RDONLY ;	
	
	if( strcmp( fs,"vfat" ) == 0 || strcmp( fs,"fat" ) == 0 || strcmp( fs,"msdos" ) == 0 || strcmp( fs,"umsdos" ) == 0 ){
		h = fopt( "dmask=077,uid=",mapper,fs,m_point,mode,mountflags,&opt,id ) ;
	}else if( strcmp( fs,"affs" ) == 0 || strcmp( fs,"hfs" ) == 0 || strcmp( fs,"iso9660" ) == 0 ){
		h = fopt( "uid=",mapper,fs,m_point,mode,mountflags,&opt,id ) ;		
	}else{
		opt = String( mode ) ;
		h = mount( mapper,m_point,fs,mountflags,NULL ) ;
		if( h == 0 && mountflags != MS_RDONLY ){			
			chmod( m_point,S_IRWXU ) ;
			chown( m_point,id,id ) ;
		}
	}

	*options = opt ;
	return h ;
}

int mount_volume( const char * mapper,const char * m_point,const char * mode,uid_t id )
{
	struct mntent mt  ;
	blkid_probe blkid ;
	char * path ;
	int h ;
	const char * cf ;	
	FILE * f ;
#if USE_NEW_LIBMOUNT_API
	struct libmnt_lock * m_lock ;
#else
	mnt_lock * m_lock ;
#endif
	string_t options = NULL ;
	string_t fs = NULL ;
	
	blkid = blkid_new_probe_from_filename( mapper ) ;
	blkid_do_probe( blkid );
	
	if( blkid_probe_lookup_value( blkid,"TYPE",&cf,NULL ) != 0 ){
		/*
		 * Attempt to read volume file system has failed because either an attempt to open a plain based volumes with
		 * a wrong password was made or the volume has no file system.
		 */		
		blkid_free_probe( blkid );
		return 4 ;		
	}
	
	fs = String( cf ) ;
	
	blkid_free_probe( blkid );
	
	/*
	 * Currently, i dont know how to use mount system call to use ntfs-3g instead of ntfs to mount ntfs file systems.
	 * Use fork to use mount executable as a temporary solution.
	*/
	if( strcmp( StringContent( fs ),"ntfs" ) == 0 ){
		StringDelete( &fs ) ;
		h = mount_ntfs( mapper,m_point,mode,id ) ;
		switch( h ){
			case 0  : return 0 ;
			case 16 : return 12 ;
			default : return 1 ;
		}
	}
	 
	path = realpath( "/etc/mtab",NULL ) ;
	
	if( strncmp( path,"/proc",5 ) == 0 )
		h = mount_mapper( mapper,m_point,mode,id,StringContent( fs ),&options ) ;
	else{
		/* "/etc/mtab" is not a symbolic link to /proc/mounts, manually,add an entry to it since 
		 * mount command does not
		 */		
		m_lock = mnt_new_lock( "/etc/mtab~",getpid() ) ;
		if( mnt_lock_file( m_lock ) != 0 ){
			h = 12 ;
		}else{		
			h = mount_mapper( mapper,m_point,mode,id,StringContent( fs ),&options ) ;
			if( h == 0 ){
				f = setmntent( "/etc/mtab","a" ) ;	
				mt.mnt_fsname = ( char * ) mapper ;
				mt.mnt_dir    = ( char * ) m_point ;
				mt.mnt_type   = ( char * ) StringContent( fs ) ;	
				mt.mnt_opts   = ( char * ) StringContent( options ) ;
				mt.mnt_freq   = 0 ;
				mt.mnt_passno = 0 ;
				addmntent( f,&mt ) ;	
				endmntent( f ) ;
			}
			mnt_unlock_file( m_lock ) ;
		}	
		mnt_free_lock( m_lock ) ;
	}
	free( path ) ;
	StringDelete( &fs ) ;
	StringDelete( &options ) ;
	return h ;
}

