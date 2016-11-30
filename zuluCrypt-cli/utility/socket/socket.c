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

#include "socket.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netdb.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>

struct SocketType_t
{
	int socket_server ;
	int type ;
	int protocol ;
	int cmax ;
	int domain ;
	socklen_t size ;
	int fd ;
	void * socket ;
	char * inetAddress ;
};

static void ( *_fcn_ )( void )  = NULL ;

void SocketExitOnMemoryExaustion( void ( *f )( void ) )
{
	_fcn_ = f ;
}

static socket_t _SocketError( void )
{
	if( _fcn_ != NULL ){
		_fcn_() ;
	}
	return SocketVoid ;
}

static inline socket_t _SocketLocal( void )
{
	socket_t s = malloc( sizeof( struct SocketType_t ) ) ;
	struct sockaddr_un * e ;

	if( s == NULL ){
		return SocketVoid ;
	}else{
		memset( s,'\0',sizeof( struct SocketType_t ) ) ;

		s->socket = malloc( sizeof( struct sockaddr_un ) ) ;

		if( s->socket == NULL ){
			free( s ) ;
			return _SocketError() ;
		}else{
			s->size = sizeof( struct sockaddr_un ) ;
			memset( s->socket,'\0',s->size ) ;
			e = s->socket ;
			e->sun_family = AF_UNIX ;
			return s ;
		}
	}
}

static inline socket_t _SocketNet( void )
{
	socket_t s = malloc( sizeof( struct SocketType_t ) ) ;
	struct sockaddr_in * e ;

	if( s == NULL ){
		return SocketVoid ;
	}else{
		memset( s,'\0',sizeof( struct SocketType_t ) ) ;

		s->socket = malloc( sizeof( struct sockaddr_in ) ) ;

		if( s->socket == NULL ){
			free( s ) ;
			return _SocketError() ;
		}else{
			s->inetAddress = NULL ;
			s->size = sizeof( struct sockaddr_in ) ;
			memset( s->socket,'\0',s->size ) ;
			e = s->socket ;
			e->sin_family = AF_INET ;
			return s ;
		}
	}
}

static inline socket_t _SocketNet6( void )
{
	socket_t s = malloc( sizeof( struct SocketType_t ) ) ;
	struct sockaddr_in6 * e ;

	if( s == NULL ){
		return SocketVoid ;
	}else{
		memset( s,'\0',sizeof( struct SocketType_t ) ) ;

		s->socket = malloc( sizeof( struct sockaddr_in6 ) ) ;

		if( s->socket == NULL ){
			free( s ) ;
			return _SocketError() ;
		}else{
			s->inetAddress = NULL ;
			s->size = sizeof( struct sockaddr_in6 ) ;
			memset( s->socket,'\0',s->size ) ;
			e = s->socket ;
			e->sin6_family = AF_INET6 ;
			return s ;
		}
	}
}

socket_t Socket( int domain,int type,int protocol )
{
	socket_t s = SocketVoid ;
	int fd ;

	if( domain != AF_UNIX && domain != AF_INET && domain != AF_INET6 ){
		return SocketVoid ;
	}

	fd = socket( domain,type,protocol ) ;

	if( fd == -1 ){
		return SocketVoid ;
	}

	fcntl( fd,F_SETFD,FD_CLOEXEC ) ;

	switch( domain ){
		case AF_UNIX : s = _SocketLocal() ; break ;
		case AF_INET : s = _SocketNet()   ; break ;
		case AF_INET6: s = _SocketNet6()  ; break ;
	}

	if( s == SocketVoid ){
		close( fd ) ;
		return SocketVoid ;
	}else{
		s->domain = domain ;
		s->type = type ;
		s->protocol = protocol ;
		s->cmax = 1 ;
		s->socket_server = 0 ;
		s->fd = fd ;
		return s ;
	}
}

int SocketFileDescriptor( socket_t s )
{
	if( s == SocketVoid ){
	      return -1 ;
	}else{
	      return s->fd ;
	}
}

