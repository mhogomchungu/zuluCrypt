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

#include "../process/process.h"
/*
 * below header file does not ship with the source code, it is created at configure time
 * */
#include "libmount_header.h"

#define FAT_FAMILY_FS 1
#define OTHER_FAMILY_FS 2

typedef struct{
	const char * device ;
	const char * m_point ;
	const char * mode ;
	const char * fs ;
	uid_t uid ;
	unsigned long m_flags ;
}m_struct;

static int mount_fs( int type,const m_struct * mst, string_t * st )
{
	string_t opt = String( mst->mode ) ;
	
	string_t uid = StringIntToString( mst->uid ) ;
	
	const char * copt = StringContent( uid ) ;	
	
	if( type == FAT_FAMILY_FS )
		StringAppend( opt,",dmask=077,uid=UID,gid=UID" ) ;
	else
		StringAppend( opt,",uid=UID,gid=UID" ) ;
	
	copt = StringReplaceString( opt,"UID",copt ) + 3 ;
	
	StringDelete( &uid ) ;
	
	*st = opt ;
	
	return mount( mst->device,mst->m_point,mst->fs,mst->m_flags,copt ) ;
}

static int mount_ntfs( const m_struct * mst )
{
	const char * copt ;
	
	int status ;	
	
	process_t p ;
	
	string_t uid = StringIntToString( mst->uid );		
	string_t q ;
	
	if( strcmp( mst->mode,"ro" ) == 0 )
		q = String( "dmask=077,umask=077,ro,uid=UID,gid=UID" ) ;
	else
		q = String( "dmask=077,umask=077,rw,uid=UID,gid=UID" ) ;
	
	copt = StringReplaceString( q,"UID",StringContent( uid ) ) ;
		
	p = Process( ZULUCRYPTmount ) ;
	
	ProcessSetArgumentList( p,"-t","ntfs-3g","-o",copt,mst->device,mst->m_point,'\0' ) ;

	ProcessSetOption( p,CLOSE_BOTH_STD_OUT ) ;

	ProcessStart( p ) ;
	
	status = ProcessExitStatus( p ) ; 
	
	ProcessDelete( &p ) ;
	
	StringMultipleDelete( &q,&uid,'\0' ) ;
	
	return status ;
}

static int mount_mapper( const m_struct * mst, string_t * st )
{
	int h ;
	const char * fs = mst->fs ;

	if( strcmp( fs,"vfat" ) == 0 || strcmp( fs,"fat" ) == 0 || strcmp( fs,"msdos" ) == 0 || strcmp( fs,"umsdos" ) == 0 ){
		h = mount_fs( FAT_FAMILY_FS,mst,st ) ;
	}else if( strcmp( fs,"affs" ) == 0 || strcmp( fs,"hfs" ) == 0 || strcmp( fs,"iso9660" ) == 0 ){
		h = mount_fs( OTHER_FAMILY_FS,mst,st ) ;		
	}else{
		*st = String( mst->mode ) ;
		h = mount( mst->device,mst->m_point,mst->fs,mst->m_flags,NULL ) ;
		if( h == 0 && mst->m_flags != MS_RDONLY ){
			chmod( mst->m_point,S_IRWXU|S_IRWXG|S_IRWXO ) ;
		}
	}
	return h ;
}

int zuluCryptMountVolume( const char * mapper,const char * m_point,const char * mode,uid_t id )
{
	struct mntent mt  ;
	blkid_probe blkid ;
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
	
	m_struct mst ;
	mst.device = mapper ;
	mst.m_point = m_point ;
	mst.mode = mode ;
	mst.uid = id ;
	
	if( strcmp( mode,"ro" ) == 0 )
		mst.m_flags = MS_RDONLY ;
	else
		mst.m_flags = 0 ;
	
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
	if( StringEqual( fs,"ntfs" ) == 0 ){
		StringDelete( &fs ) ;
		switch( mount_ntfs( &mst ) ){
			case 0  : return 0 ;
			case 16 : return 12 ;
			default : return 1 ;
		}
	}
	 
	mst.fs = StringContent( fs ) ;

	/*
	 * mtab_is_at_etc() is defined in print_mounted_volumes.c
	 * 
	 */
	
	if( zuluCryptMtabIsAtEtc() != 0 ){
		h = mount_mapper( &mst,&options ) ;
	}else{
		/* 
		 * "/etc/mtab" is not a symbolic link to /proc/mounts, manually,add an entry to it since 
		 * mount API does not
		 */		
		m_lock = mnt_new_lock( "/etc/mtab~",getpid() ) ;
		if( mnt_lock_file( m_lock ) != 0 ){
			h = 12 ;
		}else{		
			h = mount_mapper( &mst,&options ) ;
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
	
	StringMultipleDelete( &fs,&options,'\0' ) ;

	return h ;
}

