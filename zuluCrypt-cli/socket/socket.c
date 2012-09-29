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

struct Socket_t
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

socket_t SocketLocal( const char * address )
{
	socket_t s = Socket( "local" ) ;
	SocketSetHostAddress( s,address ) ;
	return s ;
}

socket_t Socket( const char * domain ) 
{
	socket_t s = ( socket_t ) malloc( sizeof( struct Socket_t ) ) ;
	
	if( s == NULL )
		return SocketVoid ;
	
	if( strcmp( domain,"local" ) == 0 ){		
		s->local = ( struct sockaddr_un * ) malloc( sizeof( struct sockaddr_un ) ) ;
		if( s->local == NULL ){
			free( s ) ;
			return SocketVoid ;
		}else{
			s->domain = AF_UNIX ;
			s->size = sizeof( struct sockaddr_un ) ;
			memset( s->local,'\0',s->size ) ;
			s->local->sun_family = AF_UNIX ;
		}
	}else if( strcmp( domain,"net" ) == 0 ){
		s->net = ( struct sockaddr_in  * ) malloc( sizeof( struct sockaddr_in ) ) ;
		if( s->net == NULL ){
			free( s ) ;
			return SocketVoid ;
		}else{
			s->domain = AF_INET ;
			s->size = sizeof( struct sockaddr_in ) ;
			memset( s->net,'\0',s->size ) ;
			s->net->sin_family = AF_INET ;
		}
	}else{
		free( s ) ;
		return SocketVoid ;
	}
	
	s->type = SOCK_STREAM ;
	s->protocol = 0 ;
	s->cmax = 1 ;
	s->socket_server = 0 ;
	s->fd = socket( s->domain,s->type,s->protocol ) ;
	return s ;	
}

void SocketSetOptionType( socket_t s,int option ) 
{
	if( s != SocketVoid )
		s->type = option ;
}

void SocketSetProtocolType( socket_t s,int protocol ) 
{
	if( s != SocketVoid )
		s->protocol = protocol ;
}

void SocketDelete( socket_t * x )
{
	socket_t s ;
	
	if( x == NULL )
		return ;
	if( *x == SocketVoid )
		return ;
	
	s = *x ;
	*x = SocketVoid ;
	
	if( s->domain == AF_UNIX ){
		free( s->local ) ;
	}else{
		free( s->net ) ;
	}
	
	free( s ) ;
}

void SocketSetPortNumber( socket_t s,int port ) 
{
	if( s != SocketVoid )
		if( s->domain == AF_INET )
			s->net->sin_port = htons( port );
}

#ifdef __STDC__
struct addrinfo {
	int     ai_flags;
	int     ai_family;
	int     ai_socktype;
	int     ai_protocol;
	size_t  ai_addrlen;
	struct  sockaddr *ai_addr;
	char    *ai_canonname;     
	struct  addrinfo *ai_next;
};

int getaddrinfo( const char *,const char *,const struct addrinfo *,struct addrinfo ** );
void freeaddrinfo( struct addrinfo * );
#endif

