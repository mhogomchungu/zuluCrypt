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
#include "libmount_header.h"

#define FAT_FAMILY_FS 1
#define OTHER_FAMILY_FS 2

typedef struct{
	const char * original_device ;
	const char * device ;
	const char * m_point ;
	const char * fs ;
	const char * opts ;
	uid_t uid ;
	unsigned long m_flags ;
	const char * fs_flags ;
}m_struct;

static inline int zuluExit( int st,stringList_t stl )
{
	StringListDelete( &stl ) ;
	return st ;
}

static inline char * _evaluate_tag( const char * tag,const char * entry,blkid_cache * cache )
{
	char * f = NULL ;
	string_t st = String( entry ) ;
	int index = StringIndexOfChar( st,0,' ' ) ;
	if( index >= 0 ){
		f = blkid_evaluate_tag( tag,StringSubChar( st,index,'\0' ),cache ) ;
	}
	StringDelete( &st ) ;
	return f ;
}

static inline char * _evaluate_tag_by_id( string_t st )
{
	char * r = NULL ;
	ssize_t index = StringIndexOfChar( st,0,' ' ) ;
	if( index >= 0 ){
		r = zuluCryptRealPath( StringSubChar( st,index,'\0' ) ) ;
		StringSubChar( st,index,' ' ) ;
	}
	return r ;
}

string_t zuluCryptGetFstabEntry( const char * device )
{
	string_t xt = StringGetFromFile( "/etc/fstab" );
	
	stringList_t fstabList  ;
	
	StringListIterator it  ;
	StringListIterator end ;
	
	ssize_t index ;
	
	char * ac ;
	const char * entry ;
	
	size_t len = strlen( device ) ;
	
	blkid_cache cache = NULL ;

	if( xt == StringVoid )
		return StringVoid ;
	
	fstabList = StringListStringSplit( xt,'\n' ) ;
	
	StringDelete( &xt ) ;
	
	if( fstabList == StringListVoid )
		return StringVoid ;
		
	it  = StringListBegin( fstabList ) ;
	end = StringListEnd( fstabList ) ;
	
	if( blkid_get_cache( &cache,NULL ) != 0 )
		cache = NULL ;
	
	for( ; it != end ; it++ ){
		entry = StringContent( *it ) ;
		if( strncmp( entry,"/dev/",5 ) == 0 ){
			if( strncmp( entry,"/dev/root",9 ) == 0 ){
				/*
				 * zuluCryptResolveDevRoot() is defined in ./print_mounted_volumes.c 
				 */
				ac =  zuluCryptResolveDevRoot() ;
				if( ac != NULL ){
					if( strncmp( ac,device,len ) == 0 ){
						xt = StringCopy( *it ) ;
						StringReplaceString( xt,"/dev/root",ac ) ;
						free( ac ) ;
						break ;
					}else{
						free( ac ) ;
					}
				}
			}else if( strncmp( entry,"/dev/disk/by",12 ) == 0 ){
				ac = _evaluate_tag_by_id( *it ) ;
				if( ac != NULL ){
					if( strncmp( ac,device,len ) == 0 ){
						xt = StringCopy( *it ) ;
						index = StringIndexOfChar( xt,0,' ' ) ;
						if( index >= 0 ){
							StringRemoveLeft( xt,index ) ;
							StringPrepend( xt,ac ) ;
						}
						free( ac ) ;
						break ;
					}else{
						free( ac ) ;
					}
				}
			}else{
				if( strncmp( entry,device,len ) == 0 ){
					xt = StringCopy( *it ) ;
					break ;
				}
			}
		}else if( strncmp( entry,"UUID=",5 ) == 0 ){
			entry = StringRemoveString( *it,"\"" ) ;
			ac = _evaluate_tag( "UUID",entry + 5,&cache ) ;
			if( ac != NULL ){
				if( strncmp( ac,device,len ) == 0 ){
					xt = StringCopy( *it ) ;
					index = StringIndexOfChar( xt,0,' ' ) ;
					if( index >= 0 ){
						StringRemoveLeft( xt,index ) ;
						StringPrepend( xt,ac ) ;
					}
					free( ac ) ;
					break ;
				}else{
					free( ac ) ;
				}
			}
		}else if( strncmp( entry,"LABEL=",6 ) == 0 ){
			entry = StringRemoveString( *it,"\"" ) ;
			ac = _evaluate_tag( "LABEL",entry + 6,&cache ) ;
			if( ac != NULL ){
				if( strncmp( ac,device,len ) == 0 ){
					xt = StringCopy( *it ) ;
					index = StringIndexOfChar( xt,0,' ' ) ;
					if( index >= 0 ){
						StringRemoveLeft( xt,index ) ;
						StringPrepend( xt,ac ) ;
					}
					free( ac ) ;
					break ;
				}else{
					free( ac ) ;
				}
			}
		}else if( entry[ 0 ] == '/' ){
			if( StringStartsWith( *it,device ) ){
				xt = StringCopy( *it ) ;
				break ;
			}
		}else{
			continue ;
		}
	}
	
	if( cache != NULL )
		blkid_put_cache( cache ) ;
	StringListDelete( &fstabList ) ;
	return xt ;
}

