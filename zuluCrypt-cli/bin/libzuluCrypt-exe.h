  /* 
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

  
#ifndef ZULUCRYPTEXE
#define ZULUCRYPTEXE

#ifdef __cplusplus
extern "C" {
#endif	
/*
 * this structure holds command line arguments. * 
 * It is instantiated in main.c * 
 */
typedef struct struct_opts_1{
	const char * plugin_path ;
	const char * device ;
	const char * mount_point ;
	const char * m_opts ;
	const char * fs_opts ;
	const char * key_source ;
	const char * key ;
	const char * fs ;
	const char * type ;
	const char * rng ;
	const char * existing_key_source ;
	const char * existing_key ;
	const char * new_key ;
	const char * new_key_source ;
	const char * argv ;
	const char * tcrypt_hidden_volume_size ;
	const char ** tcrypt_hidden_volume_key  ;
	const char ** tcrypt_hidden_volume_key_file ;
	char * const * env ;
	char action ;
	int partition_number ;
	int dont_ask_confirmation ;
	int open_no_mount ;
	int mount_point_option ;
	int print_partition_type ;
	int share ;
}struct_opts;

/*
 * these two defines are used in save_and_restore_luks_header.c  
 */
#define LUKS_HEADER_RESTORE 1
#define LUKS_HEADER_SAVE 0

/*
 * get command line arguments and in struct_opts structure to be used in various calls * 
 */
void zuluCryptEXEGetOpts( int argc,char * argv[],struct_opts * ) ;

/*
 * this function is responsibe for creating and restoring luks header.
 * 
 * the function is defined in save_and_restore_luks_header.c
 */
int zuluCryptEXESaveAndRestoreLuksHeader( const struct_opts * opts,uid_t uid,int option  ) ;

/*
 * this function is responsibe for printing information about an opened volume.
 * It is defined in volume_info.c  
 */
int zuluCryptEXEVolumeInfo( const char * mapper,const char * device,uid_t ) ;

/*
 * this function is responsibe for closing an opened volume.
 * It is defined in close_volume.c 
 */
int zuluCryptEXECloseVolume( const char * device,const char * mapping_name,uid_t ) ;

/*
 * this function is responsibe for opening volumes.
 * It is defined in open_volume.c * 
 */
int zuluCryptEXEOpenVolume( const struct_opts *,const char * mapping_name,uid_t uid ) ;

/*
 * this function is responsibe for creating volumes.
 * It is defined in create_volume.c
 */		   
int zuluCryptEXECreateVolume( const struct_opts *,const char * mapping_name,uid_t uid ) ;

/*
 * this function is responsibe for adding keys to luks volumes.
 * It is defined in add_key.c
 */
int zuluCryptEXEAddKey( const struct_opts *,uid_t ) ;

/*
 * this function is responsibe for removing keys from luks files.
 * It is defined in remove_key.c
 */		
int zuluCryptEXERemoveKey( const struct_opts *,uid_t ) ;

/*
 * this function is responsibe for checking if a path exist or not.
 * It is defined in ../lib/is_path_valid.c
 */
int zuluCryptIsPathValid( const char * path ) ;

/*
 * this function is responsibe for creating a mount point after it checks to make sure a user who started the tool
 * had writing access to the parent folder.
 * It is defined in security.c * 
 */
int zuluCryptCreateMountPoint( const char * path,uid_t uid ) ;

/*
 * this function checks if a user who started the tool has writing access to a file or device they want this tool to 
 * write to.
 * It is defined in security.c
 */
int zuluCryptCanOpenPathForWriting( const char * path,uid_t uid ) ;

/*
 * this function checks if a user who started the tool has reading access to a file or device they want this tool to 
 * read from.
 * It is defined in security.c
 */
int zuluCryptCanOpenPathForReading( const char * path,uid_t uid ) ;

/* 
 * check if the volume has atleast one corrupted key slot
 */ 
void zuluCryptCheckInvalidKey( const char * device ) ;

/*
 * this function checks if a device is a system partition or not.
 * 
 * the function is defined in partitions.c * 
 */
int zuluCryptCheckIfPartitionIsSystemPartition( const char * device ) ;

/*
 * defined in crypt_file.c
 */
int zuluCryptExeFileEncrypt( const struct_opts *,uid_t uid ) ;

/*
 * defined in crypt_file.c
 */
int zuluCryptExeFileDecrypt( const struct_opts *,uid_t uid ) ;

/*
 * defined in check_opened_mapper.c
 * 
 * the function checks to see if the argument mapper has an entry in crypt_get_dir().
 * 
 * the function is used to prevent performing operations like creating a volume on devices with mapper open.
 */
int zuluCryptCheckOpenedMapper( const char * mapper ) ;

/*
 * check if a  device with a path "path" as an entry in /etc/mtab.
 * 
 * defined in process_mountinfo.c 
 */
int zuluCryptCheckIfMounted( const char * path ) ;

/*
 * function defined at write_device_with_junk.c
 */
int zuluCryptEXEWriteDeviceWithJunk( const struct_opts * opts,const char * mapping_name,uid_t uid ) ;

/*
 * function defined at write_device_with_junk.c
 */
int zuluCryptEXEOpenPlainAsMe(const struct_opts * opts,const char * mapping_name,uid_t uid ) ;

/*
 * defined in lib/status.c
 * remember to free() the return value when done with it
 * */
char * zuluCryptVolumeDeviceName( const char * ) ;

/*
 * defined at help.c
 * */
void zuluCryptEXEHelp( void ) ;

/*
 * defined at partitions.c
 */
int zuluCryptCheckSystemTools( void ) ;

/*
 * defined in partitions.c
 * remember to free() the return value when done with it * 
 */
char * zuluCryptDeviceFromUUID( const char * uuid ) ;

/*
 * defined in partitions.c
 */
int zuluCryptPrintPartitions( int option,int info,uid_t uid ) ;

/*
 * defined at process_mountinfo.c
 * remember to free() a returned pointer when done with it.
 */
char * zuluCryptGetMountPointFromPath( const char * path ) ;


#ifdef __cplusplus
}
#endif

#endif
