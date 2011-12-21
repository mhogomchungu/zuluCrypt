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

/**
 * Return the version string of the library * 
 */
char * version(void) ;

/**
 * This function checks to see if a volume is a luks volume or not.
 * 
 * input : path to a partition/file to be checked if it is a luks device
 * return values:
 *	 0 - the device is a cryptsetup device of type "luks"
 * 	 1 - the device is not a crptsetup device of type "luks".
 */
int is_luks(const char * device) ;


/**
 * This function openes a volume,creates a mount point and mount the opened volume.
 * 
 * return values:
 *	0 - success, the encrypted volume was opened and mounted successfully
 * 	1 - ERROR: No free loop device to use
 * 	2 - ERROR: There seem to already be an opened volume associated with "mapping_name" argument
 * 	3 - ERROR: device does not exist.
 * 	4 - ERROR: wrong passphrase
 * 	5 - ERROR: Cant create a mount point because a file/folder with the same exist at the mount point
 *	6 - ERROR: key file does not exist :
 *	8 - ERROR: failed to open device
 *      12 - ERROR: could not get a lock on /etc/mtab~
 */
int open_volume(const char *device, // path to a file/partition to be opened
		const char * mapper,// mapper name( will show up in /dev/mapper/ )
		const char *m_point,// mount point path, opened volume will be mounted on this path
		uid_t id,           // owner of the mount point will have this id with rwx------ permissions 
		const char * mode,  // "ro" or "rw",the former means open volume in read only mode,
				    // the latter means open in read/write mode
		const char *pass,   // encrypted volume passphrase to be used to open the volume
		const char * source // "-p" or "-f",the latter means pass is a keyfile, the former means 
		                    // pass is "naked" i.e already exist in memory 
	       ) ;	       
	       
/**
 * This function unmount the mounted opened volume,delete the mount point and then close the volume.
 * 
 * input :  mapper name used when the volume was opened
 * output:  mount point path, just incase you need it. You can pass NULL if you dont.
 * 
 *  path to mount point will be allocated dynamically on success and hence you should free it ONLY when 
 *  the function return with a success.
 * 
 * return values:
 * 	0 - success
 * 	1 - ERROR: unmounting the mount point failed,mount point or one or more files are in use
 * 	2 - ERROR: close failed, encrypted volume associated with mapping_name argument is not opened  	
 * 	3 - ERROR: close failed, volume does not have an entry in /etc/mtab
 *      
  */
int close_volume(const char * mapper,//mapper is the full address of the volume as it appears at /dev/mapper
		 char ** mount_point ) ; //returned pointer to mount point
					

/**
 * This function closes a mapper
 * 
 * return values:
 * 0 - success
 * 2 - ERROR: could not close the volume, probably because the volume is still mounted.
 * 
 */
int close_mapper( const char * mapper ) ;//mapper is the full address of the volume as it
					//appears at /dev/mapper

/**
 * This function unmounts a volume* 
 * 
 * return values
 * 0 - success
 * 1 - ERROR: mapper does not have an entry in fstab
 * 2 - ERROR: the mount point and/or one or more files are in use
 * 3 - ERROR: volume does not have an entry in /etc/mtab
 * 4 - ERROR: could not get a lock on /etc/mtab~
  */
int unmount_volume( const char * mapper, //mapper is the full address of the volume as it appears at /dev/mapper
		    char * m_point ) ;  // read close volume docs for more info on this argument
					
/**
 * This function mounts a volume
 * 
 * return values:
 * 0 -  sucess
 * 4 -  ERROR: mount failed, couldnt find valid file system in the volume
 * 12 - ERROR: could not get a lock on /etc/mtab~ * 
 */
int mount_volume( const char * mapper, // path to a file or partition to mount
		  const char * m_point,// mount point
		  const char * mode,   // mode, options are "ro" and "rw" for read only and read/write respectively
		  uid_t id ) ;         // user id the mount point should use

/**
 * This function returns a pointer to string with volume status information.
 * An example output:
 * 
 * /dev/mapper/zuluCrypt-luks is active and is in use.
 * type:      LUKS1
 * cipher:    cbc-essiv:sha256
 * keysize:   256 bits
 * device:    /dev/loop1
 * loop:      /home/ink/luks
 * offset:    4096 sectors
 * size:      200704 sectors
 * mode:      readonly
 * 
 * input :  mapper name used when the volume was opened. Mapper name is the name that shows up in /dev/mapper
 * 
 * output is a pointer to a string with volume info.
 * 
 * remember to free the returned pointer when done with the output. 
 */
char *status( const  char * mapper );//mapper is the full address of the volume as it
					//appears at /dev/mapper


/**
 * This function creates an encrypted volume.
 * return values:
 *      0 - success
 * 	1 - ERROR: device argument does not point to a file or partition
 * 	2 - ERROR: wrong argument. (probably mistyped fs and rng arguments
 * 	3 - ERROR: could not create the volume
 * 
 * NOTE: This function expected mkfs executable to be present and its full path to be /sbin/mkfs
 */