string_t zuluCryptGetMountOptionsFromFstab( const char * device,int pos )
{
	stringList_t stl ;
	string_t st = zuluCryptGetFstabEntry( device ) ;
	stl = StringListStringSplit( st,' ' ) ;
	StringDelete( &st ) ;
	st = StringListCopyStringAt( stl,pos ) ;
	StringListDelete( &stl ) ;
	return st ;
}

stringList_t zuluCryptGetFstabEntryList( const char * device )
{
	stringList_t stl ;
	string_t st = zuluCryptGetFstabEntry( device ) ;
	stl = StringListStringSplit( st,' ' ) ;
	StringDelete( &st ) ;
	return stl ;
}

static inline int fs_family( const char * fs )
{
	if(     strcmp( fs,"ntfs" ) == 0  || 
		strcmp( fs,"vfat" ) == 0  || 
		strcmp( fs,"fat" ) == 0   ||
		strcmp( fs,"msdos" ) == 0 ||
		strcmp( fs,"umsdos" ) == 0 )
		return 1 ;
	
	if( strcmp( fs,"affs" ) == 0 || strcmp( fs,"hfs" ) == 0 )
		return 2 ;
	
	if( strcmp( fs,"iso9660" ) == 0 || strcmp( fs,"udf" ) == 0 )
		return 3 ;
	
	return 0 ;
}

static inline string_t set_mount_options( m_struct * mst )
{
	string_t opt = zuluCryptGetMountOptionsFromFstab( mst->device,MOUNTOPTIONS ) ;
	
	if( opt == StringVoid ){
		opt = String( "" ) ;
	}else{
		if( StringContains( opt,"ro" ) ){
			mst->m_flags |= MS_RDONLY ;
		}
		StringMultipleAppend( opt,",",mst->fs_flags,END ) ;
	}	
	
	if( fs_family( mst->fs ) == 1 ){
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
		
		if( strcmp( mst->fs,"vfat" ) == 0 ){
			if( !StringContains( opt,"flush" ) ){
				StringAppend( opt,",flush" ) ;
			}
			if( !StringContains( opt,"shortname=" ) ){
				StringAppend( opt,",shortname=mixed" ) ;
			}
		}
		
	}else if( fs_family( mst->fs ) == 2 ){
		if( !StringContains( opt,"uid=" ) ){
			StringAppend( opt,",uid=" ) ;
			StringAppendInt( opt,mst->uid ) ;
		}
		if( !StringContains( opt,"gid=" ) ){
			StringAppend( opt,",gid=" ) ;
			StringAppendInt( opt,mst->uid ) ;
		}
	}else if( fs_family( mst->fs ) == 3 ){
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
	
	while( StringIndexOfString( opt,0,",," ) >= 0 )
		StringReplaceString( opt,",,","," );
	
	if( StringEndsWithChar( opt,',' ) )
		StringRemoveRight( opt,1 ) ;
	
	mst->opts = StringContent( opt ) ;
	return opt;
}

static string_t _mount_options( unsigned long flags,string_t * xt )
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
	
	return st ;
}

static inline int paths_are_sane( const char * device,const char * original_device __attribute__((unused)),const char * m_point,uid_t uid )
{
	/*
	 * in this function,we are checking if the paths we are about to pass to mount() are
	 * paths we expect.
	 */
	struct stat st ;
	if( device ){;}
	if( original_device ){;}
	
	if( chdir( m_point ) != 0 )
		return 0 ;
	if( stat( ".",&st ) != 0 )
		return 0 ;
	if( uid != st.st_uid )
		return 0 ;
	return 1 ;
}

static inline int mount_is_were_we_expect_it_to_be( const m_struct * mst,int h )
{
	/*
	 * in this function,we are checking if the volume is mounted where we expect it to be by comparing
	 * paths we presented to mount() and paths that showed up in /proc/self/mountinfo 
	 */
	char * e = NULL ;
	char * f ;
	int result = -1 ;
	
	if( h != 0 )
		return h ;
		
	/*
	 * zuluCryptGetMountPointFromPath() is defined in ./print_mounted_volumes.c
	 */
	f = zuluCryptGetMountPointFromPath( mst->original_device ) ;
	if( f == NULL ){
		zuluCryptUnmountVolume( mst->original_device,&e ) ;
		if( e != NULL ){
			/*
			 * Dont delete the path because we dont know what path we got
			 * rmdir( e ) ;
			 */
			free( e ) ;
		}
	}else{
		if( strcmp( f,mst->m_point ) == 0 ){
			result = 0 ;
		}else{
			/*
			 * mount reported the file system as being mounted but it is not where we expect it to be.Assume shenanigans and unmount
			 */
			zuluCryptUnmountVolume( mst->device,&e ) ;
			if( e != NULL ){
				/*
				 * Dont delete the path because we dont know what path we got
				 * rmdir( e ) ;
				 */				
				free( e ) ;
			}	
		}
		free( f ) ;
	}
	return result ;
}

