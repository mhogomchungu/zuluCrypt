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

int volume_info(  const char * mapper  )
{
	char * output ;	
	
	int xt ;
	
	struct stat st;
	
	StrHandle * p = String(  mapper  ) ;
	
	/*
	 * It appears that cryptsetup creates two mappers when a volume name has bash special characters in them.
	 * One mapper will have the special character(bogus mapper) and another with '_'(the one in use)
	 * making it hard to predict what mapper will be created and used.
	 * 
	 * Manual substitution before cryptsetup does it will give us a better control of cryptsetup behavior. 
	 */
	StringReplaceCharString( p,'_',"#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ) ;
	
	StringPrepend( p,"/dev/mapper/zuluCrypt-" ) ;
	
	if(  stat(  StringContent( p ),&st ) != 0  ) {
		
		printf( "%s is inactive\n",StringContent( p ) ) ;
		
		xt =  1 ;
	}else{
		
		output = status(  StringContent( p )  ) ;
		
		printf( "%s\n",output );
		
		free( output ) ;
		
		xt = 0 ;
	}
	
	StringDelete( p );
	
	return xt ;
}

