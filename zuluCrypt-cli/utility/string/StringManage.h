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

#ifndef STRINGMANAGE
#define STRINGMANAGE

#ifdef __cplusplus
extern "C" {
#endif	

#include <stdlib.h>
#include "String.h"

/*
 * this module will give what in C++ would be vector<string_t>
 * 
 * the module basically a container module of string_t objects
 * 
 * All argument that takes a pointer to string_t invalidates the object
 * becase the function in question takes ownership of the object 
 */

/*
 * hide the pointer nature of the handles.
 * They are supposed to be opaque, using "NULL" to show they are
 * not being used gives them away.
 */
#define StringManageVoid ( ( void * ) 0 ) 

typedef struct StringManage_t * stringManage_t ;

/*
 * initialize the structure and give its maximum length.
 * The length is given before hand so that the array can be contructed before hand.
 * This is necessary because we dont want objects to move around in memory in the size was increased dynamically
 */
stringManage_t StringManage( size_t ) ;

/*
 * gives a pointer to the next available slot in the array structure for assignment
 * Useful when multiple assignment is designed at the same time. 
 * 
 * possible use case:
 * stringManage_t stm = StringManage( 10 ) ;
 * string_t * p = StringManageAssign() ;
 * string_t * q = StringManageAssign() ; * 
 * *p = String( "abc" ) ;
 * *q = String( "def" ) ;
 * 
 * same thing can be done with:
 * 
 * stringManage_t stm = StringManage( 10 ) ;
 * string_t p ; 
 * p = String( "abc" ) ;
 * StringManageInsertAtLast( stm,&p ) ;
 * p = String( "def" ) ;
 * StringManageInsertAtLast( stm,&p ) ;
 * 
 * Which one to use depends on code clarity and/or a matter of preference
 */
string_t * StringManageAssign( stringManage_t ) ;

/*
 * StringDelete() will be called on all string_t handled by a handle stm
 * 
 * example:
 * stringManage_t stm = StringManage( 10 ) ;
 * ..
 * ..
 * StringManageDelete( &stm )
 */
void StringManageDelete( stringManage_t * stm ) ;

/*
 * same as above but StringClearDelete() is used
 */
void StringManageClearDelete( stringManage_t * ) ;

/*
 * return the number of string_t elements in the structure
 */
ssize_t StringManageSize( stringManage_t ) ;

/*
 * return the number of maximum string_t objects that can be stored.
 * The argument to StringManage() is returned.
 */
size_t StringManageMaxSize( stringManage_t ) ;

/*
 * return a string_t objects at a given index
 * StringVoid is returned on error
 */
string_t StringManageStringAt( stringManage_t,size_t index ) ;

/*
 * remove a string_t object at position index
 * This command invalidates all returned value of StringManageStringAt()
 * and StringManageAssign() 
 */
void StringManageRemoveAt( stringManage_t,size_t index ) ;

/*
 * insert a string_t object at position index by passing its address.
 * returned value is a string_t object now in the structure or StringVoid on error
 * This command invalidates all returned value of StringManageStringAt()
 * and StringManageAssign() 
 * 
 * example:
 * ge_t stm = StringManage( 10 ) ;
 * string_t p ; 
 * p = String( "abc" ) ;
 * StringManageInsertAt( stm,&p,0 ) ;
 * p = String( "def" ) ;
 * StringManageInsertAtLast( stm,&p,1 ) ;
 */
string_t StringManageInsertAt( stringManage_t,string_t *,size_t index ) ;

/*
 * replace whatever is at position index in the structure with the given string_t given by its address
 * returned value is a string_t object now in the structure or StringVoid on error
 * This command invalidates all returned value of StringManageStringAt()
 * and StringManageAssign() 
 */
string_t StringManageReplaceAt( stringManage_t,string_t *,size_t index ) ;

/*
 * append a string_t object to the structure.
 * returned value is a string_t object now in the structure or StringVoid on error
 * This function does not invalidate returned values of StringManageStringAt() and StringManageAssign()
 * and is the reason why a fixed size container is used.
 */
string_t StringManageInsertAtLast( stringManage_t,string_t * ) ;


#ifdef __cplusplus
}
#endif

#endif