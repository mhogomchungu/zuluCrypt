
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
 * In your key module, call this function to pass the key to zuluCrypt-cli
 *  
 * Your key module executable will be started with 3 argument.
 * The first argument(argv[0]) is the name of your executable
 * The second argument(argv[1]) is the path to local socket used to pass the key to zuluCrypt-cli
 * The third argument(argv[2]) is a maximun number of bytes you can use to pass the key.
 *
 * Just pass the second argument as is to the first argument of zuluCryptPlugInServer.
 * pass atoi(argv[2]) to the second argument of zuluCryptPlugInServer.
 * pass a buffer with the key to the third argument of zuluCryptPlugInServer.
 *  
 */

void zuluCryptPluginManager( const char * sockpath,int size,const char * buffer ) ;

#ifdef __cplusplus
}
#endif

#endif
