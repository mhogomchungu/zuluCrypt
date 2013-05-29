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
 
 
#ifndef ZULUCRYPT_BIN
#define ZULUCRYPT_BIN

#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <libcryptsetup.h>
#include <grp.h>

#include "../utility/string/String.h" 
#include "../utility/string/StringList.h" 
#include "../constants.h"
#include "../lib/libzuluCrypt.h"
#include "../utility/socket/socket.h"
#include "../utility/process/process.h"
#include "libzuluCrypt-exe.h"
#include "security.h"

/*
 * These functions are here and not in libzuluCrypt-exe.h because they use string_t and the type is not supposed to be
 * seen in public API
 */

/*
 * global_variable_user_uid global variable is defined in security.c
 */
extern uid_t global_variable_user_uid ;

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
int zuluCryptPartitionIsSystemPartition( const char * dev,uid_t uid ) ;

/*
 * this function is defined in is_luks.c
 */
int zuluCryptVolumeIsNotLuks( const char * dev ) ;

/*
 * this function is defined in partitions.c
 */
int zuluCryptPrintDeviceProperties( int type ) ;

/*
 * this function is defined in security.c
 */
int zuluCryptSecurityCheckPartitionPermissions( uid_t uid ) ;

/*
 * this function is defined in partitions.c
 */
char * zuluCryptDeviceFromLabel( const char * label ) ;

/*
 * this function is defined in security.c
 */
int zuluCryptUserIsAMemberOfAGroup( uid_t uid,const char * groupname ) ;

/*
 * this function is defined in security.c
 */
int zuluCryptSecurityPathIsValid( const char * path,uid_t uid ) ;

/*
 * this function is defined in get_opts.c
 */
void zuluCryptEXEGetOptsSetDefault( struct_opts * stopts ) ;

/*
 * this function is defined in mount_flags.c
 */
int zuluCryptMountFlagsAreNotCorrect( const char * mode,uid_t uid,unsigned long * flags ) ;

/*
 * this function is defined in security.c
 */
int zuluCryptSecurityGainElevatedPrivileges( void ) ;

/*
 * this function is defined in security.c
 */
int zuluCryptSecurityDropElevatedPrivileges( void ) ;

/*
 * this function is defined in security.c
 */
char * zuluCryptSecurityEvaluateDeviceTags( const char * tag,const char * values ) ;

/*
 * this function is defined in security.c
 */
string_t zuluCryptSecurityGetFileSystemFromDevice( const char * path ) ;

/*
 * this function is defined in real_path.c
 */
int zuluCryptPathStartsWith( const char * path,const char * start ) ;

/*
 * this function is defined in real_path.c
 */
int zuluCryptPathDoesNotStartsWith( const char * path,const char * start ) ;

/*
 * this function is defined in mount_fs_options.c
 */
int zulucryptFileSystemIsNotSupported( const char * fs ) ;

/*
 * this function is defined in ../lib/real_path.c
 */
char * zuluCryptRealPath( const char * path ) ;

/*
 * this function is defined in clear_dead_mapper.c
 */
void zuluCryptClearDeadMappers( uid_t uid ) ;

/*
 * this function is defined in save_and_restore_luks_header.c 
 */
int zuluCryptHeaderMatchBackUpHeader( const char * device,const char * header_backup,uid_t uid ) ;

/*
 * this function is defined in bind.c
 */
int zuluCryptBindUnmountVolume( stringList_t mountinfo,const char * device,uid_t uid ) ;

/*
 * this function is defined in bind.c
 */
int zuluCryptBindMountVolume( const char * device,string_t,unsigned long flags ) ;

/*
 * this function is defined in bind.c
 */
int zuluCryptBindSharedMountPointPathTaken( string_t path ) ;

/*
 * this function is defined in ../pluginManager/zuluCryptPluginManager.c 
 */
string_t zuluCryptPluginManagerGetKeyFromModule( const char * device,const char * name,const char * uuid,uid_t uid,const struct_opts * opts ) ;


#endif 
