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

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include "zuluCrypt.h"
#include <unistd.h>
#include "String.h"
#include <fcntl.h>

StrHandle * get_passphrase( void )
{	
	
	//I got the big chunk of this code from: http://www.gnu.org/s/hello/manual/libc/getpass.html
	char c[2] ;
	StrHandle * p ;
	
	struct termios old, new;
			
       /* Turn echoing off and fail if we can't. */
	if (tcgetattr (1, &old) != 0)
		exit(-1);

	new = old;
	new.c_lflag &= ~ECHO;
			
	if (tcsetattr (1, TCSAFLUSH, &new) != 0)
		exit(-1);
		
	c[1] = '\0' ;
	c[0] = getchar() ;
	
	p = StringCpy( c ) ;
	
	while( ( c[0] = getchar() ) != '\n' ){
		
		StringCat( p, c ) ;
	}
	
	(void) tcsetattr (1, TCSAFLUSH, &old);	
	
	return p ;
}

void help( void )
{
	printf("will write docs at some point\n");	
}

int volume_info( char * mapping_name )
{
	struct stat st ;
	char x[512] ;
	int Z ;
	
	StrHandle *s = StringCpy("/dev/mapper/zuluCrypt-") ;
	StringCat( s, mapping_name ) ;
	
	if( stat( StringCont( s ),&st) == 0 ){			
		status(mapping_name,x ,512 ) ;
		printf("%s",x);
		Z = 0 ;
	}else{
		printf("No opened volume associated with provided name\n") ;
		Z = 1 ;
	}
	
	StringDelete( s ) ;
	
	return Z ;	
}

int close_opened_volume( char * mapping_name )
{	
	int st = close_volume( mapping_name ) ;
	switch( st ) {
	case 0 : printf("SUCCESS: volume successfully closed\n");
		break ;
	case 1 : printf("ERROR: close failed, encrypted volume with that name does not exist\n");
		break ;
			
	case 2 : printf("ERROR: close failed, the mount point and/or one or more files are in use\n");
		break ;
			
	default :
		; //shouldnt get here			
	}	
	return st ;	
}

int open_volumes(int argn, char * device, char * mapping_name,int id, char * mount_point, char * mode,char *source, char * pass)
{
	StrHandle * p ;
	int st ;
	int f ;
	struct stat Q ;
	char *c ;

	if (strncmp(mount_point,",\0",2)==0){
			
		printf("ERROR, \",\"(comma) is not a valid mount point\n") ;
		return 9 ;			
	}		

	if (strncmp(mode,"ro",2) != 0){
		if (strncmp(mode,"rw",2) != 0){
			printf("ERROR: Wrong mode, run zuluCrypt with \"-h\" for help\n");;
			return 1 ;	
		}
	}		

	if (strncmp(mount_point,"-\0",2) ==0 ){
		mount_point = NULL ;
	}
	
	if ( argn == 5 ){
		printf( "Enter passphrase: " ) ;

		p = get_passphrase();				
		
		printf("\n") ;	
		
		st = open_volume(device, mapping_name,mount_point,id,mode,StringCont( p )) ;
		StringDelete( p ) ;
		
	}else if ( argn == 7 ){

		if( strcmp(source,"-p") == 0 ){
			
			st = open_volume(device, mapping_name,mount_point,id,mode, pass) ;
		
		}else if ( strcmp(source,"-f") == 0 ){
			
			if ( stat(pass,&Q) == 0 ){			
				
				c = ( char * ) malloc( sizeof(char) *  Q.st_size  ) ;
			
				f = open( pass,O_RDONLY ) ;
			
				read(f,c,Q.st_size) ;
				close(f);
				st = open_volume(device, mapping_name,mount_point,id,mode,c) ;
				free( c ) ;
			}else{
				printf("ERROR: passphrase file does not exist\n");

				return 6 ;
			}
		}else{
			printf("ERROR: Wrong option, run zuluCrypt with \"-h\" for help\n");
			return 7 ;
		}
	}
	else{
		printf("ERROR: Wrong number of arguments, run zuluCrypt with \"-h\" for help\n");
		printf("%d\n",argn) ;
		return 8 ;			
	}

	switch ( st ){
			
	case 0 : printf("SUCCESS: Volume opened successfully\n");
		break ;
			
	case 1 : printf("ERROR: No free loop device to use\n") ; 
		break ;			
			
	case 2 : printf("ERROR: There seem to be an open volume accociated with given address\n");
		break ;			
			
	case 3 : printf("ERROR: No file exist on given path\n") ; 
		break ;
			
	case 4 : printf("ERROR: Wrong passphrase\n");
		break ;
			
	case 5 : printf("ERROR: a file or folder already exist at mount point\n") ;
		break ;
	default :
		;			
	}	
	return st ;
}


