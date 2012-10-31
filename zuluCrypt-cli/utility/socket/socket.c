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

#include "socket.h"

struct SocketType_t
{
	int socket_server ;
	int type ;
	int protocol ;
	int cmax ;
	int domain ;
	socklen_t size ;
	int fd ;
	struct sockaddr_un * local ;
	struct sockaddr_in * net ;
};

static void __debug( const char * msg )
{
	printf( "%s\n",msg ) ;
	fflush( stdout );
}

static void __debug1( const char * msg )
{
	perror( msg ) ;
	fflush( stdout );
}

static void ( *__SocketErrorFunction__ )( void )  = NULL ;

void SocketExitOnMemoryExaustion( void ( *f )( void ) )
{
	__SocketErrorFunction__ = f ;
}

static socket_t _SocketError( void )
{
	if( __SocketErrorFunction__ != NULL )
		( *__SocketErrorFunction__ )() ;
	
	return SocketVoid ;
}

socket_t Socket( int domain,int type,int protocol ) 
{
	socket_t s ;
	int fd = socket( domain,type,protocol ) ;
	
	if( fd == -1 )
		return SocketVoid ;

	s = ( socket_t ) malloc( sizeof( struct SocketType_t ) ) ;
		
	if( s == NULL ){
		close( fd ) ;
		return _SocketError() ;
	}

	memset( s,'\0',sizeof( struct SocketType_t ) ) ;
	
	if( domain == AF_UNIX ){
		s->local = ( struct sockaddr_un * ) malloc( sizeof( struct sockaddr_un ) ) ;
		if( s->local == NULL ){
			free( s ) ;
			close( fd ) ;
			return _SocketError() ;
		}else{
			s->size = sizeof( struct sockaddr_un ) ;
			memset( s->local,'\0',s->size ) ;
			s->local->sun_family = AF_UNIX ;
		}
	}else if( domain == AF_INET ){
		s->net = ( struct sockaddr_in  * ) malloc( sizeof( struct sockaddr_in ) ) ;
		if( s->net == NULL ){
			free( s ) ;
			close( fd ) ;
			return _SocketError() ;
		}else{
			s->size = sizeof( struct sockaddr_in ) ;
			memset( s->net,'\0',s->size ) ;
			s->net->sin_family = AF_INET ;
		}
	}else{
		close( fd ) ;
		free( s ) ;
		return SocketVoid ;
	}
	
	s->domain = domain ;
	s->type = type ;
	s->protocol = protocol ;
	s->cmax = 1 ;
	s->socket_server = 0 ;
	s->fd = fd ;
	return s ;
}

int SocketFileDescriptor( socket_t s ) 
{
	return s == SocketVoid ? -1 : s->fd ;
}

socket_t SocketLocalWithOptions( const char * address,int type,int protocol ) 
{
	socket_t s = Socket( AF_UNIX,type,protocol ) ;
	if( s != SocketVoid ){
		size_t size = sizeof( s->local->sun_path ) ;
		strncpy( s->local->sun_path,address,size ) ;
		s->local->sun_path[ size - 1 ] = '\0' ;
	}
	return s ;
}

socket_t SocketLocal( const char * address )
{
	return SocketLocalWithOptions( address,SOCK_STREAM,0 ) ;
}

socket_t SocketNetByNameWithOptions( const char * address,int port,int type,int protocol ) 
{
	socket_t s = SocketVoid ; 
	struct addrinfo hint ;
	struct addrinfo * addr ;
	struct sockaddr_in * addr_in ;
	
	memset ( &hint,0,sizeof( hint ) ) ;
	hint.ai_family = AF_INET ;
	hint.ai_socktype = type ;
	
	if( getaddrinfo( address,NULL,&hint,&addr ) == 0 ){
			
		s = Socket( AF_INET,type,protocol ) ;
	
		if( s != SocketVoid ){
			addr_in = ( struct sockaddr_in * ) addr->ai_addr ;
			memcpy( &s->net->sin_addr,&addr_in->sin_addr,sizeof( addr_in->sin_addr ) ) ;
			s->net->sin_port = htons( port );
		}
		
		freeaddrinfo( addr ) ;
	}
	
	return s ;
}

socket_t SocketNetByName( const char * address,int port )
{
	return SocketNetByNameWithOptions( address,port,SOCK_STREAM,0 ) ;
}

socket_t SocketNetByIPAddress( const char * address,int port ) 
{	
	socket_t s = Socket( AF_INET,SOCK_STREAM,0 ) ;
	if( s != SocketVoid ){
		inet_aton( address,&s->net->sin_addr ) ;
		s->net->sin_port = htons( port );
	}
	return s ;
}

const char * SocketAddress( socket_t s )
{	
	if( s == SocketVoid )
		return NULL ;
	
	if( s->domain == AF_UNIX )
		return s->local->sun_path ;
	else
		return inet_ntoa( s->net->sin_addr ) ;
}

