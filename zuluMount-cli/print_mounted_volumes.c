 
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

#include <mntent.h>
#include <sys/statvfs.h>
#include <blkid/blkid.h>
#include <stdint.h>
#include <libcryptsetup.h>

#include "../zuluCrypt-cli/libmount_header.h"
#include "../zuluCrypt-cli/string/String.h"
#include "../zuluCrypt-cli/string/StringList.h"
#include "../zuluCrypt-cli/process/process.h"

stringList_t mount_partitionList( void ) ;

#ifdef __STDC__
char * realpath( const char * path, char * resolved_path ) ;
#endif

int mtab_is_at_etc( void ) ;

char * volume_device_name( const char * ) ;

int mtab_is_at_etc( void )
{
	char * path = realpath( "/etc/mtab",NULL ) ;
	int st ;
	
	if( path == NULL ){
		return 1 ;
	}else{
		st = strcmp( path,"/etc/mtab" ) ;
		
		free( path ) ;
		
		return st == 0 ? 0 : 1 ;
	}
}

static const char * substitute_chars( string_t st )
{
	StringReplaceString( st,"\\012","\n" ) ;			
	StringReplaceString( st,"\\040"," " ) ;
	StringReplaceString( st,"\\134","\\" ) ;
	return StringReplaceString( st,"\\011","\\t" ) ;
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

	if( mtab_is_at_etc() != 0 ){
		q = StringGetFromVirtualFile( "/proc/mounts" ) ;
	}else{
		m_lock = mnt_new_lock( "/etc/mtab~",getpid() ) ;
		
		if( mnt_lock_file( m_lock ) == 0 ){
			q = StringGetFromFile( "/etc/mtab" ) ;		
			mnt_unlock_file( m_lock ) ;
		}		

		mnt_free_lock( m_lock ) ;		
	}
	
	if( q == NULL )
		return NULL ;
	
	stl = StringListStringSplit( &q,'\n' ) ;
	
	if( stl == NULL ){
		StringDelete( &q ) ;
		return NULL ;
	}
	
	return stl ;
}

static void format_size_1( char * buffer,int x,int y,const char * z )
{
	buffer[ x ] = buffer[ y ] ; 
	buffer[ y ] = '.' ;
	strcpy( buffer + x + 1,z ) ; 
}

static void format_size( char * buffer,const char * buff )
{
	strcpy( buffer,buff ) ;
	
	switch( strlen( buff ) ){
		case 0 : 
		case 1 :  
		case 2 : 
		case 3 : strcat( buffer," B" )             ; break ;
		case 4 : format_size_1( buffer,2,1," KB" ) ; break ; 
		case 5 : format_size_1( buffer,3,2," KB" ) ; break ;
		case 6 : format_size_1( buffer,4,3," KB" ) ; break ;
		case 7 : format_size_1( buffer,2,1," MB" ) ; break ; 
		case 8 : format_size_1( buffer,3,2," MB" ) ; break ;
		case 9 : format_size_1( buffer,4,3," MB" ) ; break ;
		case 10: format_size_1( buffer,2,1," GB" ) ; break ;
		case 11: format_size_1( buffer,3,2," GB" ) ; break ;
		case 12: format_size_1( buffer,4,3," GB" ) ; break ;
		case 13: format_size_1( buffer,2,1," TB" ) ; break ;
		case 14: format_size_1( buffer,3,2," TB" ) ; break ;
		case 15: format_size_1( buffer,4,3," TB" ) ; break ;			
	}	
}

