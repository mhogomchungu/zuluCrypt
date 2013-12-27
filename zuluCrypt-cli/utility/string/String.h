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

#ifndef STRINGTYPE
#define STRINGTYPE

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
/*
 * Takes a pointer to a function to be called when memory allocation can not take place
 * ie if the system has run out of memory and malloc() or realloc() has failed.
 * This function is optional and "StringVoid" will be returned on memory exaustion if the function
 * isnt set.
 */
void StringExitOnMemoryExaustion( void (*)( void ) ) ;

/*
 * string_t type is a  string handle,all string operation should happen through this handle.
 */
typedef struct StringType * string_t ;

/*
 * use this variable to terminate variadic functions
 * we dont use NULL here because NULL is defined simply as 0 and things break in C++
 */
#define ENDDELETE ( ( string_t * ) 0 )
#define END ( ( const char * ) 0 )

/*
 * create a custom type to represent a string_t pointing to NULL while hiding the pointer nature of string_t
 * string_t is an opaque handle and NULL assignment "gives unnecessary info" about its nature.
 */
#define StringVoid ( ( string_t ) 0 )

/*
 * initialize a handle with a C string
 */
string_t String( const char * cstring ) ;

/*
 * initialize a handle with an empty C string
 */
string_t StringEmpty( void ) ;

/*
 * ininitialize a string handle by memcpy() length characters from a string
 * a NULL character will be appended to the string.
 */
string_t StringWithSize( const char * string,size_t length ) ;

/*
 * Append a string pointer to by s into a string handled by handle st
 *
 * On success,a concatenated string is returned.
 * On error,NULL is returned and the original string remain intact.
 */
const char * StringAppend( string_t st,const  char * s )  ;

/*
 * Append a u_int64_t number to the string
 */
const char * StringAppendInt( string_t,u_int64_t ) ;

/*
 * subsititue all occurance of string str with a number num
 */
const char * StringSubStringWithInt( string_t st,const char * str,u_int64_t num ) ;

/*
 * Append multitple const char * strings to a string handled by handle st.
 * NOTE: The list must be terminated a NULL character.
 * example usage
 *  string_t st = String( "abc" ) ;
 *  StringMultipleAppend( st,"def","ghi",NULL ) ;
 */
const char * StringMultipleAppend( string_t st,... )  __attribute__ ( ( sentinel ) ) ;

/*
 * Append a string handled by xt into a string handled by handle st
 *
 * On success,a concatenated string is returned.
 * On error,NULL is returned and the original string remain intact.
 */
const char * StringAppendString( string_t st,string_t xt )  ;

/*
 * check if a string handled by st has str in it
 * return 1 if str is found
 * return 0 if str is not found
 */
int StringContains( string_t st,const char * str ) ;

/*
 * Append multiple string_t to a string handled by handle st .
 * Same requirement as StringMultipleAppend apply
 * NOTE:The series must be NULL terminated.
 */
const char * StringAppendMultipleString( string_t st,... )  __attribute__ ( ( sentinel ) ) ;

/*
 * Append a char c into a string handled by handle st
 *
 * On success,a concatenated string is returned.
 * On error,NULL is returned and the original string remain intact.
 */
const char * StringAppendChar( string_t st,char c )  ;

/*
 * Prepend a string pointed to by s into a string handled by handle st
 *
 * On success,a concatenated string is returned
 * On error,NULL is returned and the original string remain intact
 */
const char * StringPrepend( string_t st,const char * s )  ;

/*
 * prepend multiple const char * to a string handled by string_t st.
 * NOTE: entries will be prepended in the order they are presented
 * NOTE: a list of entries must be NULL terminated
 *  example usage
 *  string_t st = String( "ghi" ) ;
 *  StringMultipleAppend( st,"def","abc",NULL ) ;
 */
const char * StringMultiplePrepend( string_t st,... )   __attribute__ ( ( sentinel ) ) ;

/*
 * prepend multiple string_t to a string handled by string_t st.
 * NOTE: entries will be prepended in the order they are presented
 * NOTE: a list of entries must be NULL terminated
 */
const char * StringMultiplePrependString( string_t st,... )  __attribute__ ( ( sentinel ) ) ;

/*
 * Prepend a string_t xt into a string handled by handle st
 *
 * On success,a concatenated string is returned
 * On error,NULL is returned and the original string remain intact
 */
