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
 
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <libcryptsetup.h>

#include "../string/String.h" 
#include "../string/StringList.h" 
#include "../constants.h"
#include "../zuluCrypt.h"

#ifdef __STDC__
char * realpath( const char * path, char * resolved_path ) ;
#endif
/*
 * this structure holds command line arguments. * 
 * It is instantiated in main.c * 
 */
typedef struct struct_opts_1{
	const char * device ;
	const char * mount_point ;
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
	char action ;
	int partition_number ;
	int dont_ask_confirmation ;
	int interactive_passphrase ;
	int open_no_mount ;
}struct_opts;

/*
 * these two defines are used in save_and_restore_luks_header.c  
 */
#define LUKS_HEADER_RESTORE 1
#define LUKS_HEADER_SAVE 0

/*
 * this function is responsibe for creating and restoring luks header.
 * 
 * the function is defined in save_and_restore_luks_header.c
 */
int save_and_restore_luks_header( const struct_opts * opts,uid_t uid,int option  ) ;

/*
 * this function reads user passphrase interactively.
 * It is defined and used in main.c
 */
string_t get_passphrase( void ) ;
 
/*
 * this function is responsibe for printing information about an opened volume.
 * It is defined in volume_info.c  
 */
int volume_info( const char * mapper,const char * device,uid_t ) ;
 
/*
 * this function is responsibe for closing an opened volume.
 * It is defined in close_volume.c 
 */
int close_opened_volume( const char * device,const char * mapping_name,uid_t ) ;
 
/*
 * this function is responsibe for opening volumes.
 * It is defined in open_volume.c * 
 */
int open_volumes( const struct_opts *,const char * mapping_name,uid_t uid ) ;

/*
 * this function is responsibe for creating volumes.
 * It is defined in create_volume.c
 */		   
int create_volumes( const struct_opts *,const char * mapping_name,uid_t uid ) ;

/*
 * this function is responsibe for adding keys to luks volumes.
 * It is defined in add_key.c
 */
int addkey( const struct_opts *,uid_t ) ;

/*
 * this function is responsibe for removing keys from luks files.
 * It is defined in remove_key.c
 */		
int removekey( const struct_opts *,uid_t ) ;

/*
 * this function is responsibe for checking if a path exist or not.
 * It is defined in ../lib/is_path_valid.c
 */
int is_path_valid(const char * path ) ;

/*
 * this function is responsibe substituting bash special characters with an underscore.
 * The explanation for why it does that is in the source file.
 * The function is defined in replace_bash_special_chars.c * 
 */
void replace_bash_special_chars( string_t * ) ;

/*
 * thiw function reads a passphrase from a key file after it makes sure a user who started the
 * tool had reading access to the file.
 * It is defined in security.c
 */
int get_pass_from_file( const char * path,uid_t uid,string_t * st ) ;

/*
 * this function is responsibe for printing a list of opened volumes.
 * It primary purpose is for the GUI pqrt of the tool and hence the output isnt formatted.
 * It is defined in print_opened_volumes.c
 */
int print_opened_volumes( uid_t ) ;

/*
 * this function makes a unique mapper name based on user UID to make sure one user can not manage another user
 * opened volumes.
 * It is defined in create_mapper_name.c
 */
string_t create_mapper_name( const char * device,const char * mapping_name,uid_t uid,int ) ;

/*
 * this function is responsibe for creating a mount point after it checks to make sure a user who started the tool
 * had writing access to the parent folder.
 * It is defined in security.c * 
 */
int create_mount_point( const char * path,uid_t uid ) ;

/*
 * this function checks if a user who started the tool has writing access to a file or device they want this tool to 
 * write to.
 * It is defined in security.c
 */
int can_open_path_for_writing( const char * path,uid_t uid ) ;

/*
 * this function checks if a user who started the tool has reading access to a file or device they want this tool to 
 * read from.
 * It is defined in security.c
 */
int can_open_path_for_reading( const char * path,uid_t uid ) ;

/* 
 * check if the volume has atleast one corrupted key slot
 */ 
void check_invalid_key( const char * device ) ;

/*
 * this function checks if a device is a system partition or not.
 * 
 * the function is defined in partitions.c * 
 */
int check_partition( const char * device ) ;

/*
 * defined in crypt_file.c
 */
int file_encrypt( const struct_opts *,const char *,uid_t uid ) ;

/*
 * defined in crypt_file.c
 */
int file_decrypt( const struct_opts *,const char *,uid_t uid ) ;

/*
 * defined in check_opened_mapper.c
 * 
 * the function checks to see if the argument mapper has an entry in crypt_get_dir().
 * 
 * the function is used to prevent performing operations like creating a volume on devices with mapper open.
 */
int check_opened_mapper( const char * mapper ) ;
