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
#include "../lib/includes.h"
#include <dirent.h>

#include <blkid/blkid.h>

/*
 * This source file deals with parsing partition list from "/proc/partitions,"/etc/fstab" and "/etc/mtab".
 * 
 * For security reasons,a normal user it is not allowed to create volumes in system partitions.
 * 
 * A System partition is defined as a partition with an active entries in /etc/fstab and/or /etc/crypttab.
 * 
 * This policy is in place to prevent a normal user from attempting to create volumes in internal partitions
 * intentially to destroy other people's data.
 * 
 * External,pluggable usb based partitions are not considered to be system partitions and the tool can be used to create volumes in those.
 * 
 * Internal partitions are considered part of the system and the policy is in place to make sure a normal user does not
 * perform actions reserved for root user.
 * 
 */

/*
 * this function reads a line from a fine, it does what gets() does,it just handles the memory dynamically * 
 */

/*
 * major minor  #blocks  name
 * 
 8        0   78150744 sda *
 8        1   11566768 sda1
 8        2          1 sda2
 8        5   66581361 sda5
 8       16  312571224 sdb
 8       17    1044193 sdb1
 8       18          1 sdb2
 8       21  311524416 sdb5
 8       32     250879 sdc
 8       33     250608 sdc1
 *
 * above output is the output of "cat /proc/partitions" and below function was build again it.
 * The list of partitions is taken from the 4th field and only sdX and hdY entries are taken
 * 
 */

/*
 * this function is defined in ../lib/status.c
 */
void zuluCryptFormatSize( char * buffer,const char * buff ) ;

/*
 * defined in this source file
 */
stringList_t zuluCryptGetPartitionFromCrypttab( void ) ;

stringList_t zuluCryptGetPartitionFromConfigFile( const char * path ) ;

static inline int _allowedDevice( const char * device )
{
	if( strlen( device  ) > 3 ){
		if( strncmp( device,"hd",2 ) == 0 || 
			strncmp( device,"sd",2 ) == 0 ||
			strncmp( device,"mmc",3 ) == 0 ){
			return 1 ;
		}
	}else if( strncmp( device,"sr",2 ) == 0 ){
		return 1 ;
	}
	
	return 0 ;
}

static stringList_t _zuluCryptAddLVMVolumes( stringList_t stl )
{
	const char * path = "/dev/mapper/" ;
	DIR * dir = opendir( path ) ;
	struct dirent * entry ;
	const char * m_path ;
	string_t st = StringVoid ;
	
	if( dir != NULL ){
		st = String( path ) ;
		while( ( entry = readdir( dir ) ) != NULL ){
			m_path = entry->d_name ;
			if( strcmp( m_path,"." ) == 0 ){
				continue ;
			}
			if( strcmp( m_path,".." ) == 0 ){
				continue ;
			}
			if( strcmp( m_path,"control" ) == 0 ){
				continue ;
			}
			if( strstr( m_path,"zuluCrypt" ) != NULL ){
				continue ;
			}
			StringInsertAndDelete( st,12,m_path ) ;
			stl = StringListAppendString( stl,st ) ;
		}
		StringDelete( &st ) ;
		closedir( dir ) ;
	}
	
	return stl ;
}

stringList_t zuluCryptPartitionList( void )
{
	const char * device ;
	
	ssize_t index ;
	
	StringListIterator it ;
	StringListIterator end ;
	
	stringList_t stl   = StringListVoid ;
	stringList_t stl_1 = StringListVoid ;
	
	string_t st = StringGetFromVirtualFile( "/proc/partitions" ) ;
	string_t st_1 ;
	
	if( st == StringVoid )
		return StringListVoid ;
	
	stl = StringListStringSplit( st,'\n' ) ;
	
	StringDelete( &st ) ;
	
	if( stl == StringListVoid )
		return StringListVoid ;
	
	st_1 = String( "/dev/" ) ;
	
	it  = StringListBegin( stl ) ;
	end = StringListEnd( stl ) ;
	
	for( ; it != end ; it++ ){
		st = *it ;
		index = StringLastIndexOfChar( st,' ' ) ;
		if( index != -1 ){
			device = StringContent( st ) + index + 1 ;
			if( _allowedDevice( device ) ){
				StringInsertAndDelete( st_1,5,device ) ;
				stl_1 = StringListAppendString( stl_1,st_1 ) ;
			}
		}
	}
	
	StringDelete( &st_1 ) ;
	StringListDelete( &stl ) ;
	return _zuluCryptAddLVMVolumes( stl_1 ) ;
}