const char * StringPrependString( string_t st,string_t xt )  ;
/*
 * Prepend a char c into a string handled by handle st
 *
 * On success,a concatenated string is returned
 * On error,NULL is returned and the original string remain intact
 */
const char * StringPrependChar( string_t st,char c )  ;

/*
 * Inherit a string pointed to by data and return a string handle to the string on success or NULL on error.
 * This function should inherit strings only from a dynamically created memory.
 * The original string remains intact when the function error out.
 */
string_t StringInherit( char ** data ) ;

/*
 * Inherit a string of size s pointed to by data and return a string handle to the string on success or NULL on error.
 * This function should inherit strings only from a dynamically created memory.
 * The original string remains intacct when the function error out.
 * size is the size of the string
 * length is the size of the buffer holding the string
 */
string_t StringInheritWithSize( char ** data,size_t size,size_t length ) ;

/*
 * Returns a const pointer to a string handled by handle st.
 * The returned pointer is guaranteed to be valid only if no further operation is performed on the string.
 */
static __inline__ const char * StringContent( string_t st ) ;

/*
 * This function returns a pointer to a string and is guaranteed to be valid as long as StringDelete() is
 * not called on the string.
 *
 * The underlying string can be obtained after the returned double pointer is derefenced.
 */
static __inline__ const char ** StringPointer( string_t st ) ;

/*
 * printf() the string handled by handle st *
 */
void StringPrint( string_t st ) ;

/*
 * printf() the string handled by handle st and followed by a newline
 * it has the same effect as StringPrint() followed by printf("\n")
 */
void StringPrintLine( string_t st ) ;

/*
 * Returns an editable string copy of a string handled by handle st.
 * Remember to free it when you are done using it.
 * NULL is returned when the copy can not be made.
 */
char * StringCopy_1( string_t st ) ;

/*
 * Make a copy of string_t st
 * return NULL on error.
 */
string_t StringCopy( string_t st ) ;

/*
 * returns a copy of a string.
 * remember to free() it when done with it
 */
char * StringCopy_2( const char * ) ;

/*
 * Returns an editable copy of a string made up of upto x characters
 * Remember to free string when you are done with it.
 * returns NULL on error. *
 */
char * StringCopy_3( string_t st,size_t x ) ;

/*
 * Write "size" amount of content of a string handled by "st" to a buffer pointed by "buffer"  *
 */
void StringReadToBuffer( string_t st,char * buffer,size_t size ) ;

/*
 * Remember to clean after yourself.
 * Always call this function when you are done with the string handled by handle st.
 * The function will not attempt to double delete,or delete unassigned handle( NULL handle ),for
 * this to work,make sure your handles are NULL ininitialized.
 * NULL is assigned to handle to invalidate it
 */
void StringDelete( string_t * st );

/*
 * clear the string before StringDelete() it
 */
void StringClearDelete( string_t * st ) ;

/*
 * Remember to clean after yourself.
 * delete multitple string_t objects.
 * NOTE: a list of entries must be NULL terminated
 * The function will not attempt to double delete,or delete unassigned handle( NULL handle ),for
 * this to work,make sure your handled are NULL ininitialized.
 * NULL is assigned to each handle to invalidate them
 */
void StringMultipleDelete( string_t * st,... )  __attribute__ ( ( sentinel ) ) ;

/*
 * input argument: String handle
 * return value: a pointer to the string managed by the handle.
 *
 * This function deletes the handle presented through argument st and returns
 * a pointer to the string handled by st.
 *
 * Remember to free the pointer( using free() ) when you are done with it
 *
 * Use this function when you no longer need the handle i.e dont want to do string
 * manipulation through the handle but want the pointer to the string it managed.
 */
char * StringDeleteHandle( string_t * st ) ;

/*
 * Return the length of the string handled by handle st.
 */
ssize_t StringLength( string_t st ) ;

/*
 * Return a character at position p.First position is at "0".
 *
 * NOTE: it is your responsibility to make sure p is within range
 */
char StringCharAt( string_t st,size_t p ) ;

/*
 * Returns the last character in the string
 */
char StringCharAtLast( string_t st ) ;

/*
 * return the position of the first occurance of character s starting from position p.
 *
 * -1 is returned if the character is not in the string.
 *
 * NOTE: first character in the string is at position 0
 */
ssize_t StringIndexOfChar( string_t st,size_t p,char s ) ;

