
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

#include <signal.h>

#ifdef __STDC__
#include <bits/sigaction.h>
int sigaction( int sig,const struct sigaction *,struct sigaction * );
#endif

static int __exit_as_requested ;
static int __sig_caught ;

void sigTERMhandler( int sig )
{
	__exit_as_requested = 1 ;
	__sig_caught = sig ;
	printf( "\nINFO: caught signal %d,cleaning up\n",__sig_caught ) ;
}

/*
 * define in partition.c
 * 
 * the function is used to check is a presented path is a system partition or not * 
 */
int check_if_partition_is_system_partition( const char * ) ;

static int return_value( string_t * st, int status ) 
{
	switch( status ){
		case 0 : printf( "SUCCESS: mapper created successfully\n" ) ;
			 if( st != NULL ){
				printf( "opened mapper path: " ) ;
				StringPrintLine( *st ) ;
			 }
			 break ;
		case 1 : printf( "ERROR: could not create mapper\n" )                                          ;break ;
		case 2 : printf( "ERROR: could not resolve device path\n" )                                    ;break ;
		case 3 : printf( "\nSUCCESS: random data successfully written\n" )                             ;break ;
		/*4 is currently un used */
		case 5 : printf( "INFO: user chose not to proceed\n" )                                         ;break ;	
		case 6 : printf( "ERROR: policy prevents non root user opening mapper on system partition\n" ) ;break ;	
		case 7 : /* 7 is used when returning with no feedback */				       ;break ;
		case 8 : printf( "ERROR: insufficitied privilege to oped device in read/write mode\n" )        ;break ;
		case 9 : printf( "ERROR: device path is invalid\n" )                                           ;break ;	
		case 10: printf( "ERROR: passphrase file does not exist\n" )				       ;break ;
		case 11: printf( "ERROR: could not get enought memory to hold the key file\n" )  	       ;break ;
		case 12: printf( "ERROR: insufficient privilege to open key file for reading\n" )	       ;break ;	
		case 13: printf( "ERROR: can not open a mapper on a device with an opened mapper\n" )          ;break ;	
		case 14: printf( "ERROR: can not open a mapper on a mounted device\n" )                        ;break ;	
		case 15: printf( "INFO: signal caught,exiting prematurely\n" ) 				       ;break ;
		case 16: printf( "ERROR: can not get passphrase in silent mode\n" )			       ;break ;	
		case 17: printf( "ERROR: insufficient memory to hold passphrase\n" );			       ;break ;	
		case 18: printf( "ERROR: insufficient memory to hold 3 characters?really?\n" );		       ;break ;		
				
	}
	
	if( st != NULL )
		StringDelete( st ) ;
	
	return status ;
}

static int open_plain_as_me_1(const struct_opts * opts,const char * mapping_name,uid_t uid,int op )
{
	string_t mapper ;
	string_t passphrase  ;	
	string_t p ;
	
	size_t len = 0 ;
	
	int k ;
	
	int i			 = opts->interactive_passphrase ;
	const char * source      = opts->key_source ;
	const char * pass        = opts->key ;
	
	const char * cpass = NULL ;
	
	char * dev ;
	char * key ; 	
	
	const char * device = opts->device ;
	
	int j ;
	int n ;
	
	const char * cmapper ;
	
	switch( can_open_path_for_reading( device,uid ) ){
		case 1 : return return_value( NULL,8 ) ;
		case 2 : return return_value( NULL,9 ) ;		
	}
	
	switch( can_open_path_for_writing( device,uid ) ){
		case 1 : return return_value( NULL,8 ) ;
		case 2 : return return_value( NULL,9 ) ;		
	}
	
	if( uid != 0 ){
		if( check_if_partition_is_system_partition( opts->device ) == 1 ){
			return return_value( NULL,6 ) ;
		}
	}
	
	dev = realpath( device,NULL );
	
	if( dev == NULL )
		return return_value( NULL,2 ) ;	
	
	mapper = create_mapper_name( dev,mapping_name,uid,OPEN ) ;
	
	p = create_mapper_name( dev,mapping_name,uid,CLOSE ) ;
	
	j = check_opened_mapper( StringContent( p ) ) ;
	
	/*
	 * defined in print_mounted_volumes.c
	 */
	n = check_if_mounted( dev ) ;
	
	free( dev ) ;
	StringDelete( &p ) ;
	
	if( j == 1 )
		return return_value( &mapper,13 ) ;
	
	if( n == 1 )
		return return_value( &mapper,14 ) ;
	
	if ( i == 1 ){
		printf( "Enter passphrase: " ) ;	
		switch( StringSilentlyGetFromTerminal( &passphrase ) ){
			case 1 : return return_value( &mapper,16 ) ;
			case 2 : return return_value( &mapper,17 ) ;
		}
		printf( "\n" ) ;
		cpass = StringContent( passphrase ) ;
		len = StringLength( passphrase ) ;
	}else{
		if( source == NULL || pass == NULL ){
			
			printf("WARNING: getting key from \"/dev/urandom\" because atleast one required argument is missing\n" ) ;		
			
			k = open( "/dev/urandom",O_RDONLY ) ;
		
			key = ( char * ) malloc( sizeof( char ) * ( KEY_SIZE + 1 ) ) ;
			
			read( k,key,KEY_SIZE );
			
			close( k );
		
			key[ KEY_SIZE ] = '\0' ;
		
			passphrase = StringInheritWithSize( &key,KEY_SIZE ) ;
			cpass = StringContent( passphrase ) ;
			len = StringLength( passphrase ) ;
			
		}else if( strcmp( source,"-p" ) == 0 ){
			passphrase = String( pass ) ;
			cpass = StringContent( passphrase ) ;
			len = StringLength( passphrase ) ;
		}else if( strcmp( source,"-f" ) == 0 ){		
			/*
			 * function is defined at "security.c"
			 */
			switch( get_pass_from_file( pass,uid,&passphrase ) ){
				case 1 : return return_value( &mapper,10 ) ; 
				case 2 : return return_value( &mapper,11 ) ; 				
				case 4 : return return_value( &mapper,12 ) ;
			}
			cpass = StringContent( passphrase ) ;
			len = StringLength( passphrase ) ;
		}
	}
	
	/*
	 * Open a plain mapper, so that we can write to device through it
	 */
	if( open_plain( device,StringContent( mapper ),"rw",cpass,len ) != 0 )
		return return_value( &mapper,1 ) ;		
	
	/*
	 * Create a mapper path(usually at /dev/mapper) associated with opened plain mapper above.
	 */
	cmapper = StringMultiplePrepend( mapper,"/",crypt_get_dir(),'\0' ) ;
	
	/*
	 *  mapper path is usually a soft link to /dev/dm-X
	 *  resolve the mapper path to its respective /dev/dm-X and set permission on it.
	 *  
	 * We set permission of /dev/dm-X pointing to the device to "u+rw" because we want notmal user to be able
	 * to write to the device through the mapper.
	 * 
	 * Useful when a normal user want to delete content of the device by writing random data to it.	 * 
	 */
	dev = realpath( cmapper,NULL ) ;
	
	if( dev != NULL ){
		chown( dev,uid,0 ) ;
		chmod( dev,S_IRWXU ) ;
		free( dev ) ;
	}	
	
	StringDelete( &passphrase ) ;
	
	if( op == 1 )
		return return_value( &mapper,0 ) ;
	else{
		StringDelete( &mapper ) ;
		return 0 ;
	}
}