static int _zuluCryptCheckSYSifDeviceIsSystem( const char * device )
{
	char c ;
	const char * path ;
	int r ;	
	string_t st ;
	string_t xt ;
	
	/*
	 * turn off udev for now,it does not seem to detect external usb based devices reliably. 
	 */
	return 0 ;
	
	st = String( device ) ;
	/*
	 * this loop will convert something like: "/dev/sdc12" to "/dev/sdc"
	 * basically,it removes digits from the end of the string
	 */
	while( 1 ){
		c = StringCharAtLast( st ) ;
		if( c >= '0' && c <= '9' ){
			StringRemoveRight( st,1 ) ;
		}else{
			break ;
		}
	}
	
	StringReplaceString( st,"/dev/","/sys/block/" ) ;
	path = StringAppend( st,"/removable" ) ;
	/*
	 * path will be something like "/sys/block/sda/removable"
	 */
	xt = StringGetFromVirtualFile( path ) ;
	StringDelete( &st ) ;
	if( xt == StringVoid )
		return 0 ;
	r = StringEqual( xt,"0\n" ) ;
	StringDelete( &xt ) ;
	return r ;	
}

stringList_t zuluCryptPartitions( int option )
{
	ssize_t index ;
	
	const char * device ;
	
	stringList_t non_system = StringListVoid ;
	stringList_t system     = StringListVoid ;
	
	string_t st ;
	
	stringList_t p ;
	stringList_t stl = zuluCryptPartitionList() ;
	
	StringListIterator it  ;
	StringListIterator end ;
	
	if( stl == StringListVoid )
		return StringListVoid ;
	
	if( option == ALL_PARTITIONS )
		return stl ;
	
	non_system = stl ;

	/*
	 * zuluCryptGetFstabList() is defined in ../lib/mount_volume.c
	 */
	stl = zuluCryptGetFstabList() ;
		
	if( stl == StringListVoid ){
		StringListDelete( &non_system ) ;
		return StringListVoid ;
	}
	
	it  = StringListBegin( stl ) ;
	end = StringListEnd( stl ) ; 
	
	for(  ; it != end ; it++ ){
		st = *it ;
		if( StringStartsWith( st,"/" ) ){
			index = StringIndexOfChar( st,0,' ' ) ;
			if( index != -1 ){
				device = StringSubChar( st,index,'\0' ) ;
				system = StringListAppend( system,device ) ;
				StringListRemoveString( non_system,device ) ;
			}
		}
	}

	StringListDelete( &stl ) ;
	
	/*
	 * Read from confi files to get additional devices to be considered as system devices
	 */
	p = zuluCryptGetPartitionFromCrypttab() ;
	StringListAppendList( system,p ) ;
	StringListDelete( &p ) ;
	p = zuluCryptGetPartitionFromConfigFile( "/etc/zuluCrypt-system" ) ;
	StringListAppendList( system,p ) ;
	StringListDelete( &p ) ;
	
	/*
	 * At this point:
	 * "system" contains system devices.
	 * "non_system" contains non system devices.
	 * 
	 * now we check non_system devices agains entries in /sys/ to see if udev reported them as system and move them to system 
	 * if it does . 
	 */
	it  = StringListBegin( non_system ) ;
	end = StringListEnd( non_system ) ;
	
	do{
		device = StringContent( *it ) ;
		if( _zuluCryptCheckSYSifDeviceIsSystem( device ) ){
			StringListAppend( system,device ) ;
			StringListRemoveString( non_system,device ) ;
			it  = StringListBegin( non_system ) ;
			end = StringListEnd( non_system ) ;
		}else{
			it++ ;
		}
	}while( it != end ) ;
	
	/*
	 * Now we read from a config file that contains devices that are not to be considered system and remove them from
	 * the system list if present in that list
	 */
	p = zuluCryptGetPartitionFromConfigFile( "/etc/zuluCrypt-nonsystem" ) ;
	if( p != StringListVoid ){
		it  = StringListBegin( p ) ;
		end = StringListEnd( p ) ;
		for( ; it != end ; it++ ){
			device = StringContent( *it ) ;
			StringListRemoveString( system,device ) ;
		}
		StringListDelete( &p ) ;
	}
	
	if( option == SYSTEM_PARTITIONS ){
		StringListDelete( &non_system ) ;
		return system  ;
	}else{
		StringListDelete( &system ) ;
		return non_system  ;
	}
}

