 
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

#include "includes.h"

/*
 * below header file does not ship with the source code, it is created at configure time * 
 */
#include "../libmount_header.h"

#define UID_SIZE 6

/*
 * defined at ../lib/status.c * 
 */
char * volume_device_name( const char * ) ;


static void print_UUID( const char * entry )
{
	const char * d = strstr( entry,"-UUID-" ) + 6 ;
	const char * c = d - 1 ;
	
	printf( "UUID=\"" ) ;
	
	while( *++c != ' ' )
		printf("%c",*c ) ;
	
	printf( "\"\t" ) ;
	
	while( *++c != ' ' )
		printf("%c",*c ) ;
	printf( "\n" ) ;	
}

static void print_NAAN( const char * entry )
{
	char * volume ;
	const char * c = entry - 1 ;
	const char * d ;
	string_t p = String( "" ) ;
	char s[ 2 ];
	s[ 1 ] = '\0' ;
	
	while( *++c != ' ' )	{
		*s = *c ;
		StringAppend( p,s ) ; 
	}
	
	d = StringContent( p ) ;

	volume = volume_device_name( d ) ;
	
	printf( "%s\t",volume ) ;
	
	while( *++c != ' ' )
		printf( "%c",*c ) ;
	
	printf( "\n" );
	StringDelete( &p ) ;
	free( volume ) ;
}

static void print( uid_t uid,char * path )
{
	char uid_s[ UID_SIZE ] ;
	char * cuid ;
	int len ;
	const char * entry ;
	const char * c ;
	
	string_t q ;
	string_t p = String( "/dev/mapper/zuluCrypt-" ) ;
	cuid = StringIntToString( uid_s,UID_SIZE,uid ) ;
	StringAppend( p,cuid ) ;
	len = StringLength( p ) ;
	entry = StringContent( p ) ;
	
	q = StringGetFromFile( path ) ;	
	c = StringContent( q ) ;
	
	/*
	 * examine the first line.
	 */
	if( strncmp( c,entry,len ) == 0 ){
		if( strncmp( c + len + 1,"UUID",4 ) == 0 )
			print_UUID( c ) ;
		else
			print_NAAN( c ) ;
	}
	/*
	 * examine the remaining lines.
	 */
	while( ( c = strstr( c,"\n" ) ) != NULL )
	{
		c++ ;
		if( strncmp( c,entry,len ) == 0 ){
			if( strncmp( c + len + 1,"UUID",4 ) == 0 )
				print_UUID( c ) ;
			else
				print_NAAN( c ) ;
		}
	}
	
	StringDelete( &p ) ;
	StringDelete( &q ) ;	
}

static int free_return( char * path,int st )
{
	free( path ) ;
	return st ;
}

int print_opened_volumes( uid_t uid )
{
#if USE_NEW_LIBMOUNT_API
	struct libmnt_lock * m_lock ;
#else
	mnt_lock * m_lock ;
#endif
	char * path ;
	
	path = realpath( "/etc/mtab",NULL ) ;
	
	if( strncmp( path,"/etc/",5 ) == 0 ){
		m_lock = mnt_new_lock( "/etc/mtab~",getpid() ) ;
		
		if( mnt_lock_file( m_lock ) != 0 )
			return free_return( path,1 ) ;
		
		print( uid,path ) ;
		
		mnt_unlock_file( m_lock ) ;
		mnt_free_lock( m_lock ) ;
	}else
		print( uid,path ) ;
	
	return free_return( path,0 ) ;
}

