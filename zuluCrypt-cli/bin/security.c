/*
 *
 *  Copyright (c) 2012
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

#define ZULUDEBUG 0

static int create_group( const char * groupname ) __attribute__((unused)) ;

static int create_group( const char * groupname )
{
	process_t p ;
	int st = 1 ;
	zuluCryptSecurityGainElevatedPrivileges() ;
	p = Process( ZULUCRYPTgroupadd ) ;
	ProcessSetArgumentList( p,"-f",groupname,NULL ) ;
	ProcessStart( p ) ;
	st = ProcessWaitUntilFinished( &p ) ;
	zuluCryptSecurityDropElevatedPrivileges();
	return st == 0 ;
}

static int _polkitAuthenticated( void )
{
	return 0 ;
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
		st = 0 ;
	}else if( uid == 0 ){
		st = 1 ;
	}else{
		zuluCryptSecurityGainElevatedPrivileges() ;

		pass = getpwuid( uid ) ;

		if( pass == NULL ){
			st = 0 ;
		}else{
			grp = getgrnam( groupname ) ;

			if( grp == NULL ){
				/*
				* 	dont autocreate groups
				*	create_group( groupname )  ;
				*/
				st = 0 ;
			}else{
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
			}
		}
		zuluCryptSecurityDropElevatedPrivileges();
	}

	if( st == 0 ){
		return _polkitAuthenticated() ;
	}else{
		return st ;
	}
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
void zuluCryptSetUserUIDForPrivilegeManagement( uid_t uid )
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

void zuluCryptSecuritySanitizeTheEnvironment( uid_t uid,stringList_t * stx )
{
	extern char ** environ ;
	const char ** env = ( const char ** ) environ ;
	ssize_t index ;

	stringList_t stl = StringListVoid ;

	string_t st ;

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

	StringListGetIterators( stl,&it,&end ) ;

	while( it != end ){
		st = *it ;
		it++ ;
		index = StringIndexOfChar( st,0,'=' ) ;
		if( index >= 0 ){
			unsetenv( StringSubChar( st,index,'\0' ) ) ;
			StringSubChar( st,index,'=' ) ;
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
