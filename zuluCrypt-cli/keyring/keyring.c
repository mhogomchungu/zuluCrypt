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

#include <zuluCrypt/zuluCryptPluginManager.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gnome-keyring.h>

static GnomeKeyringPasswordSchema LPS =
{
	GNOME_KEYRING_ITEM_GENERIC_SECRET,
	{
		{ "gvfs-luks-uuid", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
		{ NULL,0 }
	}
};

int main( int argc,char * argv[] )
{
	//const char * exe    = argv[ 0 ] ;
	//const char * device = argv[ 1 ] ;
	const char * uuid   = argv[ 2 ] ;
	const char * addr   = argv[ 3 ] ;
	//int          size   = atoi( argv[ 4 ] ) ;	
	
	int i ;
	
	char UUID[ 65 ] ;
	
	//const char * msg ;
	const char * e ;	
	char * f ;
	
	void * handle ;
	
	gchar * key ;
	GnomeKeyringResult r ;
	
	if( strcmp( uuid,"Nil" ) == 0 )
		return 1 ;
	
	strcpy( UUID,"luks-" ) ;
	
	e = uuid + 6 ;
	f = UUID + 5 ;
		
	for( i = 0 ; i < 65 ; i++ ){
		
		if( e[i] == '\"' ){
			f[i] = '\0' ;
			break ;
		}else{
			f[i] = e[i] ;		
		}
	}
	
	handle = zuluCryptPluginManagerStartConnection( addr ) ;
	
	r = gnome_keyring_find_password_sync( &LPS,&key,"gvfs-luks-uuid",UUID,NULL ) ;
	
	if( r == GNOME_KEYRING_RESULT_OK ){
		zuluCryptPluginManagerSendKey( handle,( char * )key,strlen( ( char * )key ) ) ;
		gnome_keyring_free_password( key ) ;
		i = 0 ;
	}else{
		//msg = ( const char * )gnome_keyring_result_to_message( r );
		//printf( "failed to get key from keyring, reason:%s\n",msg ) ;		 
		i = 1 ;
	}
	
	zuluCryptPluginManagerCloseConnection( handle ) ;
	
	return i ;
}
