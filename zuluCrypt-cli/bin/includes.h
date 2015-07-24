 /*
  *
  *  Copyright (c) 2011-2015
  *  name : Francis Banyikwa
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

#define STRING_MEMORY_HACK 1

#include "../utility/string/String.h"
#include "../utility/string/StringList.h"
#include "../constants.h"
#include "../lib/libzuluCrypt.h"
#include "../utility/socket/socket.h"
#include "../utility/process/process.h"
#include "libzuluCrypt-exe.h"
#include "security.h"
#include "locale_path.h"
#include "../lib/includes.h"

typedef struct{

	int iteration_count ;
	string_t type ;

}tvcrypt ;

/*
 * these two defines are used in save_and_restore_volume_header.c
 */
#define VOLUME_HEADER_RESTORE 1
#define VOLUME_HEADER_SAVE 0

/*
 * These functions are here and not in libzuluCrypt-exe.h because they use string_t and the type is not supposed to be
 * seen in public API
 */

/*
 * global_variable_user_uid global variable is defined in security.c
 */
extern uid_t global_variable_user_uid ;

/*
 * zuluCryptTrueCryptVeraCryptVolumeInfo() is defined in create_volume.c
 */
void zuluCryptTrueCryptVeraCryptVolumeInfo( const char * type,tvcrypt * e ) ;

/*
 * this function is defined in security.c
 */
int zuluCryptSecurityConvertUID( uid_t uid,const char * u_id ) ;

/*
 * this function is defined in volumes.c
 */
int zuluCryptVolumeIsInSystemVolumeList( const char * device ) ;

/*
 * this function is defined in open_volume.c
 */
stringList_t zuluCryptCreateKeyFiles( const char * const * list,int s ) ;

/*
 * this function is defined in open_volume.c
 */
void zuluCryptDeleteKeyFiles( stringList_t stl ) ;

/*
 * this function is defined in module_system.c
 * It gets a passphrase from a .so file as a plugin.
 */
string_t GetKeyFromModule( const char * path,uid_t ) ;

/*
 * this function is defined in get_key_from_socket.c
 */
void zuluCryptGetKeyFromSocket( const char * path,string_t *,uid_t uid ) ;

/*
 * this function returns "$HOME/" and is defined in ../pluginManager/zuluCryptPluginManager.c
 */
string_t zuluCryptGetUserHomePath( uid_t ) ;

/*
 * this function is defined in volumes.c
 */
int zuluCryptPartitionIsSystemPartition( const char * dev,uid_t uid ) ;

/*
 * this function is defined in is_luks.c
 */
int zuluCryptVolumeIsNotLuks( const char * dev ) ;

/*
 * this function is defined in volumes.c
 */
int zuluCryptPrintDeviceProperties( int type ) ;

/*
 * this function is defined in security.c
 */
int zuluCryptSecurityCheckPartitionPermissions( uid_t uid ) ;

/*
 * this function is defined in volumes.c
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
string_t zuluCryptPluginManagerGetKeyFromModule( const char * device,const char * name,
						 const char * uuid,uid_t uid,const struct_opts * opts,int * r ) ;

/*
 * this function is defined in volumes.c
 */
stringList_t zuluCryptGetPartitionFromCrypttab( void ) ;

/*
 * this function is defined in volumes.c
 */
stringList_t zuluCryptGetPartitionFromConfigFile( const char * path ) ;

/*
 * this function is defined in volumes.c
 */
stringList_t zuluCryptPartitionList( void ) ;

/*
 * this function is defined in create_mount_point.c
 */
string_t zuluCryptCreateMountPoint( const char * device,const char * m_point,const char * m_opts,uid_t uid ) ;

/*
 * this function is defined in create_mount_point.c
 */
int zuluCryptMountPointPrefixMatch( const char * path,uid_t uid,string_t * m_point ) ;

/*
 * this function is defined in path_access.c
 */
int zuluCryptGetPassFromFile( const char * path,uid_t uid,string_t * st ) ;

/*
 * this function is defined in path_access.c
 */
char * zuluCryptUUIDFromPath( const char * device ) ;

/*
 * this function is defined in path_access.c
 */
char * zuluCryptEvaluateDeviceTags( const char * tag,const char * path ) ;

/*
 * zuluCryptRunTest() is defined in test.c
 */
int zuluCryptRunTest( void ) ;

/*
 * zuluCryptDeviceIsSupported() is defined in volumes.c
 */
int zuluCryptDeviceIsSupported( const char * device,uid_t uid ) ;

/*
 * zuluCryptPartitions() is defined in volumes.c
 */
stringList_t zuluCryptPartitions( int option,uid_t uid ) ;

/*
 * zuluCryptGetVolumeSize() is defined in volumes.c
 */
u_int64_t zuluCryptGetVolumeSize( const char * device ) ;

/*
 * zuluCryptGetAListOfAllPartitions() is defined in volumes.c
 */
stringList_t zuluCryptGetAListOfAllVolumes( void ) ;

#endif
