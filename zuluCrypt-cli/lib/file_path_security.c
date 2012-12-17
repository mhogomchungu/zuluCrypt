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

struct stat global_variable_file_struct  ;
int zuluCryptGetDeviceFileProperties( const char * file,int * fd,string_t * st_dev )
{
	string_t st ;
	char * c ;
	
	*fd = open( file,O_RDONLY ) ;
	
	memset( &global_variable_file_struct,'\0',sizeof( struct stat ) ) ;
	
	if( *fd != -1 ){
		st = String( "/proc/self/fd/" ) ;
		StringAppendInt( st,*fd ) ;
		c = zuluCryptRealPath( StringContent( st ) ) ;
		StringDelete( &st ) ;
		*st_dev = StringInherit( &c ) ;
		/*
		 * we arent root privilgeed sp we will get here only when an attempt to 
		 * open a file is made.We take file properties of the file and compare them
		 * with file properties after we reopen the file to attach a loop device in 
		 */
		
		/*
		 * global_variable_file_struct is a global variable declared in ../lib/includes.h
		 * and defined in ../lib/create_loop_device.c
		 */
		fstat( *fd,&global_variable_file_struct ) ;
		if( S_ISREG( global_variable_file_struct.st_mode ) ){
			/*
			 * We got a file,we dont close it,we hang on to it to prevent the file from being
			 * replaced under us by an attacker. The file descriptor will be compared against the file
			 * that is actually used to create a loop device in create_loop_device.c
			 */
			return 0 ;
		}
		if( S_ISBLK( global_variable_file_struct.st_mode ) ){
			close( *fd ) ;
			*fd = -1 ;
			return 0 ;
		}
		if( S_ISLNK( global_variable_file_struct.st_mode ) ){
			close( *fd ) ;
			*fd = -1 ;
			return 1 ;
		}		
		if( S_ISDIR( global_variable_file_struct.st_mode ) ){
			close( *fd ) ;
			*fd = -1 ;
			return 2 ;
		}
		if( global_variable_file_struct.st_nlink == 0 || global_variable_file_struct.st_nlink > 1 ){
			close( *fd ) ;
			*fd = -1 ;
			return 3 ;
		}
	}
	return 4 ;
}
