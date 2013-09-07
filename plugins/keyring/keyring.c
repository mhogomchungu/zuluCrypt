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

/*
 * http://developer.gnome.org/gnome-keyring/stable/gnome-keyring-Simple-Password-Storage.html#gnome-keyring-find-password-sync
 */

#include "../../zuluCrypt-cli/pluginManager/libzuluCryptPluginManager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <gnome-keyring.h>

static GnomeKeyringResult getKey( const char * UUID,gchar ** key )
{	
	GnomeKeyringResult r ;
	
	GnomeKeyringPasswordSchema lps ;
	
	lps.item_type = GNOME_KEYRING_ITEM_GENERIC_SECRET ;
	
	lps.attributes[0].name = "gvfs-luks-uuid" ;
	lps.attributes[0].type = GNOME_KEYRING_ATTRIBUTE_TYPE_STRING ;
	
	lps.attributes[1].name = NULL ;
	lps.attributes[1].type = 0 ;
	
	/*
	 * in some versions of gnome,uuid start with "luks-UUID"
	 */
	r = gnome_keyring_find_password_sync( &lps,key,"gvfs-luks-uuid",UUID,NULL ) ;
	
	if( r != GNOME_KEYRING_RESULT_OK ){
		/*
		 * in other versions of gnome,uuid start with "UUID"
		 */
		UUID = UUID + strlen( "luks-" ) ;
		r = gnome_keyring_find_password_sync( &lps,key,"gvfs-luks-uuid",UUID,NULL ) ;
	}
	
	return r ;
}

int main( int argc,char * argv[] )
{
	/*
	 * const char * exe    = argv[ 0 ] ;           name of this executable
	 * const char * device = argv[ 1 ] ;           path to the encrypted volume
	 * int          size   = atoi( argv[ 4 ] ) ;   key maximum length
	 * 
	 * argv[ 5 ] is argument list given to zuluCrypt-cli
	 * const char * arg_v   = argv[ 5 ] ;          command line arguments passed to zuluCrypt-cli/zuluMount-cli
	 *  
	 */
	
	const char * uuid = argv[ 2 ] ;
	const char * addr = argv[ 3 ] ;
	
	int st = 1 ;
	char UUID[ 64 ] ;
	void * handle ;
	gchar * key ;
	const char * e ;
	
	if( argc <= 3 ){
		return 1 ;
	}
	if( strcmp( uuid,"Nil" ) == 0 ){
		return 1 ;
	}
		
	strcpy( UUID,"luks-" ) ;
	strcat( UUID,uuid ) ;

	/*
	 * this function is to be called as soon as possible.
	 * It will fail if called 10 seconds after the plugin is started.  
	 */
	handle = zuluCryptPluginManagerOpenConnection( addr ) ;
	
	if( handle ){
		if( getKey( UUID,&key ) == GNOME_KEYRING_RESULT_OK ){
			e = ( const char * ) key ;
			zuluCryptPluginManagerSendKey( handle,e,strlen( e ) ) ;
			gnome_keyring_free_password( key ) ;
			st = 0 ;
		}else{
			st = 1 ;
		}
		
		zuluCryptPluginManagerCloseConnection( handle ) ;
	}else{
		st = 1 ;
	}
	
	return st ;
}
