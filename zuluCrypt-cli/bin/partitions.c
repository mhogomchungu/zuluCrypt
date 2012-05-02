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
static stringList_t partitionList( void )
{
	const char * device ;	
	
	size_t i ;
	size_t j ;	
	ssize_t index ;	
	
	stringList_t stl ;	
	stringList_t stl_1 = NULL ;	
	
	string_t st = StringGetFromVirtualFile( "/proc/partitions" ) ;	
	
	if( st == NULL )
		return NULL ;
	
	string_t st_1 ;
	
	stl = StringListStringSplit( &st,'\n' ) ;
	
	if( stl == NULL )
		return NULL ;

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

char * device_from_uuid( const char * uuid )
{
	return blkid_evaluate_tag( "UUID",uuid,NULL ) ;
}

char * device_from_label( const char * label )
{
	return blkid_evaluate_tag( "LABEL",label,NULL ) ;
}

static stringList_t partitions( int option )
{
	string_t st  ;
	stringList_t stl ;
	
	const char * entry ;
	const char * device ;
	char * ac ;
	
	ssize_t index ;

	size_t i ;
	size_t j ;
	
	stringList_t non_system = NULL ;
	stringList_t system = NULL ;
	
	if( option == ALL_PARTITIONS )
		return partitionList() ;
	
	non_system = partitionList() ;
	system = NULL ;

	st = StringGetFromFile( "/etc/fstab" );
	
	stl = StringListStringSplit( &st,'\n' ) ;
	
	j = StringListSize( stl ) ;
	
	for( i = 0 ; i < j ; i++ ){
		
		st = StringListStringAt( stl,i ) ;

		entry = StringContent( st ) ;
		
		if( entry[0] == '#' || entry[0] == '\n' )
			continue ;
		
		index = StringIndexOfChar( st,0,' ' ) ;
		
		if( index == - 1 )
			continue ;
		
		StringSubChar( st,index,'\0' ) ;
				
		device = StringRemoveString( st,"\"" ) ;		
		
		if ( strncmp( device,"/dev/",5 ) == 0 ){			
			system = StringListAppend( system,device ) ;
			StringListRemoveString( non_system ,device ) ;
		}else if( strncmp( entry,"UUID",4 ) == 0 ){
			ac = device_from_uuid( device + 5 ) ;
			if( ac != NULL ){
				system = StringListAppend( system,ac ) ;
				StringListRemoveString( non_system,ac ) ;
				free( ac ) ;
			}
		}else if( strncmp( entry,"LABEL",5 ) == 0 ){
			ac = device_from_label( device + 6 ) ;
			if( ac != NULL ){
				system = StringListAppend( system,ac ) ;
				StringListRemoveString( non_system,ac ) ;
				free( ac ) ;
			}
		}		
	}
	
	StringListDelete( &stl ) ;
	
	if( option == SYSTEM_PARTITIONS ){
		StringListDelete( &non_system ) ;
		return system  ;
	}else{
		StringListDelete( &system ) ;
		return non_system  ;		
	}
}

int print_partitions( int option )
{
	size_t i ;
	size_t j ;
	stringList_t stl ;
	switch( option ){	
		case 1 : stl = partitions( ALL_PARTITIONS ) ;
		break ;
		case 2 : stl = partitions( SYSTEM_PARTITIONS ) ;
		break ;
		case 3 : stl = partitions( NON_SYSTEM_PARTITIONS ) ;
		break ;
	}	
	
	if( stl == NULL ){
		printf( "ERROR: unable to print a list of partitions you have requested\n" ) ;
		return 1 ;
	}
	
	j = StringListSize( stl ) ;
	for( i = 0 ; i < j ; i++ )	
		printf("%s\n",StringListContentAt( stl,i ) );
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
stringList_t get_partition_from_crypttab( void )
{
	stringList_t stl ;
	stringList_t stl_1 = NULL ;
	string_t st  ;

	const char * entry ;
	char * ac ;
	
	ssize_t index ;
	ssize_t index_1 ;
	
	size_t i ;
	size_t j ;
	
	st = StringGetFromFile( "/etc/crypttab" );
	
	if( st == NULL )
		return NULL ;
	
	stl = StringListStringSplit( &st,'\n' ) ;
	
	if( stl == NULL )
		return NULL ;
	
	j = StringListSize( stl ) ;
	
	if( j == 0 )
		return NULL ;
	
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
			ac = device_from_uuid( strstr( StringContent( st ),"=" ) + 1 );    

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

stringList_t get_partition_from_zulutab()
{
	size_t i ;
	size_t j ;
	
	const char * entry ;
	char * ac ;
	
	stringList_t stl ;
	stringList_t stl_1 = NULL ;
	
	string_t st = StringGetFromFile( "/etc/zuluCrypttab" ) ;
	
	if( st == NULL )
		return NULL ;

	stl = StringListStringSplit( &st,'\n' ) ;
	
	if( stl == NULL )
		return NULL ;
	
	j = StringListSize( stl ) ;
	
	for( i = 0 ; i < j ; i++ ){
		
		st = StringListStringAt( stl,i ) ;
		
		StringRemoveString( st,"\"" ) ;
		
		entry = StringContent( st ) ;
		
		if( entry[ 0 ] == '#' )
			continue ;
		
		if( strncmp( entry,"UUID=",5 ) == 0 ){			
			
			ac = device_from_uuid( entry + 5 ) ;
			
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

int check_partition( const char * dev )
{	
	stringList_t stl_1 ;
	stringList_t stl_2 ;
	stringList_t stl_3 ;
	
	ssize_t index_1 = -1 ;
	ssize_t index_2 = -1 ;
	ssize_t index_3 = -1 ;
	
	char * device = realpath( dev,NULL ) ;
		
	if( device == NULL )
		return 2 ;
	
	stl_1 = partitions( SYSTEM_PARTITIONS ) ;
	index_1 = StringListContains( stl_1,device );
	StringListDelete( &stl_1 ) ;
	
	stl_2 = get_partition_from_crypttab() ;
	
	if( stl_2 != NULL ){
		
		index_2 = StringListContains( stl_2,device );
		StringListDelete( &stl_2 ) ;		
	}	
	
	stl_3 = get_partition_from_zulutab() ;
	
	if( stl_3 != NULL ){
		
		index_3 = StringListContains( stl_3,device );
		StringListDelete( &stl_3 ) ;		
	}
	
	free( device ) ;
	
	if( index_1 >= 0 || index_2 >= 0 || index_3 >= 0 )
		return 1 ;
	else
		return 0 ;
}
