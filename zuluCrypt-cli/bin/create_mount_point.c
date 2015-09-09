/*
 *
 *  Copyright (c) 2013-2015
 *  name : Francis Banyikwa
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

static void _chown( const char * x,uid_t y,gid_t z )
{
	if( chown( x,y,z ) ){;}
}
static void _chmod( const char * x,mode_t y )
{
	if( chmod( x,y ) ){;}
}
static void _mkdir( const char * x,mode_t y )
{
	if( mkdir( x,y ) ){;}
}

static string_t _create_path_0( const char * m_point,uid_t uid,string_t path )
{
	if( mkdir( m_point,S_IRWXU ) == 0 ){

		_chown( m_point,uid,uid ) ;
	}else{
		StringDelete( &path ) ;
	}

	return path ;
}

static string_t _create_path( uid_t uid,string_t path,int need_privileges )
{
	string_t st = StringVoid ;

	const char * m_point = StringContent( path ) ;

	if( m_point != NULL ){

		if( need_privileges ){

			zuluCryptSecurityGainElevatedPrivileges() ;

			st = _create_path_0( m_point,uid,path ) ;

			zuluCryptSecurityDropElevatedPrivileges() ;
		}else{
			st = _create_path_0( m_point,uid,path ) ;
		}
	}

	return st ;
}

static string_t _create_mount_point_1( const char * device,uid_t uid,string_t path,int need_privileges )
{
	string_t st ;
	char * loop_path = NULL ;

	if( StringPrefixEqual( device,"/dev/loop" ) ){
		/*
		 * zuluCryptLoopDeviceAddress_1() is defined in ../lib/create_loop_device.c
		 */
		device = loop_path = zuluCryptLoopDeviceAddress_1( device ) ;
	}

	StringMultipleAppend( path,device + StringLastIndexOfChar_1( device,'/' ) + 1,NULL ) ;

	st = _create_path( uid,path,need_privileges ) ;

	StringFree( loop_path ) ;

	return st ;
}

static string_t _create_home_default_mount_point( const char * device,uid_t uid,string_t path )
{
	return _create_mount_point_1( device,uid,path,0 ) ;
}

static string_t _create_default_mount_point( const char * device,uid_t uid,string_t path )
{
	return _create_mount_point_1( device,uid,path,1 ) ;
}

static string_t _create_mount_point_0( const char * label,uid_t uid,string_t path,int need_privileges )
{
	const char * q = strrchr( label,'/' ) ;
	const char * e ;

	if( q == NULL ){
		StringAppend( path,label ) ;
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
				return StringVoid ;
			}
			while( 1 ){
				if( e == label ){
					StringAppend( path,e + 1 ) ;
					StringRemoveRight( path,1 ) ;
					break ;
				}else if( *e == '/' ){
					StringAppend( path,e + 1 ) ;
					StringRemoveRight( path,1 ) ;
					break ;
				}else{
					e-- ;
				}
			}
		}else{
			StringAppend( path,q + 1 ) ;
		}
	}

	return _create_path( uid,path,need_privileges ) ;
}

static string_t _create_home_custom_mount_point( const char * label,uid_t uid,string_t path )
{
	return _create_mount_point_0( label,uid,path,0 ) ;
}

static string_t _create_custom_mount_point( const char * label,uid_t uid,string_t path )
{
	return _create_mount_point_0( label,uid,path,1 ) ;
}

static string_t create_home_mount_point( const char * device,const char * label,uid_t uid )
{
	/*
	 * zuluCryptGetUserHomePath() is defined in ../lib/user_home_path.c
	 */
	string_t path = zuluCryptGetUserHomePath( uid ) ;

	if( label == NULL ){
		return _create_home_default_mount_point( device,uid,path ) ;
	}else{
		return _create_home_custom_mount_point( label,uid,path ) ;
	}
}

static int mount_point_prefix_match_0( const char * m_path,uid_t uid,string_t * m_point,int home_prefix )
{
	int st ;
	/*
	 * zuluCryptGetUserName() is defined in ../lib/user_home_path.c
	 */
	string_t uname ;
	/*
	 * below constant are set in ../constants.h
	 */
	const char * str ;

	if( home_prefix ){

		uname = zuluCryptGetUserHomePath( uid ) ;
		str = StringContent( uname ) ;
	}else{
		uname = zuluCryptGetUserName( uid ) ;
		StringPrepend( uname,"/run/media/private/" ) ;
		str = StringAppendChar( uname,'/' ) ;
	}

	st = StringPrefixEqual( m_path,str ) ;

	if( m_point ){
		*m_point = uname ;
	}else{
		StringDelete( &uname ) ;
	}

	return st ;
}

static int home_mount_point_prefix_match( const char * m_path,uid_t uid,string_t * m_point )
{
	return mount_point_prefix_match_0( m_path,uid,m_point,1 ) ;
}

static int mount_point_prefix_match( const char * m_path,uid_t uid,string_t * m_point )
{
	return mount_point_prefix_match_0( m_path,uid,m_point,0 ) ;
}

void zuluCryptCreateMountPath( const char * path )
{
	struct stat st ;
	stat( path,&st ) ;

	_mkdir( path,S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH | S_IROTH ) ;
	_chown( path,0,0 ) ;
	_chmod( path,st.st_mode | S_IXOTH | S_IROTH ) ;
}

static string_t create_mount_point( const char * device,const char * label,uid_t uid )
{
	string_t path ;

	zuluCryptSecurityGainElevatedPrivileges() ;

	path = zuluCryptGetUserName( uid ) ;

	zuluCryptCreateMountPath( "/run" ) ;
	zuluCryptCreateMountPath( "/run/media" ) ;
	zuluCryptCreateMountPath( "/run/media/private" ) ;
	zuluCryptCreateMountPath( StringPrepend( path,"/run/media/private/" ) ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	StringAppendChar( path,'/' ) ;

	if( label == NULL ){
		return _create_default_mount_point( device,uid,path ) ;
	}else{
		return _create_custom_mount_point( label,uid,path ) ;
	}
}

static int home_mount_prefix( void )
{
	return USE_HOME_PATH_AS_MOUNT_PREFIX ;
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
