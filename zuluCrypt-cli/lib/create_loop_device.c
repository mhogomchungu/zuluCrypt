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

static int zuluExit( int result,string_t st,int fd_loop,int fd_path )
{
	if( st == 0 ){
		StringDelete( &st ) ;
		if( fd_loop != -1 )
			close( fd_loop ) ;
	}
	if( fd_path != -1 ){
		close( fd_path ) ;
	}
	return result ;
}

char * zuluCryptLoopDeviceAddress( const char * device )
{
	int fd ;
	char * path ;
	struct loop_info64 l_info ;
	string_t xt ;
	string_t st = String( "/sys/block/" ) ;
	StringMultipleAppend( st,device + 5,"/loop/backing_file",END ) ;
	xt = StringGetFromVirtualFile( StringContent( st ) ) ;
	StringDelete( &st ) ;
	if( xt == StringVoid ){
		memset( &l_info,'\0',sizeof( struct loop_info64 ) ) ;
		fd = open( device,O_RDONLY ) ;
		ioctl( fd,LOOP_GET_STATUS64,&l_info ) ;
		path = zuluCryptRealPath( ( char * ) l_info.lo_file_name ) ;
		close( fd ) ;
		return path ;
	}else{
		StringRemoveRight( xt,1 ) ;
		return StringDeleteHandle( &xt ) ;
	}
}

char * zuluCryptGetFileNameFromFileDescriptor( int fd )
{
	char * c ;
	string_t xt ;
	struct stat st ;
	
	if( fstat( fd,&st ) != 0 )
		return NULL ;
	if( st.st_nlink == 0 || st.st_nlink > 1 )
		return NULL ;
	
	xt = String( "/proc/self/fd/" ) ;
	StringAppendInt( xt,fd ) ;
	c = zuluCryptRealPath( StringContent( xt ) ) ;
	StringDelete( &xt ) ;
	return c ;
}

/*
 * Here,we check if the path we sent to open() is the path open() used. This check is necessary to 
 * guard against some known hypothetical exploits
 * 
 */
static int _paths_are_not_sane( int fd,const char * path )
{
	struct stat p ;
	struct stat q = global_variable_file_struct ;
	if( path ){;}
	if( fstat( fd,&p ) != 0 )
		return 1 ;
	if( !S_ISREG( p.st_mode ) )
		return 1 ;
	if( ( p.st_dev == q.st_dev ) && ( p.st_ino == q.st_ino ) ){
		return 0 ;
	}else{
		return 1 ;
	}
}

int zuluCryptAttachLoopDeviceToFile( const char * path,int mode,int * loop_fd,string_t * loop_device )
{
	size_t size ;
	string_t loopd = StringVoid ;
	int fd_loop = -1 ;
	int fd_path = -1 ;
	int devnr ;
	const char * loop ;
	struct loop_info64 l_info ;
	
	if( strncmp( path,"/dev/",5 ) == 0 )
		return zuluExit( 0,loopd,fd_loop,fd_path ) ;
	
	memset( &l_info,'\0',sizeof( struct loop_info64 ) ) ;
	
	fd_loop = open( "/dev/loop-control",O_RDONLY ) ;

	if( fd_loop == -1 )
		return 0 ;
	
	devnr = ioctl( fd_loop,LOOP_CTL_GET_FREE );
	
	close( fd_loop ) ;
	
	if( devnr < 0 )
		return 0 ;
	
	loopd = String( "/dev/loop" ) ;
	loop = StringAppendInt( loopd,devnr ) ;
	
	fd_path = open( path,mode ) ;
	
	if( fd_path == -1 )
		return zuluExit( 0,loopd,fd_loop,fd_path ) ;
	
	if( _paths_are_not_sane( fd_path,path ) )
		return zuluExit( 0,loopd,fd_loop,fd_path ) ;
			
	fd_loop = open( loop,mode ) ;
	
	if( fd_loop == -1 )
		return zuluExit( 0,loopd,fd_loop,fd_path ) ;
	
	if( ioctl( fd_loop,LOOP_SET_FD,fd_path ) == -1 )
		return zuluExit( 0,loopd,fd_loop,fd_path ) ;
	
	if( ioctl( fd_loop,LOOP_GET_STATUS64,&l_info ) == -1 )
		return zuluExit( 0,loopd,fd_loop,fd_path ) ;
	
	l_info.lo_flags |= LO_FLAGS_AUTOCLEAR;
	
	size = sizeof( l_info.lo_file_name ) ;
	strncpy( ( char * )l_info.lo_file_name,path,size ) ;
	l_info.lo_file_name[ size - 1 ] = '\0' ;
	
	if( ioctl( fd_loop,LOOP_SET_STATUS64,&l_info ) == -1 )
		return zuluExit( 0,loopd,fd_loop,fd_path ) ;
	
	*loop_device = loopd ;
	*loop_fd = fd_loop ;
	
	return zuluExit( 1,loopd,fd_loop,fd_path ) ;
}
