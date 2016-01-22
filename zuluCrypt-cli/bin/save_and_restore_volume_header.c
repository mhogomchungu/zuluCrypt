
/*
 *
 *  Copyright (c) 2012-2015
 *  name : Francis Banyikwa
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
#include "truecrypt_support_1.h"

#define SIZE 512

#define ignore_result( x ) if( x ){;}

static int zuluExit( int st,const char * dev )
{
	switch( st ){
		case 0 : printf( gettext( "SUCCESS: Header saved successfully\n" ) )						; break ;
		case 1 : printf( gettext( "SUCCESS: Header restored successfully\n" ) )						; break ;
		case 2 : printf( gettext( "ERROR: Presented device is not a LUKS device\n" ) ) 					; break ;
		case 3 : printf( gettext( "ERROR: Failed to perform requested operation\n" ) )					; break ;
		case 4 : printf( gettext( "ERROR: Failed to perform requested operation\n" ) )					; break ;
		case 5 : printf( gettext( "INFO: Operation terminater per user request\n" ) )					; break ;
		case 6 : printf( gettext( "ERROR: Path to be used to create a back up file is occupied or permission denied\n" ) ); break ;
		case 7 : printf( gettext( "ERROR: Failed to perform requested operation\n" ) )					; break ;
		case 8 : printf( gettext( "ERROR: Insufficient privilege to open backup header file for reading\n" ) )		; break ;
		case 9 : printf( gettext( "ERROR: Invalid path to back up header file\n" ) )					; break ;
		case 10: printf( gettext( "ERROR: Insufficient privilege to create a backup header in a destination folder\n" )); break ;
		case 11: printf( gettext( "ERROR: Invalid path to device\n" ) )							; break ;
		case 12: printf( gettext( "ERROR: Argument for path to a backup  header file is missing\n" ) )			; break ;
		case 13: printf( gettext( "ERROR: Argument for path to a backup  header file is missing\n" ) )			; break ;
		case 14: printf( gettext( "ERROR: Only root and \"zulucrypt\" group members can restore and back up luks headers on system devices\n" )	); break ;
		case 15: printf( gettext( "ERROR: Insufficient privilege to open device for writing\n" ) )			; break ;
		case 16: printf( gettext( "ERROR: Could not resolve path to device\n" ) )					; break ;
		case 17: printf( gettext( "ERROR: Backup file does not appear to contain luks header\n" ) )			; break ;
		case 18: printf( gettext( "ERROR: Insufficient privilege to open device for reading\n" ) )			; break ;
		case 19: printf( gettext( "ERROR: Insufficient memory to hold your responce\n" ) )	 			; break ;
		case 20: printf( gettext( "ERROR: Wrong password entered or volume is not a truecrypt volume\n" ) )		; break ;
	}

	StringFree( dev ) ;

	if( st == 1 ){

		return 0 ;
	}else{
		return st ;
	}
}

/*
 * This place is a bit involving and an explanation seem to be the least i can do.
 *
 * We do not want cryptsetup and tcplay to access file paths a normal can modify for security
 * reasons and hence we copy user provided files to "/run/zuluCrypt" folder and then
 * pass these copied files to cryptsetup or tcplay.This happens when we are restoring
 * header files.
 *
 * When we are taking header backups,we let these tools create the header backups in
 * "/run/zuluCrypt" and then we copied these created header files from "/run/zuluCrypt"
 * to where the normal user expect them to be.
 *
 * normal user should zero access to "/run/zuluCrypt" path.
 */

/*
 * Below function creates a secured folder path,ie a folder path a normal user has no access to
 */
static string_t _create_work_directory( void )
{
	/*
	 * ZULUCRYPTtempFolder and ZULUCRYPtmountMiniPath are set in ../constants.h
	 */
	const char * temp_path = "/run/zuluCrypt/" ;
	struct stat xt ;
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH | S_IROTH ;

	zuluCryptSecurityGainElevatedPrivileges() ;

	#define path_does_not_exist( x ) stat( x,&xt ) != 0

	if( path_does_not_exist( "/run" ) ){

		mkdir( "/run",mode ) ;
		ignore_result( chown( "/run",0,0 ) ) ;
	}
	if( path_does_not_exist( temp_path ) ){

		mkdir( temp_path,S_IRWXU ) ;
		ignore_result( chown( temp_path,0,0 ) ) ;
	}

	zuluCryptSecurityDropElevatedPrivileges() ;

	return String( temp_path ) ;
}

