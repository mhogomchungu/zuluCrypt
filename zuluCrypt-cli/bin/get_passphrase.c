/*
 * 
 *  Copyright ( c ) 2012
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
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

string_t get_passphrase( void )
{	
	/* I got the big chunk of this code from: http://www.gnu.org/s/hello/manual/libc/getpass.html */
	
	const char * msg = "failed to turn off echoing to mask passphrase,bailing out with \"Nil\" as a passphrase" ;
	char c ;
	string_t p ;
	struct termios old ;
	struct termios new ;
	int i ;
	
	/* Turn echoing off and fail if we can't. */
	if ( tcgetattr ( 1,&old ) != 0 ){
		printf( msg ) ;
		return String( "Nil" ) ;
	}
	
	new = old;
	new.c_lflag &= ~ECHO;
	
	if ( tcsetattr ( 1,TCSAFLUSH,&new ) != 0 ){
		printf( msg ) ;		
		return String( "Nil" ) ;
	}
	
	p = String( "" ) ;
	
	/*
	 * interactive passphrase can have a maximum of 512 characters because its cryptsetup default
	 */
	for( i = 0 ; i < 512 ; i++ ){	
		
		c = getchar() ;		
		if( c == '\n' )
			break ;
		
		StringAppendChar( p,c ) ;
	}
	
	( void ) tcsetattr ( 1,TCSAFLUSH,&old );
	
	return p ;
}
