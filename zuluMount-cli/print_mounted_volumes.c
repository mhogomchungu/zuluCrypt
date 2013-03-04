 
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
#include "../zuluCrypt-cli/lib/includes.h"

/*
 * below header file is generated at configure time
 */
#include "libmount_header.h"

#include "../zuluCrypt-cli/bin/includes.h"

stringList_t zuluCryptPartitionList( void ) ;

void zuluCryptFormatSize( char * buffer,const char * buff ) ;

typedef struct{
	stringList_t stz ;
	const char * z ;
	size_t k ;
}ARGS;

void zuluMountPartitionProperties( const char * device,const char * UUID,const char * mapper,const char * m_point )
{
	#define SIZE 64
	
	const char * g ;
	
	blkid_probe blkid ;
	
	struct statvfs vfs ;
	uint64_t total ;
	uint64_t used ;
	uint64_t free_space ;
	uint32_t block_size ;
	int64_t blkid_device_size ;
	
	char buff[ SIZE ] ;
	char * buffer = buff ;
	char format[ SIZE ] ;
	char * loop_device ;
	
	char * volType ;
		
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
	
	if( UUID != NULL ){
		printf( "%s\t",UUID ) ;
	}else{
		if( StringPrefixEqual( device,"/dev/loop" ) ){
			/*
			* zuluCryptLoopDeviceAddress() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
			*/
			loop_device = zuluCryptLoopDeviceAddress( device ) ;
			if( loop_device != NULL ){
				printf( "%s\t",loop_device ) ;
				free( loop_device ) ;
			}else{
				printf( "%s\t",device ) ;
			}
		}else{
			printf( "%s\t",device ) ;
		}
	}
	
	if( m_point == NULL ){
		printf( "Nil\t" ) ;
	}else{
		printf( "%s\t",m_point ) ;
	}
	
	if( StringsAreNotEqual( device,mapper ) ){
		/*
		 * We will get here when a volume is encrypted.
		 * device will contain something like /dev/sdc1
		 * mapper will contain something like /dev/mapper/XYZ
		 */
		/*
		 * zuluCryptGetVolumeTypeFromMapperPath() is defined in ../zuluCrypt-cli/lib/status.c
		 */
		volType = zuluCryptGetVolumeTypeFromMapperPath( mapper ) ;
		printf( "%s",volType ) ;
		free( volType ) ;
	}else{
		if( blkid_probe_lookup_value( blkid,"TYPE",&g,NULL ) == 0 ){
			printf( "%s",g ) ;
		}else{
			/*
			 * couldnt read a file system,assuming the partition has no file system 
			 */
			printf( "Nil" ) ;
		}
	}
	
	blkid_free_probe( blkid );
	
	blkid = blkid_new_probe_from_filename( mapper ) ;
	
	if( blkid == NULL ){
		zuluCryptSecurityDropElevatedPrivileges();
		printf( "\tNil\tNil\tNil\n" ) ;
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
	
	free_space = block_size * vfs.f_bavail  ;
		
	used = total - free_space ;

	snprintf( buff,SIZE,"%.2f%%",100 * ( ( float ) used / ( float ) total ) ) ;
	printf( "\t%s\n",buff ) ;
	zuluCryptSecurityDropElevatedPrivileges();
}

static void _printUnmountedVolumes( const char * device )
{
	zuluMountPartitionProperties( device,NULL,device,NULL ) ;
}

static void _printDeviceProperties( string_t entry,void * arg )
{	
	stringList_t stx ;
	const char * q ;
	char * x ;
	
	ARGS * args = ( ARGS * ) arg ;
	size_t  _k = args->k ;
	const char * _z = args->z ;
	stringList_t _stz = args->stz ;
	
	const char * e ;
	const char * f ;
	
	ssize_t index ;
	
	string_t st = StringVoid ;
	string_t n  = StringVoid ;
	
	stx = StringListStringSplit( entry,' ' ) ;
		
	if( stx == StringListVoid ){
		return ;
	}
	
	q = StringListContentAt( stx,0 ) ;
	
	if( StringPrefixMatch( q,_z,_k ) ){
		/*
		 * we will get in here only with encrypted volumes,
		 * q will contain something like /dev/mapper/zuluCrypt-500-NAAN-plain-1688
		 * _z will contain something like /dev/mapper/zuluCrypt-500
		 * _k is the length of _z
		 */
		
		/*
		 * zuluCryptSecurityGainElevatedPrivileges() and zuluCryptSecurityDropElevatedPrivileges()
		 * are defined in ../zuluCrypt-cli/bin/security.c 
		 */
		zuluCryptSecurityGainElevatedPrivileges() ;
		/*
		 * zuluCryptVolumeDeviceName() is defined in ../zuluCrypt-cli/lib/status.c
		 */
		x = zuluCryptVolumeDeviceName( q ) ;
		zuluCryptSecurityDropElevatedPrivileges();
		
		if( x != NULL ){
			if( StringPrefixEqual( q + _k,"-UUID-" ) ){
				st = String( q ) ;
				StringRemoveLeft( st,_k + 6 ) ;
				e = StringPrepend( st,"UUID=\"" ) ;
				index = StringLastIndexOfChar( st,'-' ) ;
				if( index >= 0 ){
					StringSubChar( st,index,'\"' ) ;
					e = StringSubChar( st,index+1,'\0' ) ;
				}
			}else{
				e = x ;
			}
				
			StringListRemoveString( _stz,x ) ;
			/*
			 * zuluCryptDecodeMtabEntry() is defined in ../zuluCrypt-cli/lib/mount_volume.c.c
			 * it decodes space,tab,new line and backslash characters since they are written differently in "/etc/mtab" 
			 */
			f = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;
			zuluMountPartitionProperties( x,e,q,f ) ;
			free( x ) ;
			StringDelete( &st ) ;
		}
	}else{
		StringListRemoveString( _stz,q ) ;
		
		e = zuluCryptDecodeMtabEntry( StringListStringAt( stx,0 ) ) ;
		f = zuluCryptDecodeMtabEntry( StringListStringAt( stx,1 ) ) ;
		
		zuluMountPartitionProperties( e,NULL,e,f ) ;
	}
	
	zuluCryptSecurityDropElevatedPrivileges();
	StringDelete( &n ) ;
	StringListDelete( &stx ) ;
}

void zuluMountPrintDeviceProperties_1( string_t entry,uid_t uid )
{
	ARGS args ;
	string_t filter = String( crypt_get_dir() ) ;
	StringAppend( filter,"/zuluCrypt-" ) ;
	args.z = StringAppendInt( filter,uid ) ;
	args.k = StringLength( filter ) ;
	args.stz = StringListVoid ;
	_printDeviceProperties( entry,( void * )&args ) ;
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
	
	ARGS args ;
	
	stringList_t stl ;
	
	/*
	 * zuluCryptGetMountInfoList() is  defined in ../zuluCrypt-cli/lib/process_mountinfo.c
	 * It returns contents of "/etc/mtab"
	 */
	stl = zuluCryptGetMountInfoList() ;
		
	if( stl == StringListVoid ){
		return 1;
	}
	/*
	 * zuluCryptPartitionList() is defined in ../zuluCrypt-cli/partitions.c
	 * It returns partition list read from /proc/partitions"
	 */
	args.stz = zuluCryptPartitionList() ;
	
	if( args.stz == StringListVoid ){
		StringListDelete( &stl ) ;
		return 1;
	}
	
	filter = String( crypt_get_dir() ) ;
	StringAppend( filter,"/zuluCrypt-" ) ;
	
	args.z = StringAppendInt( filter,uid ) ;
	args.k = StringLength( filter ) ;
		
	/*
	 * print all entries that are in "/etc/mtab" ie mounted partitions. 
	 */
	StringListForEach_1( stl,_printDeviceProperties,( void * )&args ) ;
	
	/*
	 * print all entries that are not in "/etc/mtab" ie not mounted partitions. 
	 */
	StringListForEachString( args.stz,_printUnmountedVolumes ) ;
	
	StringDelete( &filter ) ;
	StringListMultipleDelete( &stl,&args.stz,ENDDELETE ) ;
	
	return 0 ;
}
