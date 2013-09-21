/*
 * 
 *  Copyright ( c ) 2012
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
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
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

int zuluCryptSecureOpenFile( const char * path,int * fd,string_t * file,uid_t uid )
{
	int st ;
	int f = -1 ;
	uid_t org = geteuid() ;
	char * dev ;
	seteuid( uid ) ;
	f = open( path,O_RDONLY ) ;
	if( f != -1 ){
		dev = zuluCryptGetFileNameFromFileDescriptor( f ) ;
		*file = StringInherit( &dev ) ;
		*fd = f ;
		st = 1 ;
	}else{
		st = 0  ;
	}
	seteuid( org ) ;
	return st ;
}

void zuluCryptDeleteFile( const char * file )
{
	void * map ;
	struct stat st ;
	int fd = open( file,O_WRONLY ) ;
	if( fd != -1 ){
		fstat( fd,&st ) ;
		map =  mmap( 0,st.st_size,PROT_WRITE,MAP_PRIVATE,fd,0 ) ;
		if( map != MAP_FAILED ){
			memset( map,'\0',st.st_size ) ;
			munmap( map,st.st_size ) ;
		}
		close( fd ) ;
	}
	unlink( file ) ;
}

static int _check_if_device_is_supported( int st,uid_t uid,char ** dev )
{
	const char * cfs ;
	string_t fs ;
	seteuid( 0 ) ;
	
	if( st == 0 ){
		/*
		* zuluCryptGetFileSystemFromDevice() is defined in mount_volume.c
		*/
		fs = zuluCryptGetFileSystemFromDevice( *dev ) ;
		seteuid( uid ) ;
		if( fs != StringVoid ){
			cfs = StringContent( fs ) ;
			if( StringHasComponent( cfs,"member" ) || StringHasComponent( cfs,"swap" ) ){
				st = 100 ;
			}
			StringDelete( &fs ) ;
		}
	}else{
		/*
		 * safely do free( *dev ) followed by *dev = NULL
		 */
		StringFree_3( dev ) ;
	}
	
	return st ;
}

static char * device_path( string_t st )
{
	/*
	 * These conversions seem to be unnecessary,leave them for now
	 */
	string_t xt ;
	char * c = NULL ;
	if( StringStartsWith( st,"/dev/mapper/" ) ){
		/*
		 * zuluCryptConvertIfPathIsLVM() is defin in status.c
		 */
		xt = zuluCryptConvertIfPathIsLVM( StringContent( st ) ) ;
		StringDelete( &st ) ;
		c = StringDeleteHandle( &xt ) ;
	}else if( StringStartsWith( st,"/dev/md" ) ){
		/*
		 * zuluCryptResolveMDPath() is defined in process_mountinfo.c
		 */
		c = zuluCryptResolveMDPath( StringContent( st ) ) ;
		StringDelete( &st ) ;
	}else{
		c = StringDeleteHandle( &st ) ;
	}
	return c ;
}

