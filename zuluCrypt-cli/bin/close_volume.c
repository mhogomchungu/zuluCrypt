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

int close_opened_volume(  char * mapping_name  )
{	
	 int st ;	 
	 char * m_point = NULL ;
	 
	 StrHandle * p = String(  mapping_name  ) ;
	 
	 StringReplaceCharString( p,'_',"#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ) ;
	 
	 StringPrepend( p,"/dev/mapper/zuluCrypt-" ) ;	
	 
	 st = close_volume( StringContent( p ), &m_point ) ;
	 
	 switch(  st  ) {
		 case 0 : printf( "SUCCESS: volume closed successfully \n" );
			  remove( m_point ) ;
			  free( m_point ) ;
			  break ;
		 case 1 : printf( "ERROR: close failed, encrypted volume with that name does not exist\n" );
			  break ;			
		 case 2 : printf( "ERROR: close failed, the mount point and/or one or more files are in use\n" );
			  break ;
		 case 3 : printf( "ERROR: close failed, volume does not have an entry in /etc/mtab\n" ) ;
			  break ;
		 case 4 : printf( "ERROR: close failed, could not get a lock on /etc/mtab~\n" ) ;
				 break ;
		 default :
			 ; //shouldnt get here			
	 }	
	 StringDelete(  p  ) ;	 
	 return st ;	
}
