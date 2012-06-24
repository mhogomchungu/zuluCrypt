
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

#include "String.h"

/*
 * a string memory block gows by below factor when it expands
 */
#define FACTOR 1.5

/*
 * when an empty string is initialized,star it with below buffer size
 */
#define STRING_INIT_SIZE 32

struct StringType
{	/*
	 *the size of the string
	 */	
	size_t size ;
	/*
	 * the size of the string buffer
	 */
	size_t length ;
	/*
	 * pointer to the string
	 */
	char * string ; 
};

static inline char * __StringExpandMemory( string_t st,size_t new_size )
{
	if( new_size + 1 > st->length ) {
		st->length = new_size * FACTOR ; 
		return realloc( st->string,st->length ) ;
	}else
		return st->string ;
}

void StringDelete( string_t * xt )
{
	string_t st = *xt ;
	*xt = NULL ;
	free( st->string ) ;
	free( st ) ;
}

char * StringDeleteHandle( string_t * xt )
{
	char * c ;
	string_t st = *xt ;
	c = st->string ;
	free( st ) ;
	*xt = NULL ;
	return c ;
}

string_t StringCopy( string_t st )
{	
	string_t xt ;
	
	char * c = ( char * ) malloc( sizeof( char ) * ( st->size + 1 ) ) ;
	
	if( c == NULL )
		return NULL ;
	
	xt = ( string_t ) malloc ( sizeof( struct StringType ) ) ;
	
	if( st == NULL ){
		free( c ) ;
		return NULL ;
	}
	
	memcpy( c,st->string,st->size + 1 ) ;
	
	xt->size = st->size ;
	xt->length = st->size ;
	xt->string = c ;
	
	return xt ;
}

string_t String( const char * cstring )
{
	size_t size = strlen( cstring ) ;
	
	string_t st = ( string_t ) malloc ( sizeof( struct StringType ) ) ;
	
	if( st == NULL )
		return NULL ;
	
	if( size < STRING_INIT_SIZE / 2 ){
		
		st->string = ( char * ) malloc( sizeof( char ) * STRING_INIT_SIZE ) ;		
		if ( st->string == NULL )
			return NULL ;
		memcpy( st->string,cstring,size + 1 ) ;
		st->size = size ;
		st->length = STRING_INIT_SIZE ;
		
	}else{	
		st->string = NULL ;
		st->size = size ;
		st->length = 0 ;
	
		st->string = __StringExpandMemory( st,size ) ;
	
		if ( st->string == NULL ){
			free( st ) ;
			return NULL ;
		}
	
		memcpy( st->string,cstring,size + 1 ) ;
	}
	return st ;	
}

void StringReadToBuffer( string_t st,char * buffer,size_t size )
{
	memcpy( buffer,st->string,size ) ;
}

string_t StringInherit( char ** data )
{
	return StringInheritWithSize( data,strlen( *data ) ) ;
}

void StringPrint( string_t st )
{
	printf( "%s",st->string ) ;
}

void StringPrintLine( string_t st ) 
{
	printf( "%s\n",st->string ) ;
}

string_t StringInheritWithSize( char ** data,size_t s )
{
	string_t st = ( string_t ) malloc ( sizeof( struct StringType ) ) ;
	
	if( st == NULL )
		return NULL ;
	
	st->size = s ;	
	st->length = s ;
	st->string = *data ;
	*data = NULL ;
	return st ;	
}

string_t StringWithSize( const char * s,size_t len )
{
	char * c = ( char * ) malloc( sizeof( char ) * ( len + 1 ) ) ;
	memcpy( c,s,len ) ;
	*( c + len ) = '\0' ;
	return StringInheritWithSize( &c,len ) ;
}

ssize_t StringIndexOfString( string_t st,size_t p,const char * s )
{
	char * c = strstr( st->string + p,s ) ;
	
	return c == NULL ? -1 : c - st->string;	
}

