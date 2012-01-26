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
string_t partitionList(void)
{
	string_t all ;
	FILE * fd ;
	
	char buffer[ BUFFER_SIZE ];
	char device[ DEVICE_LENGHT ] ;
	char * b ;
	char * c ;
	char * d ;
	
	fd = fopen( "/proc/partitions","r" ) ;
	
	fgets( buffer,1,fd  ) ;
	fgets( buffer,1,fd  ) ;
	
	all = String( "" );	
	
	strcpy( device, "/dev/" ) ;
	b = device + 5 ;
	
	while ( fgets( buffer,BUFFER_SIZE,fd  ) != NULL ){
		c = buffer ;
		while(  *++c != '\n'  ) { ; }
		d = c ;
		while(  *--d != ' '  ) { ; }
		d++ ;		
		if( strlen(  d  ) == 4 || (  strncmp(  d, "hd", 2  ) != 0 && strncmp(  d, "sd", 2 ) != 0  )  )
			continue ;
		strcpy( b , d ) ;
		StringAppend(  all, device );
	}	
	fclose( fd );

	return all ;
}

int device_from_uuid(char * dev, const char * uuid )
{
	char device[ DEVICE_LENGHT ] ;
	const char * d ;
	const char * e ;
	const char * f ;
	
	int j ;
	int k ;
	
	blkid_probe bp ;
	string_t all = partitionList() ;
	e = StringContent( all ) ;

	/*
	 * Below code will take into account UUID given within quotation marks ie:
	 * UUID="2468d6a7-9a71-4312-8bd9-662f982fade5"	 * 
	 */
	if( *( uuid + 5 ) == '\"' )
		uuid = uuid + 6 ;
	else
		uuid = uuid + 5 ;
	
	while( *e ){
		d = e ;
		
		while ( *e++ != '\n' ) { ; }
		
		j = e - d - 1 ;
		
		strncpy( device, d, j ) ; 
		device[ j ] = '\0' ;
		
		bp = blkid_new_probe_from_filename( device ) ;
		blkid_do_probe( bp );
		k = blkid_probe_lookup_value( bp, "UUID", &f, NULL );
		
		if( k == 0){
			if( strncmp( uuid,f,UUID_LENGTH  ) == 0 ){
				strcpy( dev,device ) ;
				StringDelete( &all ) ;
				blkid_free_probe( bp );
				return 0 ;
			}			
		}
		blkid_free_probe( bp );
	}
	StringDelete( &all ) ;
	return -1 ;	
}

void blkid( const char * type,const char * entry, int size, string_t system, string_t non_system )
{	
	char device[ DEVICE_LENGHT ] ;
	const char * f ;
	const char * e = StringContent( non_system ) ;
	int j ;
	int k ;

	blkid_probe bp ;
	while( *e ){
		f = e ;
		
		while ( *e++ != '\n' ) { ; }
		
		j = e - f - 1 ;
		
		strncpy( device, f, j ) ; 
		device[ j ] = '\0' ;	
		
		bp = blkid_new_probe_from_filename( device ) ;
		blkid_do_probe( bp );
		k = blkid_probe_lookup_value( bp, type, &f, NULL );
		
		if( k == 0 ){
			if( strcmp( f, entry + size ) == 0 ){	
				device[ j ] = '\n' ;
				device[ j + 1 ] = '\0' ;
				StringAppend( system, device ) ;
				StringRemoveString( non_system , device ) ;
				blkid_free_probe( bp );
				return ;
			}	
		}
		blkid_free_probe( bp );
	}	
}

char * partitions( int option )
{
	char buffer[ BUFFER_SIZE ];
	char device[ DEVICE_LENGHT ] ;
	char * c = NULL ;
	char * d = NULL ;
	
	FILE * fd ;
	string_t all ;
	string_t system ;
	string_t non_system ;
	
	all = partitionList() ;

	if( option == ALL_PARTITIONS )
		return StringDeleteHandle( &all ) ;
	
	non_system = all ;
	
	system = String( "" );

	fd = fopen("/etc/fstab","r");
	
	while( fgets( buffer,BUFFER_SIZE,fd  ) != NULL ){
		c = buffer ;
		while( *++c != ' ' ) { ; }
		*c = '\0' ;
		if ( strncmp( buffer, "/dev/",5 ) == 0 ){
			strcpy( device,buffer ) ;
			strcat( device, "\n" ) ;
			StringAppend( system,device ) ;
			StringRemoveString( non_system , device ) ;
		}else if ( strncmp( buffer, "UUID",4 ) == 0 ){;
		
			blkid( "UUID",buffer, 5, system, non_system ) ;  
			
		}else if ( strncmp( buffer, "LABEL",5 ) == 0 ){
 ;
			blkid( "LABEL",buffer, 6, system, non_system ) ;
		}		
	}

	fclose( fd ) ;
	
	fd = fopen( "/etc/crypttab","r" );
	
	if(  fd != NULL  ){
		while (  fgets( buffer,BUFFER_SIZE,fd  ) != NULL  ){	
			if( buffer[0] == '#' )
				continue ;
			if( buffer[0] == '\n' )
				continue ;
			c = buffer ;
			while(  *++c != '/'  ) { ; }
			d = c ;
			while(  *++d != ' '  ) { ; }
			*d++ = '\n' ;
			*d = '\0' ;
			StringAppend(  system, buffer  ) ;	
			StringRemoveString( non_system, buffer ) ;
		}
		fclose( fd ) ;
	}	

	if(  option == SYSTEM_PARTITIONS  ){
		StringDelete(  &non_system  ) ;
		return StringDeleteHandle(  &system  ) ;
	}else{
		StringDelete(  &system  ) ;
		return StringDeleteHandle(  &non_system  ) ;		
	}
}
