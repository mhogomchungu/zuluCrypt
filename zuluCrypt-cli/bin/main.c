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
string_t device_from_uuid( const char * uuid ) ;

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
meaning of symbols:\n\
[] = required option\n\
() = optional argument,default exists\n\
*  = default option\n\
/  = alternatives for the same option\n\
{}  = not allowed option\n\
\n\
zuluCrypt-cli -o [ -d ] [ -m ] ( -e ) [ -p/-f/-h ]\n\
zuluCrypt-cli -O [ -d ] { -m } ( -e ) [ -p/-f/-h ]\n\
zuluCrypt-cli -q [ -d ]\n\
zuluCrypt-cli -i [ -d ]\n\
zuluCrypt-cli -c [ -d ] [ -p/-f /-h ] ( -k ) ( -t ) ( -z  ) ( -g )\n\
zuluCrypt-cli -r [ -d ] [ -p/-f/-h ] \n\
zuluCrypt-cli -a [ -d ] [ [ -y/-u ] [ -l/-n ] ]/[ -h ]\n\
zuluCrypt-cli -b [ -d ]\n\
zuluCrypt-cli -w [ -d ]   d argument must be something like: UUID=\"2468d6a7-9a71-4312-8bd9-662f982fade5\" ( or without quotes )\n\
zuluCrypt-cli -D [ -d ]   d device must be mapper path at /dev/mapper/\n\
zuluCrypt-cli -A\n\
zuluCrypt-cli -S\n\
zuluCrypt-cli -N\n\
zuluCrypt-cli -N\n\
\n\
examples:\n\
create volume: zuluCrypt-cli -c -d /dev/sdc1 -z ext4 -t luks -p xxx\n\
open volume  : zuluCrypt-cli -o -d /dev/sdc1 -m ~/sdc1 -e ro -p xxx\n\
close volume ; zuluCrypt-cli -q -d /dev/sdc1\n\
remove key   ; zuluCrypt-cli -r -d /dev/sdc1 -p xxx\n\
add key      : zuluCrypt-cli -a -d /dev/sdc1 -y xxx -l yyy\n\
get device path from mapper  : zuluCrypt-cli -D -d /dev/mapper/zuluCrypt-sdc1\n\
check if partition with UUID is present : zuluCrypt-cli -w -d UUID=\"d2d210b8-0b1f-419f-9172-9d509ea9af0c\"\n\
operation list\n\n\
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
-L         print a list of all opened volumes and their mount point.The list is not formatted\n\
\n\
options that goes with above operations:\n\
-e         mode for opening volumes(ro*/rw)\n\
-k         do not ask for confirmation when doing dangerous operations\n\
-d         path to a file or partition with encrypted volume\n\
-m         path to where mount point folder will be created\n\
-z         file system type installed(ext2,ext3,ext4* etc)\n\
-t         type of volume (plain/luks*)\n\
-g         random number generator (/dev/random or /dev/urandom*)\n\
-h         get passphrase interactively\n\
-p         passphrase \n\
-f         path to keyfile\n\
-y         passphrase already in the volume(required by -a if -u is absent and -h is also absent)\n\
-u         path to keyfile with passphrase already in the volume(required by -a if -y is absent and -h is also absent)\n\
-l         passphrase to be added(required by -a if -n is absent and -h is also absent)\n\
-n         path to keyfile with a passphrase to be added (required by -a if -l is absent and -h is also absent)\n";
	printf( "%s\n",help ) ;
}

