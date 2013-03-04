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
#include <stdlib.h>


/*
 * This source file attempts to "clear a dead mapper".A dead mapper happens when a user opens
 * an encrypted volume and then just unplugs it without properly closing the mapper 
 */

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
			 * folder exists at mount point prefix but it is not mounted,this is one we want to delete
			 */
			m_path = StringMultipleAppend( p,"/",m_path,END ) ;
			rmdir( m_path ) ;
		}
	}
	
	closedir( dir ) ;
	StringDelete( &p ) ;
	/*
	 * zuluCrypRemoveEntryFromMtab() is defined in ../lib/unmount_volume.c
	 */
	zuluCrypRemoveEntryFromMtab( path ) ;
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
		
		/*
		 * zuluCryptCloseMapper() is defined in ../lib/close_mapper.c
		 */
		if( zuluCryptCloseMapper( path ) == 0 ){
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
	}else{
		/*
		 * First attempt to close a mapper,try to unmount it and then deactivate the mapper if 
		 * the operation succeed
		 */
		stx = StringListSplit( StringListContentAt( stl,index ),' ' ) ;
	
		m_point = StringListContentAt( stx,1 ) ;
	
		if( umount( m_point ) == 0 ){
			rmdir( m_point ) ;
			zuluCryptCloseMapper( path ) ;
			/*
			 * zuluCryptMtabIsAtEtc() is defined in ../lib/mount_volume.c
			 */
			if( zuluCryptMtabIsAtEtc() ){
				/*
				* zuluCrypRemoveEntryFromMtab() is defined in ../lib/unmount_volume.c
				*/
				zuluCrypRemoveEntryFromMtab( path ) ;
			}
		}else{
			if( umount2( m_point,MNT_DETACH ) == 0 ){
				if( zuluCryptMtabIsAtEtc() ){
					zuluCrypRemoveEntryFromMtab( path ) ;
				}
			}
		}
		
		StringListDelete( &stx ) ;
	}
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
	
	stringList_t stl ;
	string_t p ;
	string_t z ;
#if TRUECRYPT_TCPLAY
	/*
	 * return pematurely because crypt_get_device_name() does not work
	 * as expected with truecrypt volumes opened with tcplay
	 */
	return ;
#endif	
	/*
	 * zuluCryptGetMountInfoList()  is defined in ../lib/process_mountinfo.c
	 */
	stl = zuluCryptGetMountInfoList() ;
	z = String( dir_path ) ;
	
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
		if( StringPrefixMatch( entry->d_name,m,len ) ){
			e = StringAppendAt( z,len1,entry->d_name ) ;
			if( crypt_init_by_name( &cd,e ) == 0 ){
				if( crypt_get_device_name( cd ) == NULL ){
					/*
					 * we will get here is the mapper is "active" but the underlying device is gone
					 */
					remove_mapper( e,stl,uid ) ;
				}
				crypt_free( cd ) ;
			}else{
				/*
				 * we shouldnt get here
				 */
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