socket_t SocketLocalWithOptions( const char * address,int type,int protocol )
{
	socket_t s = Socket( AF_UNIX,type,protocol ) ;
	struct sockaddr_un * e ;
	size_t l = sizeof( e->sun_path ) ;
	if( s != SocketVoid ){
		e = s->socket ;
		strncpy( e->sun_path,address,l ) ;
	}
	return s ;
}

socket_t SocketLocal( const char * address )
{
	return SocketLocalWithOptions( address,SOCK_STREAM,0 ) ;
}

socket_t SocketNetWithOptions( const char * address,int port,int type,int protocol )
{
	socket_t s = SocketVoid ;

	struct addrinfo * info ;
	struct addrinfo hint ;
	struct sockaddr_in * e ;

	memset( &hint,'\0',sizeof( hint ) ) ;

	hint.ai_family = AF_INET ;
	hint.ai_socktype = type ;
	hint.ai_protocol = protocol ;

	if( getaddrinfo( address,NULL,&hint,&info) == 0 ){
		s = Socket( AF_INET,type,protocol ) ;
		if( s != SocketVoid ){
			e = s->socket ;
			memcpy( e,info->ai_addr,info->ai_addrlen ) ;
			e->sin_port = htons( port ) ;
		}

		freeaddrinfo( info ) ;
	}

	return s ;
}

socket_t SocketNet( const char * address,int port )
{
	return SocketNetWithOptions( address,port,SOCK_STREAM,0 ) ;
}

socket_t SocketNetWithOptions6( const char * address,int port,int type,int protocol )
{
	socket_t s = SocketVoid ;

	struct addrinfo hint ;
	struct addrinfo * info ;
	struct sockaddr_in6 * e ;

	memset( &hint,'\0',sizeof( struct addrinfo ) ) ;

	hint.ai_family = AF_INET6 ;
	hint.ai_socktype = type ;
	hint.ai_protocol = protocol ;

	if( getaddrinfo( address,NULL,&hint,&info ) == 0 ){
		s = Socket( AF_INET6,type,protocol ) ;
		if( s != SocketVoid ){
			e = s->socket ;
			memcpy( e,info->ai_addr,info->ai_addrlen ) ;
			e->sin6_port = htons( port ) ;
		}

		freeaddrinfo( info ) ;
	}

	return s ;
}

socket_t SocketNet6( const char * address,int port )
{
	return SocketNetWithOptions6( address,port,SOCK_STREAM,0 ) ;
}

static inline const char * _SocketNetAddress( socket_t s )
{
	const char * c ;
	struct sockaddr_in * e = s->socket ;

	if( s->inetAddress == NULL ){
		s->inetAddress = malloc( sizeof( char ) * INET_ADDRSTRLEN ) ;
		if( s->inetAddress == NULL ){
			_SocketError() ;
			return NULL ;
		}

		c = inet_ntop( AF_INET,&e->sin_addr,s->inetAddress,INET_ADDRSTRLEN ) ;

		if( c == NULL ){
			free( s->inetAddress ) ;
			s->inetAddress = NULL ;
			return NULL ;
		}
	}

	return s->inetAddress ;
}

static inline const char * _SocketNetAddress6( socket_t s )
{
	const char * c ;
	struct sockaddr_in6 * e = s->socket ;

	if( s->inetAddress == NULL ){
		s->inetAddress = malloc( sizeof( char ) * INET6_ADDRSTRLEN ) ;
		if( s->inetAddress == NULL ){
			_SocketError() ;
			return NULL ;
		}

		c = inet_ntop( AF_INET6,&e->sin6_addr,s->inetAddress,INET6_ADDRSTRLEN ) ;

		if( c == NULL ){
			free( s->inetAddress ) ;
			s->inetAddress = NULL ;
			return NULL ;
		}
	}

	return s->inetAddress ;
}

static inline const char * _SocketLocalAddress( socket_t s )
{
	struct sockaddr_un * e = s->socket ;
	return e->sun_path ;
}

