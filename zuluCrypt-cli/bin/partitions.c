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

#include <mntent.h>
#include <blkid/blkid.h>

#define BUFFER_SIZE 512

stringList_t partitionList( void )
{
	stringList_t stl = NULL;
	FILE * fd ;	
	char buffer[ BUFFER_SIZE ];
	char device[ DEVICE_LENGHT ] ;	
	char * c ;
	char * d ;
	
	fd = fopen( "/proc/partitions","r" ) ;
	
	fgets( buffer,1,fd  ) ;
	fgets( buffer,1,fd  ) ;

	strcpy( device, "/dev/" ) ;
	
	while( fgets( buffer,BUFFER_SIZE,fd  ) != NULL ){
		c = buffer ;
		while( *++c != '\n' ) { ; }
		*c = '\0';
		d = c ;
		while( *--d != ' ' ) { ; }
		d++ ;		
		
		if( strlen( d ) <= 3  )
			continue ;
		
		if( ( strncmp( d,"hd", 2 ) == 0 || strncmp( d,"sd",2 ) == 0 ) ){
			strcpy( device + 5, d ) ;
			stl = StringListAppend( stl,device );
		}	
	}
		
	fclose( fd );

	return stl ;
}

int device_from_uuid( char * dev, const char * uuid )
{
	const char * f ;
	const char * device ;
	int i ;
	int j ;
	int k ;
	blkid_probe bp ;
	stringList_t stl = partitionList() ;

	/*
	 * Below code will take into account UUID given within quotation marks ie:
	 * UUID="2468d6a7-9a71-4312-8bd9-662f982fade5"	 * 
	 */
	if( *( uuid + 5 ) == '\"' )
		uuid = uuid + 6 ;
	else
		uuid = uuid + 5 ;
	
	j = StringListSize( stl ) ; 
	
	for( i = 0 ; i < j ; i++ ){
		device = StringListContentAt( stl,i ) ;
		
		bp = blkid_new_probe_from_filename( device ) ;
		blkid_do_probe( bp );
		k = blkid_probe_lookup_value( bp,"UUID",&f,NULL );
		
		if( k == 0 ){
			if( strncmp( uuid,f,UUID_LENGTH ) == 0 ){
				strcpy( dev,device ) ;
				StringListDelete( &stl ) ; ;
				blkid_free_probe( bp );
				return 0 ;
			}			
		}
		blkid_free_probe( bp );
	}

	StringListDelete( &stl ) ;
	return -1 ;	
}

void blkid( const char * type,const char * entry, int size, stringList_t * system, stringList_t non_system )
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

stringList_t partitions( int option )
{
	char buffer[ BUFFER_SIZE ];
	char device[ DEVICE_LENGHT ] ;
	char * c = NULL ;
	char * d = NULL ;
	
	FILE * fd ;
	
	stringList_t non_system = NULL ;
	stringList_t system = NULL ;
	
	if( option == ALL_PARTITIONS )
		return partitionList() ;
	
	non_system = partitionList() ;
	system = NULL ;

	fd = fopen("/etc/fstab","r");
	
	while( fgets( buffer,BUFFER_SIZE,fd  ) != NULL ){
		c = buffer ;
		while( *++c != ' ' ) { ; }
		*c = '\0' ;
		if ( strncmp( buffer, "/dev/",5 ) == 0 ){
			strcpy( device,buffer ) ;
			system = StringListAppend( system,device ) ;
			StringListRemoveString( non_system ,device ) ;
		}else if ( strncmp( buffer, "UUID",4 ) == 0 ){;
			blkid( "UUID",buffer, 5, &system, non_system ) ;  
			
		}else if ( strncmp( buffer, "LABEL",5 ) == 0 ){
			blkid( "LABEL",buffer, 6, &system, non_system ) ;
		}		
	}

	fclose( fd ) ;
	
	fd = fopen( "/etc/crypttab","r" );
	
	if( fd != NULL ){
		while ( fgets( buffer,BUFFER_SIZE,fd  ) != NULL ){	
			if( buffer[0] == '#' || buffer[0] == '\n')
				continue ;
			c = buffer ;
			while(  *++c != '/'  ) { ; }
			d = c ;
			while(  *++d != ' '  ) { ; }
			*d = '\0' ;
			system = StringListAppend( system,buffer ) ;	
			StringListRemoveString( non_system, buffer ) ;
		}
		fclose( fd ) ;
	}	

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
		case '1' : stl = partitions( ALL_PARTITIONS ) ;
		break ;
		case '2' : stl = partitions( SYSTEM_PARTITIONS ) ;
		break ;
		case '3' : stl = partitions( NON_SYSTEM_PARTITIONS ) ;
		break ;
		default:
			printf( "wrong argument\n" );
			return 1 ;
	}
	j = StringListSize( stl ) ;
	for( i = 0 ; i < j ; i++ )	
		printf("%s\n",StringListContentAt( stl,i ) );
	StringListDelete( &stl ) ;
	return 0 ;
}

ssize_t check_partition( const char * device )
{
	ssize_t index ;
	stringList_t stl = partitions( SYSTEM_PARTITIONS ) ;
	index = StringListContains( stl,device );
	StringListDelete( &stl ) ;
	return index ;	
}
