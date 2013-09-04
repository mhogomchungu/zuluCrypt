 
/*
 * 
 *  Copyright (c) 2012
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
#include <libcryptsetup.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/mman.h>
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZE 512

static int zuluExit( int st )
{
	switch( st ){
		case 0 : printf( gettext( "SUCCESS: header saved successfully\n" ) )						; break ;
		case 1 : printf( gettext( "SUCCESS: header restored successfully\n" ) )						; break ;
		case 2 : printf( gettext( "ERROR: presented device is not a LUKS device\n" ) ) 					; break ;
		case 3 : printf( gettext( "ERROR: failed to perform requested operation\n" ) )					; break ;
		case 4 : printf( gettext( "ERROR: failed to perform requested operation\n" ) )					; break ;
		case 5 : printf( gettext( "INFO: operation terminater per user request\n" ) )					; break ;
		case 6 : printf( gettext( "ERROR: path to be used to create a back up file is occupied or permission denied\n" ) ); break ;
		case 7 : printf( gettext( "ERROR: failed to perform requested operation\n" ) )					; break ;
		case 8 : printf( gettext( "ERROR: insufficient privilege to open backup header file for reading\n" ) )		; break ;
		case 9 : printf( gettext( "ERROR: invalid path to back up header file\n" ) )					; break ;
		case 10: printf( gettext( "ERROR: insufficient privilege to create a backup header in a destination folder\n" )); break ;
		case 11: printf( gettext( "ERROR: invalid path to device\n" ) )							; break ;
		case 12: printf( gettext( "ERROR: argument for path to a backup  header file is missing\n" ) )			; break ;
		case 13: printf( gettext( "ERROR: argument for path to a backup  header file is missing\n" ) )			; break ;
		case 14: printf( gettext( "ERROR: only root and \"zulucrypt\" group members can restore and back up luks headers on system devices\n" )	); break ;
		case 15: printf( gettext( "ERROR: insufficient privilege to open device for writing\n" ) )			; break ;
		case 16: printf( gettext( "ERROR: could not resolve path to device\n" ) )					; break ;
		case 17: printf( gettext( "ERROR: backup file does not appear to contain luks header\n" ) )			; break ;
		case 18: printf( gettext( "ERROR: insufficient privilege to open device for reading\n" ) )			; break ;
		case 19: printf( gettext( "ERROR: insufficient memory to hold your responce\n" ) )	 			; break ;
	}
	
	if( st == 1 ){
		return 0 ;
	}else{
		return st ;
	}
}

/*
 * We do not want to pass user managed paths to cryptsetup so:
 *
 * 1. When creating a luks header backup,we pass to cryptsetup a path to /dev/shm/zuluCrypt to create a header there
 *    and then copy it to user provided path.
 * 2. When restoring a luks header,we copy the backup file to /dev/shm/zuluCrypt and then we pass the copied path to
 *    zuluCrypt.
 *
 *   A normal user will have no access to /dev/shm/zuluCrypt and will allow secure handling of user provided paths in 
 *   backing up and restoring luks headers. 
 */

/*
 * Below function creates a secured folder path,ie a folder path a normal user has no access to
 */
static string_t create_work_directory( void )
{
	/*
	 * ZULUCRYPTtempFolder and ZULUCRYPtmountMiniPath are set in ../constants.h
	 */
	const char * temp_path = "/run/zuluCrypt/" ;
	struct stat xt ;
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	#define PATH_DOES_NOT_EXIST( x ) stat( x,&xt ) != 0
	
	if( PATH_DOES_NOT_EXIST( "/run" ) ){
		mkdir( "/run",S_IRWXU ) ;
		chown( "/run",0,0 ) ;
	}
	if( PATH_DOES_NOT_EXIST( temp_path ) ){
		mkdir( temp_path,S_IRWXU ) ;
		chown( temp_path,0,0 ) ;
	}
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	return String( temp_path ) ;
}

/*
 * Below function copies a file owned and managed by a user to a secured location so that can be accessed securely.
 * path returns a path to where a copy of the file is located and ready to be accessed securely
 * It is the responsibility of the called to zuluCryptDeleteFile(path) when done with the copy and free(path) memory when done with it
 */