void SocketSetHostAddress( socket_t s,const char * address ) 
{
	size_t size ;
	struct addrinfo * addr ;
	struct sockaddr_in * addr_in ;
	
	if( s->domain == AF_UNIX ){
		size = sizeof( s->local->sun_path ) ;
		strncpy( s->local->sun_path,address,size ) ;
		s->local->sun_path[ size - 1 ] = '\0' ;
	}else{
		if( getaddrinfo( address,NULL,NULL,&addr ) == 0 ){
			addr_in = ( struct sockaddr_in * ) addr->ai_addr ;
			s->net->sin_addr.s_addr = addr_in->sin_addr.s_addr ;
			freeaddrinfo( addr ) ;
		}
	}
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

void SocketSetHostIPAddress( socket_t s,const char * address ) 
{
	if( s != SocketVoid )
		if( s->domain == AF_INET )
			s->net->sin_addr.s_addr = inet_addr( address ) ;
}

int SocketBind( socket_t s )
{
	if( s == SocketVoid )
		return -1 ;
	if( s->domain == AF_UNIX ){
		s->socket_server = 1 ;		
		unlink( s->local->sun_path ) ;
		return bind( s->fd,( struct sockaddr * )s->local,s->size ) ;
	}else{
		return bind( s->fd,( struct sockaddr * )s->net,s->size ) ;
	}
}

socket_t SocketAccept( socket_t s ) 
{
	socket_t x = ( socket_t ) malloc( sizeof( struct Socket_t ) ) ;
	
	if( x == NULL )
		return SocketVoid ;
	
	memset( x,'\0',sizeof( struct Socket_t ) ) ;
	
	if( s->domain == AF_UNIX ){
		x->local = ( struct sockaddr_un * ) malloc( sizeof( struct sockaddr_un ) ) ;
		if( x->local == NULL ){
			free( x ) ;
			x = SocketVoid ;
		}else{
			memset( x->local,'\0',sizeof( struct sockaddr_un ) ) ;
			x->fd = accept( s->fd,( struct sockaddr * )x->local,&x->size ) ;
			if( x->fd == -1 ){
				free( x->local ) ;
				free( x ) ;
				x = SocketVoid ;
			}else{
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
			x = SocketVoid ;
		}else{
			memset( x->local,'\0',sizeof( struct sockaddr_in ) ) ;	
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

void SocketSetListenMaximum( socket_t s,int m ) 
{
	if( s != SocketVoid )
		s->cmax = m ;
}

int SocketConnect( socket_t s ) 
{
	if( s == SocketVoid )
		return -1 ;
	if( s->domain == AF_UNIX )
		return connect( s->fd,( struct sockaddr * )s->local,s->size ) ;
	else
		return connect( s->fd,( struct sockaddr * )s->net,s->size ) ;
}

int SocketListen( socket_t s ) 
{
	if( s == SocketVoid )
		return -1 ;
	return listen( s->fd,s->cmax ) ;
}

#define BUFFSIZE 64
size_t SocketGetData( socket_t s,char ** buffer,size_t len ) 
{
	int fd ;
	size_t i ;
	size_t buffCount = BUFFSIZE ;
	
	char * c ;
	char * d = NULL ;
	
	if( s == SocketVoid )
		return 0 ;
	
	fd = s->fd ;
	
	c = ( char * ) malloc( sizeof( char ) * BUFFSIZE ) ;
	
	if( c == NULL )
		return -1 ;
	
	for( i = 0 ; i < len ; i++ ){
	
		if( i == buffCount ){
			
			buffCount += BUFFSIZE ;
			d = realloc( c,buffCount ) ;
			
			if( d == NULL ){
				free( c ) ;
				return 0 ;
			}else{
				c = d ;
			}
		}
		
		if( read( fd,c + i,1 ) <= 0 )
			break ;
	}	
	
	if( i > 0 ){
		d = realloc( c,i + 1 ) ;
		if( d == NULL ){
			free( c ) ;
			return 0 ;
		}else{
			*( d + i ) = '\0' ;
			*buffer = d ;
		}
	}else{
		free( c ) ;
	}

	return i ;
}

ssize_t SocketSendData( socket_t s,const char * buffer,size_t len ) 
{		
	size_t sent = 0 ;
	size_t remain = len ;
	
	if( s == SocketVoid || buffer == NULL )
		return -1 ;
	do{
		sent = sent + write( s->fd,buffer + sent,remain ) ;
		remain = remain - sent ;
	}while( sent != len );
	
	return sent ;
}

int SocketClose( socket_t s ) 
{
	int st ;
	if( s == SocketVoid )
		return -1 ;
		
	shutdown( s->fd,SHUT_RDWR ) ;
	st = close( s->fd ) ;
	if( s->domain == AF_UNIX && s->socket_server )
		unlink( s->local->sun_path ) ;
	return st ;	
}