const char * SocketAddress( socket_t s )
{
	if( s == SocketVoid ){
		return NULL ;
	}else{
		switch( s->domain ){
			case AF_UNIX : return _SocketLocalAddress( s ) ;
			case AF_INET : return _SocketNetAddress( s ) ;
			case AF_INET6: return _SocketNetAddress6( s ) ;
			default      : return NULL ;
		}
	}
}

int SocketBind( socket_t s )
{
	struct sockaddr * e ;
	struct sockaddr_un * f ;

	char buffer[ sizeof( f->sun_path ) + 1 ] ;

	if( s == SocketVoid ){
		return 0 ;
	}

	e = s->socket ;

	if( s->domain == AF_UNIX ){
		s->socket_server = 1 ;
		f = s->socket ;
		strncpy( buffer,f->sun_path,sizeof( f->sun_path ) ) ;
		*( buffer + sizeof( f->sun_path ) ) = '\0' ;
		unlink( buffer ) ;
		return bind( s->fd,e,s->size ) == 0 ;
	}else if( s->domain == AF_INET ){
		return bind( s->fd,e,s->size ) == 0 ;
	}else if (s->domain == AF_INET6 ){
		return bind( s->fd,e,s->size ) == 0 ;
	}else{
		return 0 ;
	}
}

static inline socket_t _SocketAcceptLocal( socket_t s )
{
	struct sockaddr_un * e ;
	struct sockaddr_un * z ;

	struct sockaddr * f ;

	size_t size = sizeof( struct sockaddr_un ) ;
	socket_t x = malloc( sizeof( struct SocketType_t ) ) ;

	if( x == NULL ){
		return _SocketError() ;
	}else{
		memset( x,'\0',sizeof( struct SocketType_t ) ) ;

		x->socket = malloc( size ) ;

		if( x->socket == NULL ){
			free( x ) ;
			x = _SocketError() ;
		}else{
			x->size = size ;
			memset( x->socket,'\0',size ) ;
			f = x->socket ;
			x->fd = accept( s->fd,f,&x->size ) ;
			if( x->fd == -1 ){
				free( x->socket ) ;
				free( x ) ;
				x = SocketVoid ;
			}else{
			  	e = s->socket ;
				z = x->socket ;
				strncpy( z->sun_path,e->sun_path,sizeof( e->sun_path ) ) ;
				x->inetAddress = NULL ;
				x->domain = s->domain ;
				x->type = s->type ;
				x->protocol = s->protocol ;
				x->cmax = 1 ;
				x->socket_server = 0 ;
			}
		}
		return x ;
	}
}

static inline socket_t _SocketAcceptNet( socket_t s )
{
	struct sockaddr * f ;

	size_t size = sizeof( struct sockaddr_in ) ;
	socket_t x = ( socket_t ) malloc( sizeof( struct SocketType_t ) ) ;

	if( x == NULL ){
		return _SocketError() ;
	}else{
		memset( x,'\0',sizeof( struct SocketType_t ) ) ;

		x->socket = malloc( size ) ;
		if( x->socket == NULL ){
			free( x ) ;
			x = _SocketError() ;
		}else{
			x->size = size ;
			memset( x->socket,'\0',size ) ;
			f = x->socket ;
			x->fd = accept( s->fd,f,&x->size ) ;
			if( x->fd == -1 ){
				free( x->socket ) ;
				free( x ) ;
				x = SocketVoid ;
			}else{
				x->inetAddress = NULL ;
				x->domain = s->domain ;
				x->type = s->type ;
				x->protocol = s->protocol ;
				x->cmax = 1 ;
				x->socket_server = 0 ;
			}
		}
		return x ;
	}
}

static inline socket_t _SocketAcceptNet6( socket_t s )
{
	struct sockaddr * f ;

	size_t size = sizeof( struct sockaddr_in6 ) ;
	socket_t x = malloc( sizeof( struct SocketType_t ) ) ;

	if( x == NULL ){
		return _SocketError() ;
	}else{
		memset( x,'\0',sizeof( struct SocketType_t ) ) ;

		x->socket = malloc( size ) ;
		if( x->socket == NULL ){
			free( x ) ;
			x = _SocketError() ;
		}else{
			x->size = size ;
			memset( x->socket,'\0',size ) ;
			f = x->socket ;
			x->fd = accept( s->fd,f,&x->size ) ;
			if( x->fd == -1 ){
				free( x->socket ) ;
				free( x ) ;
				x = SocketVoid ;
			}else{
				x->inetAddress = NULL ;
				x->domain = s->domain ;
				x->type = s->type ;
				x->protocol = s->protocol ;
				x->cmax = 1 ;
				x->socket_server = 0 ;
			}
		}
		return x ;
	}
}

