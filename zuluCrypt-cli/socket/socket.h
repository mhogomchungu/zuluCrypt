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
 

#ifndef SOCKET
#define SOCKET

#ifdef __cplusplus
extern "C" {
#endif	

#include <sys/types.h>         
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

typedef struct Socket_t * socket_t ;

socket_t SocketLocal( const char * address ) ;

socket_t Socket( const char * domain ) ;

const char * SocketAddress( socket_t ) ;

void SocketSetPortNumber( socket_t,int port ) ;

void SocketSetHostAddress( socket_t,const char * address ) ;

void SocketSetHostIPAddress( socket_t,const char * address ) ;

int SocketBind( socket_t ) ;

socket_t SocketAccept( socket_t ) ;

int SocketConnect( socket_t ) ;

int SocketListen( socket_t ) ;

size_t SocketGetData( socket_t,char ** buffer,size_t len ) ;

ssize_t SocketSendData( socket_t,const char * buffer,size_t len ) ;

void SockectSetReadOption( socket_t,int option ) ;

void SocketSetWriteOption( socket_t,int option ) ;

int SocketClose( socket_t ) ;

void SocketSetListenMaximum( socket_t,int ) ;

void SocketDelete( socket_t * ) ;

void SocketSetOptionType( socket_t,int option ) ;

void SocketSetProtocolType( socket_t,int protocol ) ;


#ifdef __cplusplus
}
#endif

#endif
