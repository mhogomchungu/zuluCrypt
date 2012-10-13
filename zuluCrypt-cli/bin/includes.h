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
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <libcryptsetup.h>

#include "../utility/string/String.h" 
#include "../utility/string/StringList.h" 
#include "../constants.h"
#include "../lib/libzuluCrypt.h"
#include "../utility/socket/socket.h"
#include "libzuluCrypt-exe.h"
#include "security.h"

#ifdef __STDC__
char * realpath( const char * path, char * resolved_path ) ;
int getopt(int argc, char * const argv[], const char *optstring);
extern char *optarg;
extern int optind, opterr, optopt;
#endif

/*
 * These functions are here and not in libzuluCrypt-exe.h because they use string_t and the type is not supposed to be
 * seen in public API
 */
/*
 * this function makes a unique mapper name based on user UID to make sure one user can not manage another user
 * opened volumes.
 * It is defined in create_mapper_name.c
 */
string_t zuluCryptCreateMapperName( const char * device,const char * mapping_name,uid_t uid,int ) ;

/*
 * thiw function reads a passphrase from a key file after it makes sure a user who started the
 * tool had reading access to the file.
 * It is defined in security.c
 */
int zuluCryptSecurityGetPassFromFile( const char * path,uid_t uid,string_t * st ) ;

/*
 * this function is defined in module_system.c
 * It gets a passphrase from a .so file as a plugin.
 */
string_t GetKeyFromModule( const char * path,uid_t ) ;

/*
 * this function is defined in get_key_from_socket.c
 */

size_t zuluCryptGetKeyFromSocket( const char * path,string_t *,uid_t uid ) ;

/*
 * this function returns "$HOME/" and is defined in ../pluginManager/zuluCryptPluginManager.c
 */
string_t zuluCryptGetUserHomePath( uid_t ) ;

/*
 * this function is defined in partitions.c
 */
int zuluCryptPartitionIsSystemPartition( const char * dev ) ;

/*
 * this function is defined in is_luks.c
 */
int zuluCryptVolumeIsNotLuks( const char * dev ) ;
