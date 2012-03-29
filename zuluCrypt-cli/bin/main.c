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
#include "../string/StringList.h"
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
	const char * help = "\
usage: zuluCrypt-cli < operation > < options specific to the operation >\n\
operation list\n\
-c         create an encrypted volume\n\
-o         open and encrypted volume\n\
-O         open an encrypted volume but do not mount it( -m therefore not needed )\n\
-q         close an opened encrypted volume\n\
-r         remove a key from luks volume\n\
-a         add a key to luks volume\n\
-i         check if a device contain a luks volume\n\
-s         check if a device is opened and print its properties if it is\n\
-b         show status of each slot of luks volume.\"0\"=empty,\"1\"=occupied,\"2\"=invalid slot,\"3\"=last occupied\n\
-A         print the list of all partitions on the system\n\
-N         print a list of non system partitions on the system( partitions with no active entries in /etc/fstab and /etc/crypttab\n\
-S         print a list of system partitions on the system( partitions with active entries in /etc/fstab and /etc/crypttab\n\
-w         check if UUID matches UUID of any partition\n\
-D         get device path from mapper( located at /dev/mapper )\n\
\n\
options that goes with above operations:\n\
-k         dont ask for confirmation when doing dangerous operations( used by -c and -r )\n\
-d         path to a file or partition with encrypted volume( required by all except by -v )\n\
-m         a folder will be created at this path to be used as mount point when opening volume( required by -o )\n\
-z         file system type(ext2,ext3 etc) ( required by -c )\n\
-t         type of volume( plain or luks ) ( required by -c )\n\
-g         random number generator (/dev/random or /dev/urandom) ( used by -c,/dev/urandom is used if absent )\n\
-h         get passphrase interactively,if absent then -p or -f must be used ( used with -c,-r,-a)\n\
-p         passphrase ( used with -c and -r )\n\
-f         passphrase is in a key file ( used with -c and -r )\n\
-y         passphrase already in the volume ( required by -a if -u is absent and -h is also absent )\n\
-u         passphrase in a key file to be checked against a key already in the volume ( required by -a if -y is absent and -h is also absent\n\
-l         passphrase to be added ( required by -a if -n is absent and -h is also absent )\n\
-n         passphrase in a keyfile to be added ( required by -a if -l is absent and -h is also absent\n\
\n\
examples:\n\
create volume: zuluCrypt-cli -c -d /dev/sdc1 -z ext4 -t luks -p xxx\n\
open volume  : zuluCrypt-cli -o -d /dev/sdc1 -m ~/sdc1 -e ro -p xxx\n\
close volume ; zuluCrypt-cli -q -d /dev/sdc1\n\
remove key   ; zuluCrypt-cli -r -d /dev/sdc1 -f xxx\n\
add key      : zuluCrypt-cli -a -d /dev/sdc1 -y xxx -l yyy\n\
get device path from mapper  : zuluCrypt-cli -D -d /dev/mapper/zuluCrypt-sdc1\n\
check if partition with UUID is present : zuluCrypt-cli -w -d UUID=\"d2d210b8-0b1f-419f-9172-9d509ea9af0c\"\n\
";
	printf( "%s\n",help ) ;
}

void get_opts( int argc , char *argv[],struct_opts * stopts )
{
	int c ;	
	stopts->device = NULL ;
	stopts->mount_point = NULL ;
	stopts->action = NULL ;
	stopts->mode = "ro" ;
	stopts->key_source = NULL ;
	stopts->key = NULL ;
	stopts->fs = NULL ;
	stopts->type = NULL ;
	stopts->rng = "/dev/urandom" ;
	stopts->existing_key_source = NULL ;
	stopts->existing_key = NULL ;
	stopts->new_key = NULL ;
	stopts->new_key_source = NULL ;
	stopts->partition_number = -1 ;
	stopts->dont_ask_confirmation = -1 ;
	stopts->interactive_passphrase = -1 ;
	stopts->open_no_mount = -1 ;
	
	while ( (c = getopt(argc,argv,"LOASNDkhocsarqwibm:d:p:f:e:z:g:y:u:l:n:j:t:") ) != -1 ) {
		switch( c ){	
			case( 'L' ) : stopts->action = "print"      ; break ;			     
			case( 'o' ) : stopts->action = "open"       ; break ;
			case( 'c' ) : stopts->action = "create"     ; break ;
			case( 's' ) : stopts->action = "status"     ; break ;
			case( 'a' ) : stopts->action = "addkey"     ; break ;
			case( 'r' ) : stopts->action = "removekey"  ; break ;
			case( 'q' ) : stopts->action = "close"      ; break ;
			case( 'w' ) : stopts->action = "checkUUID"  ; break ;
			case( 'i' ) : stopts->action = "isLuks"     ; break ;
			case( 'b' ) : stopts->action = "emptyslots" ; break ;
			case( 'D' ) : stopts->action = "device"     ; break ;
			case( 'O' ) : stopts->action = "open"     ;
				      stopts->open_no_mount = 1 ;
				      break ;
			case( 'k' ) : stopts->dont_ask_confirmation = 1 ;
				      break ;
			case( 'A' ) : stopts->partition_number = ALL_PARTITIONS ;
				      stopts->action = "partitions" ;
				      break ;
			case( 'S' ) : stopts->partition_number = SYSTEM_PARTITIONS ;
				      stopts->action = "partitions" ;
				      break ;
			case( 'N' ) : stopts->partition_number = NON_SYSTEM_PARTITIONS ;
				      stopts->action = "partitions" ;
				      break ;
			case( 't' ) : stopts->type = optarg ;
				      break ;
			case( 'm' ) : stopts->mount_point = optarg ;
				      break ;
			case( 'd' ) : stopts->device = optarg ; 
				      break ;
			case( 'p' ) : stopts->key_source = "-p" ; 
				      stopts->key = optarg ;
				      break ;
			case( 'f' ) : stopts->key_source = "-f" ;
				      stopts->key = optarg ;
				      break ;
			case( 'e' ) : stopts->mode = optarg ; 
				      break ;
			case( 'z' ) : stopts->fs = optarg ;
				      break ;
			case( 'g' ) : stopts->rng = optarg ; 
				      break ;
			case( 'y' ) : stopts->existing_key_source = "-p" ; 
				      stopts->existing_key = optarg ;
				      break ;
			case( 'u' ) : stopts->existing_key_source = "-f" ; 
				      stopts->existing_key = optarg ;			
				      break ;
			case( 'l' ) : stopts->new_key_source = "-p" ; 
				      stopts->new_key = optarg ;
				      break ;
			case( 'n' ) : stopts->new_key_source = "-f" ; 
				      stopts->new_key = optarg ;
				      break ;
			case( 'h' ) : stopts->interactive_passphrase = 1 ;
				      break ;
			default: printf( "run zuluCrypt-cli --help for help\n" ) ; 
				 exit( 100 ) ;
		}
	}
}

