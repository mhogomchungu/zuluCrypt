/*
 *
 *  Copyright (c) 2011
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

#ifndef ZULUCRYPT_LIB
#define ZULUCRYPT_LIB

#include "../utility/string/String.h"
#include "../utility/string/StringList.h"
#include "../utility/process/process.h"
#include "../constants.h"
#include "libzuluCrypt.h"
#include "../bin/bash_special_chars.h"

#include <gcrypt.h>

#include "support_whirlpool.h"

/*
 * below header is created at build time,it is set by CMakeLists.txt located in the root folder
 */
#include "version.h"

typedef struct{
	const char * device ;
	const char * m_point ;
	const char * fs ;
	const char * opts ;
	uid_t uid ;
	unsigned long m_flags ;
	const char * fs_flags ;
}m_struct;

typedef struct{
	const char * device ;
	const char * header_source ;
	const char * tmp_path ;
	const char * opt ;
	const char * key ;
	const char * rng ;
	const char * header_key ;
	const char * header_key_source ;
	const char * header_new_key ;
	const char * header_new_key_source ;
	uid_t uid ;
	string_t ( *getKey )( int * ) ;
}info_t ;

typedef struct{
	const char * device ;
	const char * offset ;
	const char * mapper_name ;
	const char * key ;
	const char * m_point ;
	const char * fs_opts ;
	const char * m_opts ;
	size_t       key_len ;
	uid_t        uid ;
	int          volume_type ;
	int          key_source ;
	int luks_detached_header ;
	int tcrypt_keyfiles_count ;
	int veraCrypt_volume ;
	const char * const * tcrypt_keyfiles ;
	unsigned long m_flags ;
}open_struct_t ;

typedef struct{
	const char * device ;
	const char * passphrase ;
	size_t       passphrase_size ;
	const char * const * keyfiles ;
	size_t       keyfiles_number ;
	const char * passphrase_h ;
	size_t       passphrase_h_size ;
	const char * const * keyfiles_h ;
	size_t       keyfiles_h_number ;
	const char *  encryption_options ;
	u_int64_t    hidden_volume_size ;
	const char * fs ;
	const char * fs_h ;
}create_tcrypt_t ;

/*
 * we only support whirlpool with cryptsetup >= 1.6.4 and libgcrypt >= 1.6.1
 *
 * read section 8.3  of cryptsetup FAQ for more info.
 */

static inline int zuluCryptWhirlpoolIsSupported()
{
#ifdef GCRYPT_VERSION_NUMBER
	return GCRYPT_VERSION_NUMBER >= 0x010601 && SUPPORT_WHIRLPOOL ;
#else
	return SUPPORT_WHIRLPOOL ;
#endif
}

/*
 * zuluCryptCreateTCryptVolume() is defined in create_tcrypt.c
 */
int zuluCryptCreateTCryptVolume( const create_tcrypt_t * ) ;

/*
 * zuluCryptOpenVolume_1() is defined in open_volume.c
 */
int zuluCryptOpenVolume_1( const open_struct_t * ) ;

/*
 * zuluCryptOpenVolume_1() is defined in open_volume.c
 */
int zuluCryptOpenVolume_2( const open_struct_t * ) ;

/*
 * zuluCryptOpenPlain_1() is defined in open_plain.c
 */
int zuluCryptOpenPlain_1( const open_struct_t * ) ;

/*
 * zuluCryptOpenLuks_1() is defined in open_luks.c
 */
int zuluCryptOpenLuks_1( const open_struct_t * ) ;

/*
 * zuluCryptOpenLuks_1() is defined in open_luks.c
 */
int zuluCryptOpenLuks_2( const open_struct_t * ) ;

/*
 * zuluCryptOpenTcrypt_1() is defined in open_tcrypt.c
 */
int zuluCryptOpenTcrypt_1( const open_struct_t * ) ;

/*
 * zuluCryptOpenVolume_0() is defined in open_volume.c
 */
int zuluCryptOpenVolume_0( int( *function )( const open_struct_t * ),const open_struct_t * ) ;


int zuluCryptAttachLoopDeviceToFileUsingFileDescriptor( int fd_path,int * fd_loop,int mode,string_t * loop_device ) ;

/*
 * this function is defined in status.c
 */
void zuluCryptFileSystemProperties( string_t p,const char * mapper,const char * m_point ) ;

/*
 * this function is defined in create_mapper_name.c
 */
const char * zuluCryptMapperPrefix( void ) ;

string_t zuluCryptCreateMapperName( const char * device,const char * mapping_name,uid_t uid,int i ) ;

