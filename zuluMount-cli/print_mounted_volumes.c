 
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

/*
 * below header file is generated at configure time
 */
#include "libmount_header.h"

#include "../zuluCrypt-cli/bin/includes.h"

stringList_t zuluCryptPartitionList( void ) ;

stringList_t zuluCryptGetMtabList( void ) ;

void zuluCryptFormatSize( char * buffer,const char * buff ) ;

const char * zuluCryptDecodeMtabEntry( string_t st ) ;

char * zuluCryptVolumeDeviceName( const char * ) ;

static stringList_t _stz ;
static const char * _z ;
static size_t _k ;

static string_t _mapper_filter( uid_t uid )
{
	string_t filter = String( crypt_get_dir() ) ;
	StringAppend( filter,"/zuluCrypt-" ) ;
	_z = StringAppendInt( filter,uid ) ;
	_k = StringLength( filter ) ;
	return filter ;
}

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
	
	if( !zuluCryptSecurityGainElevatedPrivileges() ){
		printf( "%s\tNil\tNil\tNil\tNil\tNil\n",device ) ;
		return ;
	}
	blkid = blkid_new_probe_from_filename( device ) ;
	
	if( blkid == NULL ){
		zuluCryptSecurityDropElevatedPrivileges();
		printf( "%s\tNil\tNil\tNil\tNil\tNil\n",device ) ;
		return ;
	}

	blkid_do_probe( blkid );
	
	blkid_device_size = ( int64_t ) blkid_probe_get_size( blkid ) ;
	
	if( blkid_device_size <= 0 ){
		zuluCryptSecurityDropElevatedPrivileges();
		printf( "%s\tNil\tNil\tNil\tNil\tNil\n",device ) ;
		blkid_free_probe( blkid );
		return ;
	}
	
	printf( "%s\t",device ) ;
	if( m_point == NULL ){
		printf( "Nil" ) ;
	}else{
		printf( "%s",m_point ) ;
	}
	
	if( blkid_probe_lookup_value( blkid,"TYPE",&g,NULL ) == 0 ){
		printf( "\t%s",g ) ;
	}else{
		if( strcmp( device,mapper ) == 0 ){
			printf( "\tNil" ) ;
		}else{
			printf( "\tcrypto_PLAIN" ) ;
		}
	}
	
	blkid_free_probe( blkid );
	
	blkid = blkid_new_probe_from_filename( mapper ) ;
	
	if( blkid == NULL ){
		zuluCryptSecurityDropElevatedPrivileges();
		printf( "Nil\tNil\tNil\n" ) ;
		return ;
	}
	
	blkid_do_probe( blkid );
	
	if( blkid_probe_lookup_value( blkid,"LABEL",&g,NULL ) == 0 ){
		printf( "\t%s",g ) ;
	}else{
		printf( "\tNil" ) ;
	}
	
	blkid_free_probe( blkid );
		
	if( blkid_device_size == -1 ){
		printf( "\tNil" ) ;
	}else{
		g = StringIntToString_1( buffer,SIZE,blkid_device_size ) ;
		zuluCryptFormatSize( format,g ) ;
		printf( "\t%s",format ) ;
	}
	
	if( m_point == NULL ){
		zuluCryptSecurityDropElevatedPrivileges();
		printf( "\tNil\n" ) ;
		return ;
	}
		
	if( statvfs( m_point,&vfs ) != 0 ){
		zuluCryptSecurityDropElevatedPrivileges();
		printf( "\tNil\n" ) ;
		return ;
	}
	
	block_size = vfs.f_frsize ;
	
	total = block_size * vfs.f_blocks  ;
	
	free =  block_size * vfs.f_bavail  ;
		
	used = total - free ;

	snprintf( buff,SIZE,"%.2f%%",100 * ( ( float ) used / ( float ) total ) ) ;
	printf( "\t%s\n",buff ) ;
	zuluCryptSecurityDropElevatedPrivileges();
}

