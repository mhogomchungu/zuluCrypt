/*
 * 
 *  Copyright ( c ) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

//function prototypes
StrHandle * get_passphrase(  void  ) ;

void help(  void  ) ;
		 
int check_system_tools( void ) ;

StrHandle * get_passphrase(  void  )
{	
	
	//I got the big chunk of this code from: http://www.gnu.org/s/hello/manual/libc/getpass.html
	char c[2] ;
	StrHandle * p ;
	struct termios old, new;
			
       /* Turn echoing off and fail if we can't. */
	if ( tcgetattr ( 1, &old ) != 0 )
		exit( -1 );

	new = old;
	new.c_lflag &= ~ECHO;
			
	if ( tcsetattr ( 1, TCSAFLUSH, &new ) != 0 )
		exit( -1 );
		
	c[1] = '\0' ;
	c[0] = getchar(  ) ;
	
	p = String(  c  ) ;
	
	while(  (  c[0] = getchar(  )  ) != '\n'  )		
		StringAppend(  p, c  ) ;
	
	( void ) tcsetattr ( 1, TCSAFLUSH, &old );		
	
	return p ;
}

void help(  void  )
{
	printf( "run \"man zuluCrypt-cli\" for documentation on how to run this tool.\n" ) ;
	printf( "The same documentation can be found at http://code.google.com/p/zulucrypt\n" );	
}

int check_system_tools( void )
{	
	struct stat st ;
	StrHandle * p ;
	
	if( stat( ZULUCRYPTmkfs,&st ) == 0  ){
		
		return 0 ;
	}	
	p = String(  ZULUCRYPTmkfs ) ;
	StringAppend(  p , ZULUCRYPTzuluCrypt ) ;
	
	printf( "this program will not work as expected on your system\n" );
	printf( "because one or more of the following tools are either not present\n" ) ;
	printf( "or not where they are expected to be.\n%s\n",StringContent(  p  ) );
	
	StringDelete(  p  ) ;
	
	return 1 ;
}
int main(  int argc , char *argv[] )
{
	char * action = argv[1] ;
	
	char * device = argv[2] ;
	
	struct stat st ;
	
	uid_t id ;
	
	int status ;
	
	char *  mapping_name ;
	char * c ;
	
	id = getuid(  );	
	
	setuid( 0 );
	
	if(  check_system_tools(  ) == 1  )
		return 100 ;
	
	if (  argc < 2  ){
		help(  );
		return 10 ;
	}
	if (  strcmp(  action, "-h"  ) == 0 || strcmp(  action, "--help"  ) == 0 || strcmp(  action, "-help"  ) == 0  ){			
		help(  );	
		return 10 ;
	}
	
	if (  strcmp(  action, "-v"  ) == 0 || strcmp(  action, "-version"  ) == 0 || strcmp(  action, "--version"  ) == 0  ){		
		printf( "%s\n",version(  ) );
		return 10 ;
	}
	
	if (  argc < 3  ){
		help(  );
		return 10 ;
	}
		
	if (  ( c = strrchr( device,'/' ) ) != NULL ) {
		mapping_name =  c + 1  ;	
		
	}else{
		mapping_name =  device  ;			
	}		
	
	if(  strcmp(  action, "isLuks"  ) == 0  ){
	
		status =  is_luks(  device  ) ;
		
		if(  status == 0  )
			printf( "\"%s\" is a luks device\n",device ) ;
		else
			printf( "\"%s\" is not a luks device\n",device ) ;
		
	}else if (  strcmp(  action, "status"  ) == 0  ){			

		status = volume_info(  mapping_name  ) ;
		
	}else if (  strcmp(  action, "close"  ) == 0  ){			

		status =  close_opened_volume(  mapping_name  ) ;
		
	}else if (  strcmp(  action, "open"  ) == 0  ){
		
		status =  open_volumes( argc,device,mapping_name,id,argv[3],argv[4],argv[5],argv[6]  ) ;		
		
	}else if( strcmp( action,"create" ) == 0  ){

		status =  create_volumes( argc ,device,argv[3],argv[4],argv[5],argv[6],argv[7]  ) ;	
		
	}else if( strcmp( action,"addkey" ) == 0  ){
		
		status =  addkey( argc,device,argv[3],argv[4],argv[5],argv[6] ) ;
		
	}else if( strcmp( action,"removekey" ) == 0  ){
				
		status =  removekey( argc, device, argv[3],argv[4]  );	
	
	}else if (  strcmp( action,"partitions" ) == 0  ){

		switch(  argv[2][0]  ){			
			
			case '1' : c = partitions(  ALL_PARTITIONS  ) ;
				   break ;
			case '2' : c = partitions(  SYSTEM_PARTITIONS  ) ;
				   break ;
			case '3' : c = partitions(  NON_SYSTEM_PARTITIONS  ) ;
				   break ;
			default:
				   printf( "wrong argument\n" );
				   return 1 ;
		}		
		printf( "%s", c  ) ;
		free(  c  ) ;
		return 0 ;
		
	}else if( strcmp( action,"emptyslots" ) == 0  ){
		
		if(  stat( device,&st ) != 0  ){
			printf( "path \"%s\" does not point to a device\n",device ) ;
			status = 1 ;			
		}else{
			c = empty_slots(  device  ) ;
			
			if(  c == NULL  ){
				printf( "device \"%s\" is not a luks device\n",device ) ;
				status = 2 ;
			}else{
				printf( "%s\n",c  ) ;
				status = 0 ;
				free(  c  ) ;
			}		
		}
	}else{
		printf( "ERROR: Wrong argument\n" ) ;
		help(  );
		status =  10 ;
	}
	
	return status ; 		
}