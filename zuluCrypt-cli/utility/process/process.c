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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "process.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <grp.h>

#define _ignore_result( x ) if( x ){;}

struct ProcessType_t{
	pid_t pid ;
	int fd_0[ 2 ] ; /* this variable is used to write to child process      */
	int fd_1[ 2 ] ; /* this variable is used to read from child's std out   */
	int fd_2[ 2 ] ; /* this variable is used to read from child's std error */
	ProcessStatus state ;
	int std_io ;
	char * exe ;
	char ** args ;
	int signal ;
	int wait_status ;
	pthread_t * thread ;
	ProcessStructure str ;
};

static void _f( void )
{
}

static void ( *_memory_error )( void ) = _f ;

void ProcessExitOnMemoryExaustion( void ( *f )( void ) )
{
	_memory_error = f ;
}

static void * _ProcessError( void )
{
	_memory_error() ;
	return NULL ;
}

ProcessStructure * ProcessArgumentStructure( process_t p )
{
	if( p == ProcessVoid ){
		return NULL ;
	}else{
		return &p->str ;
	}
}

void ProcessSetEnvironmentalVariable( process_t p,char * const * env )
{
	if( p != ProcessVoid ){
		p->str.env = env ;
	}
}

static process_t _process( const char * path )
{
	process_t p  ;

	size_t len ;

	if( path == NULL ){
		len = 0 ;
	}else{
		len = strlen( path ) ;
	}

	p = malloc( sizeof( struct ProcessType_t ) ) ;

	if( p == NULL ){
		return _ProcessError() ;
	}

	if( len == 0 ){
		p->exe = malloc( sizeof( char ) ) ;
		if( p->exe == NULL ){
			free( p ) ;
			return _ProcessError() ;
		}
		p->exe[ 0 ] = '\0' ;
	}else{
		p->exe = malloc( sizeof( char ) * ( len + 1 ) ) ;
		if( p->exe == NULL ){
			free( p ) ;
			return _ProcessError() ;
		}
		memcpy( p->exe,path,len + 1 ) ;
	}

	p->std_io = 0       ;
	p->wait_status = -1 ;
	p->thread = NULL    ;
	p->fd_0[ 0 ] = -1   ;
	p->args     = NULL  ;
	p->str.args = NULL  ;
	p->str.timeout = -1 ;
	p->str.env = NULL   ;
	p->str.user_id = -1 ;
	p->str.priority = 0 ;
	p->str.signal = SIGTERM ;
	p->state = ProcessHasNotStarted ;
	return p ;
}

void ProcessSetArgumentList( process_t p,... )
{
	char * entry ;
	char ** args  ;
	char ** e ;
	size_t size = sizeof( char * ) ;
	int index = 1 ;
	va_list list ;

	if( p == ProcessVoid ){
		return ;
	}

	args = p->args ;

	va_start( list,p ) ;

	while( 1 ){

		entry = va_arg( list,char * ) ;
		e = realloc( args,( 1 + index ) * size ) ;

		if( e == NULL ){
			free( args ) ;
			va_end( list ) ;
			_ProcessError() ;
			return ;
		}else{
			args = e ;
		}

		if( entry == NULL ){
			*( args + index ) = NULL ;
			break ;
		}else{
			*( args + index ) = entry ;
			index++ ;
		}
	}

	va_end( list ) ;

	p->args      = args ;
	p->args[ 0 ] = p->exe ;
	p->str.args  = args ;
}

process_t Process( const char * path,... )
{
	char * entry ;
	char ** args  ;
	char ** e ;
	size_t size = sizeof( char * ) ;
	int index = 1 ;
	va_list list ;
	process_t p ;

	if( path == NULL ){
		return _process( NULL ) ;
	}

	p = _process( path ) ;

	if( p == ProcessVoid ){
		return ProcessVoid ;
	}

	args = p->args ;

	va_start( list,path ) ;

	while( 1 ){

		entry = va_arg( list,char * ) ;
		e = realloc( args,( 1 + index ) * size ) ;

		if( e == NULL ){
			ProcessCleanUp( &p ) ;
			free( args ) ;
			va_end( list ) ;
			_ProcessError() ;
			return ProcessVoid ;
		}else{
			args = e ;
		}

		if( entry == NULL ){
			*( args + index ) = NULL ;
			break ;
		}else{
			*( args + index ) = entry ;
			index++ ;
		}
	}

	va_end( list ) ;

	p->args      = args ;
	p->args[ 0 ] = p->exe ;
	p->str.args  = args ;

	return p ;
}

