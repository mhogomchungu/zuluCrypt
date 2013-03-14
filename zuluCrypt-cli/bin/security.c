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
	if( c == ZULUCRYPTread ){
		f = open( path,O_RDONLY );
	}else{
		f = open( path,O_WRONLY );
	}
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

static int create_group( const char * groupname ) __attribute__((unused)) ;

static int create_group( const char * groupname )
{
	process_t p ;
	int st = 1 ;
	zuluCryptSecurityGainElevatedPrivileges() ;
	p = Process( ZULUCRYPTgroupadd ) ;
	ProcessSetArgumentList( p,"-f",groupname,ENDLIST ) ;
	ProcessStart( p ) ;
	st = ProcessExitStatus( p ) ;
	ProcessDelete( &p ) ;
	zuluCryptSecurityDropElevatedPrivileges();
	return st == 0 ;
}

int zuluCryptUserIsAMemberOfAGroup( uid_t uid,const char * groupname )
{
	int st = 0 ;
	int i = 0 ;
	struct group * grp ;
	struct passwd * pass ;
	
	const char ** entry ;
	const char * name ;
	
	if( groupname == NULL ){
		return 0 ;
	}
	if( uid == 0 ){
		return 1 ;
	}
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	pass = getpwuid( uid ) ;
	
	if( pass == NULL ){
		zuluCryptSecurityDropElevatedPrivileges();
		return 0 ;
	}
	
	grp = getgrnam( groupname ) ;
	
	if( grp == NULL ){
		/*
		 * 	dont autocreate groups
		 *	create_group( groupname )  ;
		 */
		zuluCryptSecurityDropElevatedPrivileges();
		return 0 ;
	}
	
	name = ( const char * )pass->pw_name ;
	entry = ( const char ** )grp->gr_mem ;
	
	while( entry[ i ] != NULL ){
		if( StringsAreEqual( entry[ i ],name ) ){
			st = 1 ;
			break ;
		}else{
			i++ ;
		}
	}
	
	zuluCryptSecurityDropElevatedPrivileges();
	
	return st ;
}

