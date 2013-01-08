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

#include "includes.h"
#include "../lib/includes.h"
#include <sys/types.h>
#include <string.h>
#include <dirent.h>

/*
 * This function is called when a dead mapper is noticed,a dead mapper show up when a device is removed
 * without being properly closed.
 */
static void remove_mapper( const char * path )
{
	char * m_point  ;
	/*
	 * zuluCryptPartitionIsMounted() is defined in ../lib/print_mounted_volumes.c
	 */
	if( zuluCryptPartitionIsMounted( path ) ){
		if( zuluCryptUnmountVolume( path,&m_point ) == 0 ){
			rmdir( m_point ) ;
			free( m_point ) ;
		}
	}
	
	/*
	 * zuluCryptCloseMapper() is defined in close_mapper.c
	 */
	zuluCryptCloseMapper( path ) ;
}

void zuluCryptClearDeadMappers( uid_t uid )
{
	struct crypt_device * cd ;
	const char * dir_path = crypt_get_dir() ;
	DIR * dir = opendir( dir_path ) ;
	struct dirent * entry ;
	const char * m ;
	const char * e ;
	size_t len ;
	size_t len1 ;
	
	string_t p ;
	string_t z = String( dir_path ) ;
	StringAppend( z,"/" ) ;
	len1 = StringLength( z ) ;
	
	p = String( "zuluCrypt-" );
	m = StringAppendInt( p,uid ) ;
	len = StringLength( p ) ;
	
	/*
	 * zuluCryptSecurityGainElevatedPrivileges() is defined in security.c
	 */
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	while( ( entry = readdir( dir ) ) != NULL ){
		
		if( strncmp( entry->d_name,m,len ) == 0 ){
			e = StringInsertAndDelete( z,len1,entry->d_name ) ;
			if( crypt_init_by_name( &cd,e ) == 0 ){
				if( crypt_get_device_name( cd ) == NULL ){
					remove_mapper( e ) ;
				}
				crypt_free( cd ) ;
			}else{
				remove_mapper( e ) ;
			}
		}
	}
	
	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in security.c
	 */
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	StringDelete( &p ) ;
	StringDelete( &z ) ;
	
	closedir( dir ) ;
}