/*
 * return the position of the first occurance of string s starting from position p.
 *
 * -1 is returned if s isnt in the string
 *
 * NOTE: first character in the string is at position 0
 */
ssize_t StringIndexOfString( string_t st,size_t p,const char * s ) ;

/*
 * Return the position of the last occurance of character s
 *
 * -1 is returned if the character isnt in the string
 */
ssize_t StringLastIndexOfChar( string_t st,char s ) ;

/*
 * Returns the last occurance of a string pointed by s
 *
 * -1 is returned of the string is not found.
 *
 * NOTE: first character in the string is at position 0
 *
 */
ssize_t StringLastIndexOfString( string_t st,const char * s ) ;

/*
 * Return a const string starting at position p. First position is at 0
 *
 * NOTE: it is your responsibility to make sure p is within range.
 */
const char * StringStringAt( string_t st,size_t p ) ;

/*
 * return 1 if a string is a part of stringlist
 * return 0 otherwise or if StringVoid
 */
int StringOwned( string_t ) ;

/*
 * call mlock() on the string
 */
int StringLock( string_t ) ;

/*
 * call munlock() on the string
 */
int StringUnlock( string_t ) ;

/*
 *Check to see if the string pointer by st ends with string s.
 *
 * return 1 is it does
 *
 * return 0 if it doesnt *
 */
int StringEndsWith( string_t st,const char * s ) ;

/*
 * check to see if the string handled by handle st starts with a string s
 * return 1 if it does
 * return 0 if it doesnt
 */
int StringStartsWith( string_t st,const char * s ) ;

/*
 * check to see the string_t object starts with atleast one of the arbitrary number of cstrings
 * return 1 if it does
 * return 0 if it does not
 *
 * exampe:
 * st = String( "abcdef" ) ;
 * int r = StringAtLeastOneStartsWith( st,"rrr","www","abc",NULL ) ;
 * r will contain "1" since "abc" argument match the beginning of the string_t object
 * NOTE:The series must be NULL terminated.
 */
int StringAtLeastOneStartsWith( string_t st,... ) __attribute__ ( ( sentinel ) ) ;

/*
 *Check to see if the string pointer by st ends with char s.
 *
 * return 1 is it does
 *
 * return 0 if it doesnt *
 */
int StringEndsWithChar( string_t st,char s ) ;

/*
 * Insert a string s from position x and returns a pointer to the new string
 *
 * on error returns NULL and the original string remains intact
 */
const char * StringInsertString( string_t st,size_t x,const char * s ) ;

/*
 * Insert a char s at position x and returns a pointer to the new string
 *
 * on error returns NULL and the original string remains intact
 */
const char * StringInsertChar( string_t st,size_t x,char s ) ;

/*
 * subsititue whatever character is at position x by character s
 * returns a pointer to the string containing the subsititue
 */
const char * StringSubChar( string_t st,size_t x,char s ) ;

/*
 * start from position x and subsititue all characters in the string by string s.
 * returns a pointer with the substitution.
 */
const char * StringSubString( string_t st,size_t x,const char * s ) ;

/*
 * remove all character after index x and then insert string s at index.
 */
const char * StringAppendAt( string_t st,size_t x,const char * s ) ;

/*
 * start at position x and remove y character(s) going right and returns a pointer
 * to the new string or NULL on error and the original string remain intact.
 * Careful though,make sure you dont delete past the string length
 */
const char * StringRemoveLength( string_t st,size_t x,size_t y ) ;

/*
 * remove all occurances of string s in a string handled by handle st.
 * return a pointer to the resulting string on success and NULL on error and the original string
 * remain intact
 *
 */
const char * StringRemoveString( string_t st,const char * s ) ;

/*
 * starting at position p,remove all occurances of string s in a string handled by handle st.
 * return a pointer to the resulting string on success and NULL on error and the original string
 * remain intact
 *
 */
const char * StringRemoveStringPos( string_t st,const char * s,size_t p ) ;

/*
 * remove the first x characters counting from the right ie,remove the last x characters from the string
 * Returns a pointer to the result,NULL on error and the original string remain intact
 */
const char * StringRemoveRight( string_t st,size_t x ) ;

/*
 * Clear the string by writing '\0' using memset() essentiall wipe out all of its current content.
 * This is a bit more expensive but useful if you want to start afresh with an empty string.
 */
void StringClear( string_t st ) ;