int ProcessExecute( const char * exe,... )
{
	char * entry ;
	char ** args  ;
	char ** e ;
	size_t size = sizeof( char * ) ;
	int index = 1 ;
	va_list list ;
	process_t p ;

	if( exe == NULL ){
		return -1 ;
	}

	p = _process( exe ) ;

	if( p == ProcessVoid ){
		return -1 ;
	}

	args = p->args ;

	va_start( list,exe ) ;

	while( 1 ){

		entry = va_arg( list,char * ) ;
		e = realloc( args,( 1 + index ) * size ) ;

		if( e == NULL ){
			ProcessCleanUp( &p ) ;
			free( args ) ;
			va_end( list ) ;
			_ProcessError() ;
			return -1 ;
		}else{
			args = e ;
		}

		if( entry == NULL ){
			*( args + index ) = NULL ;
			break ;
		}else{
			*( args + index ) = entry ;
			index++ ;
		}
	}

	va_end( list ) ;

	p->args      = args ;
	p->args[ 0 ] = p->exe ;
	p->str.args  = args ;

	ProcessStart( p ) ;

	return ProcessWaitUntilFinished( &p ) ;
}

static void * __timer( void * x )
{
	process_t p = x ;

	sleep( p->str.timeout ) ;

	kill( p->pid,p->str.signal ) ;

	p->state = ProcessCancelled ;

	return NULL ;
}

static void __ProcessStartTimer( process_t p )
{
	p->thread = malloc( sizeof( pthread_t ) ) ;

	if( p->thread == NULL ){
		_ProcessError()  ;
	}else{
		pthread_create( p->thread,NULL,__timer,p ) ;
	}
}

void ProcessSetOptionPriority( process_t p,int priority )
{
	if( p != ProcessVoid ){
		p->str.priority = priority ;
	}
}

pid_t ProcessStart( process_t p )
{
	const char * exe ;

	if( pipe( p->fd_0 ) == -1 ){
		return -1 ;
	}
	if( pipe( p->fd_1 ) == -1 ){
		return -1 ;
	}
	if( pipe( p->fd_2 ) == -1 ){
		return -1 ;
	}

	p->pid = fork() ;

	if( p->pid == -1 ){
		return -1 ;
	}
	if( p->pid == 0 ){
		if( p->str.user_id != ( uid_t )-1 ){
			/*
			 * drop privileges permanently
			 */
			_ignore_result( seteuid( 0 ) ) ;
			_ignore_result( setgid( p->str.user_id ) ) ;
			_ignore_result( setgroups( 1,&p->str.user_id ) ) ;
			_ignore_result( setegid( p->str.user_id ) ) ;
			_ignore_result( setuid( p->str.user_id ) ) ;
		}

		dup2( p->fd_0[ 0 ],0 ) ;
		dup2( p->fd_1[ 1 ],1 ) ;
		dup2( p->fd_2[ 1 ],2 ) ;

		close( p->fd_1[ 0 ] ) ;
		close( p->fd_0[ 1 ] ) ;
		close( p->fd_2[ 0 ] ) ;

		if( p->str.priority != 0 ){
			setpriority( PRIO_PROCESS,0,p->str.priority ) ;
		}

		exe = p->str.args[ 0 ] ;

		if( p->str.env != NULL ){
			execve( exe,p->str.args,p->str.env ) ;
		}else{
			execv( exe,p->str.args ) ;
		}

		/*
		 * execv has failed :-(
		 */
		_Exit( 1 ) ;
		/*
		 * child process block ends here
		 */
	}

	/*
	 * parent process continues from here
	 */
	close( p->fd_0[ 0 ] ) ;
	close( p->fd_1[ 1 ] ) ;
	close( p->fd_2[ 1 ] ) ;

	p->state = ProcessIsStillRunning ;

	if( p->str.timeout != -1 ){
		__ProcessStartTimer( p ) ;
	}

	return p->pid ;
}

#define FACTOR 2
static inline char * __bufferExpandMemory( char * buffer,size_t new_size,size_t * buffer_size )
{
	char * e ;
	if( new_size >= *buffer_size ) {
		*buffer_size = new_size * FACTOR ;
		e = realloc( buffer,*buffer_size ) ;
		if( e == NULL ){
			free( buffer )  ;
			_ProcessError() ;
			return NULL     ;
		}else{
			return e ;
		}
	}else{
		return buffer ;
	}
}

