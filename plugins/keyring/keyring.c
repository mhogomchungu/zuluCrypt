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

static GnomeKeyringPasswordSchema lps =
{
	GNOME_KEYRING_ITEM_GENERIC_SECRET,
	{
		{ "gvfs-luks-uuid", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
		{ NULL,0 }
	}
};

static GnomeKeyringResult getKey( const char * UUID,gchar ** key )
{	
	GnomeKeyringResult r ;
	/*
	 * gnome2 format,uuid start with luks-UUID
	 */
	r = gnome_keyring_find_password_sync( &lps,key,"gvfs-luks-uuid",UUID,NULL ) ;
	
	if( r != GNOME_KEYRING_RESULT_OK ){
		/*
		 * gnome3 format,uuid start with UUID
		 */
		r = gnome_keyring_find_password_sync( &lps,key,"gvfs-luks-uuid",UUID + 5,NULL ) ;
	}
	return r ;	
}

int main( int argc __attribute__(( unused )),char * argv[] )
{
	/*
	 * const char * exe    = argv[ 0 ] ;
	 * const char * device = argv[ 1 ] ;
	 * int          size   = atoi( argv[ 4 ] ) ;
	 * 
	 * argv[ 5 ] is argument list given to zuluCrypt-cli
	 * const char * arg_v   = argv[ 5 ] ;
	 * const char * msg ; 
	 */	
	
	const char * uuid = argv[ 2 ] ;
	const char * addr = argv[ 3 ] ;
	
	int i ;
	char UUID[ 64 ] ;
	void * handle ;
	gchar * key ;
	const char * e ;
		
	if( strcmp( uuid,"Nil" ) == 0 )
		return 1 ;
	
	strcpy( UUID,"luks-" ) ;
	strncpy( UUID,uuid,64 ) ;

	handle = zuluCryptPluginManagerOpenConnection( addr ) ;
	
	if( handle ){
		if( getKey( uuid,&key ) == GNOME_KEYRING_RESULT_OK ){
			e = ( const char * ) key ;
			zuluCryptPluginManagerSendKey( handle,e,strlen( e ) ) ;
			gnome_keyring_free_password( key ) ;
			zuluCryptPluginManagerCloseConnection( handle ) ;
		}
	}
	
	return i ;
}