/*
 * forget about all content in the string and start afresh as if the string is not allocated.
 * The old data is still there and will be overwritten by new data as the string get filled up
 */
void StringReset( string_t st ) ;

/*
 * remove the first x characters from the string counting from the left
 * Returns a pointer to the result,NULL on error and the original string remain intact
 */
const char * StringRemoveLeft( string_t st,size_t x ) ;

/*
 * Return a sub string starting at position x and made up of y characters
 *
 * NULL is returned if the operation fail.
 *
 * Remember to free the returned string when done with it. *
 */
string_t StringMidString( string_t st,size_t x,size_t y ) ;

/*
 * replace all occurance of string x by string y
 * return a pointer to the resulting string on success or NULL on error and the original string
 * remain intact.
 */
const char * StringReplaceString( string_t st,const char * x,const char * y ) ;

/*
 * replace the string managed by string_t object by string str
 * return a pointer to the new string managed by string_t object or NULL if either of the argument is NULL
 */
const char * StringReplace( string_t,const char * str ) ;

/*
 * starting at position p,replace all occurance of string x by string y
 * return a pointer to the resulting string on success or NULL on error and the original string
 * remain intact.
 */
const char * StringReplaceStringPos( string_t st,const char * x,const char * y,size_t p ) ;

/*
 * replace all occurance of char x by char y
 * Return a const pointer to a modified string
 */
const char * StringReplaceChar( string_t st,char x,char y ) ;

/*
 * start at index z,replace the first occurance of character x by character y
 * Only the first character will be subsituted if found
 * NULL is returned if substitution could not take place
 * modified string is returned if substitution took place
 */
const char * StringReplaceChar_1( string_t st,size_t z,char x,char y ) ;

/*
 * starting at position p,replace all occurance of char x by char y
 * Return a const pointer to a modified string
 */
const char * StringReplaceCharPos( string_t st,char x,char y,size_t p ) ;

/*
 *  Replace all characters in y by x in a string handled by st
 */
const char * StringReplaceCharString( string_t st,char x,const char * y ) ;

/*
 * Starting at position,Replace all characters in y by x in a string handled by st
 */
const char * StringReplaceCharStringPos( string_t st,char x,const char * y,size_t p ) ;

/*
 * convert a number z into a string and store the result into array x of size y.
 *
 * return value: a pointer to the beginning of the result(use this pointer and not x).
 *
 * NOTE: Its your responsibility to make sure the resulting string fit into array x.
 * If z has N digits,then the array must be atleast N+1 in size,null character
 * takes the last spot.
 */

char * StringIntToString_1( char * x,size_t y,u_int64_t z ) ;

/*
 * convert a number z into a string
 */
string_t StringIntToString( u_int64_t ) ;

/*
 * convert a string made up of digits to a u_int64_t value
 * eg:
 * u_int64_t x = StringConvertToInt( "43542" ) ;
 */
u_int64_t StringConvertToInt( const char * ) ;

/*
 * Compare a string handled by handle x to a string handled by handle y.
 * return 1 if they are equal
 * return 0 if they are not equal
 */
int StringEqualString( string_t x,string_t y ) ;

/*
 * Compare a string handled by handle x to a string pointer to by y.
 * return 1 if they are equal
 * return 0 if they are not equal
 */
int StringEqual( string_t x,const char * y ) ;

/*
 * compare string managed by x with a series of cstrings and return true if atleast one of them match
 * NOTE:the series of strings must be NULL terminated
 * eg.
 * string_t st = String( "abc" ) ;
 * int equal = StringMatchOneAtLeastOne( st,"def","ugf",NULL ) ;
 */
int StringAtLeastOneMatch( string_t x,... )  __attribute__ ( ( sentinel ) ) ;

/*
 * compare cstring x with a series of cstrings and return true if atleast one of them match
 * NOTE:the series of strings must be NULL terminated
 * eg.
 * int equal = StringMatchOneAtLeastOne_1( "def","ugf","rrt",NULL ) ;
 */
int StringAtLeastOneMatch_1( const char * x,... )  __attribute__ ( ( sentinel ) ) ;

/*
 * Insert character x infront of every character that appear in string y in a string handled by handle st.
 * Retun a poiter to the final string on success and NULL on error and the original string remain intact
 */
const char * StringInsertCharString( string_t st,char x,const char * y ) ;

/*
 * Starting at position p,insert character x infront of every character that appear in string y in a string handled by handle st.
 * Retun a poiter to the final string on success and NULL on error and the original string remain intact
 */
