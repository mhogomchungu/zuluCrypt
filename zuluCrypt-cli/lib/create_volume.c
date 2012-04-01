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

#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

/*
 * Some mkfs.xxx tools like mkfs.reiserfs requires and option to be passed to run them in non-interactive mode.
 * Some mkfs.xxx tools like mkfs.ext4 do not. There does not seem to be a standard way to know which tool 
 * require an option and which doesnt and i cant know all of them. The ones i know are specified in the code below in the child process.
 * 
 * I am not aware of any mkfs.xxx tool that will take 20 seconds to complete its task.Hence each mkfs.xxx is given 20 seconds to complete.
 * A tool that continue to run after 20 seconds is assumed to be stuck at an interactive prompt and will be killed since a tool
 * stuck in interactive mode waiting for user input will have the library and whoever is using it. * 
 */

typedef struct st_2{
	pid_t * pid  ;
	int * status ;
}st_1;

void * kill_hanged_mkfs( void * p )
{
	st_1 * st = ( st_1 * ) p ;
	sleep( 20 ) ;
	if( *( st->status ) < 0 ){	
		/*
		 *  mkfs.xxx is still running, kill the process
		 */
		kill( *( st->pid ),SIGKILL ) ;		
	}
	return ( void * ) 0 ; 
}

#define DEVICE_MAPPER "/dev/mapper/zuluCrypt-create-new"
#define MAPPER        "zuluCrypt-create-new"

int create_volume( const char * dev,const char * fs,const char * type,const char * pass,size_t pass_size,const char * rng )
{
	int status ;
	pid_t pid ;
	pthread_t thread ;
	st_1 st ;

	if ( is_path_valid( dev ) == 1 )
		return 1 ;
		
	if( strcmp( type,"luks" ) == 0 )
		if(  strcmp( rng,"/dev/random" ) != 0 )
			if( strcmp( rng,"/dev/urandom" ) != 0 )
				return 2 ;
			
	if( is_path_valid( DEVICE_MAPPER ) == 0 )
		close_mapper( DEVICE_MAPPER );	
	
	if( strcmp( type,"luks" )  == 0 ){
		if( create_luks( dev,pass,pass_size,rng ) != 0 )	
			return 3 ;
		if( open_luks( dev,MAPPER,"rw",pass,pass_size ) != 0 )
			return 3 ;
	}else if( strcmp( type,"plain") == 0 ){
		if( open_plain( dev,MAPPER,"rw",pass,pass_size,"cbc-essiv:sha256" ) )
			return 3 ;		
	}else{
		return 2 ;
	}		
	
	pid = fork() ;
	if( pid == -1 )
		return 3 ;	
	if( pid == 0 ){
		close( 1 ); 
		close( 2 );
		if( strcmp( fs,"ext2" ) == 0 || strcmp( fs,"ext3" ) == 0 || strcmp( fs,"ext4" ) == 0 )
			execl( ZULUCRYPTmkfs,"mkfs","-t",fs,"-m","1",DEVICE_MAPPER,( char * ) 0 ) ;
		else if( strcmp( fs,"reiserfs" ) == 0 )
			execl( ZULUCRYPTmkfs,"mkfs","-t","reiserfs","-f","-f","-q",DEVICE_MAPPER,( char * ) 0 ) ;	
		else if( strcmp( fs,"jfs" ) == 0 )
			execl( ZULUCRYPTmkfs,"mkfs","-t","jfs","-q",DEVICE_MAPPER,( char * ) 0 ) ;
		else if( strcmp( fs,"ntfs" ) == 0 )
			execl( ZULUCRYPTmkfs,"mkfs","-t","ntfs","-f",DEVICE_MAPPER,( char * ) 0 ) ;
		else
			execl( ZULUCRYPTmkfs,"mkfs","-t",fs,DEVICE_MAPPER,( char * ) 0 ) ;
	}
	
	st.pid = &pid ;
	st.status = &status ;
	/*
	 * waitpid below will set status value to a number > 0 before it return, signifying the forked process has finished.
	 * checking this value is one of the ways the thread can know the forked process is still running 
	 */
	status = -1 ; 
	
	pthread_create( &thread,NULL,kill_hanged_mkfs,( void * ) &st );
	
	waitpid( pid,&status,0 ) ;	
	
	pthread_cancel( thread );
	
	close_mapper( "/dev/mapper/zuluCrypt-create-new" );	
	
	if( status == 0 )
		return 0 ;
	else
		return 3 ;	
}