size_t ProcessGetOutPut( process_t p,char ** data,ProcessIO std_io )
{
	#define SIZE 64
	#define BUFFER_SIZE 128
	char * buffer ;
	char * e ;
	char buff[ SIZE ] ;
	size_t size = 0 ;
	size_t count ;
	size_t buffer_size = BUFFER_SIZE ;

	int fd ;

	if( p == ProcessVoid ){
		return 0 ;
	}

	buffer = malloc( sizeof( char ) * BUFFER_SIZE ) ;

	if( buffer == NULL ){
		_ProcessError() ;
		return 0 ;
	}

	switch( std_io ){
		case ProcessStdOut   : fd = p->fd_1[ 0 ] ; break ;
		case ProcessStdError : fd = p->fd_2[ 0 ] ; break ;
		default  : return 0 ;
	}

	while( 1 ) {
		count = read( fd,buff,SIZE ) ;
		buffer = __bufferExpandMemory( buffer,size + count,&buffer_size ) ;

		if( buffer == NULL ){
			_ProcessError() ;
			return 0 ;
		}else{
			memcpy( buffer + size,buff,count ) ;
		}

		size += count ;

		if( count < SIZE ){
			break ;
		}
	}

	if( size > 0 ){
		e = realloc( buffer,size + 1 ) ;
		if( e == NULL ){
			free( buffer ) ;
			_ProcessError() ;
			return 0 ;
		}else{
			*( e + size ) = '\0' ;
			*data = e ;
		}
	}

	return size ;
}

ProcessStatus ProcessState( process_t p )
{
	if( p != ProcessVoid ){
		return p->state ;
	}else{
		return ProcessStatusUndefined ;
	}
}

ssize_t ProcessGetOutPut_1( process_t p,char * buffer,int size,ProcessIO std_io )
{
	if( p != ProcessVoid ){
		switch( std_io ){
			case ProcessStdOut   : return read( p->fd_1[ 1 ],buffer,size ) ;
			case ProcessStdError : return read( p->fd_2[ 1 ],buffer,size ) ;
			default              : return -1 ;
		}
	}else{
		return -1 ;
	}
}

size_t ProcessWrite( process_t p,const char * data,size_t len )
{
	if( p != ProcessVoid ){
		return write( p->fd_0[ 1 ],data,len ) ;
	}else{
		return 0 ;
	}
}

void ProcessCloseStdWrite( process_t p )
{
	close( p->fd_0[ 0 ] ) ;
	p->fd_0[ 0 ] = -1 ;
}

void ProcessSetOptionTimeout( process_t p,int timeout,int signal )
{
	if( p != ProcessVoid ){
		p->str.signal  = signal ;
		p->str.timeout = timeout ;
	}
}

static void _ProcessDelete( process_t px )
{
	if( px->thread != NULL ){
		pthread_cancel( *( px->thread ) ) ;
		free( px->thread ) ;
	}

	close( px->fd_2[ 1 ] ) ;
	close( px->fd_1[ 1 ] ) ;

	if( px->fd_0[ 0 ] != -1 ){
		close( px->fd_0[ 0 ] ) ;
	}
	if( px->wait_status == -1 ){
		waitpid( px->pid,0,WNOHANG ) ;
	}

	free( px->args ) ;

	free( px->exe ) ;

	free( px ) ;
}

void ProcessCleanUp( process_t * p )
{
	if( p != NULL && *p != ProcessVoid ){
		_ProcessDelete( *p ) ;
		*p = ProcessVoid ;
	}
}

int ProcessTerminate( process_t p )
{
	int st ;

	if( p == ProcessVoid ){
		return -1 ;
	}else{
		p->state = ProcessCancelled ;
		st = kill( p->pid,SIGTERM ) ;
		waitpid( p->pid,0,WNOHANG ) ;
		p->wait_status = 1 ;
		return st ;
	}
}

void ProcessSetOptionUser( process_t p,uid_t uid )
{
	if( p != ProcessVoid ){
		p->str.user_id = uid ;
	}
}

int ProcessKill( process_t p )
{
	int st ;
	if( p == ProcessVoid ){
		return -1 ;
	}else{
		p->state = ProcessCancelled ;
		st = kill( p->pid,SIGKILL ) ;
		waitpid( p->pid,0,WNOHANG ) ;
		p->wait_status = 1 ;
		return st ;
	}
}

int ProcessWaitUntilFinished( process_t * e )
{
	int s ;

	process_t p ;

	if( e == NULL || *e == ProcessVoid ){
		return -1 ;
	}else{
		p = *e ;
		*e = ProcessVoid ;

		s = ProcessExitStatus( p ) ;

		_ProcessDelete( p ) ;

		return s ;
	}
}

int ProcessExitStatus( process_t p )
{
	int s ;

	if( p == ProcessVoid ){
		return -1 ;
	}else{
		waitpid( p->pid,&s,0 ) ;

		p->state = ProcessCompleted ;
		p->wait_status = 1 ;

		if( WIFEXITED( s ) == 0 ){
			return -1 ;
		}else{
			return WEXITSTATUS( s ) ;
		}
	}
}

void ProcessWait( process_t p )
{
	ProcessExitStatus( p ) ;
}

void ProcessSetArguments( process_t p,char * const s[] )
{
	if( p != ProcessVoid ){
		p->str.args = s ;
	}
}