ssize_t StringLastIndexOfChar( string_t st,char s ) 
{
	char * c = st->string + st->size  ;
	char * d = st->string ;
	
	while( --c != d )
		if ( *c == s )
			return c - d ;
		
	return -1 ;	
}

ssize_t StringLastIndexOfString( string_t st,const char * s ) 
{
	ssize_t p = -1 ;
	
	size_t len = strlen( s ) ;
	
	char * c ;
	char * d = st->string ;
	char * e = st->string ;
	
	if( len == 0 )
		return -1 ;
	
	while( 1 )
	{
		c = strstr( d,s ) ;
		if( c != NULL )
		{
			p = c - e ;
			d = d + len ;
		}else{
			break ;
		}
	}
	return p ;
}

ssize_t StringIndexOfChar( string_t st,size_t p,char s ) 
{	
	char * c ;
	char d[ 2 ] ;
	d[ 1 ] = '\0' ;
	d[ 0 ] = s ;

	c = strstr( st->string + p,d ) ;
	
	return c == NULL ? -1 : c - st->string ;
}

const char * StringRemoveLength( string_t st,size_t x ,size_t y ) 
{	
	memmove( st->string + x,st->string + x + y,st->size - y - x + 1 ) ;
	
	st->size -= y ;
	
	return st->string ;
}

const char * StringClear( string_t st )
{
	st->size = 0 ;
	st->string[0] = '\0' ;
	return st->string ;
}

const char * StringRemoveRight( string_t st,size_t x ) 
{
	return StringRemoveLength( st,x,st->size - x ) ;
}

const char * StringRemoveLeft( string_t st,size_t x ) 
{
	return StringRemoveLength( st,0,x ) ;
}

const char * StringCrop( string_t st,size_t x,size_t y ) 
{
	memmove( st->string,st->string + x,st->size - x + 1 ) ;
	
	st->size = st->size - x - y ;
	
	*( st->string + st->size ) = '\0';
	
	return st->string ;
}

size_t StringLength( string_t st )
{
	return st->size ;	
}

const char * StringContent( string_t st )
{
	return st->string ;
}

const char ** StringPointer( string_t st ) 
{
	return ( const char ** )&st->string ;
}

char * StringCopyChar( string_t st )
{
	return StringLengthCopy( st,st->size ) ;	
}

char * StringLengthCopy( string_t st,size_t l )
{
	char * c ;

	c = ( char * )malloc( sizeof( char ) * ( l + 1 ) ) ;
	
	if( c == NULL )
		return NULL ;
	
	strncpy( c,st->string,l ) ;
	
	*( c + l ) = '\0' ;
	
	return c ;	
}

int StringEndsWithString( string_t st,const char * s ) 
{
	size_t j = strlen(s) ;
	size_t i = strncmp(st->string + st->size - j, s, j ) ;
	
	if( i == 0 )
		return 0 ;
	else
		return 1 ;	
}

int StringEndsWithChar( string_t st,char s )
{
	if ( * ( st->string + st->size -1 ) == s )
		return 0 ;
	else
		return 1 ;
}

char StringCharAt( string_t st,size_t p )
{
	return * ( st->string + p )  ;
}

const char * StringStringAt( string_t st,size_t p )
{
	return st->string + p ;	
}

const char * StringSubChar( string_t st,size_t x,char s )
{	
	st->string[ x ] = s ;
	return st->string ;
}

static void Stringsrcs__( string_t st,char x,const char * y,size_t p )
{
	size_t i ;
	size_t j ;
	size_t k = strlen( y ) ;
	size_t l = st->size ;	
	char * c = st->string ;
	
	for( j = p ; j < l ; j++ )
	{		
		for( i = 0 ; i < k ; i++ )
		{		
			if( * ( c + j ) == * ( y + i ) )
			{				
				* ( c + j ) = x ;
				break ;
			}
		}	
	}
}

const char * StringReplaceCharStringPos( string_t st,char x,const char * y,size_t p ) 
{
	Stringsrcs__( st,x,y,p ) ;
	return st->string ;
}

