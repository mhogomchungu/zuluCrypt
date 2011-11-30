/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

#ifndef FBString
#define FBString

#ifdef __cplusplus
extern "C" {
#endif	

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * String handle, all string operation should happen through this handle.
 */
typedef struct StringHandle StrHandle ;

/*
 * Initializes the string.
 * Argument:      A character array to be stored
 * Return value:  A handle to use for subsequent access to the array, NULL is returned on fail.
 */
StrHandle * String(const char * data) ;


/*
 * Append a string pointer to by s into a string handled by handle st 
 *
 * On success, a concatenated string is returned.
 * On error, NULL is returned and the original string remain intact. 
 */
const char * StringAppend(StrHandle * st ,const  char * s)  ;

/*
 * Prepend a string pointed to by s into a string handled by handle st
 * 
 * On success, a concatenated string is returned
 * On error, NULL is returned and the original string remain intact   
 */
const char * StringPrepend(StrHandle * st ,const  char * s)  ;

/*
 * Inherit a string pointed to by data and return a string handle to the string on success or NULL on error. 
 * This function should inherit strings only from a dynamically created memory.  
 */
StrHandle * StringInherit( char * data ) ;

/* 
 * Returns a const pointer to a string handled by handle st. 
 */
const char * StringContent( StrHandle * st) ;

/* 
 * Returns an editable string copy of a string handled by handle st. 
 * Remember to free it when you are done using it.
 * NULL is returned when the copy can not be made. 
 */
char * StringCopy( StrHandle * st ) ;


/*
 * Write "size" amount of content of a string handled by "st" to a buffer pointed by "buffer"  *
 */
void StringReadToBuffer( StrHandle * st,char * buffer, int size) ;


/*
 * Remember to clean after yourself.
 * Always call this function when you are done with the string handled by handle st.
 */
void StringDelete(StrHandle * st);

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
char * StringDeleteHandle( StrHandle * st) ;


/*
 * Return the length of the string handled by handle st.
 */
int StringLength(StrHandle * st) ;


/*
 * Return a character at position p.First position is at "0".
 * 
 * NOTE: it is your responsibility to make sure p is within range
 */
char StringCharAt( StrHandle * st, int p) ;

/*
 * return the position of the first occurance of character s starting from position p.
 * 
 * -1 is returned if the character is not in the string.
 * 
 * NOTE: first character in the string is at position 0
 */
int StringIndexOfChar( StrHandle * st, int p , char s ) ;


/*
 * return the position of the first occurance of string s starting from position p.
 *  
 * -1 is returned if s isnt in the string
 * 
 * NOTE: first character in the string is at position 0
 */
int StringIndexOfString( StrHandle * st,int p, const char * s ) ;


/*
 * Return the position of the last occurance of character s
 * 
 * -1 is returned if the character isnt in the string
 */
int StringLastIndexOfChar(StrHandle * st , char s) ;


/*
 * Returns the last occurance of a string pointed by s
 * 
 * -1 is returned of the string is not found.
 * 
 * NOTE: first character in the string is at position 0
 * 
 */
int StringLastIndexOfString(StrHandle * st , const char * s) ;


/*
 * Return a const string starting at position p. First position is at 0 
 *  
 * NOTE: it is your responsibility to make sure p is within range.
 */
const char * StringStringAt( StrHandle * st , int p) ;


/*
 *Check to see if the string pointer by st ends with string s. 
 * 
 * return 0 is it does
 * 
 * return -1 if it doesnt * 
 */
int StringEndsWithString(StrHandle * st , const char * s) ;


/*
 *Check to see if the string pointer by st ends with char s. 
 * 
 * return 0 is it does
 * 
 * return -1 if it doesnt * 
 */
int StringEndsWithChar(StrHandle * st ,char s) ;


/*
 * Insert a string s from position x and returns a pointer to the new string
 * 
 * on error returns NULL and the original string remains intact  
 */
const char * StringInsertString(StrHandle * st, int x, const char * s ) ;


/*
 * Insert a char s at position x and returns a pointer to the new string
 * 
 * on error returns NULL and the original string remains intact  
 */
const char * StringInsertChar(StrHandle * st, int x, char s ) ;


/*
 * subsititue whatever character is at position x by character s
 * returns a pointer to the string containing the subsititue
 */
const char * StringSubChar(StrHandle * st, int x, char s ) ;


/*
 * start from position x and subsititue all characters in the string by string s.
 * returns a pointer with the substitution.  
 */
const char * StringSubString(StrHandle * st, int x, const char * s ) ;


/*
 * start at position x and remove y character(s) going right and returns a pointer
 * to the new string or NULL on error and the original string remain intact.
 * Careful though, make sure you dont delete past the string length  
 */
const char * StringRemoveString(StrHandle * st,int x , int y) ;

/*
 * remove all occurances of string s in a string handled by handle st.
 * return a pointer to the resulting string on success and NULL on error and the original string
 * remain intact
 * 
 */
const char * StringRemoveStringString(StrHandle * st, const char * s ) ;

/*
 * Cut the string from position x and discard all elements to the right of x. 
 * Returns a pointer to the result, NULL on error and the original string remain intact
 */
const char * StringRemoveRight(StrHandle * st, int x) ;


/*
 * Cut the string from position x and discard all elements to the left of x. 
 * Returns a pointer to the result, NULL on error and the original string remain intact
 */
const char * StringRemoveLeft(StrHandle * st, int x) ;


/*
 * Returns an editable copy of a string made up of upto x characters
 * Remember to free string when you are done with it.
 * returns NULL on error. * 
 */
char * StringLengthCopy( StrHandle * st,int x ) ;


/*
 * Return an editable string starting at position x and made up of y characters
 *
 * NULL is returned if the operation fail.
 * 
 * Remember to free the returned string when done with it. * 
 */
char * StringMidString( StrHandle * st , int x, int y ) ;


/*
 * replace all occurance of string x by string y 
 * return a pointer to the resulting string on success or NULL on error and the original string
 * remain intact.
 */
const char * StringReplaceString( StrHandle * st, const char * x, const char * y) ;


/*
 * replace all occurance of char x by char y  
 * Return a const pointer to a modified string   
 */
const char * StringReplaceChar( StrHandle * st,char x, char y) ;

/*
 *  Replace all characters in y by x in a string handled by st
 */
const char * StringReplaceCharString( StrHandle * st, char x, const char * y ) ;

/*
 * convert a number z into a string of size y pointer to by x.
 * 
 * return value: a pointer to the beginning of the result(use this pointer and not x).
 * 
 * NOTE: Its your responsibility to make sure the resulting string fit into a buffer x.
 * If the number has N digits, then the string must be atleast N+1 in size, null character
 * takes the last spot.  
 */

char * intToString( char * x, int y, uint64_t z ) ;

/*
 * Compare a string handled by handle x to a string handled by handle y and return 0 if they are equal and 1 if they are not  
 */
int StringCompare( StrHandle * x , StrHandle * y ) ;

/*
 * Compare a string handled by handle x to a string pointer to by y and return 0 if they are equal and 1 if they are not  
 */
int StringCompareString( StrHandle * x, const char * y ) ;

/*
 * Insert character x infront of every character that appear in string y in a string handled by handle st.
 * Retun a poiter to the final string on success and NULL on error and the original string remain intact
 */
const char * StringInsertCharString( StrHandle * st, char x, const char * y ) ;

/*
 * Insert character x infront of every character y in a string handled by handle st.
 * Retun a poiter to the final string on success and NULL on error and the original string remain intact
 */
const char * StringInsertCharChar( StrHandle * st, char x, char y ) ;

/*
 * Crop off the first x elements and the last y elements from the string handled by handle st
 * 
 * Return a pointer to the cropped string on success and NULL on error and the original string remain intact. 
 * NULL is also returned if cropping will result in less that zero characters in the string 
 */
const char * StringCrop( StrHandle * st, int x, int y ) ;

#ifdef __cplusplus
}
#endif

#endif