static inline int mount_ntfs( const m_struct * mst )
{
	int status ;
	process_t p ;
	string_t st ;
	const char * opts ;
	if( !paths_are_sane( mst->device,mst->original_device,mst->m_point,mst->uid ) ) 
		return -1 ;
	p = Process( ZULUCRYPTmount ) ;
	st = String( "" ) ;	
	st = _mount_options( mst->m_flags,&st ) ;
	if( mst->m_flags & MS_RDONLY )
		StringPrepend( st,"ro" ) ;
	else
		StringPrepend( st,"rw" ) ;
	
	opts = StringReplaceString( st,",,","," );
	
	ProcessSetArgumentList( p,"-t","ntfs-3g","-o",opts,mst->device,mst->m_point,ENDLIST ) ;
	ProcessStart( p ) ;
	status = ProcessExitStatus( p ) ; 
	ProcessDelete( &p ) ;
	StringDelete( &st ) ;
	return mount_is_were_we_expect_it_to_be( mst,status ) ;
}

static inline int mount_mapper( const m_struct * mst )
{
	int h = -1;
	
	if( paths_are_sane( mst->device,mst->original_device,mst->m_point,mst->uid ) ){
		h = mount( mst->device,mst->m_point,mst->fs,mst->m_flags,mst->opts + 3 ) ;
		if( h == 0 && mst->m_flags != MS_RDONLY ){
			chmod( mst->m_point,S_IRWXU|S_IRWXG|S_IRWXO ) ;
		}
	}
	
	return mount_is_were_we_expect_it_to_be( mst,h ) ;
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

int zuluCryptMountVolume( const char * path,const char * m_point,unsigned long mount_opts,const char * fs_opts,uid_t uid )
{
	struct mntent mt  ;
	int h ;
	FILE * f ;
#if USE_NEW_LIBMOUNT_API
	struct libmnt_lock * m_lock ;
#else
	mnt_lock * m_lock ;
#endif
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
		 * failed to read file system,probably because the volume does have any or is an encrypted plain volume
		 */
		return zuluExit( 4,stl ) ;
	}
	
	if( StringEqual( fs,"crypto_LUKS" ) ){
		/*
		 * we cant mount an encrypted volume, exiting
		 */
		return zuluExit( 4,stl ) ;
	}

	/*
	 * zuluCryptMountHasNotAllowedFileSystemOptions() is defined in ./mount_fs_options.c
	 */
	if( zuluCryptMountHasNotAllowedFileSystemOptions( uid,fs_opts,fs ) )
		return zuluExit( -1,stl ) ;
	
	mst.fs_flags = fs_opts ;
	mst.fs = StringContent( fs ) ;
	opts = StringListAssign( stl ) ;
	*opts = set_mount_options( &mst ) ;
	
	if( strncmp( path,"/dev/",5 ) != 0 ){
		loop = StringListAssign( stl ) ;
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in ./create_loop_device.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( mst.device,O_RDWR,&fd,loop ) ){
			mst.device = StringContent( *loop ) ;
		}else{
			return zuluExit( -1,stl ) ;
		}
	}
	
	if( strncmp( mst.device,"/dev/loop",9 ) == 0 ){
		loop = StringListAssign( stl ) ;
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ./create_loop_device.c
		 */
		e = zuluCryptLoopDeviceAddress( mst.device ) ;
		*loop = StringInherit( &e ) ;
		mst.original_device = StringContent( *loop ) ;
		if( mst.original_device == NULL )
			return zuluExit( -1,stl ) ;
	}
		
	if( StringEqual( fs,"ntfs" ) ){
		/*
		 * Currently, i dont know how to use mount system call to use ntfs-3g instead of ntfs to mount ntfs file systems.
		 * use mount executable as a temporary solution.
		 */
		switch( mount_ntfs( &mst ) ){
			case 0  : return zuluExit( 0,stl )  ;
			case 16 : return zuluExit( 12,stl ) ;
			default : return zuluExit( 1,stl )  ;
		}
	}
		 
	/*
	 * zuluCryptMtabIsAtEtc() is defined in print_mounted_volumes.c
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
				mt.mnt_dir    = ( char * ) mst.m_point ;
				mt.mnt_type   = ( char * ) mst.fs ;
				
				_mount_options( mst.m_flags,opts ) ;
				
				if( strncmp( mst.device,"/dev/loop",9 ) == 0 ){
					mt.mnt_fsname = ( char * ) mst.original_device ;
					mt.mnt_opts = ( char * ) StringMultipleAppend( *opts,",loop=",mst.device,END ) ;
					if( fd != -1 ){
						close( fd ) ;
					}
				}else{
					mt.mnt_fsname = ( char * ) mst.device ;
					mt.mnt_opts = ( char * ) StringContent( *opts );
				}
				mt.mnt_freq   = 0 ;
				mt.mnt_passno = 0 ;
				addmntent( f,&mt ) ;
				endmntent( f ) ;
			}
			
			mnt_unlock_file( m_lock ) ;
		}

		mnt_free_lock( m_lock ) ;
	}
	
	return zuluExit( h,stl ) ;
}