int open_plain_as_me(const struct_opts * opts,const char * mapping_name,uid_t uid )
{
	return open_plain_as_me_1( opts,mapping_name,uid,1 ) ;
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
	string_t confirm ;
	
	double size ;	
	double size_written ;
	
	const char * device =  opts->device ;	
	char * dev ;
	
	char buffer[ SIZE ] ;
	
	int ratio ;
	int prev_ratio ;
	int k ;	
	
	struct sigaction sigac;
	
	memset( &sigac,'\0',sizeof( struct sigaction ) ) ;
	
	sigac.sa_handler = &sigTERMhandler ;
	
	sigaction( SIGINT,&sigac,NULL ) ;
	sigaction( SIGTERM,&sigac,NULL ) ;
	sigaction( SIGHUP,&sigac,NULL ) ;	
	
	__exit_as_requested = 0 ;
	
	dev = realpath( device,NULL ) ;
	
	if( dev == NULL )
		return 2 ;
	
	if( ( k = open_plain_as_me_1( opts,mapping_name,uid,0 ) ) != 0 ) 
		return k ;
	
	mapper = create_mapper_name( dev,mapping_name,uid,OPEN ) ;
	
	StringMultiplePrepend( mapper,"/",crypt_get_dir(),'\0' ) ;
	
	if( opts->dont_ask_confirmation == -1 ){
		printf( "\nWARNING, device \"%s\" will be overwritten with random data destroying all present data.\n",device ) ;
		printf( "Are you sure you want to proceed? Type \"YES\" and press enter if you are sure: " ) ;
		
		confirm = StringGetFromTerminal() ;
		if( confirm == NULL )
			return return_value( &mapper,17 ) ;
		else{
			k = StringEqual( confirm,"YES" ) ;
			StringDelete( &confirm ) ;
		
			if( k == 1 ){			
				close_mapper( StringContent( mapper ) ) ;			
				return return_value( &mapper,5 ) ;
			}
		}
	}
	
	k = open( StringContent( mapper ),O_WRONLY ) ;

	size = ( double ) blkid_get_dev_size( k ) ;
	     
	memset( buffer,0,SIZE ) ;
	
	size_written = 0 ;
	prev_ratio = -1 ;
	
	while( write( k,buffer,SIZE ) > 0 ){

		if( __exit_as_requested == 1 )
			break ;
		
		size_written += SIZE ;
		
		ratio = ( int ) ( ( size_written / size ) * 100 ) ;

		if( ratio > prev_ratio ){
			printf( "\rpercentage complete: %d%%",ratio ) ;
			fflush( stdout );
			prev_ratio = ratio ;
		}
	}	
		
	close( k ) ;
	
	close_mapper( StringContent( mapper ) ) ;
		
	if( __exit_as_requested == 1 ) 
		return return_value( &mapper,15 ) ;
	else
		return return_value( &mapper,3 ) ;
	
}