const char * StringReplaceCharString( string_t st,char x,const char * y ) 
{
	return StringReplaceCharStringPos( st,x,y,0 )  ;
}

const char * StringSubString( string_t st, size_t x,const char * s ) 
{
	memcpy( st->string + x,s,strlen( s ) );
	return st->string ;
}

const char * StringInsertChar( string_t st,size_t x,char s ) 
{
	char c[2] ;
	c[0] = s ;
	c[1] = '\0' ;
	return StringInsertString( st,x,c ) ;
}

const char * StringPrepend( string_t st,const char * s )
{
	char * c ;	
	size_t len = strlen( s ) ;
	
	c = __StringExpandMemory( st,st->size + len ) ;
	
	if( c != NULL )	{
		st->string = c ;
		memmove( st->string + len,st->string,st->size + 1 ) ;
		memcpy( st->string,s,len ) ;
		st->size += len ;
	}
	return c ;	
}

const char * StringPrependString( string_t st,string_t xt ) 
{
	return StringPrepend( st,xt->string ) ;
}

const char * StringPrependChar( string_t st,char c )
{
	char s[ 2 ] ;
	s[ 1 ] = '\0' ;
	s[ 0 ] = c ;
	return StringPrepend( st,s ) ;
}

const char * StringAppend( string_t st,const char * s ) 
{
	char * c ;	
	size_t len = strlen( s ) ;

	c = __StringExpandMemory( st,st->size + len ) ;
	
	if( c != NULL )	{
		st->string = c ;
		memcpy( st->string + st->size,s,len + 1 ) ;
		st->size += len ;
	}
	return c ;
}

const char * StringMultipleAppend( string_t st,... ) 
{
	const char * entry ;
	va_list list ;
	va_start( list,st ) ;
	
	while( 1 ){
		entry = va_arg( list,const char * ) ;
		if( entry == '\0' )
			break ;
		StringAppend( st,entry ) ;
	}
	
	va_end( list ) ;
	return st->string ;
}

const char * StringAppendMultipleString( string_t st,... ) 
{
	string_t entry ;
	va_list list ;
	va_start( list,st ) ;
	
	while( 1 ){
		entry = va_arg( list,string_t ) ;
		if( entry == '\0' )
			break ;
		StringAppend( st,entry->string ) ;
	}
	
	va_end( list ) ;
	return st->string ;
}

const char * StringMultiplePrepend( string_t st,... )  
{
	const char * entry ;
	va_list list ;
	va_start( list,st ) ;
	
	while( 1 ){
		entry = va_arg( list,const char * ) ;
		if( entry == '\0' )
			break ;
		StringPrepend( st,entry ) ;
	}
	
	va_end( list ) ;
	return st->string ;
}

const char * StringPrependMultipleString( string_t st,... )  
{
	string_t entry ;
	va_list list ;
	va_start( list,st ) ;
	
	while( 1 ){
		entry = va_arg( list,string_t ) ;
		if( entry == '\0' )
			break ;
		StringPrepend( st,entry->string ) ;
	}
	
	va_end( list ) ;
	return st->string ;
}

const char * StringAppendString( string_t st,string_t xt )  
{
	return StringAppend( st,xt->string ) ;
}

const char * StringAppendChar( string_t st,char c )
{
	char * d = __StringExpandMemory( st,st->size + 1 ) ;
	
	if( d != NULL ){
		st->string[ st->size ] = c ;
		st->string[ st->size + 1 ] = '\0' ;
		st->size += 1 ;
	}
	
	return d ;
}

const char * StringInsertString( string_t st,size_t x,const char * s )
{
	char * c ;	
	size_t len = strlen( s ) ;
	
	c = __StringExpandMemory( st,len ) ;
	
	if( c != NULL )	{
		st->string = c ;
		memmove( st->string + len + x,st->string + x,st->size - x + 1 ) ;
		memcpy( st->string + x,s,len ) ;
		st->size += len ;
	}
	return c ;	
}

