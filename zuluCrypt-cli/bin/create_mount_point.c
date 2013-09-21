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

#if USE_HOME_PATH_AS_MOUNT_PREFIX

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
	
	if( mkdir( m_point,S_IRWXU ) == 0 ){
		st = path ;
		chown( m_point,uid,uid ) ;
	}else{
		StringDelete( &path ) ;
	}
	
	StringFree( loop_path ) ;
	
	return st ;
}

static string_t _create_custom_mount_point( const char * label,uid_t uid,string_t path )
{
	string_t st = StringVoid ;
	const char * p = StringAppend( path,"/" ) ;
	const char * q = strrchr( label,'/' ) ;
	
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
	
	return st ;
}

string_t zuluCryptCreateMountPoint( const char * device,const char * label,uid_t uid )
{
	string_t path = zuluCryptGetUserName( uid ) ;
	StringPrepend( path,"/home/" ) ;
	
	if( label == NULL ){
		return _create_default_mount_point( device,uid,path ) ;
	}else{
		return _create_custom_mount_point( label,uid,path ) ;
	}
}

int zuluCryptMountPointPrefixMatch( const char * m_path,uid_t uid,string_t * m_point )
{
	int st ;
	/*
	 * zuluCryptGetUserName() is defined in ../lib/user_home_path.c
	 */
	string_t uname = zuluCryptGetUserName( uid ) ;
	/*
	 * below constant are set in ../constants.h
	 */
	const char * str = StringPrepend( uname,"/home/" ) ;
	st = StringPrefixEqual( m_path,str ) ;
	if( m_point ){
		*m_point = uname ;
	}else{
		StringDelete( &uname ) ;
	}
	return st ;
}

#else

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
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	
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
	
	return st ;
}

int zuluCryptMountPointPrefixMatch( const char * m_path,uid_t uid,string_t * m_point )
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

string_t zuluCryptCreateMountPoint( const char * device,const char * label,uid_t uid )
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
#endif