/*
 * this function checks if path exists or not.
 * return 1 if the path exists
 * return 0 if it does not
 */
int zuluCryptPathIsValid( const char * path ) ;

/*
 * this function checks if a path exists or not
 * return 1 if the path does not exists
 * return 0 if the path exist
 */
int zuluCryptPathIsNotValid( const char * path ) ;

/*
 * zuluCryptDecodeMountEntry() is defined in mount_volume.c
 */
const char * zuluCryptDecodeMountEntry( string_t ) ;

/*
 * zuluCryptDecodeMountEntry() is defined in mount_volume.c
 */
const char * zuluCryptEncodeMountEntry( string_t ) ;

/*
 *this function is deined in ../lib/process_mountinfo.c
 */
stringList_t zuluCryptOpenedVolumesList( uid_t ) ;

/*
 * this is defined in print_mounted_volumes.c
 */
int zuluCryptPartitionIsMounted( const char * device ) ;

/*
 *  parse "/etc/fstab" and return a field at position "pos" on a line
 *  that corresponds to device "device".
 */
#define MOUNTPOINT   1
#define FILESYSTEM   2
#define MOUNTOPTIONS 3
#define DUMPOPTION   4
#define FSCKOPTION   5

string_t zuluCryptGetMountOptionsFromFstab( const char * device,int pos,uid_t uid ) ;

/*
 * this function is defined in process_mountinfo.c
 */

string_t zuluCryptGetMountEntry( const char * path ) ;

/*
 * this function is defined in process_mountinfo.c
 */
string_t zuluCryptGetMountEntry_1( stringList_t stl,const char * path ) ;

/*
 * this function is defined in is_luks.c
 */
int zuluCryptVolumeIsNotLuks( const char * dev ) ;

/*
 * this function is defined in ./create_loop_device.c
 */
int zuluCryptAttachLoopDeviceToFile( const char * path,int mode,int * fd,string_t * loop_device ) ;

/*
 * this function is defined in ./mount_volume.c
 */
string_t zuluCryptGetFileSystemFromDevice( const char * device ) ;

/*
 * this function is defined in ./create_loop_device.c
 */
char * zuluCryptLoopDeviceAddress( const char * device ) ;

/*
 * this function is defined in ./create_loop_device.c
 */
char * zuluCryptLoopDeviceAddress_1( const char * device ) ;

/*
 * this function is defined in ./create_loop_device.c
 */
string_t zuluCryptLoopDeviceAddress_2( const char * device ) ;

/*
 * this function is defined in ./create_loop_device.c
 */
char * zuluCryptGetLoopDeviceAddress( const char * device ) ;

/*
 * this function is defined in ./process_mountinfo.c
 */
stringList_t zuluCryptGetMoutedListFromMountInfo( void ) ;

/*
 * this function is defined in ./process_mountinfo.c
 */
char * zuluCryptGetMountPointFromPath( const char * path ) ;

/*
 * this function is defined in ./process_mountinfo.c
 */
char * zuluCryptGetMountPointFromPath_1( const char * path ) ;

/*
 * this function is defined in ./process_mountinfo.c
 */
char * zuluCryptResolveDevRoot( void ) ;

/*
 * this function is defined in ./process_mountinfo.c
 */
stringList_t zuluCryptGetFstabEntryList( const char * device,uid_t uid ) ;

/*
 * this function is defined in blkid_evaluate_tag.c
 */
char * zuluCryptDeviceFromUUID( const char * uuid ) ;

/*
 * this function is defined in blkid_evaluate_tag.c
 */
char * zuluCryptDeviceFromLabel( const char * label ) ;

/*
 * this function is defined in blkid_evaluate_tag.c
 */
char * zuluCryptDeviceFromPARTUUID( const char * p_uuid ) ;

/*
 * this function is defined in blkid_evaluate_tag.c
 */
char * zuluCryptDeviceFromPARTLABEL( const char * p_label ) ;

/*
 * this function is defined in blkid_evaluate_tag.c
 */
char * zuluCryptUUIDFromPath_1( const char * device ) ;

/*
 * this function is defined in volume_types.c
 */
int zuluCryptGetVolumeType( const char * device,const char * pass,size_t pass_size ) ;

/*
 * this function is defined in open_tcrypt.c
 */
int zuluCryptVolumeIsTcrypt( const char * device,const char * key,int key_source ) ;

/*
 * this function is defined in mount_fs_options.c
 */
int zuluCryptMountHasNotAllowedFileSystemOptions( uid_t uid,const char * fs_opts,string_t s ) ;

/*
 * this functioon is defined in ./real_path.c
 */
