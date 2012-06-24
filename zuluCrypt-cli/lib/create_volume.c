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
 * provides Process API to call mkfs.xxx tools through fork()/execl() to create file systems in encrypted volumes
 */
#include "../process/process.h"

static int result( int st,string_t x,string_t y,string_t z )
{
	if( x != NULL )
		StringDelete( &x ) ;
	if( y != NULL )
		StringDelete( &y ) ;
	if( z != NULL )
		StringDelete( &z ) ;
	return st ;
}

int create_volume( const char * dev,const char * fs,const char * type,const char * pass,size_t pass_size,const char * rng )
{
	int wait ;
	int status ;
	
	process_t p ;
	
	string_t cmd = NULL ;
	string_t m = NULL ;
	string_t pid = StringIntToString( getpid() ) ;
	
	const char * device_mapper ;
	const char * mapper ;	
	const char * copts ;
	
	if ( is_path_valid( dev ) != 0 )
		return result( 1,cmd,m,pid ) ;
		
	if( strcmp( type,"luks" ) == 0 )
		if( strcmp( rng,"/dev/random" ) != 0 )
			if( strcmp( rng,"/dev/urandom" ) != 0 )
				return result( 2,cmd,m,pid ) ;
			
	m = String( crypt_get_dir() ) ;	
		
	StringAppend( m,"/zuluCrypt-create-new-" ) ;
	device_mapper = StringAppendString( m,pid ) ;
	
	mapper = device_mapper + StringLastIndexOfChar( m,'/' ) + 1 ;
	
	if( is_path_valid( device_mapper ) == 0 )
		close_mapper( device_mapper );	
	
	if( strcmp( type,"luks" )  == 0 ){
		if( create_luks( dev,pass,pass_size,rng ) != 0 )	
			return result( 3,cmd,m,pid ) ;
		if( open_luks( dev,mapper,"rw",pass,pass_size ) != 0 )
			return result( 3,cmd,m,pid ) ; ;
	}else if( strcmp( type,"plain") == 0 ){
		if( open_plain( dev,mapper,"rw",pass,pass_size ) )
			return result( 3,cmd,m,pid ) ; ;		
	}else{
		return result( 2,cmd,m,pid ) ; ;
	}		

	cmd = String( ZULUCRYPTmkfs ) ;
	
	if( strcmp( fs,"ext2" ) == 0 || strcmp( fs,"ext3" ) == 0 || strcmp( fs,"ext4" ) == 0 ){
		StringAppend( cmd," -t extX -m 1 " ) ;
		StringReplaceString( cmd,"extX",fs ) ;	
		wait = 0 ;
	}else if( strcmp( fs,"reiserfs" ) == 0 ){
		StringAppend( cmd," -t reiserfs -f -f -q " ) ;
		wait = 0 ;
	}else if( strcmp( fs,"jfs" ) == 0 ){
		StringAppend( cmd," -t jfs -q " ) ;
		wait = 0 ;
	}else if( strcmp( fs,"ntfs" ) == 0 ){
		StringAppend( cmd," -t ntfs -f " ) ;
		wait = 0 ;
	}else{		
		/*
		 * unhandled fs are processed here.They are given 60 seconds to accomplish their task
		 * and are assumed to be running in interactive more and are blocked waiting for user input
		 * when they fail to return in time and hence are killed since we cant get to them from GUI 
		 */
		StringAppend( cmd," -t FS " ) ;
		StringReplaceString( cmd,"FS",fs ) ;
		wait = 1 ;
	}
	
	copts = StringAppend( cmd,device_mapper ) ;

	p = Process( copts ) ;
	
	ProcessSetOption( p,CLOSE_BOTH_STD_OUT ) ;

	if( wait == 1 )
		ProcessSetOptionTimeout( p,60,SIGKILL ) ;	
		
	ProcessStart( p ) ;

	status = ProcessExitStatus( p ) ;
	
	close_mapper( device_mapper );	
	
	ProcessDelete( &p ) ;
	
	if( status == 0 )
		return result( 0,cmd,m,pid ) ;
	else
		return result( 3,cmd,m,pid ) ;
}
