 
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

#include "libmount_header.h"
#include "../zuluCrypt-cli/string/String.h"
#include "../zuluCrypt-cli/string/StringList.h"
#include "../zuluCrypt-cli/process/process.h"

stringList_t zuluCryptPartitionList( void ) ;

stringList_t zuluCryptGetMtabList( void ) ;

void zuluCryptFormatSize( char * buffer,const char * buff ) ;

const char * zuluCryptDecodeMtabEntry( string_t st ) ;

char * zuluCryptVolumeDeviceName( const char * ) ;

#ifdef __STDC__
char * realpath( const char * path,char * resolved_path ) ;
#endif

void zuluMountPartitionProperties( const char * device,const char * mapper,const char * m_point )
{
	#define SIZE 64
	
	const char * g ;
	
	blkid_probe blkid ;
	
	struct statvfs vfs ;
	uint64_t total ;
	uint64_t used ;
	uint64_t free ;
	uint32_t block_size ;
	int64_t blkid_device_size ;
	
	char buff[ SIZE ] ;	
	char * buffer = buff ;
	char format[ SIZE ] ;
	
	blkid = blkid_new_probe_from_filename( device ) ;
	
	blkid_do_probe( blkid );
	
	blkid_device_size = ( int64_t ) blkid_probe_get_size( blkid ) ;
		
	if( blkid_probe_lookup_value( blkid,"TYPE",&g,NULL ) == 0 ){
		printf( "\t%s",g ) ;
	}else{
		if( strcmp( device,mapper ) == 0 )
			printf( "\tNil" ) ;
		else
			printf( "\tcrypto_PLAIN" ) ;
	}
	
	blkid_free_probe( blkid );
	
	blkid = blkid_new_probe_from_filename( mapper ) ;
	blkid_do_probe( blkid );
	
	if( blkid_probe_lookup_value( blkid,"LABEL",&g,NULL ) == 0 )
		printf( "\t%s",g ) ;
	else
		printf( "\tNil" ) ;
	
	blkid_free_probe( blkid );
		
	if( blkid_device_size == -1 ){
		printf( "\tNil" ) ;		
	}else{
		g = StringIntToString_1( buffer,SIZE,blkid_device_size ) ;
		zuluCryptFormatSize( format,g ) ;
		printf( "\t%s",format ) ;
	}
	
	if( m_point == NULL ){
		printf( "\tNil\n" ) ;
		return ;
	}
		
	if( statvfs( m_point,&vfs ) != 0 ){
		printf( "\tNil\n" ) ;
		return ;
	}
	
	block_size = vfs.f_frsize ;
	
	total = block_size * vfs.f_blocks  ;	
	
	free =  block_size * vfs.f_bavail  ;
		
	used = total - free ;

	snprintf( buff,SIZE,"%.2f%%",100 * ( ( float ) used / ( float ) total ) ) ;
	printf( "\t%s\n",buff ) ;	
}

/*
 * This function takes contents of "/etc/mtab" and "/proc/partitions" and compare them.
 * It first print information about partitions with entries in "/etc/mtab" and then
 * the remaining entries net effect being it prints information about partitions that
 * are mounted first and then print information about partitions that are not mounted. 
 */
int zuluMountPrintMountedVolumes( uid_t uid )
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
	
	stringList_t stx ;
	stringList_t stl ;
	stringList_t stz ;
	
	/*
	 * get_mtab_list() is  defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c
	 * It returns contents of "/etc/mtab"
	 */
	stl = zuluCryptGetMtabList() ;
		
	if( stl == StringListVoid )
		return 1;
	
	/*
	 * partitionList() is defined in ../zuluCrypt-cli/partitions.c
	 * It returns edited contents of "/proc/partitions"
	 */
	stz = zuluCryptPartitionList() ;	
	
	if( stz == StringListVoid ){
		StringListDelete( &stl ) ;
		return 1;
	}
	
	mapper = StringIntToString( uid ) ;
	
	z = StringMultiplePrepend( mapper,"/zuluCrypt-",crypt_get_dir(),'\0' ) ;
	
	k = StringLength( mapper ) ;
	
	j = StringListSize( stl ) ;
	
	/*
	 * This loop prints partitions with entries in "/etc/mtab" 
	 */
	for( i = 0 ; i < j ; i++ ){
		
		e = StringListContentAt( stl,i ) ;
		
		if( strncmp( e,"/dev/sd",7 ) != 0 )
			if( strncmp( e,"/dev/hd",7 ) != 0 )
				if( strncmp( z,e,k ) != 0 )
					continue ;
			
		stx = StringListSplit( e,' ' ) ;
		
		if( stx == StringListVoid )
			continue ;		
		
		q = StringListContentAt( stx,0 ) ;
		
		if( strncmp( q,e,k ) == 0 ){			
			/*
			 * volume_device_name() is defined in ../zuluCrypt-cli/lib/status.c
			 * It takes cryptsetup path in "/dev/mapper" and return a device path associated with
			 * the mapper
			 */
			x = zuluCryptVolumeDeviceName( q ) ;
			
			if( x != NULL ){
				
				StringListRemoveString( stz,x ) ;
				
				/*
				 * only display partitions,no volumes in files,that zuluCrypt territory
				 */
				if( strncmp( x,"/dev/",5 ) == 0 ){
				
					/*
					* substitute_chars() is defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c
					* it decodes space,tab,new line and backslash characters since they are written differently in "/etc/mtab" 
					*/
					f = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;				
				
					printf( "%s\t%s",x,f ) ;				
					zuluMountPartitionProperties( x,q,f ) ;
				}
				
				free( x ) ;
			}	
		}else{			
			StringListRemoveString( stz,q ) ;			
			e = zuluCryptDecodeMtabEntry( StringListStringAt( stx,0 ) ) ;
			f = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;
			
			printf( "%s\t%s",e,f ) ;
			zuluMountPartitionProperties( e,e,f ) ;			
		}		
		
		StringListDelete( &stx ) ;
	}
	
	j = StringListSize( stz ) ;
	
	/*
	 * this loop prints entries that are not in "/etc/mtab" ie not mounted partitions. 
	 */
	for( i = 0 ; i < j ; i++ ){
		e = StringListContentAt( stz,i ) ;
		printf( "%s\tNil",e ) ;
		zuluMountPartitionProperties( e,e,NULL ) ;		
	}
	
	StringDelete( &mapper ) ;
	StringListMultipleDelete( &stl,&stz,'\0' ) ;
	
	return 0 ;
}

