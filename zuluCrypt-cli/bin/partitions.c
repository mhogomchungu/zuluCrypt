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

#include "includes.h"

#include <blkid/blkid.h>

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

/*
 * defined in this source file
 */
stringList_t zuluCryptGetPartitionFromCrypttab( void ) ;

stringList_t zuluCryptGetPartitionFromZulutab( void ) ;

stringList_t zuluCryptPartitionList( void )
{
	const char * device ;	
	
	size_t i ;
	size_t j ;	
	ssize_t index ;	
	
	stringList_t stl   = StringListVoid ;	
	stringList_t stl_1 = StringListVoid ;	
	
	string_t st = StringGetFromVirtualFile( "/proc/partitions" ) ;	
	string_t st_1 ;
	
	if( st == StringVoid )
		return StringListVoid ;
	
	stl = StringListStringSplit( st,'\n' ) ;
	
	StringDelete( &st ) ;
	
	if( stl == StringListVoid )
		return StringListVoid ;
	
	j = StringListSize( stl )  ;
	
	st_1 = String( "/dev/" ) ;
	
	for( i = 0 ; i < j ; i++ ){
		
		st = StringListStringAt( stl,i ) ;

		index = StringLastIndexOfChar( st,' ' ) ;

		if( index == -1 )
			continue ;
		
		device = StringContent( st ) + index + 1 ;

		if( strlen( device  ) <= 3  )
			continue ;
		
		if( ( strncmp( device,"hd",2 ) == 0 || strncmp( device,"sd",2 ) == 0 ) ){			
			StringInsertAndDelete( st_1,5,device ) ;
			stl_1 = StringListAppendString( stl_1,st_1 ) ;
		}	
	}
	
	StringDelete( &st_1 ) ;
	StringListDelete( &stl ) ;
	return stl_1 ;
}

char * zuluCryptDeviceFromUUID( const char * uuid )
{
	return blkid_evaluate_tag( "UUID",uuid,NULL) ;
}

char * zuluCryptDeviceFromLabel( const char * label )
{	
	return blkid_evaluate_tag( "LABEL",label,NULL ) ;
}

static void appendSystemList( stringList_t system,stringList_t stl )
{
	size_t j ;
	size_t i ;
	
	if( stl == StringListVoid )
		return ;
	
	j = StringListSize( stl ) ;
	
	for( i = 0 ; i < j ; i++ )
		StringListAppend( system,StringListContentAt( stl,i ) ) ;
}

stringList_t zuluCryptPartitions( int option )
{
	string_t st  ;
	
	const char * entry ;
	const char * device ;
	char * ac ;
	
	ssize_t index ;

	size_t i ;
	size_t j ;
	
	stringList_t non_system = StringListVoid ;
	stringList_t system     = StringListVoid ;
	
	stringList_t p ;
	stringList_t stl = zuluCryptPartitionList() ;
	
	if( stl == StringListVoid )
		return StringListVoid ;
	
	if( option == ALL_PARTITIONS )
		return stl ;
	
	non_system = stl ;
	system = StringListVoid ;

	st = StringGetFromFile( "/etc/fstab" );
	
	if( st == StringVoid ){
		StringListDelete( &non_system ) ;
		return StringListVoid ;
	}
	
	stl = StringListStringSplit( st,'\n' ) ;
	
	StringDelete( &st ) ;
	
	if( stl == StringListVoid ){
		StringListDelete( &non_system ) ;
		return StringListVoid ;
	}
	
	j = StringListSize( stl ) ;
	
	for( i = 0 ; i < j ; i++ ){
		
		st = StringListStringAt( stl,i ) ;

		entry = StringContent( st ) ;
		
		if( entry[0] == '#' )
			continue ;
		
		index = StringIndexOfChar( st,0,' ' ) ;
		
		if( index == - 1 )
			continue ;
		
		StringSubChar( st,index,'\0' ) ;
				
		device = StringRemoveString( st,"\"" ) ;		
		
		if ( strncmp( device,"/dev/",5 ) == 0 ){			
			system = StringListAppend( system,device ) ;
			StringListRemoveString( non_system,device ) ;
		}else if( strncmp( entry,"UUID",4 ) == 0 ){
			ac = zuluCryptDeviceFromUUID( device + 5 ) ;
			if( ac != NULL ){
				system = StringListAppend( system,ac ) ;
				StringListRemoveString( non_system,ac ) ;
				free( ac ) ;
			}
		}else if( strncmp( entry,"LABEL",5 ) == 0 ){
			ac = zuluCryptDeviceFromLabel( device + 6 ) ;
			if( ac != NULL ){
				system = StringListAppend( system,ac ) ;
				StringListRemoveString( non_system,ac ) ;
				free( ac ) ;
			}
		}		
	}
	
	StringListDelete( &stl ) ;	
	
	p = zuluCryptGetPartitionFromCrypttab() ;
	appendSystemList( system,p ) ;
	StringListDelete( &p ) ;
	p = zuluCryptGetPartitionFromZulutab() ;
	appendSystemList( system,p ) ;
	StringListDelete( &p ) ;
	
	if( option == SYSTEM_PARTITIONS ){
		StringListDelete( &non_system ) ;
		return system  ;
	}else{
		StringListDelete( &system ) ;
		return non_system  ;		
	}
}

