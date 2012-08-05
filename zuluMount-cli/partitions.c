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
#include <blkid/blkid.h>

#include "../zuluCrypt-cli/string/String.h"
#include "../zuluCrypt-cli/string/StringList.h"
#include "../zuluCrypt-cli/constants.h"

/*
 * This source file deals with parsing partition list from "/proc/partitions,"/etc/fstab" and "/etc/mtab".
 * 
 * For security reasons,a normal user it is not allowed to create volumes in system partitions.
 * 
 * A System partition is defined as a partition with an active entries in /etc/fstab and/or /etc/crypttab.
 * 
 * This policy is in place to prevent a normal user from attempting to create volumes in internal partitions
 * intentially to destroy other people's data.
 * 
 * External,pluggable usb based partitions are not considered to be system partitions and the tool can be used to create volumes in those.
 * 
 * Internal partitions are considered part of the system and the policy is in place to make sure a normal user does not
 * perform actions reserved for root user.
 * 
 */

/*
 * this function reads a line from a fine, it does what gets() does,it just handles the memory dynamically * 
 */

/*
 * major minor  #blocks  name
 * 
 8        0   78150744 sda *
 8        1   11566768 sda1
 8        2          1 sda2
 8        5   66581361 sda5
 8       16  312571224 sdb
 8       17    1044193 sdb1
 8       18          1 sdb2
 8       21  311524416 sdb5
 8       32     250879 sdc
 8       33     250608 sdc1
 *
 * above output is the output of "cat /proc/partitions" and below function was build again it.
 * The list of partitions is taken from the 4th field and only sdX and hdY entries are taken
 * 
 */

stringList_t mount_partitionList( void )
{
	const char * device ;	
	
	string_t st_1 ;
	
	size_t i ;
	size_t j ;	
	ssize_t index ;	
	
	stringList_t stl ;	
	stringList_t stl_1 = NULL ;	
	
	string_t st = StringGetFromVirtualFile( "/proc/partitions" ) ;	
	
	if( st == NULL )
		return NULL ;
	
	stl = StringListStringSplit( &st,'\n' ) ;
	
	if( stl == NULL ){
		StringDelete( &st ) ;
		return NULL ;
	}
	
	j = StringListSize( stl )  ;
	
	for( i = 0 ; i < j ; i++ ){
		
		st = StringListStringAt( stl,i ) ;

		index = StringLastIndexOfChar( st,' ' ) ;

		if( index == -1 )
			continue ;
		
		device = StringContent( st ) + index + 1 ;
		
		if( strlen( device  ) <= 3  )
			continue ;
	
		if( ( strncmp( device,"hd",2 ) == 0 || strncmp( device,"sd",2 ) == 0 ) ){			
			st_1 = String( "/dev/" ) ;
			StringAppend( st_1,device ) ;
			stl_1 = StringListAppendString( stl_1,&st_1 ) ;			
		}	
	}
	
	StringListDelete( &stl ) ;
	return stl_1 ;
}

int mount_print_partitions( void )
{
	size_t i ;
	size_t j ;
	
	stringList_t stl = mount_partitionList() ;
	
	if( stl == NULL ){
		printf( "ERROR: unable to print requested list of partitions\n" ) ;
		return 1 ;
	}
	
	j = StringListSize( stl ) ;
	
	for( i = 0 ; i < j ; i++ )	
		StringListPrintLineAt( stl,i ) ;
	
	StringListDelete( &stl ) ;
	
	return 0 ;
}
