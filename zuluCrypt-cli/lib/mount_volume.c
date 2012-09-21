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
#include "../string/StringList.h"

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
	const char * opts ;
	uid_t uid ;
	unsigned long m_flags ;
}m_struct;

static string_t resolveUUIDAndLabel( string_t st )
{
	char * e ;
	string_t xt ;
	if( StringStartsWith( st,"LABEL=" ) ) {
		e = blkid_evaluate_tag( "LABEL",StringContent( st ) + 6,NULL ) ;
		if( e != NULL ){
			xt = StringInherit( &e ) ;
		}else{
			xt = StringCopy( st ) ;			
		}
	}else if( StringStartsWith( st,"UUID=" ) ){
		e = blkid_evaluate_tag( "UUID",StringContent( st ) + 5,NULL ) ;
		if( e != NULL ){
			xt = StringInherit( &e ) ;		
		}else{
			xt = StringCopy( st ) ;			
		}
	}else{
		xt = StringCopy( st ) ;
	}
	
	return xt ;		
}

static string_t zuluCryptGetMountOptionsFromFstab( const char * device,int pos )
{
	string_t options = StringVoid ;
	string_t entry = StringVoid;	
	string_t fstab = StringGetFromFile( "/etc/fstab" );
	
	stringList_t fstabList  ;
	stringList_t entryList  ;
	
	size_t i ;
	size_t j ;
	
	int st ;
	
	if( fstab == StringVoid )
		return StringVoid ;
	
	fstabList = StringListStringSplit( fstab,'\n' ) ;
	
	StringDelete( &fstab ) ;
	
	if( fstabList == StringListVoid )
		return StringListVoid ;
		
	j = StringListSize( fstabList ) ;
	
	for( i = 0 ; i < j ; i++ ){		
		
		entry = StringListStringAt( fstabList,i ) ;		
		entryList = StringListStringSplit( entry,' ' ) ;
		
		if( entryList == StringListVoid )
			continue ;
		
		entry = StringListStringAt( entryList,0 ) ;
		
		entry = resolveUUIDAndLabel( entry ) ;

		st = StringEqual( entry,device ) ;
		
		StringDelete( &entry ) ;
		
		if( st == 1 ){			
			options = StringListDetachAt( entryList,pos ) ;
			StringListDelete( &entryList ) ;
			break ;			
		}else{
			StringListDelete( &entryList ) ;			
		}
	}		
	
	StringListDelete( &fstabList ) ;
	
	return options ;	
}

static int ms_family( const char * fs )
{
	if( strcmp( fs,"ntfs" ) == 0 || strcmp( fs,"vfat" ) == 0 || strcmp( fs,"fat" ) == 0 || strcmp( fs,"msdos" ) == 0 || strcmp( fs,"umsdos" ) == 0 )
		return 1 ;
	else
		return 0 ;		
}

static int other_fs( const char * fs )
{
	if( strcmp( fs,"affs" ) == 0 || strcmp( fs,"hfs" ) == 0 || strcmp( fs,"iso9660" ) == 0 )
		return 1 ;
	else
		return 0 ;
}

