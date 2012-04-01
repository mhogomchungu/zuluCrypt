
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

#ifdef __STRING_DEBUG
	#define DEBUG 1
#else
	#define DEBUG 0
#endif

#if THREAD_SAFE
void STRINGdebug__( string_t st,char * c )
{
	if( st == NULL )
		return ;
	printf("%s: st:%d  st->string:%d  rc:%d rc:%d mutex:%d  text:%s\n",c,(int)st,(int)st->string,*(st->rc),(int)st->rc,(int)st->mutex,st->string );
}
#else
void STRINGdebug__( string_t st,char * c )
{
	if( st == NULL )
		return ;
	printf("%s: st:%d  st->string:%d  rc:%d rc:%d text:%s\n",c,(int)st,(int)st->string,*(st->rc),(int)st->rc,st->string );
}
#endif

inline void StringCNSH__( string_t st,string_t nst )
{
	st->size = nst->size ;
	st->string = nst->string ;
	st->rc = nst->rc ;
#if THREAD_SAFE
	st->mutex = nst->mutex ;
#endif
	free( nst ) ;
}

inline int StringLockMutex__( string_t st )
{
	int rc ;
#if THREAD_SAFE 
	pthread_mutex_lock( st->mutex ) ;
#endif	
	rc = *( st->rc ) ;
	if( rc != 1 )
		*( st->rc ) = *( st->rc) - 1 ;
	return rc ;
}

inline void StringUnlockMutex__( string_t st )
{
#if THREAD_SAFE 	
	pthread_mutex_unlock( st->mutex ) ;
#endif
}

void StringDelete( string_t * xt )
{
	string_t st = *xt ;
	if( st != NULL ) 
	{		
		*xt = NULL ;		
		if( StringLockMutex__( st )  == 1 ){	
#if DEBUG
#if THREAD_SAFE
			printf("deleting string_t:%d string:%d mutex:%d\n",(int)st,(int)st->string,(int)st->mutex) ;
#else
			printf("deleting string_t:%d string:%d \n",(int)st,(int)st->string) ;
#endif			
#endif
#if THREAD_SAFE	
			pthread_mutex_unlock( st->mutex ) ;			
			pthread_mutex_destroy( st->mutex ) ;			
			free( st->mutex );
#endif			
			free( st->string ) ;
			free( st->rc ) ;
		}else{	
#if THREAD_SAFE	
			pthread_mutex_unlock( st->mutex ) ;			
#endif
#if DEBUG
			printf("deleting string_t:%d decrementing rc to:%d  string_t:%d text:%s\n",(int)st,*(st->rc),(int)st->string,st->string) ;

#endif			
		}
		free( st ) ;		
	}	
}

char * StringDeleteHandle( string_t * xt )
{
	char * c  ;
	string_t st = *xt ;
	*xt = NULL ;
	
	if( StringLockMutex__( st ) == 1  ){
#if DEBUG
		printf( "deleting a handle:%d\n",(int)st );
#endif
		c = st->string ;
#if THREAD_SAFE		
		pthread_mutex_unlock( st->mutex ) ;
		pthread_mutex_destroy( st->mutex ) ;
		free( st->mutex ) ;
#endif 
	}else{
#if DEBUG
		printf( "deleting a handle:%d decrementing rc to:%d\n",(int)st,*(st->rc) - 1 );
#endif
		c = StringLengthCopy( st,st->size ) ;
#if THREAD_SAFE	
		pthread_mutex_unlock( st->mutex ) ;
#endif		
	}
	free( st ) ;
	return c ;
}

string_t StringCopy( string_t st )
{
	string_t new_st ;
	
	if( st == NULL )
		return NULL ;
#if THREAD_SAFE	
	pthread_mutex_lock( st->mutex ) ;		
#endif
	*( st->rc ) = *( st->rc) + 1 ;
	
	new_st = ( string_t ) malloc( sizeof( struct StringType ) ) ;
	
	if( new_st == NULL )
		return NULL ;

	new_st->size  = st->size  ;
	
	new_st->string = st->string ;
	
	new_st->rc = st->rc ;
#if THREAD_SAFE
	new_st->mutex = st->mutex ;
	pthread_mutex_unlock( st->mutex ) ;
#endif	
	
#if DEBUG
	printf("creating a new string copy at:%d from %d\n",(int)new_st,(int)st ) ;
#endif
	return new_st ;
}

