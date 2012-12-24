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
#include <fcntl.h>
#include "includes.h"
#include <errno.h>

/*
 * global_variable_file_struct is a global variable declared in ../lib/includes.h
 * and defined in ../lib/create_loop_device.c
 */

static void _get_path_from_file( int fd,string_t * st_dev )
{
	char * c ;
	string_t st = String( "/proc/self/fd/" ) ;
	StringAppendInt( st,fd ) ;
	c = zuluCryptRealPath( StringContent( st ) ) ;
	StringDelete( &st ) ;
	*st_dev = StringInherit( &c ) ;
}

int zuluCryptSecureOpenFile( const char * path,int * fd,string_t * file,uid_t uid )
{
	int st ;
	int f = -1 ;
	uid_t org = geteuid() ;
	seteuid( uid ) ;
	f = open( path,O_RDONLY ) ;
	if( f != -1 ){
		_get_path_from_file( f,file ) ;
		*fd = f ;
		st = 1 ;
	}else{
		st = 0  ;
	}
	seteuid( org ) ;
	return st ;
}

int zuluCryptGetDeviceFileProperties( const char * file,int * fd,string_t * st_dev,uid_t uid )
{
	int st = 100 ;
	int xt = 0 ;
	int lfd ;
	
	struct stat stat_st ;
	struct stat stat_st_1 ;
	/*
	 * try to open the device with user privileges
	 */
	seteuid( uid ) ;
		
	*fd = open( file,O_RDONLY ) ;
	
	if( *fd != -1 ){
		fstat( *fd,&stat_st ) ;
		fcntl( *fd,F_SETFD,FD_CLOEXEC ) ;
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
				 * check to make sure the file is got earlier is the same one we got now.
				 * ie check to make sure the file wasnt swapped btw calls.
				 */
				if( stat_st.st_dev == stat_st_1.st_dev && stat_st.st_ino == stat_st_1.st_ino ){
					seteuid( 0 ) ;
					/*
					 * zuluCryptAttachLoopDeviceToFileUsingFileDescriptor() is defined in ./create_loop_device.c
					 */
					xt = zuluCryptAttachLoopDeviceToFileUsingFileDescriptor( lfd,O_RDWR,st_dev ) ;
					seteuid( uid ) ;
					close( *fd ) ;
					*fd = lfd ;
				}
			}else{
				/*
				 * we can not open the file in read write mode
				 */
				seteuid( 0 ) ;
				/*
				 * zuluCryptAttachLoopDeviceToFileUsingFileDescriptor() is defined in ./create_loop_device.c
				 */
				xt = zuluCryptAttachLoopDeviceToFileUsingFileDescriptor( *fd,O_RDONLY,st_dev ) ;
				seteuid( uid ) ;
			}
			
			if( xt != 1 ){
				st = 100 ;
				close( *fd ) ;
				*fd = -1 ;
			}else{
				st = 0 ;
			}
		}else{
			close( *fd ) ;
			*fd = -1 ;
			if( S_ISBLK( stat_st.st_mode ) ){
				if( uid == 0 ) {
					/*
					 * we got a block device and we are root,accept it
					 */
					st = 0 ;
				}else{
					/*
					 * user has access to block device,odd,reject
					 */
					st = 1 ;
				}
			}else if( S_ISDIR( stat_st.st_mode ) ){
				st = 2 ;
			}else{
				switch( errno ){
					case EACCES : st = 4 ; break ;
					/*
					 * more may follow
					 */
				}
			}
		}
	}else{
		/*
		 * failed to open it with normal user privileges,try to open it with root's
		 */
		
		seteuid( 0 ) ;
		
		*fd = open( file,O_RDONLY ) ;
		
		if( *fd != -1 ){
			/*
			 * A user has access to the device.
			 * we close the file when we are done examining them because they can not be moved under us and we dont have to
			 * hold on to them.Besides,we cant even if we want to as cryptsetup will demand exclusive access to them. 
			 */
			if( S_ISBLK( stat_st.st_mode ) ){
				if( StringStartsWith( *st_dev,"/dev/shm" ) ){
					/*
					 * we do not support this path
					 */
					st = 1 ;
				}else if( StringStartsWith( *st_dev,"/dev/" ) ){
					/*
					 * got the block device we want,accept it
					 */
					st = 0 ;
				}else{
					/*
					 * reject others
					 */
					st = 100 ;
				}
			}else if( S_ISDIR( stat_st.st_mode ) ){
				st = 2 ;
			}else{
				switch( errno ){
					case EACCES : st = 4 ; break ;
					/*
					 * more may follow
					 */
				}
			}
			close( *fd ) ;
			*fd = -1 ;
		}else{
			/*
			 * invalid path or something i dont know,reject
			 */
			st = 100 ;
		}
		
		seteuid( uid ) ;
	}
	return st ;
}
