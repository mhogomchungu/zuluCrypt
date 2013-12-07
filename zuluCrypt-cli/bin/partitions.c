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
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libintl.h>
#include <locale.h>

#include <blkid/blkid.h>

#include "udev_support.h"

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

static inline int _allowedDevice( const char * device )
{
	const char * fsType ;
	int st ;
	blkid_probe blkid ;
	string_t str ;
	int sts ;
	
	if( StringPrefixMatch( device,"sr",2 ) ){
		/*
		 * device is probably a cdrom or dvdrom,allow them
		 */
		return 1 ;
	}
	
	sts = StringSize( device ) ;
	
	if( sts == 3 ){
		/*
		 * we will get here with a device with an address of "/dev/XYZ".
		 * This device is either not partitioned or is a root address of a partitioned device
		 * Support it only if it has a recognizable file system.
		 */
		str = String( "/dev/" ) ;
		blkid = blkid_new_probe_from_filename( StringAppend( str,device ) ) ;
		StringDelete( &str ) ;
		if( blkid == NULL ){
			return 0 ;
		}else{
			blkid_do_probe( blkid ) ;
			st = blkid_probe_lookup_value( blkid,"TYPE",&fsType,NULL ) ;
			blkid_free_probe( blkid ) ;
			return st == 0 ;
		}
	}else if( sts > 3 ){
		return StringAtLeastOnePrefixMatch( device,"hd","sd","md","mmc",NULL ) ;
	}else{
		return 0 ;
	}
}

static stringList_t _zuluCryptAddLVMVolumes( stringList_t stl )
{
	struct stat lvm ;
	DIR * dir = opendir( "/dev/mapper/" ) ;
	struct dirent * entry ;
	const char * m_path ;
	const char ** e ;
	string_t st = StringVoid ;
	ssize_t index ;
	ssize_t index_1 ;
	
	if( dir != NULL ){
		st = String( "/dev/" ) ;
		e = StringPointer( st ) ;
		while( ( entry = readdir( dir ) ) != NULL ){
			m_path = entry->d_name ;
			if( StringAtLeastOneMatch_1( m_path,".","..","control",NULL ) ){
				;
			}else{
				/*
				* LVM volumes have two paths,one has a format of "/dev/mapper/ABC-DEF" and the 
				* other has a format of "/dev/ABC/DEF". 
				*
				* below code converts the former format to the latter one and assume the volume is LVM
				* if the converted path is found in "/dev"
				* 
				* handle double dashes as explained here: http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=690246
				*/
				StringAppendAt( st,5,m_path ) ;
				
				index = StringLastIndexOfString( st,"--" ) ;
				if( index != -1 ){
					index_1 = StringIndexOfString( st,index+2,"-" ) ;
					if( index_1 != -1 ){
						m_path = StringSubChar( st,index_1,'/' ) ;
						while( StringHasComponent( *e,"--" ) ){
							m_path = StringReplaceString( st,"--","-" ) ;
						}
						if( stat( m_path,&lvm ) == 0 ){
							stl = StringListAppend( stl,m_path ) ;
						}
					}else{
						StringRemoveLength( st,index,2 ) ;
						index_1 = StringLastIndexOfChar( st,'-' ) ;
						m_path = StringInsertChar( st,index,'-' ) ;
						while( StringHasComponent( *e,"--" ) ){
							m_path = StringReplaceString( st,"--","-" ) ;
						}
						if( index_1 != -1 ){
							m_path = StringSubChar( st,index_1,'/' ) ;
							if( stat( m_path,&lvm ) == 0 ){
								stl = StringListAppend( stl,m_path ) ;
							}
						}
					}
				}else{
					index = StringLastIndexOfChar( st,'-' ) ;
					if( index != -1 ){
						m_path = StringSubChar( st,index,'/' ) ;
						if( stat( m_path,&lvm ) == 0 ){
							stl = StringListAppend( stl,m_path ) ;
						}
					}
				}
			}
		}
		StringDelete( &st ) ;
		closedir( dir ) ;
	}
	
	return stl ;
}