inline string_t StringPrepare__( size_t size )
{
	string_t st = ( string_t ) malloc ( sizeof( struct StringType ) ) ;
	
	if( st == NULL )
		return NULL ;
	
	st->rc = ( int * ) malloc( sizeof( int ) ) ;
	
	if( st->rc == NULL )
	{
		free( st ) ;
		return NULL ;			
	}

#if THREAD_SAFE
	st->mutex = ( pthread_mutex_t * ) malloc( sizeof( pthread_mutex_t ) ) ;
	if( st->mutex == NULL )
	{
		free( st->rc ) ;
		free( st ) ;
		return NULL ;		
	}
	pthread_mutex_init( st->mutex,NULL ) ;
#endif	
	*( st->rc ) =  1 ;
	
#if DEBUG
	printf("creating a new string at:%d\n",(int)st ) ;
#endif
	return st ;
}

string_t String( const char * c )
{
	size_t size = strlen( c ) ;	
	string_t st = StringPrepare__( size ) ;
	if( st == NULL )
		return NULL ;
	
	st->string = ( char * ) malloc ( sizeof ( char ) * ( size + 1 ) ) ;
	
	if ( st->string == NULL )
	{
#if THREAD_SAFE
		free( st->mutex ) ;
#endif	
		free( st->rc ) ;
		free( st ) ;
		return NULL ;
	}
	st->size = size ;
	memcpy( st->string,c,size + 1 ) ;
	return st ;	
}

void StringReadToBuffer( string_t st,char * buffer, size_t size )
{
	size_t i  ;	
	for ( i = 0 ; i < size ; i++ )
		buffer[i] = st->string[i] ;
}

string_t StringInherit( char ** data )
{
	return StringInheritWithSize( data,strlen( *data ) ) ;
}