void get_opts( int argc , char *argv[],struct_opts * stopts )
{
	int c ;	
	stopts->device = NULL ;
	stopts->mount_point = NULL ;
	stopts->action = '\0' ;
	stopts->mode = "ro" ;
	stopts->key_source = NULL ;
	stopts->key = NULL ;
	stopts->fs = "ext4" ;
	stopts->type = "luks" ;
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
			case( 'L' ) : stopts->action = 'L'      ; break ;			     
			case( 'o' ) : stopts->action = 'o'      ; break ;
			case( 'c' ) : stopts->action = 'c'      ; break ;
			case( 's' ) : stopts->action = 's'      ; break ;
			case( 'a' ) : stopts->action = 'a'      ; break ;
			case( 'r' ) : stopts->action = 'r'      ; break ;
			case( 'q' ) : stopts->action = 'q'      ; break ;
			case( 'w' ) : stopts->action = 'w'      ; break ;
			case( 'i' ) : stopts->action = 'i'      ; break ;
			case( 'b' ) : stopts->action = 'b'      ; break ;
			case( 'D' ) : stopts->action = 'D'      ; break ;
			case( 'O' ) : stopts->action = 'O'      ;
				      stopts->open_no_mount = 1 ; break ;
			case( 'k' ) : stopts->dont_ask_confirmation = 1 ;
				      break ;
			case( 'A' ) : stopts->partition_number = ALL_PARTITIONS ;
				      stopts->action = 'A' ;
				      break ;
			case( 'S' ) : stopts->partition_number = SYSTEM_PARTITIONS ;
				      stopts->action = 'S' ;
				      break ;
			case( 'N' ) : stopts->partition_number = NON_SYSTEM_PARTITIONS ;
				      stopts->action = 'N' ;
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

static int get_device( const char * device )
{
	char * c = volume_device_name( device ) ;
	if( c == NULL ){
		printf( "ERROR: could not get device address from mapper address\n" ) ;
		return 1 ;
	}else{
		printf( "%s\n",c ) ;		
		free( c ) ;
		return 0 ;
	}
}

static int check_if_luks( const char * device )
{
	int status =  is_luks( device ) ;
	
	if( status == 0 )
		printf( "\"%s\" is a luks device\n",device ) ;
	else
		printf( "\"%s\" is not a luks device\n",device ) ;
	
	return status ;
}

static int check_empty_slots( const char * device )
{
	int status ;
	char * c  ;
	if( is_path_valid( device ) != 0 ){
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
	return status ;
}

static int check_UUID( const char * device )
{
	string_t p = device_from_uuid( device ) ;
	
	if( p != NULL ){
		printf( "%s\n",StringContent( p ) ) ;
		StringDelete( &p ) ;
		return 0 ;
	}else{
		printf("ERROR: could not find any partition with the presented UUID\n") ;
		return 1 ;
	}
}

static int exe( struct_opts * clargs, const char * mapping_name,uid_t uid )
{
	switch( clargs->action ){
		case 'w' : return check_UUID( clargs->device ) ;
		case 'b' : return check_empty_slots( clargs->device ) ;
		case 'i' : return check_if_luks( clargs->device ) ;
		case 'D' : return get_device( clargs->device ) ;
		case 's' : return volume_info( mapping_name,clargs->device,uid ) ;
		case 'q' : return close_opened_volume( clargs->device,mapping_name,uid ) ;
		case 'o' : return open_volumes( clargs,mapping_name,uid ) ;
		case 'O' : return open_volumes( clargs,mapping_name,uid ) ;		
		case 'c' : return create_volumes( clargs,uid ) ;
		case 'a' : return addkey( clargs,uid ) ;
		case 'r' : return removekey( clargs,uid );
	}
	printf("ERROR!!!!!!!!!!: cli option missed!\n" );
	return 200 ; /* shouldnt get here */	
}

stringList_t get_partition_from_crypttab( void ) ;

int main( int argc , char *argv[] )
{
	const char * device ;
	const char * mapping_name ;
	char * ac ;
	char action ;
	int st ;
	uid_t uid ;
	/*
	 * string_t is prototyped as "typedef struct StringType * string_t" at ../string/StringTypes.h
	 * string_t type is therefore a pointer and it is appropriate to assign NULL to it	 
	 */
	string_t p = NULL ;
	string_t q = NULL ;
	
	struct_opts clargs ;

	if( argc == 1 ){
		help();
		return 1;
	}
	if( argc == 2 ){
		ac = argv[ 1 ] ;
		if ( strcmp( ac,"-h" ) == 0 || strcmp( ac,"--help" ) == 0 || strcmp( ac,"-help" ) == 0 ){			
			help();	
			return 0 ;
		}	
		if ( strcmp( ac,"-v" ) == 0 || strcmp( ac,"-version" ) == 0 || strcmp( ac,"--version" ) == 0 ){		
			printf( "%s\n",version() );
			return 0 ;
		}
	}
	
	get_opts( argc,argv,&clargs );
	
	uid = getuid();
	
	setuid( 0 );
	
	action = clargs.action ;
	device = clargs.device ;
	
	/*
	 * below tests are here because they do not use -d option
	 */	
	switch( action ){
		case 'A':
		case 'N':
		case 'S': return print_partitions( clargs.partition_number ) ;	
		case 'L': return print_opened_volumes( uid ) ;
	}
	if( action == '\0' ){
		printf("ERROR: \"action\" argument is missing\n" ) ;
		return 130 ;
	}
	if( device == NULL ){
		printf("ERROR: required option( device path ) is missing for this operation\n" ) ;
		return 120 ;
	}
	if( strncmp( device,"UUID=", 5 ) == 0 ){

		q = String( device ) ;	
		StringSubChar( q,4,'-' ) ;
		StringRemoveString( q,"\"" ) ;
		
		mapping_name = StringContent( q ) ;

		p = device_from_uuid( mapping_name + 5 ) ;
		
		if( p != NULL ) {		
			clargs.device = StringContent( p ) ;			
			st = exe( &clargs,mapping_name,uid );			
			StringDelete( &p ) ;
			StringDelete( &q ) ;
			return st ;
		}else{
			printf("ERROR: could not find any partition with the presented UUID\n") ;
			StringDelete( &q ) ;
			return 110 ;			
		}	
	}else{
		if ( ( ac = strrchr( device,'/' ) ) != NULL ) {
			mapping_name =  ac + 1  ;
		}else{
			mapping_name =  device  ;			
		}
	}

	return exe( &clargs,mapping_name,uid ) ;	
} 