void zuluCryptPrintPartitionProperties( const char * device )
{	
	#define SIZE 64
	char sizebuffer[ SIZE ] ;
	char sizebuffer_1[ SIZE ] ;
	
	const char * e ;
	
	uint64_t size ;
	
	blkid_probe blkid ;
	zuluCryptSecurityGainElevatedPrivileges() ;
	blkid = blkid_new_probe_from_filename( device ) ;
	
	printf( "%s\t",device ) ;
	
	if( blkid == NULL ){
		zuluCryptSecurityDropElevatedPrivileges();
		printf( "Nil\tNil\tNil\tNil\n" ) ;
		return ;
	} 
	
	blkid_do_probe( blkid );
	
	size = blkid_probe_get_size( blkid ) ;
	
	e = StringIntToString_1( sizebuffer,SIZE,size ) ;
	/*
	 * below function is defined in ../lib/status.c
	 */
	zuluCryptFormatSize( sizebuffer_1,e ) ;
	
	printf( "%s\t",sizebuffer_1 ) ;
	
	if( blkid_probe_lookup_value( blkid,"LABEL",&e,NULL ) == 0 )
		printf( "%s\t",e ) ;
	else
		printf( "Nil\t" ) ;
	
	if( blkid_probe_lookup_value( blkid,"TYPE",&e,NULL ) == 0 )
		printf( "%s\t",e ) ;
	else
		printf( "Nil\t" ) ;
	
	if( blkid_probe_lookup_value( blkid,"UUID",&e,NULL ) == 0 )
		printf( "%s\n",e ) ;
	else
		printf( "Nil\n" ) ;
	
	blkid_free_probe( blkid );
	zuluCryptSecurityDropElevatedPrivileges();
}

int zuluCryptPrintPartitions( int option,int info )
{
	stringList_t stl = StringListVoid ;
	
	switch( option ){
		case 1 : stl = zuluCryptPartitions( ALL_PARTITIONS ) 	   ;break ;
		case 2 : stl = zuluCryptPartitions( SYSTEM_PARTITIONS )    ;break ;
		case 3 : stl = zuluCryptPartitions( NON_SYSTEM_PARTITIONS );break ;
	}
	
	if( stl == StringListVoid ){
		printf( "ERROR: unable to print requested list of partitions\n" ) ;
		return 1 ;
	}
	
	if( info ){
		StringListForEachString( stl,zuluCryptPrintPartitionProperties ) ;
	}else{
		StringListPrintList( stl ) ;
	}
	
	StringListDelete( &stl ) ;
	
	return 0 ;
}

/*
 * this function will parse /etc/crypttab to see if it has any entries to be used as system partition.
 * 
 * sample example of the file content this function was build on.
 * 
 
 * secret /dev/sda15 none
 * secret_1 UUID=d2d210b8-0b1f-419f-9172-9d509ea9af0c none 
 * 
 */
