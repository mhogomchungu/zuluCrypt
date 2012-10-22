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
#include <sys/time.h>
#include <fcntl.h>

#define SocketVoid     ( ( void * ) 0 )  

typedef struct SocketType_t * socket_t ;

/*
 * creates a local unix socket
 * SocketVoid is returned on error
 * socket is created with default type of SOCK_STREAM and protocol value of 0
 */
socket_t SocketLocal( const char * address ) ;

/*
 * creates a local unix socket
 * type is the second argument to socket()
 * protocol is the third argument to socket()
 * SocketVoid is returned on error
 */
socket_t SocketLocalWithOptions( const char * address,int type,int protocol ) ;

/*
 * create an internet socket with an address of "address" and at port number "port"
 * eg socket_t socket = SocketNetByName( "google.com",80 ) ;
 * 
 * SocketVoid is returned on error 
 * socket is created with default type of SOCK_STREAM and protocol value of 0 
 */
socket_t SocketNetByName( const char * address,int port ) ;

/*
 * create an internet socket with an address of "address" and at port number "port"
 * eg socket_t socket = SocketNetByName( "google.com",80 ) ;
 * 
 * type is the second argument to socket()
 * protocol is the third argument to socket()
 * 
 * SocketVoid is returned on error 
 */
socket_t SocketNetByNameWithOptions( const char * address,int port,int type,int protocol ) ;

/*
 * create an internet socket with an IP address of "address" and at port number "port"
 * eg socket_t socket = SocketNetByName( "127.0.0.1",80 ) ; 
 * 
 * SocketVoid is returned on error 
 * socket is created with default type of SOCK_STREAM and protocol value of 0 
 */
socket_t SocketNetByIPAddress( const char * address,int port ) ;

/*
 * return an address associated with the socket.
 * 
 * NULL is returned if SocketVoid is passed or on error
 */
const char * SocketAddress( socket_t ) ;

/*
 * bind a socket. This will make a socket a server socket
 * 1 is returned on success
 * 0 is returned on error
 */
int SocketBind( socket_t ) ;

/*
 * return a file descriptor associated with a socket
 */
int SocketFileDescriptor( socket_t ) ;

/*
 * Accept a socket
 * SocketVoid is returned on error
 */
socket_t SocketAccept( socket_t ) ;

/*
 *  Wait for upto time seconds for an attempt to connect.
 *  SocketVoid is returned if time time expire or on error.
 *  A socket_t object is returned on success.
 */
socket_t SocketAcceptWithTimeOut( socket_t,int time ) ;

/*
 * connect to a server
 * 1 is returned on success
 * 0 is returned on error
 */
int SocketConnect( socket_t ) ;

/*
 * listen for an incomming connection
 * 1 is returned on success
 * 0 is returned on error
 */
int SocketListen( socket_t ) ;

/*
 * get data from socket and allocate memory dynamically for it and return a pointer to the allocated memory through pointer "buffer"
 * 
 * on success,the returned value is greater than 0 and the buffer should be free() when done with it.
 * on error,the returned value is 0 and no memory is allocated
 * 
 * get only len number of bytes.
 * 
 * The buffer is NULL terminated
 * 
 * This function will block until len bytes are read on the other side close the connection
 * 
 * eg
 * char * data ;
 * socket_t client = SocketNetByIPAddress( "127.0.0.1",1331 ) ;
 * SocketConnect( client ) ;
 * SocketGetData( client,&data,32 ) ;
 * puts( data ) ;
 * free( data ) ;
 * SocketClose( client ) ;
 * SocketDelete( &clien ) ;
 * 
 * returned value is the number of bytes read.
 */
size_t SocketGetData( socket_t,char ** buffer,size_t len ) ;

/*
 * get an unbounded amount of data from socket,create memory for it dynamically and return a pointer to the allocated memory through pointer "buffer"
 * 
 * on success,the returned value is greater than 0 and the buffer should be free() when done with it.
 * on error,the returned value is 0 and no memory is allocated
 * returned value is the number of bytes read
 * The buffer is NULL terminated
 */
size_t SocketGetData_1( socket_t,char ** buffer ) ;

/*
 * get data from a socket and put in a user managed buffer "buffer" of size len
 * 
 * This function will block until len bytes are read on the other side close the connection 
 * 
 * returned value is the number of bytes read or -1 on error
 * The buffer is not NULL terminated 
 */
ssize_t SocketGetData_2( socket_t,char * buffer,size_t len ) ;

/*
 * send data to a socket,
 * return number of bytes sent or -1 on error
 */
ssize_t SocketSendData( socket_t,const char * buffer,size_t len ) ;

/*
 * set the socket to not block the calling thread waiting for events over the socket
 * -1 is returned on error, other numbers on success
 */
int SocketSetDoNotBlock( socket_t ) ;

/*
 * set the socket to a block the calling thread waiting for events over the socket
 * This is the default setting.
 * -1 is returned on error, other numbers on success
 */
int SocketSetBlock( socket_t ) ;

/*
 * return 1 if a socket is a blocking socket
 * return 0 if a socket is a non blocking socket 
 * return -1 on error( if socket is SocketVoid ).
 */
int SocketIsBlocking( socket_t ) ;

/*
 * close the connection
 * 1 is returned on success
 * 0 is returned on error
 */
int SocketClose( socket_t ) ;

/*
 * set the maximum number of connections to accept.
 * This function must be called before SocketListen() to change the default from 1.
 */
void SocketSetListenMaximum( socket_t,int ) ;

/*
 * clean up all resources used by the socket
 */
void SocketDelete( socket_t * ) ;

/*
examples:
 
a server program that listens for new connections and print data they send until it receices "exit" message

#include "socket.h"
#include <string.h>
#include <stdio.h>

int main( void )
{
	//socket_t server = SocketLocal( "/tmpt/socketLocal" ) ;
	
	socket_t server = SocketNetByName( "localhost",4000 ) ;
	socket_t client ;
	
	SocketBind( server ) ;
	
	char data[ 33 ] ;
	int p ;
	while( 1 ){
		SocketListen( server ) ;
	
		client = SocketAccept( server ) ;
	
		p = SocketGetData_2( client,data,32 ) ;
		data[ p ] = '\0' ;
		if( strcmp( data,"exit" ) == 0 ){
			printf( "exit command received, exiting\n" ) ;
			SocketClose( client ) ;
			SocketDelete( &client ) ;
			break ;
		}else{	
			printf( "message received: %s\n",data ) ;
			SocketClose( client ) ;
			SocketDelete( &client ) ;
		}
	}
	
	SocketClose( server ) ;	
	SocketDelete( &server ) ;
	
	return 0 ;
}

A client program that sends a message to a server

#include "socket.h"
#include <string.h>
#include <stdio.h>

int main( int argc,char * argv[] )
{
	if( argc != 2 )
		return printf( "wrong number of arguments,expecting one argument\n" ) ;
	
	const char * data = argv[ 1 ] ;
	
	//socket_t client = SocketLocal( "/tmpt/socketLocal" ) ;	
	socket_t client = SocketNetByName( "localhost",4000 ) ;
	
	SocketConnect( client ) ;	
	
	SocketSendData( client,data,strlen( data ) ) ;
	
	SocketClose( client ) ;
	
	SocketDelete( &client ) ;
	
	return 0 ;
}
*/

#ifdef __cplusplus
}
#endif

#endif
