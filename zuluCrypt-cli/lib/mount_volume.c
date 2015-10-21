/*
 *
 *  Copyright (c) 2011-2015
 *  name : Francis Banyikwa
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

/*
 * These functions are moved to parse_fstab.c
 *
 * string_t zuluCryptGetFstabEntry( const char * device )
 * string_t zuluCryptGetMountOptionsFromFstab( const char * device,int pos )
 * stringList_t zuluCryptGetFstabEntryList( const char * device )
 */

typedef enum { FAT_FAMILY_FS,OTHER_FAMILY_FS,READ_ONLY_FS,DEFAULT_FS_TYPE } FS_TYPE ;

static int zuluExit( int st,int fd,string_t x,string_t y,string_t z )
{
	StringMultipleDelete( &x,&y,&z,NULL ) ;
	if( fd != -1 ){
		close( fd ) ;
	}
	return st ;
}

static FS_TYPE fs_family( const char * fs )
{
	if( StringAtLeastOneMatch_1( fs,"ntfs","vfat","fat","msdos","umsdos","exfat",NULL ) ){

		return FAT_FAMILY_FS ;

	}else if( StringAtLeastOneMatch_1( fs,"affs","hfs",NULL ) ){

		return OTHER_FAMILY_FS ;

	}else if( StringAtLeastOneMatch_1( fs,"iso9660","udf","squashfs",NULL ) ){

		return READ_ONLY_FS ;
	}else{
		return DEFAULT_FS_TYPE ;
	}
}

/*
 * custom options per file system
 */
static void _get_file_system_options_from_config_file_1( const char * fs,string_t st )
{
	const char * e ;

	StringListIterator it  ;
	StringListIterator end ;

	string_t zt ;
	string_t xt = StringGetFromFile( "/etc/zuluCrypt/generic_fs_options" ) ;

	stringList_t stl = StringListStringSplit( xt,'\n' ) ;

	stringList_t stz ;

	StringDelete( &xt ) ;

	StringListGetIterators( stl,&it,&end ) ;

	while( it != end  ){

		zt = *it ;

		it++ ;

		if( StringStartsWith( zt,fs ) ){

			stz = StringListStringSplit( zt,' ' ) ;

			e = StringListContentAtSecondPlace( stz ) ;

			StringMultipleAppend( st,",",e,NULL ) ;

			StringListDelete( &stz ) ;

			break ;
		}
	}

	StringListDelete( &stl ) ;
}

/*
 * custom options per volume set through file system's UUID
 */
static void _get_file_system_options_from_config_file( const char * device,string_t st )
{
	char * f ;
	const char * e ;

	StringListIterator it  ;
	StringListIterator end ;

	string_t xt = StringGetFromFile( "/etc/zuluCrypt/fs_options" ) ;

	stringList_t stl = StringListStringSplit( xt,'\n' ) ;

	stringList_t stz ;

	StringDelete( &xt ) ;

	/*
	 * zuluCryptUUIDFromPath_1() is defined in resolve_paths.c
	 */
	f = zuluCryptUUIDFromPath_1( device ) ;

	StringListGetIterators( stl,&it,&end ) ;

	while( it != end  ){

		e = StringRemoveString( *it,"\"" ) ;

		it++ ;

		if( StringPrefixMatch( e,"UUID=",5 ) ){

			if( StringPrefixEqual( e + 5,f ) ){

				stz = StringListSplit( e,' ' ) ;

				e = StringListContentAtSecondPlace( stz ) ;

				StringMultipleAppend( st,",",e,NULL ) ;

				StringListDelete( &stz ) ;

				break ;
			}
		}
	}

	StringListDelete( &stl ) ;
	StringFree( f ) ;
}

static const char * _remove_duplicates( string_t st )
{
	const char ** z = StringPointer( st ) ;

	while( StringHasComponent( *z,",," ) ){

		StringReplaceString( st,",,","," ) ;
	}

	if( StringEndsWithChar( st,',' ) ){

		return StringRemoveRight( st,1 ) ;
	}else{
		return StringContent( st ) ;
	}
}