stringList_t zuluCryptGetPartitionFromCrypttab( void )
{
	stringList_t stl   = StringListVoid ;
	stringList_t stl_1 = StringListVoid ;
	string_t st  ;

	char * ac ;
	
	ssize_t index ;
	ssize_t index_1 ;
	
	StringListIterator it  ;
	StringListIterator end ;
	
	st = StringGetFromFile( "/etc/crypttab" );
	
	if( st == StringVoid )
		return StringListVoid ;
	
	stl = StringListStringSplit( st,'\n' ) ;
	
	StringDelete( &st ) ;
	
	if( stl == StringListVoid )
		return StringListVoid ;
	
	it  = StringListBegin( stl ) ;
	end = StringListEnd( stl ) ;
	
	for( ; it != end ; it++ ){
		st = *it ;
		if( StringStartsWith( st,"#" ) )
			continue ; 
		index = StringIndexOfChar( st,0,'/' ) ;
		if( index == -1 ){
			/*
			 * check above did not find '/' character and we are in this block assuming the line uses UUID
			 * 
			 */
			index = StringIndexOfChar( st,0,'U' ) ;
			if( index == -1 )
				continue ;
			index = StringIndexOfChar( st,index,' ' ) ;
			if( index == -1 )
				continue ;
			StringSubChar( st,index,'\0' ) ;
			StringRemoveString( st,"\"" ) ;  /* remove quotes if they are used */
			/*
			 * zuluCryptSecurityEvaluateDeviceTags() is defined in ./security.c
			 */
			ac = strstr( StringContent( st ),"=" ) ;
			if( ac != NULL ){
				ac = zuluCryptSecurityEvaluateDeviceTags( "UUID", ac + 1 );    
				if( ac != NULL ){	
					stl_1 = StringListAppend( stl_1,ac ) ;
					free( ac ) ;
				}
			}
		}else{
			/*
			 * the entry is of the first format,work to get the device address 
			 */
			index_1 = StringIndexOfChar( st,index,' ' ) ; /*index is set before the conditional statement above */
				
			if ( index_1 == -1 )
				continue ;
			StringSubChar( st,index_1,'\0' ) ; 
			stl_1 = StringListAppend( stl_1,StringContent( st ) + index ) ;
		}
	}
	
	StringListDelete( &stl ) ;
	return stl_1 ;
}

stringList_t zuluCryptGetPartitionFromConfigFile( const char * path )
{
	StringListIterator it  ;
	StringListIterator end ;
	
	char * ac ;
	
	stringList_t stl ;
	stringList_t stl_1 = StringListVoid ;
	
	string_t st = StringVoid ;
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	st = StringGetFromFile( path ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	if( st == StringVoid )
		return StringListVoid ;

	stl = StringListStringSplit( st,'\n' ) ;
	
	StringDelete( &st ) ;
	
	if( stl == StringListVoid )
		return StringListVoid ;
	
	it  = StringListBegin( stl ) ;
	end = StringListEnd( stl ) ;
	
	for( ; it != end ; it++ ){
		st = *it ;
		StringRemoveString( st,"\"" ) ;
		if( StringStartsWith( st,"UUID=" ) ){
			/*
			 * zuluCryptSecurityEvaluateDeviceTags() is defined in ./security.c
			 */
			ac = zuluCryptSecurityEvaluateDeviceTags( "UUID",StringContent( st ) + 5 ) ;
			if( ac != NULL ){
				stl_1 = StringListAppend( stl_1,ac ) ;
				free( ac ) ;
			}
		}else{
			stl_1 = StringListAppendString( stl_1,st ) ;
		}
	}
	
	StringListDelete( &stl ) ;
	
	return stl_1 ;
}

int _zuluCryptPartitionIsSystemPartition( const char * dev )
{	
	stringList_t stl ;
	ssize_t index = -1 ;
	stl = zuluCryptPartitions( SYSTEM_PARTITIONS ) ;
	if( stl != StringListVoid ){
		index = StringListContains( stl,dev );
		StringListDelete( &stl ) ;
	}
	return index >= 0 ;
}

int zuluCryptPartitionIsSystemPartition( const char * device )
{
	char * dev ;
	if( strncmp( device,"/dev/loop",9 ) == 0 ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ../lib/create_loop_device.c
		 */
		dev = zuluCryptLoopDeviceAddress( device ) ;
		if( dev == NULL )
			return 0 ;
		if( _zuluCryptPartitionIsSystemPartition( dev ) ){
			free( dev ) ;
			return 1 ;
		}else if( _zuluCryptCheckSYSifDeviceIsSystem( dev ) ){
			free( dev ) ;
			return 1 ;
		}else{
			free( dev ) ;
		}
	}else{
		if( _zuluCryptPartitionIsSystemPartition( device ) ){
			return 1 ;
		}else{
			return _zuluCryptCheckSYSifDeviceIsSystem( device ) ;
		}
	}
	return 0 ;
}
