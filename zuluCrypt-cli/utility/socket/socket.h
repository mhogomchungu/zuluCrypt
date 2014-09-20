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

typedef struct SocketType_t * socket_t ;

#define SocketVoid ( ( socket_t ) 0 )

/*
 * Takes a pointer to a function to be called when memory allocation can not take place
 * ie if the system has run out of memory and malloc() or realloc() has failed.
 * This function is optional and "SocketVoid" will be returned on memory exaustion if the function
 * isnt set.
 */
void SocketExitOnMemoryExaustion( void (*)( void ) ) ;

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
 * create an IPv4 internet socket with an address of "address" and at port number "port"
 * eg socket_t socket = SocketNetByName( "google.com",80 ) ;
 *
 * address could be something like "google.com" or "192.168.1.1"
 *
 * SocketVoid is returned on error
 * socket is created with default type of SOCK_STREAM and protocol value of 0
 */
socket_t SocketNet( const char * address,int port ) ;

/*
 * create an internet socket with an address of "address" and at port number "port"
 * eg socket_t socket = SocketNetByName( "google.com",80 ) ;
 *
 * type is the second argument to socket()
 * protocol is the third argument to socket()
 *
 * SocketVoid is returned on error
 */
socket_t SocketNetWithOptions( const char * address,int port,int type,int protocol ) ;

/*
 * create an IPv6 internet socket with an address of "address" and at port number "port"
 * eg socket_t socket = SocketNet6( "google.com",80 ) ;
 *
 * address could be something like "google.com"
 *
 * SocketVoid is returned on error
 * socket is created with default type of SOCK_STREAM and protocol value of 0
 */
socket_t SocketNet6( const char * address,int port ) ;

/*
 * create an IPv6 internet socket with an address of "address" and at port number "port"
 * eg socket_t socket = SocketNetWithOptions6( "google.com",80,SOCK_STREAM,0 ) ;
 *
 * type is the second argument to socket()
 * protocol is the third argument to socket()
 *
 * SocketVoid is returned on error
 */
socket_t SocketNetWithOptions6( const char * address,int port,int type,int protocol ) ;

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
socket_t SocketAcceptWithTimeOut( socket_t,time_t time ) ;

/*
 * connect to a server a client socket is to connect to
 * 1 is returned on success
 * 0 is returned on error and the socket is invalidated, a new socket will have to be create to repeat
 * the operation
 */
int SocketConnect( socket_t * client ) ;

/*
 * listen for an incomming connection
 * 1 is returned on success
 * 0 is returned on error
 */
int SocketListen( socket_t ) ;

/*
 * get bounded amount of data from socket and allocate memory dynamically for it and return a pointer to the allocated memory through pointer "buffer"
 *
 * on success,the returned value is greater than 0 and the buffer should be free() when done with it.
 * on error,the returned value is 0 and no memory is allocated
 *
 * get only len number of bytes.
 *
 * The buffer is NULL terminated
 *
 * It is the responsibility of the caller to free() the buffer
 * This function will block until len bytes are read on the other side close the connection
 *
 * eg
 * char * data ;
 * socket_t client = SocketNetByIPAddress( "127.0.0.1",1331 ) ;
 * SocketConnect( client ) ;
 * SocketGetData_1( client,&data,32 ) ;
 * puts( data ) ;
 * free( data ) ;
 * SocketClose( client ) ;
 * SocketDelete( &clien ) ;
 *
 * returned value is the number of bytes read or -1 on error
 */
ssize_t SocketGetData_1( socket_t,char ** buffer,size_t len ) ;

/*
 * get an unbounded amount of data from socket,create memory for it dynamically and return a pointer to the allocated memory through pointer "buffer"
 *
 * on success,the returned value is greater than 0 and the buffer should be free() when done with it.
 * on error,the returned value is 0 and no memory is allocated
 * returned value is the number of bytes read or -1 on error
 * The buffer is NULL terminated
 *
 * It is the responsibility of the caller to free() the buffer
 */
ssize_t SocketGetData( socket_t,char ** buffer ) ;

/*
 * get data from a socket and put in a user managed buffer "buffer" of size len
 * The buffer will be null terminated and hence must have additional space to accomodate null character.
 * If you expect to read a maximum of 32 bytes from the socket for example, you must have a buffer with atleast 33 blocks in size
 * and you must pass 33 to the function.
 * returned value is the number of bytes read or -1 on error
 * The buffer is not NULL terminated
 */
ssize_t SocketGetData_2( socket_t,char * buffer,size_t len ) ;

/*
 * this function behaves like SocketGetData_2 but it takes an additional argument
 * of time in seconds for how long the function should wait for data and it returns
 * -1 if timer expired
 */
ssize_t SocketGetData_3( socket_t,char * buffer,size_t len,int timeout ) ;

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
 * close a socket and free up all used resources
 */
void SocketClose( socket_t * ) ;

/*
 * close the write channel on the socket
 */
void SocketCloseWriteChannel( socket_t ) ;

/*
 * close the read channel on the socket
 */
void SocketCloseReadChannel( socket_t ) ;

/*
 * set the maximum number of connections to accept.
 * This function must be called before SocketListen() to change the default from 1.
 */
void SocketSetListenMaximum( socket_t,int ) ;

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
	while( 1 ){
		SocketListen( server ) ;

		client = SocketAccept( server ) ;

		SocketGetData_2( client,data,sizeof( data ) ) ;
		if( strcmp( data,"exit" ) == 0 ){
			printf( "exit command received, exiting\n" ) ;
			SocketClose( &client ) ;
			break ;
		}else{
			printf( "message received: %s\n",data ) ;
			SocketClose( &client ) ;
		}
	}

	SocketClose( &server ) ;

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

	SocketClose( &client ) ;
	return 0 ;
}
*/

#ifdef __cplusplus
}
#endif

#endif