static void _printUnmountedVolumes( const char * device )
{
	zuluMountPartitionProperties( device,device,NULL ) ;
}

static void _printDeviceProperties( string_t entry )
{	
	stringList_t stx ;
	const char * q ;
	char * x ;
	
	const char * e ;
	const char * f ;

	stx = StringListStringSplit( entry,' ' ) ;
		
	if( stx == StringListVoid )
		return ;
	q = StringListContentAt( stx,0 ) ;
	
	if( strncmp( q,_z,_k ) == 0 ){
		/*
		 * zuluCryptVolumeDeviceName() is defined in ../zuluCrypt-cli/lib/status.c
		 * It takes cryptsetup path in "/dev/mapper" and return a device path associated with
		 * the mapper
		 */
		/*
		 * zuluCryptSecurityGainElevatedPrivileges() ;
		 */
		zuluCryptSecurityGainElevatedPrivileges() ;
		x = zuluCryptVolumeDeviceName( q ) ;
		zuluCryptSecurityDropElevatedPrivileges();
		/*
		 * zuluCryptSecurityDropElevatedPrivileges();
		 */
		if( x != NULL ){
			StringListRemoveString( _stz,x ) ;
			/*
			 * zuluCryptDecodeMtabEntry() is defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c
			 * it decodes space,tab,new line and backslash characters since they are written differently in "/etc/mtab" 
			 */
			f = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;
			zuluMountPartitionProperties( x,q,f ) ;	
			free( x ) ;
		}
	}else if( strncmp( q,"/dev/mapper/",12 ) == 0 ){
		/*
		 * 	lvm volumes and its kind goes here,do nothing for now
		 */
			StringListRemoveString( _stz,q ) ;
		
			e = zuluCryptDecodeMtabEntry( StringListStringAt( stx,0 ) ) ;
			f = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;
		
			zuluMountPartitionProperties( e,e,f ) ;
	}else{
		StringListRemoveString( _stz,q ) ;
		
		e = zuluCryptDecodeMtabEntry( StringListStringAt( stx,0 ) ) ;
		f = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;
		
		zuluMountPartitionProperties( e,e,f ) ;
	}
	
	zuluCryptSecurityDropElevatedPrivileges();
	
	StringListDelete( &stx ) ;
}

void zuluMountPrintDeviceProperties_1( string_t entry,uid_t uid )
{
	string_t filter = _mapper_filter( uid ) ;
	_printDeviceProperties( entry ) ;
	StringDelete( &filter ) ;
}

/*
 * This function takes contents of "/etc/mtab" and "/proc/partitions" and compare them.
 * It first print information about partitions with entries in "/etc/mtab" and then
 * the remaining entries net effect being it prints information about partitions that
 * are mounted first and then print information about partitions that are not mounted. 
 */
int zuluMountPrintMountedVolumes( uid_t uid )
{
	string_t filter ;
	
	stringList_t stl ;
	
	/*
	 * zuluCryptGetMtabList() is  defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c
	 * It returns contents of "/etc/mtab"
	 */
	stl = zuluCryptGetMtabList() ;
		
	if( stl == StringListVoid )
		return 1;
	
	/*
	 * zuluCryptPartitionList() is defined in ../zuluCrypt-cli/partitions.c
	 * It returns partition list read from /proc/partitions"
	 */
	_stz = zuluCryptPartitionList() ;
	
	if( _stz == StringListVoid ){
		StringListDelete( &stl ) ;
		return 1;
	}
	
	filter = _mapper_filter( uid ) ;
		
	/*
	 * print all entries that are in "/etc/mtab" ie mounted partitions. 
	 */
	StringListForEach( stl,_printDeviceProperties ) ;
	
	/*
	 * print all entries that are not in "/etc/mtab" ie not mounted partitions. 
	 */
	StringListForEachString( _stz,_printUnmountedVolumes ) ;
	
	StringDelete( &filter ) ;
	StringListMultipleDelete( &stl,&_stz,ENDDELETE ) ;
	
	return 0 ;
}