int SocketBind( socket_t s )
{
	if( s == SocketVoid )
		return 0 ;
	if( s->domain == AF_UNIX ){
		s->socket_server = 1 ;
		unlink( s->local->sun_path ) ;
		return bind( s->fd,( struct sockaddr * )s->local,s->size ) == 0 ? 1 : 0 ;
	}else{
		return bind( s->fd,( struct sockaddr * )s->net,s->size ) == 0 ? 1 : 0 ;
	}
}

socket_t SocketAccept( socket_t s ) 
{
	socket_t x = ( socket_t ) malloc( sizeof( struct SocketType_t ) ) ;
	
	if( x == NULL )
		return _SocketError() ;
	
	memset( x,'\0',sizeof( struct SocketType_t ) ) ;
	__debug( "accepting a socket" ) ;
	if( s->domain == AF_UNIX ){
		x->local = ( struct sockaddr_un * ) malloc( sizeof( struct sockaddr_un ) ) ;
		if( x->local == NULL ){
			free( x ) ;
			x = _SocketError() ;
		}else{
			memset( x->local,'\0',sizeof( struct sockaddr_un ) ) ;
			x->fd = accept( s->fd,( struct sockaddr * )x->local,&x->size ) ;
			if( x->fd == -1 ){
				__debug1( "failed to accept a socket" ) ;
				free( x->local ) ;
				free( x ) ;
				x = SocketVoid ;
			}else{
				__debug( "a socket accepted" ) ;
				x->domain = AF_UNIX ;
				/*
					x->local->sun_family = AF_UNIX ;
				*/
				x->type = SOCK_STREAM ;
				x->protocol = 0 ;
				x->cmax = 1 ;
				x->socket_server = 0 ;
			}
		}
	}else{
		x->net = ( struct sockaddr_in * ) malloc( sizeof( struct sockaddr_in ) ) ;
		if( x->net == NULL ){
			free( x ) ;
			x = _SocketError() ;
		}else{
			memset( x->net,'\0',sizeof( struct sockaddr_in ) ) ; 
			x->fd = accept( s->fd,( struct sockaddr * )x->net,&x->size ) ;
			if( x->fd == -1 ){
				free( x->net ) ;
				free( x ) ;
				x = SocketVoid ;
			}else{
				x->domain = AF_INET ;
				/*
					x->net->sin_family = AF_INET ;
				*/	
				x->type = SOCK_STREAM ;
				x->protocol = 0 ;
				x->cmax = 1 ;
				x->socket_server = 0 ;
			}
		}
	}
	
	return x ;
}

int SocketIsBlocking( socket_t s ) 
{
	int flags ;
	
	if( s == SocketVoid )
		return -1 ;
	
	flags = fcntl( s->fd,F_GETFL,0 ) ;
	return flags != ( flags | O_NONBLOCK ) ;
}

#define READ  1
#define WRITE 0

static inline int __SocketTimeOut( socket_t s,int time,int mode )
{
	int fd = s->fd ;
	fd_set readfd ;
	fd_set writefd ;

	struct timeval interval ;
	
	interval.tv_sec = time ;
	interval.tv_usec = 0 ;
	
	FD_ZERO( &readfd ) ;
	FD_SET( fd,&readfd ) ;
	
	FD_ZERO( &writefd ) ;
	FD_SET( fd,&writefd ) ;
	
	if( mode == READ ){
		select( fd + 1,&readfd,NULL,NULL,&interval ) ;
		return FD_ISSET( fd,&readfd ) ;
	}else{
		select( fd + 1,NULL,&writefd,NULL,&interval ) ;
		return FD_ISSET( fd,&writefd ) ;
	}
}

socket_t SocketAcceptWithTimeOut( socket_t s,int time ) 
{	
	socket_t client = SocketVoid ;
	int st = __SocketTimeOut( s,time,READ ) ;
	if( st ){
		__debug( "socket timed out with a connection request" ) ;
		client = SocketAccept( s ) ;
	}else{
		__debug1( "socket timed out without a connection request" ) ;
	}
	return client ;
}

static inline void __SocketClose( socket_t * p ) 
{	
	socket_t s = *p ;
	int fd = s->fd ;
	
	*p = SocketVoid ;
	
	shutdown( fd,SHUT_RDWR ) ;
	close( fd ) ;
	
	if( s->domain == AF_UNIX ) {
		if( s->socket_server ){
			unlink( s->local->sun_path ) ;
			__debug( "closed a server sockket" ) ;
		}else{
			__debug( "closed a client sockket" ) ;
		}
		free( s->local ) ;
		
	}else{
		free( s->net ) ;
	}
	
	free( s ) ;
}

void SocketClose( socket_t * p ) 
{
	if( p != NULL && *p != SocketVoid )
		__SocketClose( p ) ;
}

static inline int __SocketConnect( socket_t s ) 
{
	if( s->domain == AF_UNIX ){
		return connect( s->fd,( struct sockaddr * )s->local,s->size ) == 0 ? 1 : 0 ;
	}else{
		return connect( s->fd,( struct sockaddr * )s->net,s->size ) == 0 ? 1 : 0 ;
	}
}

