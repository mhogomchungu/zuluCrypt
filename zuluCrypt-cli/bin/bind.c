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

int zuluCryptBindUnmountVolume( const char * device,const char * mapper,uid_t uid )
{
	/*
	 * zuluCryptGetMoutedListFromMountInfo() is defined in ../lib/process_mountinfo.c 
	 */
	stringList_t stx = zuluCryptGetMoutedListFromMountInfo() ;
	stringList_t stl ;
	string_t xt ;
	string_t st ;
	ssize_t index = -1 ;
	const char * f ; 
	const char * e ;
	const char * g ;
	int r = 1 ;
	int k ;
	char * dev = NULL ;
	
	if( StringPrefixMatch( device,"/dev/loop",9 ) ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ../lib/status.c 
		 */
		dev = zuluCryptLoopDeviceAddress( device ) ;
		if( dev != NULL ){
			device = dev ;
		}
	}else if( StringsAreNotEqual( device,mapper ) ){
		/*
		 * we will get here when trying to unmount an encrypted partition
		 */
		device = mapper ;
	}
	
	index = StringListHasStartSequence( stx,device ) ;
	
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
			
			f = StringPrepend( xt,"/run/share/" ) ;

			/*
			 * f will now contain something like " /media/share/sdc1"
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
				 * in /run/media/$USER and the other in /run/share
				 */
				e = StringListContentAt( stx,index ) ;
				
				if( StringPrefixEqual( e,device ) ){
					f = zuluCryptDecodeMtabEntry( xt ) ;
					/*
					 * good,the device associated with the shared mount is the same as that of the
					 * private mount,try to unmount it.
					 */
					for( k = 0 ; k < 3 ; k++ ){
						/*
						 * try to unmount 3 times before giving up
						 */
						if( umount( f ) == 0 ){
							/*
							 * zuluCrypRemoveEntryFromMtab() is defined in ../lib/unmount_volume.c
							 */
							zuluCrypRemoveEntryFromMtab( device ) ;
							rmdir( f ) ;
							r = 0 ;
							break ;
						}else{
							sleep( 1 ) ;
							r = 3 ;
						}
					}
				}else{
					
					/*
					 * bad,the device associated with the shared mount is different from the private mount.
					 * possible reason could be a collision of some sort,maybe a different tool use the mount path
					 * Bail out as we dont know what we are unmounting
					 */
					r = 5 ;
				}
			}
		}
		
		StringMultipleDelete( &xt,&st,END ) ;
	}
	
	StringListDelete( &stx ) ;
	
	if( dev != NULL ){
		free( dev ) ;
	}
	
	return r ;
	
}

int zuluCryptBindMountVolume( const char * device,string_t z_path,unsigned long flags ) 
{
	/*
	 * str structure is defined in ../lib/includes.h
	 */
	m_struct str ;
	struct stat st ;
	string_t path ;
	string_t entry ;
	
	stringList_t stl ;
	ssize_t index = StringLastIndexOfChar( z_path,'/' ) ;
	const char * o_path = StringContent( z_path ) ;
	const char * m_path ;
	int xt ;
	
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH | S_IROTH ;
	
	if( index == -1 ){
		return 1 ;
	}
	
	path = String( "/run/share/" ) ;
	m_path = StringAppend( path,o_path + index + 1 ) ;
		
	if( stat( "/run",&st ) != 0 ){
		mkdir( "/run",mode ) ;
		chown( "/run",0,0 ) ;
	}
	
	if( stat( "/run/share",&st ) != 0 ){
		mkdir( "/run/share",mode ) ;
		chown( "/run/share",0,0 ) ;
	}
	
	if( stat( m_path,&st ) == 0 ){
		 ;
	}else{
		mkdir( m_path,S_IRWXU | S_IRWXG | S_IRWXG ) ;
		chown( m_path,0,0 ) ;
	}
	
	xt = mount( o_path,m_path,"",flags|MS_BIND,"" ) ;
	
	if( xt == 0 ){
		/*
		 * zuluCryptGetMtabEntry() is defined in ../lib/process_mountinfo.c
		 */
		entry = zuluCryptGetMtabEntry( device ) ;
		if( entry != StringVoid ){
			/*
			 * zuluCryptMtabIsAtEtc() is defined in ../lib/mount_volume.c
			 */
			if( zuluCryptMtabIsAtEtc() ){
				stl = StringListStringSplit( entry,' ' ) ;
				StringDelete( &entry ) ;
				str.device          = device ;
				str.original_device = StringListContentAt( stl,0 ) ;
				str.m_point         = m_path ;
				str.fs              = StringListContentAt( stl,2 ) ;
			
				entry = StringListStringAt( stl,3 ) ;
			
				str.opts            = StringPrepend( entry,"bind," ) ;
				/*
				* zuluCryptAddEntryToMtab() is defined in ../lib/mount_volume.c
				*/
				zuluCryptAddEntryToMtab( &str ) ;
				StringListDelete( &stl ) ;
			}
		}
	}
	StringDelete( &path ) ;
	return xt ;
}
