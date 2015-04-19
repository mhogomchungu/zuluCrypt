
/*
 *
 *  Copyright (c) 2012
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <blkid/blkid.h>
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <sys/stat.h>

#include <libcryptsetup.h>

#define SIZE 1024
#define KEY_SIZE 128

#include <signal.h>

static int __exit_as_requested ;
static int __sig_caught ;

void sigTERMhandler( int sig )
{
	__exit_as_requested = 1 ;
	__sig_caught = sig ;
	printf( "\nINFO: caught signal %d,cleaning up\n",__sig_caught ) ;
}

/*
 * define in partition.c
 *
 * the function is used to check is a presented path is a system partition or not *
 */

static int zuluExit( stringList_t stl, int status )
{
	ssize_t index ;
	switch( status ){
		case 0 : printf( gettext( "SUCCESS: Mapper created successfully\n" ) ) ;
			 index = StringListHasStartSequence( stl,crypt_get_dir() ) ;
			 if( index >= 0 ){
				 printf( gettext( "Opened mapper path: " ) ) ;
				 StringListPrintLineAt( stl,index ) ;
			 }
			 break ;
		case 1 : printf( gettext( "ERROR: Could not create mapper\n" ) )					;break ;
		case 2 : printf( gettext( "ERROR: Could not resolve device path\n" ) )					;break ;
		case 3 : printf( gettext( "\nSUCCESS: Random data successfully written\n" ) )				;break ;
		case 5 : printf( gettext( "INFO: User chose not to proceed\n" ) ) 					;break ;
		case 8 : printf( gettext( "ERROR: Insufficitied privilege to oped device \n" ) ) 			;break ;
		case 9 : printf( gettext( "ERROR: Device path is invalid\n" ) )						;break ;
		case 10: printf( gettext( "ERROR: Passphrase file does not exist\n" ) ) 				;break ;
		case 11: printf( gettext( "ERROR: Could not get enought memory to hold the key file\n" ) ) 	 	;break ;
		case 12: printf( gettext( "ERROR: Insufficient privilege to open key file for reading\n" ) )	       	;break ;
		case 13: printf( gettext( "ERROR: Can not open a mapper on a device with an opened mapper\n" ) )	;break ;
		case 14: printf( gettext( "ERROR: Can not open a mapper on a mounted device\n" ) ) 			;break ;
		case 15: printf( gettext( "INFO: Signal caught,exiting prematurely\n" ) )				;break ;
		case 16: printf( gettext( "ERROR: Can not get passphrase in silent mode\n" ) )				;break ;
		case 17: printf( gettext( "ERROR: Insufficient memory to hold passphrase\n" ) ) 			;break ;
		case 18: printf( gettext( "ERROR: Insufficient memory to hold 3 characters?really?\n" ) ) 		;break ;
		case 19: printf( gettext( "ERROR: Insufficient privilege to open the file with your privileges?\n" ) )  ;break ;
	}

	StringListClearDelete( &stl ) ;
	return status ;
}

