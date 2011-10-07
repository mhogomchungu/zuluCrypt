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

typedef struct {
	int size ;
	char *string ; 
}StrHandle;


/*
 * Initializes the string.
 * Argument:      A character array to be stored
 * Return value:  A handle to use for subsequent access to the array, NULL is returned on fail.
 */
StrHandle * StringCpy(const char * data) ;



/*
 * A string is a character array, this functions appends a string by doing the following: 
 * 1. Request enough memory to hold the old array and the new array  
 * 2. Copy the old array to the newly requested memory.
 * 3. Concatenate the new array at the end of the old one on the newly requested memory
 * 4. Delete old array
 *  
 * Note: Nothing happens when step 1 fails. A way to check if the operation passed or not is to call StringLength function before and after
 *       and compare the string lengths, If they are equal, the operation failed.
 * 
 */
const char * StringCat(StrHandle * st ,const  char * data)  ;


/*
 * "cont" is a short form of "content"
 * Returns a const string
 * 
 */
const char * StringCont( StrHandle * st) ;

/*
 * "cont" is a short form of "content"
 * Returns an editable string.
 * Remember to free it when you are done using it.
 * 
 */
char * StringContCopy( StrHandle *st ) ;


/*
 * Write to content of a string handled by "st" to a buffer pointed by "buffer" 
 *
 */
void StringReadToBuffer( StrHandle * st,char *buffer, int size) ;


/*
 * Remember to clean after yourself.
 */
void StringDelete(StrHandle * st);


/*
 * Return the length of the string
 */
int StringLength(StrHandle * st) ;


/*
 * Return a character at position p.First position is at "0".
 */

char StringCharAt( StrHandle * st, int p) ;

/*
 * return the position of the first occurance of character s
 */

int StringPosChar( StrHandle * st , char s ) ;

/*
 * return the position of the first occurance of string s
 * NULL is returned when s isnt in the string
 */
int StringPosString( StrHandle * st, const char * s ) ;

/*
 * Return a string starting at position p. First position is at 0 
 *  
 */
const char * StringFrom( StrHandle * st , int p) ;

/*
 * Insert a string s from position x and returns a pointer to the new string
 * 
 * on error returns NULL and the original string remains intact
 * 
 */
const char * StringInsert(StrHandle * st, int x, const char * s ) ;

/*
 * Insert a char s at position x and returns a pointer to the new string
 * 
 * on error returns NULL and the original string remains intact
 * 
 */
const char * StringCharInsert(StrHandle * st, int x, char s ) ;


/*
 * subsititue whatever character is at position x by character s
 * returns a pointer to the string containing the subsititue
 */
const char * StringCharSub(StrHandle * st, int x, char s ) ;

/*
 * start from position x and subsititue all characters in the string by string s.
 * Careful though, make sure the substitution string is not long enough to overflow
 * the length of the original string.
 * returns a pointer with the substitution.  
 */
const char * StringStringSub(StrHandle * st, int x, const char * s ) ;


/*
 * start at position x and remove y character(s) going right and returns a pointer
 * to the new string or NULL on error.
 * Careful though, make sure you dont delete past the string length * 
 */
const char * StringStringRemove(StrHandle *st,int x , int y) ;


/*
 * Cut the string from position x and discard all elements to the right of x. 
 * Returns a pointer to the result, NULL on error and the original string remain intact
 */
const char * StringChop(StrHandle * st, int x) ;

/*
 * Returns a copy of a string made up of upto x characters
 * Remember to free string when you are done with it.
 * returns NULL on error.
 * 
 */
char * StringContCopyLength( StrHandle *st,int x ) ;


#ifdef __cplusplus
}
#endif

#endif
