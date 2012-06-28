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

#include <errno.h>
#include <unistd.h>

#include "../constants.h"

#ifdef __STDC__
int seteuid( uid_t );
#endif

/*
 * 
 * This source file makes sure the user who started the tool has permission to perform operations they want on paths they presented.  
 * 
 * 
 */


static int has_access( const char * path,int c,uid_t uid )
{
	int f ;
	
	int st = -1 ;
	
	uid_t org = getuid() ;
	
	seteuid( uid ) ;
	
	if( c == READ )
		f = open( path,O_RDONLY );
	else
		f = open( path,O_WRONLY );
	
	if( f >= 0 ){
		close( f ) ;
		st = 0 ;
	}else{
		switch( errno ){
			case EACCES : st = 1 ; break ; /* permission denied */
			case ENOENT : st = 2 ; break ; /* invalid path*/
			default     : st = 3 ; break ; /* common error */    
		}
	}
	
	seteuid( org ) ;
	
	return st ; 
}

int can_open_path_for_reading( const char * path,uid_t uid )
{
	return has_access( path,READ,uid ) ;
}

int can_open_path_for_writing( const char * path,uid_t uid )
{
	return has_access( path,WRITE,uid ) ;
}

int create_mount_point( const char * path,uid_t uid )
{
	int st ;
	
	uid_t org = geteuid();    
	
	seteuid( uid ) ;            
	
	st = mkdir( path,S_IRWXU ) ;
	
	seteuid( org ) ;           
	
	if( st == 0 )
		return 0 ;
	else{
		if( errno == EACCES ) 
			return 1 ;
		else if( errno == EEXIST )
			return 2 ;
		else
			return 3 ;
	}
}

/*
 *  return values:
 *  4 -permission denied
 *  1  invalid path
 *  2  insufficient memory to open file
 *  0  success * 
 */
int get_pass_from_file( const char * path,uid_t uid,string_t * st )
{
	switch ( can_open_path_for_reading( path,uid ) ){
		case 1 : return 4 ;
		case 2 : return 1 ;
	}
	
	switch( StringGetFromFile_3( st,path,0,KEYFILE_MAX_SIZE ) ){
		case 1 : return 1 ;
		case 2 : return 4 ;
		case 3 : return 2 ;
	}
	return 0 ;
}
		