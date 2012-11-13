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

#include <errno.h>
#include <unistd.h>

#include "../constants.h"
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
/*
 * This source file makes sure the user who started the tool( usually non root user ) has permission 
 * to perform operations they want on paths they presented.
 * 
 * This feature allows tradition unix permissions to be set on a paths to control non user access to volumes  
 */

static int has_access( const char * path,int c,uid_t uid )
{
	int f ;
	
	uid_t org = getuid() ;
	
	seteuid( uid ) ;
	
	if( c == READ )
		f = open( path,O_RDONLY );
	else
		f = open( path,O_WRONLY );
	
	seteuid( org ) ;
	
	if( f >= 0 ){
		close( f ) ;
		return 0 ;
	}else{
		switch( errno ){
			case EACCES : return 1 ; /* permission denied */
			case ENOENT : return 2 ; /* invalid path*/
			default     : return 3 ; /* common error */    
		}
	}
}

static int check_group( struct group ** grp1,const char * groupname )
{
	struct group * grp ;
	grp = getgrnam( groupname ) ;
	if( grp != NULL ){
		*grp1 = grp ;
		return 0 ;
	}else{
		return 1 ;
	}
}

static int create_group( const char * groupname )
{
	process_t p = Process( ZULUCRYPTgroupadd ) ;
	ProcessSetArgumentList( p,"-f",groupname,ENDLIST ) ;
	ProcessStart( p ) ;
	ProcessExitStatus( p ) ;
	ProcessDelete( &p ) ;
	return 1 ;
}

static int has_access_1( int op,uid_t uid )
{
	int i = 0 ;
	struct group * grp ;
	struct passwd * pass ;
	
	const char ** entry ;
	const char * name ;
	
	const char * groupname ;
	
	if( op == READ )
		groupname = "zulucrypt-read" ;
	else
		groupname = "zulucrypt-write";
	
	pass = getpwuid( uid ) ;
	
	if( pass == NULL )
		return 3 ;
	
	if( check_group( &grp,groupname ) )
		return create_group( groupname ) ;
	
	name = ( const char * )pass->pw_name ;
	entry = ( const char ** )grp->gr_mem ;
	
	while( entry[ i ] != NULL ){
		if( strcmp( entry[ i ],name ) == 0 ){
			return 0 ;
		}else{
			i++ ;
		}
	}
	
	return 1 ;
}

int zuluCryptSecurityCheckPartitionPermissions( uid_t uid )
{
	int read ;
	int write;
	
	if( uid == 0 )
		return 1 ;
	
	read = has_access_1( READ,uid )   == 0 ;
	write = has_access_1( WRITE,uid ) == 0 ;
	
	if( read && write ){
		puts( "read and write" ) ;
		return 1 ;
	}else if( read && !write ){
		puts( "read only" ) ;
		return 2 ;
	}else if( !read && write ){
		puts( "write only" ) ;
		return 3 ;
	}else if( !read && !write ){
		puts( "no read and no write" ) ;
		return 4 ;
	}
	/*
	 * shouldnt get here
	 */
	return 0 ;
}

int zuluCryptSecurityCanOpenPathForReading( const char * path,uid_t uid )
{
	if( uid == 0 ){
		return 0 ;
	}else if( strncmp( path,"/dev/",5 ) != 0 ){
		return has_access( path,READ,uid ) ;
	}else{
		return has_access_1( READ,uid ) ;
	}
}

int zuluCryptSecurityCanOpenPathForWriting( const char * path,uid_t uid )
{
	if( uid == 0 ){
		return 0 ;
	}else if( strncmp( path,"/dev/",5 ) != 0 ){
		return has_access( path,WRITE,uid ) ;
	}else{
		return has_access_1( WRITE,uid ) ;
	}
}

int zuluCryptSecurityCreateMountPoint( const char * path,uid_t uid )
{
	int st ;
	
	uid_t org = geteuid();    
	
	seteuid( uid ) ;            
	
	st = mkdir( path,S_IRWXU ) ;
	
	seteuid( org ) ;           
	
	if( st == 0 )
		return 0 ;
	else{
		switch( errno ){
			case EACCES : return 1 ; 
			case EEXIST : return 2 ; ; 
			default     : return 3 ; ;     
		}
	}
}

/*
 *  return values:
 *  5 - couldnt get key from the socket
 *  4 -permission denied
 *  1  invalid path
 *  2  insufficient memory to open file
 *  0  success * 
 */
int zuluCryptSecurityGetPassFromFile( const char * path,uid_t uid,string_t * st )
{
	/*
	 * zuluCryptGetUserHomePath() is defined in ../lib/user_get_home_path.c
	 */
	string_t p = zuluCryptGetUserHomePath( uid ) ;
	const char * z = StringAppend( p,".zuluCrypt-socket" ) ;
	size_t s = StringLength( p ) ;
	
	if( strncmp( path,z,s ) == 0 ){
		StringDelete( &p ) ;
		/*
		 * path that starts with $HOME/.zuluCrypt-socket is treated not as a path to key file but as path
		 * to a local socket to get a passphrase 
		 * 
		 * This function is defined in ../pluginManager/zuluCryptPluginManager.c
		 */
		return zuluCryptGetKeyFromSocket( path,st,uid ) > 0 ? 0 : 5 ;
	}
	
	StringDelete( &p ) ;
	
	switch ( zuluCryptSecurityCanOpenPathForReading( path,uid ) ){
		case 1 : return 4 ;
		case 2 : return 1 ;
	}
	
	switch( StringGetFromFile_3( st,path,0,KEYFILE_MAX_SIZE ) ){
		case 1 : return 1 ;
		case 2 : return 4 ;
		case 3 : return 2 ;
	}
	
	return 0 ;
}
