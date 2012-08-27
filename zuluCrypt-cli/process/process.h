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

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>

#ifdef __STDC__
int kill( pid_t,int );
int seteuid( uid_t uid );
int setegid( uid_t uid ) ;
#endif


#ifndef PROCESS
#define PROCESS

#ifdef __cplusplus
extern "C" {
#endif	
	
typedef struct Process_t * process_t ;

/*
 * An example of how to use the library to call ls with arguments and get its output while closing its std error 
 * 
 *  process_t p = Process( "/bin/ls -l -h" ) ;
 * 
 *  ProcessSetOption( p,CLOSE_STD_ERROR ) ;
 *  ProcessSetOption( p,READ_STD_OUT ) ;
 *  
 *  ProcessStart( p ) ;
 * 
 *  char * c = ProcessGetOutPut( p ) ;
 * 
 *  ProcessDelete( &p ) ;
 * 
 *  printf("%s\n",c);
 *  free( c ) ;
 * 
 */

/*
 * create a handle to a process that will be started.
 * Path to executable to be started must be in full path format.
 *
 * By defaults, arguments aew separated by space character,the delimiter can be changed with ProcessSetOptionDelimiter()
 * 
 * Make sure delimiter character do not exist consercutively and do not exist at the ends of the argument string.A crash
 * will most likey happen is this expectation is not obeyed.
 *  
 */
process_t Process( const char * path ) ;

/*
 * call fork() and then exec and run a program specified when handle p was created.
 * return value: pid of the child process 
 */
pid_t ProcessStart( process_t p ) ;

/*
 * pass data to the child process,the child process will get the data from reading its stdin.
 * ProcessSetOption( p,WRITE_STD_IN ) must be called first for this to work.
 * 
 */
int ProcessWrite( process_t p,const char * data ) ;

/*
 * default delimiter is ' '( space character ),set another character with this function to change it
 * 
 */
void ProcessSetOptionDelimiter( process_t,char ) ;

/*
 * remember to clean after yourself
 */
void ProcessDelete( process_t * ) ;

/*
 * send a forked process sigterm to terminate it.
 */
int ProcessTerminate( process_t ) ;

/*
 * run the child process with privileges of a user with UID of uid
 */
void ProcessSetUser( process_t,uid_t uid ) ;

/*
 * send a forked process sigkill to kill it
 */
int ProcessKill( process_t ) ;

/*
 * the forked process is started by calling, execl.
 * this function can be used to set argument list *
 */
void ProcessSetArguments( process_t p,char * const argv[] ) ;

/*
 * the forked process is started with arguments list given as const char * and the
 * last entry ust be null.
 * example:
 * process_t p = Process( "/usr/bin/du" ) ;
 * ProcessSetArgumentList( p,"-s","-c","-h",'\0' ) ;
 * ProcessStart( p ) ;
 * ProcessDelete( &p ) ;
 */
void ProcessSetArgumentList( process_t p,... ) ;
/*
 * get state of the process handled by handle p 
 */
#define HAS_NOT_START 1
#define RUNNING 2
#define FINISHED 3
#define CANCELLED 4
int ProcessState( process_t p ) ;

/*
 * Set how the forked process is to behave.
 */
#define CLOSE_STD_OUT 		1
#define CLOSE_STD_ERROR		2
#define CLOSE_BOTH_STD_OUT 	3
#define READ_STD_OUT 		4
#define READ_STD_ERROR 		8
#define WRITE_STD_IN 		12

void ProcessSetOption( process_t,int ) ;

/*
 * wait for "timeout" seconds and then send the forked process a signal "signal"
 */
void ProcessSetOptionTimeout( process_t p,int timeout,int signal ) ;

/*
 * waitpid() for forked process to exit and get its exit status.
 * If the exit status is not waiter for,waitpid() with WNOHANG argument will be called when calling ProcessDelete
 * to make sure the forked process doesnt turn to a zombie.
 */
int ProcessExitStatus( process_t ) ;

/*
 * read get contents of std out from the process.
 * remember to free() the return buffer when done with it
 * 
 * this function must be called after ProcessStart()
 */
char * ProcessGetOutPut( process_t ) ;

/*
 * read size number of bytes from the ourput of the forket process.
 */
int ProcessGetOutPut_1( process_t,char * buffer,int size ) ;

#ifdef __cplusplus
}
#endif

#endif
