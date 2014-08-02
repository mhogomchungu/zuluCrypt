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
#ifndef STRINGLISTTYPE
#define STRINGLISTTYPE

#ifdef __cplusplus
extern "C" {
#endif

#include "String.h"
#include <stdarg.h>

/*
 * stringList_t is a type to be used as a handle with these functions
 *
 * this library implements a an array of strings_t type dynamically and add other functionality
 *
 */
typedef struct StringListType * stringList_t ;

/*
 * create a custom type to represent a stringList_t pointing to NULL while hiding the pointer nature of stringList_t
 * string_t is an opaque handle and NULL assignment "gives unnecessary info" about its nature.
 */
#define StringListVoid ( ( stringList_t ) 0 )

/*
 * an STL style iterator type
 */
typedef string_t * StringListIterator ;

/*
 *  returns a pointer to the memory block occupying the first element on the list
 */
StringListIterator StringListBegin( stringList_t ) ;

/*
 * returns a pointer to a memory block past the end of the array,STL style
 */
StringListIterator StringListEnd( stringList_t ) ;

/*
 * set iterator begin and iterator end
 */
void StringListGetIterators( stringList_t,StringListIterator * begin,StringListIterator * end ) ;

/*
 * call a function with a signature void foo( string_t ) on each entry in the list
 */
void StringListForEach( stringList_t stl,void (*)( string_t ) ) ;

/*
 * call a function with a signature void foo( string_t,void * ) on each entry in the list
 * The third argument will be passed on to the function specified in the second argument
 */
void StringListForEach_1( stringList_t stl,void (*)( string_t,void * ),void * ) ;

/*
 * call a function with a signature void foo( const char *,void * ) on each entry in the list
 * The third argument will be passed on to the function specified in the second argument
 */
void StringListForEachString_1( stringList_t stl,void (*)( const char *,void * ),void * ) ;

/*
 * call a function with a signature void foo( const char * ) on each entry in the list
 */
void StringListForEachString( stringList_t stl,void (*)( const char * ) ) ;

/*
 * Takes a pointer to a function to be called when memory allocation can not take place
 * ie if the system has run out of memory and malloc() or realloc() has failed.
 * This function is optional and "StringListVoid" will be returned on memory exaustion if the function
 * isnt set.
 */
void StringListExitOnMemoryExaustion( void (*)( void ) ) ;

/*
 *  create stringlist handle with a C string. All stringlist operations should happen through this handle
 */
stringList_t StringList( const char * cstring ) ;

/*
 *  create stringlist handle with a string_t. All stringlist operations should happen through this handle
 *  this function will take ownership of st,invalidating the handle after auto deleting its contents
 */
stringList_t StringListString( string_t * ) ;

/*
 * create a stringList_t object with empty content
 */
stringList_t StringListInit( void ) ;

/*
 * Create a StringList object with arraySize string_t "slots" and expose them for easier assignment of multiple string_t objects.
 *
 * example use case
 * stringList_t stl ;
 * string_t * x = StringListArray( &stl,3 ) ;
 * x[0] = String( "abc" ) ;
 * x[1] = String( "def" ) ;
 * x[2] = String( "ghi" ) ;
 * ...
 * ...
 * NOTE:This is a dangerous function as it does not guard against StringVoid.
 * Assign StringVoid and segfault will surely happen sooner or later.
 */
string_t * StringListArray( stringList_t *,size_t arraySize ) ;

/*
 * Return the number of elements in the list
 */
size_t StringListSize( stringList_t stl ) ;

/*
 * create a  stringlist from splitted cstring using splitter as a splitting trigger.
 */
stringList_t StringListSplit( const char * cstring,char splitter ) ;

/*
 * add a new spot at the end of stringList_t object and return its position.
 *
 * eg usage:
 * string_t * p = StringListAssign( stl ) ;
 * *p = String( "abc" ) ;
 *
 * another way of doing the same thing:
 * string_t p = StringListAssignString( stl,String( "abc" ) ) ;
 * NOTE:This is a dangerous function as it does not guard against StringVoid.
 * Assign StringVoid and segfault will surely happen sooner or later.
 */
string_t * StringListAssign( stringList_t ) ;

/*
 * add a string_t objest to the end of stringList_t object
 * string_t object passed is returned on success,StringVoid is return otherwise
 */
string_t StringListAssignString( stringList_t,string_t ) ;

/*
 * create a stringlist from string_t using splitter as a splitting trigger.
 */
stringList_t StringListStringSplit( string_t st,char splitter ) ;

/*
 * return a string_t element at position index.
 * First element is at position 0.
 */
string_t StringListStringAt( stringList_t stl,size_t index ) ;

static __inline__ string_t StringListStringAtFirstPlace( stringList_t stl )
{
	return StringListStringAt(stl,0 ) ;
}

static __inline__ string_t StringListStringAtSecondPlace( stringList_t stl )
{
	return StringListStringAt(stl,1 ) ;
}

/*
 *  return a pointer to a string at position index.
 * First element is at position 0. *
 */
const char * StringListContentAt( stringList_t stl,size_t index ) ;

static __inline__ const char * StringListContentAtFirstPlace( stringList_t stl )
{
	return StringListContentAt( stl,0 ) ;
}

static __inline__ const char * StringListContentAtSecondPlace( stringList_t stl )
{
	return StringListContentAt( stl,1 ) ;
}

/*
 * strcmp() a strinf at index with cstring
 */
int StringListContentAtEqual( stringList_t stl,size_t index,const char * cstring ) ;

/*
 * printf() string content at a given index
 */
void StringListPrintAt( stringList_t stl,size_t index ) ;

/*
 * printf() all elements in the list,one line per element
 */
void StringListPrintList( stringList_t stl ) ;

/*
 * printf() with a new line string content at a given index
 */
void StringListPrintLineAt( stringList_t stl,size_t index ) ;

/*
 *  return a pointer to a string at the last elenent in the list.
 */
const char * StringListContentAtLast( stringList_t stl ) ;

/*
 *  return a string_t at the last elenent in the list.
 */
string_t StringListStringAtLast( stringList_t stl ) ;

/*
 *  return an index position of an element with cstring.
 *  return -1 if there is no element in the list with the string.
 */
ssize_t StringListContains( stringList_t stl,const char * cstring ) ;

/*
 * return 1 if the managed list has atleast one cstring entry
 */
static int StringListHasEntry( stringList_t stl,const char * cstring ) ;

/*
 * return 1 if the managed list has no cstring entry
 */
static int StringListHasNoEntry( stringList_t stl,const char * cstring ) ;

/*
 * return an index position of the first string_t object in the stringList_t object with a character
 * sequence str ;
 * return -1 if no such sequence exist
 */
ssize_t StringListHasSequence( stringList_t stl,const char * str ) ;

/*
 * return the first string_t object in the stringList_t object with a character
 * sequence str ;
 * return StringVoid if no such sequence exist
 */
string_t StringListHasSequence_1( stringList_t stl,const char * str ) ;

/*
 * return an index position of the first string_t object in the stringList_t object with a character
 * sequence that starts with str ;
 * return -1 if no such sequence exist
 */
ssize_t StringListHasStartSequence( stringList_t stl,const char * str ) ;

/*
 * return the first string_t object in the stringList_t object with a character
 * sequence that starts with str ;
 * return StringVoid if no such sequence exist
 */
string_t StringListHasStartSequence_1( stringList_t stl,const char * str ) ;

/*
 * append an entry into the list.
 * if stl == NULL, then the function call is the same as "StringList( cstring )"
 */
stringList_t StringListAppend( stringList_t stl,const char * cstring ) ;

/*
 * append an entry into the list if the entry is not already in the list
 * if stl == NULL, then the function call is the same as "StringList( cstring )"
 */
stringList_t StringListAppendIfAbsent( stringList_t stl,const char * cstring ) ;

/*
 * append an entry into the list and take ownership of it.
 * if stl == NULL, then the function call is the same as "StringList( cstring )"
 */
stringList_t StringListAppendString_1( stringList_t stl,string_t * ) ;

/*
 * append an entry into the list.
 * if stl == NULL, then the function call is the same as "StringList( cstring )"
 */
stringList_t StringListAppendString( stringList_t stl,string_t ) ;

/*
 * append an entry into the list by taking only len characters from the string.
 * if stl == NULL, then the function call is the same as "StringList( cstring )"
 */
stringList_t StringListAppendSize( stringList_t stl,const char * cstring,size_t len ) ;

/*
 * Append a stringList_t on the second argument to stringList_t on the first argument
 * return the stringList_t on the first argument
 */
stringList_t StringListAppendList( stringList_t,stringList_t ) ;

/*
 * prepend an entry into the list
 * if stl == NULL then the function is the same as "StringList( cstring )"
 */
stringList_t StringListPrepend( stringList_t stl,const char * cstring ) ;

/*
 * prepend an entry into the list by taking len characters from the string.
 * if stl == NULL then the function is the same as "StringList( cstring )"
 */
stringList_t StringListPrependSize( stringList_t stl,const char * cstring,size_t len ) ;

/*
 * insert an element as position index.
 * the first position is at position 0
 * if index == size of the list,the operations will have the same effect as appending the entry to the end of the list
 */
stringList_t StringListInsertAt( stringList_t stl,const char * cstring,size_t index ) ;

/*
 * insert string_t as position index.
 * the first position is at position 0
 *
 * this function will take ownership of st,invalidating the handle after auto deleting its contents
 *  * if index == size of the list,the operations will have the same effect as appending the entry to the end of the list
 */
stringList_t StringListStringInsertAt( stringList_t stl,string_t *,size_t index ) ;

/*
 * insert an element as position index by taking only len characters from the string
 * the first position is at position 0
 * if index == size of the list,the operations will have the same effect as appending the entry to the end of the list
 */
stringList_t StringListInsertAtSize( stringList_t stl,const char * cstring,size_t len, size_t index ) ;

/*
 * remove an entry from the list bt deleting it
 * the first position is at position 0
 */
stringList_t StringListRemoveAt( stringList_t stl, size_t index ) ;

/*
 * go through every string in the list and remove all strings that starts with str
 * returned value is the number of elements removed
 */
size_t StringListRemoveIfStringStartsWith( stringList_t stl,const char * str ) ;

/*
 * go through every string in the list and remove all strings has str character sequence
 * returned value is the number of elements removed
 */
size_t StringListRemoveIfStringContains( stringList_t stl,const char * str ) ;

/*
 * remove a string_t object from the managed list if it manages string str.
 * this function is to be used if there maybe multiple occurances of the string.
 * If there is atmost one entry,then StringListRemoveString() is a function to use.
 */
size_t StringListRemoveIfPresent( stringList_t stl,const char * str ) ;

/*
 * remove a string_t object from the managed list if it is found.
 * this function is to be used if there maybe multiple occurances of the string.
 */
size_t StringListRemoveIfPresent_1( stringList_t stl,string_t st ) ;

/*
 * remove an entry from the list at position index and "let it free",the detached entry is now a fully fledged string_t object.
 * the first position is at position 0
 */
string_t StringListDetachAt( stringList_t stl, size_t index ) ;

/*
 *  remember to clean after yourself
 *  when function return, stl will point to NULL
 */
void StringListDelete( stringList_t * stl ) ;

/*
 * returns NULL terminated array of strings( char * argv[] ) composed of strings managed by the stringlist
 * NULL is returned on error.
 * NOTE: remember to free() the returned value but DONOT free its content.
 */
const char * const * StringListStringArray( stringList_t ) ;

/*
 * It does what the above does but it reuses the buffer and hence its better if the function is called
 * multiple times.
 * second argument will hold the size of the buffer without counting the NULL terminator
 * The two first arguments must be initilized to example values
 * Example usage
 * char * const * buffer = NULL ;
 * size_t buffer_size = 0 ;
 * stringList_t stl = StringList( "abc" ) ;
 * buffer = StringListStringArray_1( buffer,&buffer_size,stl ) ;
 */
char * const * StringListStringArray_1( char * const *,size_t *,stringList_t ) ;

/*
 * remember to clean after yourself
 * overwite string objects as they are cleaned up
 */
void StringListClearDelete( stringList_t * stl ) ;

/*
 * delete multiple stringList_t * objects.
 * NOTE; The last element on the argument list must be '\0'
 */
void StringListMultipleDelete( stringList_t * stl,... )  __attribute__ ( ( sentinel ) ) ;

/*
 * find the first element with cstring and remove it and return the position of where the element was.
 * return -1 when there is no element in the list with cstring.
 * call the function repeatedly until you get -1 if elements can repeat.
 */
ssize_t StringListRemoveString( stringList_t stl,const char * cstring ) ;

/*
 * add a cstring to the list and take it over( own it )
 * cstring will be NULL on function return.
 * cstring must be created will malloc.
 * s is the size of the string
 * l is the size of the buffer holding the string
 */
stringList_t StringListAppendWithSize( stringList_t stl,char ** cstring,size_t s,size_t l ) ;

/*
 * create a stringlist with cstring and take it over( own it )
 * cstring will point to NULL on function return.
 * cstring must be created with malloc.
 * s is the size of the string
 * l is the size of the buffer holding the string
 */
stringList_t StringListWithSize( char ** cstring,size_t s,size_t l ) ;

/*
 * returns a copy of a string list
 */
stringList_t StringListCopy( stringList_t stl ) ;

/*
 * make a string_t copy of a string at position pos
 * first position is at pos = 0
 */
string_t StringListCopyStringAt( stringList_t,size_t pos ) ;

static __inline__ string_t StringListCopyStringAtFirstPlace( stringList_t stl )
{
	return StringListCopyStringAt( stl,0 ) ;
}

static __inline__ string_t StringListCopyStringAtSecondPlace( stringList_t stl )
{
	return StringListCopyStringAt( stl,1 ) ;
}

static __inline__ int StringListHasEntry( stringList_t stl,const char * cstring )
{
	return StringListContains( stl,cstring ) != -1 ;
}

static __inline__ int StringListHasNoEntry( stringList_t stl,const char * cstring )
{
	return StringListContains( stl,cstring ) == -1 ;
}

/*
 * move entry at position x to position y and entry at position y to position x *
 * first entry is at position 0.
 */
stringList_t StringListSwap( stringList_t stl, size_t x,size_t y ) ;

#ifdef __cplusplus
}
#endif

#endif