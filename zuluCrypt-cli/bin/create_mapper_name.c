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

#include "includes.h"

/*
 * UID number is a 16 bit number and hence it takes 5 digits,6 should be more than enough.
 */
#define UID_SIZE 6

/*
 * This function is responsible for creating a mapper name,the mapper name will show up at "/dev/mapper" if the volume
 * is successfully opened.
 * 
 * mapping_name is taken from a path to a volume presented by the user and then "zuluCrypt-XXX-YYYY is added to it to give
 * a mapper name unique to zuluCrypt.
 * 
 * XXX is the UID of the user that run the program,YYYY tells if the volume uses UUID or not."NAAN" pads the volume length
 * to make sure the length of "zuluCrypt-XXX-YYYY" is always the same regardless of what the user provided.
 * 
 * XXX is there for security reason.It makes sure one user can not manage another user's mappers 
 */

string_t create_mapper_name( const char * mapping_name,uid_t uid,int i )
{
	char uid_s[ UID_SIZE ] ;
	char * c = StringIntToString( uid_s,UID_SIZE,uid ) ;
	
	string_t p ;
	
	if( i == OPEN )
		p = String( "zuluCrypt-" ) ;
	else
		p = String( "/dev/mapper/zuluCrypt-" ) ;
	
	StringAppend( p,c ) ;
	
	if( strncmp( mapping_name,"UUID-",5 ) != 0 )
		StringAppend( p,"-NAAN-" ) ;
	else
		StringAppend( p,"-" ) ;	
	
	StringAppend( p,mapping_name ) ;
	
	replace_bash_special_chars( p ) ;
	
	return p ;
}