socket_t SocketAccept( socket_t s )
{
	switch( s->domain ){
		case AF_UNIX : return _SocketAcceptLocal( s ) ;
		case AF_INET : return _SocketAcceptNet( s ) ;
		case AF_INET6: return _SocketAcceptNet6( s ) ;
		default      : return SocketVoid ;
	}
}

int SocketIsBlocking( socket_t s )
{
	int flags ;

	if( s == SocketVoid ){
		return -1 ;
	}else{
		flags = fcntl( s->fd,F_GETFL,0 ) ;
		return flags != ( flags | O_NONBLOCK ) ;
	}
}

#define READ  1
#define WRITE 0

static inline int _SocketNotTimedOut( socket_t s,time_t time,int mode )
{
	int fd = s->fd ;
	fd_set fdset ;

	struct timeval interval ;

	memset( &interval,'\0',sizeof( struct timeval ) ) ;

	interval.tv_sec = time ;
	interval.tv_usec = 0 ;

	FD_ZERO( &fdset ) ;
	FD_SET( fd,&fdset ) ;

	if( mode == READ ){
		select( fd + 1,&fdset,NULL,NULL,&interval ) ;
		return FD_ISSET( fd,&fdset ) ;
	}else{
		select( fd + 1,NULL,&fdset,NULL,&interval ) ;
		return FD_ISSET( fd,&fdset ) ;
	}
}

socket_t SocketAcceptWithTimeOut( socket_t s,time_t time )
{
	if( _SocketNotTimedOut( s,time,READ ) ){
		return SocketAccept( s ) ;
	}else{
		return SocketVoid ;
	}
}

static inline void _SocketClose( socket_t * p )
{
	socket_t s = *p ;
	int fd = s->fd ;
	struct sockaddr_un * e ;

	*p = SocketVoid ;

	shutdown( fd,SHUT_RDWR ) ;
	close( fd ) ;

	if( s->domain == AF_UNIX ) {
		if( s->socket_server ){
			e = s->socket ;
			unlink( e->sun_path ) ;
		}
	}

	free( s->socket ) ;

	free( s->inetAddress ) ;

	free( s ) ;
}

void SocketClose( socket_t * p )
{
	if( p != NULL && *p != SocketVoid ){
		_SocketClose( p ) ;
	}
}

void SocketCloseWriteChannel( socket_t s )
{
	if( s != NULL ){
		shutdown( s->fd,SHUT_WR ) ;
	}
}

void SocketCloseReadChannel( socket_t s )
{
	if( s != NULL ){
		shutdown( s->fd,SHUT_RD ) ;
	}
}

static inline int _SocketConnect( socket_t s )
{
	return connect( s->fd,s->socket,s->size ) == 0 ;
}

int SocketConnect( socket_t * s )
{
	if( s == NULL || *s == SocketVoid ){
		return 0 ;
	}
	if( _SocketConnect( *s ) ){
		return 1 ;
	}else{
		_SocketClose( s ) ;
		return 0 ;
	}
}

void SocketSetListenMaximum( socket_t s,int m )
{
	if( s != SocketVoid ){
		s->cmax = m ;
	}
}

int SocketSetDoNotBlock( socket_t s )
{
	int flags ;
	if( s == SocketVoid ){
		return -1 ;
	}else{
		flags = fcntl( s->fd,F_GETFL,0 );
		if( flags == -1 ){
			return -1 ;
		}else{
			return fcntl( s->fd,F_SETFL,flags | O_NONBLOCK ) ;
		}
	}
}