/*
 * Below function copies a file owned and managed by a user to a secured location so that it can be accessed securely.
 */
static int _secure_file_path( const char ** path,const char * source )
{
	int fd_source ;
	int fd_temp ;
	char buffer[ SIZE ] ;
	size_t len ;
	const char * temp_path ;
	struct stat ststr ;

	string_t st_path = _create_work_directory() ;

	StringAppend( st_path,"0-" ) ;
	temp_path = StringAppendInt( st_path,syscall( SYS_gettid ) ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	fd_source = open( source,O_RDONLY ) ;

	if( fd_source == -1 ){

		StringDelete( &st_path ) ;
		return 0 ;
	}

	fstat( fd_source,&ststr ) ;

	if( ststr.st_size >= 3145728 ){
		/*
		 * headers are less than 3MB so we obvious have a wrong file
		 */
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

	while( 1 ){

		len = read( fd_source,buffer,SIZE ) ;

		if( len < SIZE ){

			ignore_result( write( fd_temp,buffer,len ) ) ;

			break ;
		}else{
			ignore_result( write( fd_temp,buffer,len ) ) ;
		}
	}

	close( fd_source ) ;
	close( fd_temp ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	*path = StringDeleteHandle( &st_path ) ;
	return 1 ;
}

/*
 * this function return a secured file path to be used to create a file at the path
 */
static const char * _secure_file_path_1( void )
{
	string_t st_path = _create_work_directory() ;

	StringAppend( st_path,"1-" ) ;
	StringAppendInt( st_path,syscall( SYS_gettid ) ) ;

	return StringDeleteHandle( &st_path ) ;
}

/*
 * Below function copies a secured file from secured location to a user owned and managed location.
 * The source file will be deleted when the copy is done.
 */
static int _secure_copy_file( const char * source,const char * dest,uid_t uid )
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

				ignore_result( write( fd_dest,buffer,len ) ) ;

				break ;				
			}else{
				ignore_result( write( fd_dest,buffer,len ) ) ;
			}
		}

		ignore_result( chmod( dest,S_IRUSR ) ) ;
		ignore_result( chown( dest,uid,uid ) ) ;

		st = 0 ;
	}

	/*
	 * zuluCryptDeleteFile() is defined in ../lib/file_path_security.c
	 */
	zuluCryptDeleteFile( source ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;
	return st ;
}

static int _save_luks_header( const struct_opts * opts,const char * temp_path,const char * path,uid_t uid  )
{
	struct crypt_device * cd ;
	int st ;

	if( crypt_init( &cd,opts->device ) != 0 ){

		st = 3 ;
	}else{
		st = crypt_header_backup( cd,NULL,temp_path ) ;

		crypt_free( cd ) ;

		if( st == 0 ){

			st = _secure_copy_file( temp_path,path,uid ) ;
		}else{
			st = 4 ;
		}
	}
	return st ;
}

static int zuluExit_1( int r,string_t st,string_t xt )
{
	StringMultipleDelete( &xt,&st,NULL ) ;
	return r ;
}

static string_t _get_password( int * r )
{
	string_t st = StringVoid ;

	*r = 1 ;

	printf( gettext( "Enter passphrase in the volume: " ) ) ;

	/*
	 * ZULUCRYPT_KEY_MAX_SIZE is set in ../constants.h
	 */
	StringSilentlyGetFromTerminal_1( &st,ZULUCRYPT_KEY_MAX_SIZE ) ;
	return st ;
}

static string_t _get_password_0( int * r )
{
	string_t st = StringVoid ;

	*r = 1 ;

	printf( gettext( "Enter passphrase in the header file: " ) ) ;

	/*
	 * ZULUCRYPT_KEY_MAX_SIZE is set in ../constants.h
	 */
	StringSilentlyGetFromTerminal_1( &st,ZULUCRYPT_KEY_MAX_SIZE ) ;
	return st ;
}

