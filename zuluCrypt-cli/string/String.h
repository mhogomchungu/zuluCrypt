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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <termios.h>

/*
 * create a custom type to represent a string_t pointing to NULL while hiding the pointer nature of string_t
 * string_t is an opaque handle and NULL assignment "gives unnecessary info" about its nature.  
 */
#define StringVoid ( ( void * ) 0 ) 

/*
 * string_t type is a  string handle,all string operation should happen through this handle.  
 */
typedef struct StringType * string_t ;

/*
 * initialize a handle with a C string
 */
string_t String( const char * cstring ) ;

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
 * Append multitple const char * strings to a string handled by handle st.
 * NOTE: The list must be terminated a NULL character.
 * example usage
 *  string_t st = String( "abc" ) ;
 *  StringMultipleAppend( st,"def","ghi",'\0' ) ; 
 */
const char * StringMultipleAppend( string_t st,... )  ;

/*
 * Append a string handled by xt into a string handled by handle st 
 *
 * On success,a concatenated string is returned.
 * On error,NULL is returned and the original string remain intact. 
 */
const char * StringAppendString( string_t st,string_t xt )  ;

/*
 * check if a string handled by st has str in it
 * return 0 if str is found
 * return 1 if str is not found
 */
int StringContains( string_t st,const char * str ) ;

/*
 * Append multiple string_t to a string handled by handle st .
 * Same requirement as StringMultipleAppend apply 
 */
const char * StringAppendMultipleString( string_t st,... )  ;

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
 *  StringMultipleAppend( st,"def","abc",'\0' ) ;
 */
const char * StringMultiplePrepend( string_t st,... )  ;

/*
 * prepend multiple string_t to a string handled by string_t st.
 * NOTE: entries will be prepended in the order they are presented
 * NOTE: a list of entries must be NULL terminated
 */
const char * StringMultiplePrependString( string_t st,... )  ;

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
 */
string_t StringInheritWithSize( char ** data,size_t s ) ;

/* 
 * Returns a const pointer to a string handled by handle st. 
 * The returned pointer is guaranteed to be valid only if no further operation is performed on the string.
 */
const char * StringContent( string_t st ) ;

/*
 * This function returns a pointer to a string and is guaranteed to be valid as long as StringDelete() is
 * not called on the string.
 * 
 * The underlying string can be obtained after the returned double pointer is derefenced. 
 */
const char ** StringPointer( string_t st ) ;

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
char * StringCopyChar( string_t st ) ;

/*
 * Make a copy of string_t st
 * return NULL on error.
 */
string_t StringCopy( string_t st ) ;

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
void StringMultipleDelete( string_t * st,... );

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
 *Check to see if the string pointer by st ends with string s. 
 * 
 * return 0 is it does
 * 
 * return 1 if it doesnt * 
 */
int StringEndsWithString( string_t st,const char * s ) ;


/*
 *Check to see if the string pointer by st ends with char s. 
 * 
 * return 0 is it does
 * 
 * return 1 if it doesnt * 
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
const char * StringInsertAndDelete( string_t st,size_t x,const char * s ) ;


/*
 * start at position x and remove y character(s) going right and returns a pointer
 * to the new string or NULL on error and the original string remain intact.
 * Careful though,make sure you dont delete past the string length  
 */
const char * StringRemoveLenght( string_t st,size_t x,size_t y ) ;

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
 * Cut the string from position x and discard all elements to the right of x. 
 * Returns a pointer to the result,NULL on error and the original string remain intact
 */
const char * StringRemoveRight( string_t st,size_t x ) ;

/*
 * Clear the string by writing '\0' to a string buffer handled by handle st. Essentiall remove all elements in it.  
 */
void StringClear( string_t st ) ;

/*
 * Cut the string from position x and discard all elements to the left of x. 
 * Returns a pointer to the result,NULL on error and the original string remain intact
 */
const char * StringRemoveLeft( string_t st,size_t x ) ;


/*
 * Returns an editable copy of a string made up of upto x characters
 * Remember to free string when you are done with it.
 * returns NULL on error. * 
 */
char * StringLengthCopy( string_t st,size_t x ) ;


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

char * StringIntToString_1( char * x,size_t y,uint64_t z ) ;

/*
 * convert a number z into a string
 */
string_t StringIntToString( uint64_t ) ;

/*
 * Compare a string handled by handle x to a string handled by handle y and return 0 if they are equal and 1 if they are not  
 */
int StringCompare( string_t x,string_t y ) ;

/*
 * Compare a string handled by handle x to a string pointer to by y and return 0 if they are equal and 1 if they are not  
 */
int StringEqual( string_t x,const char * y ) ;

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
 *  Open a file given by path and return a string_t handle through agrument st with the content of the file .  
 *  return value: 0 - opefation succeeded.
 *                1 - path is invalid.
 *                2 - could not open file for reading.
 * 		  3 - could not allocate memory to host file content 
 */
int StringGetFromFile_1( string_t * st,const char * path ) ;

/*
 *  Open a file given by path and return a string_t handle through agrument st with the content of the file .
 *  Strart reading the file from offset byte and read length bytes.  
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

#ifdef __cplusplus
}
#endif

#endif
