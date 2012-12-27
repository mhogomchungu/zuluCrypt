 
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

#define SIZE 512

static int zuluExit( int st,int fd,string_t xt )
{
	switch( st ){
		case 0 : printf( "SUCCESS: header saved successfully\n" ) 						; break ;
		case 1 : printf( "SUCCESS: header restored successfully\n" ) 						; break ;
		case 2 : printf( "ERROR: presented device is not a LUKS device\n" ) 					; break ;
		case 3 : printf( "ERROR: failed to read/write header,is the volume open?\n" ) 				; break ;
		case 4 : printf( "ERROR: failed to read/write header,is the volume open?\n" )				; break ;
		case 5 : printf( "INFO: operation terminater per user request\n" ) 					; break ;
		case 6 : printf( "ERROR: path to be used to create a back up file is occupied or permission denied\n" )	; break ;
		case 7 : printf( "ERROR: failed to restore\n" ) 							; break ;
		case 8 : printf( "ERROR: insufficient privilege to open backup header file for reading\n" ) 		; break ;
		case 9 : printf( "ERROR: invalid path to back up header file\n" ) 					; break ;
		case 10: printf( "ERROR: insufficient privilege to create a backup header in a destination folder\n" ) 	; break ;
		case 11: printf( "ERROR: invalid path to device\n" ) 							; break ;
		case 12: printf( "ERROR: argument for path to a backup  header file is missing\n" ) 			; break ;
		case 13: printf( "ERROR: argument for path to a backup  header file is missing\n" ) 			; break ;
		case 14: printf( "ERROR: only root and zulucrypt-system members can restore and back up luks headers on system devices\n" )	; break ;
		case 15: printf( "ERROR: insufficient privilege to open device for writing\n" ) 			; break ;
		case 16: printf( "ERROR: could not resolve path to device\n" ) 						; break ;
		case 17: printf( "ERROR: backup file does not appear to contain luks header\n" ) 			; break ;
		case 18: printf( "ERROR: insufficient privilege to open device for reading\n" ) 			; break ;
		case 19: printf( "ERROR: insufficient memory to hold your responce\n" )		 			; break ;
	}
	
	StringDelete( &xt ) ;
	if( fd != -1 )
		close( fd );
	
	return st == 1 ? 0 : st ;
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

static int create_work_directory( string_t * st )
{
	const char * temp_path = "/dev/shm/zuluCrypt/" ;
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	if( mkdir( temp_path,S_IRWXU ) != 0 ){
		if( errno == EEXIST ){
			/*
			 * directory exists,we can continue
			 */
			 ;
		}else if( errno == ENOENT ){
			/*
			 * one of the parent directory does not exist,assume there is no tmpfs and exit
			 */
			zuluCryptSecurityDropElevatedPrivileges() ;
			return 0 ;
		}else{
			/*
			 * whatever it is,it cant be good,exit
			 */
			return 0 ;
		}
	}
	
	chown( temp_path,0,0 ) ;
	*st = String( temp_path ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	return 1 ;
}

static int secure_file_path( char ** path,const char * source )
{
	int fd_source ;
	int fd_temp ;
	char buffer[ SIZE ] ;
	size_t len ;
	string_t st_path ;
	const char * temp_path ;
	
	if( !create_work_directory( &st_path ) )
		return 0 ;
	
	temp_path = StringAppendInt( st_path,syscall( SYS_gettid ) ) ;	
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	fd_source = open( source,O_RDONLY ) ;
	
	if( fd_source == -1 ){
		StringDelete( &st_path ) ;
		return 0 ;
	}
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	fd_temp = open( temp_path,O_WRONLY | O_CREAT ) ;
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

static inline int secure_file_path_1( char ** path )
{
	string_t st_path ;
	if( create_work_directory( &st_path ) ){
		StringAppendInt( st_path,syscall( SYS_gettid ) ) ;
		*path = StringDeleteHandle( &st_path ) ;
		return 1 ;
	}else{
		return 0 ;
	}
}

static inline int secure_copy_file( const char * source,const char * dest,uid_t uid )
{
	int st = 4 ;
	int fd_source ;
	int fd_dest ;
	size_t len ;
	char buffer[ SIZE ] ;
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	fd_dest = open( dest,O_WRONLY | O_CREAT ) ;
	if( fd_dest == -1 ){
		zuluCryptSecurityGainElevatedPrivileges() ;
		unlink( source ) ;
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
	
	unlink( source ) ;
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	return st ;
}

static int save_header( const char * device,const char * path,uid_t uid )
{
	int st = 4 ;
	struct crypt_device * cd ;
	char * temp_path ;
	
	if( !secure_file_path_1( &temp_path ) ){
		return 4 ;
	}

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
	
	free( temp_path ) ;
	return st ;
}

static int restore_header( const char * device,const char * path,int k,uid_t uid )
{
	struct crypt_device * cd  ;
	char * temp_path ;
	
	int st = 7;
	string_t confirm ;
	const char * warn = "\
Are you sure you want to replace a header on device \"%s\" with a backup copy at \"%s\"?\n\
Type \"YES\" and press Enter to continue: " ;
	if( uid ){;}
	if( k == -1 ){
		zuluCryptSecurityDropElevatedPrivileges() ;
		printf( warn,device,path ) ;
				
		confirm = StringGetFromTerminal_1( 3 ) ;
		if( confirm != StringVoid ){
			k = StringEqual( confirm,"YES" ) ;
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
	unlink( temp_path ) ;
	free( temp_path ) ;
	crypt_free( cd ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	return st ;
}

int zuluCryptEXESaveAndRestoreLuksHeader( const struct_opts * opts,uid_t uid,int option  )
{
	int dev_fd = -1 ;
	string_t sec_dev = StringVoid ;
	
	int fd = -1 ;
	const char * device = opts->device ;
	
	/*
	 * using key_key here because i do not want to introduce a key field in the structure.
	 */
	const char * path = opts->key ;
	int st ;
	int k ;
	string_t sec_file = StringVoid ;
		
	/*
	 * zuluCryptPartitionIsSystemPartition() is defined in partitions.c
	 */
	k = zuluCryptPartitionIsSystemPartition( device ) ;
		
	if( k == 1 ){
		if( uid != 0 || !zuluCryptUserIsAMemberOfAGroup( uid,"zulucrypt-system" ) )
		return zuluExit( 14,fd,sec_file ) ;
	}
		
	if( path == NULL ){
		if( option == LUKS_HEADER_RESTORE )
			return zuluExit( 12,fd,sec_file ) ;
		else
			return zuluExit( 13,fd,sec_file ) ;
	}
	
	if( strncmp( device,"/dev/",5 ) != 0 ){
		zuluCryptSecurityGainElevatedPrivileges() ;
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in ../lib/create_loop_device.c 
		 */
		if( zuluCryptAttachLoopDeviceToFile( device,O_RDWR,&dev_fd,&sec_dev ) ){
			device = StringContent( sec_dev ) ;
			if( option == LUKS_HEADER_RESTORE ){
				st = restore_header( device,path,opts->dont_ask_confirmation,uid ) ;
			}else{
				st = save_header( device,path,uid ) ;
			}
		}else{
			st = 7 ;
		}
	}else{
		if( option == LUKS_HEADER_RESTORE ){
			st = restore_header( device,path,opts->dont_ask_confirmation,uid ) ;
		}else{
			st = save_header( device,path,uid ) ;
		}
	}
	
	if( dev_fd != -1 )
		close( dev_fd ) ;
	StringDelete( &sec_dev ) ;
	
	return zuluExit( st,fd,sec_file ) ;
}
