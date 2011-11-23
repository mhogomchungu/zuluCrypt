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

int unmount_volume( const char * map )
{
	StrHandle * p ;
	
	StrHandle * q ;
	
	StrHandle * z ;
	
	struct stat st ;		
	
	char * c ;
	
	char * d = NULL ; 
	
	FILE * f ;	
	
	char buffer[256] ;
	
	int h ;
	
	if ( stat( map , &st ) != 0 )
		return 1 ;		
	
	/*
	 * Go through /etc/mtab to found out mount point so that we can delete the mount point folder
	 * when unmounting succeed 
	 */
	f = fopen( "/etc/mtab","r" ) ;
	
	while( fgets( buffer,256,f ) != NULL ){
		
		if( strncmp( buffer,map,strlen( map ) )  == 0 ){
			
			c = d = buffer + strlen( map ) + 1  ;
			
			while ( *++c != ' ' ) { ; }
			
			*c = '\0' ;	
			
			break ;
		}		
	}
	fclose( f );
	
	if ( d == NULL )
		return 3 ;			
	
	p = String( d ) ;	
	
	/*
	 * a space character in /etc/mtab file is stored as "\040".
	 * replace these characters with space character again	  
	 */	
	StringReplaceString( p,"\\040"," " ) ;	
	
	z = String( StringContent( p ) ) ;
	
	/*
	 * Escape all characters that triggers bash intelligence to prevent it
	 * from "acting crazy" when a user uses a creative path name
	 */
	StringInsertCharString( z,'\\',"#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ) ;
	
	/*
	 * mount/umount system calls do not add entries in mtab and 
	 * tools like kdiskfree do not see the volumes when they dont have
	 * entries in /etc/mtab and manually adding/removing entries 
	 * in /etc/mtab corrupts the file.
	 * 
	 * workaround is to use the mount/umount executables through popen to mount/unmount volumes and
	 * deal with popen/bash issues.
	 */
	
	q = String( ZULUCRYPTumount ) ;
	
	StringAppend( q, " " );
	
	StringAppend( q, StringContent( z ) ) ;
	
	StringAppend( q, "  1>/dev/null 2>&1 ; " ) ;
	
	StringAppend( q,ZULUCRYPTecho ) ;
	
	StringAppend( q, " $?" ) ;
	
	execute( StringContent( q ),buffer,1 ) ;
	
	if( buffer[0] != '0' )		
		h = 2 ;			
	else{
		rmdir( StringContent( p ) ) ;	
		h = 0 ;
	}		
	
	StringDelete( p ) ;	
	
	StringDelete( q ) ;
	
	StringDelete( z ) ;
	
	return h ;
}
