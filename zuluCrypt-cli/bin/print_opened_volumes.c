 
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

/*
 * defined at ../lib/status.c 
 */
char * volume_device_name( const char * ) ;

static void print_UUID( const char * entry )
{
	const char * c = entry - 1 ;
	
	printf( "UUID=\"" ) ;

	while( *++c != ' ' )
		printf( "%c",*c ) ;
	
	printf( "\"\t" ) ;
	
	while( *++c != ' ' )
		printf( "%c",*c ) ;
	printf( "\n" ) ;	
}

static void print_NAAN( const char * entry )
{
	char * volume ;
	const char * c = entry - 1;

	string_t p = String( "" ) ;
	
	while( *++c != ' ' )
		StringAppendChar( p,*c ) ; 

	volume = volume_device_name( StringContent( p ) ) ;
	
	if( volume == NULL )
		return ;
	
	printf( "%s\t",volume ) ;
	
	while( *++c != ' ' )
		printf( "%c",*c ) ;
	
	printf( "\n" );
	StringDelete( &p ) ;
	free( volume ) ;
}

static void print( uid_t uid,stringList_t stl )
{
	const char * entry ;
	const char * c ;
	size_t len ;
	size_t j ;
	size_t i ;
	
	string_t z = StringIntToString( uid ) ;
	string_t p = String( "/dev/mapper/zuluCrypt-" ) ;

	StringAppend( p,StringContent( z ) ) ;
	
	len = StringLength( p ) ;
	entry = StringContent( p ) ;
	
	j = StringListSize( stl )  ;
	
	for( i = 0 ; i < j ; i++ ){
		
		c = StringListContentAt( stl,i ) ;
		
		if( strncmp( c,entry,len ) == 0 ){
			if( strncmp( c + len + 1,"UUID",4 ) == 0 )
				print_UUID( c + len + 6 ) ;
			else
				print_NAAN( c ) ;
		}
	}
	
	StringDelete( &p ) ;
	StringDelete( &z ) ;
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
	
	string_t q ;
	
	stringList_t stl ;
	path = realpath( "/etc/mtab",NULL ) ;
	
	if( path == NULL )
		return 1 ;
	
	if( strncmp( path,"/proc/",6 ) == 0 ){
		q = StringGetFromVirtualFile( path ) ;
	}else{
		m_lock = mnt_new_lock( "/etc/mtab~",getpid() ) ;
		
		if( mnt_lock_file( m_lock ) != 0 )
			return free_return( path,1 ) ;
		
		q = StringGetFromFile( path ) ;	
		
		mnt_unlock_file( m_lock ) ;
		mnt_free_lock( m_lock ) ;		
	}
	
	stl = StringListStringSplit( &q,'\n' ) ;
	
	print( uid,stl ) ;
	
	StringListDelete( &stl ) ;
	
	return free_return( path,0 ) ;
}

