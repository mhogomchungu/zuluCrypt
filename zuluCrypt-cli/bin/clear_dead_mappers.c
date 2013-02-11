 /*
 * 
 *  Copyright (c) 2012
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
#include "../lib/includes.h"
#include <sys/types.h>
#include <string.h>
#include <dirent.h>

#include <mntent.h>
#include <sys/mount.h>
#include <stdlib.h>

/*
 * below header file does not ship with the source code, it is created at configure time
 * */
#include "libmount_header.h"

static void mtab_remove_entry( const char * path )
{
	FILE * f ;
	FILE * g ;
	
#if USE_NEW_LIBMOUNT_API
	struct libmnt_lock * lock ;
#else
	mnt_lock * lock ;
#endif
	struct mntent * mt ;
	
	size_t path_len ;
	
	if( !zuluCryptMtabIsAtEtc() )
		return ;
	
	path_len = strlen( path ) ;
	
	f = setmntent( "/etc/mtab","r" ) ;
	
	lock = mnt_new_lock( "/etc/mtab~",getpid() ) ;
	
	if( mnt_lock_file( lock ) == 0 ){
		g = setmntent( "/etc/mtab-zC","w" ) ;
		while( ( mt = getmntent( f ) ) != NULL ){
			if( strncmp( mt->mnt_fsname,path,path_len ) != 0 ){
				addmntent( g,mt ) ;
			}
		}
		endmntent( g ) ;
		endmntent( f ) ;
		rename( "/etc/mtab-zC","/etc/mtab" ) ;
		chown( "/etc/mtab",0,0 ) ;
		chmod( "/etc/mtab",S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) ;
		mnt_unlock_file( lock ) ;
	}else{
		endmntent( f ) ;
	}	
	
	mnt_free_lock( lock ) ;
}

static void delete_mount_point_with_no_device( const char * path,uid_t uid,stringList_t stl )
{
	/*
	 * zuluCryptGetUserName() is defined in ../lib/user_home_path.c
	 */
	string_t p = zuluCryptGetUserName( uid ) ;
	DIR * dir = opendir( StringPrepend( p,"/run/media/" ) ) ;
	struct dirent * entry ;
	const char * m_path ;
	
	if( dir == NULL ){
		StringDelete( &p ) ;
		return ;
	}
	
	if( path ){;}
	
	while( ( entry = readdir( dir ) ) != NULL ){
		m_path = entry->d_name ;
		if( strcmp( m_path,"." ) == 0 )
			continue ;
		if( strcmp( m_path,".." ) == 0 )
			continue ;
		if( StringListHasSequence( stl,m_path ) != 0 ){
			/*
			 * TODO: there maybe collisions here if more that one path starts with the same character sequence
			 */
			/*
			 * folder exists at mount point prefix but it is not mounted,this is one we want to delete
			 */
			
			m_path = StringMultipleAppend( p,"/",m_path,END ) ;
			rmdir( m_path ) ;
		}
	}
	
	closedir( dir ) ;
	StringDelete( &p ) ;
	mtab_remove_entry( path ) ;
}

/*
 * This function is called when a dead mapper is noticed,a dead mapper show up when a device is removed
 * without being properly closed.
 */
static void remove_mapper( const char * path,stringList_t stl,uid_t uid )
{
	const char * m_point ;
	stringList_t stx ;
	
	ssize_t index = StringListHasStartSequence( stl,path ) ;

	if( index == -1 ){
		/*
		 * path has no entry in "proc/self/mountinfo" but does in "/dev/mapper,
		 * this happens when a mapper is already "unmount2()" below.Just deactivate it
		 * and return 
		 */
		if( crypt_deactivate( NULL,path ) == 0 ){
			/*
			 * mount point is no longer busy and not in /proc/self/mountinfo.
			 * Search the user mount point prefix to see if there is a mount point
			 * with no associated device and delete it
			 */
			delete_mount_point_with_no_device( path,uid,stl ) ;
		}else{
			/*
			 * mount point is still busy
			 */
			;
		}
		return ;
	}
	
	stx = StringListSplit( StringListContentAt( stl,index ),' ' ) ;
	
	m_point = StringListContentAt( stx,1 ) ;
	
	if( umount( m_point ) == 0 ){
		rmdir( m_point ) ;
		crypt_deactivate( NULL,path ) ;
		mtab_remove_entry( path ) ;
	}else{
		if( umount2( m_point,MNT_DETACH ) == 0 ){
			mtab_remove_entry( path ) ;
		}
	}
	
	StringListDelete( &stx ) ;
}

void zuluCryptClearDeadMappers( uid_t uid )
{
	struct crypt_device * cd ;
	const char * dir_path = crypt_get_dir() ;
	DIR * dir = opendir( dir_path ) ;
	struct dirent * entry ;
	const char * m ;
	const char * e ;
	size_t len ;
	size_t len1 ;
	/*
	 * zuluCryptGetMtabList() is defined in ../lib/print_mounted_volumes.c
	 */
	stringList_t stl = zuluCryptGetMtabList() ;
	string_t p ;
	string_t z = String( dir_path ) ;
	StringAppend( z,"/" ) ;
	len1 = StringLength( z ) ;
	
	p = String( "zuluCrypt-" );
	m = StringAppendInt( p,uid ) ;
	len = StringLength( p ) ;
	
	/*
	 * zuluCryptSecurityGainElevatedPrivileges() is defined in security.c
	 */
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	while( ( entry = readdir( dir ) ) != NULL ){
		
		if( strncmp( entry->d_name,m,len ) == 0 ){
			e = StringAppendAt( z,len1,entry->d_name ) ;
			if( crypt_init_by_name( &cd,e ) == 0 ){
				if( crypt_get_device_name( cd ) == NULL ){
					remove_mapper( e,stl,uid ) ;
				}
				crypt_free( cd ) ;
			}else{
				remove_mapper( e,stl,uid ) ;
			}
		}
	}
	
	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in security.c
	 */
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	StringListDelete( &stl ) ;
	StringDelete( &p ) ;
	StringDelete( &z ) ;
	closedir( dir ) ;
}
