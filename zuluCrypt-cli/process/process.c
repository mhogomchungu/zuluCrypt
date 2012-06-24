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

#include "process.h"

struct Process_t{
	size_t len ;
	pid_t pid ;
	int pd[2] ;
	int state ;
	int std_io ;
	char * exe ;
	char delimiter ;
	char ** args ;
	int args_source ;
	int signal ;
	int timeout ;
	int wait_status ;
};

typedef struct st_2{
	pid_t pid  ;
	int time ;
	int signal ;
	int * status ;
	process_t p;
}st_1;

void ProcessSetArgumentList( process_t p,... )
{
	char * entry ;
	char ** args  ;
	size_t size = sizeof( char * ) ;
	int index = 0 ;
	va_list list ;
	
	args = ( char ** )malloc( size ) ;
	args[ index ] = p->exe ;
	index++ ;
	
	va_start( list,p ) ;
	
	while( 1 ){
		entry = va_arg( list,char * ) ;
		args = ( char ** )realloc( args,( 1 + index ) * size ) ;
		args[ index ] = entry ;
		index++ ;
		if( entry == '\0' )
			break ;
	}
	
	va_end( list ) ;
	
	p->args = args ;
	p->args_source = 0 ;	
}

static void ProcessSetArguments_1( process_t p ) 
{
	/*
	 * this function converts a one dimentional array into a two dimentional array as expected by the second argument of execl
	 * 
	 */
	size_t k = 0 ;
	
	char * c ;
	const char * d ;
	
	char ** f ;
	char delimiter ; 
	
	if( p->args != NULL ){
		/*
		 * Assuming the arments list was set by ProcessSetArguments()
		 */
		return ;		
	}
	
	p->args_source = 0 ;

	delimiter = p->delimiter ;
	
	d = p->exe - 1 ;
	
	/*
	 * find out to how many pieces we should break the string
	 */
	while( *++d ){
		if( *d == delimiter ) ;
		k++ ;
	}
	
	/*
	 * create an array of pointers,each slot will point to each "piece" of string creating a 2-D array
	 */
	f = ( char ** ) malloc( sizeof( char * ) * ( k + 2 ) ) ;

	*f = p->exe ;
	
	c = p->exe - 1;

	k = 1 ;
	
	while( *++c ){
		if( *c == delimiter ){
			*c = '\0' ;      /* add null to break a string into pieces */
			f[ k ] = c + 1 ; /* point to the beginning of the next piece   */
			k++ ;
		}
	}
	
	f[ k ] = '\0' ;
	
	p->args = f ;
}