int zuluCryptGetDeviceFileProperties( const char * file,int * fd_path,int * fd_loop,char ** dev,uid_t uid )
{
	int st = 100 ;
	int xt = 0 ;
	int lfd ;
	
	const char * dev_1 = NULL ;
	string_t st_dev = StringVoid ;
	
	struct stat stat_st ;
	struct stat stat_st_1 ;
	/*
	 * try to open the device with user privileges
	 */
	seteuid( uid ) ;
	
	*dev = NULL ;
	
	*fd_path = open( file,O_RDONLY ) ;
	
	if( *fd_path != -1 ){
		fstat( *fd_path,&stat_st ) ;
		fcntl( *fd_path,F_SETFD,FD_CLOEXEC ) ;
		/*
		 * A user has access to the device.They should get here only with paths to files they have access to. 
		 * Allow access to files only
		 */
		if( S_ISREG( stat_st.st_mode ) ){
			/*
			 * we can open file in read mode,let see if we can in write mode too 
			 */
			lfd = open( file,O_RDWR ) ;
			if( lfd != -1 ){
				/*
				 * we can open the file in read write mode
				 */
				fstat( lfd,&stat_st_1 ) ;
				fcntl( lfd,F_SETFD,FD_CLOEXEC ) ;
				
				/*
				 * check to make sure the file we got earlier is the same one we got now.
				 * ie check to make sure the file wasnt changed btw calls.
				 */
				if( stat_st.st_dev == stat_st_1.st_dev && stat_st.st_ino == stat_st_1.st_ino ){
					close( *fd_path ) ;
					*fd_path = lfd ;
					seteuid( 0 ) ;
					/*
					 * zuluCryptAttachLoopDeviceToFileUsingFileDescriptor() is defined in ./create_loop_device.c
					 */
					xt = zuluCryptAttachLoopDeviceToFileUsingFileDescriptor( *fd_path,fd_loop,O_RDWR,&st_dev ) ;
					seteuid( uid ) ;
					*dev = device_path( st_dev ) ;
				}
			}else{
				/*
				 * we can not open the file in write mode,continue with read only access
				 */
				seteuid( 0 ) ;
				/*
				 * zuluCryptAttachLoopDeviceToFileUsingFileDescriptor() is defined in ./create_loop_device.c
				 */
				xt = zuluCryptAttachLoopDeviceToFileUsingFileDescriptor( *fd_path,fd_loop,O_RDONLY,&st_dev ) ;
				seteuid( uid ) ;
				*dev = device_path( st_dev ) ;
			}				
			if( xt != 1 ){
				st = 100 ;
				close( *fd_path ) ;
				*fd_path = -1 ;
			}else{
				dev_1 = zuluCryptGetFileNameFromFileDescriptor( *fd_path ) ;
				if( StringPrefixMatch( dev_1,"/dev/shm/",9 ) ){
					st =1 ;
					close( *fd_path ) ;
					*fd_path = -1 ;
				}else{
					st = 0 ;
				}
				StringFree( dev_1 ) ;
			}
		}else{
			if( S_ISBLK( stat_st.st_mode ) ){
				if( uid == 0 ) {
					/*
					 * we got a block device and we are root,accept it
					 */
					*dev = zuluCryptGetFileNameFromFileDescriptor( *fd_path ) ;
					st = 0 ;
				}else{
					/*
					 * odd,normal user has access to a block device,allow it only if the
					 * device is in "/dev/" but not in "/dev/shm"
					 */
					*dev = zuluCryptGetFileNameFromFileDescriptor( *fd_path ) ;
					if( StringPrefixMatch( *dev,"/dev/shm/",9 ) ){
						st = 1 ;
					}else if( StringPrefixMatch( *dev,"/dev/",5 ) ){
						st = 0 ;
					}
				}
			}else if( S_ISDIR( stat_st.st_mode ) ){
				st = 2 ;
			}else{
				/*
				 * whatever it is,it cant be good,reject it
				 */
				st = 100 ;
			}
			close( *fd_path ) ;
			*fd_path = -1 ;
		}
	}else{
		/*
		 * failed to open above with users privileges,try to open the device with root's privileges.
		 * We should only accept block devices in "/dev/" but not in "/dev/shm". 
		 */
		seteuid( 0 ) ;
		
		*fd_path = open( file,O_RDONLY ) ;
		
		if( *fd_path != -1 ){
			fstat( *fd_path,&stat_st ) ;
			/*
			 * zuluCryptGetFileNameFromFileDescriptor() is defined in ./create_loop_device.c
			 */
			*dev = zuluCryptGetFileNameFromFileDescriptor( *fd_path ) ;
			
			if( S_ISBLK( stat_st.st_mode ) ){
				if( StringPrefixMatch( *dev,"/dev/shm/",9 ) ){
					/*
					* we do not support this path
					*/
					st = 1 ;
				}else if( StringPrefixMatch( *dev,"/dev/",5 ) ){
					/*
					* got a block device,accept it
					*/
					st = 0 ;
				}else{
					/*
					* reject others
					*/
					st = 100 ;
				}			
			}else{
				/*
				 * whatever it is,it cant be good,reject it
				 */
				st = 100 ;
			}
			/*
			 * We are closing the file because we dont need to hold on to it as paths in "/dev/" can not be moved under us by
			 * normal users.
			 */
			close( *fd_path ) ;
			*fd_path = -1 ;
		}else{
			/*
			 * invalid path or something i dont know,reject
			 */
			st = 100 ;
		}
		
		seteuid( uid ) ;
	}
	
	return _check_if_device_is_supported( st,uid,dev ) ;
}
