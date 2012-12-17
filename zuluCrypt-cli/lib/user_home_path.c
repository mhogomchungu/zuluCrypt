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

#include <pwd.h>
#include "includes.h"

string_t zuluCryptGetUserHomePath( uid_t uid ) 
{
	struct passwd * pass = getpwuid( uid ) ;
	string_t p = String( pass->pw_dir ) ;
	StringAppend( p,"/" ) ;
	return p ;
}

string_t zuluCryptGetUserName( uid_t uid ) 
{
	struct passwd * pass = getpwuid( uid ) ;
	return String( pass->pw_name ) ;
}