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

/*
 * vfat volumes are mounted with root:user ownership and with 755 permissions and they cant be changed.
 * 
 * remounting the partition doesnt seem to work so, the partition is first unmounted and then remounted
 * with proper permissions.
 * 
 */
void vfat( StrHandle * q , const char * map, uid_t id, const char *  mode )
{	
	StrHandle * p ;
	
	FILE * f ;	
	
	char buffer[256] ;	
	
	int k = strlen( map ) ;
	
	char * c ;
	
	f = fopen( "/etc/mtab","r" ) ;
	
	while( fgets( buffer,256,f ) != NULL ){
		
		if( strncmp( buffer,map, k )  == 0 ){
			
			c = buffer + k + 1 ;
			
			while( *c++ != ' ' ) { ; }
			
			if( strncmp( c ,"vfat", 4 ) == 0 ){
				
				p = String( ZULUCRYPTumount ) ;
				
				StringAppend( p, " " ) ;
				
				StringAppend( p , map ) ;
				
				execute( StringContent( p ),NULL,0 ) ;
				
				StringDelete( p ) ;
				
				p = String( ZULUCRYPTmount ) ;
				
				StringAppend( p , " -o dmask=007,uid=" ) ;
				
				StringAppend( p , intToString(buffer, 10, id )) ;
				
				StringAppend( p , ",gid=" ) ;
				
				StringAppend( p , intToString(buffer, 10, id )) ;
				
				StringAppend( p , " " ) ;
				
				StringReplaceString( q ,ZULUCRYPTmount, StringContent( p ) ) ;
				
				execute( StringContent( q ),NULL,0 ) ;
				
				StringDelete( p ) ;
			}			
			break ;
		}		
	}
	fclose( f );
}

int mount_volume( const char * mapper,const char * m_point,const char * mode,uid_t id )
{
	StrHandle * p ;
	
	StrHandle * q ;
	
	StrHandle * x ;
	
	char s[2] ;
	
	p = String( m_point ) ;		
	
	int h ;
	
	if ( mkdir( StringContent( p ), S_IRWXU  ) != 0 ){
		
		StringAppend( p, ".zc") ;
		
		if ( mkdir( StringContent( p ),S_IRWXU  ) != 0 ){
			
			close_mapper( mapper );
			
			StringDelete( p ) ;
			
			return 5 ;		
		}
	}
	
	if ( strncmp( mode, "ro",2 ) == 0 )
		q = String( ZULUCRYPTmount " -r " ) ;
	else
		q = String( ZULUCRYPTmount " -w " ) ;
	
	StringAppend( q , mapper ) ;
	
	StringAppend( q , " " );
	
	x = String( StringContent( p ) ) ;
	
	StringInsertCharString( x,'\\',"#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ) ;
	
	StringAppend( q , StringContent( x ) ) ;
	
	StringAppend( q , "  2>/dev/null 1>&2 ; " ) ;
	
	StringAppend( q , ZULUCRYPTecho ) ;
	
	StringAppend( q , " $?" );
	
	execute( StringContent( q ),s,1 ) ;	
	
	if( s[0] != '0' ){
		
		remove( StringContent( p ) ) ;
		
		close_mapper( mapper );
		
		h = 4 ;
		
	}else{
		
		chown( StringContent( p ), id, id ) ;
		
		chmod( StringContent( p ), S_IRWXU ) ;
		
		vfat( q ,mapper,id,mode ) ; 
		
		h = 0 ;
	}
	
	StringDelete( p ) ;
	
	StringDelete( q ) ;
	
	StringDelete( x ) ;
	
	return h ;		
}

