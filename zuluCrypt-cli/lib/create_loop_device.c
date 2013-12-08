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
#include <sys/ioctl.h>
#include <linux/loop.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int zuluExit( int result,string_t st,int fd_loop,int fd_path )
{
	if( st == 0 ){
		StringDelete( &st ) ;
		if( fd_loop != -1 ){
			close( fd_loop ) ;
		}
	}
	if( fd_path != -1 ){
		close( fd_path ) ;
	}
	return result ;
}

string_t zuluCryptLoopDeviceAddress_2( const char * device )
{
	int fd ;
	char * path ;
	struct loop_info64 l_info ;
	string_t st = String( "" ) ;
	const char * e = StringMultipleAppend( st,"/sys/block/",device + 5,"/loop/backing_file",END ) ;
	string_t xt = StringGetFromVirtualFile( e ) ;
	StringDelete( &st ) ;
	if( xt == StringVoid ){
		memset( &l_info,'\0',sizeof( struct loop_info64 ) ) ;
		fd = open( device,O_RDONLY ) ;
		ioctl( fd,LOOP_GET_STATUS64,&l_info ) ;
		path = zuluCryptRealPath( ( char * ) l_info.lo_file_name ) ;
		close( fd ) ;
		xt = StringInherit( &path ) ;
	}else{
		StringRemoveRight( xt,1 ) ;
	}
	StringReplaceString( xt," ","\\040" ) ;
	return xt ;
}

char * zuluCryptLoopDeviceAddress( const char * device )
{
	string_t st = zuluCryptLoopDeviceAddress_2( device ) ;
	return StringDeleteHandle( &st ) ;
}

char * zuluCryptLoopDeviceAddress_1( const char * device )
{
	int fd ;
	char * path ;
	struct loop_info64 l_info ;
	string_t st = String( "/sys/block/" ) ;
	string_t xt = StringGetFromVirtualFile( StringMultipleAppend( st,device + 5,"/loop/backing_file",END ) ) ;
	StringDelete( &st ) ;
	if( xt == StringVoid ){
		memset( &l_info,'\0',sizeof( struct loop_info64 ) ) ;
		fd = open( device,O_RDONLY ) ;
		ioctl( fd,LOOP_GET_STATUS64,&l_info ) ;
		path = zuluCryptRealPath( ( char * ) l_info.lo_file_name ) ;
		close( fd ) ;
		return path;
	}else{
		StringRemoveRight( xt,1 ) ;
		return StringDeleteHandle( &xt ) ;
	}
}

char * zuluCryptGetLoopDeviceAddress( const char * device )
{
	char * z = NULL ;
	const char * e ;

	string_t st = StringVoid ;
	string_t xt = StringVoid ;

	int i ;
	int r ;

	z = zuluCryptLoopDeviceAddress_1( device ) ;

	if( z == NULL ){
		return NULL ;
	}else{
		st = String( "" ) ;
		for( i = 0 ; i < 255 ; i++ ){
			StringAppendAt( st,0,"/sys/block/loop" ) ;
			StringAppendInt( st,i ) ;
			e = StringAppend( st,"/loop/backing_file" ) ;
			xt = StringGetFromVirtualFile( e ) ;
			e = StringRemoveRight( xt,1 ) ;
			r = StringsAreEqual( e,z ) ;
			StringDelete( &xt ) ;
			if( r ){
				StringAppendAt( st,0,"/dev/loop" ) ;
				e = StringAppendInt( st,i ) ;
				if( StringsAreEqual( device,e ) ){
					;
				}else{
					break ;
				}
			}else{
				StringAppendAt( st,0,"" ) ;
			}
		}

		StringFree( z ) ;

		if( StringEqual( st,"" ) ){
			StringDelete( &st ) ;
			return NULL ;
		}else{
			return StringDeleteHandle( &st ) ;
		}
	}
}

char * zuluCryptGetFileNameFromFileDescriptor( int fd )
{
	char * c = NULL ;
	char * d ;
	string_t xt = String( "/proc/self/fd/" ) ;
	c = zuluCryptRealPath( StringAppendInt( xt,fd ) ) ;
	StringDelete( &xt ) ;
	if( StringPrefixMatch( c,"/dev/mapper/",12 ) ){
		/*
		 * zuluCryptConvertIfPathIsLVM() is defined in status.c
		 */
		xt = zuluCryptConvertIfPathIsLVM( c ) ;
		free( c ) ;
		c = StringDeleteHandle( &xt ) ;
	}else if( StringPrefixMatch( c,"/dev/md",7 ) ){
		/*
		 * zuluCryptResolveMDPath() is defined in process_mountinfo.c
		 */
		d = zuluCryptResolveMDPath( c ) ;
		free( c ) ;
		c = d ;
	}
	return c ;
}

/*
 * Here,we check if the path we sent to open() is the path open() used. This check is necessary to
 * guard against some known hypothetical exploits
 */
static int _paths_are_not_sane( int fd,const char * path )
{
	char * c = zuluCryptGetFileNameFromFileDescriptor( fd ) ;
	int st ;
	if( c != NULL ){
		st = strcmp( c,path ) ;
		free( c ) ;
		return st != 0 ;
	}else{
		return 1 ;
	}
}