string_t StringInheritWithSize( char ** data,size_t s )
{
	string_t st = StringPrepare__( s ) ;
	if( st == NULL )
		return NULL ;
	st->size = s ;	
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

ssize_t StringIndexOfString( string_t st,size_t p, const char * s )
{
	char * c = strstr( st->string + p,s ) ;
	
	if( c == NULL )
		return -1 ;
	else
		return  c - st->string;	
}

ssize_t StringLastIndexOfChar( string_t st , char s ) 
{
	char * c = st->string + st->size  ;
	char * d = st->string ;
	
	while( --c != d )
		if ( *c == s )
			return c - d ;
		
	return -1 ;	
}

ssize_t StringLastIndexOfString( string_t st ,const char * s ) 
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

ssize_t StringIndexOfChar( string_t st, size_t p , char s ) 
{	
	char * c ;
	char d[ 2 ] ;
	d[ 1 ] = '\0' ;
	d[ 0 ] = s ;

	c = strstr( st->string + p,d ) ;
	if( c == NULL )
		return -1 ;
	else
		return c - st->string ;
}

const char * StringRemoveLength( string_t st,size_t x , size_t y ) 
{	
	char * c ;
	char * d ;
	size_t new_size ;
	string_t nst ;
	string_t mt = st ;
	
	if( StringLockMutex__( mt ) == 1 )
	{
		new_size = st->size - y ;
		memmove( st->string + x,st->string + x + y,st->size - y - x + 1 ) ;
		c = realloc( st->string,new_size + 1 ) ;
		if( c != NULL )
		{
			st->string = c ;
			st->size = new_size ;	
		}
	}else{
		new_size = st->size - y ;
		c = d = ( char * ) malloc( sizeof( char ) * ( new_size + 1 ) ) ;
		if( c != NULL )
		{
			strncpy( c,st->string,x );
			strcpy( c + x,st->string + x + y ) ;
			nst = StringInheritWithSize( &d,new_size ) ;
			StringCNSH__( st,nst ) ;
		}
	}
	StringUnlockMutex__( mt ) ;
	return c ;
}

const char * StringClear( string_t st )
{
	char * c ;
	char * d ;
	string_t nst ;
	string_t mt = st ;
	if( StringLockMutex__( mt ) == 1 )
	{
		c = realloc( st->string,1 ) ;
		if( c != NULL )
		{
			st->size = 0 ;
			st->string = c ;
			*( st->string ) = '\0' ;
		}		
	}else{
		d = c = ( char * ) malloc( sizeof( char ) ) ;
		if( c!= NULL )
		{
			*c = '\0' ;
			nst = StringInheritWithSize( &d,1 ) ;
			StringCNSH__( st,nst ) ;
		}
	}
	StringUnlockMutex__( mt ) ;
	return c ;
}

const char * StringRemoveRight( string_t st, size_t x ) 
{
	return StringRemoveLength( st,x,st->size - x ) ;
}

const char * StringRemoveLeft( string_t st, size_t x ) 
{
	return StringRemoveLength( st,0,x ) ;
}

const char * StringCrop( string_t st, size_t x, size_t y ) 
{
	char * c ;	
	char * d ;
	size_t new_size ;
	string_t nst ;
	string_t mt = st ;
	
	if( StringLockMutex__( mt ) == 1 )
	{
		new_size = st->size - x - y ;
		memmove( st->string,st->string + x,st->size - x + 1 ) ;
		c = realloc( st->string,new_size + 1 ) ;
		if( c != NULL )
		{
			st->string = c ;
			*( st->string + new_size ) = '\0';
			st->size = new_size ;
		}
	}else{
		new_size = st->size - x - y ;
		d = c = ( char * ) malloc( sizeof( char ) * ( new_size + 1 ) ) ;
		if( c != NULL )
		{
			strncpy( c,st->string + x,new_size );	
			*( st->string + new_size ) = '\0';		
			nst = StringInheritWithSize( &d,new_size ) ;
			StringCNSH__( st,nst ) ;
			c = st->string ;
		}
	}
	StringUnlockMutex__( mt ) ;
	return c ;
}

size_t StringLength( string_t st )
{
	return st->size ;	
}

const char * StringContent( string_t st )
{
	return st->string ;
}

int StringReferenceCount( string_t st )
{
	return *( st->rc ) ;
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

int StringEndsWithString( string_t st , const char * s ) 
{
	size_t j = strlen(s) ;
	size_t i = strncmp(st->string + st->size - j, s, j ) ;
	
	if( i == 0 )
		return 0 ;
	else
		return 1 ;	
}

int StringEndsWithChar( string_t st ,char s )
{
	if ( * ( st->string + st->size -1 ) == s )
		return 0 ;
	else
		return 1 ;
}

char StringCharAt( string_t st, size_t p )
{
	return * ( st->string + p )  ;
}

const char * StringStringAt( string_t st , size_t p )
{
	return st->string + p ;	
}

const char * StringSubChar( string_t st, size_t x, char s )
{	
	char * c = NULL ;
	char * d ;
	string_t nst;
	string_t mt = st ;
	
	if( StringLockMutex__( mt ) == 1 ){	
		* ( st->string + x ) = s ;
		c = st->string ;
	}else{
		d = c = StringLengthCopy( st,st->size ) ;
		if( c != NULL )
		{
			* ( c + x ) = s ;
			nst = StringInheritWithSize( &d,st->size ) ;
			StringCNSH__( st,nst ) ;
			c = st->string ;
		}
	}
	StringUnlockMutex__( mt ) ;
	return c ;	
}

void Stringsrcs__( string_t st, char x, const char * y,size_t p )
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

const char * StringReplaceCharStringPos( string_t st, char x, const char * y,size_t p ) 
{
	char * c = NULL ;
	char * d ;
	string_t nst ;
	string_t mt = st ;
	
	if( StringLockMutex__( mt ) == 1 )
	{	
		Stringsrcs__( st,x,y,p ) ;
		c = st->string ;
	}else{
		d = c = StringLengthCopy( st,st->size ) ;
		if( c != NULL )
		{
			nst = StringInheritWithSize( &d,st->size ) ;
			Stringsrcs__( nst,x,y,p ) ;		
			StringCNSH__( st,nst ) ;
			c = st->string ;
		}
	}
	StringUnlockMutex__( mt ) ;
	return c ;
}

const char * StringReplaceCharString( string_t st, char x, const char * y ) 
{
	return StringReplaceCharStringPos( st,x,y,0 )  ;
}

const char * StringSubString( string_t st, size_t x, const char * s ) 
{
	string_t nst ;
	char * e = NULL ;
	char * d ;
	string_t mt = st ;
	
	if( StringLockMutex__( mt ) == 1 ){
		memcpy( st->string + x,s,strlen( s ) );
		e = st->string ;
	}else{
		d = e = StringLengthCopy( st,st->size ) ;
		if( e != NULL )
		{
			memcpy( e + x,s,strlen( s ) );		
			nst = StringInheritWithSize( &d,st->size ) ;
			StringCNSH__( st,nst ) ;
			e = st->string ;
		}
	}
	StringUnlockMutex__( mt ) ;
	return  e ;
}

const char * StringInsertChar( string_t st, size_t x,char s ) 
{
	char c[2] ;
	c[0] = s ;
	c[1] = '\0' ;
	return StringInsertString( st, x, c ) ;
}

const char * StringPrepend( string_t st ,const  char * s )
{
	char * c ;	
	char * d ;
	size_t len ;
	size_t new_size ;
	string_t nst ;
	string_t mt = st ;
	
	len = strlen( s ) ;	
	
	if( StringLockMutex__( mt ) == 1 )
	{
		new_size = st->size + len ;
		c = realloc( st->string,new_size + 1 ) ;
		if( c != NULL )
		{
			st->string = c ;
			memmove( st->string + len,st->string,st->size + 1 ) ;
			memcpy( st->string,s,len ) ;
			st->size = new_size ;
		}
	}else{
		new_size = st->size + len ;
		d = c = ( char * ) malloc( sizeof( char ) * ( new_size + 1 ) ) ;
		if( c != NULL )
		{
			memcpy( c,s,len ) ;
			memcpy( c + len,st->string,st->size + 1 ) ;
			nst = StringInheritWithSize( &d,new_size ) ;
			StringCNSH__( st,nst ) ;
		}
	}
	StringUnlockMutex__( mt ) ;
	return c ;	
}

const char * StringPrependChar( string_t st,char c )
{
	char s[ 2 ] ;
	s[ 1 ] = '\0' ;
	s[ 0 ] = c ;
	return StringPrepend( st,s ) ;
}

const char * StringAppend( string_t st ,const char * s ) 
{
	char * c ;	
	char * d ;
	size_t len ;
	size_t new_size ;
	string_t nst ;
	len = strlen( s ) ;
	string_t mt = st ;
	
	if( StringLockMutex__( mt ) == 1 )
	{
		new_size = st->size + len ;		
		c = realloc( st->string,new_size + 1 ) ;
		if( c != NULL )
		{
			st->string = c ;
			strcpy( st->string + st->size,s ) ;
			st->size = new_size ;
		}
	}else{
		new_size = st->size + len ;	
		d = c = ( char * ) malloc( sizeof( char ) * ( new_size + 1 ) ) ;
		if( c != NULL )
		{
			memcpy( c,st->string,st->size ) ;
			memcpy( c + st->size,s,len + 1 ) ;
			nst = StringInheritWithSize( &d,new_size ) ;
			StringCNSH__( st,nst ) ;
		}
	}
	StringUnlockMutex__( mt ) ;
	return c ;
}

const char * StringAppendChar( string_t st,char c )
{
	char s[ 2 ] ;
	s[ 1 ] = '\0' ;
	s[ 0 ] = c ;
	return StringAppend( st,s ) ;
}

const char * StringInsertString( string_t st, size_t x, const char * s )
{
	char * c ;	
	char * d ;
	size_t len ;
	size_t new_size ;
	string_t nst ;
	string_t mt = st ;
	
	len = strlen( s ) ;	
	
	if( StringLockMutex__( mt ) == 1 )
	{
		new_size = st->size + len ;
		c = realloc( st->string,new_size + 1 ) ;
		if( c != NULL )
		{
			st->string = c ;
			memmove( st->string + len + x,st->string + x,st->size - x + 1 ) ;
			memcpy( st->string + x,s,len ) ;
			st->size = new_size ;
		}
	}else{
		new_size = st->size + len ;
		d = c = ( char * ) malloc( sizeof( char ) * ( new_size + 1 ) ) ;
		if( c != NULL )
		{
			strncpy( c,st->string,x ) ;
			strcpy( c + x,s ) ;
			strcpy( c + x + len,st->string + x ) ;		
			nst = StringInheritWithSize( &d,new_size ) ;
			StringCNSH__( st,nst ) ;
		}
	}
	StringUnlockMutex__( mt ) ;
	return c ;	
}

string_t StringMidString( string_t st , size_t x, size_t y ) 
{
	char * c ;
	
	c = ( char * ) malloc ( sizeof( char ) * ( y + 1 ) ) ;
	if( c == NULL )
		return NULL ;
	strncpy( c,st->string + x, y ) ;
	
	*( c + y ) = '\0' ;

	return StringInheritWithSize( &c,y ) ;
}

char * StringRS__( string_t st, const char * x, const char * s,size_t p )
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
		while( ( c = strstr( e, x ) ) != NULL )
		{
			memcpy( c,s,j ) ;
			e = e + j ;			
		}		
	}else if( j > k  ){
		while( ( c = strstr( e, x ) ) != NULL )
		{
			len = c - st->string ;
			d = realloc( st->string, st->size + j + 1 ) ;
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
		while( ( c = strstr( e, x ) ) != NULL )
		{
			len = c - st->string ; 
			diff = k - j ;
			memmove( c + j,c + k,st->size - ( c - st->string + k ) + 1 ) ;			
			memcpy( c,s,j ) ;
			d = realloc( st->string,st->size - diff + 1 ) ;
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

const char * StringReplaceStringPos( string_t st, const char * x, const char * s,size_t p ) 
{
	char * c ;
	char * d ;
	string_t nst ;
	string_t mt = st ;
	
	if( StringLockMutex__( mt ) == 1 )
	{
		d = StringRS__( st,x,s,p ) ;		
	}else{
		c = d = ( char * ) malloc( sizeof( char ) * ( st->size + 1 ) ) ;

		if( d != NULL )
		{
			memcpy( d,st->string,st->size + 1 ) ;
			nst = StringInheritWithSize( &c,st->size ) ;
			StringCNSH__( st,nst ) ;
			d = StringRS__( st,x,s,p ) ;
		}
	}	
	StringUnlockMutex__( mt ) ;
	return d ;
}

const char * StringReplaceString( string_t st, const char * x, const char * s ) 
{
	return StringReplaceStringPos( st,x,s,0 ) ;
}

const char * StringRemoveStringPos( string_t st, const char * s,size_t p ) 
{
	char * c ;
	char * d = NULL ;
	string_t nst ;
	string_t mt = st ;
	
	if( StringLockMutex__( mt ) == 1 )
	{
		d = StringRS__( st,s,"",p ) ;
	}else{
		c = d = ( char * ) malloc( sizeof( char ) * ( st->size + 1 ) ) ;
		
		if( d != NULL )
		{
			memcpy( d,st->string,st->size + 1 ) ;
			nst = StringInheritWithSize( &c,st->size ) ;
			StringCNSH__( st,nst ) ;
			d = StringRS__( st,s,"",p ) ;
		}		
	}
	StringUnlockMutex__( mt ) ;
	return d ;
}

const char * StringRemoveString( string_t st, const char * s )
{
	return StringRemoveStringPos( st,s,0 ) ;
}

char * StringRC__( string_t st, char x, char y,size_t p )
{
	char * c = st->string - 1 + p ;	
	while ( *++c  )
		if( *c == x )
			*c = y ;
	return st->string ;
}

const char * StringReplaceCharPos( string_t st, char x, char y,size_t p )
{	
	char * c = NULL ;
	char * d ;
	string_t nst ;
	string_t mt = st ;
	
	if( StringLockMutex__( mt ) == 1 ){	
		c = StringRC__( st,x,y,p ) ;
	}else{
		d = c = StringLengthCopy( st,st->size ) ;
		if( c != NULL )
		{
			nst = StringInheritWithSize( &d,st->size ) ;
			c = StringRC__( nst,x,y,p ) ;		
			StringCNSH__( st,nst ) ;
		}
	}
	StringUnlockMutex__( mt ) ;
	return c ;
}

const char * StringReplaceChar( string_t st, char x, char y )
{
	return StringReplaceCharPos( st,x,y,0 ) ;
}

string_t StringIntToString( uint64_t z )
{
	string_t st = String( "" ) ;
	
	do{
		StringPrependChar( st,z % 10 + '0' ) ;
		z = z / 10 ;		
	}while( z != 0 ) ;
	
	return st ;
}

char * StringIntToString_1( char * x, size_t y,uint64_t z )
{
	char *c =  x + y - 1  ;
	
	*c-- = '\0' ;   
	
	do{
		*c-- = z % 10 + '0' ;
		z = z / 10 ;            
		
	}while( z != 0 ) ;
	
	return ++c ;
}

int StringCompare( string_t x , string_t y ) 
{
	if( x->size != y->size )
		return 1 ;
	else if( strcmp( x->string, y->string ) != 0 )
		return 1 ;
	else
		return 0 ;	
}

int StringCompareString( string_t x, const char * y )
{
	if( strcmp( x->string, y ) == 0 )
		return 0 ;
	else
		return 1 ;	
}

inline char * StringICS__( string_t st, char x, const char * s,size_t p )
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
				e = realloc( st->string,st->size + 2 ) ;
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

const char * StringInsertCharStringPos( string_t st, char x, const char * s,size_t p ) 
{	
	char * d = NULL ;
	char * c ;
	string_t nst ;
	string_t mt = st ;
	
	if( StringLockMutex__( mt ) == 1 )
	{
		d = StringICS__( st,x,s,p ) ;
	}else{
		c = d = ( char *) malloc( sizeof( char ) * ( st->size + 1 ) ) ;	
		
		if( d != NULL )
		{
			memcpy( d,st->string,st->size + 1 ) ;			
			nst = StringInheritWithSize( &c,st->size ) ;
			d = StringICS__( nst,x,s,p ) ;
			StringCNSH__( st,nst ) ;
		}
	}
	StringUnlockMutex__( mt ) ;	
	return d ;
}

const char * StringInsertCharString( string_t st, char x, const char * s ) 
{
 	return StringInsertCharStringPos( st,x,s,0 ) ;
}

const char * StringInsertCharChar( string_t st, char x, char y )
{
	char c[2] ;
	c[0] = y ;
	c[1] = '\0' ;
	return StringInsertCharString( st, x, c ) ;
}

int StringGetFromFile_1( string_t * str,const char * path ) 
{
	struct stat st ;
	int fd ;
	char * c ;
	
	if( stat( path,&st ) != 0 )
		return 1 ;
	
	if( ( fd = open( path,O_RDONLY ) ) == -1 )
		return 2 ;	
	
	c = ( char * ) malloc( sizeof( char ) * ( st.st_size + 1 ) ) ; 

	if(  c == NULL )  
		return 3 ;

	*( c + st.st_size ) = '\0' ;
	
	read( fd,c,st.st_size ) ;
	
	close( fd ) ;

	*str = StringInheritWithSize( &c, st.st_size ) ;
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

void StringWriteToFile( string_t st,const char * path, int mode ) 
{
	size_t size = st->size ;
	int fd ;
#if THREAD_SAFE
	pthread_mutex_lock( st->mutex ) ;
#endif
	if( mode == 1 )
		fd = open( path, O_WRONLY | O_CREAT | O_TRUNC ) ;
	else
		fd = open( path, O_WRONLY | O_CREAT | O_APPEND ) ;
	
	do{		
		size = size - write(fd,st->string,size ) ;
	}while( size != 0 ) ;
	
	close( fd ) ;
#if THREAD_SAFE
	pthread_mutex_unlock( st->mutex ) ;
#endif	
	chown( path,getuid(),getgid() ) ;
	chmod( path,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ) ;
}

string_t StringGetFromVirtualFile( const char * path ) 
{
	int c ;
	
	string_t st = String( "" ) ;
	
	char s[ 2 ] ;	
	
	FILE * f = fopen( path,"r" ) ;
	
	if( f == NULL )
		return NULL ;
	
	s[ 1 ] = '\0' ;
	
	while( ( c = getc( f ) ) != EOF )
	{
		s[ 0 ] = ( char ) c ;
		StringAppend( st,s ) ;
	}
	
	fclose( f ) ;
	
	return st ;
}