static int _modify_tcrypt( info_t * info,const struct_opts * opts )
{
	int k = 4 ;
	int r ;

	string_t st = StringVoid ;
	string_t xt = StringVoid ;

	if( StringsAreEqual( opts->key_source,"-p" ) ){

		info->header_key            = opts->key ;
		info->header_key_source     = "passphrase" ;
		info->header_new_key_source = "new_passphrase" ;

	}else if( opts->key == NULL && StringsAreNotEqual( opts->key_source,"-f" ) ){

		st = info->getKey( &r ) ;

		if( r ){
			info->key = StringContent( st ) ;
			info->header_key            = info->key ;
			info->header_key_source     = "passphrase" ;
			info->header_new_key_source = "new_passphrase" ;
		}else{
			return zuluExit_1( k,st,xt ) ;
		}
	}else{
		/*
		 * function is defined at "path_access.c"
		 */
		zuluCryptGetPassFromFile( opts->key,info->uid,&st ) ;

		zuluCryptSecurityGainElevatedPrivileges() ;

		if( st == StringVoid ){

			return zuluExit_1( k,st,xt ) ;
		}else{
			if( StringHasComponent( opts->key,".zuluCrypt-socket" ) ){

				info->key = StringContent( st ) ;
				info->header_key            = info->key ;
				info->header_key_source     = "passphrase" ;
				info->header_new_key_source = "new_passphrase" ;
			}else{
				/*
				 * zuluCryptCreateKeyFile_1 is defined in ../lib/open_tcrypt.c
				 */
				xt = zuluCryptCreateKeyFile_1( st,"tcrypt-bk-" ) ;

				if( xt == StringVoid ){

					return zuluExit_1( k,st,xt ) ;
				}else{
					info->key = StringContent( xt ) ;
					info->header_key            = info->key ;
					info->header_key_source     = "keyfiles" ;
					info->header_new_key_source = "new_keyfiles" ;
				}
			}
		}
	}

	info->header_new_key = info->header_key ;

	/*
	 * zuluCryptModifyTcryptHeader() is defined in ../lib/create_tcrypt.c
	 */
	k = zuluCryptModifyTcryptHeader( info ) ;

	if( xt != StringVoid ){
		/*
		 * zuluCryptDeleteFile_1() is defined in ../lib/file_path_security.c
		 */
		zuluCryptDeleteFile_1( xt ) ;
	}

	return zuluExit_1( k,st,xt ) ;
}

static int _save_truecrypt_header( const struct_opts * opts,const char * temp_path,const char * path,uid_t uid )
{
	int r ;

	/*
	 * info_t structure is declared in ../lib/include.h
	 */
	info_t info ;

	memset( &info,'\0',sizeof( info_t ) ) ;

	info.device        = opts->device ;
	info.header_source = "save_header_to_file" ;
	info.getKey        = _get_password ;
	info.tmp_path      = temp_path ;
	info.uid           = uid ;
	info.rng           = opts->rng ;
	info.opt           = opts->m_opts ;

	r = _modify_tcrypt( &info,opts ) ;

	if( opts->key == NULL && StringsAreNotEqual( opts->key_source,"-f" ) ){

		printf( "\n" ) ;
	}
	if( r == TC_OK ){

		return _secure_copy_file( temp_path,path,uid ) ;
	}else{
		return 20 ;
	}
}

static int _restore_truecrypt_header( const struct_opts * opts,const char * temp_path,uid_t uid )
{
	int r ;
	/*
	 * info_t structure is declared in ../lib/include.h
	 */
	info_t info ;

	memset( &info,'\0',sizeof( info_t ) ) ;

	info.device        = opts->device ;
	info.header_source = "header_from_file" ;
	info.getKey        = _get_password_0 ;
	info.tmp_path      = temp_path ;
	info.uid           = uid ;
	info.rng           = opts->rng ;
	info.opt           = opts->m_opts ;

	r = _modify_tcrypt( &info,opts ) ;

	if( opts->key == NULL && StringsAreNotEqual( opts->key_source,"-f" ) ){

		printf( "\n" ) ;
	}
	if( r == TC_OK ){

		return 1 ;
	}else{
		return 20 ;
	}
}

static int _save_header( const struct_opts * opts,const char * path,uid_t uid )
{
	int st = 4 ;
	const char * temp_path = _secure_file_path_1() ;

	zuluCryptSecurityGainElevatedPrivileges() ;

	if( zuluCryptVolumeIsLuks( opts->device ) ){

		st = _save_luks_header( opts,temp_path,path,uid ) ;
	}else{
		st = _save_truecrypt_header( opts,temp_path,path,uid ) ;
	}

	zuluCryptSecurityDropElevatedPrivileges() ;

	StringFree( temp_path ) ;
	return st ;
}

static int _restore_luks_header( const struct_opts * opts,const char * temp_path )
{
	int st ;
	struct crypt_device * cd  ;	

	if( crypt_init( &cd,opts->device ) != 0 ){

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

		crypt_free( cd ) ;
	}
	return st ;
}

