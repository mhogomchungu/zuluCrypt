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
#include <stdio.h>
#include <libcryptsetup.h>

static int zuluExit( int st,struct crypt_device * cd )
{
	crypt_free( cd ) ;
	return st ;
}

const char * zuluCryptBitLockerType()
{
	return "crypto_BitLocker" ;
}

const char * zuluCryptBitLockerFolderPrefix( void )
{
	return "cryptoBitlocker" ;
}

string_t zuluCryptBitLockerMapperPath( uid_t uid )
{
	const char * m = zuluCryptBitLockerFolderPrefix() ;

	return String_1( "/run/media/private/",getpwuid( uid )->pw_name,"/",m,NULL ) ;
}

string_t zuluCryptBitLockerMapperName( const char * e )
{
	string_t st ;

	if( zuluCryptNoPartitionLoopDevice( e ) ){

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
	struct crypt_device * cd = NULL ;

	int r ;

	if( StringEndsWith_1( e,"/dislocker-file" ) ){

		return 1 ;
	}else{
#ifdef CRYPT_BITLK
		if( !StringPrefixEqual( e,crypt_get_dir() ) ){

			return 0 ;
		}
		if( crypt_init_by_name( &cd,e ) != 0 ){

			return 0 ;
		}

		r = StringsAreEqual( crypt_get_type( cd ),CRYPT_BITLK ) ;

		crypt_free( cd ) ;

		return r ;
#else
		return 0 ;
#endif
	}
}

char * zuluCryptBitLockerUnmountPath( const char * e )
{
	/*
	 * e will contain something like "/woof/bar/meaw/dislocker-file"
	 * and we want to remove the "/dislocker-file" part.
	 */
	char * m = StringCopy_2( e ) ;

	ssize_t s = StringLastIndexOfChar_1( m,'/' ) ;

	if( s != -1 ){

		*( m + s ) = '\0' ;
	}

	return m ;
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

int zuluCryptBitLockerlock( string_t mapperPath,char ** mount_point )
{
	const char * n = StringContent( mapperPath ) ;

	int s ;

	char * e = NULL ;

	char * m = zuluCryptGetALoopDeviceAssociatedWithAnImageFile( n ) ;

	int r ;

	if( m == NULL ){

		return 6 ;
	}

	r = zuluCryptUnmountVolume( m,&e ) ;

	StringFree( m ) ;

	if( r == 0 ){

		s = zuluCryptCloseMapper( n ) ;

		if( s == 0 ){

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

			fprintf( stderr,"Looking for dislocker-fuse at \"%s\" : PASSED\n",e ) ;

			return e ;
		}else{
			fprintf( stderr,"Looking for dislocker-fuse at \"%s\" : FAILED\n",e ) ;
		}
	}

	return NULL ;
}

int zuluCryptBitLockerUnlock( const open_struct_t * opts,string_t * xt )
{
	int r ;

	const char * exe ;

	string_t st ;

	const char * mapper_path ;

	process_t p ;

	string_t m ;

	char * env[ 2 ] = { NULL,NULL } ;

	struct crypt_device * cd = NULL ;

	uint32_t flags ;

	if( zuluCryptUseCryptsetupBitLocker() ){

#ifdef CRYPT_BITLK
		if( crypt_init( &cd,opts->device ) != 0 ){

			return 4 ;
		}
		if( crypt_load( cd,CRYPT_BITLK,NULL ) != 0 ){

			return zuluExit( 4,cd ) ;
		}
		if( StringHasComponent( opts->m_opts,"ro" ) ){

			flags = CRYPT_ACTIVATE_READONLY ;
		}else{
			flags = CRYPT_ACTIVATE_ALLOW_DISCARDS ;
		}

		r = crypt_activate_by_passphrase( cd,
						  opts->mapper_name,
						  CRYPT_ANY_SLOT,
						  opts->key,
						  opts->key_len,
						  flags ) ;

		if( r == 0 ){

			*xt = String( opts->mapper_path ) ;

			return zuluExit( 0,cd ) ;
		}else{
			return zuluExit( 4,cd ) ;
		}
#else
		return 4 ;
#endif
	}

	exe = _dislocker_fuse_path() ;

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

		p = Process( exe,opts->device,"-u","--","-oallow_root",mapper_path,NULL ) ;

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
