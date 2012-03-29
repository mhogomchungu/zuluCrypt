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
 
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../string/String.h" 
#include "../string/StringList.h" 
#include "../executables.h"
#include "../zuluCrypt.h"
 
#define ALL_PARTITIONS        1
#define SYSTEM_PARTITIONS     2
#define NON_SYSTEM_PARTITIONS 3

#define UUID_LENGTH 36
#define DEVICE_LENGHT 12

typedef struct struct_opts_1{
	const char * device ;
	const char * mount_point ;
	const char * action ;
	const char * mode ;
	const char * key_source ;
	const char * key ;
	const char * fs ;
	const char * type ;
	const char * rng ;
	const char * existing_key_source ;
	const char * existing_key ;
	const char * new_key ;
	const char * new_key_source ;
	int partition_number ;
	int dont_ask_confirmation ;
	int interactive_passphrase ;
	int open_no_mount ;
}struct_opts;

string_t get_passphrase( void ) ;
 
stringList_t partitions( int option ) ;
 
int volume_info( const char * mapper,const char * device,uid_t ) ;
 
int close_opened_volume( const char * mapping_name,uid_t ) ;
 
int open_volumes( const struct_opts *,const char * mapping_name,uid_t uid ) ;
		   
int create_volumes( const struct_opts *,uid_t ) ;

int addkey( const struct_opts *,uid_t ) ;
		
int removekey( const struct_opts *,uid_t ) ;

int is_path_valid(const char * path ) ;

int is_path_valid_by_euid( const char * path,uid_t uid ) ;

void replace_bash_special_chars( string_t ) ;

int get_pass_from_file( const char * path,uid_t uid,string_t * st ) ;

int print_opened_volumes( uid_t ) ;

#define OPEN 1
#define CLOSE 2
string_t create_mapper_name( const char * mapping_name,uid_t uid,int ) ;