static int secure_file_path( const char ** path,const char * source )
{
	int fd_source ;
	int fd_temp ;
	char buffer[ SIZE ] ;
	size_t len ;
	const char * temp_path ;
	
	string_t st_path = create_work_directory() ;
	
	StringAppend( st_path,"0-" ) ;
	temp_path = StringAppendInt( st_path,syscall( SYS_gettid ) ) ;
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	fd_source = open( source,O_RDONLY ) ;
	
	if( fd_source == -1 ){
		StringDelete( &st_path ) ;
		return 0 ;
	}
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	fd_temp = open( temp_path,O_WRONLY | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP |S_IROTH ) ;
	if( fd_temp == -1 ){
		close( fd_source ) ;
		StringDelete( &st_path ) ;
		return 0 ;
	}
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	while( 1 ){
		len = read( fd_source,buffer,SIZE ) ;
		if( len < SIZE ){
			write( fd_temp,buffer,len ) ;
			break ;
		}else{
			write( fd_temp,buffer,len ) ;
		}
	}
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	close( fd_source ) ;
	close( fd_temp ) ;
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	*path = StringDeleteHandle( &st_path ) ;
	return 1 ;
}

/*
 * this function return a secured file path to be used to create a file at the path
 */
static inline const char * secure_file_path_1( void )
{
	string_t st_path = create_work_directory() ;
	StringAppend( st_path,"1-" ) ;
	StringAppendInt( st_path,syscall( SYS_gettid ) ) ;
	return StringDeleteHandle( &st_path ) ;
}

/*
 * Below function copies a secured file from secured location to a user owned and managed location.
 * The source file will be deleted when the copy is done.
 */
static inline int secure_copy_file( const char * source,const char * dest,uid_t uid )
{
	int st = 4 ;
	int fd_source ;
	int fd_dest ;
	size_t len ;
	char buffer[ SIZE ] ;
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	fd_dest = open( dest,O_WRONLY | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP |S_IROTH ) ;
	if( fd_dest == -1 ){
		zuluCryptSecurityGainElevatedPrivileges() ;
		zuluCryptDeleteFile( source ) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
		return 6 ;
	}
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	fd_source = open( source,O_RDONLY ) ;
	
	if( fd_source != -1 ){
		while( 1 ){
			len = read( fd_source,buffer,SIZE ) ;
			if( len < SIZE ){
				write( fd_dest,buffer,len ) ;
				break ;
			}else{
				write( fd_dest,buffer,len ) ;
			}
		}
		chmod( dest,S_IRUSR ) ;
		chown( dest,uid,uid ) ;
		st = 0 ;
	}
	
	/*
	 * zuluCryptDeleteFile() is defined in ../lib/file_path_security.c
	 */
	zuluCryptDeleteFile( source ) ;
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	return st ;
}

static int save_header( const char * device,const char * path,uid_t uid )
{
	int st = 4 ;
	struct crypt_device * cd ;
	
	const char * temp_path = secure_file_path_1() ;

	if( zuluCryptSecurityGainElevatedPrivileges() ){
		if( crypt_init( &cd,device ) != 0 ){
			st = 3 ;
		}else{
			if( crypt_header_backup( cd,NULL,temp_path ) == 0 ){
				st = secure_copy_file( temp_path,path,uid ) ;
			}else{
				st = 4 ;
			}
			zuluCryptSecurityGainElevatedPrivileges() ;
			crypt_free( cd ) ;
		}
		zuluCryptSecurityDropElevatedPrivileges() ;
	}
	
	StringFree( temp_path ) ;
	return st ;
}

