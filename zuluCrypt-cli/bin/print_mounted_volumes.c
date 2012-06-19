 
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

#include <libcryptsetup.h>
/*
 * below header file does not ship with the source code, it is created at configure time 
 */
#include "../libmount_header.h"

/*
 * defined at ../lib/status.c 
 */
char * volume_device_name( const char * ) ;

static const char * substitute_chars( string_t st )
{
	StringReplaceString( st,"\\012","\n" ) ;			
	StringReplaceString( st,"\\040"," " ) ;
	StringReplaceString( st,"\\134","\\" ) ;
	return StringReplaceString( st,"\\011","\\t" ) ;
}

static void print( uid_t uid,stringList_t stl )
{
	const char * c ;	
	const char * d ;
	const char * e ;
	char * f ;
	
	size_t len ;
	size_t j ;
	size_t i ;
	ssize_t k ;
	
	stringList_t stx ;
	
	string_t q ; 

	string_t p = StringIntToString( uid ) ;
	
	StringPrepend( p,"/zuluCrypt-" ) ;
	StringPrepend( p,crypt_get_dir() ) ;
	
	len = StringLength( p ) ;
	e = StringContent( p ) ;
	
	j = StringListSize( stl )  ;
	
	for( i = 0 ; i < j ; i++ ){
		
		c = StringListContentAt( stl,i ) ;
		
		if( strncmp( c,e,len ) == 0 ){
			
			stx = StringListSplit( c,' ' ) ;
			
			if( stx == NULL )
				continue ;

			if( strncmp( c + len + 1,"UUID",4 ) == 0 ){
				q = StringListStringAt( stx,0 ) ;
				k = StringLastIndexOfChar( q,'-' ) ;
				if( k != -1 ){
					StringSubChar( q,k,'\0' ) ;
					c = StringContent( q ) + len + 6 ;
					d = substitute_chars( StringListStringAt( stx,1 ) ) ;
					printf( "UUID=\"%s\"\t%s\n",c,d ) ;	
				}
			}else{
				f = volume_device_name( StringListContentAt( stx,0 ) ) ;
				
				if( f != NULL ){
					d = substitute_chars( StringListStringAt( stx,1 ) ) ;					
					printf( "%s\t%s\n",f,d ) ;			
					free( f ) ;
				}
			}
			
			StringListDelete( &stx ) ;
		}
	}
	
	StringDelete( &p ) ;
}

static stringList_t get_mtab_list( void )
{
#if USE_NEW_LIBMOUNT_API
	struct libmnt_lock * m_lock ;
#else
	mnt_lock * m_lock ;
#endif
	string_t q = NULL ;
	stringList_t stl ;
	
	char * path = realpath( "/etc/mtab",NULL ) ;
	
	if( path == NULL )
		return NULL ;
	
	if( strncmp( path,"/proc/",6 ) == 0 ){
		q = StringGetFromVirtualFile( path ) ;
	}else{
		m_lock = mnt_new_lock( "/etc/mtab~",getpid() ) ;
		
		if( mnt_lock_file( m_lock ) == 0 ){
			q = StringGetFromFile( path ) ;		
			mnt_unlock_file( m_lock ) ;
		}		

		mnt_free_lock( m_lock ) ;		
	}
	
	free( path ) ;
	
	if( q == NULL )
		return NULL ;
	
	stl = StringListStringSplit( &q,'\n' ) ;
	
	if( stl == NULL ){
		StringDelete( &q ) ;
		return NULL ;
	}
	
	return stl ;
}

int print_opened_volumes( uid_t uid )
{
	stringList_t stl = get_mtab_list() ;
	
	if( stl == NULL )
		return 1 ;
	
	print( uid,stl ) ;
	
	StringListDelete( &stl ) ;
	
	return 0 ;
}

int check_if_mounted( const char * path )
{
	int st = 0 ;
	
	size_t i ;
	size_t j ;
	ssize_t k ;
	
	string_t entry ;
	
	const char * e ;
	
	stringList_t stl = get_mtab_list() ;
	
	if( stl == NULL )
		return 2 ;
	
	j = StringListSize( stl ) ;
	
	for( i = 0 ; i < j ; i++ ){
		entry = StringListStringAt( stl,i ) ;
		
		k = StringIndexOfChar( entry,0,' ' ) ;
		
		if( k != -1 ){
			e = StringSubChar( entry,k,'\0' ) ;
			
			if( strcmp( e,path ) == 0 ){
				st = 1 ;
				break ;
			}
		}
	}
	
	StringListDelete( &stl ) ;
	return st ;
}