static int has_device_access( const char * path,int c )
{
	int f ;
	if( c == ZULUCRYPTread ){
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
	if( StringPrefixMatch( device,"/dev/shm/",9 ) ){
		return 4 ;
	}
	if( StringPrefixMatch( device,"/dev/",5 ) ){
		zuluCryptSecurityGainElevatedPrivileges() ;
		st = has_device_access( device,ZULUCRYPTread ) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
		return st ;
	}else{
		zuluCryptSecurityDropElevatedPrivileges() ;
		return has_device_access( device,ZULUCRYPTread ) ;
	}
}

int zuluCryptSecurityDeviceIsWritable( const char * device,uid_t uid )
{	
	int st ;
	if( uid ){;}
	if( StringPrefixMatch( device,"/dev/shm/",9 ) ){
		return 4 ;
	}
	if( StringPrefixMatch( device,"/dev/",5 ) ){
		zuluCryptSecurityGainElevatedPrivileges() ;
		st = has_device_access( device,ZULUCRYPTwrite ) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
		return st ;
	}else{
		zuluCryptSecurityDropElevatedPrivileges() ;
		return has_device_access( device,ZULUCRYPTwrite ) ;
	}
}

static int has_security_access( const char * path,int mode )
{
	int st ;
	if( StringPrefixMatch( path,"/dev/shm/",9 ) ){
		st = 2 ;
	}
	if( !StringPrefixMatch( path,"/dev/",5 ) ){
		st = has_access( path,mode ) ;
	}else{
		zuluCryptSecurityGainElevatedPrivileges() ;
		st = has_access( path,mode ) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
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
	if( !StringPrefixMatch( path,"/dev/",5 ) ){
		zuluCryptSecurityDropElevatedPrivileges();
		return has_access( path,ZULUCRYPTread ) == 0 ;
	}else{
		if( zuluCryptSecurityGainElevatedPrivileges() ){
			st = has_access( path,ZULUCRYPTread ) == 0 ;
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
	return check_permissions( path,ZULUCRYPTread,"zulucrypt",uid ) ;
}

int zuluCryptSecurityCanOpenPathForWriting( const char * path,uid_t uid )
{
#if ZULUDEBUG
	printf( "check_write_permissions for: \"%s\":uid=%d:euid=%d\n",path,(int)getuid(),(int)geteuid() ) ;
#endif
	return check_permissions( path,ZULUCRYPTwrite,"zulucrypt-write",uid ) ;
}

static string_t _create_default_mount_point( const char * device,uid_t uid,string_t path )
{
	string_t st = StringVoid ;
	char * loop_path = NULL ;
	const char * m_point ;
	if( StringPrefixMatch( device,"/dev/loop",9 ) ){
		/*
		 * zuluCryptLoopDeviceAddress_1() is defined in ../lib/create_loop_device.c
		 */
		device = loop_path = zuluCryptLoopDeviceAddress_1( device ) ;
	}
	
	m_point = StringAppend( path,strrchr( device,'/' ) ) ;
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	if( mkdir( m_point,S_IRWXU ) == 0 ){
		st = path ;
		chown( m_point,uid,uid ) ;
	}else{
		StringDelete( &path ) ;
	}
	
	zuluCryptSecurityDropElevatedPrivileges() ;

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

int zuluCryptSecurityMountPointPrefixMatch( const char * m_path,uid_t uid,string_t * m_point )
{
	int st ;
	/*
	 * zuluCryptGetUserName() is defined in ../lib/user_home_path.c
	 */
	string_t uname = zuluCryptGetUserName( uid ) ;
	/*
	 * below constant are set in ../constants.h
	 */
	const char * str = StringPrepend( uname,"/run/media/private/" ) ;
	st = StringPrefixEqual( m_path,str ) ;
	if( m_point ){
		*m_point = uname ;
	}else{
		StringDelete( &uname ) ;
	}
	return st ;
}

string_t zuluCryptSecurityCreateMountPoint( const char * device,const char * label,uid_t uid )
{
	const char * m_point ;
	string_t path ;
	struct stat st ;
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH | S_IROTH ;
	
	zuluCryptSecurityGainElevatedPrivileges() ;

	path = zuluCryptGetUserName( uid ) ;
	
	/*
	 * below constants are set in ../constants.h
	 * ZULUCRYPtmountMiniPath contains "/run"
	 * ZULUCRYPTmountPath contains "/run/media"
	 */
	if( stat( "/run",&st ) != 0 ){
		mkdir( "/run/",mode ) ;
		chown( "/run/",0,0 ) ;
	}
	if( stat( "/run/media",&st ) != 0 ){
		mkdir( "/run/media",mode ) ;
		chown( "/run/media",0,0 ) ;
	}
	if( stat( "/run/media/private",&st ) != 0 ){
		mkdir( "/run/media/private",mode ) ;
		chown( "/run/media/private",0,0 ) ;
	}
	
	m_point = StringPrepend( path,"/run/media/private/" ) ;
	
	if( stat( m_point,&st ) != 0 ){
		mkdir( m_point,S_IRUSR | S_IXUSR ) ;
		chown( m_point,uid,uid ) ;
	}
	
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
	 * zuluCryptGetUserHomePath() is defined in ../lib/user_home_path.c
	 */
	p = zuluCryptGetUserHomePath( uid ) ;
	z = StringAppend( p,".zuluCrypt-socket" ) ;
	s = StringLength( p ) ;
	
	zuluCryptSecurityDropElevatedPrivileges();
	
	if( StringPrefixMatch( path,z,s ) ){
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
	

	/*
	 * ZULUCRYPT_KEYFILE_MAX_SIZE is set in ../constants.h
	 */
	switch( StringGetFromFile_3( st,path,0,ZULUCRYPT_KEYFILE_MAX_SIZE ) ){
		case 1 : return 1 ;
		case 2 : return 4 ;
		case 3 : return 2 ;
	}
	
	return 0 ;
}

int zuluCryptSecurityGainElevatedPrivileges( void )
{	
	/*
	printf( "GAINING:uid=%d:euid=%d\n",getuid(),geteuid() ) ;
	*/
	if( seteuid( 0 ) == 0 ){
		return 1 ;
	}else{
		if( zuluCryptSecurityPrivilegeElevationError ){
			zuluCryptSecurityPrivilegeElevationError( "WARNING: failed to seteuid root" ) ;
		}
	}
	return  0  ;
}

uid_t global_variable_user_uid ;
void zuluCryptGetUserUIDForPrivilegeManagement( uid_t uid ) 
{
	global_variable_user_uid = uid ;
}

void ( *zuluCryptSecurityPrivilegeElevationError )( const char * ) = NULL ;

void zuluCryptSecuritySetPrivilegeElevationErrorFunction( void ( *f ) ( const char * ) ) 
{
	zuluCryptSecurityPrivilegeElevationError = f ;
}

int zuluCryptSecurityDropElevatedPrivileges( void )
{		
	/*
	 printf( "DROPPING:uid=%d:euid=%d\n",getuid(),geteuid() ) ;
	 */
	if( seteuid( global_variable_user_uid ) != 0 ){
		if( zuluCryptSecurityPrivilegeElevationError ){
			zuluCryptSecurityPrivilegeElevationError( "ERROR: seteuid() failed" ) ;
		}
	}
	return 1 ;
}

char * zuluCryptSecurityEvaluateDeviceTags( const char * tag,const char * path )
{
	char * result = NULL ;
	zuluCryptSecurityGainElevatedPrivileges() ;
	result = blkid_evaluate_tag( tag,path,NULL ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	return result ;
}

string_t zuluCryptSecurityGetFileSystemFromDevice( const char * path )
{
	string_t st = StringVoid ;
	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptGetFileSystemFromDevice() is defined in ../lib/mount_volume.c
	 */
	st = zuluCryptGetFileSystemFromDevice( path ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
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
	
	for( ; it != end ; it++ ){
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
	return 0 ;
}

char * zuluCryptSecurityUUIDFromPath( const char * device )
{
	blkid_probe blkid ;
	const char * c = NULL ;
	string_t st = StringVoid ;
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	blkid = blkid_new_probe_from_filename( device ) ;
	
	if( blkid != NULL ){
		blkid_do_probe( blkid );
		blkid_probe_lookup_value( blkid,"UUID",&c,NULL ) ;
		st = String( c ) ;
	}
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	return StringDeleteHandle( &st ) ;
}

void zuluCryptSecurityPrintPermissions( void )
{
	puts( "----------------------" ) ;
	printf( "uid:%d\neuid:%d\n",(int)getuid(),(int)geteuid() ) ;
	puts( "----------------------" ) ;
}
