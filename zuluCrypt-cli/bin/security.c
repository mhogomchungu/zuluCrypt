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
#include "../lib/includes.h"
#include <blkid/blkid.h>
#include <errno.h>
#include <unistd.h>
#include <grp.h> 
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

#define ZULUDEBUG 0

static int has_access( const char * path,int c )
{
	int f ;
	if( c == READ )
		f = open( path,O_RDONLY );
	else
		f = open( path,O_WRONLY );
	
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

static int create_group( const char * groupname )
{
	process_t p ;
	int st = 1 ;
	if( zuluCryptSecurityGainElevatedPrivileges() ){
		p = Process( ZULUCRYPTgroupadd ) ;
		ProcessSetArgumentList( p,"-f",groupname,ENDLIST ) ;
		ProcessStart( p ) ;
		st = ProcessExitStatus( p ) ;
		ProcessDelete( &p ) ;
		zuluCryptSecurityDropElevatedPrivileges();
	}
	return st == 0 ;
}

int zuluCryptUserIsAMemberOfAGroup( uid_t uid,const char * groupname )
{
	int i = 0 ;
	struct group * grp ;
	struct passwd * pass ;
	
	const char ** entry ;
	const char * name ;
	
	if( groupname == NULL )
		return 0 ;
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	pass = getpwuid( uid ) ;
	
	if( pass == NULL ){
		zuluCryptSecurityDropElevatedPrivileges();
		return 0 ;
	}
	
	grp = getgrnam( groupname ) ;
	
	if( grp == NULL ){
		create_group( groupname )  ;
		zuluCryptSecurityDropElevatedPrivileges();
		return 0 ;
	}
	
	name = ( const char * )pass->pw_name ;
	entry = ( const char ** )grp->gr_mem ;
	
	while( entry[ i ] != NULL ){
		if( strcmp( entry[ i ],name ) == 0 ){
			return 1 ;
		}else{
			i++ ;
		}
	}
	
	zuluCryptSecurityDropElevatedPrivileges();
	
	return 0 ;
}

static int has_device_access( const char * path,int c )
{
	int f ;
	if( c == READ ){
		f = open( path,O_RDONLY );
	}else{
		f = open( path,O_WRONLY );
	}
	
	if( f == -1 ){
		switch( errno ){
			case EACCES : return 1 ; /* permission denied */
			case ENOENT : return 2 ; /* invalid path*/
			default     : return 4 ; /* common error */    
		}
	}else{
		close( f ) ;
		return 0 ;
	}
}
/*
 * 1-permissions denied
 * 2-invalid path
 * 3-shenanigans
 * 4-common error 
 */
int zuluCryptSecurityDeviceIsReadable( const char * device,uid_t uid )
{
	int st ;
	if( uid ){;}
	if( strncmp( device,"/dev/shm/",9 ) == 0 )
		return 4 ;
	if( strncmp( device,"/dev/",5 ) == 0 ){
		zuluCryptSecurityGainElevatedPrivileges() ;
		st = has_device_access( device,READ ) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
		return st ;
	}else{
		zuluCryptSecurityDropElevatedPrivileges() ;
		return has_device_access( device,READ ) ;
	}
}

int zuluCryptSecurityDeviceIsWritable( const char * device,uid_t uid )
{	
	int st ;
	if( uid ){;}
	if( strncmp( device,"/dev/shm/",9 ) == 0 )
		return 4 ;
	if( strncmp( device,"/dev/",5 ) == 0 ){
		zuluCryptSecurityGainElevatedPrivileges() ;
		st = has_device_access( device,WRITE ) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
		return st ;
	}else{
		zuluCryptSecurityDropElevatedPrivileges() ;
		return has_device_access( device,WRITE ) ;
	}
}

static int has_security_access( const char * path,int mode )
{
	int st ;
	if( strncmp( path,"/dev/shm/",9 ) == 0 )
		st = 2 ;
	if( strncmp( path,"/dev/",5 ) != 0 ){
		st = has_access( path,mode ) ;
	}else{
		if( zuluCryptSecurityGainElevatedPrivileges() ){
			st = has_access( path,mode ) ;
			zuluCryptSecurityDropElevatedPrivileges() ;
		}else{
			st = 3 ;
		}
	}
	return st ;
}

static int check_permissions( const char * path,int mode,const char * groupname,uid_t uid )
{
#if ZULUDEBUG
	printf( "check_permissions for: \"%s\":uid=%d:euid=%d\n",path,(int)getuid(),(int)geteuid() ) ;
#endif
	if( uid == 0 ){
		return has_access( path,mode ) ;
	}else{
		/*
		 * zuluCryptPartitionIsSystemPartition() is defined in ./partitions.c
		 */
		if( zuluCryptPartitionIsSystemPartition( path ) ){
			if( zuluCryptUserIsAMemberOfAGroup( uid,groupname ) ){
				return has_security_access( path,mode ) ;
			}else{
				return 3 ;
			}
		}else{
			return has_security_access( path,mode ) ;
		}
	}
}

int zuluCryptSecurityPathIsValid( const char * path,uid_t uid __attribute__((unused)) )
{
	int st = 0 ;
	if( strncmp( path,"/dev/",5 ) != 0 ){
		zuluCryptSecurityDropElevatedPrivileges();
		return has_access( path,READ ) == 0 ;
	}else{
		if( zuluCryptSecurityGainElevatedPrivileges() ){
			st = has_access( path,READ ) == 0 ;
			zuluCryptSecurityDropElevatedPrivileges() ;
		}
		return st ;
	}
}

int zuluCryptSecurityCanOpenPathForReading( const char * path,uid_t uid )
{
#if ZULUDEBUG
	printf( "check_read_permissions for: \"%s\":uid=%d:euid=%d\n",path,(int)getuid(),(int)geteuid() ) ;
#endif
	return check_permissions( path,READ,"zulucrypt",uid ) ;
}

int zuluCryptSecurityCanOpenPathForWriting( const char * path,uid_t uid )
{
#if ZULUDEBUG
	printf( "check_write_permissions for: \"%s\":uid=%d:euid=%d\n",path,(int)getuid(),(int)geteuid() ) ;
#endif
	return check_permissions( path,WRITE,"zulucrypt-write",uid ) ;
}

static string_t _create_default_mount_point( const char * device,uid_t uid,string_t path )
{
	string_t st = StringVoid ;
	char * loop_path = NULL ;
	const char * m_point ;
	if( strncmp( device,"/dev/loop",9 ) == 0 ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ../lib/create_loop_device.c
		 */
		loop_path = zuluCryptLoopDeviceAddress( device ) ;
		device = loop_path ;
	}
	
	m_point = StringAppend( path,strrchr( device,'/' ) ) ;
	if( zuluCryptSecurityGainElevatedPrivileges() ){
		if( mkdir( m_point,S_IRWXU ) == 0 ){
			st = path ;
			chown( m_point,uid,uid ) ;
		}else{
			StringDelete( &path ) ;
		}
		zuluCryptSecurityDropElevatedPrivileges() ;
	}
	if( loop_path != NULL ){
		free( loop_path ) ;
	}
	return st ;
}

static string_t _create_custom_mount_point( const char * label,uid_t uid,string_t path )
{
	string_t st = StringVoid ;
	const char * p = StringAppend( path,"/" ) ;
	const char * q = strrchr( label,'/' ) ;
	if( zuluCryptSecurityGainElevatedPrivileges() ){
		if( q == NULL ){
			p = StringAppend( path,label ) ;
		}else{
			p = StringAppend( path,q + 1 ) ;
		}
		if( mkdir( p,S_IRWXU ) == 0 ){
			st = path ;
			chown( p,uid,uid ) ;
		}else{
			StringDelete( &path ) ;
		}
		zuluCryptSecurityDropElevatedPrivileges() ;
	}
	return st ;
}

int zuluCryptSecurityMountPointPrefixMatch( const char * m_path,uid_t uid )
{
	int st ;
	int xt ;
	/*
	 * zuluCryptGetUserName() is defined in ../lib/user_home_path.c
	 */
	string_t uname = zuluCryptGetUserName( uid ) ;
	const char * str = StringPrepend( uname,"/run/media/" ) ;
	xt = StringLength( uname ) ;
	st = strncmp( str,m_path,xt ) ;
	StringDelete( &uname ) ;
	return st == 0 ;
}

string_t zuluCryptSecurityCreateMountPoint( const char * device,const char * label,uid_t uid )
{
	const char * m_point ;
	string_t path ;
	if( !zuluCryptSecurityGainElevatedPrivileges() )
		return StringVoid ;
	path = zuluCryptGetUserName( uid ) ;
	mkdir( "/run",S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH | S_IROTH ) ;
	chown( "/run/",0,0 ) ;
	mkdir( "/run/media",S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH | S_IROTH ) ;
	chown( "/run/media",0,0 ) ;
	m_point = StringPrepend( path,"/run/media/" ) ;
	mkdir( m_point,S_IRUSR | S_IXUSR ) ;
	chown( m_point,uid,uid ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	if( label == NULL ){
		return _create_default_mount_point( device,uid,path ) ;
	}else{
		return _create_custom_mount_point( label,uid,path ) ;
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
	size_t s ;
	string_t p ; 
	const char * z ;

	/*
	 * zuluCryptPathStartsWith() is defined in ./real_path.c
	 */
	if( zuluCryptPathStartsWith( path,"/dev/" ) ){
		/*
		 * whats wrong with you? :-) cant get a key from "/dev/"
		 */
		return 4 ;
	}
	
	/*
	 * zuluCryptGetUserHomePath() is defined in ../lib/user_home_path.c
	 */
	p = zuluCryptGetUserHomePath( uid ) ;
	z = StringAppend( p,".zuluCrypt-socket" ) ;
	s = StringLength( p ) ;
	
	if( strncmp( path,z,s ) == 0 ){
		StringDelete( &p ) ;
		zuluCryptSecurityDropElevatedPrivileges();
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
	
	zuluCryptSecurityDropElevatedPrivileges();

	switch( StringGetFromFile_3( st,path,0,KEYFILE_MAX_SIZE ) ){
		case 1 : return 1 ;
		case 2 : return 4 ;
		case 3 : return 2 ;
	}
	
	return 0 ;
}

int zuluCryptSecurityGainElevatedPrivileges( void )
{	
	if( seteuid( 0 ) == 0 ){
		if( setuid( 0 ) == 0 ){
			return 1 ;
		}else{
			puts( "WARNING: failed to seteuid root" ) ;
		}
	}else{
		puts( "WARNING: failed to setuid root" ) ;
	}
	return  0  ;
}

uid_t global_variable_user_uid ;
void zuluCryptGetUserUIDForPrivilegeManagement( uid_t uid ) 
{
	global_variable_user_uid = uid ;
}

int zuluCryptSecurityDropElevatedPrivileges( void )
{	
	gid_t gid = getgid() ;
	/*
	 * setgroups() requires seteuid(0) ;
	 */
	seteuid( 0 ) ;
	if( setgroups( 1,&gid ) != 0 ){
		perror( "setgroups() fail" ) ;
	}
	if( setegid( global_variable_user_uid ) != 0 ){
		perror( "setegid() fail" ) ;
	}
	if( seteuid( global_variable_user_uid ) != 0 ){
		perror( "seteuid() fail" ) ;
	}
	return 1 ;
}

char * zuluCryptSecurityEvaluateDeviceTags( const char * tag,const char * path )
{
	char * result = NULL ;
	if( zuluCryptSecurityGainElevatedPrivileges() ){
		result = blkid_evaluate_tag( tag,path,NULL) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
	}
	return result ;
}

string_t zuluCryptSecurityGetFileSystemFromDevice( const char * path )
{
	string_t st = StringVoid ;
	if( zuluCryptSecurityGainElevatedPrivileges() ){
		/*
		 * zuluCryptGetFileSystemFromDevice() is defined in ../lib/mount_volume.c
		 */
		st = zuluCryptGetFileSystemFromDevice( path ) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
	}
	return st ;
}

void zuluCryptSecuritySanitizeTheEnvironment( uid_t uid,stringList_t * stx )
{
	extern char ** environ ;
	const char ** env = ( const char ** ) environ ;
	ssize_t index ;
	stringList_t stl = StringListVoid ;
	StringListIterator  it ;
	StringListIterator end ;
	
	if( uid ){;}
	/*
	 * First,we make a copy of the enviromental varibales
	 * Second,we clear the enviromental variable because we dont want it
	 * Third,we return a copy of the enviromental variable because we want to pass it along
	 * the plugins 
	 */
	while( *env ){
		stl = StringListAppend( stl,*env ) ;
		env++ ;
	}

	it  = StringListBegin( stl ) ;
	end = StringListEnd( stl ) ;
	
	for( ; it != end ;it++ ){
		index = StringIndexOfChar( *it,0,'=' ) ;
		if( index >= 0 ){
			unsetenv( StringSubChar( *it,index,'\0' ) ) ;
			StringSubChar( *it,index,'=' ) ;
		}
	}
	
	*stx = stl ;
}

int zuluCryptSecurityUserOwnTheFile( const char * device,uid_t uid )
{
	if( device ){ ; }
	if( uid ){ ; }
	
	/*
	 * global_variable_file_struct variable is defined in ../lib/includes.h
	 * It is set in the main function.
	 * 
	 * It is set it defined and set in zuluCryptGetDeviceFileProperties() defined in ../lib/create_loop_device.c
	 */
	return 0 ;
}

char * zuluCryptSecurityUUIDFromPath( const char * device )
{
	blkid_probe blkid ;
	const char * c = NULL ;
	char * d = NULL;
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	blkid = blkid_new_probe_from_filename( device ) ;
	
	if( blkid != NULL ){
		blkid_do_probe( blkid );
		blkid_probe_lookup_value( blkid,"UUID",&c,NULL ) ;
		if( c != NULL ){
			d = strdup( c ) ;
		}
		blkid_free_probe( blkid );
	}
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	return d ;
}

void zuluCryptSecurityPrintPermissions( void )
{
	puts( "----------------------" ) ;
	printf( "uid:%d\neuid:%d\n",(int)getuid(),(int)geteuid() ) ;
	puts( "----------------------" ) ;
}
