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

#ifndef PROCESS
#define PROCESS

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

typedef struct{
	/*
	 * if this variable is set,then it is expect to be in the same format the last argument of execv() expect
	 */
	const char * const * args ;
	/*
	 * this variable is set will cause the program to be executed will run with the identify of this user.
	 */
	uid_t user_id ;
	/*
	 * If this variable is set,then it is expected to be in the format the last argument of execve() expect
	 */
	const char * const * env ;
	/*
	 * If this variable is set,the process will be terminate if its still running after timeout seconds
	 * The default signal sent is sigterm
	 */
	int timeout ;
	/*
	 * if this variable is set,then this signal will be sent when the timeout above expires
	 */
	int signal ;
	/*
	 * priority of forked process will run with
	 */
	int priority ;
}ProcessStructure ;

typedef struct ProcessType_t * process_t ;

#define ProcessVoid ( ( process_t ) 0 )

/*
 * Examples on how to use the library are at the end of this header file
 */

/*
 * Takes a pointer to a function to be called when memory allocation can not take place
 * ie if the system has run out of memory and malloc() or realloc() has failed.
 * This function is optional and "ProcessVoid" will be returned on memory exaustion if the function
 * isnt set.
 */
void ProcessExitOnMemoryExaustion( void (*)( void ) ) ;

/*
 * create a handle to a process that will be started.
 * Path to executable to be started must be in full path format
 */
process_t Process( const char * path ) ;

/*
 * This structure can be used an an alternative way to set various options.
 * There exist a separate function to set each one of them
 */
ProcessStructure * ProcessArgumentStructure( process_t ) ;

/*
 * call fork() and then exec and run a program specified when handle p was created.
 * return value: pid of the child process
 */
pid_t ProcessStart( process_t p ) ;

/*
 * pass data to the child process,the child process will get the data from reading its stdin.
 * ProcessSetOption( p,WRITE_STD_IN ) must be called first for this to work. *
 */
size_t ProcessWrite( process_t p,const char * data,size_t len ) ;

/*
 * close the writing connection to the child process.This maybe necessary if a child process
 * blocks waiting for more data in its std in until EOF is received.
 */
void ProcessCloseStdWrite( process_t p ) ;

/*
 * send a forked process sigterm to terminate it.
 */
int ProcessTerminate( process_t ) ;

/*
 * run the child process with privileges of a user with UID of uid
 */
void ProcessSetOptionUser( process_t,uid_t uid ) ;

/*
 * set the nice value the forked process will run with
 */
void ProcessSetOptionPriority( process_t,int priority ) ;

/*
 * send a forked process sigkill to kill it
 */
int ProcessKill( process_t ) ;

/*
 * the forked process will be started with arguments list given by the second argument
 * look at the example at the end of this header file for info on how to use the API
 */
void ProcessSetArguments( process_t p,const char * const argv[] ) ;

/*
 * look at the example at the end of this header file for info on how to use the API
 */
void ProcessSetArgumentList( process_t p,... ) ;

/*
 * set the child process to start with the given enviromental variables
 */
void ProcessSetEnvironmentalVariable( process_t p,const char * const * env ) ;

/*
 * get state of the process handled by handle p
 */
typedef enum{
	ProcessHasNotStarted,
	ProcessIsStillRunning,
	ProcessCompleted,
	ProcessCancelled,
	ProcessStatusUndefined
}ProcessStatus;

ProcessStatus ProcessState( process_t p ) ;

/*
 * wait for "timeout" seconds and then send the forked process a signal "signal"
 */
void ProcessSetOptionTimeout( process_t p,int timeout,int signal ) ;

/*
 * block waiting for the forked process to exit and then get its exit status.
 */
int ProcessExitStatus( process_t ) ;

/*
 * block waiting for the forked process to exit.
 */
void ProcessWait( process_t ) ;

/*
 * block waiting for the forked process to exit and then get its exit status and then clean up
 * used resources.
 *
 * Use this function if you have to wait for the forked process to finish so that you can
 * get its exit status.
 */
int ProcessWaitUntilFinished( process_t * ) ;

/*
 * clean up used resources and dont wait for the forked process.
 * Use this function if you dont want to wait for the exit status or if the forked
 * process is meant to live past the life of the parent process.
 */
void ProcessCleanUp( process_t * ) ;

/*
 * get contents of std out/std error from the process.
 * remember to free() the return buffer when done with it.
 *
 * return value is the size of the data.
 * this function must be called after ProcessStart()
 */

typedef enum{
	ProcessStdIn,
	ProcessStdOut,
	ProcessStdError
}ProcessIO;

size_t ProcessGetOutPut( process_t,char ** data,ProcessIO ) ;

/*
 * read size number of bytes from the ourput of the forket process.
 */
ssize_t ProcessGetOutPut_1( process_t,char * buffer,int size,ProcessIO ) ;

#ifdef __cplusplus
}
#endif

#endif

/*
 * Example use case of ProcessSetArgumentList API
 */
#if 0

#include "process.h"
#include <stdio.h>
#include <stdlib.h>

int main( void )
{
	process_t p = Process( "/bin/ls" ) ;
	ProcessSetArgumentList( p,"-l",NULL ) ;
	ProcessStart( p ) ;
	char * c = NULL ;
	while( 1 ){
		ProcessGetOutPut( p,&c,ProcessStdOut ) ;
		if( c ){
			printf( "%s",c ) ;
			free( c ) ;
			c = NULL ;
		}else{
			break ;
		}
	}
	ProcessWaitUntilFinished( &p ) ;
	return 0 ;
}

#endif

/*
 * Example use case of ProcessSetArguments API
 */
#if 0

#include "process.h"
#include <stdio.h>
#include <stdlib.h>

int main( void )
{
	const char * argv[ 3 ] ;
	process_t p = Process( "" ) ;
	argv[ 0 ] = "/bin/ls" ;
	argv[ 1 ] = "-l" ;
	argv[ 2 ] = NULL ;
	ProcessSetArguments( p,argv ) ;
	ProcessStart( p ) ;
	char * c = NULL ;
	while( 1 ){
		ProcessGetOutPut( p,&c,ProcessStdOut ) ;
		if( c ){
			printf( "%s",c ) ;
			free( c ) ;
			c = NULL ;
		}else{
			break ;
		}
	}
	ProcessWaitUntilFinished( &p ) ;
	return 0 ;
}

#endif


/*
 * Example use case of ProcessArgumentStructure API
 */
#if 0

#include "process.h"
#include <stdio.h>
#include <stdlib.h>

int main( void )
{
	const char * argv[ 3 ] ;
	process_t p = Process( "" ) ;
	argv[ 0 ] = "/bin/ls" ;
	argv[ 1 ] = "-l" ;
	argv[ 2 ] = NULL ;
	ProcessStructure * str = ProcessArgumentStructure( p ) ;
	str->args =  argv ;
	ProcessStart( p ) ;
	char * c = NULL ;
	while( 1 ){
		ProcessGetOutPut( p,&c,ProcessStdOut ) ;
		if( c ){
			printf( "%s",c ) ;
			free( c ) ;
			c = NULL ;
		}else{
			break ;
		}
	}
	ProcessWaitUntilFinished( &p ) ;
	return 0 ;
}

#endif