static string_t set_mount_options( m_struct * mst )
{
	/*
	 * zuluCryptGetMountOptionsFromFstab() is defined in parse_fstab.c
	 */
	string_t opt = zuluCryptGetMountOptionsFromFstab( mst->device,MOUNTOPTIONS,mst->uid ) ;

	FS_TYPE fsFamily = fs_family( mst->fs ) ;

	const char * f[] = { "nouser","users","user","defaults","noauto","auto","nodev","dev",
		"noexec","exec","nosuid","suid","bind","mandlock","move","noatime","nodiratime","remount","silent",
		"synchronous",NULL } ;

	const char ** z = f ;
	const char * e ;

	if( opt == StringVoid ){

		if( mst->fs_flags != NULL ){

			opt = String( mst->fs_flags ) ;
		}else{
			opt = StringEmpty() ;
		}
	}else{
		if( StringContains( opt,"ro" ) ){

			mst->m_flags |= MS_RDONLY ;
		}

		StringMultipleAppend( opt,",",mst->fs_flags,NULL ) ;
	}

	_get_file_system_options_from_config_file_1( mst->fs,opt ) ;

	_get_file_system_options_from_config_file( mst->device,opt ) ;

	if( fsFamily == FAT_FAMILY_FS ){

		if( StringDoesNotContain( opt,"dmask=" ) ){

			StringAppend( opt,",dmask=0000" ) ;
		}
		if( StringDoesNotContain( opt,"umask=" ) ){

			StringAppend( opt,",umask=0000" ) ;
		}
		if( StringDoesNotContain( opt,"uid=" ) ){

			StringAppend( opt,",uid=" ) ;
			StringAppendInt( opt,mst->uid ) ;
		}
		if( StringDoesNotContain( opt,"gid=" ) ){

			StringAppend( opt,",gid=" ) ;
			StringAppendInt( opt,mst->uid ) ;
		}
		if( StringDoesNotContain( opt,"fmask=" ) ){

			StringAppend( opt,",fmask=0111" ) ;
		}
		if( StringsAreEqual( mst->fs,"vfat" ) ){

			if( StringDoesNotContain( opt,"flush" ) ){

				StringAppend( opt,",flush" ) ;
			}
			if( StringDoesNotContain( opt,"shortname=" ) ){

				StringAppend( opt,",shortname=mixed" ) ;
			}
		}
	}else if( fsFamily == OTHER_FAMILY_FS ){

		if( StringDoesNotContain( opt,"uid=" ) ){

			StringAppend( opt,",uid=" ) ;
			StringAppendInt( opt,mst->uid ) ;
		}
		if( StringDoesNotContain( opt,"gid=" ) ){

			StringAppend( opt,",gid=" ) ;
			StringAppendInt( opt,mst->uid ) ;
		}
	}else if( fsFamily == READ_ONLY_FS ){

		mst->m_flags |= MS_RDONLY ;
	}else{
		/*
		 * ext file systems and raiserfs among others go here
		 * we dont set any options for them.
		 */
		;
	}

	/*
	 * remove mount options to leave only file system options
	 */
	while( 1 ){
		e = *z ;
		z++ ;
		if( e == NULL ){
			break ;
		}else{
			StringRemoveString( opt,e ) ;
		}
	}
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

	mst->opts = _remove_duplicates( opt ) ;
	return opt ;
}

static const char * _mount_options( unsigned long flags,string_t st )
{
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

	return _remove_duplicates( st ) ;
}

static int _mount_FUSEfs( const m_struct * mst,string_t st )
{
	const char * opts = _mount_options( mst->m_flags,st ) ;

	char * const env[ 2 ] = { "PATH=/bin:/usr/bin:/sbin:/usr/sbin",NULL } ;

	process_t p = Process( ZULUCRYPTmount,NULL ) ;

	ProcessSetEnvironmentalVariable( p,env ) ;

	if( StringsAreEqual( mst->fs,"ntfs" ) ){

		if( StringHasComponent( opts,"ignore_case" ) ){

			ProcessSetArgumentList( p,"-n","-t","lowntfs-3g","-o",opts,mst->device,mst->m_point,NULL ) ;
		}else{
			ProcessSetArgumentList( p,"-n","-t","ntfs-3g","-o",opts,mst->device,mst->m_point,NULL ) ;
		}
	}else{
		ProcessSetArgumentList( p,"-t",mst->fs,"-o",opts,mst->device,mst->m_point,NULL ) ;
	}

	ProcessStart( p ) ;

	return ProcessWaitUntilFinished( &p ) ;
}

