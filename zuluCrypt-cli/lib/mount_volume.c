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
#include <blkid/blkid.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define FAT_FAMILY_FS 1
#define OTHER_FAMILY_FS 2

/*
 * These functions are moved to parse_fstab.c
 * 
 * string_t zuluCryptGetFstabEntry( const char * device )
 * string_t zuluCryptGetMountOptionsFromFstab( const char * device,int pos )
 * stringList_t zuluCryptGetFstabEntryList( const char * device ) 
 */

static inline int zuluExit( int st,int fd,stringList_t stl )
{
	StringListDelete( &stl ) ;
	if( fd != -1 ){
		close( fd ) ;
	}
	return st ;
}

static inline int fs_family( const char * fs )
{
	if( StringAtLeastOneMatch_1( fs,"ntfs,","vfat","fat","msdos","umsdos",NULL ) ){
		return 1 ;
	}else if( StringAtLeastOneMatch_1( fs,"affs","hfs",NULL ) ){
		return 2 ;
	}else if( StringAtLeastOneMatch_1( fs,"iso9660","udf","squashfs",NULL ) ){
		return 3 ;
	}else{
		return 0 ;
	}
}

static inline string_t set_mount_options( m_struct * mst )
{
	/*
	 * zuluCryptGetMountOptionsFromFstab() is defined in parse_fstab.c
	 */
	string_t opt = zuluCryptGetMountOptionsFromFstab( mst->device,MOUNTOPTIONS,mst->uid ) ;
	
	int fsFamily = fs_family( mst->fs ) ;
	
	if( opt == StringVoid ){
		opt = String( "" ) ;
		StringAppend( opt,mst->fs_flags ) ;
	}else{
		if( StringContains( opt,"ro" ) ){
			mst->m_flags |= MS_RDONLY ;
		}
		StringMultipleAppend( opt,",",mst->fs_flags,END ) ;
	}
	
	if( fsFamily == 1 ){
		if( !StringContains( opt,"dmask=" ) ){
			StringAppend( opt,",dmask=0000" ) ;
		}
		if( !StringContains( opt,"umask=" ) ){
			StringAppend( opt,",umask=0000" ) ;
		}
		if( !StringContains( opt,"uid=" ) ){
			StringAppend( opt,",uid=" ) ;
			StringAppendInt( opt,mst->uid ) ;
		}
		if( !StringContains( opt,"gid=" ) ){
			StringAppend( opt,",gid=" ) ;
			StringAppendInt( opt,mst->uid ) ;
		}
		if( !StringContains( opt,"fmask=" ) ){
			StringAppend( opt,",fmask=0111" ) ;
		}
		if( StringsAreEqual( mst->fs,"vfat" ) ){
			if( !StringContains( opt,"flush" ) ){
				StringAppend( opt,",flush" ) ;
			}
			if( !StringContains( opt,"shortname=" ) ){
				StringAppend( opt,",shortname=mixed" ) ;
			}
		}
	}else if( fsFamily == 2 ){
		if( !StringContains( opt,"uid=" ) ){
			StringAppend( opt,",uid=" ) ;
			StringAppendInt( opt,mst->uid ) ;
		}
		if( !StringContains( opt,"gid=" ) ){
			StringAppend( opt,",gid=" ) ;
			StringAppendInt( opt,mst->uid ) ;
		}
	}else if( fsFamily == 3 ){
		mst->m_flags |= MS_RDONLY ;
	}else{
		/*
		 * ext file systems and raiserfs among others go here
		 * we dont set any options for them.
		 */
		;
	}
	
	/*
	 * Below options are not file system options and are rejectected by mount() command and hence we are removing them.
	 */
	StringRemoveString( opt,"nouser" ) ;
	StringRemoveString( opt,"users" ) ;
	StringRemoveString( opt,"user" ) ;
	StringRemoveString( opt,"defaults" ) ;
	StringRemoveString( opt,"noauto" ) ;
	StringRemoveString( opt,"auto" ) ;
	
	/*
	 * remove below two now because we are going to add them below,reason for removing them 
	 * and readding them is because we want to make sure they are at the beginning of the string
	 */
	StringRemoveString( opt,"ro" ) ;
	StringRemoveString( opt,"rw" ) ;
	
	if( mst->m_flags & MS_RDONLY ){
		StringPrepend( opt,"ro," ) ;
	}else{
		StringPrepend( opt,"rw," ) ;
	}
	
	while( StringIndexOfString( opt,0,",," ) >= 0 ){
		StringReplaceString( opt,",,","," );
	}
	if( StringEndsWithChar( opt,',' ) ){
		StringRemoveRight( opt,1 ) ;
	}
	mst->opts = StringContent( opt ) ;
	return opt;
}

static void _mount_options( unsigned long flags,string_t * xt )
{	
	string_t st = *xt ;
	
	if( flags & MS_NODEV ){
		StringAppend( st,",nodev" ) ; 
	}
	if( flags & MS_NOEXEC ){
		StringAppend( st,",noexec" ) ;
	}
	if( flags & MS_NOSUID ){
		StringAppend( st,",nosuid" ) ;
	}
	if( flags & MS_BIND ){
		StringAppend( st,",bind" ) ;
	}
	if( flags & MS_MANDLOCK ){
		StringAppend( st,",mandlock" ) ;
	}
	if( flags & MS_MOVE ){
		StringAppend( st,",move" ) ;
	}
	if( flags & MS_NOATIME ){
		StringAppend( st,",noatime" ) ;
	}
	if( flags & MS_NODIRATIME ){
		StringAppend( st,",nodiratime" ) ;
	}
	if( flags & MS_RELATIME  ){
		StringAppend( st,",relatime" ) ;
	}
	if( flags & MS_REMOUNT ){
		StringAppend( st,",remount" ) ;
	}
	if( flags & MS_SILENT ){
		StringAppend( st,",silent" ) ;
	}
	if( flags & MS_STRICTATIME ){
		StringAppend( st,",strictatime" ) ;
	}
	if( flags & MS_SYNCHRONOUS ){
		StringAppend( st,",synchronous" ) ;
	}
}

