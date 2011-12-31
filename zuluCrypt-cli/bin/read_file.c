 /*
  * 
  *  Copyright (c) 2011
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
 
int read_file(char ** content,char * path)
{
	struct stat st;
	
	char * c ;
	
	int z ;
	
	if( stat( path, &st ) != 0 )
		return 1 ;	
	
	c = ( char * ) malloc ( sizeof( char ) * ( st.st_size + 1 ) ) ;				
	
	if( c == NULL )
		return 2 ;		
	
	*( c + st.st_size ) = '\0' ;				
	
	z = open( path, O_RDONLY ) ;				
	
	read(  z, c, st.st_size  ) ;				
	
	close( z ) ;	
	
	*content = c ;
	
	return 0 ;
}
