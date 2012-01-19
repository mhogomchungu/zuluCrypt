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

/*
 * cryptsetup 1.4.1 and previous have a bug and its triggered when the mapper has one or more bash
 * special characters, this functions substitute bash special characters for an underscore to
 * work around the bug. 
*/

void replace_bash_special_chars( string_t * st )
{
	StringReplaceCharString( *st,'_',"#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ) ;
}