int SocketConnect( socket_t * s ) 
{
	if( s == NULL || *s == SocketVoid )
		return 0 ;
	if(  __SocketConnect( *s ) ){
		return 1 ;
	}else{
		__SocketClose( s ) ;
		return 0 ;
	}	
}

void SocketSetListenMaximum( socket_t s,int m ) 
{
	if( s != SocketVoid )
		s->cmax = m ;
}

int SocketSetDoNotBlock( socket_t s ) 
{
	int flags ;
	if( s == SocketVoid )
		return -1 ;
	flags = fcntl( s->fd,F_GETFL,0 );
	return flags == -1 ? -1 : fcntl( s->fd,F_SETFL,flags | O_NONBLOCK ) ;
}

int SocketSetBlock( socket_t s )
{
	int flags ;
	if( s == SocketVoid )
		return -1 ;
	flags = fcntl( s->fd,F_GETFL,0 );

	return flags == -1 ? -1 : fcntl( s->fd,F_SETFL,flags & ~O_NONBLOCK  ) ;
}
 
int SocketListen( socket_t s ) 
{
	if( s == SocketVoid )
		return 0 ;
	else
		return listen( s->fd,s->cmax ) == 0 ? 1 : 0 ;
}

ssize_t SocketGetData_2( socket_t s,char * buffer,size_t len ) 
{	
	ssize_t e ;
	if( s == SocketVoid )
		return -1 ;
		
	len = len - 1 ;
	e = read( s->fd,buffer,len ) ; 
	if( e >= 0 )
		buffer[ e ] = '\0' ;
	return e ;
}

#define BUFFSIZE 32
#define BUFFERINIT 64
#define FACTOR 2

static inline char * __expandBuffer( char * buffer,size_t new_size,size_t * buff_size )
{
	char * e ;
	if( new_size >= *buff_size ){
		*buff_size = *buff_size * FACTOR ;
		e = realloc( buffer,*buff_size ) ;
		if( e == NULL ){
			free( buffer ) ;
			_SocketError() ;
			return NULL ;
		}else{
			return e ;
		}
	}else{
		return buffer ;
	}
}

size_t SocketGetData_1( socket_t s,char ** e ) 
{
	int fd ;
	ssize_t result ;
	size_t total = 0 ;
	size_t buff_size = BUFFERINIT ;
	
	char buffer[ BUFFSIZE ] ;
	char * d ;
	char * f ;
	
	if( s == SocketVoid )
		return 0 ;
	
	f = ( char * ) malloc( sizeof( char ) * buff_size ) ;
	
	if( f == NULL ){
		_SocketError() ;
		return 0 ;
	}
	
	fd = s->fd ;
	
	while( 1 ){
		result = read( fd,buffer,BUFFSIZE ) ;
		
		if( result <= 0 )
			break ;
		
		d = __expandBuffer( f,total + result,&buff_size ) ;
		
		if( d == NULL )
			return 0 ;
		
		f = d ;
		
		memcpy( f + total,buffer,result ) ;
		total = total + result ;
	}
	
	if( total ){
		d = ( char * ) realloc( f,total + 1 ) ;
		if( d == NULL ){
			free( f ) ;
			_SocketError() ;
			return 0 ;
		}else{
			d[ total ] = '\0' ;
			*e = d ;
		}
	}else{
		free( f ) ;
	}
	
	return total ;
}

size_t SocketGetData( socket_t s,char ** e,size_t len ) 
{
	int fd ;
	ssize_t result ;
	size_t total = 0 ;
	size_t buff_size = BUFFERINIT ;
	
	char buffer[ BUFFSIZE ] ;
	char * d ;
	char * f ;
	
	if( s == SocketVoid )
		return 0 ;

	f = ( char * ) malloc( sizeof( char ) * buff_size ) ;
	
	if( f == NULL ){
		_SocketError() ;
		return 0 ;
	}
	
	fd = s->fd ;
	
	while( 1 ){
		result = read( fd,buffer,BUFFSIZE ) ;
		
		if( result <= 0 )
			break ;
		
		d = __expandBuffer( f,total + result,&buff_size ) ;
		
		if( d == NULL )
			return 0 ;
		
		f = d ;
		
		memcpy( f + total,buffer,result ) ;
		total = total + result ;
		
		if( total >= len ){
			total = len ;
			break ;
		}
	}
	
	if( total ){
		d = ( char * ) realloc( f,total + 1 ) ;
		if( d == NULL ){
			free( f ) ;
			_SocketError() ;
			return 0 ;
		}else{
			d[ total ] = '\0' ;
			*e = d ;
		}
	}else{
		free( f ) ;
	}
	
	return total ;
}

ssize_t SocketSendData( socket_t s,const char * buffer,size_t len ) 
{		
	size_t sent = 0 ;
	size_t remain = len ;
	
	if( s == SocketVoid || buffer == NULL || len == 0 )
		return -1 ;
	do{
		sent = sent + write( s->fd,buffer + sent,remain ) ;
		remain = remain - sent ;
	}while( sent != len );
	
	return sent ;
}
