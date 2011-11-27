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

#include "includes.h"

#include <mntent.h>
#include <blkid/blkid.h>

char * partitions( int option )
{
	char * b ;
	
	char * c ;
	
	char * d ;
	
	const char * e ;
	
	const char * f ;
	
	const char * uuid ;
	
	int j ;
	
	int k ;
	
	char buffer[512];
	
	//char label[16];
	char device[12] ;
	
	struct mntent * mt ;
	
	blkid_probe bp ;
	
	StrHandle * all ;
	StrHandle * system ;
	StrHandle * non_system ;
	
	FILE * fd ;
	
	fd = fopen( "/proc/partitions","r" ) ;
	
	fgets( buffer,1,fd  ) ;
	fgets( buffer,1,fd  ) ;
	
	all = String( "" );	
	
	strcpy( device, "/dev/" ) ;
	
	b = device + 5 ;
	
	while (  fgets( buffer,512,fd  ) != NULL  ){
		
		c = buffer ;
		
		while(  *c++ != '\n'  ) { ; }
		
		d = c ;
		
		while(  *--d != ' '  ) { ; }
		
		d++ ;		
		
		if( strlen(  d  ) == 4 || (  strncmp(  d, "hd", 2  ) != 0 && strncmp(  d, "sd", 2 ) != 0  )  )
			continue ;
		
		strcpy( b , d ) ;
		
		StringAppend(  all, device );
	}
	
	fclose( fd );	
	
	if(  option == ALL_PARTITIONS  )
		return StringDeleteHandle(  all  ) ;
	
	non_system = all ;
	
	system = String( "" );

	fd = setmntent("/etc/fstab", "r");
	
	while( ( mt = getmntent( fd ) ) != NULL ){
		if ( strncmp( mt->mnt_fsname, "/dev/",5 ) == 0 ){

			strcpy( device,mt->mnt_fsname ) ;
			
			strcat( device, "\n" ) ;
			
			StringAppend( system,device ) ;
			
			StringRemoveStringString( non_system , device ) ;
				       
		}else if ( strncmp( mt->mnt_fsname, "UUID",4 ) == 0 ){

			e = StringContent( all ) ;
			
			while( 1 ){
			
				f = e ;
						
				while ( *e++ != '\n' ) { ; }
				
				j = e - f - 1 ;
			
				strncpy( device, f, j ) ; 
				
				device[ j ] = '\0' ;	
				
				bp = blkid_new_probe_from_filename( device ) ;

				blkid_do_probe( bp );

				k = blkid_probe_lookup_value( bp, "UUID", &uuid, NULL );
				
				if( k == 0 ){
					if( strcmp( uuid, mt->mnt_fsname + 5 ) == 0 ){					

						device[ j ] = '\n' ;
					
						device[ j + 1 ] = '\0' ;

						StringAppend(  system, device ) ;

						StringRemoveStringString( non_system , device ) ;					

						break ;
					}	
				}
				blkid_free_probe( bp );
			}
		}else if ( strncmp( mt->mnt_fsname, "LABEL",5 ) == 0 ){
			
			e = StringContent( all ) ;
			
			while( 1 ){
				
				f = e ;
				
				while ( *e++ != '\n' ) { ; }
				
				j = e - f - 1 ;
				
				strncpy( device, f, j ) ; 
				
				device[ j ] = '\0' ;				
				
				bp = blkid_new_probe_from_filename( device ) ;
				
				blkid_do_probe( bp );
				
				k = blkid_probe_lookup_value( bp, "LABEL", &uuid, NULL );
				
				if( k == 0 ){
					if( strcmp( uuid, mt->mnt_fsname + 6 ) == 0 ){					
					
						device[ j ] = '\n' ;
					
						device[ j + 1 ] = '\0' ;
					
						StringAppend(  system, device ) ;
					
						StringRemoveStringString( non_system , device ) ;					
					
						break ;
					}
				}				
				blkid_free_probe( bp );
			}
		}
	}

	endmntent( fd ) ;
	
	fd = fopen( "/etc/crypttab","r" );
	
	if(  fd != NULL  ){
		
		while (  fgets( buffer,512,fd  ) != NULL  ){	
			
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
			
			StringRemoveStringString( non_system, buffer ) ;
		}
		
		fclose( fd ) ;
	}	

	if(  option == SYSTEM_PARTITIONS  ){
		StringDelete(  non_system  ) ;
		return StringDeleteHandle(  system  ) ;
	}else{
		StringDelete(  system  ) ;
		return StringDeleteHandle(  non_system  ) ;		
	}
}