static int open_plain_as_me_1(const struct_opts * opts,const char * mapping_name,uid_t uid,int op )
{
	/*
	 * Below is a form of memory management.All strings are collected in a stringlist object to easily delete them
	 * when the function returns.This allows for the function to have multiple exit points without risks of leaking
	 * memory from manually examining each exit point to make sure all strings are deleted or go with multiple goto
	 * code deleting blocks to take into account different exit points.
	 */
	stringList_t stl ;
	string_t * stringArray  = StringListArray( &stl,5 ) ;
	string_t * mapper     = &stringArray[ 0 ] ;
	string_t * passphrase = &stringArray[ 1 ] ;
	string_t * p          = &stringArray[ 2 ] ;
	string_t * dev_st     = &stringArray[ 3 ] ;
	string_t * dev_1      = &stringArray[ 4 ] ;

	size_t len = 0 ;

	const char * source      = opts->key_source ;
	const char * pass        = opts->key ;

	int k = opts->ask_confirmation ;

	const char * cpass = NULL ;

	char * d ;

	const char * device = opts->device ;
	const char * dev = opts->device ;

	int j ;
	int n ;

	const char * cmapper ;

	if( StringPrefixEqual( device,"/dev/loop" ) ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ../lib/create_loop_device.c
		 */
		d = zuluCryptLoopDeviceAddress( device ) ;
		*dev_st = StringInherit( &d ) ;
		dev = StringContent( *dev_st ) ;
		*dev_1 = StringCopy( *dev_st ) ;
		device = StringReplaceString( * dev_1,"\\040"," " ) ;
	}

	/*
	 * zuluCryptPartitionIsSystemPartition() is defined in ./partition.c
	 */
	if( zuluCryptPartitionIsSystemPartition( device,uid ) ){
		if( uid != 0 ){
			return zuluExit( stl,8 ) ;
		}
	}
	/*
	 * ZULUCRYPTlongMapperPath and ZULUCRYPTshortMapperPath are in ../constants.h
	 * zuluCryptCreateMapperName() is defined at ../lib/create_mapper_name.c
	 */
	*mapper = zuluCryptCreateMapperName( device,mapping_name,uid,ZULUCRYPTshortMapperPath ) ;

	*p = zuluCryptCreateMapperName( device,mapping_name,uid,ZULUCRYPTlongMapperPath ) ;

	j = zuluCryptCheckOpenedMapper( StringContent( *p ) ) ;

	/*
	 * zuluCryptPartitionIsMounted() is defined in ../lib/process_mountinfo.c
	 */
	n = zuluCryptPartitionIsMounted( dev ) ;

	if( j == 1 ){
		return zuluExit( stl,13 ) ;
	}
	if( n == 1 ){
		return zuluExit( stl,14 ) ;
	}
	if( k == 0 ){
		*passphrase = StringRandomString( 64 ) ;
		cpass = StringContent( *passphrase ) ;
		len = StringLength( *passphrase ) ;
	}else if( source == NULL ){
		printf( gettext( "Enter passphrase: " ) ) ;
		/*
		 * ZULUCRYPT_KEY_MAX_SIZE is set in ../constants.h
		 */
		switch( StringSilentlyGetFromTerminal_1( passphrase,ZULUCRYPT_KEY_MAX_SIZE ) ){
			case 1 : return zuluExit( stl,16 ) ;
			case 2 : return zuluExit( stl,17 ) ;
		}
		printf( "\n" ) ;
		cpass = StringContent( *passphrase ) ;
		len = StringLength( *passphrase ) ;
	}else{
		if( strcmp( source,"-p" ) == 0 ){
			*passphrase = String( pass ) ;
			cpass = StringContent( *passphrase ) ;
			len = StringLength( *passphrase ) ;
		}else if( strcmp( source,"-f" ) == 0 ){
			/*
			 * zuluCryptGetPassFromFile() is defined at "path_access.c"
			 */
			switch( zuluCryptGetPassFromFile( pass,uid,passphrase ) ){
				case 1 : return zuluExit( stl,10 ) ;
				case 2 : return zuluExit( stl,11 ) ;
				case 4 : return zuluExit( stl,12 ) ;
			}
			cpass = StringContent( *passphrase ) ;
			len = StringLength( *passphrase ) ;
		}
	}

	if( zuluCryptSecurityGainElevatedPrivileges() ){
		/*
		 * zuluCryptOpenPlain() is defined in ../lib/open_plain.c
		 */
		if( zuluCryptOpenPlain( device,StringContent( *mapper ),"rw",cpass,len ) != 0 ){
			zuluCryptSecurityDropElevatedPrivileges() ;
			return zuluExit( stl,1 ) ;
		}
	}

	zuluCryptSecurityDropElevatedPrivileges() ;

	/*
	 * Create a mapper path(usually at /dev/mapper) associated with opened plain mapper above.
	 */
	cmapper = StringMultiplePrepend( *mapper,"/",crypt_get_dir(),NULL ) ;

	/*
	 *  mapper path is usually a soft link to /dev/dm-X
	 *  resolve the mapper path to its respective /dev/dm-X and set permission on it.
	 *
	 * We set permission of /dev/dm-X pointing to the device to "u+rw" because we want notmal user to be able
	 * to write to the device through the mapper.
	 *
	 * Useful when a normal user want to delete content of the device by writing random data to it.
	 */
	d = zuluCryptRealPath( cmapper ) ;
	if( zuluCryptSecurityGainElevatedPrivileges() ){
		if( d != NULL ){
			chown( d,uid,0 ) ;
			chmod( d,S_IRWXU ) ;
			StringFree( d ) ;
		}
		zuluCryptSecurityDropElevatedPrivileges() ;
	}else{
		return zuluExit( stl,1 ) ;
	}

	if( op == 1 ){
		return zuluExit( stl,0 ) ;
	}else{
		StringListClearDelete( &stl ) ;
		return 0 ;
	}
}

