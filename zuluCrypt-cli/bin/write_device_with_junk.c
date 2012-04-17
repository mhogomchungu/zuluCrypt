/*
 * 
 *  Copyright (c) 2012
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <blkid/blkid.h>

#include <libcryptsetup.h>

#define SIZE 1024
#define KEY_SIZE 128

/*
 * define in partition.c
 * 
 * the function is used to check is a presented path is a system partition or not * 
 */
int check_partition( const char * ) ;

static int return_value( string_t * st, int status ) 
{
	switch( status ){
		case 0 : printf( "SUCCESS: mapper created successfully\n" ) 				       ;break ;
		case 1 : printf( "ERROR: could not create mapper\n" )                                          ;break ;
		case 2 : printf( "ERROR: could not resolve device path\n" )                                    ;break ;
		case 3 : printf( "SUCCESS: random data successfully written\n" )                               ;break ;
		/*4 is deliberately skipped */
		case 5 : printf( "INFO: user chose not to proceed\n" )                                         ;break ;	
		case 6 : printf( "ERROR: policy prevents non root user opening mapper on system partition\n" ) ;break ;	
		case 7 : /* 7 is used when returning with no feedback */				       ;break ;
		case 8 : printf( "ERROR: insufficitied privilege to oped device for reading\n" )               ;break ;
		case 9 : printf( "ERROR: device path is invalid\n" )   ;                                       ;break ;	
	}
	
	if( st != NULL )
		StringDelete( st ) ;
	
	return status ;
}

static int open_plain_as_me_1(const struct_opts * opts,const char * mapping_name,uid_t uid )
{
	string_t mapper ;
	
	int k ;
	
	char * dev ;
	char key[ KEY_SIZE ] ;	
	
	const char * device = opts->device ;
	
	switch( can_open_path_for_reading( device,uid ) ){
		case 1 : return return_value( NULL,8 ) ;
		case 2 : return return_value( NULL,9 ) ;		
	}
	
	if( uid != 0 ){
		if( check_partition( opts->device ) == 1 ){
			return return_value( NULL,6 ) ;
		}
	}
	
	dev = realpath( device,NULL );
	
	if( dev == NULL )
		return return_value( NULL,2 ) ;	
	
	mapper = create_mapper_name( dev,mapping_name,uid,OPEN ) ;
	
	free( dev ) ;
	
	k = open( "/dev/urandom",O_RDONLY ) ;
	
	read( k,key,KEY_SIZE );
	
	close( k );
	
	/*
	 * Open a plain mapper, so that we can write to device through it
	 */
	if( open_plain( device,StringContent( mapper ),"rw",key,KEY_SIZE,"cbc-essiv:sha256" ) != 0 )
		return return_value( &mapper,1 ) ;		
	
	/*
	 * Create a mapper path(usually at /dev/mapper) associated with opened plain mapper above.
	 */
	StringPrepend( mapper,"/" ) ;
	StringPrepend( mapper,crypt_get_dir() ) ;
	
	/*
	 *  mapper path is usually a soft link to /dev/dm-X
	 *  resolve the mapper path to its respective /dev/dm-X and set permission on it.
	 *  
	 * We set permission of /dev/dm-X pointing to the device to "u+rw" because we want notmal user to be able
	 * to write to the device through the mapper.
	 * 
	 * Useful when a normal user want to delete content of the device by writing random data to it.	 * 
	 */
	dev = realpath( StringContent( mapper ),NULL ) ;
	
	if( dev != NULL ){
		chown( dev,uid,0 ) ;
		chmod( dev,S_IRWXU ) ;
		free( dev ) ;
	}	
	
	return return_value( &mapper,4 ) ;		
}

int open_plain_as_me(const struct_opts * opts,const char * mapping_name,uid_t uid )
{
	int st = open_plain_as_me_1( opts,mapping_name,uid );
	
	if( st == 4 )
		return return_value( NULL,0 ) ;
	else
		return return_value( NULL,st ) ;
}

/*
 * Purpose of this function is to open a device and write random data to it as a way of hiding information on the disk.
 * 
 * The above is accomplished by opening a plain mapper against the device and then write to the device through the mapper
 * 
 */
int write_device_with_junk( const struct_opts * opts,const char * mapping_name,uid_t uid )
{	
	string_t mapper ;	
	
	double size ;	
	double size_written ;
	
	const char * device =  opts->device ;	
	char * dev ;
	
	char buffer[ SIZE ] ;
	
	int ratio ;
	int prev_ratio ;
	int k ;	
	
	dev = realpath( device,NULL ) ;
	
	if( dev == NULL )
		return_value( NULL,2 ) ;
	
	if( ( k = open_plain_as_me_1( opts,mapping_name,uid ) ) != 4 ) 
		return return_value( NULL,7 ) ;
	
	mapper = create_mapper_name( dev,mapping_name,uid,OPEN ) ;
	
	StringPrepend( mapper,"/" ) ;
	StringPrepend( mapper,crypt_get_dir() ) ;
	
	if( opts->dont_ask_confirmation == -1 ){
		printf( "\nWARNING, device \"%s\" will be overwritten with random data destroying all present data.\n",device ) ;
		printf( "Are you sure you want to proceed? Type 'Y' and press enter if you are sure: " ) ;
		
		buffer[ 0 ] = getchar() ;
		while( getchar() != '\n' ) { ; }
		
		if( buffer[ 0 ] != 'Y' ){
			close_mapper( StringContent( mapper ) ) ;			
			return return_value( NULL,5 ) ;
		}
	}
	
	k = open( StringContent( mapper ),O_WRONLY ) ;

	size = ( double ) blkid_get_dev_size( k ) ;
	     
	memset( buffer,0,SIZE ) ;
	
	size_written = 0 ;
	prev_ratio = -1 ;
	
	while( 1 ){
		
		if( size - size_written <= SIZE ){
			printf( "\rpercentage complete: 100" ) ;
			write( k,buffer,size - size_written ) ;
			break ;			
		}
		
		size_written += SIZE ;
		
		ratio = ( int ) ( ( size_written / size ) * 100 ) ;

		if( ratio % 5 == 0 )
			if( ratio != prev_ratio )
				printf( "\rpercentage complete: %d\n",ratio ) ;
			
		prev_ratio = ratio ;
		
		write( k,buffer,SIZE ) ;		
	}	
	
	close( k ) ;
	
	printf( "\n" ) ;
	
	close_mapper( StringContent( mapper ) ) ;
		
	return return_value( &mapper,3 ) ;
}
