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
static int read_line( string_t * st,FILE * f )
{
	int s ;
	StringClear( *st ) ;
	
	s = fgetc( f ) ;
	
	if( s == EOF )
		return -1 ;	
	
	if( s == '\n' )
		StringAppendChar( *st,( char ) s ) ;
	else{
		StringAppendChar( *st,( char ) s ) ;
		do{
			s = fgetc( f ) ;
			StringAppendChar( *st,( char ) s ) ;
		}
		while( s != '\n' ) ;		
	}
	return 0 ;
}

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
	string_t st = String( "" ) ;
	stringList_t stl = NULL;
	
	FILE * f ;
	
	const char * device ;	

	ssize_t index ;

	f = fopen( "/proc/partitions","r" ) ;
	
	read_line( &st,f ) ;
	read_line( &st,f ) ;
	
	while( read_line( &st,f ) != -1 ){
		
		index = StringLastIndexOfChar( st,' ' ) ;

		if( index == -1 )
			continue ;

		StringCrop( st,index + 1,1 ) ;
				
		if( StringLength( st ) <= 3  )
			continue ;
	
		device = StringContent( st ) ;
		
		if( ( strncmp( device,"hd", 2 ) == 0 || strncmp( device,"sd",2 ) == 0 ) ){
			device = StringPrepend( st,"/dev/" ) ;			
			stl = StringListAppend( stl,device );
		}	
	}
	
	fclose( f );
	StringDelete( &st ) ;
	return stl ;
}

string_t device_from_uuid( const char * uuid )
{
	string_t st ;
	const char * f ;
	const char * device ;
	int i ;
	int j ;
	int k ;
	blkid_probe bp ;
	stringList_t stl = partitionList() ;
	
	j = StringListSize( stl ) ; 
	
	for( i = 0 ; i < j ; i++ ){
		device = StringListContentAt( stl,i ) ;
		
		bp = blkid_new_probe_from_filename( device ) ;
		blkid_do_probe( bp );
		k = blkid_probe_lookup_value( bp,"UUID",&f,NULL );
		
		if( k == 0 ){
			if( strcmp( uuid,f ) == 0 ){
				st = String( device ) ;
				StringListDelete( &stl ) ; ;
				blkid_free_probe( bp );
				return st ;
			}			
		}
		blkid_free_probe( bp );
	}

	StringListDelete( &stl ) ;
	return NULL ;	
}

static void blkid( const char * type,const char * entry, int size, stringList_t * system, stringList_t non_system )
{	
	const char * device ;
	const char * f ;
	int i ;
	int j ;
	int k ;
	blkid_probe bp ;
	
	j = StringListSize( non_system ) ;
	
	for( i = 0 ; i < j ; i++ ){
		device = StringListContentAt( non_system,i ) ;
		
		bp = blkid_new_probe_from_filename( device ) ;
		blkid_do_probe( bp );
		k = blkid_probe_lookup_value( bp, type, &f, NULL );
		
		if( k == 0 ){
			if( strcmp( f, entry + size ) == 0 ){	
				*system = StringListAppend( *system,device ) ;
				StringListRemoveString( non_system,device ) ;
				blkid_free_probe( bp );
				return ;
			}	
		}
		blkid_free_probe( bp );
	}	
}

static stringList_t partitions( int option )
{
	string_t st = String( "" ) ;

	const char * entry ;
	const char * device ;

	ssize_t index ;
	
	FILE * f ;
	
	stringList_t non_system = NULL ;
	stringList_t system = NULL ;
	
	if( option == ALL_PARTITIONS )
		return partitionList() ;
	
	non_system = partitionList() ;
	system = NULL ;

	f = fopen("/etc/fstab","r");
	
	while( read_line( &st,f ) != -1 ){
		
		entry = StringContent( st ) ;

		if( entry[0] == '#' || entry[0] == '\n' )
			continue ;
		
		index = StringIndexOfChar( st,0,' ' ) ;
		
		if( index == - 1 )
			continue ;
		
		entry = StringRemoveRight( st,index ) ;
				
		device = StringRemoveString( st,"\"" ) ;		
		
		if ( strncmp( entry,"/dev/",5 ) == 0 ){			
			system = StringListAppend( system,device ) ;
			StringListRemoveString( non_system ,device ) ;
		}else if( strncmp( entry,"UUID",4 ) == 0 ){;
			blkid( "UUID",device,5,&system,non_system ) ;  			
		}else if( strncmp( entry,"LABEL",5 ) == 0 ){
			blkid( "LABEL",device,6,&system, non_system ) ;
		}		
	}
	
	fclose( f ) ;
	
	StringDelete( &st ) ;
	
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
	stringList_t stl = NULL ;
	string_t st = String( "" ) ;
	string_t q ;
	const char * device ;
	const char * entry ;
	ssize_t index ;
	ssize_t index_1 ;
	
	FILE * f = fopen( "/etc/crypttab","r" );
  
	if( f != NULL ){
		while( read_line( &st,f ) != -1 ){
			
			entry = StringContent( st ) ;			
		 
			if( entry[0] == '#' || entry[0] == '\n' )
				continue ;
		 
			index = StringIndexOfChar( st,0,'/' ) ;
			if( index == -1 ){
				/*
				 * did not find '/' character,assuming the line uses UUID,get the UUID by 
				 * removing fields on both of its sides
				 */
				index = StringIndexOfChar( st,0,'U' ) ;
				
				if( index == -1 )
					continue ;
				
				index = StringIndexOfChar( st,index,' ' ) ;

				if( index == -1 )
					continue ;
				
				StringRemoveRight( st,index ) ;
				
				StringRemoveString( st,"\"" ) ;  /* remove quotes if they are used */

				/* 
				 * resolve the UUID to its device address 
				 * q will have NULL  most likely if the drive with UUID is not attached				 
				 */
				q = device_from_uuid( strstr( StringContent( st ),"=" ) + 1 );    

				if( q != NULL ){	
					stl = StringListAppend( stl,StringContent( q ) ) ;
					StringDelete( &q ) ;					
				}
			}else{		
				/*
				 * the entry is of the first format,work to get the device address 
				 */
				index_1 = StringIndexOfChar( st,index,' ' ) ; /*index is set before the conditional statement above */
				
				if ( index_1 == -1 )
					continue ;
				
				StringRemoveRight( st,index_1 ) ;
		 
				device = StringRemoveLeft( st,index ) ;	
				stl = StringListAppend( stl,device ) ;
			}			
		}
		fclose( f ) ;		
	}
	StringDelete( &st ) ;

	return stl ;
}

ssize_t check_partition( const char * device )
{
	ssize_t index_1 = -1 ;
	ssize_t index_2 = -1 ;
	
	stringList_t stl_1 = partitions( SYSTEM_PARTITIONS ) ;
	index_1 = StringListContains( stl_1,device );
	StringListDelete( &stl_1 ) ;
	
	stringList_t stl_2 = get_partition_from_crypttab() ;
	
	if( stl_2 != NULL ){
		
		index_2 = StringListContains( stl_2,device );
		StringListDelete( &stl_2 ) ;		
	}	
	
	if( index_1 >= 0 || index_2 >= 0 )
		return 1 ;
	else
		return 0 ;
}
