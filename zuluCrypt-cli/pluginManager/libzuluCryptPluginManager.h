
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
 * argv[1] - the device path with the encrypted volume
 * argv[2] - the UUID of the encrypted volume in a form of UUID="blablbabla"
 * argv[3] - the socket path.The address to where the local socket is created.
 * argv[4] - the maximum number of bytes that will be read. *  
 */

/*
 * The key to zuluCrypt-cli will be sent through a unix local socket.This function will create the connection on the plugin side.
 * The argument it takes is argv[3].
 * 
 * The returned value is handle to the connection to be used for subsequent calls.
 * 
 * A plugin is expected to call this function within 20 seconds of its life.
 */
void * zuluCryptPluginManagerStartConnection( const char * sockpath ) ;

/*
 * This function sends the key to zuluCrypt-cli.
 * The first argument is a handle returned above
 * The second argument is a buffer to the key to be sent
 * The third argument is the length of the buffer
 */
int zuluCryptPluginManagerSendKey( void * p,const char * key,size_t length ) ;

/*
 * Close the connection and free up all used memory.
 */
void zuluCryptPluginManagerCloseConnection( void * p ) ;

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
 * 	const char * addr   = argv[3] ;
 * 	int len             = atoi( argv[4] ) ;
 * 
 * 	void * handle = zuluCryptPluginManagerStartConnection( addr ) ;
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
