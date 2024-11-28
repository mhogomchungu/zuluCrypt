/*
 *
 *  Copyright (c) 2012-2015
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
#include <errno.h>
#include <unistd.h>
#include <grp.h>
#include "../constants.h"
#include <sys/types.h>
#include <pwd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/*
 * This source file makes sure the user who started the tool( usually non root user ) has permission
 * to perform operations they want on paths they presented.
 *
 * This feature allows tradition unix permissions to be set on a paths to control non user access to volumes
 */

/*
 * zuluCryptUserIsAMemberOfAGroup() was moved to ../lib/mount_fs_options.c
 */

#define ZULUDEBUG 0

static uid_t _original_UID ;
static const char * _run_time_path ;

/*
 * set the function to be called when an attempt to evelate or downgrade privileges fail.
 */
static void ( *zuluCryptSecurityPrivilegeElevationError )( const char * ) = NULL ;

const char * zuluCryptRunTimePath( void )
{
	return _run_time_path ;
}

void zuluCryptExeSetOriginalUID( uid_t s )
{
	_original_UID = s ;
}

int zuluCryptExeOriginalUserIsNotRoot( void )
{
	return _original_UID != 0 ;
}

int zuluCryptSecurityGainElevatedPrivileges( void )
{
	/*
	 * printf( "GAINING:uid=%d:euid=%d\n",getuid(),geteuid() ) ;
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

int zuluCryptSecurityConvertUID( uid_t uid,const char * u_id,uid_t * user_id )
{
	extern char ** environ ;

	const char * sudo_uid  = NULL ;

	char ** e = environ ;

	const char * s ;

	*user_id = uid ;

	for( ; *e != NULL ; e++ ){

		s = *e ;

		if( StringPrefixMatch( s,"SUDO_UID=",9 ) ){

			sudo_uid = s + 9 ;
			break ;
		}
	}

	if( u_id != NULL ){

		if( uid == 0 ){

			*user_id = (uid_t)StringConvertToInt( u_id ) ;

			return 0 ;
		}else{
			return 1 ;
		}

	}else if( sudo_uid != NULL ){

		if( uid == 0 ){

			*user_id = (uid_t)StringConvertToInt( sudo_uid ) ;

			return 0 ;
		}else{
			return 1 ;
		}
	}else{
		return 0 ;
	}
}

uid_t global_variable_user_uid ;
void zuluCryptSetUserUIDForPrivilegeManagement( uid_t uid )
{
	global_variable_user_uid = uid ;
}

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

void zuluCryptSecuritySanitizeTheEnvironment( uid_t uid,stringList_t * stx )
{
	extern char ** environ ;
	const char ** env = ( const char ** ) environ ;
	ssize_t index ;

	string_t xt ;

	stringList_t stl = StringListVoid ;

	string_t st ;

	StringListIterator  it ;
	StringListIterator end ;

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

	StringListGetIterators( stl,&it,&end ) ;

	while( it != end ){

		st = *it ;

		it++ ;

		index = StringIndexOfChar( st,0,'=' ) ;

		if( index >= 0 ){

			unsetenv( StringSubChar( st,(size_t)index,'\0' ) ) ;
			StringSubChar( st,(size_t)index,'=' ) ;
		}
	}

	xt = String( "/tmp/zuluCrypt-" ) ;

	StringAppendInt( xt,uid ) ;

	StringListAppendString_1( &stl,&xt ) ;

	_run_time_path = StringListContentAtLast( stl ) ;

	*stx = stl ;
}

int zuluCryptSecurityUserOwnTheFile( const char * device,uid_t uid )
{
	if( device && uid ){}
	return 0 ;
}

void zuluCryptSecurityLockMemory_1( string_t st )
{
	if( st != StringVoid ){

		mlock( StringContent( st ),StringLength( st ) ) ;
	}
}

void zuluCryptSecurityUnlockMemory_1( string_t st )
{
	void * e ;
	size_t f ;

	if( st != StringVoid ){

		e = ( void * )StringContent( st ) ;

		f = StringLength( st ) ;

		memset( e,'\0',f ) ;

		munlock( e,f ) ;
	}
}

void zuluCryptSecurityLockMemory( stringList_t stl )
{
	StringListIterator it   ;
	StringListIterator end  ;

	string_t st ;

	StringListGetIterators( stl,&it,&end ) ;

	while( it != end ){

		st = *it ;

		it++ ;

		mlock( StringContent( st ),StringLength( st ) ) ;
	}
}

void zuluCryptSecurityUnlockMemory( stringList_t stl )
{
	StringListIterator it  ;
	StringListIterator end ;

	string_t st ;

	void * e ;
	size_t f ;

	StringListGetIterators( stl,&it,&end ) ;

	while( it != end ){

		st = *it ;

		it++ ;

		if( st != StringVoid ){

			e = ( void * )StringContent( st ) ;

			f = StringLength( st ) ;

			memset( e,'\0',f ) ;

			munlock( e,f ) ;
		}
	}
}

void zuluCryptSecurityPrintPermissions( void )
{
	puts( "----------------------" ) ;
	printf( "uid:%d\neuid:%d\n",(int)getuid(),(int)geteuid() ) ;
	puts( "----------------------" ) ;
}
