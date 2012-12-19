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

struct stat global_variable_file_struct  ;
int zuluCryptGetDeviceFileProperties( const char * file,int * fd,string_t * st_dev,uid_t uid )
{
	int st = 100 ;
	memset( &global_variable_file_struct,'\0',sizeof( struct stat ) ) ;
	
	/*
	 * try to open the device with user privileges
	 */
	seteuid( uid ) ;
	
	*fd = open( file,O_RDONLY ) ;
	
	if( *fd != -1 ){
		_get_path_from_file( *fd,st_dev ) ;
		/*
		 * A user has access to the device.They should get here only with paths to files they have access to. 
		 * Allow access to files only
		 */
		fstat( *fd,&global_variable_file_struct ) ;
		global_variable_file_struct_is_set = 1 ;		
		if( S_ISREG( global_variable_file_struct.st_mode ) ){
			/*
			 * We got a file,we dont close it,we hang on to it to prevent the file from being
			 * replaced under us by an attacker. The file descriptor will be compared against the file
			 * that is actually used to create a loop device in create_loop_device.c
			 */
			st = 0 ;
		}else{
			close( *fd ) ;
			*fd = -1 ;
			if( S_ISBLK( global_variable_file_struct.st_mode ) ){
				if( uid == 0 ) {
					/*
					 * 
					 */
					st = 0 ;
				}else{
					/*
					 * user has access to block device,odd,reject
					 */
					st = 1 ;
				}
			}else if( S_ISDIR( global_variable_file_struct.st_mode ) ){
				st = 2 ;
			}else if( global_variable_file_struct.st_nlink == 0 || global_variable_file_struct.st_nlink > 1 ){
				st = 3 ;
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
			fstat( *fd,&global_variable_file_struct ) ;
			global_variable_file_struct_is_set = 1 ;
			if( S_ISBLK( global_variable_file_struct.st_mode ) ){
				_get_path_from_file( *fd,st_dev ) ;
				if( StringStartsWith( *st_dev,"/dev/" ) ){
					/*
					 * got the block device we want,accept it
					 */
					st = 0 ;
				}else{
					/*
					 * reject others
					 */
					;
				}
			}else if( S_ISDIR( global_variable_file_struct.st_mode ) ){
				st = 2 ;
			}else if( global_variable_file_struct.st_nlink == 0 || global_variable_file_struct.st_nlink > 1 ){
				st = 3 ;
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
			;
		}
		
		seteuid( uid ) ;
	}
	return st ;
}
