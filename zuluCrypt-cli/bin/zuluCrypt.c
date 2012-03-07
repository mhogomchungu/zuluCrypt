/*
 * 
 *  Copyright ( c ) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "includes.h"

/*
 * function prototypes in this source file.
 * */
string_t get_passphrase( void ) ;

/*
 * defined in lib/status.c
 * */
char * volume_device_name( const char * ) ;

/*
 * function prototypes in this source file.
 * */
void help( void ) ;
		 
int check_system_tools( void ) ;

/*
 * defined in partitions.c
 */
int device_from_uuid(char * dev, const char * uuid ) ;

/*
 * defined in partitions.c
 */
int print_partitions( int option ) ;

string_t get_passphrase( void )
{	
	//I got the big chunk of this code from: http://www.gnu.org/s/hello/manual/libc/getpass.html
	char c[2] ;
	string_t p ;
	struct termios old, new;
			
       /* Turn echoing off and fail if we can't. */
	if ( tcgetattr ( 1, &old ) != 0 )
		exit( -1 );

	new = old;
	new.c_lflag &= ~ECHO;
			
	if ( tcsetattr ( 1, TCSAFLUSH, &new ) != 0 )
		exit( -1 );
	c[1] = '\0' ;
	c[0] = getchar() ;
	p = String( c ) ;
	while( ( c[0] = getchar() ) != '\n' )		
		StringAppend( p, c ) ;
	( void ) tcsetattr ( 1, TCSAFLUSH, &old );	
	return p ;
}

void help( void )
{
	printf( "run \"man zuluCrypt-cli\" for documentation on how to run this tool.\n" ) ;
	printf( "The same documentation can be found at http://code.google.com/p/zulucrypt\n" );	
}

int main( int argc , char *argv[] )
{
	char * action = argv[1] ;
	char * device = argv[2] ;
	
	int status ;
	char *  mapping_name ;
	char * c ;
	char dev[12];
	char m_name[42] ;

	uid_t id = getuid();		

	if ( argc < 2 ){
		help();
		return 1 ;
	}
	if ( strcmp( action, "-h" ) == 0 || strcmp( action, "--help" ) == 0 || strcmp( action, "-help" ) == 0 ){			
		help();	
		return 0 ;
	}
	if ( strcmp( action, "-v" ) == 0 || strcmp( action, "-version" ) == 0 || strcmp( action, "--version" ) == 0 ){		
		printf( "%s\n",version() );
		return 0 ;
	}
	if ( argc < 3 ){
		help();
		return 1 ;
	}
	if( strcmp( action,"checkUUID") == 0 ){		
		if( device_from_uuid( dev,device ) == 0 ){
			printf( "%s\n",dev ) ;
			return 0 ;
		}else{
			printf("ERROR: could not find any partition with the presented UUID\n") ;
			return 1 ;
		}	
	}
	if( strcmp( action,"partitions" ) == 0 ){
		
		return print_partitions( argv[2][0] ) ;

	}
	if( strncmp( device, "UUID=", 5 ) == 0 ){
		strcpy(m_name,"UUID-");
		if( device_from_uuid( dev,device ) == 0 ) {
			if( *( device + 5 ) == '\"')
				strncpy( m_name + 5,device + 6,UUID_LENGTH ) ;
			else
				strncpy( m_name + 5,device + 5,UUID_LENGTH ) ;
			*( m_name + 41 ) = '\0' ;
			mapping_name = m_name ;
			device = dev ;
		}else{
			printf("ERROR: could not find any partition with the presented UUID\n") ;
			return 11 ;			
		}	
	}else{
		if ( ( c = strrchr( device,'/' ) ) != NULL ) {
			mapping_name =  c + 1  ;
		}else{
			mapping_name =  device  ;			
		}
	}
	//all below code need root's priviledes to work
	setuid( 0 );
	
	if( strcmp( action,"emptyslots" ) == 0 ){
		if( is_path_valid( device ) == 1 ){
			printf( "path \"%s\" does not point to a device\n",device ) ;
			status = 1 ;			
		}else{
			c = empty_slots( device ) ;
			if( c == NULL ){
				printf( "device \"%s\" is not a luks device\n",device ) ;
				status = 2 ;
			}else{
				printf( "%s\n",c ) ;
				status = 0 ;
				free( c ) ;
			}		
		}
	}else if( strcmp( action, "isLuks" ) == 0 ){
		status =  is_luks( device ) ;
		if( status == 0 )
			printf( "\"%s\" is a luks device\n",device ) ;
		else
			printf( "\"%s\" is not a luks device\n",device ) ;		
	}else if ( strcmp( action, "device" ) == 0 ){
		c = volume_device_name( device ) ;
		if( c == NULL )
			status = 1 ;
		else{
			printf("%s\n",c) ;		
			free(c) ;
			status = 0 ;
		}
	}else if ( strcmp( action, "status" ) == 0 ){
		
		status = volume_info( mapping_name, argv[2] ) ;
		
	}else if ( strcmp( action, "close" ) == 0 ){			

		status =  close_opened_volume( mapping_name ) ;
		
	}else if ( strcmp( action, "open" ) == 0 ){
		
		status =  open_volumes( argc,device,mapping_name,id,argv[3],argv[4],argv[5],argv[6] ) ;		
		
	}else if( strcmp( action,"create" ) == 0 ){

		status =  create_volumes( argc,device,argv[3],argv[4],argv[5],argv[6],argv[7] ) ;	
		
	}else if( strcmp( action,"addkey" ) == 0 ){
		
		status =  addkey( argc,device,argv[3],argv[4],argv[5],argv[6] ) ;
		
	}else if( strcmp( action,"removekey" ) == 0 ){
				
		status =  removekey( argc, device, argv[3],argv[4] );	
	
	}else{
		printf( "ERROR: Wrong argument\n" ) ;
		help();
		status =  10 ;
	}
	return status ; 		
} 		
