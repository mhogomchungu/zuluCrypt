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
#ifndef EXECUTABLES_H

#define EXECUTABLES_H

#include "bin_path.h"

#define ZULUCRYPTmkfs        	"/sbin/mkfs" 
#define ZULUCRYPTmount      	"/bin/mount"  
#define ZULUCRYPTmkfs_dir      	"/sbin"  

#define ALL_PARTITIONS        1
#define SYSTEM_PARTITIONS     2
#define NON_SYSTEM_PARTITIONS 3

#define UUID_LENGTH 36
#define DEVICE_LENGHT 12

#define OPEN 1
#define CLOSE 2

#define READ 1
#define WRITE 0

/*
 * The below numbers are cryptsetup default numbers.
 */
#define KEY_MAX_SIZE 512
#define KEYFILE_MAX_SIZE 8192000
#define INTMAXKEYZISE  8192000
#define CHARMAXKEYZISE "8192000"

#endif
