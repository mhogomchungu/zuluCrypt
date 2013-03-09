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

#include <mntent.h>
#include <sys/mount.h>
#include <stdlib.h>

/*
 * below header file does not ship with the source code, it is created at configure time
 * */
#include "libmount_header.h"

static int entry_found( const char * m_dir,char ** m_point )
{
	string_t st ;
	int h ;
	int i ;
	
	/*
	 * try 5 times on one second intervals to umount the volume.
	 * Trying to unmount more than once seem to be necessary sometimes
	 *  when the opened volume is accessed over samba share. 
	 */
	for( i = 0 ; i < 5 ; i++ ){
		h = umount( m_dir ) ;
		if( h == 0 ){
			break ;
		}else{
			perror( "kk" ) ;
			sleep( 1 ) ;
		}
	}
	
	if( h == 0 && m_point != NULL ){
		st = String( m_dir ) ;
		*m_point = StringDeleteHandle( &st ) ;
	}
	
	return h ;
}

int zuluCrypRemoveEntryFromMtab( const char * device ) 
{
#if USE_NEW_LIBMOUNT_API
	struct libmnt_lock * lock ;
#else
	mnt_lock * lock ;
#endif
	struct mntent * mt ;
	
	struct stat str ;
	
	FILE * f ;
	FILE * g ;
	
	int found = 0 ;
	const char * e ;
	
	string_t st ;
	
	size_t dev_len = strlen( device ) ;
	ssize_t index ;
	
	int h ;

	lock = mnt_new_lock( "/etc/mtab~",getpid() ) ;
	
	f = setmntent( "/etc/mtab","r" ) ;
	
	if( mnt_lock_file( lock ) != 0 ){
		h = 4 ;
	}else{
		g = setmntent( "/etc/mtab-zuluCrypt","w" ) ;
		while( ( mt = getmntent( f ) ) != NULL ){
			if( StringPrefixMatch( mt->mnt_fsname,device,dev_len ) ){
				found = 1 ;
			}else{
				addmntent( g,mt ) ;
			}
		}
		
		endmntent( g ) ;
		if( found ){
			rename( "/etc/mtab-zuluCrypt","/etc/mtab" ) ;
			chown( "/etc/mtab",0,0 ) ;
			chmod( "/etc/mtab",S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) ;
		}else{
			if( StringPrefixMatch( device,"/dev/",5 ) ){
				/*
				 * entry not found,assuming the entry is an LVM volume,in /dev/abc/def format while mtab has it in 
				 * /dev/mapper/abc-def format convert it and try again 
				 */
				found = 0 ;
				endmntent( f ) ;
				f = setmntent( "/etc/mtab","r" ) ;
				unlink( "/etc/mtab-zuluCryp" ) ;
				g = setmntent( "/etc/mtab-zuluCrypt","w" ) ;
				
				st = String( device ) ;
				index = StringLastIndexOfChar( st,'/' ) ;
				if( index != -1 ){
					StringSubChar( st,index,'-' ) ;
					e = StringReplaceString( st,"/dev/","/dev/mapper/" ) ;
					if( stat( e,&str ) == 0 ){
						index = StringIndexOfChar( st,0,' ' ) ;
						/*
						 * yap,volume is an LVM volume
						 */
						while( ( mt = getmntent( f ) ) != NULL ){
							if( StringPrefixMatch( mt->mnt_fsname,e,index ) ){
								found = 1 ;
							}else{
								addmntent( g,mt ) ;
							}
						}
					}
				}
			
				endmntent( g ) ;
			
				StringDelete( &st ) ;
				
				if( found ){
					rename( "/etc/mtab-zuluCrypt","/etc/mtab" ) ;
					chown( "/etc/mtab",0,0 ) ;
					chmod( "/etc/mtab",S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) ;
				}else{
					/*
					 * volume doesnt seem to have an entry in mtab
					 */
					;
				}
			}
		}
			
		mnt_unlock_file( lock ) ;
		h = 0 ;
	}
	
	endmntent( f ) ;
	mnt_free_lock( lock ) ;
	return h ;
}

int zuluCryptUnmountVolume( const char * device,char ** m_point )
{
	char * m ;
	int h = 3 ;
	
	char * loop_path = NULL ;
	
	if( StringPrefixMatch( device,"/dev/loop",9 ) ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ./create_loop_device.c
		 */
		loop_path = zuluCryptLoopDeviceAddress( device ) ;
		if( loop_path != NULL ){
			device = loop_path ;
		}
	}
	
	/*
	 * zuluCryptGetMountPointFromPath() is defined in ./process_mountinfo.c
	 */
	m = zuluCryptGetMountPointFromPath( device ) ;

	if( m != NULL ){
		h = entry_found( m,m_point ) ;
		free( m ) ;
		if( h == 0 ){
			/*
			 *zuluCryptMtabIsAtEtc() is defined in ./mount_volume.c 
			 */
			if( zuluCryptMtabIsAtEtc() ){
				h = zuluCrypRemoveEntryFromMtab( device ) ;
			}	
		}
	}
		
	if( h != 0 && h != 3 && h != 4 )
		h = 2 ;

	if( loop_path != NULL ){
		free( loop_path ) ;
	}
	
	return h ;
}