string_t StringMidString( string_t st,size_t x,size_t y ) 
{
	char * c ;
	
	c = ( char * ) malloc ( sizeof( char ) * ( y + 1 ) ) ;
	if( c == NULL )
		return NULL ;
	strncpy( c,st->string + x, y ) ;
	
	*( c + y ) = '\0' ;

	return StringInheritWithSize( &c,y ) ;
}

static char * StringRS__( string_t st,const char * x,const char * s,size_t p )
{
	char * c ;
	char * d = st->string ;
	char * e = st->string + p ;

	size_t j = strlen( s ) ;
	size_t k = strlen( x ) ;
	size_t len ;
	size_t diff ;
	
	if( j == k )
	{
		while( ( c = strstr( e,x ) ) != NULL )
		{
			memcpy( c,s,j ) ;
			e = e + j ;			
		}		
	}else if( j > k  ){
		while( ( c = strstr( e,x ) ) != NULL )
		{
			len = c - st->string ;
			d = __StringExpandMemory( st,st->size + j ) ;
			if( d != NULL )
			{	
				st->string = d ;
				c = st->string + len ;					
				memmove( c + j,c + k,st->size - ( c - st->string ) + 1 ) ;				
				memcpy( c,s,j ) ;
				st->size = st->size + j - k ;		
				e = st->string + len + j ;
			}
		}
	}else if( k > j ){
		while( ( c = strstr( e,x ) ) != NULL )
		{
			len = c - st->string ; 
			diff = k - j ;
			memmove( c + j,c + k,st->size - ( c - st->string + k ) + 1 ) ;			
			memcpy( c,s,j ) ;
			d = __StringExpandMemory( st,st->size - diff ) ;
			if( d != NULL )
			{
				st->string = d ;
				st->size = st->size - diff ;
				e = st->string + len ;
			}
		}
	}
	
	return d ;
}

const char * StringReplaceStringPos( string_t st,const char * x,const char * s,size_t p ) 
{
	return StringRS__( st,x,s,p ) ;
}

const char * StringReplaceString( string_t st,const char * x,const char * s ) 
{
	return StringReplaceStringPos( st,x,s,0 ) ;
}

const char * StringRemoveStringPos( string_t st,const char * s,size_t p ) 
{
	return StringRS__( st,s,"",p ) ;
}

const char * StringRemoveString( string_t st,const char * s )
{
	return StringRemoveStringPos( st,s,0 ) ;
}

static char * StringCRC__( string_t st, char x,char y,size_t p )
{
	char * c = st->string - 1 + p ;	
	while ( *++c  )
		if( *c == x )
			*c = y ;
	return st->string ;
}


const char * StringReplaceCharPos( string_t st,char x,char y,size_t p )
{	
	return StringCRC__( st,x,y,p ) ;
}

const char * StringReplaceChar( string_t st,char x,char y )
{
	return StringReplaceCharPos( st,x,y,0 ) ;
}

string_t StringIntToString( uint64_t z )
{
	/*
	 *  2^64 has a maximum of 19 digits,64 byte buffer is more that enough
	 */
	char buffer[ 64 ] ;
	buffer[ 63 ] = '\0' ;
	int i = 63 ;

	do{
		i-- ;
		buffer[ i ] = z % 10 + '0' ;
		z = z / 10 ;		
	}while( z != 0 && i != 0 ) ;
	
	return String( buffer + i ) ;
}

char * StringIntToString_1( char * x,size_t y,uint64_t z )
{
	char *c =  x + y - 1  ;
	
	*c-- = '\0' ;   
	
	do{
		*c-- = z % 10 + '0' ;
		z = z / 10 ;            
		
	}while( z != 0 ) ;
	
	return ++c ;
}

int StringCompare( string_t x,string_t y ) 
{
	if( x->size != y->size )
		return 1 ;
	else if( strcmp( x->string,y->string ) != 0 )
		return 1 ;
	else
		return 0 ;	
}

int StringCompareString( string_t x,const char * y )
{
	return strcmp( x->string,y ) == 0 ? 0 : 1 ;	
}

