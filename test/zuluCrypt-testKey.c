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

#include "../zuluCrypt-cli/pluginManager/libzuluCryptPluginManager.h"
#include <string.h>

int main( int argc,char * argv[] )
{
	/*
	 * const char * exe    = argv[ 0 ] ;
	 * const char * device = argv[ 1 ] ;
	 * const char * uuid   = argv[ 2 ] ; 
	 * int          size   = atoi( argv[ 4 ] ) ;
	 * 
	 * argv[ 5 ] is argument list given to zuluCrypt-cli
	 * const char * argv   = argv[ 5 ] ;
	 * const char * msg ; 
	 */	
	
	const char * key = "xyz" ;
	void * handle ;
	
	if( argc < 3 )
		return 1 ;
	
	handle = zuluCryptPluginManagerOpenConnection( argv[ 3 ] ) ;
	if( handle ){
		zuluCryptPluginManagerSendKey( handle,key,strlen( key ) ) ;
		zuluCryptPluginManagerCloseConnection( handle ) ;
		return 0 ;
	}else{
		return 1 ;
	}
} 
