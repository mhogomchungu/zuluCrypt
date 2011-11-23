/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

char * status( const char * mapper )
{		
	#define SIZE 31
	
	char buffer[ SIZE + 1 ] ;
	
	char loop[512] ;
	
	char path[512];
	
	char * c = NULL ;
	
	char * d = NULL ;
	
	const char * e ;
	
	int i ;
	
	struct crypt_device * cd1 = NULL;
	
	struct crypt_device * cd;
	
	crypt_status_info csi ;
	
	struct crypt_active_device cad ;	
	
	StrHandle * p ;
	
	StrHandle * q ;
	
	i = crypt_init_by_name( &cd,mapper );
	
	//if( i != 0 ){
		//	c = NULL ;
	//	goto out ;
	//}
	
	i = crypt_get_active_device( cd1,mapper,&cad ) ;
	
	//if( i != 0 ){
		//	c = NULL ;
	//	goto out ;
	//}
	
	p = String( mapper ) ;
	
	csi = crypt_status( cd, mapper );	
	
	switch( csi ){
		case CRYPT_INACTIVE :
			StringAppend( p," is inactive.\n" ) ; 	
			goto out ;
		case CRYPT_ACTIVE   : 
			StringAppend( p," is active.\n" ) ;
			break ;
		case CRYPT_BUSY     : 	
			StringAppend( p," is active and is in use.\n" ) ;
			break ;
		case CRYPT_INVALID  : 
			StringAppend( p," is invalid.\n" ) ;	
			goto out ;
	}	
	
	StringAppend( p," type:      " );
	
	e = crypt_get_type( cd ) ;
	
	if( strcmp( e,"LUKS1" ) == 0 )
		StringAppend( p,"luks1" ) ;
	else if( strcmp( e,"plain") )
		StringAppend( p,"plain" ) ;
	
	StringAppend( p,"\n cipher:    " );
	
	StringAppend( p,crypt_get_cipher_mode( cd ) ) ;
	
	StringAppend( p,"\n keysize:   " );
	
	StringAppend( p,intToString( buffer,SIZE,8 * crypt_get_volume_key_size( cd ) ) ) ;
	
	StringAppend( p," bits" );
	
	StringAppend( p,"\n device:    " );
	
	e = crypt_get_device_name( cd ) ;
	
	StringAppend( p, e ) ;
	
	if( strncmp( e ,"/dev/loop",9 ) == 0 ){
		
		q = String( ZULUCRYPTlosetup ) ;
		
		StringAppend( q," " );
		
		StringAppend( q, e ) ;
		
		execute( StringContent( q ),loop,511 ) ;
		
		StringDelete( q ) ;
		
		c = loop ;
		
		while( *c++ != '(' ) { ; }
		
		d = c ;
		
		while( *++d != ')' ) { ; }
		
		*d = '\0' ;		
		
		realpath( c,path ) ;
		
		StringAppend( p,"\n loop:      " );
		
		StringAppend( p,path );
	}
	
	StringAppend( p,"\n offset:    ");
	
	StringAppend( p,intToString( buffer,SIZE,crypt_get_data_offset( cd ) ) )  ;	
	
	StringAppend( p," sectors" ) ;	
	
	StringAppend( p,"\n size:      " );
	
	StringAppend( p,intToString( buffer,SIZE,cad.size ) ) ;	
	
	StringAppend( p," sectors" );
	
	StringAppend( p,"\n mode:      " );
	
	if( cad.flags == 1 )
		StringAppend( p,"readonly" );
	else
		StringAppend( p,"read/write" );			
	
	out:
	crypt_free( cd );	
	crypt_free( cd1 );	
	return StringDeleteHandle( p ) ;
}