int zuluCryptEXEOpenPlainAsMe(const struct_opts * opts,const char * mapping_name,uid_t uid )
{
	return open_plain_as_me_1( opts,mapping_name,uid,1 ) ;
}

/*
 * Purpose of this function is to open a device and write random data to it as a way of hiding information on the disk.
 *
 * The above is accomplished by opening a plain mapper against the device and then write to the device through the mapper
 *
 */
int zuluCryptEXEWriteDeviceWithJunk( const struct_opts * opts,const char * mapping_name,uid_t uid )
{
	stringList_t stl   = StringListInit() ;
	string_t * mapper  = StringListAssign( stl ) ;
	string_t * confirm = StringListAssign( stl );

	double size ;
	double size_written ;

	const char * device =  opts->device ;

	char buffer[ SIZE ] ;

	int ratio ;
	int prev_ratio ;
	int k ;

	struct sigaction sigac;

	memset( &sigac,'\0',sizeof( struct sigaction ) ) ;

	sigac.sa_handler = &sigTERMhandler ;

	sigaction( SIGINT,&sigac,NULL ) ;
	sigaction( SIGTERM,&sigac,NULL ) ;
	sigaction( SIGHUP,&sigac,NULL ) ;

	__exit_as_requested = 0 ;

	if( ( k = open_plain_as_me_1( opts,mapping_name,uid,0 ) ) != 0 ){
		return k ;
	}
	*mapper = zuluCryptCreateMapperName( device,mapping_name,uid,ZULUCRYPTshortMapperPath ) ;

	StringMultiplePrepend( *mapper,"/",crypt_get_dir(),NULL ) ;

	if( opts->ask_confirmation ){
		printf( gettext( "\nWARNING, device \"%s\" will be overwritten with random data destroying all present data.\n" ),device ) ;
		printf( gettext( "Are you sure you want to proceed? Type \"YES\" and press enter if you are sure: " ) ) ;

		*confirm = StringGetFromTerminal_1( 3 ) ;
		if( *confirm == StringVoid ){
			return zuluExit( stl,17 ) ;
		}else{
			k = StringEqual( *confirm,gettext( "YES" ) ) ;

			if( k == 0 ){
				if( zuluCryptSecurityGainElevatedPrivileges() ){
					zuluCryptCloseMapper( StringContent( *mapper ) ) ;
					zuluCryptSecurityDropElevatedPrivileges() ;
				}
				return zuluExit( stl,5 ) ;
			}
		}
	}

	k = open( StringContent( *mapper ),O_WRONLY ) ;

	size = ( double ) blkid_get_dev_size( k ) ;

	memset( buffer,0,SIZE ) ;

	size_written = 0 ;
	prev_ratio = -1 ;

	while( write( k,buffer,SIZE ) > 0 ){

		if( __exit_as_requested == 1 ){
			break ;
		}
		size_written += SIZE ;

		ratio = ( int ) ( ( size_written / size ) * 100 ) ;

		if( ratio > prev_ratio ){
			printf( "\r%s %d%%",gettext( "percentage complete: " ),ratio ) ;
			fflush( stdout );
			prev_ratio = ratio ;
		}
	}

	close( k ) ;
	if( zuluCryptSecurityGainElevatedPrivileges() ){
		zuluCryptCloseMapper( StringContent( *mapper ) ) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
	}

	if( __exit_as_requested == 1 ){
		return zuluExit( stl,15 ) ;
	}else{
		return zuluExit( stl,3 ) ;
	}
}
