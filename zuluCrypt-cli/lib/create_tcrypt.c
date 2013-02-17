 
 /*
  * 
  *  Copyright (c) 2013
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

int zuluCryptCreateTCrypt( const char * dev,const char * pass,size_t pass_size,const char * rng )
{
	process_t p ;
	struct stat st ;
	int r ;
	if( rng ){;}
	
	if( stat( ZULUCRYPTtcrypt,&st ) != 0 ){
		return 1 ;
	}
	
	p = Process( ZULUCRYPTtcrypt ) ;
	ProcessSetArgumentList( p,"-z","-c","-d",dev,ENDLIST ) ;
	ProcessStart( p ) ;
	
	ProcessWrite( p,pass,pass_size ) ;
	ProcessCloseStdWrite( p ) ;
	r = ProcessExitStatus( p ) ;
	ProcessDelete( &p ) ;
	return r ;
}