int create_volumes(int argn ,char *device, char *fs, char * mode, char * pass )
{
	int flush_the_stdin_buffer ;
	StrHandle * p ;
	StrHandle * q ;
	char x[512] ;
	char Y ;
	int st ;
	
	if( realpath(device, x ) == NULL )
		return 4 ;		
		
	if(strncmp(x,"/dev/",5) == 0 ){
		printf("currently not supporting creating volumes on hard drives.\n") ;
		printf("This is an suid program and without any checks,this functionality\nwill enable ");
		printf("normal users to effectively delete a partition like roots.\n");
			
		return 6 ;
	}
			 
	if( argn == 5 ){
		printf("ARE YOU SURE YOU WANT TO CREATE/OVERWRITE: \"%s\" ? Type \"Y\" if you are\n",device);
		scanf("%c",&Y);
		
		if ( Y != 'Y')
			return 3 ;
			
		//flush the buffer stdin buffer	
		while ((flush_the_stdin_buffer = getchar()) != '\n' && flush_the_stdin_buffer != EOF);
			
		printf("Enter passphrase: ") ;

		p = get_passphrase();
			
		printf("\nRe enter passphrase: ") ;

		q = get_passphrase();	
			
		printf("\n") ;
			
		if(strcmp(StringCont( p ),StringCont( q )) != 0){
			printf("\nERROR: passphrases do not match\n") ;
			printf("%s %d\n",StringCont( p ),StringLength( p ));
			printf("%s %d\n",StringCont( q ),StringLength( q ));
				
			StringDelete( q ) ;
			StringDelete( p ) ;
			return 3 ;
		}		
			
	}else if ( argn == 6 ){
		p = StringCpy( pass ) ;								
	}else{
		printf("ERROR: Wrong number of arguments, run zuluCrypt with \"-h\" for help\n");
		return 4 ;
	}		
	st = create_volume(device,fs,mode,StringCont( p )) ;;
		
	if ( st == 1 ){
		printf("ERROR: File path given does not point to a file or partition\n") ;
	}else if ( st == 2 ){	
		printf("ERROR: Unrecognized volume type.\n");
	}	
	
	if ( argn == 5 ){
		StringDelete( p ) ;	
		StringDelete( q ) ;	
	}else
		StringDelete( p ) ;	
	
	return st ;
}

int main( int argc , char *argv[])
{
	char * action = argv[1] ;
	char * device = argv[2] ;

	uid_t id ;
	
	char *  mapping_name ;
	char * c ;
	
	id = getuid();	
	
	setuid(0);
	
	if ( strcmp( action, "-h" ) == 0 || strcmp( action, "--help" ) == 0 || strcmp( action, "-help" ) == 0 ){			
		help();	
		return 10 ;
	}
	
	if ( argc < 3 ){
		help();
		return 10 ;
	}
		
	if ( (c = strrchr(device,'/')) != NULL) {
		mapping_name =  c + 1  ;	
		
	}else{
		mapping_name =  device  ;			
	}		

	if ( strcmp( action, "status" ) == 0 ){			

		return volume_info( mapping_name ) ;
		
	}else if ( strcmp( action, "close" ) == 0 ){			

		return close_opened_volume( mapping_name ) ;
		
	}else if ( strcmp( action, "open" ) == 0 ){
		
		return open_volumes(argc,argv[2],mapping_name,id,argv[3],argv[4],argv[5],argv[6] ) ;		
		
	}else if(strcmp(action,"create") == 0 ){

		return create_volumes(argc ,argv[2],argv[3],argv[4],argv[5] ) ;
	}	
	
	return 0 ; //shouldnt get here
		
}