int SocketSetBlock( socket_t s )
{
	int flags ;
	if( s == SocketVoid ){
		return -1 ;
	}else{
		flags = fcntl( s->fd,F_GETFL,0 );
		if( flags == -1 ){
			return -1 ;
		}else{
			return fcntl( s->fd,F_SETFL,flags & ~O_NONBLOCK  ) ;
		}
	}
}

int SocketListen( socket_t s )
{
	if( s == SocketVoid ){
		return 0 ;
	}else{
		return listen( s->fd,s->cmax ) == 0 ;
	}
}

ssize_t SocketGetData_2( socket_t s,char * buffer,size_t len )
{
	ssize_t e ;
	if( s == SocketVoid ){
		return -1 ;
	}else{
		len = len - 1 ;
		e = read( s->fd,buffer,len ) ;
		if( e >= 0 ){
			buffer[ e ] = '\0' ;
		}
		return e ;
	}
}

ssize_t SocketGetData_3( socket_t s,char * buffer,size_t len,int timeout )
{
	if( _SocketNotTimedOut( s,( time_t )timeout,READ ) ){
		return SocketGetData_2( s,buffer,len ) ;
	}else{
		return -1 ;
	}
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

ssize_t SocketGetData( socket_t s,char ** e )
{
	int fd ;
	ssize_t result ;
	size_t total = 0 ;
	size_t buff_size = BUFFERINIT ;

	char buffer[ BUFFSIZE ] ;
	char * d ;
	char * f ;

	if( s == SocketVoid ){
		return -1 ;
	}

	f = malloc( sizeof( char ) * buff_size ) ;

	if( f == NULL ){
		_SocketError() ;
		return -1 ;
	}else{
		fd = s->fd ;

		while( 1 ){
			result = read( fd,buffer,BUFFSIZE ) ;

			if( result <= 0 ){
				if( total ){
					break ;
				}else{
					free( f ) ;
					if( result < 0 ){
						return -1 ;
					}else{
						return 0 ;
					}
				}
			}else{
				d = __expandBuffer( f,total + result,&buff_size ) ;

				if( d == NULL ){
					free( f ) ;
					_SocketError() ;
					return -1 ;
				}else{
					f = d ;
					memcpy( f + total,buffer,result ) ;
					total = total + result ;
				}
			}
		}

		if( total ){
			d = ( char * ) realloc( f,total + 1 ) ;
			if( d == NULL ){
				free( f ) ;
				_SocketError() ;
				return -1 ;
			}else{
				d[ total ] = '\0' ;
				*e = d ;
			}
		}else{
			free( f ) ;
		}

		return total ;
	}
}

ssize_t SocketGetData_1( socket_t s,char ** e,size_t len )
{
	int fd ;
	ssize_t result ;
	size_t total = 0 ;
	size_t buff_size = BUFFERINIT ;

	char buffer[ BUFFSIZE ] ;
	char * d ;
	char * f ;

	if( s == SocketVoid ){
		return -1 ;
	}

	f = malloc( sizeof( char ) * buff_size ) ;

	if( f == NULL ){
		_SocketError() ;
		return -1 ;
	}else{
		fd = s->fd ;
		while( 1 ){
			result = read( fd,buffer,BUFFSIZE ) ;

			if( result <= 0 ){

				if( total ){
					break ;
				}else{
					free( f ) ;
					if( result < 0 ){
						return -1 ;
					}else{
						return 0 ;
					}
				}
			}else{
				d = __expandBuffer( f,total + result,&buff_size ) ;

				if( d == NULL ){
					free( f ) ;
					_SocketError() ;
					return -1 ;
				}else{
					f = d ;

					memcpy( f + total,buffer,result ) ;
					total = total + result ;

					if( total >= len ){
						total = len ;
						break ;
					}
				}
			}
		}

		if( total ){
			d = realloc( f,total + 1 ) ;
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
}

ssize_t SocketSendData( socket_t s,const char * buffer,size_t len )
{
	if( s == SocketVoid || buffer == NULL || len == 0 ){
		return -1 ;
	}else{
		return write( s->fd,buffer,len ) ;
	}
}
