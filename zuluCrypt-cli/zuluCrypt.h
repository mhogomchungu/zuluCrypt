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
 * input : path to a partition/file to be checked if it is a luks device
 * return values:
 *	 0 - the device is a cryptsetup device of type "luks"
 * 	 1 - the device is not a crptsetup device of type "luks", it could be of type "plain" though
 * *     2 - ERROR: couldnt find cryptsetup.so library in /usr/local/lib,/usr/lib and /lib
 */
int is_luks(const char * device) ;


/*
 * return values:
 *	0 - success, the encrypted volume was opened and mounted successfully
 * 	1 - ERROR: No free loop device to use
 * 	2 - ERROR: There seem to already be an opened volume associated with "mapping_name" argument
 * 	3 - ERROR: device does not exist.
 * 	4 - ERROR: wrong passphrase
 * 	5 - ERROR: Cant create a mount point because a file/folder with the same exist at the mount point
 *	6 - ERROR: key file does not exist :
 *      7 - ERROR: couldnt find cryptsetup.so library in /usr/local/lib,/usr/lib and /lib
 */
int open_volume(const char *device, // path to a file/partition to be opened
		const char * mapper,// mapper name( will show up in /dev/mapper/map )
		const char *m_point,// mount point path, opened volume will be mounted on this path
		uid_t id,           // owner of the mount point will have this id with rwx------ permissions 
		const char * mode,  // "ro" or "rw",the former means open volume in read only mode,
				    // the latter means open in read/write mode
		const char *pass,   // encrypted volume passphrase to be used to open the volume
		const char * source // "-p" or "-f",the latter means pass is a keyfile, the former means 
		                    // pass is "naked" i.e already exist in memory 
	       ) ;	       
	       
/*
 * input :  mapper name used when the volume was opened
 * return values:
 * 	0 - success
 * 	1 - unmounting the mount point failed,mount point or one or more files are in use
 * 	2 - close failed, encrypted volume associated with mapping_name argument is not opened * 	
 * 	3 - ERROR: couldnt find cryptsetup.so library in /usr/local/lib,/usr/lib and /lib
  */
int close_volume(const char * mapper) ; 


/*
 * input :  mapper name used when the volume was opened
 * 
 * output is a pointer to a string with volume info.
 * remember to free the pointer when done with the output.
 * 
 * NULL output means: ERROR: couldnt find cryptsetup.so library in /usr/local/lib,/usr/lib and /lib
 * or couldnt open volume
 */
char *status( const  char * mapper );


/*
 * return values:
 *      0 - success
 * 	1 - ERROR: device argument does not point to a file or partition
 * 	2 - ERROR: wrong argument. (probably mistyped fs and rng arguments
 * * 	3 - ERROR: couldnt find cryptsetup.so library in /usr/local/lib,/usr/lib and /lib
 */
int create_volume(const char * device,    // path to a file or partition
		  const  char * fs,       //file system to use in the volume(ext2,ext3.ext4,vfat)
		  const char * type,      //type of volume to create( luks or plain )
		  const char * passphrase,//passphrase to use to create the volume
		  const char *rng);       //random number generator to use ( /dev/random or /dev/urandom )
		                          //required when creating luks volume, just pick one if you
		                          //creating a plain device, it will be ignored		                        

/*
 * INPUT: 
 * 	device      - path to an encrypted file or partition
 * 	existingkey - a key that already exist in the encrypted volume
 * 	new key     - new key to be added to the volume
 * 
 * return value:
 * 	0 - success, the new key was added successfully
 *      1 - ERROR: The presented key does not exist in the volume
 *      2 - ERROR: could not open encrypted volume
 *      3 - ERROR: device either doesnt exist or not a luks device
 * 	4 - ERROR: couldnt find cryptsetup.so library in /usr/local/lib,/usr/lib and /lib
 */
int add_key(const char * device, const char * existingkey,const  char * newkey) ;



/*
 * INPUT: device:      path to an encrypted device
 *        passphrase: a key already in the volume to be removed
 * 
 * return value: 
 * 0 - success - a key is successfully removed
 * 1 - ERROR: device is not a luks device or does not exist
 * 2 - ERROR: passphrase is not present in the volume
 * 3 - ERROR: could not open luks device
 * 4 - ERROR: couldnt find cryptsetup.so library in /usr/local/lib,/usr/lib and /lib
 */
int remove_key(const  char * device , const char * passphrase ) ;

/*
 * INPUT: device - path to encrypted volume
 * 
 * OUTPUT:
 * slots: array of 8 digits representing empty slots,
 * 	0 is for disabled/unoccupied/inactive
 * 	1 is for enabled/occupied/active 	
 *      2 is for invalid
 * 	3 is for last active key
 * 
 *       example:
 *	00100000 means, slot number 3 is occupied/enabled, the rest are not(emply/disabled).
 * 
 * return value:
 * 0 - success  
 * 1 - ERROR: device does not exist or is not a luks device
 * 2 - ERROR: could not open luks device  
 * 3 - ERROR: couldnt find cryptsetup.so library in /usr/local/lib,/usr/lib and /lib
 */
int empty_slots(char * slots , const char * device ) ;


#ifdef __cplusplus
}
#endif

#endif