static stringList_t _zuluCryptAddMDRAIDVolumes( stringList_t stl )
{	
	DIR * dir = opendir( "/dev/md/" ) ;
	struct dirent * entry ;
	char * e ;
	const char * f ;
	string_t st = StringVoid ;
	
	if( dir != NULL ){
		while( ( entry = readdir( dir ) ) != NULL ){
			f = entry->d_name ;
			if( StringAtLeastOneMatch_1( f,".","..","md-device-map",NULL ) ){
				 ;
			}else{
				st = String( "/dev/md/" ) ;
				e = zuluCryptRealPath( StringAppend( st,f ) ) ;
				if( e != NULL ){
					StringListRemoveString( stl,e ) ;
					free( e ) ;
				}
				stl = StringListAppendString_1( stl,&st ) ;
			}
		}
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
	
	if( st == StringVoid ){
		return StringListVoid ;
	}
	
	stl = StringListStringSplit( st,'\n' ) ;
	
	StringDelete( &st ) ;
	
	if( stl == StringListVoid ){
		return StringListVoid ;
	}
	
	it  = StringListBegin( stl ) + 1 ;
	end = StringListEnd( stl ) ;
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	while( it != end ){
		st = *it ;
		it++ ;
		index = StringLastIndexOfChar( st,' ' ) ;
		if( index != -1 ){
			device = StringContent( st ) + index + 1 ;
			if( _allowedDevice( device ) ){
				st_1 = String( "/dev/" ) ;
				StringAppend( st_1,device ) ;
				stl_1 = StringListAppendString_1( stl_1,&st_1 ) ;
			}
		}
	}
	zuluCryptSecurityDropElevatedPrivileges() ;
	StringListDelete( &stl ) ;
	return _zuluCryptAddLVMVolumes( _zuluCryptAddMDRAIDVolumes( stl_1 ) ) ;
}

static int _zuluCryptCheckSYSifDeviceIsSystem( const char * device )
{
	/*
	 * UDEV_SUPPORT is set at configure time by "-DUDEVSUPPORT=true" option,the option being absent equals "-DUDEVSUPPORT=false"
	 * To set the option, configure with "-DUDEVSUPPORT=true"
	 */
#if UDEVSUPPORT
	/*
	 * udev support is enabled
	 */
	char c ;
	const char * path ;
	int r ;
	string_t xt ;
	string_t st = String( device ) ;
	
	while( 1 ){
		/*
		 * this loop will convert something like: "/dev/sdc12" to "/dev/sdc"
		 * basically,it removes digits from the end of the string
		 */
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
	if( xt == StringVoid ){
		return 0 ;
	}else{
		r = StringEqual( xt,"0\n" ) ;
		StringDelete( &xt ) ;
		return r ;
	}
#else
	if( device ){;}
	/*
	 * udev support is disabled 
	 */
	return 0 ;
#endif
}

stringList_t zuluCryptPartitions( int option,uid_t uid )
{
	ssize_t index ;
	
	const char * device ;
	
	stringList_t non_system = StringListVoid ;
	stringList_t system     = StringListVoid ;
	
	string_t st ;
	
	stringList_t p ;
	stringList_t stl = zuluCryptPartitionList() ;
	
	StringListIterator start ;
	StringListIterator it  ;
	StringListIterator end ;
	
	if( stl == StringListVoid ){
		return StringListVoid ;
	}
	if( option == ZULUCRYPTallPartitions ){
		return stl ;
	}
	
	non_system = stl ;

	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptGetFstabList() is defined in ../lib/parse_fstab.c
	 */
	stl = zuluCryptGetFstabList( uid ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	it  = StringListBegin( stl ) ;
	end = StringListEnd( stl ) ; 
	/*
	 * gather an initial list of system and non system partitions by comparing entries in "/etc/fstab" and "/proc/partitions"
	 * fstab entries makes an initial list of system partitions.
	 * the difference btw list in "/proc/partitions" and "/etc/fstab" makes an initial list of non system partitions.
	 */
	while( it != end ){
		st = *it ;
		it++ ;
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
	 * read entried from "crypttab" and then add them to "system" if absent in that list and remove them from "non system" if present
	 * in that list
	 */
	p = zuluCryptGetPartitionFromCrypttab() ;
	if( p != StringListVoid ){
		it  = StringListBegin( p ) ;
		end = StringListEnd( p ) ;
		while( it != end ){
			device = StringContent( *it ) ;
			it++ ;
			if( StringListContains( system,device ) == -1 ){
				StringListAppend( system,device ) ;
			}
			StringListRemoveIfStringEqual( non_system,device ) ;
		}
		StringListDelete( &p ) ;
	}
	
	/*
	 * read entried from "zuluCrypt-system" and then add them to "system" if absent in that list and remove them from "non system" if present
	 * in that list
	 */
	p = zuluCryptGetPartitionFromConfigFile( "/etc/zuluCrypt-system" ) ;
	if( p != StringListVoid ){
		it  = StringListBegin( p ) ;
		end = StringListEnd( p ) ;
		while( it != end ){
			device = StringContent( *it ) ;
			it++ ;
			if( StringListContains( system,device ) == -1 ){
				StringListAppend( system,device ) ;
			}
			StringListRemoveIfStringEqual( non_system,device ) ;
		}
		StringListDelete( &p ) ;
	}
	
	/*
	 * At this point:
	 * "system" contains system devices gathered from fstab,zuluCrypt-system and crypttab
	 * "non_system" contains non system devices gathered from /proc/partitions minus system partitions. 
	 */
	
	it = StringListBegin( non_system ) ;
	start = it ;
	
	/*
	 * now we consult udev if enabled and we move partition in the "non system" list to "system" list if udev think they are system
	 */
	while( it != StringListEnd( non_system ) ){
		if( _zuluCryptCheckSYSifDeviceIsSystem( StringContent( *it ) ) ){
			StringListAppendString( system,*it ) ;
			StringListRemoveAt( non_system,it - start ) ;
		}else{
			it++ ;
		}
	}
	
	/*
	 * Now we read from a config file that contains devices that are not to be considered system and remove them from
	 * the system list if present in that list and add them to non system list if absent in that list
	 */
	p = zuluCryptGetPartitionFromConfigFile( "/etc/zuluCrypt-nonsystem" ) ;
	if( p != StringListVoid ){
		it  = StringListBegin( p ) ;
		end = StringListEnd( p ) ;
		while( it != end ){
			device = StringContent( *it ) ;
			it++ ;
			StringListRemoveString( system,device ) ;
			if( StringListContains( non_system,device ) == -1 ){
				StringListAppend( non_system,device ) ;
			}
		}
		StringListDelete( &p ) ;
	}

	if( option == ZULUCRYPTsystemPartitions ){
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
		zuluCryptSecurityDropElevatedPrivileges() ;
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
	
	if( blkid_probe_lookup_value( blkid,"LABEL",&e,NULL ) == 0 ){
		printf( "%s\t",e ) ;
	}else{
		printf( "Nil\t" ) ;
	}
	if( blkid_probe_lookup_value( blkid,"TYPE",&e,NULL ) == 0 ){
		printf( "%s\t",e ) ;
	}else{
		printf( "Nil\t" ) ;
	}
	if( blkid_probe_lookup_value( blkid,"UUID",&e,NULL ) == 0 ){
		printf( "%s\n",e ) ;
	}else{
		printf( "Nil\n" ) ;
	}
	blkid_free_probe( blkid );
	zuluCryptSecurityDropElevatedPrivileges();
}

static void _zuluCryptPrintUnMountedPartitionProperties( stringList_t stl )
{
	/*
	 * zuluCryptGetMountInfoList() is defined in ../lib/process_mountinfo.c
	 */
	stringList_t stx = zuluCryptGetMountInfoList() ;
	
	StringListIterator it  = StringListBegin( stl )  ;
	StringListIterator end = StringListEnd( stl ) ;
	
	string_t st ;
	
	while( it != end ){
		st = *it ;
		it++ ;
		if( StringListHasStartSequence( stx,StringAppend( st," " ) ) == -1 ){
			zuluCryptPrintPartitionProperties( StringRemoveRight( st,1 ) ) ;
		}
	}
	
	StringListDelete( &stx ) ;
}

int zuluCryptPrintPartitions( int option,int info,uid_t uid )
{
	stringList_t stl = StringListVoid ;
	
	switch( option ){
		case 1 : stl = zuluCryptPartitions( ZULUCRYPTallPartitions,uid )       ;break ;
		case 2 : stl = zuluCryptPartitions( ZULUCRYPTsystemPartitions,uid )    ;break ;
		case 3 : stl = zuluCryptPartitions( ZULUCRYPTnonSystemPartitions,uid ) ;break ;
	}
	
	if( stl == StringListVoid ){
		printf( gettext( "ERROR: unable to print requested list of partitions\n" ) ) ;
		return 1 ;
	}
	
	switch( info ){
		case 1 : StringListForEachString( stl,zuluCryptPrintPartitionProperties )   ; break ;
		case 2 : _zuluCryptPrintUnMountedPartitionProperties( stl ) 	   	    ; break ;
		default: StringListPrintList( stl ) ;
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
	
	const char * e ;
	
	ssize_t index ;
	ssize_t index_1 ;
	
	StringListIterator it  ;
	StringListIterator end ;
	
	st = StringGetFromFile( "/etc/crypttab" );
	
	if( st == StringVoid ){
		return StringListVoid ;
	}
	
	stl = StringListStringSplit( st,'\n' ) ;
	
	StringDelete( &st ) ;
	
	if( stl == StringListVoid ){
		return StringListVoid ;
	}
	
	it  = StringListBegin( stl ) ;
	end = StringListEnd( stl ) ;
	
	while( it != end ){
		st = *it ;
		it++ ;
		if( StringStartsWith( st,"#" ) ){
			continue ; 
		}
		index = StringIndexOfChar( st,0,'/' ) ;
		if( index == -1 ){
			/*
			 * check above did not find '/' character and we are in this block assuming the line uses UUID 
			 */
			index = StringIndexOfChar( st,0,'U' ) ;
			if( index == -1 ){
				continue ;
			}
			index = StringIndexOfChar( st,index,' ' ) ;
			if( index == -1 ){
				continue ;
			}
			StringSubChar( st,index,'\0' ) ;
			StringRemoveString( st,"\"" ) ;  /* remove quotes if they are used */
			/*
			 * zuluCryptEvaluateDeviceTags() is defined in path_access.c
			 */
			ac = strstr( StringContent( st ),"=" ) ;
			if( ac != NULL ){
				ac = zuluCryptEvaluateDeviceTags( "UUID", ac + 1 ) ;
				stl_1 = StringListAppend( stl_1,ac ) ;
				StringFree( ac ) ;
			}
		}else{
			/*
			 * the entry is of the first format,work to get the device address 
			 */
			index_1 = StringIndexOfChar( st,index,' ' ) ; /*index is set before the conditional statement above */
				
			if( index_1 == -1 ){
				continue ;
			}
			
			e = StringSubChar( st,index_1,'\0' ) + index ;
			
			if( StringPrefixMatch( e,"/dev/disk/by-",13 ) ){
				ac = zuluCryptRealPath( e ) ;
				if( StringPrefixMatch( ac,"/dev/mapper/",12 ) ){
					st = zuluCryptConvertIfPathIsLVM( ac ) ;
					stl_1 = StringListAppendString_1( stl_1,&st ) ;
				}else{
					stl_1 = StringListAppend( stl_1,ac ) ;
				}
				StringFree( ac ) ;
			}else if( StringPrefixMatch( e,"/dev/md",7 ) ){
				/*
				 * zuluCryptResolveMDPath() is defined in ../lib/process_mountinfo.c
				 */
				ac = zuluCryptResolveMDPath( e ) ;
				stl_1 = StringListAppend( stl_1,ac ) ;
				StringFree( ac ) ;
			}else if( StringPrefixMatch( e,"/dev/mapper/",12 ) ){
				/*
				 * zuluCryptConvertIfPathIsLVM() is defined in ../lib/status.c
				 */
				st = zuluCryptConvertIfPathIsLVM( e ) ;
				stl_1 = StringListAppendString_1( stl_1,&st ) ;
			}else{
				stl_1 = StringListAppend( stl_1,e ) ;
			}
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
	const char * e ;
	
	stringList_t stl ;
	stringList_t stl_1 = StringListVoid ;
	
	string_t st = StringVoid ;
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	st = StringGetFromFile( path ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	if( st == StringVoid ){
		return StringListVoid ;
	}
	
	stl = StringListStringSplit( st,'\n' ) ;
	
	StringDelete( &st ) ;
	
	if( stl == StringListVoid ){
		return StringListVoid ;
	}
	
	it  = StringListBegin( stl ) ;
	end = StringListEnd( stl ) ;
	
	while( it != end ){
		st = *it ;
		it++ ;
		if( StringStartsWith( st,"#" ) ){
			continue ;
		}
		if( StringStartsWith( st,"UUID=" ) ){
			StringRemoveString( st,"\"" ) ;
			
			/*
			 * zuluCryptEvaluateDeviceTags() is defined in path_access.c
			 */
			ac = zuluCryptEvaluateDeviceTags( "UUID",StringContent( st ) + 5 ) ;
			stl_1 = StringListAppend( stl_1,ac ) ;
			StringFree( ac ) ;
		}else{
			e = StringContent( st ) ;
			
			if( StringPrefixMatch( e,"/dev/disk/by-",13 ) ){
				ac = zuluCryptRealPath( e ) ;
				if( StringPrefixMatch( ac,"/dev/mapper/",12 ) ){
					st = zuluCryptConvertIfPathIsLVM( ac ) ;
					stl_1 = StringListAppendString_1( stl_1,&st ) ;
				}else{
					stl_1 = StringListAppend( stl_1,ac ) ;
				}
				StringFree( ac ) ;
			}else if( StringPrefixMatch( e,"/dev/md",7 ) ){
				/*
				 * zuluCryptResolveMDPath() is defined in ../lib/process_mountinfo.c
				 */
				ac = zuluCryptResolveMDPath( e ) ;
				stl_1 = StringListAppend( stl_1,ac ) ;
				StringFree( ac ) ;
			}else if( StringPrefixMatch( e,"/dev/mapper/",12 ) ){
				/*
				 * zuluCryptConvertIfPathIsLVM() is defined in ../lib/status.c
				 */
				st = zuluCryptConvertIfPathIsLVM( e ) ;
				stl_1 = StringListAppendString_1( stl_1,&st ) ;
			}else{
				stl_1 = StringListAppend( stl_1,e ) ;
			}
		}
	}
	
	StringListDelete( &stl ) ;
	
	return stl_1 ;
}

int _zuluCryptPartitionIsSystemPartition( const char * dev,uid_t uid )
{	
	stringList_t stl ;
	ssize_t index = -1 ;
	stl = zuluCryptPartitions( ZULUCRYPTsystemPartitions,uid ) ;
	if( stl != StringListVoid ){
		index = StringListContains( stl,dev ) ;
		StringListDelete( &stl ) ;
	}
	return index >= 0 ;
}

int zuluCryptPartitionIsSystemPartition( const char * device,uid_t uid )
{
	char * dev ;
	int st ;
	if( StringPrefixMatch( device,"/dev/loop",9 ) ){
		/*
		 * zuluCryptLoopDeviceAddress_1() is defined in ../lib/create_loop_device.c
		 */
		dev = zuluCryptLoopDeviceAddress_1( device ) ;
		if( dev == NULL ){
			return 0 ;
		}
		if( _zuluCryptPartitionIsSystemPartition( dev,uid ) ){
			st = 1 ;
		}else if( _zuluCryptCheckSYSifDeviceIsSystem( dev ) ){
			st = 1 ;
		}else{
			st = 0 ;
		}
		free( dev ) ;
		return st ;
	}else{
		if( _zuluCryptPartitionIsSystemPartition( device,uid ) ){
			return 1 ;
		}else{
			return _zuluCryptCheckSYSifDeviceIsSystem( device ) ;
		}
	}
	return 0 ;
}