int zuluCryptPrintPartitions( int option )
{
	size_t i ;
	size_t j ;
	
	stringList_t stl = StringListVoid ;
	
	switch( option ){	
		case 1 : stl = zuluCryptPartitions( ALL_PARTITIONS ) 	   ;break ;
		case 2 : stl = zuluCryptPartitions( SYSTEM_PARTITIONS )    ;break ;
		case 3 : stl = zuluCryptPartitions( NON_SYSTEM_PARTITIONS );break ;
	}	
	
	if( stl == StringListVoid ){
		printf( "ERROR: unable to print requested list of partitions\n" ) ;
		return 1 ;
	}
	
	j = StringListSize( stl ) ;
	
	for( i = 0 ; i < j ; i++ )	
		StringListPrintLineAt( stl,i ) ;
	
	StringListDelete( &stl ) ;
	
	return 0 ;
}

/*
 * this function will parse /etc/crypttab to see if it has any entries to be used as system partition.
 * 
 * sample example of the file content this function was build on.
 * 
 
 * secret /dev/sda15 none
 * secret_1 UUID=d2d210b8-0b1f-419f-9172-9d509ea9af0c none 
 * 
 */
stringList_t zuluCryptGetPartitionFromCrypttab( void )
{
	stringList_t stl   = StringListVoid ;
	stringList_t stl_1 = StringListVoid ;
	string_t st  ;

	const char * entry ;
	char * ac ;
	
	ssize_t index ;
	ssize_t index_1 ;
	
	size_t i ;
	size_t j ;
	
	st = StringGetFromFile( "/etc/crypttab" );
	
	if( st == StringVoid )
		return StringListVoid ;
	
	stl = StringListStringSplit( st,'\n' ) ;
	
	StringDelete( &st ) ;
	
	if( stl == StringListVoid )
		return StringListVoid ;	
	
	j = StringListSize( stl ) ;
	
	if( j == 0 )
		return StringListVoid ;
	
	for( i = 0 ; i < j ; i++ ){
			
		st = StringListStringAt( stl,i ) ;
	
		entry = StringContent( st ) ;			

		if( entry[0] == '#' || entry[0] == '\n' )
			continue ;
		 
		index = StringIndexOfChar( st,0,'/' ) ;
	
		if( index == -1 ){
			/*
			 * check above did not find '/' character and we are in this block assuming the line uses UUID
			 * 
			 */
			index = StringIndexOfChar( st,0,'U' ) ;
				
			if( index == -1 )
				continue ;
				
			index = StringIndexOfChar( st,index,' ' ) ;

			if( index == -1 )
				continue ;
				
			StringSubChar( st,index,'\0' ) ;
				
			StringRemoveString( st,"\"" ) ;  /* remove quotes if they are used */
			
			/* 
			 * resolve the UUID to its device address 
			 * q will have NULL  most likely if the drive with UUID is not attached				 
			 */
			ac = zuluCryptDeviceFromUUID( strstr( StringContent( st ),"=" ) + 1 );    

			if( ac != NULL ){	
				stl_1 = StringListAppend( stl_1,ac ) ;
				free( ac ) ;					
			}
		}else{		
			/*
			 * the entry is of the first format,work to get the device address 
			 */
			index_1 = StringIndexOfChar( st,index,' ' ) ; /*index is set before the conditional statement above */
				
			if ( index_1 == -1 )
				continue ;
				
			StringSubChar( st,index_1,'\0' ) ;
		 
			stl_1 = StringListAppend( stl_1,StringContent( st ) + index ) ;
		}			
	}
	
	StringListDelete( &stl ) ;
	return stl_1 ;
}

stringList_t zuluCryptGetPartitionFromZulutab()
{
	size_t i ;
	size_t j ;
	
	const char * entry ;
	char * ac ;
	
	stringList_t stl ;
	stringList_t stl_1 = StringListVoid ;
	
	string_t st = StringGetFromFile( "/etc/zuluCrypttab" ) ;
	
	if( st == StringVoid )
		return StringListVoid ;

	stl = StringListStringSplit( st,'\n' ) ;
	
	StringDelete( &st ) ;
	
	if( stl == StringListVoid )
		return StringListVoid ;	
	
	j = StringListSize( stl ) ;
	
	for( i = 0 ; i < j ; i++ ){
		
		st = StringListStringAt( stl,i ) ;
		
		StringRemoveString( st,"\"" ) ;
		
		entry = StringContent( st ) ;
		
		if( entry[ 0 ] == '#' )
			continue ;
		
		if( strncmp( entry,"UUID=",5 ) == 0 ){			
			
			ac = zuluCryptDeviceFromUUID( entry + 5 ) ;
			
			if( ac != NULL ){

				stl_1 = StringListAppend( stl_1,ac ) ;
				free( ac ) ;
			}
		}else
			stl_1 = StringListAppend( stl_1,entry ) ;		
	}
	
	StringListDelete( &stl ) ;
	
	return stl_1 ;	
}

int zuluCryptPartitionIsSystemPartition( const char * dev )
{	
	stringList_t stl ;
	
	ssize_t index = -1 ;
	
	char * device = realpath( dev,NULL ) ;
		
	if( device == NULL )
		return 2 ;
	
	stl = zuluCryptPartitions( SYSTEM_PARTITIONS ) ;
	
	if( stl != StringListVoid ){
		index = StringListContains( stl,device );
		StringListDelete( &stl ) ;
	}	
	
	free( device ) ;
	
	return index >= 0 ? 1 : 0 ;
}
