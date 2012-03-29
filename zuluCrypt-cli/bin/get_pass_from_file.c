
/*
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

/*
 * return values
 * 0 success
 * 1 stat fail,file does not exist
 * 2 permission error,lack necessary permissions to access the file
 * 3 could not open file for reading,no read access
 * 4 insufficient memory,run out of memory
 */
int get_pass_from_file( const char * path,uid_t uid,string_t * st )
{
	switch ( is_path_valid_by_euid( path,uid ) ){
		case 1 : return 1 ;
		case 2 : return 2 ;
	}
	switch( StringGetFromFile_1( st,path ) ){
		case 1 : return 1 ;
		case 2 : return 3 ;
		case 3 : return 4 ;
	}
	return 0 ;
}
 