static string_t set_mount_options( m_struct * mst )
{
	string_t opt = zuluCryptGetMountOptionsFromFstab( mst->device,3 ) ;
	string_t uid = StringIntToString( mst->uid );		
	
	if( opt == StringVoid ){
		opt = String( mst->mode ) ;
		StringAppend( opt,"," ) ;
	}else{
		StringMultiplePrepend( opt,",",mst->mode,'\0' ) ;
	}
	
	if( ms_family( mst->fs ) ){	
		if( !StringContains( opt,"dmask=" ) )
			StringAppend( opt,",dmask=0000" ) ;
		if( !StringContains( opt,"umask=" ) )
			StringAppend( opt,",umask=0000" ) ;
		if( !StringContains( opt,"uid=" ) )
			StringAppend( opt,",uid=UID" ) ;
		if( !StringContains( opt,"gid=" ) )
			StringAppend( opt,",gid=UID" ) ;			
		if( !StringContains( opt,"fmask=" ) )
			StringAppend( opt,",fmask=0000" ) ;
		if( strcmp( mst->fs,"vfat" ) ){
			if( !StringContains( opt,"flush" ) )
				StringAppend( opt,",flush" ) ;
			if( !StringContains( opt,"shortname=" ) )
				StringAppend( opt,",shortname=mixed" ) ;			
		}
		
	}else if( other_fs( mst->fs ) ){ 
		
		if( !StringContains( opt,"uid=" ) )
			StringAppend( opt,"uid=UID" ) ;
		if( !StringContains( opt,"gid=" ) )
			StringAppend( opt,"gid=UID" ) ;			
	}else{
		
		/*
		 * ext file systems and raiserfs among others go here
		 * we dont set any options for them.
		 */
		;
	}
	
	StringReplaceString( opt,"UID",StringContent( uid ) );
	StringReplaceString( opt,",,","," );
	
	StringRemoveString( opt,"user" ) ;
	StringRemoveString( opt,"users" ) ;
	StringRemoveString( opt,"default" ) ;
		
	if( StringEndsWithChar( opt,',' ) )
		StringSubChar( opt,StringLength( opt ) - 1,'\0' ) ;		
	
	StringDelete( &uid ) ;
	
	if( strcmp( mst->fs,"ntfs" ) == 0 )		
		mst->opts = StringContent( opt ) ;
	else
		mst->opts = StringContent( opt ) + 3 ;
	
	return opt;	
}

static int mount_ntfs( const m_struct * mst )
{
	int status ;	
	
	process_t p = Process( ZULUCRYPTmount ) ;
	
	ProcessSetArgumentList( p,"-t","ntfs-3g","-o",mst->opts,mst->device,mst->m_point,'\0' ) ;

	ProcessSetOption( p,CLOSE_BOTH_STD_OUT ) ;

	ProcessStart( p ) ;
	
	status = ProcessExitStatus( p ) ; 
	
	ProcessDelete( &p ) ;
	
	return status ;
}

static int mount_mapper( const m_struct * mst )
{
	int h = mount( mst->device,mst->m_point,mst->fs,mst->m_flags,mst->opts ) ;
	
	if( h == 0 && mst->m_flags != MS_RDONLY && ms_family( mst->fs ) == 0 && other_fs( mst->fs ) == 0 )
		chmod( mst->m_point,S_IRWXU|S_IRWXG|S_IRWXO ) ;	
	
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
	string_t options = StringVoid ;
	string_t fs = StringVoid ;
	
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
	
	mst.fs = StringContent( fs ) ;
	
	options = set_mount_options( &mst ) ;
	
	/*
	 * Currently, i dont know how to use mount system call to use ntfs-3g instead of ntfs to mount ntfs file systems.
	 * Use fork to use mount executable as a temporary solution.
	*/
	if( StringEqual( fs,"ntfs" ) ){
		h = mount_ntfs( &mst ) ;
		StringMultipleDelete( &fs,&options,'\0' ) ;		
		switch( h ){
			case 0  : return 0 ;
			case 16 : return 12 ;
			default : return 1 ;
		}
	}
	 
	/*
	 * mtab_is_at_etc() is defined in print_mounted_volumes.c
	 * 1 is return if "mtab" is found to be a file located at "/etc/"
	 * 0 is returned otherwise,probably because "mtab" is a soft like to "/proc/mounts"
	 */
	
	if( !zuluCryptMtabIsAtEtc() ){
		h = mount_mapper( &mst ) ;
	}else{
		/* 
		 * "/etc/mtab" is not a symbolic link to /proc/mounts, manually,add an entry to it since 
		 * mount API does not
		 */		
		m_lock = mnt_new_lock( "/etc/mtab~",getpid() ) ;
		if( mnt_lock_file( m_lock ) != 0 ){
			h = 12 ;
		}else{		
			h = mount_mapper( &mst ) ;
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

