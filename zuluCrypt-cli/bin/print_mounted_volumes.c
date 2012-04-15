 
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

static void print_UUID( const char * mapper,const char * m_point )
{
	printf( "UUID=\"%s\"\t%s\n",mapper,m_point ) ;	
}

static void print_NAAN( const char * mapper,const char * m_point )
{
	char * volume = volume_device_name( mapper ) ;
	
	if( volume == NULL )
		return ;
	
	printf( "%s\t%s\n",volume,m_point ) ;
	
	free( volume ) ;
}

static void print( uid_t uid,stringList_t stl )
{
	const char * d ;
	const char * c ;
	const char * e ;
	
	size_t len ;
	size_t j ;
	size_t i ;
	size_t k ;
	
	stringList_t stx ;
	
	string_t q ; 
	string_t z = StringIntToString( uid ) ;
	string_t p = String( "/dev/mapper/zuluCrypt-" ) ;

	StringAppend( p,StringContent( z ) ) ;
	
	len = StringLength( p ) ;
	e = StringContent( p ) ;
	
	j = StringListSize( stl )  ;
	
	for( i = 0 ; i < j ; i++ ){
		
		c = StringListContentAt( stl,i ) ;
		
		if( strncmp( c,e,len ) == 0 ){
			
			stx = StringListSplit( c,' ' ) ;
			
			if( strncmp( c + len + 1,"UUID",4 ) == 0 ){
				q = StringListStringAt( stx,0 ) ;
				StringRemoveLeft( q,len + 6 ) ;
				k = StringLastIndexOfChar( q,'-' ) ;
				if( k != -1 ){
					StringRemoveRight( q,k ) ;
					c = StringContent( q ) ;
					d = StringListContentAt( stx,1 ) ;					
					print_UUID( c,d ) ;					
				}
			}else{
				c = StringListContentAt( stx,0 ) ;
				d = StringListContentAt( stx,1 ) ;
				print_NAAN( c,d ) ;
			}
			
			StringListDelete( &stx ) ;
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

