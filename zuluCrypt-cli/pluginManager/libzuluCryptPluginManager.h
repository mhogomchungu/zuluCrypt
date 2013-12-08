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

#ifndef ZULUCRYPTPLUGINMANAGER
#define ZULUCRYPTPLUGINMANAGER

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * This header and its associated library with the same name provide a plugin infrastructure
 * that allow passage of keys from a key module to zuluCrypt-cli in a safe way.
 *
 * In your key module, call functions in this header file to pass the key to zuluCrypt-cli
 *
 * The following options will be passed to the module.
 * argv[0] - the name of the module
 * argv[1] - the device path with the encrypted volume to be opened
 * argv[2] - the UUID of the encrypted volume or "Nil" if the encrypted volume has no UUID( plain volume )
 * argv[3] - a token to be used for communication btw the plugin and zuluCrypt-cli.
 * argv[4] - the maximum number of bytes that will be read.
 * argv[5] - the command line argument list as presented to zuluCrypt-cli
 */

/*
 * This function opens a connection btw the plugin and zuluCrypt-cli.
 *
 * This function is expected to be called within 10 seconds of the plugin life or the connection will be closed.
 * The time out is there to make sure the backend doesnt hang waiting for a plugin that could not be started.
 *
 * NULL is returned if the connection can not be made.
 * This function will block while trying to establish a connection.
 */
void * zuluCryptPluginManagerOpenConnection( const char * token ) ;

/*
 * This function sends the key to zuluCrypt-cli through a connection established by the command above.
 * The first argument is a handle returned above
 * The second argument is a buffer to the key to be sent
 * The third argument is the length of the buffer
 */
ssize_t zuluCryptPluginManagerSendKey( void * handle,const char * key,size_t length ) ;

/*
 * Close the connection and free up all used memory.
 */
void zuluCryptPluginManagerCloseConnection( void * handle ) ;

/*
 * Sample plugin on how to use the library
 *
 * #include <zuluCrypt/libzuluCryptPluginManager.h>
 * #include <stdlib.h>
 *
 * int main( int argc,char * argv[] )
 * {
 * 	const char * exe    = argv[0] ;
 * 	const char * device = argv[1] ;
 *      const char * uuid   = argv[2] ;
 * 	const char * token  = argv[3] ;
 * 	int len             = atoi( argv[4] ) ;
 * 	const char * arg    = argv[ 5 ] ;
 *
 * 	void * handle = zuluCryptPluginManagerStartConnection( token ) ;
 *
 *	zuluCryptPluginManagerSendKey( handle,"xyz",3 ) ;
 *
 * 	zuluCryptPluginManagerCloseConnection( handle ) ;
 *
 * 	return 0 ;
 * }
 */

#ifdef __cplusplus
}
#endif

#endif