static inline int mount_ntfs( m_struct * mst )
{
	int status ;
	const char * opts ;
	
	process_t p = Process( ZULUCRYPTmount ) ;
	string_t st = set_mount_options( mst ) ;
	
	_mount_options( mst->m_flags,&st ) ;
	opts = StringReplaceString( st,",,","," ) ;
	
	ProcessSetArgumentList( p,"-n","-t","ntfs-3g","-o",opts,mst->device,mst->m_point,ENDLIST ) ;
	ProcessStart( p ) ;
	
	status = ProcessExitStatus( p ) ;
	
	ProcessDelete( &p ) ;
	StringDelete( &st ) ;
	
	return status ;
}

static inline int mount_volume( const m_struct * mst )
{
	int h = mount( mst->device,mst->m_point,mst->fs,mst->m_flags,mst->opts + 3 ) ;
	if( h == 0 && mst->m_flags != MS_RDONLY ){
		chmod( mst->m_point,S_IRWXU|S_IRWXG|S_IRWXO ) ;
	}
	return h;
}

string_t zuluCryptGetFileSystemFromDevice( const char * device )
{
	string_t st = StringVoid ;
	const char * cf = NULL ;
	blkid_probe blkid = blkid_new_probe_from_filename( device ) ;
	if( blkid != NULL ){
		blkid_do_probe( blkid );
		blkid_probe_lookup_value( blkid,"TYPE",&cf,NULL ) ;
		st = String( cf ) ;
		blkid_free_probe( blkid );
	}
	return st ;
}

const char * zuluCryptDecodeMountEntry( string_t st )
{
	StringReplaceString( st,"\\012","\n" ) ;
	StringReplaceString( st,"\\040"," " ) ;
	StringReplaceString( st,"\\134","\\" ) ;
	return StringReplaceString( st,"\\011","\\t" ) ;
}

int zuluCryptMountVolume( const char * path,const char * m_point,unsigned long mount_opts,const char * fs_opts,uid_t uid )
{
	int h ;
	
	stringList_t stl = StringListInit() ;
	
	string_t * opts ;
	string_t fs ;
	string_t * loop ;
	int fd = -1 ;
	m_struct mst ;
	char * e ;
		
	/*
	 * device and original_device are initially the same but
	 * device may change to something like /dev/loop1 if device to be mounted is a file
	 */
	mst.device = path ;
	mst.original_device = path  ;
	
	mst.m_point = m_point ;
	mst.uid = uid ;
	
	mst.m_flags = mount_opts ;
	opts = StringListAssign( stl ) ;
		
	/* 
	 * zuluCryptGetFileSystemFromDevice() is defined in this source file
	 */
	fs = StringListAssignString( stl,zuluCryptGetFileSystemFromDevice( path ) );
	
	if( fs == StringVoid ){
		/*
		 * failed to read file system,probably because the volume does have any or 
		 * a plain volume was opened with a wrong key
		 */
		return zuluExit( 4,fd,stl ) ;
	}
	
	if( StringEqual( fs,"crypto_LUKS" ) ){
		/*
		 * we cant mount an encrypted volume, exiting
		 */
		return zuluExit( 4,fd,stl ) ;
	}

	/*
	 * zuluCryptMountHasNotAllowedFileSystemOptions() is defined in ./mount_fs_options.c
	 */
	if( zuluCryptMountHasNotAllowedFileSystemOptions( uid,fs_opts,fs ) ){
		return zuluExit( -1,fd,stl ) ;
	}
	mst.fs_flags = fs_opts ;
	mst.fs = StringContent( fs ) ;
	opts = StringListAssign( stl ) ;
	*opts = set_mount_options( &mst ) ;
	
	if( !StringPrefixMatch( path,"/dev/",5 ) ){
		loop = StringListAssign( stl ) ;
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in ./create_loop_device.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( mst.device,O_RDWR,&fd,loop ) ){
			mst.device = StringContent( *loop ) ;
		}else{
			return zuluExit( -1,fd,stl ) ;
		}
	}
	
	if( StringPrefixMatch( mst.device,"/dev/loop",9 ) ){
		loop = StringListAssign( stl ) ;
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ./create_loop_device.c
		 */
		e = zuluCryptLoopDeviceAddress( mst.device ) ;
		*loop = StringInherit( &e ) ;
		mst.original_device = StringContent( *loop ) ;
		if( mst.original_device == NULL ){
			return zuluExit( -1,fd,stl ) ;
		}
	}
		
	if( StringEqual( fs,"ntfs" ) ){
		/*
		 * Currently, i dont know how to use mount system call to use ntfs-3g instead of ntfs to mount ntfs file systems.
		 * use mount executable as a temporary solution.
		 */
		switch( mount_ntfs( &mst ) ){
			case 0  : return zuluExit( 0,fd,stl )  ;
			case 16 : return zuluExit( 12,fd,stl ) ;
			default : return zuluExit( 1,fd,stl )  ;
		}
	}else{
		h = mount_volume( &mst ) ;
	}

	return zuluExit( h,fd,stl ) ;
}
