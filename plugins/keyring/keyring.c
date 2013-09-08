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

#include <libsecret-1/libsecret/secret.h>

static int getKey( const char * path,const char * uuid,gchar ** key )
{	
	gchar * c ;
	SecretSchema lps ;
	char UUID[ 64 ] ;
	
	memset( &lps,'\0',sizeof( SecretSchema ) ) ;
	
	lps.name  = "lxqt.Wallet.zuluCrypt.zuluCrypt" ;
	lps.flags = SECRET_SCHEMA_NONE ;
	
	lps.attributes[0].name = "string" ;
	lps.attributes[0].type = SECRET_SCHEMA_ATTRIBUTE_STRING ;
	
	lps.attributes[1].name = "NULL" ;
	lps.attributes[1].type = 0 ;
	
	if( strcmp( uuid,"Nil" ) == 0 ){
		c = secret_password_lookup_sync( &lps,NULL,NULL,"string",path,NULL ) ;
		if( c != NULL ){
			*key = c ;
			return 1 ;
		}else{
			return 0 ;
		}
	}
	
	snprintf( UUID,64,"UUID=\"%s\"",uuid ) ;
	c = secret_password_lookup_sync( &lps,NULL,NULL,"string",UUID,NULL ) ;
	
	if( c != NULL ){
		*key = c ;
		return 1 ;
	}else{
		snprintf( UUID,64,"UUID=%s",uuid ) ;
		c = secret_password_lookup_sync( &lps,NULL,NULL,"string",UUID,NULL ) ;
		if( c != NULL ){
			*key = c ;
			return 1 ;
		}else{
			c = secret_password_lookup_sync( &lps,NULL,NULL,"string",uuid,NULL ) ;
			if( c != NULL ){
				*key = c ;
				return 1 ;
			}else{
				return 0 ;
			}
		}
	}
	
	return 0 ;
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
	
	const char * path = argv[ 1 ] ;
	const char * uuid = argv[ 2 ] ;
	const char * addr = argv[ 3 ] ;
	
	int st = 1 ;
	void * handle ;
	gchar * key ;
	const char * e ;
	
	if( argc <= 3 ){
		return 1 ;
	}
	
	/*
	 * this function is to be called as soon as possible.
	 * It will fail if called 10 seconds after the plugin is started.  
	 */
	handle = zuluCryptPluginManagerOpenConnection( addr ) ;
	
	if( handle ){
		if( getKey( path,uuid,&key ) ){
			e = ( const char * ) key ;
			zuluCryptPluginManagerSendKey( handle,e,strlen( e ) ) ;
			secret_password_free( key ) ;
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
