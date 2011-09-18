/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
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

#ifndef ZULUCRYPT
#define ZULUCRYPT

#ifdef __cplusplus
extern "C" {
#endif	

/*
 *  Arguments to different functions:
 * 
 * device        - path to a encrypted device to work with eg. /dev/sda5, /dev/loop5
 * 
 * mapping_name  - a name to use to map the device(name will show up at /dev/mapper). All access to
 *                 the device will happen through this mapping_name
 * 
 * output        - a buffer to hold command output. 
 * 
 * size          - size of the output buffer 
 * 
 * fs            - file system type to use when creating an encrypted volume
 * 
 * type          - type of encrypted volume to create, options are "plain" and "luks"
 * 
 * passphrase    - passphrase to open an encrypted volume
 */


/*
 * outputs:
 *	 0 - the device is a cryptsetup device of type "luks"
 * 	 1 - the device is not a crptsetup device of type "luks", it could be of type "plain" though
 */
int is_luks(char * device) ;





/*
 * outputs:
 *	0 - success, the encrypted volume was opened and mounted successfully
 * 	1 - ERROR: open failed, given path does not point to a file or partition
 * 	2 - ERROR: There seem to already be an opened volume associated with "mapping_name" argument
 * 	3 - ERROR: Can not open and encrypted file. No free loop device to use.
 * 	4 - ERROR: wrong passphrase
 * 	5 - ERROR: Cant create a mount point because a file/folder with the same exist at the mount point
 *	6 - ERROR: encrypted volume has an unrecognized file system type,supported file systems are : ext4,ext3,ext2,vfat,reiserfs
 */
int open_volume(char *device, char * mapping_name, char *m_point, uid_t id,char * mode, char * passphrase) ;







/*
 * outputs:
 * 	2 - close failed, encrypted volume associated with mapping_name argument is not opened
 * 	1 - unmounting the mount point failed,mount point or one or more files are in use.
 * 	0 - success
  */
int close_volume(char * mapping_name) ;




void status(  char * mapping_name , char * output, int size);





/*
 * outputs:
 * 	1 - ERROR: device argument does not point to a file or partition
 * 	2 - ERROR: A volume associated with the same device address is already opened.
 */
int create_volume(char * device, char * fs,char * type, char * passphrase);  

int add_key(char * device, char * existingkey, char * newkey) ;


#ifdef __cplusplus
}
#endif

#endif