const char * StringInsertCharStringPos( string_t st,char x,const char * y,size_t p ) ;

/*
 * Insert character x infront of every character y in a string handled by handle st.
 * Retun a poiter to the final string on success and NULL on error and the original string remain intact
 */
const char * StringInsertCharChar( string_t st,char x,char y ) ;

/*
 * Crop off the first x elements and the last y elements from the string handled by handle st
 *
 * Return a pointer to the cropped string on success and NULL on error and the original string remain intact.
 * NULL is also returned if cropping will result in less that zero characters in the string
 */
const char * StringCrop( string_t st,size_t x,size_t y ) ;

/*
 * getchar() until a newline or EOF character is reached and put the characters to a string_t object
 * The function basically get its content from the terminal ( stdin ) .
 * NULL is returned if sufficient memory can not be optained to hold terminal content.
 */
string_t StringGetFromTerminal( void ) ;

/*
 * getchar() of upto s characters or until a newline or EOF character is reached and put the characters to a string_t object
 * The function basically get its content from the terminal ( stdin ) .
 * NULL is returned if sufficient memory can not be optained to hold terminal content.
 */
string_t StringGetFromTerminal_1( size_t s ) ;

/*
 * Turn echo off and get string from the terminal.
 * This function is ideal for reading passphrases from the terminal.
 * 1 is returned when character echoing can not be turned off.
 * 2 is returned if sufficient memory can not be optained to hold terminal content.
 * 0 is returned on success.
 */
int StringSilentlyGetFromTerminal( string_t * ) ;

/*
 * Turn echo off and get a string of upto s characters from the terminal.
 * This function is ideal for reading passphrases from the terminal.
 * 1 is returned when character echoing can not be turned off.
 * 2 is returned if sufficient memory can not be optained to hold terminal content.
 * 0 is returned on success.
 */
int StringSilentlyGetFromTerminal_1( string_t *,size_t s ) ;

/*
 *  Open a file given by path and return a string_t handle
 *
 *  NULL is returned if the file could not be opened for reading or could not get sufficient memory to hold the file.
 */
string_t StringGetFromFile( const char * path ) ;

/*
 * constract a string_t object with size random characters read from "/dev/urandom.
 * StringVoid is returned on error.
 */
string_t StringRandomString( size_t size ) ;

/*
 *  Open a file given by path and return a string_t handle through agrument st with the content of the file .
 *  return value: 0 - opefation succeeded.
 *                1 - path is invalid.
 *                2 - could not open file for reading.
 * 		  3 - could not allocate memory to host file content
 */
int StringGetFromFile_1( string_t * st,const char * path ) ;

/*
 *  Open a file given by path and return a string_t handle through agrument st with the content of the file .
 *  Start reading the file from offset byte and read length bytes.
 *  return value: 0 - opefation succeeded.
 *                1 - path is invalid.
 *                2 - could not open file for reading.
 * 		  3 - could not allocate memory to host file content
 */
int StringGetFromFile_3( string_t * st,const char * path,size_t offset,size_t length ) ;

/*
 *  Open a file given by path and return a string_t handle with hhe content of the file .
 *  return value through status:
 *                0 - opefation succeeded.
 *                1 - path is invalid.
 *                2 - could not open file for reading.
 * 		  3 - could not allocate memory to host file content
 *
 * This function does the same thing the previous one does,the different is how return values are returned.
 */
string_t StringGetFromFile_2( const char * path,int * status ) ;

/*
 * reads a maximum of "length" bytes from a file given by "path" argument starting at "offset" offset
 * and then lock the memory buffer.
 * return values:
 * 0 - success
 * 1 - file operation failed
 * 2 - memory operation failed
 *
 * The mlock()ed string object will be returned through str argument
 * negative value of length means reads the entire file content
 *
 * warning memory may or may not lock
 */
int StringGetFromFileMemoryLocked( string_t * str,const char * path,size_t offset,ssize_t length ) ;

/*
 *  White the string managed by handle st to a file given by path and return the number of bytes written. *
 */

#define CREATE 1  /*if the file does not exist,create it,if the file exist,trancate it*/
#define APPEND 2  /* if the file exist,append it*/
void StringWriteToFile( string_t st,const char * path,int mode ) ;

/*
 * Open a virtual file given by path return a string_t handle with the content of the file.
 * Virtual files are like those in /proc,you cab read stuff from them but their sizes are always zero
 */
