/*
 *
 *  Copyright (c) 2019
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
#include <pwd.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

const char * zuluCryptBitLockerType()
{
	return "crypto_BitLocker" ;
}

string_t zuluCryptBitLockerMapperPath( uid_t uid )
{
	return String_1( "/run/media/private/",getpwuid( uid )->pw_name,"/cryptoBitlocker",NULL ) ;
}

string_t zuluCryptBitLockerMapperName( const char * e )
{
	string_t st ;

	if( StringPrefixEqual( e,"/dev/loop" ) ){

		st = zuluCryptLoopDeviceAddress_2( e ) ;
	}else{
		st = String( e ) ;
	}

	StringReplaceChar( st,'/','_' ) ;

	return st ;
}

string_t zuluCryptBitLockerVolumeFS( const char * e )
{
	int fd = open( e,O_RDONLY ) ;

	int r  ;

	int fd_loop ;

	string_t st = StringVoid ;

	string_t xt = StringVoid ;

	if( fd != -1 ){

		r = zuluCryptAttachLoopDeviceToFileUsingFileDescriptor( fd,&fd_loop,O_RDONLY,&st ) ;

		if( r == 1 ){

			xt = zulucryptGetBlkidFileSystem( StringContent( st ) ) ;
		}

		close( fd ) ;
		close( fd_loop ) ;
	}

	StringDelete( &st ) ;

	return xt ;
}

string_t zuluCryptBitLockerFullMapperPath( uid_t uid,const char * e )
{
	string_t a = zuluCryptBitLockerMapperPath( uid ) ;
	string_t b = zuluCryptBitLockerMapperName( e ) ;

	StringAppendString( a,b ) ;

	StringAppend( a,"/dislocker-file" ) ;

	StringDelete( &b ) ;

	return a ;
}

int zuluCryptBitLockerVolume( const char * e )
{
	return StringEndsWith_1( e,"/dislocker-file" ) ;
}

int zuluCryptBitLockerVolume_1( const char * e )
{
	stringList_t stl = StringListSplit( e,' ' ) ;
	int s = zuluCryptBitLockerVolume( StringListContentAtFirstPlace( stl ) ) ;
	StringListDelete( &stl ) ;
	return s ;
}

string_t zuluCryptBitLockerResolveMapperPath( const char * e,uid_t uid )
{
	string_t q = String( e ) ;
	string_t z = zuluCryptBitLockerMapperPath( uid ) ;

	StringRemoveString( q,StringContent( z ) ) ;
	StringRemoveString( q,"/dislocker-file" ) ;

	StringReplaceChar( q,'_','/' ) ;
	StringDelete( &z ) ;

	return q ;
}

const char * zuluCryptBitLockerCreateMapperPath( string_t e,uid_t uid )
{
	string_t st = zuluCryptBitLockerMapperPath( uid ) ;
	StringReplaceChar( e,'/','_' ) ;
	StringPrependString( e,st ) ;
	StringDelete( &st ) ;
	return StringAppend( e,"/dislocker-file" ) ;
}

int zuluCryptBitLockerlock_1( const char * mapperPath )
{
	string_t st = String( mapperPath ) ;

	int r = zuluCryptBitLockerlock( st,NULL ) ;

	StringDelete( &st ) ;

	return r ;
}

int zuluCryptBitLockerlock( string_t mapperPath,char ** mount_point )
{
	char * e = NULL ;

	int r = zuluCryptUnmountVolume( StringContent( mapperPath ),&e ) ;

	int s ;

	const char * m ;

	if( r == 0 ){

		m = StringRemoveString( mapperPath,"/dislocker-file" ) ;

		s = ProcessExecute( ZULUCRYPTumount,m,NULL ) ; ;

		if( s == 0 ){

			rmdir( m ) ;

			if( mount_point ){

				*mount_point = e ;
			}
		}else{
			StringFree( e ) ;
		}

		return s ;
	}else{
		return r ;
	}
}

static const char * _dislocker_fuse_path()
{
	struct stat st ;
	const char * e ;
	int i ;

	const char * exe[] = { "/usr/bin/dislocker-fuse",
			       "/usr/sbin/dislocker-fuse",
			       "/usr/local/bin/dislocker-fuse",
			       "/usr/local/sbin/dislocker-fuse",
			       "/bin/dislocker-fuse",
			       "/sbin/dislocker-fuse",
			       "/opt/bin/dislocker-fuse",
			       "/opt/sbin/dislocker-fuse",
			       NULL } ;

	for( i = 0 ; *( exe + i ) != NULL ; i++ ){

		e = *( exe + i ) ;

		if( stat( e,&st ) == 0 ){

			printf( "Looking for dislocker-fuse at \"%s\" : PASSED\n",e ) ;

			return e ;
		}else{
			printf( "Looking for dislocker-fuse at \"%s\" : FAILED\n",e ) ;
		}
	}

	return NULL ;
}

int zuluCryptBitLockerUnlock( const open_struct_t * opts,string_t * xt )
{
	int r ;

	const char * exe = _dislocker_fuse_path() ;

	string_t st ;

	const char * mapper_path ;

	process_t p ;

	string_t m ;

	char * env[ 2 ] = { NULL,NULL } ;

	if( exe == NULL ){

		return 15 ;
	}

	st = String_1( opts->mapper_path,opts->mapper_name,NULL ) ;

	mapper_path = StringContent( st ) ;

	if( mkdir( mapper_path,S_IRWXU ) != 0 ){

		StringDelete( &st ) ;
		return 2 ;
	}else{
		m = String_1( "DISLOCKER_PASSWORD=",opts->key,NULL ) ;

		*env = ( char * )StringContent( m ) ;

		p = Process( exe,opts->device,"-u","--",mapper_path,NULL ) ;

		ProcessSetEnvironmentalVariable( p,( char * const * )env ) ;

		ProcessStart( p ) ;

		r = ProcessWaitUntilFinished( &p ) ;

		StringDelete( &m ) ;

		if( r == 0 ){

			StringAppend( st,"/dislocker-file" ) ;
			*xt = st ;
			return 0 ;
		}else{
			rmdir( mapper_path ) ;
			StringDelete( &st ) ;
			return 4 ;
		}
	}
}
