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
#include <errno.h>

/*
 * check if a path is valid or not,nothing special or interesting here.
 */
int is_path_valid( const char * path )
{
	struct stat st ;
	if( stat( path,&st ) == 0 )
		return 0 ;
	else
		return 1 ;
}

/*
 * This function checks if path is valid and if a user has sufficient privileges to access valid paths.
 * This tool runs with suid bit set and hence with it, a normal can use it to access any other part of the system.
 * 
 * This function prevents that from happening by making sure paths are checked with the user's privileges and not root's.
 * 
 * If this work as expected, a user will not be allowed to presents paths that have no reading or writing access to.
 * 
 * This function prevents, for example, a normal user opening a volume located at another user's home directory. * 
 */

int is_path_valid_by_euid( const char * path,uid_t uid ) 
{
	int xt ;
	struct stat st ;
	
	uid_t euid = geteuid();
	
	seteuid( uid ) ;
	
	xt = stat( path,&st ) ;
	
	seteuid( euid ) ;
	
	if( xt == 0 )
		return 0 ;
	else{
		if( errno == EACCES )
			return 2 ;
		else
			return 1 ;
	}
}