static int restore_header( const char * device,const char * path,int k,uid_t uid )
{
	struct crypt_device * cd  ;
	const char * temp_path ;
	
	int st = 7;
	string_t confirm ;
	const char * warn = gettext( "\
Are you sure you want to replace a header on device \"%s\" with a backup copy at \"%s\"?\n\
Type \"YES\" and press Enter to continue: " ) ;
	if( uid ){;}
	if( k == -1 ){
		zuluCryptSecurityDropElevatedPrivileges() ;
		printf( warn,device,path ) ;
				
		confirm = StringGetFromTerminal_1( 3 ) ;
		if( confirm != StringVoid ){
			k = StringEqual( confirm,gettext( "YES" ) );
			StringDelete( &confirm ) ;
			if( k == 0 ){
				return 5 ;
			}
		}else{
			return 19 ;
		}
	}
	
	if( !secure_file_path( &temp_path,path ) ){
		return 7 ;
	}
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	if( crypt_init( &cd,device ) != 0 ){
		st = 7 ;
	}else{
		if( crypt_load( cd,NULL,NULL ) != 0 ){
			st = 2 ;
		}else{
			if( crypt_header_restore( cd,NULL,temp_path ) == 0 ){
				st = 1 ;
			}else{
				st = 7 ;
			}
		}
	}
	zuluCryptDeleteFile( temp_path ) ;
	StringFree( temp_path ) ;
	crypt_free( cd ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	return st ;
}

int zuluCryptEXESaveAndRestoreLuksHeader( const struct_opts * opts,uid_t uid,int option  )
{
	const char * device = opts->device ;
	
	/*
	 * using key_key here because i do not want to introduce a key field in the structure.
	 */
	const char * path = opts->key ;
	int st ;
	int k ;
	
	const char * dev ;
	
	if( StringPrefixMatch( device,"/dev/loop",9 ) ){
		/*
		 * zuluCryptLoopDeviceAddress_1() is defined in ../lib/create_loop_device.c
		 */
		dev = zuluCryptLoopDeviceAddress_1( device ) ;
		/*
		 * zuluCryptPartitionIsSystemPartition() is defined in partitions.c
		 */
		k = zuluCryptPartitionIsSystemPartition( dev,uid ) ;
		StringFree( dev ) ;
	}else{
		/*
		 * zuluCryptPartitionIsSystemPartition() is defined in partitions.c
		 */
		k = zuluCryptPartitionIsSystemPartition( device,uid ) ;
	}
	
	if( k == 1 ){
		if( uid != 0 || !zuluCryptUserIsAMemberOfAGroup( uid,"zulucrypt" ) ){
			return zuluExit( 14 ) ;
		}
	}
		
	if( path == NULL ){
		if( option == LUKS_HEADER_RESTORE ){
			return zuluExit( 12 ) ;
		}else{
			return zuluExit( 13 ) ;
		}
	}
	
	if( option == LUKS_HEADER_RESTORE ){
		st = restore_header( device,path,opts->dont_ask_confirmation,uid ) ;
	}else{
		st = save_header( device,path,uid ) ;
	}
	
	return zuluExit( st ) ;
}

static int files_are_equal( const char * file1,const char * file2 )
{
	struct stat st1 ;
	struct stat st2 ;
	
	int fd1;
	int fd2;
	int r = 1 ;
	
	void * map1 ;
	void * map2 ;
	
	fd1 = open( file1,O_RDONLY ) ;
	if( fd1 == -1 ){
		return 0 ;
	}
	
	fd2 = open( file2,O_RDONLY ) ;
	if( fd2 == -1 ){
		close( fd1 ) ;
		return 0 ;
	}
	
	fstat( fd1,&st1 ) ;
	fstat( fd2,&st2 ) ;
		
	map1 =  mmap( 0,st1.st_size,PROT_READ,MAP_PRIVATE,fd1,0 ) ;
	if( map1 != MAP_FAILED ){
		map2 =  mmap( 0,st2.st_size,PROT_READ,MAP_PRIVATE,fd2,0 ) ;
		if( map2 != MAP_FAILED ){
			if( st1.st_size > st2.st_size ){
				r = memcmp( map1,map2,st1.st_size ) ;
			}else{
				r = memcmp( map1,map2,st2.st_size ) ;
			}
			munmap( map2,st2.st_size ) ;
		}
		munmap( map1,st1.st_size ) ;
	}
	
	close( fd1 ) ;
	close( fd2 ) ;
	
	return r == 0 ;
}

static int _save_tmp_header( const char * device,const char * backup )
{
	struct crypt_device * cd  ;
	int st = 1 ;
	if( crypt_init( &cd,device ) == 0 ){
		st = crypt_header_backup( cd,NULL,backup ) ;
		crypt_free( cd ) ;
	}
	return st == 0 ;
}

int zuluCryptHeaderMatchBackUpHeader( const char * device,const char * header_backup,uid_t uid )
{
	const char * header_path = NULL;
	const char * device_header = NULL;
	
	int st = 0 ;
	
	if( uid ){;}
	if( device == NULL || header_backup == NULL ){
		return 0 ;
	}
	secure_file_path( &header_path,header_backup ) ;
	if( header_path == NULL ){
		return 0 ;
	}
	
	device_header = secure_file_path_1() ;
	if( device_header == NULL ){
		StringFree( header_path ) ;
		return 0 ;
	}
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	if( _save_tmp_header( device,device_header ) ){
		st = files_are_equal( header_path,device_header ) ;
	}
	
	zuluCryptDeleteFile( header_path ) ;
	zuluCryptDeleteFile( device_header ) ;
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	StringFree( header_path ) ;
	StringFree( device_header ) ;
	
	return st ;
}