static char * StringICS__( string_t st,char x,const char * s,size_t p )
{
	const char * d = s - 1 ;
	char * e ;
	char * f  ;
	size_t pos ;
	
	while( *++d )
	{
		f = st->string - 1 + p ;
		while( *++f )
		{
			if( *d == *f )
			{
				pos = f - st->string ;
				e = __StringExpandMemory( st,st->size + 2 ) ;
				if( e != NULL )
				{					
					st->string = e ;
					memmove( e + pos + 1,e + pos,st->size - pos + 1 ) ;
					*( e + pos ) = x ;
					f = e + pos + 1 ;					
					st->size++ ;					
				}
			}			
		}
	}
	return st->string ;
}

const char * StringInsertCharStringPos( string_t st,char x,const char * s,size_t p ) 
{	
	return StringICS__( st,x,s,p ) ;
}

const char * StringInsertCharString( string_t st,char x,const char * s ) 
{
 	return StringInsertCharStringPos( st,x,s,0 ) ;
}

const char * StringInsertCharChar( string_t st,char x,char y )
{
	char c[2] ;
	c[0] = y ;
	c[1] = '\0' ;
	return StringInsertCharString( st,x,c ) ;
}

int StringGetFromFile_1( string_t * str,const char * path ) 
{
	struct stat st ;
	int fd ;
	char * c ;
	
	if( stat( path,&st ) != 0 )
		return 1 ;
	
	c = ( char * ) malloc( sizeof( char ) * ( st.st_size + 1 ) ) ; 
	
	if( c == NULL )  
		return 3 ;
	
	if( ( fd = open( path,O_RDONLY ) ) == -1 )
		return 2 ;	

	*( c + st.st_size ) = '\0' ;
	
	read( fd,c,st.st_size ) ;
	
	close( fd ) ;

	*str = StringInheritWithSize( &c,st.st_size ) ;
	if( *str == NULL )
	{
		free( c ) ;
		return 3 ;
	}
	return 0 ;
}

string_t StringGetFromFile_2( const char * path,int *  status ) 
{
	string_t st = NULL ;
	*status = StringGetFromFile_1( &st,path ) ;
	return st ; 
}

string_t StringGetFromFile( const char * path )
{
	string_t st = NULL ;
	StringGetFromFile_1( &st,path ) ;
	return st ;
}

void StringWriteToFile( string_t st,const char * path,int mode ) 
{
	int fd ;

	if( mode == 1 )
		fd = open( path, O_WRONLY | O_CREAT | O_TRUNC ) ;
	else
		fd = open( path, O_WRONLY | O_CREAT | O_APPEND ) ;
	
	write( fd,st->string,st->size ) ;
	
	close( fd ) ;
	
	chown( path,getuid(),getgid() ) ;
	chmod( path,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ) ;
}

string_t StringGetFromVirtualFile( const char * path ) 
{
	#define SIZE 64
	
	char * c ;	
	char * d ;
	
	ssize_t i = -1 ;
	ssize_t j ;
	size_t size = SIZE ;
	
	FILE * f = fopen( path,"r" ) ;
	
	if( f == NULL )
		return NULL ;
	
	c = ( char * ) malloc( sizeof( char ) * SIZE ) ;
	
	if( c == NULL )
	{
		fclose( f ) ;
		return NULL ;
	}		
	
	while( ( j = getc( f ) ) != EOF )
	{
		i++ ;

		if( i < size )
			c[ i ] = ( char ) j ;
		else{
			d = c ;			
			size += SIZE ;
			c = realloc( c,size ) ;
			if( c == NULL ){
				fclose( f ) ;
				free( d ) ;
				return NULL ;
			}else{
				c[ i ] = ( char ) j ;
			}
		}
	}
	
	fclose( f ) ;
	c = realloc( c,i + 2 ) ;	
	c[ i + 1 ] = '\0' ;	
	
	return StringInheritWithSize( &c,i ) ;
}
