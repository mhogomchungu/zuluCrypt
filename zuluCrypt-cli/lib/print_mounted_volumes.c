 
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
#include "../string/StringList.h"

#include <libcryptsetup.h>
/*
 * below header file does not ship with the source code, it is created at configure time 
 */
#include "libmount_header.h"

/*
 * defined at ../lib/status.c 
 */
char * zuluCryptVolumeDeviceName( const char * ) ;

int zuluCryptMtabIsAtEtc( void )
{
	const char * mpath = "/etc/mtab" ;
	char * path = realpath( mpath,NULL ) ;
	int st ;
	
	if( path == NULL ){
		return 1 ;
	}else{
		st = strcmp( path,mpath ) ;
		
		free( path ) ;
		
		return st == 0 ? 0 : 1 ;
	}
}

const char * zuluCryptDecodeMtabEntry( string_t st )
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
	
	e = StringMultiplePrepend( p,"/zuluCrypt-",crypt_get_dir(),'\0' ) ;
	
	len = StringLength( p ) ;
	
	j = StringListSize( stl )  ;
	
	for( i = 0 ; i < j ; i++ ){
		
		c = StringListContentAt( stl,i ) ;
		
		if( strncmp( c,e,len ) == 0 ){
			
			stx = StringListSplit( c,' ' ) ;
			
			if( stx == StringListVoid )
				continue ;

			if( strncmp( c + len + 1,"UUID",4 ) == 0 ){
				q = StringListStringAt( stx,0 ) ;
				k = StringLastIndexOfChar( q,'-' ) ;
				if( k != -1 ){
					c = StringSubChar( q,k,'\0' ) + len + 6 ;
					d = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;
					printf( "UUID=\"%s\"\t%s\n",c,d ) ;	
				}
			}else{
				f = zuluCryptVolumeDeviceName( StringListContentAt( stx,0 ) ) ;
				
				if( f != NULL ){
					d = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;					
					printf( "%s\t%s\n",f,d ) ;			
					free( f ) ;
				}
			}
			
			StringListDelete( &stx ) ;
		}
	}
	
	StringDelete( &p ) ;
}

stringList_t zuluCryptGetMtabList( void )
{
#if USE_NEW_LIBMOUNT_API
	struct libmnt_lock * m_lock ;
#else
	mnt_lock * m_lock ;
#endif
	string_t q       = StringVoid     ;
	stringList_t stl = StringListVoid ;

	if( zuluCryptMtabIsAtEtc() != 0 ){
		q = StringGetFromVirtualFile( "/proc/mounts" ) ;
	}else{
		m_lock = mnt_new_lock( "/etc/mtab~",getpid() ) ;
		
		if( mnt_lock_file( m_lock ) == 0 ){
			q = StringGetFromFile( "/etc/mtab" ) ;		
			mnt_unlock_file( m_lock ) ;
		}		

		mnt_free_lock( m_lock ) ;		
	}
	
	if( q == StringVoid )
		return StringListVoid ;
	
	stl = StringListStringSplit( q,'\n' ) ;
	
	StringDelete( &q ) ;
		
	return stl ;
}

int zuluCryptPrintOpenedVolumes( uid_t uid )
{
	stringList_t stl = zuluCryptGetMtabList() ;
	
	if( stl == StringListVoid )
		return 1 ;
	
	print( uid,stl ) ;
	
	StringListDelete( &stl ) ;
	
	return 0 ;
}

char * zuluCryptGetMountPointFromPath( const char * path )
{
	size_t i ;
	size_t j ;
	
	string_t entry ;
	
	const char * e ;
	
	stringList_t stl = zuluCryptGetMtabList() ;
	stringList_t stx ;
	
	if( stl == StringListVoid )
		return NULL ;
	
	j = StringListSize( stl ) ;
	
	for( i = 0 ; i < j ; i++ ){
		e = StringListContentAt( stl,i ) ;
		stx = StringListSplit( e,' ' ) ;
		
		if( stx == StringListVoid )
			continue ;
		
		if( StringListContentAtEqual( stx,0,path ) == 0 ){
			entry = StringListDetachAt( stx,1 ) ;
			zuluCryptDecodeMtabEntry( entry ) ;
			StringListMultipleDelete( &stx,&stl,'\0' ) ;
			return StringDeleteHandle( &entry ) ;
		}else{		
			StringListDelete( &stx ) ;
		}
	}
	
	StringListDelete( &stl ) ;
	
	return NULL ;	
}

int zuluCryptCheckIfMounted( const char * path )
{
	char * p = zuluCryptGetMountPointFromPath( path ) ;
	
	if( p == NULL ){
		return 0 ;
	}else{
		free( p ) ;
		return 1 ;
	}
}

