/*
* 
*  Copyright (c) 2013
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
#include "../lib/includes.h"

int zuluCryptBindUnmountVolume( stringList_t stx,const char * device,uid_t uid )
{
	stringList_t stl ;
	string_t xt ;
	string_t st ;
	ssize_t index = -1 ;
	const char * f ; 
	const char * e ;
	const char * g ;
	char * h = NULL ;
	int r = 1 ;
	int k ;
	int delete_stx = 0 ;
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	if( stx == StringListVoid ){
		/*
		 * zuluCryptGetMoutedListFromMountInfo() is defined in ../lib/process_mountinfo.c 
		 */
		stx = zuluCryptGetMoutedListFromMountInfo() ;
		delete_stx = 1 ;
	}
	
	if( StringPrefixMatch( device,"/dev/loop",9 ) ){
		/*
		 * zuluCryptLoopDeviceAddress_2() is defined in ../lib/create_loop_device.c
		 */
		st = zuluCryptLoopDeviceAddress_2( device ) ;
		/*
		 * Add a space at the end of device to avoid a possible collision when there exists two devices,one being "/dev/sdc1" 
		 * and another "/dev/sdc12"
		 */
		index = StringListHasStartSequence( stx,StringAppend( st," " ) ) ;
		StringRemoveRight( st,1 ) ;
		device = h = StringDeleteHandle( &st ) ;
	}else{
		/*
		 * Add a space at the end of device to avoid a possible collision when there exists two devices,one being "/dev/sdc1" 
		 * and another "/dev/sdc12"
		 */
		st = String( device ) ;
		index = StringListHasStartSequence( stx,StringAppend( st," " ) ) ;
		StringDelete( &st ) ;
	}
	
	if( index == -1 ){
		/*
		 * The volume does not appear to be mounted
		 */
		r = 1 ;
	}else{
		xt = StringListStringAt( stx,index ) ;
		stl = StringListStringSplit( xt,' ' ) ;
		
		xt = StringListCopyStringAt( stl,1 ) ;
		StringListDelete( &stl ) ;
		
		st = StringCopy( xt ) ;
		/*
		 * zuluCryptDecodeMtabEntry() is defined in ../lib/mount_volume.c
		 */
		g = zuluCryptDecodeMtabEntry( st ) ;
		
		/*
		 * g will contain something like "/run/media/$USER/sdc1"
		 */
		  
		/*
		 * zuluCryptSecurityMountPointPrefixMatch() is defined in ./security.c
		 */
		k = zuluCryptSecurityMountPointPrefixMatch( g,uid,NULL ) ;
		
		if( k != 1 ){
			/*
			 * One user is attempting to unmount a bind mount from another use,disallow it
			 */
			r = 4 ;
		}else{
			index = StringLastIndexOfChar( xt,'/' ) + 1 ;
			StringRemoveLeft( xt,index ) ;
			
			f = StringPrepend( xt,"/run/media/public/" ) ;

			/*
			 * f will now contain something like "/run/media/public/sdc1"
			 */
			
			index = StringListHasSequence( stx,f ) ;
			
			if( index == -1 ){
				/*
				 * volume is not shared
				 */
			}else{
				/*
				 * volume is shared,try to unmount it
				 * a volume is assumed to be shared if its device path in mountinfo has two mount points,one
				 * in /run/media/private/$USER and the other in /run/media/public/
				 */
				e = StringListContentAt( stx,index ) ;
				
				if( StringPrefixEqual( e,device ) ){
					
					f = zuluCryptDecodeMtabEntry( xt ) ;
					/*
					 * good,the device associated with the shared mount is the same as that of the
					 * private mount,try to unmount it.
					 */
					r = 3 ;
					for( k = 0 ; k < 3 ; k++ ){
						/*
						 * try to unmount 3 times before giving up
						 */
						if( umount( f ) == 0 ){
							/*
							 * zuluCrypRemoveEntryFromMtab() is defined in ../lib/unmount_volume.c
							 */
							zuluCrypRemoveEntryFromMtab( f ) ;
							rmdir( f ) ;
							r = 0 ;
							break ;
						}else{
							sleep( 1 ) ;
						}
					}
				}else{
					/*
					 * There appear to be a collision.One user mount poit suffix collides with another
					 * user public share mount point suffix.
					 * 
					 * Dont do anything,just return
					 */
					r = 0 ;
				}
			}
		}
		
		StringMultipleDelete( &xt,&st,END ) ;
	}
		
	if( delete_stx ){
		StringListDelete( &stx ) ;
	}
	if( h != NULL ){
		free( h ) ;
	}
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	return r ;
}

