/*
 *
 *  Copyright (c) 2013
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
#include <sys/stat.h>
#include <unistd.h>
#include "mount_prefix_path.h"

static string_t _create_home_default_mount_point( const char * device,uid_t uid,string_t path )
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

	if( mkdir( m_point,S_IRWXU ) == 0 ){
		st = path ;
		chown( m_point,uid,uid ) ;
	}else{
		StringDelete( &path ) ;
	}

	StringFree( loop_path ) ;

	return st ;
}

static string_t _create_home_custom_mount_point( const char * label,uid_t uid,string_t path )
{
	string_t st = StringVoid ;

	const char * p = StringAppend( path,"/" ) ;
	const char * q = strrchr( label,'/' ) ;
	const char * e ;

	if( q == NULL ){
		p = StringAppend( path,label ) ;
	}else{
		if( *( q + 1 ) == '\0' ){
			/*
			 * -m option was given with a path that ends with "/",backtrack until you find the second "/"
			 * from the right and use it as the last "/".
			 */
			e = q - 1 ;
			if( e < label ){
				/*
				 * -m option was given with a single "/".
				 */
				StringDelete( &path ) ;
				return st ;
			}
			while( 1 ){
				if( e == label ){
					StringAppend( path,e + 1 ) ;
					p = StringRemoveRight( path,1 ) ;
					break ;
				}else if( *e == '/' ){
					StringAppend( path,e + 1 ) ;
					p = StringRemoveRight( path,1 ) ;
					break ;
				}else{
					e-- ;
				}
			}
		}else{
			p = StringAppend( path,q + 1 ) ;
		}
	}

	if( mkdir( p,S_IRWXU ) == 0 ){
		st = path ;
		chown( p,uid,uid ) ;
	}else{
		StringDelete( &path ) ;
	}

	return st ;
}

static string_t create_home_mount_point( const char * device,const char * label,uid_t uid )
{
	string_t path = zuluCryptGetUserName( uid ) ;
	if( uid == 0 ){
		StringPrepend( path,"/" ) ;
	}else{
		StringPrepend( path,"/home/" ) ;
	}

	if( label == NULL ){
		return _create_home_default_mount_point( device,uid,path ) ;
	}else{
		return _create_home_custom_mount_point( label,uid,path ) ;
	}
}

static int home_mount_point_prefix_match( const char * m_path,uid_t uid,string_t * m_point )
{
	int st ;
	/*
	 * zuluCryptGetUserName() is defined in ../lib/user_home_path.c
	 */
	string_t uname = zuluCryptGetUserName( uid ) ;
	/*
	 * below constant are set in ../constants.h
	 */
	const char * str ;

	if( uid == 0 ){
		str = StringPrepend( uname,"/" ) ;
	}else{
		str = StringPrepend( uname,"/home/" ) ;
	}

	st = StringPrefixEqual( m_path,str ) ;

	if( m_point ){
		*m_point = uname ;
	}else{
		StringDelete( &uname ) ;
	}
	return st ;
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

	StringFree( loop_path ) ;

	return st ;
}

static string_t _create_custom_mount_point( const char * label,uid_t uid,string_t path )
{
	string_t st = StringVoid ;

	const char * p = StringAppend( path,"/" ) ;
	const char * q = strrchr( label,'/' ) ;
	const char * e ;

	zuluCryptSecurityGainElevatedPrivileges() ;

	if( q == NULL ){
		p = StringAppend( path,label ) ;
	}else{
		if( *( q + 1 ) == '\0' ){
			/*
			 * -m option was given with a path that ends with "/",backtrack until you find the second "/"
			 * from the right and use it as the last "/".
			 */
			e = q - 1 ;
			if( e < label ){
				/*
				 * -m option was given with a single "/".
				 */
				StringDelete( &path ) ;
				return st ;
			}
			while( 1 ){
				if( e == label ){
					StringAppend( path,e + 1 ) ;
					p = StringRemoveRight( path,1 ) ;
					break ;
				}else if( *e == '/' ){
					StringAppend( path,e + 1 ) ;
					p = StringRemoveRight( path,1 ) ;
					break ;
				}else{
					e-- ;
				}
			}
		}else{
			p = StringAppend( path,q + 1 ) ;
		}
	}
	if( mkdir( p,S_IRWXU ) == 0 ){
		st = path ;
		chown( p,uid,uid ) ;
	}else{
		StringDelete( &path ) ;
	}

	zuluCryptSecurityDropElevatedPrivileges() ;

	return st ;
}

static int mount_point_prefix_match( const char * m_path,uid_t uid,string_t * m_point )
{
	/*
	 * zuluCryptGetUserName() is defined in ../lib/user_home_path.c
	 */
	string_t uname = zuluCryptGetUserName( uid ) ;

	const char * str = StringPrepend( uname,"/run/media/private/" ) ;

	int st = StringPrefixEqual( m_path,str ) ;
	if( m_point ){
		*m_point = uname ;
	}else{
		StringDelete( &uname ) ;
	}
	return st ;
}

static string_t create_mount_point( const char * device,const char * label,uid_t uid )
{
	const char * m_point ;
	string_t path ;
	struct stat st ;
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH | S_IROTH ;

	zuluCryptSecurityGainElevatedPrivileges() ;

	path = zuluCryptGetUserName( uid ) ;

	#define path_does_not_exist( x ) stat( x,&st ) != 0

	if( path_does_not_exist( "/run" ) ){
		mkdir( "/run/",mode ) ;
		chown( "/run/",0,0 ) ;
	}
	if( path_does_not_exist( "/run/media" ) ){
		mkdir( "/run/media",mode ) ;
		chown( "/run/media",0,0 ) ;
	}
	if( path_does_not_exist( "/run/media/private" ) ){
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

static int home_mount_prefix( void )
{
	#if USE_HOME_PATH_AS_MOUNT_PREFIX
		return 1 ;
	#else
		return 0 ;
	#endif
}

string_t zuluCryptCreateMountPoint( const char * device,const char * label,const char * m_opts,uid_t uid )
{
	if( home_mount_prefix() ){
		return create_home_mount_point( device,label,uid ) ;
	}else{
		if( StringHasComponent( m_opts,"mount-prefix=home" ) ){
			if( zuluCryptUserIsAMemberOfAGroup( uid,"zulumount" ) ){
				return create_home_mount_point( device,label,uid ) ;
			}else{
				return StringVoid ;
			}
		}else{
			return create_mount_point( device,label,uid ) ;
		}
	}
}

int zuluCryptMountPointPrefixMatch( const char * m_path,uid_t uid,string_t * m_point )
{
	if( home_mount_prefix() ){
		return home_mount_point_prefix_match( m_path,uid,m_point ) ;
	}else{
		if( mount_point_prefix_match( m_path,uid,m_point ) ){
			return 1 ;
		}else{
			return home_mount_point_prefix_match( m_path,uid,m_point ) ;
		}
	}
}