int main( int argc , char *argv[] )
{
	const char * action ;
	const char * device ;
	const char * mapping_name ;
	char * c ;
	char dev[12];
	char m_name[42] ;
	
	struct_opts clargs ;
	int status ;	

	if( argc > 1 ){
		action = argv[ 1 ] ;
		if ( strcmp( action, "-h" ) == 0 || strcmp( action, "--help" ) == 0 || strcmp( action, "-help" ) == 0 ){			
			help();	
			return 0 ;
		}	
		if ( strcmp( action, "-v" ) == 0 || strcmp( action, "-version" ) == 0 || strcmp( action, "--version" ) == 0 ){		
			printf( "%s\n",version() );
			return 0 ;
		}
	}
	
	get_opts( argc,argv,&clargs );
	
	action = clargs.action ;
	device = clargs.device ;
	
	uid_t uid = getuid();		
	setuid( 0 );
	
	if( strcmp( action,"print" ) == 0 ){
		return print_opened_volumes( uid ) ;
	}
	if( strcmp( action,"partitions" ) == 0 ){
		return print_partitions( clargs.partition_number ) ;
	}
	if( action == NULL ){
		printf("ERROR: \"action\" argument is missing\n" ) ;
		return 110 ;
	}
	if( device == NULL ){
		printf("ERROR: required option( device path ) is missing for this operation\n" ) ;
		return 110 ;
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
	if( strncmp( device,"UUID=", 5 ) == 0 ){
		strcpy(m_name,"UUID-");
		if( device_from_uuid( dev,device ) == 0 ) {
			if( *( device + 5 ) == '\"')
				strncpy( m_name + 5,device + 6,UUID_LENGTH ) ;
			else
				strncpy( m_name + 5,device + 5,UUID_LENGTH ) ;
			*( m_name + 41 ) = '\0' ;
			mapping_name = m_name ;
			clargs.device = dev ;
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
	}else if( strcmp( action,"isLuks" ) == 0 ){		

		status =  is_luks( device ) ;
		if( status == 0 )
			printf( "\"%s\" is a luks device\n",device ) ;
		else
			printf( "\"%s\" is not a luks device\n",device ) ;		
	}else if ( strcmp( action,"device" ) == 0 ){
		c = volume_device_name( device ) ;
		if( c == NULL ){
			status = 1 ;
			printf("ERROR: could not get device address from mapper address\n") ;
		}else{
			printf("%s\n",c) ;		
			free(c) ;
			status = 0 ;
		}
	}else if ( strcmp( action, "status" ) == 0 ){
		
		status = volume_info( mapping_name,device,uid ) ;
		
	}else if ( strcmp( action, "close" ) == 0 ){			

		status =  close_opened_volume( mapping_name,uid ) ;
		
	}else if ( strcmp( action, "open" ) == 0 ){
		
		status =  open_volumes( &clargs,mapping_name,uid ) ;
		
	}else if( strcmp( action,"create" ) == 0 ){
		
		status =  create_volumes( &clargs,uid ) ;		
		
	}else if( strcmp( action,"addkey" ) == 0 ){
		
		status =  addkey( &clargs,uid ) ;
		
	}else if( strcmp( action,"removekey" ) == 0 ){
				
		status =  removekey( &clargs,uid );	
	
	}else{
		printf( "ERROR: Wrong argument\n" ) ;
		help();
		status =  10 ;
	}
	return status ; 		
} 