static int _bind_mount( const char * device,const char * o_path,const char * m_path,stringList_t stl,unsigned long flags )
{
	/*
	 * str structure is defined in ../lib/includes.h
	 */
	m_struct str ;
	string_t entry ;
	
	int xt = mount( o_path,m_path,"",flags|MS_BIND,"" ) ;
	
	if( xt == 0 ){
		/*
		 * zuluCryptGetMtabEntry() is defined in ../lib/process_mountinfo.c
		 */
		entry = zuluCryptGetMtabEntry_1( stl,device ) ;
		if( entry != StringVoid ){
			/*
			 * zuluCryptMtabIsAtEtc() is defined in ../lib/mount_volume.c
			 */
			if( zuluCryptMtabIsAtEtc() ){
				stl = StringListStringSplit( entry,' ' ) ;
				str.device          = device ;
				str.original_device = StringListContentAt( stl,0 ) ;
				str.m_point         = m_path ;
				str.fs              = StringListContentAt( stl,2 ) ;
				str.opts            = StringPrepend( StringListStringAt( stl,3 ),"bind," ) ;
				/*
				 * zuluCryptAddEntryToMtab() is defined in ../lib/mount_volume.c
				 */
				zuluCryptAddEntryToMtab( &str ) ;
				StringListDelete( &stl ) ;
			}
			StringDelete( &entry ) ;
		}
	}
	
	return xt ;
}

int zuluCryptBindMountVolume( const char * device,string_t z_path,unsigned long flags ) 
{
	struct stat st ;
	string_t path ;
	string_t tmp ;
	ssize_t index = StringLastIndexOfChar( z_path,'/' ) ;
	const char * o_path = StringContent( z_path ) ;
	const char * m_path ;
	const char * e ;
	int xt ;
	
	stringList_t stl ;
	
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH | S_IROTH ;
	
	if( index == -1 ){
		return 1 ;
	}
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptGetMoutedListFromMountInfo() is defined in ../lib/process_mountinfo.c
	 */
	stl = zuluCryptGetMoutedListFromMountInfo() ;
	
	path = String( "/run/media/public/" ) ;
	m_path = StringAppend( path,o_path + index + 1 ) ;
		
	if( stat( "/run",&st ) != 0 ){
		mkdir( "/run",mode ) ;
		chown( "/run",0,0 ) ;
	}
	if( stat( "/run/media",&st ) != 0 ){
		mkdir( "/run/media",mode ) ;
		chown( "/run/media",0,0 ) ;
	}
	if( stat( "/run/media/public",&st ) != 0 ){
		mkdir( "/run/media/public",mode ) ;
		chown( "/run/media/public",0,0 ) ;
	}
	if( stat( m_path,&st ) == 0 ){
		/*
		 * bind mount point exists,this will happen if the mount point is already taken or a mount point folder
		 * was not autodeleted for some reason 
		 */
		
		tmp = StringCopy( path ) ;
		e = StringAppend( tmp," " ) ;
		
		if( StringListHasSequence( stl,e ) != -1 ){
			/*
			 * An attempt is made to bind mount on a path already bind mounted path,dont attempt to mount
			 */
			xt = 1 ;
		}else{
			/*
			 * the mount point folder is there for some reason but is not being used.
			 */
			xt = _bind_mount( device,o_path,m_path,stl,flags ) ;
		}
		StringDelete( &tmp ) ;
	}else{
		mkdir( m_path,S_IRWXU | S_IRWXG | S_IRWXG ) ;
		chown( m_path,0,0 ) ;
		xt = _bind_mount( device,o_path,m_path,stl,flags ) ;
		if( xt != 0 ){
			rmdir( m_path ) ;
		}
	}
	
	StringListDelete( &stl ) ;
	StringDelete( &path ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	return xt ;
}