static int _restore_header( const struct_opts * opts,const char * dev,const char * path,int ask_confirmation,uid_t uid )
{
	const char * temp_path = NULL ;
	int k ;
	int st = 7;
	string_t confirm ;
	const char * warn = gettext( "\
Are you sure you want to replace a header on device \"%s\" with a backup copy at \"%s\"?\n\
Type \"YES\" and press Enter to continue: " ) ;
	if( uid ){;}

	if( ask_confirmation ){
		zuluCryptSecurityDropElevatedPrivileges() ;

		printf( warn,dev,path ) ;

		confirm = StringGetFromTerminal_1( 3 ) ;

		if( confirm != StringVoid ){

			k = StringsAreEqual_2( confirm,gettext( "YES" ) );
			StringDelete( &confirm ) ;

			if( k == 0 ){

				return 5 ;
			}
		}else{
			return 19 ;
		}
	}

	if( _secure_file_path( &temp_path,path ) ){

		zuluCryptSecurityGainElevatedPrivileges() ;

		if( zuluCryptVolumeIsLuks( temp_path ) ){
			/*
			 * temp_path will point to a header back up file and it is assumed that a user
			 * want to restore a header to a luks volume since the backup header has luks signature
			 */
			st = _restore_luks_header( opts,temp_path ) ;
		}else{
			/*
			 * the header back up has no luks signature and hence it is assumed that a request is
			 * made to restore a truecrypt header backup
			 */
			st = _restore_truecrypt_header( opts,temp_path,uid ) ;
		}

		zuluCryptDeleteFile( temp_path ) ;

		StringFree( temp_path ) ;

		zuluCryptSecurityDropElevatedPrivileges() ;

		return st ;
	}else{
		return 7 ;
	}
}

int zuluCryptEXESaveAndRestoreVolumeHeader( const struct_opts * opts,uid_t uid,int option  )
{
	const char * device = opts->device ;

	const char * path = opts->back_up_file_path ;
	int st ;
	int k ;

	const char * dev = NULL ;
	const char * dev_1 = NULL ;

	if( StringPrefixEqual( device,"/dev/loop" ) ){

		/*
		 * zuluCryptLoopDeviceAddress_1() is defined in ../lib/create_loop_device.c
		 */

		dev = zuluCryptLoopDeviceAddress_1( device ) ;		
		dev_1 = dev ;

		/*
		 * zuluCryptPartitionIsSystemPartition() is defined in partitions.c
		 */

		k = zuluCryptPartitionIsSystemPartition( dev,uid ) ;
	}else{
		dev_1 = device ;
		/*
		 * zuluCryptPartitionIsSystemPartition() is defined in partitions.c
		 */
		k = zuluCryptPartitionIsSystemPartition( device,uid ) ;
	}

	if( k == 1 ){

		if( uid != 0 || !zuluCryptUserIsAMemberOfAGroup( uid,"zulucrypt" ) ){

			return zuluExit( 14,dev ) ;
		}
	}

	if( path == NULL ){

		if( option == VOLUME_HEADER_RESTORE ){

			return zuluExit( 12,dev ) ;
		}else{
			return zuluExit( 13,dev ) ;
		}
	}

	if( option == VOLUME_HEADER_RESTORE ){

		st = _restore_header( opts,dev_1,path,opts->ask_confirmation,uid ) ;
	}else{
		st = _save_header( opts,path,uid ) ;
	}

	return zuluExit( st,dev ) ;
}

static int _files_are_equal( const char * file1,const char * file2 )
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

	/*
	 * headers are less than 3MB,anything larger is automatically an error
	 */
	if( st1.st_size < 3145728 && st2.st_size < 3145728 ){

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
	}else{
		return 0 ;
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

	_secure_file_path( &header_path,header_backup ) ;

	if( header_path == NULL ){

		return 0 ;
	}

	device_header = _secure_file_path_1() ;

	if( device_header == NULL ){

		StringFree( header_path ) ;
		return 0 ;
	}

	zuluCryptSecurityGainElevatedPrivileges() ;

	if( _save_tmp_header( device,device_header ) ){

		st = _files_are_equal( header_path,device_header ) ;
	}

	zuluCryptDeleteFile( header_path ) ;
	zuluCryptDeleteFile( device_header ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	StringFree( header_path ) ;
	StringFree( device_header ) ;

	return st ;
}