int create_volume(const char * device,    // path to a file or partition
		  const char * fs,       //file system to use in the volume(ext2,ext3.ext4,vfat)
		  const char * type,      //type of volume to create( luks or plain )
		  const char * passphrase,//passphrase to use to create the volume
		  const char * rng);       //random number generator to use ( /dev/random or /dev/urandom )
		                          //required when creating luks volume, just pick one if you
		                          //creating a plain device, it will be ignored		                        

/**
 * This function adds a key to a luks volume
 * 
 * return value:
 * 	0 - success, the new key was added successfully
 *      1 - ERROR: The presented key does not exist in the volume
 *      2 - ERROR: could not open encrypted volume
 *      3 - ERROR: device either doesnt exist or not a luks device
 */
int add_key(const char * device,     //path to an encrypted file or partition
	    const char * existingkey,//a key that already exist in the encrypted volume
	    const char * newkey) ;  //new key to be added to the volume

/**
 * This function deletes a key from a luks volume.
 * 
 * return value: 
 * 0 - success - a key is successfully removed
 * 1 - ERROR: device is not a luks device or does not exist
 * 2 - ERROR: passphrase is not present in the volume
 * 3 - ERROR: could not open luks device
 */
int remove_key(const char * device ,      //path to an encrypted device
	       const char * passphrase ) ;//a key already in the volume to be removed

/**
 *This function gives information about slots in a luks volume. 
 * 
 * return value: 
 * NULL if an error occurs(if the device path is invalid or does not point to luks device.
 * 
 * If no error occur a sting made up of 0,1,2 and 3 is returned. Make sure to free it when you are done.
 * 
 * 	0 is for disabled/unoccupied/inactive
 * 	1 is for enabled/occupied/active 	
 *      2 is for invalid
 * 	3 is for last active key
 * 
 *       example:
 *	00100000 means, slot number 3 is occupied/enabled, the rest are not(emply/disabled).  
 */
char * empty_slots(const char * device ) ; 

/**
 * This function just opens a luks volume, doesnt create a mount point and doesnt mount it.
 * 
 *return values:
 * 0 - success 
 * 1 - ERROR: presented key does not exist in the volume
 * 2 - ERROR: failed to open device
 * 3 - ERROR: device path does not point to a device
 * 4 - ERROR: key file does not exist
 */
int open_luks( const char * device,      // path to encrypted file or partition
	       const char * mapping_name,// mapper name to use
	       const char * mode,        // "ro" or "rw" for opening in read only or read and write
	       const char * source,      // "-f" or "-p" for passphrase is a path to a key file or a 					  // "naked" passphrase already in memory respectively 
	       const char * passphrase ) ;// passphrase to use to open the volume
	
/**
 * This function creates a luks volume
 * 
 * return values:
 * 0 - success 
 * 1 - ERROR: could not initialize the device
 * 2 - ERROR: could not format the device
 * 3 - ERROR: could not add passphrase to the device
 * 4 - ERROR: device path does not point to a device 
 * 
 */
int create_luks(const char * device,    // path to a file or partition to create a volume in
		const char * passphrase,// passphrase to use to create a volume
		const char * rng) ;	//random number generator( /dev/random or /dev/urandom)
		
		
/**
 * This function just opens a plain volume, it doesnt create a mount point and it doesnt mount it.
 * return values:
 * 0 - success 
 * 2 - ERROR: failed to open the volume.
 * 3 - ERROR: device path does not point to a device
 * 4 - ERROR: key file does not exist
 */
int open_plain( const char * device,      // path to encrypted file or partition
		const char * mapping_name,// mapper name to use
		const char * mode,        // "ro" or "rw" for opening in read only or read and write
		const char * source,      // "-f" or "-p" for passphrase is a path to a key file or a "naked" passphrase already in memory respectively
		const char * passphrase,  // passphrase to use to open the volume
		const char * cipher );	  // cipher to use, default is "cbc-essiv:sha256" for current cryptsetup default option.  


/**
 * This function returns a device address given a mapper address.
 * Ex. IF a mapper address exists named "/dev/mapper/XYZ" and this mapper opens a volume
 * in /dev/sdc1, then calling this function wih the mentioned mapper address will return "/dev/sdc1".
 *
 * If the mapper open a regular file, the full path to the file is returned and not its loop back device.
 * 
 * NOTE: The address is stored in memory created by "malloc" command and hence you must free it with "free" command
 * when done with it 
 * 
 *  NULl is returned if the mapper device can not be opened for the reasons that include the mapper not being cryptsetup mapper.
 */
char * volume_device_name( const char * mapper ) ;

#ifdef __cplusplus
}
#endif

#endif