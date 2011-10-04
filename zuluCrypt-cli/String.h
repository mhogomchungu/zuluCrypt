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

typedef struct StringBuffer{
	char * data ;
	int size ;
	struct StringBuffer * next ;		
}StringBuffer;

typedef struct {
	int size ;
	StringBuffer * first,*current ; 
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
void StringCat(StrHandle * st ,const  char * data)  ;


/*
 * "cont" is a short form of "content"
 * Returns a string. Interesting chunks can be returned when a string was appended with StringLCat function
 * 
 */
char * StringCont( StrHandle * st) ;

/*
 * Implements a linked list.
 * The string can not be accessed as if it is a normal/simple character array.The only way to 
 * access the string when this function is used is by using StringReadtoBuffer function
 * 
 * Interesting usage can be gained with this function. 
 * 
 */
void StringListCat( StrHandle * st, const char * data ) ;


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
 * Return a character at position p.First position is at "1". Put "0" or a number greater than StringLength and your program will 
 *  most likely segfault.
 */

char StringCharAt( StrHandle * st, int p) ;


/*
 * Return a string starting at position p. First position is at "1". Put "0" or a number greater than StringLength and your program will 
 *  most likely segfault.
 */
char * StringAt( StrHandle * st , int p) ;


/*
 * add backslash to following characters: backslash,dolla sign, open bracket and double quotes.
 * 
 */
void StringSanitize(StrHandle *st) ;


#ifdef __cplusplus
}
#endif

#endif