static void partition_properties( const char * path,const char * m_point )
{
	#define SIZE 64
	
	const char * g ;
	
	blkid_probe blkid ;
	
	struct statvfs vfs ;
	uint64_t total ;
	uint64_t used ;
	uint64_t free ;
	uint32_t block_size ;
	
	char buff[ SIZE ] ;	
	char * buffer = buff ;
	char format[ SIZE ] ;
	
	blkid = blkid_new_probe_from_filename( path ) ;
	
	blkid_do_probe( blkid );
	
	total = blkid_probe_get_size( blkid ) ;
	
	if( blkid_probe_lookup_value( blkid,"TYPE",&g,NULL ) == 0 )
		printf( "\t%s",g ) ;
	else
		printf( "\tNil" ) ;
	
	if( blkid_probe_lookup_value( blkid,"LABEL",&g,NULL ) == 0 )
		printf( "\t%s",g ) ;
	else
		printf( "\tNil" ) ;
	
	blkid_free_probe( blkid );
	
	if( total >= 0 ){
		g = StringIntToString_1( buffer,SIZE,total ) ;
		format_size( format,g ) ;
		printf( "\t%s",format ) ;
	}else{
		printf( "\tNil" ) ;
	}
	
	if( m_point == NULL ){
		printf( "\tNil\tNil\n" ) ;
		return ;
	}
		
	if( statvfs( m_point,&vfs ) != 0 ){
		printf( "\tNil\tNil\n" ) ;
		return ;
	}
	
	block_size = vfs.f_frsize ;
	//total = block_size * vfs.f_blocks  ;
	free =  block_size * vfs.f_bavail  ;
		
	used = total - free ;
		
	g = StringIntToString_1( buffer,SIZE,used ) ;
	format_size( format,g ) ;
	printf( "\t%s",format ) ;
	
	snprintf( buff,SIZE,"%.2f%%",100 * ( ( float ) used / ( float ) total ) ) ;
	printf( "\t%s\n",buff ) ;	
}

int mount_print_mounted_volumes( uid_t uid )
{
	size_t i ;
	size_t j ;
	size_t k ;
	
	char * x ;
	const char * e ;
	const char * f ;
	const char * q ;
	const char * z ;
	
	string_t mapper ;
	
	stringList_t stl = get_mtab_list() ;
	stringList_t stx ;
	stringList_t stz = mount_partitionList() ;
	
	if( stl == NULL )
		return 1;
	
	if( stz == NULL )
		return 1;
	
	mapper = StringIntToString( uid ) ;
	
	z = StringMultiplePrepend( mapper,"/zuluCrypt-",crypt_get_dir(),'\0' ) ;
	
	k = StringLength( mapper ) ;
	
	j = StringListSize( stl ) ;
	
	for( i = 0 ; i < j ; i++ ){
		
		e = StringListContentAt( stl,i ) ;
		
		if( strncmp( e,"/dev/sd",7 ) != 0 )
			if( strncmp( e,"/dev/hd",7 ) != 0 )
				if( strncmp( z,e,k ) != 0 )
					continue ;
			
		stx = StringListSplit( e,' ' ) ;
		
		if( stx == NULL )
			continue ;		
		
		q = StringListContentAt( stx,0 ) ;
		
		if( strncmp( q,e,k ) == 0 ){
		
			x = volume_device_name( q ) ;
			
			if( x != NULL ){
				
				StringListRemoveString( stz,x ) ;				
				f = substitute_chars( StringListStringAt( stx,1 ) ) ;				
				
				printf( "%s\t%s",x,f ) ;				
				partition_properties( x,f ) ;
				
				free( x ) ;
			}	
		}else{			
			StringListRemoveString( stz,q ) ;			
			e = substitute_chars( StringListStringAt( stx,0 ) ) ;
			f = substitute_chars( StringListStringAt( stx,1 ) ) ;
			
			printf( "%s\t%s",e,f ) ;
			partition_properties( e,f ) ;			
		}		
		
		StringListDelete( &stx ) ;
	}
	
	j = StringListSize( stz ) ;
	
	for( i = 0 ; i < j ; i++ ){
		e = StringListContentAt( stz,i ) ;
		printf( "%s\tNil",e ) ;
		partition_properties( e,NULL ) ;		
	}
	
	StringDelete( &mapper ) ;
	StringListMultipleDelete( &stl,&stz,'\0' ) ;
	
	return 0 ;
}