string_t StringGetFromVirtualFile( const char * path ) ;

/*
 * below two functions creates a hash of the string using junkin's one at a time hash algorithm
 */
u_int32_t StringJenkinsOneAtATimeHash( const char * key ) ;

u_int32_t StringJenkinsOneAtATimeHash_1( string_t ) ;

/*
 * a few convenient "safe" functions
 */

/*
 * why doesnt free() take const void * ????
 */
static __inline__ void StringFree( const void * str )
{
	free( ( void * )str ) ;
}

/*
 * safely do free( *dev ) followed by *dev = NULL
 * this function hence should take only a double pointer.
 * ie
 * char * e = malloc(sizeof(char)) ;
 * ..;
 * StringFree_1(&e);
 */
static __inline__ void StringFree_1( const void * str )
{
	char ** c ;
	if( str != NULL ){
		c = ( char ** ) str ;
		free( *c ) ;
		*c = NULL ;
	}
}

static __inline__ size_t StringSize( const char * str )
{
	if( str == NULL ){
		return 0 ;
	}else{
		return strlen( str ) ;
	}
}

static __inline__ int StringsAreEqual( const char * x,const char * y )
{
	if( x == NULL && y == NULL ){
		return 1 ;
	}else if( x == NULL || y == NULL ){
		return 0 ;
	}else{
		return strcmp( x,y ) == 0 ;
	}
}

static __inline__ int StringsAreNotEqual( const char * x,const char * y )
{
	if( x == NULL && y == NULL ){
		return 0 ;
	}else if( x == NULL || y == NULL ){
		return 1 ;
	}else{
		return strcmp( x,y ) != 0 ;
	}
}

static __inline__ int StringPrefixMatch( const char * x,const char * y,size_t z )
{
	if( x == NULL || y == NULL ){
		return 0 ;
	}else{
		return strncmp( x,y,z ) == 0 ;
	}
}

/*
 * returns true if a atleast one cstring in the series starts with x
 * NOTE:The series must be NULL terminated.
 */
int StringAtLeastOnePrefixMatch( const char * x,... )  __attribute__ ( ( sentinel ) ) ;

/*
 * returns true if atleast one cstring is a component of x
 * NOTE:The series must be NULL terminated.
 */
int StringHasAtLeastOneComponent_1( const char * x,... ) __attribute__ ( ( sentinel ) ) ;

/*
 * returns true if atleast one cstring is a component of a string managed by object st
 * NOTE:The series must be NULL terminated.
 */
int StringHasAtLeastOneComponent( string_t st,... ) __attribute__ ( ( sentinel ) ) ;

static __inline__ int StringPrefixEqual( const char * x,const char * y )
{
	if( x == NULL || y == NULL ){
		return 0 ;
	}else{
		return strncmp( x,y,strlen( y ) ) == 0 ;
	}
}

static __inline__ ssize_t StringHasComponent_1( const char * x,const char * y )
{
	char * e ;
	if( x == NULL || y == NULL ){
		return -1 ;
	}else{
		e = strstr( x,y ) ;
		if( e == NULL ){
			return -1 ;
		}else{
			return e - x ;
		}
	}
}

static __inline__ int StringHasComponent( const char * x,const char * y )
{
	if( x == NULL || y == NULL ){
		return 0 ;
	}else{
		return strstr( x,y ) != NULL ;
	}
}

static __inline__ int StringHasNoComponent( const char * x,const char * y )
{
	if( x == NULL || y == NULL ){
		return 0 ;
	}else{
		return strstr( x,y ) == NULL ;
	}
}

static __inline__ ssize_t StringLastIndexOfChar_1( const char * str,char s )
{
	const char * c ;
	if( str == NULL ){
		return -1 ;
	}else{
		c = strrchr( str,s ) ;
		if( c == NULL ){
			return -1 ;
		}else{
			return c - str ;
		}
	}
}

static __inline__ const char * StringContent( string_t st )
{
	const char ** e ;
	if( st == StringVoid ){
		return NULL ;
	}else{
		e = ( const char ** )st ;
		return *e ;
	}
}

static __inline__ const char ** StringPointer( string_t st )
{
	if( st == StringVoid ){
		return NULL ;
	}else{
		return ( const char ** ) st ;
	}
}

#ifdef __cplusplus
}
#endif

#endif
