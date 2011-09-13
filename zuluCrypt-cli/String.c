
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


#include "String.h"


StrHandle * StringCpy(char * data)
{
	StrHandle *str = (StrHandle* )malloc(sizeof(StrHandle));
	
	str->size = strlen(data) ;
	str->first = ( StringBuffer * ) malloc(sizeof(StringBuffer)) ;
	
	str->first->data = (char *) malloc(sizeof(char) * (str->size + 1));
	str->first->size = str->size ;
	
	strcpy(str->first->data,data);
	
	str->current = str->first ;
	str->current->next = NULL ;
	
	return str ;	
}

void StringCat(StrHandle * st , char * data) 
{
	int size = strlen(data) ;	
	
	char * b = ( char * ) malloc ( sizeof(char) * ( st->size + size + 1 ) ) ;
	
	if ( b == NULL )
		return ;
	
	st->size = st->size + size ;
	
	strcpy(b,st->first->data);
	strcat(b,data) ;	
	free(st->first->data);
	
	st->first-> data = b ;	
}

int StringLength(StrHandle * st)
{
	return st->size ;	
}

void StringListCat( StrHandle * st, char * data )
{
	int size = strlen(data) ;
	st->size = st->size + size ;

	st->current = st->current->next = ( StringBuffer * ) malloc(sizeof(StringBuffer)) ;
	
	st->current->data = (char *) malloc(sizeof(char) * ( size + 1 ));
	st->current->size = size ;
	
	strcpy(st->current->data,data);
	st->current->next = NULL;	
}

char * StringCont( StrHandle * st)
{
	return st->first->data ;
}

void StringReadToBuffer( StrHandle * st,char *buffer, int size)
{	
	StringBuffer *b = st -> first ;
	
	int i,j,k ;
	
	for ( i = 0,j=0,k=0 ; i < size ; i++,j++,k++){
		
		if( b->data[j] == '\0'){
			b = b->next ;
			j = 0 ;			
		}			
		buffer[k] = b->data[j] ;		
	}
}

char StringCharAt( StrHandle * st, int p)
{
	StringBuffer *b = st -> first ;
	
	int i,j ;
	
	for ( i = 0,j=0 ; i < p ; i++,j++){
		
		if( b->data[j] == '\0'){
			b = b->next ;
			j = 0 ;			
		}			
	}
	return b -> data[j-1] ;	
}

char * StringAt( StrHandle * st , int p)
{
	StringBuffer *b = st -> first ;
	
	int i,j ;
	
	for ( i = 0,j=0 ; i < p ; i++,j++){
		
		if( b->data[j] == '\0'){
			b = b->next ;
			j = 0 ;			
		}		
	}	
	return &(b->data[j-1]) ;	
}

void StringDelete(StrHandle * st)
{	
	StringBuffer *x = st->first ;
	
	while ( x != NULL ){
		free( x -> data);
		x = x->next ;
	}	
	free(st);
}


