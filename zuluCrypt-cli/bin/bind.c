/*
*
*  Copyright (c) 2013-2015
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
#include "../lib/includes.h"

#include <sys/stat.h>
#include <unistd.h>

static void _chown( const char * x,uid_t y,gid_t z )
{
	if( chown( x,y,z ) ){;}
}

int zuluCryptBindUnmountVolume( stringList_t stx,const char * device,uid_t uid )
{
	stringList_t stl ;
	string_t xt ;
	string_t st ;
	string_t zt ;
	ssize_t index = -1 ;
	const char * f ;
	const char * g ;
	char * h = NULL ;
	int r = 1 ;
	int k ;
	int delete_stx = 0 ;

	/*
	 * zuluCryptUserIsAMemberOfAGroup() is defined in security.c
	 */
	/*
	 * root user is a member of all groups and hence is allowed
	 */
	int allowedUser = zuluCryptUserIsAMemberOfAGroup( uid,"zulumount" ) ;

	zuluCryptSecurityGainElevatedPrivileges() ;

	if( stx == StringListVoid ){
		/*
		 * zuluCryptGetMoutedListFromMountInfo() is defined in ../lib/process_mountinfo.c
		 */
		stx = zuluCryptGetMoutedListFromMountInfo() ;
		delete_stx = 1 ;
	}

	if( StringPrefixEqual( device,"/dev/loop" ) ){
		/*
		 * zuluCryptLoopDeviceAddress_2() is defined in ../lib/create_loop_device.c
		 */
		st = zuluCryptLoopDeviceAddress_2( device ) ;
		/*
		 * Add a space at the end of the device name to make sure we check the full device name to avoid possible collisions
		 * that may exist if one device is named "/home/abc" and another "/home/abcdef"
		 */
		zt = StringListHasStartSequence_1( stx,StringAppend( st," " ) ) ;
		StringRemoveRight( st,1 ) ;
		device = h = StringDeleteHandle( &st ) ;
	}else{
		/*
		 * Add a space at the end of the device name to make sure we check the full device name to avoid possible collisions
		 * that may exist if one device is named "/dev/sdc1" and another "/dev/sdc12"
		 */
		st = String( device ) ;
		zt = StringListHasStartSequence_1( stx,StringAppend( st," " ) ) ;
		StringDelete( &st ) ;
	}

	if( zt == StringVoid ){
		/*
		 * The volume does not appear to be mounted
		 */
		r = 1 ;
	}else{
		stl = StringListStringSplit( zt,' ' ) ;

		xt = StringListCopyStringAtSecondPlace( stl ) ;

		StringListDelete( &stl ) ;

		st = StringCopy( xt ) ;

		/*
		 * zuluCryptDecodeMountEntry() is defined in ../lib/mount_volume.c
		 * g will contain something like "/run/media/private/$USER/sdc1"
		 */
		g = zuluCryptDecodeMountEntry( st ) ;

		if( allowedUser ){
			/*
			 * a privileged user is attempting to unmount a shared mount point,allow them
			 */
			k = 1 ;
		}else{
			/*
			 * a non privileged user is attempting to unmount a shared mount point,allow them only if
			 * they are the one that created it
			 */
			/*
			* zuluCryptSecurityMountPointPrefixMatch() is defined in ./security.c
			*/
			k = zuluCryptMountPointPrefixMatch( g,uid,NULL ) ;
		}

		StringDelete( &st ) ;

		if( k != 1 ){
			/*
			 * One none privileged user is attempting to unmount a bind mount from another use,disallow it
			 */
			r = 4 ;
		}else{
			index = StringLastIndexOfChar( xt,'/' ) + 1 ;
			StringRemoveLeft( xt,index ) ;

			StringPrepend( xt,"/run/media/public/" ) ;

			/*
			 * f will now contain something like "/run/media/public/sdc1"
			 * space character is added before checking to avoid possible collisions
			 * as explained in above comments
			 */
			f = StringAppend( xt," " ) ;
			zt = StringListHasSequence_1( stx,f ) ;
			f = StringRemoveRight( xt,1 ) ;

			if( zt == StringVoid ){
				/*
				 * volume is not shared
				 */
			}else{
				/*
				 * volume is shared,try to unmount it
				 * a volume is assumed to be shared if its device path in mountinfo has two mount points,one
				 * in /run/media/private/$USER and the other in /run/media/public/
				 */
				if( StringStartsWith( zt,device ) ){
					f = zuluCryptDecodeMountEntry( xt ) ;
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
							rmdir( f ) ;
							r = 0 ;
							break ;
						}else{
							sleep( 1 ) ;
						}
					}
				}else{
					/*
					 * i dont see how we will get here,we shouldnt
					 */
					r = 0 ;
				}
			}
		}

		StringDelete( &xt ) ;
	}

	if( delete_stx ){
		StringListDelete( &stx ) ;
	}

	StringFree( h ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;
	return r ;
}

int zuluCryptBindSharedMountPointPathTaken( string_t path )
{
	struct stat str ;
	ssize_t index = StringLastIndexOfChar( path,'/' ) ;
	string_t st = String( "/run/media/public" ) ;
	const char * e = StringAppend( st,StringContent( path ) + index ) ;
	int r = stat( e,&str ) ;
	StringDelete( &st ) ;
	return r == 0 ;
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
	if( device ){;}

	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptGetMoutedListFromMountInfo() is defined in ../lib/process_mountinfo.c
	 */
	stl = zuluCryptGetMoutedListFromMountInfo() ;

	path = String( "/run/media/public/" ) ;
	m_path = StringAppend( path,o_path + index + 1 ) ;

	#define path_does_not_exist( x ) stat( x,&st ) != 0
	#define path_does_exist( x ) stat( x,&st ) == 0

	if( path_does_not_exist( "/run" ) ){
		mkdir( "/run",mode ) ;
		_chown( "/run",0,0 ) ;
	}
	if( path_does_not_exist( "/run/media" ) ){
		mkdir( "/run/media",mode ) ;
		_chown( "/run/media",0,0 ) ;
	}
	if( path_does_not_exist( "/run/media/public" ) ){
		mkdir( "/run/media/public",mode ) ;
		_chown( "/run/media/public",0,0 ) ;
	}
	if( path_does_exist( m_path ) ){
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
			xt = mount( o_path,m_path,"",flags|MS_BIND,"" ) ;
		}
		StringDelete( &tmp ) ;
	}else{
		mkdir( m_path,S_IRWXU | S_IRWXG | S_IRWXG ) ;
		_chown( m_path,0,0 ) ;
		xt = mount( o_path,m_path,"",flags|MS_BIND,"" ) ;
		if( xt != 0 ){
			rmdir( m_path ) ;
		}
	}

	StringListDelete( &stl ) ;
	StringDelete( &path ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	return xt ;
}
