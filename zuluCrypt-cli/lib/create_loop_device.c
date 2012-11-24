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

int zuluCryptAttachLoopDeviceToFile( const char * path,int mode,int * loop_fd,string_t * loop_device )
{
	size_t size ;
	string_t loopd ;
	int fd_loop;
	int fd_path ;
	int devnr ;
	const char * loop ;
	
	struct loop_info64 l_info ;
	
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

	if( mode == READ ){
		fd_path = open( path,O_RDONLY ) ;
		fd_loop = open( loop,O_RDONLY ) ;
	}else{
		fd_path = open( path,O_RDWR ) ;
		fd_loop = open( loop,O_RDWR ) ;
	}
	
	if( fd_path < 0 && fd_loop < 0 ){
		StringDelete( &loopd ) ;
		return 0 ;
	}else if( fd_path < 0 && fd_loop >= 0 ){
		StringDelete( &loopd ) ;
		close( fd_loop ) ;
		return 0 ;
	}else if( fd_path >= 0 && fd_loop < 0 ){
		StringDelete( &loopd ) ;
		close( fd_path ) ;
		return 0 ;
	}
		
	if( ioctl( fd_loop,LOOP_SET_FD,fd_path ) == -1 ){
		StringDelete( &loopd ) ;
		close( fd_loop ) ;
		close( fd_path ) ;
		return 0 ;
	}
	
	if( ioctl( fd_loop,LOOP_GET_STATUS64,&l_info ) == -1 ){
		StringDelete( &loopd ) ;
		close( fd_loop ) ;
		close( fd_path ) ;
		return 0 ;
	}
	
	l_info.lo_flags |= LO_FLAGS_AUTOCLEAR;
	
	size = sizeof( l_info.lo_file_name ) ;
	strncpy( ( char * )l_info.lo_file_name,path,size ) ;
	l_info.lo_file_name[ size - 1 ] = '\0' ;
	
	if( ioctl( fd_loop,LOOP_SET_STATUS64,&l_info ) == -1 ){
		StringDelete( &loopd ) ;
		close( fd_loop ) ;
		close( fd_path ) ;
		return 0 ;
	}
	
	close( fd_path ) ;
	*loop_device = loopd ;
	*loop_fd = fd_loop ;
	return 1 ;
}