static int _mount( const m_struct * mst,string_t st )
{
	int h = mount( mst->device,mst->m_point,mst->fs,mst->m_flags,mst->opts + 3 ) ;

	if( st ){;}

	if( h == 0 && mst->m_flags != MS_RDONLY ){

		chmod( mst->m_point,S_IRWXU|S_IRWXG|S_IRWXO ) ;
	}

	return h ;
}

static int _mount_volume( int ( *function )( const m_struct *,string_t ),
			  const m_struct * m,string_t st )
{
	int e = function( m,st ) ;

	if( e != 0 ){

		/*
		 * mount failed for some reason,wait 2 second and then try again
		 */
		sleep( 2 ) ;

		e = function( m,st ) ;
	}

	return e ;
}

const char * zuluCryptDecodeMountEntry( string_t st )
{
	StringReplaceString( st,"\\012","\n" ) ;
	StringReplaceString( st,"\\040"," " ) ;
	StringReplaceString( st,"\\134","\\" ) ;
	return StringReplaceString( st,"\\011","\\t" ) ;
}

const char * zuluCryptEncodeMountEntry( string_t st )
{
	StringReplaceString( st,"\\","\\134" ) ;
	StringReplaceString( st,"\n","\\012" ) ;
	StringReplaceString( st," ","\\040" ) ;
	return StringReplaceString( st,"\\t","\\011" ) ;
}

int zuluCryptMountVolume( const char * path,const char * m_point,unsigned long mount_opts,const char * fs_opts,uid_t uid )
{
	int h ;

	string_t opts = StringVoid ;
	string_t fs   = StringVoid ;
	string_t loop = StringVoid ;

	int fd = -1 ;

	m_struct mst ;
	mst.device = path ;
	mst.m_point = m_point ;
	mst.uid = uid ;
	mst.m_flags = mount_opts ;

	/*
	 * zuluCryptGetFileSystemFromDevice() is defined in blkid_evaluate_tag.c
	 */
	fs = zuluCryptGetFileSystemFromDevice( path ) ;

	if( fs == StringVoid ){
		/*
		 * failed to read file system,probably because the volume does have any or
		 * a plain volume was opened with a wrong key
		 */
		return zuluExit( 4,fd,opts,fs,loop ) ;
	}

	if( StringStartsWith( fs,"crypto" ) ){
		/*
		 * we cant mount an encrypted volume, exiting
		 */
		return zuluExit( 4,fd,opts,fs,loop ) ;
	}

	/*
	 * zuluCryptMountHasNotAllowedFileSystemOptions() is defined in ./mount_fs_options.c
	 */
	if( zuluCryptMountHasNotAllowedFileSystemOptions( uid,fs_opts,fs ) ){
		return zuluExit( -1,fd,opts,fs,loop ) ;
	}

	mst.fs_flags = fs_opts ;
	mst.fs = StringContent( fs ) ;
	opts = set_mount_options( &mst ) ;

	if( StringPrefixNotEqual( path,"/dev/" ) ){
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in ./create_loop_device.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( mst.device,O_RDWR,&fd,&loop ) ){
			mst.device = StringContent( loop ) ;
		}else{
			return zuluExit( -1,fd,opts,fs,loop ) ;
		}
	}

	/*
	 * zuluCryptFileSystemIsFUSEbased() is defined in blkid_evaluate_tag.c
	 */
	if( zuluCryptFileSystemIsFUSEbased( path ) ){

		/*
		 * These file systems dont see to work with mount() command for some reason.
		 * Them being FUSE based could be a reason.
		 */

		switch( _mount_volume( _mount_FUSEfs,&mst,opts ) ){

			case 0  : return zuluExit( 0,fd,opts,fs,loop )  ;
			case 16 : return zuluExit( 12,fd,opts,fs,loop ) ;
			default : return zuluExit( 1,fd,opts,fs,loop )  ;
		}
	}else{
		h = _mount_volume( _mount,&mst,opts ) ;
	}

	return zuluExit( h,fd,opts,fs,loop ) ;
}