int zuluCryptPathDidNotChange( const char * path ) ;

/*
 * this functioon is defined in ./real_path.c
 */
int zuluCryptPathDeviceIsBlockDevice( const char * device ) ;

/*
 * this function is defined in mount_fs_options
 */
int zulucryptFileSystemIsSupported( const char * fs ) ;

/*
 * this function is defined in user_home_path.c
 */
string_t zuluCryptGetUserName( uid_t uid ) ;

/*
 * this function is defined in real_path.c
 */
char * zuluCryptRealPath( const char * path ) ;

/*
 * this function is defined in create_loop_device.c
 */
char * zuluCryptGetFileNameFromFileDescriptor( int fd ) ;

/*
 * this function is defined in file_path_security.c
 */
int zuluCryptGetDeviceFileProperties( const char * file,int * fd_path,int * fd_loop,char ** dev,uid_t uid ) ;

/*
 * this function is defined in file_path_security.c
 */
int zuluCryptSecureOpenFile( const char * path,int * fd_path,string_t * file,uid_t uid ) ;

/*
 * this function is defined in mount_volume.c
 */
stringList_t zuluCryptGetFstabList( uid_t ) ;

/*
 * this function is defined in process_mountinfo.c
 */
stringList_t zuluCryptGetMountInfoList( void ) ;

/*
 * this function is defined in process_mountinfo.c
 */
stringList_t zuluCryptGetMountInfoList_1( void ) ;

/*
 * this function is defined in status.c
 */
char * zuluCryptGetVolumeTypeFromMapperPath( const char * mapper ) ;

/*
 * this function is defined in create_volume.c
 */
int zuluCryptCreateFileSystemInAVolume( const char * fs,const char * device_mapper ) ;

/*
 * this function is defined in resolve_paths.c
 */
string_t zuluCryptConvertIfPathIsLVM( const char * path ) ;

/*
 * this function is defined in resolve_paths.c
 */
char * zuluCryptResolveMDPath( const char * path ) ;

/*
 * this function is defined in resolve_paths.c
 */
string_t zuluCryptResolveMDPath_1( const char * path ) ;

/*
 * this function is defined in file_path_security.c
 */
void zuluCryptDeleteFile( const char * file ) ;

/*
 * this function is defined in file_path_security.c
 */
void zuluCryptDeleteFile_1( string_t file ) ;

/*
 * this function is defined in mount_volume.c
 */
int zuluCryptFileSystemIsFUSEbased( const char * device ) ;

/*
 * this function is defined in status.c
 */
void zuluCryptFormatSize( u_int64_t number,char * buffer,size_t buffer_size ) ;

/*
 * zuluCryptCreateKeyFile() is defined in open_tcrypt.c
 */
string_t zuluCryptCreateKeyFile( const char * key,size_t key_len,const char * fileName ) ;

/*
 * zuluCryptCreateKeyFile_1() is defined in open_tcrypt.c
 */
string_t zuluCryptCreateKeyFile_1( string_t key,const char * fileName ) ;

/*
 * zuluCryptResolvePath_2() is defined in resolve_paths.c
 */
string_t zuluCryptResolvePath_2( const char * path ) ;

/*
 * zuluCryptResolvePath_1() is defined in resolve_paths.c
 */
string_t zuluCryptResolvePath_1( const char * path ) ;

/*
 * zuluCryptResolvePath_3() is defined in resolve_paths.c
 */
char * zuluCryptResolvePath_3( const char * path ) ;

/*
 * zuluCryptResolvePath() is defined in resolve_paths.c
 */
char * zuluCryptResolvePath( const char * path ) ;

/*
 * zuluCryptGetMoutedListFromMountInfo_1() is defined in process_mountinfo.c
 */
stringList_t zuluCryptGetMoutedListFromMountInfo_1( void ) ;

/*
 * zuluCryptGetMoutedListFromMountInfo() is defined in process_mountinfo.c
 */
stringList_t zuluCryptGetMoutedListFromMountInfo( void ) ;

/*
 * zuluCryptGetALoopDeviceAssociatedWithAnImageFile() is defined in create_loop_device.c
 */
char * zuluCryptGetALoopDeviceAssociatedWithAnImageFile( const char * path ) ;

/*
 * zuluCryptGetAListOfMountedVolumes() is defined in process_mountinfo.c
 */
stringList_t zuluCryptGetAListOfMountedVolumes( void ) ;

/*
 * zuluCryptModifyTcryptHeader() is defined in create_tcplay.c
 */
int zuluCryptModifyTcryptHeader( const info_t * info ) ;

#endif
