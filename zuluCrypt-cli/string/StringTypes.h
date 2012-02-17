 
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
 
#ifndef STRINGTYPES__
#define STRINGTYPES__

#ifdef __cplusplus
extern "C" {
#endif	
	
#ifdef __STRING_MAKE_THREAD_SAFE
	#include <pthread.h>
	#define THREAD_SAFE 1
#else
	#define THREAD_SAFE 0
#endif

struct StringType
{
	size_t size ;
	char * string ; 
	int * rc ;
#if THREAD_SAFE	
	pthread_mutex_t * mutex ;
#endif	
};

typedef struct StringType * string_t ;

struct StringListType
{
	size_t size ;
	string_t * stp ;
};

typedef struct StringListType * stringList_t ;
#ifdef __cplusplus
}
#endif

#endif