pid_t ProcessStart( process_t p ) 
{
	if( p->std_io >= 4 )
		if( pipe( p->pd ) != 0 ) 
			return -1 ;

	p->pid = fork() ;
	
	if( p->pid == -1 )
		return -1 ;
	
	p->state = RUNNING ;
	
	ProcessSetArguments_1( p ) ;
	
	if( p->pid == 0 ){

		/*
		#define CLOSE_STD_OUT 		1
		#define CLOSE_STD_ERROR		2
		#define CLOSE_BOTH_STD_OUT 	3
		#define READ_STD_OUT 		4
						5 = 4 + 1
						6 = 4 + 2
						7 = 4 + 3
		#define READ_STD_ERROR 		8
						9 = 8 + 1
						..
						..
		#define WRITE_STD_IN 		12
						13 = 12 + 1
						..
						..
		*/
		
		switch( p->std_io ){
			case 1     : close( 1 ) ; 
				     break ;
			case 2     : close( 2 ) ; 
				     break ;
			case 3     : close( 1 ) ;
				     close( 2 ) ; 
				     break ;
			
			case 4     :	     
			case 5     : close( 1 ) ;
				     close( p->pd[ 0 ] ) ;
				     dup2( p->pd[ 1 ],1 ) ;
				     break ;
				
			case 6     :	     
			case 7     : close( 2 ) ;
				     close( 1 ) ;
			             close( p->pd[ 0 ] ) ;
			             dup2( p->pd[ 1 ],1 ) ;
			             break ;				     
			case 10    :	     
			case 8     : close( 2 ) ;
				     close( p->pd[ 0 ] ) ;
			             dup2( p->pd[ 1 ],2 ) ;
			             break ;	
			case 11    :	     
			case 9     : close( 1 ) ;
				     close( 2 ) ;
				     close( p->pd[ 0 ] ) ;
				     dup2( p->pd[ 1 ],2 ) ;
			       	     break ;

			case 12    : close( 0 ) ;
				     close( p->pd[ 1 ] ) ;
			             dup2( p->pd[ 0 ],0 ) ;
				     break ;
			case 13    : close( 1 ) ;
				     close( 0 ) ;
				     close( p->pd[ 1 ] ) ;
				     dup2( p->pd[ 0 ],0 ) ;
				     break ;
			case 14    : close( 1 ) ;
				     close( 0 ) ;
			             close( p->pd[ 1 ] ) ;
			             dup2( p->pd[ 0 ],0 ) ;
			             break ;	     
		}
		
		execv( p->args[0],p->args ) ;
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
	if( p->std_io <= 3 )
		;
	else if( p->std_io < 12 )
		close( p->pd[ 1 ] ) ;
	else
		close( p->pd[ 0 ] ) ;
	
	return p->pid ;
}

char * ProcessGetOutPut( process_t p ) 
{
	#define SIZE 64
	char * buffer = NULL ;
	char buff[ SIZE ] ;
	int size = 0 ;
	int count ;

	while( 1 ) {
		count = read( p->pd[ 0 ],buff,SIZE ) ;

		if( count == SIZE ){
			buffer = ( char * ) realloc( buffer,size + SIZE + 1 ) ;
			memcpy( buffer + size,buff,SIZE ) ;
			buffer[ size + SIZE ] = '\0' ;
			size += SIZE ;
		}else if( count < SIZE && count != 0 ){
			buffer = ( char * ) realloc( buffer,size + count + 1 ) ;
			memcpy( buffer + size,buff,count ) ;
			buffer[ size + count ] = '\0' ;
			size += count ;
		}else
			break ;	
	}	
	
	return buffer ;
}

int ProcessGetOutPut_1( process_t p,char * buffer,int size ) 
{
	return read( p->pd[ 0 ],buffer,size ) ;
}

int ProcessWrite( process_t p,const char * data ) 
{	
	return write( p->pd[ 1 ],data,strlen( data ) ) ;
}

process_t Process( const char * path ) 
{
	process_t p = ( process_t ) malloc( sizeof( struct Process_t ) ) ;
	
	if( p == NULL )
		return NULL ;
	
	p->len = strlen( path ) ;
	
	p->exe = ( char * ) malloc( sizeof( char ) * ( p->len + 1 ) ) ;

	if( p->exe == NULL ){
		free( p ) ;
		return NULL ;
	}
	
	strcpy( p->exe,path ) ;
	
	p->std_io = 0 ;
	p->delimiter = ' '  ;
	p->args = NULL      ;
	p->state = HAS_NOT_START;
	p->signal = -1 ;
	p->timeout = -1 ;
	p->wait_status = -1 ;
	p->args_source = -1 ;
	
	return p ;
}

void ProcessSetOption( process_t p,int opt ) 
{
	p->std_io += opt ;	
}

void ProcessSetOptionTimeout( process_t p,int timeout,int signal ) 
{
	p->signal = signal ;
	p->timeout = timeout ;
}

void ProcessSetOptionDelimiter( process_t p,char s ) 
{
	p->delimiter = s ;
}

void ProcessDelete( process_t * p ) 
{
	process_t px = *p ;
	*p = NULL ;

	if( px->std_io <= 3 )
		;
	else if( px->std_io < 12 )
		close( px->pd[ 0 ] ) ;
	else
		close( px->pd[ 1 ] ) ;	
	
	if( px->wait_status == -1 )
		waitpid( px->pid,0,WNOHANG ) ;
		
	if( px->args != NULL )
		if( px->args_source == 0 )
			free( px->args ) ;
	
	if( px->exe != NULL )
		free( px->exe ) ;
	
	free( px ) ;		
}

int ProcessTerminate( process_t p ) 
{
	int st ;
	p->state = CANCELLED ;
	st = kill( p->pid,SIGTERM ) ;
	waitpid( p->pid,0,WNOHANG ) ;
	
	return st ;
}

int ProcessKill( process_t p ) 
{
	int st ;
	p->state = CANCELLED ;
	
	st = kill( p->pid,SIGKILL ) ;
	waitpid( p->pid,0,WNOHANG ) ;
	
	return st ;
	
}

static void * __timer( void * x )
{
	st_1 * t = ( st_1 * ) x ;
	
	sleep( t->time ) ;

	kill( t->pid,t->signal ) ;

	t->p->state = CANCELLED ;
	
	return ( void * ) 0 ; 
}

static int ProcessWaitWithTimer( process_t p )
{
	int status ;
	
	pthread_t thread ;
	
	st_1 t ;
	
	t.pid = p->pid ;
	t.time = p->timeout ;
	t.signal = p->signal ;
	t.p = p ;
	
	pthread_create( &thread,NULL,__timer,( void * ) &t );
	
	waitpid( p->pid,&status,0 ) ;	
	
	pthread_cancel( thread ) ;
	
	return status ;
}

int ProcessExitStatus( process_t p )
{
	int status ;
	
	if( p->timeout != -1 ){
		status = ProcessWaitWithTimer( p ) ;		
	}else{
		waitpid( p->pid,&status,0 ) ;	
		p->state = FINISHED ;		
	}	
	
	p->wait_status = 1 ;
	
	return status ;
}

void ProcessSetArguments( process_t p,char * const s[] ) 
{
	p->args = ( char ** ) s ;
	p->args_source = 1 ;
}

int ProcessSetSearchPaths( const char * s ) 
{
	return -1 ;
}