static int open_loop_device_1( string_t * loop_device )
{
	string_t st = String( "" ) ;
	int i ;
	int fd ;
	const char * path ;
	struct loop_info64 l_info ;
	int r = 0 ;
	for( i = 0 ; i < 255 ; i++ ){
		StringAppendAt( st,0,"/dev/loop" ) ;
		path = StringAppendInt( st,i ) ;
		fd = open( path,O_RDONLY ) ;
		if( fd == -1 ){
			r = 0 ;
			break ;
		}
		if( ioctl( fd,LOOP_GET_STATUS64,&l_info ) != 0 ){
			if( errno == ENXIO) {
				*loop_device = st ;
				close( fd ) ;
				r = 1 ;
				break ;
			}
		}
		close( fd ) ;
	}
	StringDelete( &st ) ;
	return r ;
}

static int open_loop_device( string_t * loop_device )
{
	int devnr ;
	int fd_loop ;

	fd_loop = open( "/dev/loop-control",O_RDONLY ) ;

	if( fd_loop == -1 ){
		return open_loop_device_1( loop_device ) ;
	}else{
		devnr = ioctl( fd_loop,LOOP_CTL_GET_FREE ) ;
		close( fd_loop ) ;
		if( devnr < 0 ){
			return open_loop_device_1( loop_device ) ;
		}else{
			*loop_device = String( "/dev/loop" ) ;
			StringAppendInt( *loop_device,devnr ) ;
			return 1 ;
		}
	}
}

static int attach_device_to_loop( int fd_path,int * fd_loop,string_t loop_device,int mode )
{
	char * path ;
	size_t size ;

	struct loop_info64 l_info ;

	*fd_loop = open( StringContent( loop_device ),mode ) ;

	memset( &l_info,'\0',sizeof( struct loop_info64 ) ) ;

	if( *fd_loop == -1 ){
		return 0 ;
	}
	if( ioctl( *fd_loop,LOOP_SET_FD,fd_path ) == -1 ){
		return 0 ;
	}
	if( ioctl( *fd_loop,LOOP_GET_STATUS64,&l_info ) == -1 ){
		return 0;
	}

	l_info.lo_flags |= LO_FLAGS_AUTOCLEAR;

	path = zuluCryptGetFileNameFromFileDescriptor( fd_path ) ;
	if( path == NULL ){
		return 0 ;
	}else{
		size = sizeof( l_info.lo_file_name ) ;
		strncpy( ( char * )l_info.lo_file_name,path,size ) ;
		l_info.lo_file_name[ size - 1 ] = '\0' ;
		free( path ) ;

		if( ioctl( *fd_loop,LOOP_SET_STATUS64,&l_info ) == -1 ){
			return 0 ;
		}else{
			return 1 ;
		}
	}
}

static int _attach_loop_device_to_file( const char * path,int mode,int * loop_fd,string_t * loop_device )
{
	string_t loopd = StringVoid ;

	int fd_loop = -1 ;
	int fd_path = -1 ;

	if( !open_loop_device( &loopd ) ){
		return zuluExit( 0,loopd,fd_loop,fd_path ) ;
	}

	fd_path = open( path,mode ) ;

	if( fd_path == -1 ){
		return zuluExit( 0,loopd,fd_loop,fd_path ) ;
	}

	fcntl( fd_path,F_SETFD,FD_CLOEXEC ) ;

	if( _paths_are_not_sane( fd_path,path ) ){
		return zuluExit( 0,loopd,fd_loop,fd_path ) ;
	}
	if( attach_device_to_loop( fd_path,&fd_loop,loopd,mode ) ){
		*loop_device = loopd ;
		*loop_fd = fd_loop ;
		return zuluExit( 1,loopd,fd_loop,fd_path ) ;
	}else{
		return zuluExit( 0,loopd,fd_loop,fd_path ) ;
	}
}

int zuluCryptAttachLoopDeviceToFile( const char * path,int mode,int * loop_fd,string_t * loop_device )
{
	int i = 0 ;
	int j = 0 ;
	while( 1 ){
		i = _attach_loop_device_to_file( path,mode,loop_fd,loop_device ) ;
		if( i == 0 && j < 3 ){
			sleep( 3 ) ;
			j++ ;
		}else{
			break ;
		}
	}
	return i ;
}

static int _attach_loop_device_to_file_using_file_descriptor( int fd_path,int * fd_loop,int mode,string_t * loop_device )
{
	string_t loopd = StringVoid ;

	if( !open_loop_device( &loopd ) ){
		return 0 ;
	}else{
		if( attach_device_to_loop( fd_path,fd_loop,loopd,mode ) ){
			*loop_device = loopd ;
			return 1 ;
		}else{
			return 0 ;
		}
	}
}

int zuluCryptAttachLoopDeviceToFileUsingFileDescriptor( int fd_path,int * fd_loop,int mode,string_t * loop_device )
{
	int i = 0 ;
	int j = 0 ;
	while( 1 ){
		i = _attach_loop_device_to_file_using_file_descriptor( fd_path,fd_loop,mode,loop_device ) ;
		if( i == 0 && j < 3 ){
			sleep( 3 ) ;
			j++ ;
		}else{
			break ;
		}
	}
	return i ;
}
