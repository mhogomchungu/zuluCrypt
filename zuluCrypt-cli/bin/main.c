/*
 *
 *  Copyright ( c ) 2011-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
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
#include "../lib/libzuluCrypt.h"
#include <signal.h>
#include <sys/resource.h>
#include <locale.h>
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <grp.h>

static void _seteuid( uid_t uid )
{
	if( seteuid( uid ) ){;}
}

static void _setuid( uid_t uid )
{
	if( setuid( uid ) ){;}
}

static int zuluCryptEXEGetDevice( const char * device )
{
	/*
	 * zuluCryptVolumeDeviceName() is defined in ../lib/status.c
	 */
	char * c = NULL ;
	int st = 1 ;

	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptVolumeDeviceName() is defined in ../lib/status.c
	 */
	c = zuluCryptVolumeDeviceName( device ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	if( c == NULL ){

		printf( gettext( "ERROR: Could not get device address from mapper address\n" ) ) ;
		st = 1 ;
	}else{
		printf( "%s\n",c ) ;
		StringFree( c ) ;
		st = 0 ;
	}

	return st ;
}

static int zuluCryptEXECheckIfLuks( const char * device )
{
	int status ;

	char * e ;

	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * this zuluCryptVolumeIsLuks() is defined in ../lib/is_luks.c
	 */
	status = zuluCryptVolumeIsLuks( device ) ;

	/*
	 * zuluCryptUUIDFromPath_1() is defined in ../lib/blkid_evaluate_tag.c
	 */
	e = zuluCryptUUIDFromPath_1( device ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	if( status ){

		printf( gettext( "Device is a luks volume\n" ) ) ;

		if( e ){

			printf( "%s %s\n",gettext( "Its UUID is:" ),e ) ;
		}
	}else{
		printf( gettext( "Device is not a luks volume\n" ) ) ;
	}

	StringFree( e ) ;

	return status ;
}

static int zuluCryptEXECheckIfTcrypt( struct_opts * clargs,uid_t uid )
{
	string_t st_key     = StringVoid ;

	const char * device = clargs->device ;
	const char * key    = clargs->key ;
	const char * source = clargs->key_source ;

	size_t key_len ;

	int st = 1 ;

	if( key == NULL ){

		printf( gettext( "ERROR: Key argument is missing\n" ) ) ;
		return 1 ;
	}
	if( source == NULL ){

		printf( gettext( "ERROR: Key source argument is missing\n" ) ) ;
		return 1 ;
	}
	if( StringsAreEqual( source,"-p" ) ){

		zuluCryptSecurityGainElevatedPrivileges() ;

		/*
		 * zuluCryptGetVolumeType() is defined in ../lib/volume_type.c
		 */

		if( zuluCryptGetVolumeType( device,key,strlen( key ) ) == 2 ){

			printf( gettext( "\"%s\" is a tcrypt device\n" ),device ) ;
			st = 0 ;
		}

		zuluCryptSecurityDropElevatedPrivileges() ;

	}else if( StringsAreEqual( source,"-f" ) ){
		/*
		 * zuluCryptGetPassFromFile() is defined in path_access.c
		 */
		if( zuluCryptGetPassFromFile( NULL,key,uid,&st_key ) == 0 ){

			key = StringContent( st_key ) ;
			key_len = StringLength( st_key ) ;
			zuluCryptSecurityGainElevatedPrivileges() ;

			if( zuluCryptGetVolumeType( device,key,key_len ) == 2 ){

				printf( gettext( "\"%s\" is a tcrypt device\n" ),device ) ;
				st =  0 ;
			}else{
				printf( gettext( "\"%s\" is a not tcrypt device\n" ),device ) ;
				st = 1 ;
			}
			StringDelete( &st_key ) ;
			zuluCryptSecurityDropElevatedPrivileges() ;
		}else{
			printf( gettext( "\"%s\" is not a tcrypt device\n"),device ) ;
			st = 1 ;
		}
	}else{
		/*
		* shouldnt get here
		*/
		printf( gettext( "\"%s\" is not a tcrypt device\n" ),device ) ;
		st = 1 ;
	}

	return st  ;
}

static int zuluCryptEXECheckEmptySlots( const char * device )
{
	int status ;
	char * c  ;
	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptEmptySlots() is defined in ../lib/empty_slots.c
	 */
	c = zuluCryptEmptySlots( device ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	if( c == NULL ){

		printf( gettext( "Device \"%s\" is not a luks device\n" ),device ) ;
		status = 2 ;
	}else{
		printf( "%s\n",c ) ;
		status = 0 ;
		free( c ) ;
	}

	return status ;
}

static int zuluCryptEXECheckUUID( const char * device )
{
	printf( "%s\n",device ) ;
	return 0 ;
}

static int zuluCryptEXEHeaderMatchBackUpHeader( const char * device,const char * header_backup,uid_t uid )
{
	/*
	 * zuluCryptPartitionIsSystemPartition() is defined in partitions.c
	 */
	int r = zuluCryptPartitionIsSystemPartition( device,uid ) ;

	if( r == 1 ){

		if( uid != 0 || !zuluCryptUserIsAMemberOfAGroup( uid,"zulucrypt" ) ){

			printf( gettext( "ERROR: Insufficient privilges to operate on a system device\n" ) ) ;
			return 1 ;
		}
	}
	/*
	 * zuluCryptHeaderMatchBackUpHeader() is defined in save_and_restore_luks_header.c
	 */
	r = zuluCryptHeaderMatchBackUpHeader( device,header_backup,uid ) ;

	if( r ){
		printf( gettext( "Header backup match the one on the device\n" ) ) ;
		return 0 ;
	}else{
		printf( gettext( "Header backup does not match the one on the device\n" ) ) ;
		return 1 ;
	}
}

static int zuluCryptEXE( struct_opts * clargs,const char * mapping_name,uid_t uid )
{
	switch( clargs->action ){

		case 'H' : return zuluCryptEXEHeaderMatchBackUpHeader( clargs->device,clargs->key,uid ) ;
		case 'W' : return zuluCryptEXECheckIfTcrypt( clargs,uid ) ;
		case 'B' : return zuluCryptEXESaveAndRestoreVolumeHeader( clargs,uid,VOLUME_HEADER_SAVE ) ;
		case 'R' : return zuluCryptEXESaveAndRestoreVolumeHeader( clargs,uid,VOLUME_HEADER_RESTORE ) ;
		case 'J' : return zuluCryptEXEOpenPlainAsMe( clargs,mapping_name,uid ) ; /* function is defined in write_device_with_junk.c */
		case 'X' : return zuluCryptEXEWriteDeviceWithJunk( clargs,mapping_name,uid ) ;
		case 'w' : return zuluCryptEXECheckUUID( clargs->device ) ;
		case 'b' : return zuluCryptEXECheckEmptySlots( clargs->device ) ;
		case 'i' : return zuluCryptEXECheckIfLuks( clargs->device ) ;
		case 'P' : return zuluCryptEXEGetDevice( clargs->device ) ;
		case 's' : return zuluCryptEXEVolumeInfo( mapping_name,clargs->device,uid ) ;
		case 'q' : return zuluCryptEXECloseVolume( clargs->device,mapping_name,uid ) ;
		case 'o' : return zuluCryptEXEOpenVolume( clargs,mapping_name,uid ) ;
		case 'O' : return zuluCryptEXEOpenVolume( clargs,mapping_name,uid ) ;
		case 'c' : return zuluCryptEXECreateVolume( clargs,mapping_name,uid ) ;
		case 'a' : return zuluCryptEXEAddKey( clargs,uid ) ;
		case 'r' : return zuluCryptEXERemoveKey( clargs,uid );
		case 'E' : return zuluCryptExeFileEncrypt( clargs,uid ) ;
		case 'D' : return zuluCryptExeFileDecrypt( clargs,uid ) ;
	}

	printf( gettext( "ERROR!!!!!!!!!!: cli option missed!\n" ) ) ;
	return 200 ; /* shouldnt get here */
}

static int zuluExit( int st,stringList_t stl,stringList_t stx,const char ** env,const char * msg )
{
	zuluCryptSecurityUnlockMemory( stl ) ;
	StringListClearDelete( &stl ) ;
	StringListDelete( &stx ) ;

	if( env != NULL ){

		StringFree( env ) ;
	}
	if( msg != NULL ){

		printf( "%s\n",msg ) ;
	}
	return st ;
}

static void ExitOnMemoryExaustion( void )
{
	printf( gettext( "Unexpected exiting because you have run out of memory\n" ) ) ;
	exit( 1 ) ;
}

static int _print_uuid_from_path( const char * device )
{
	/*
	 * zuluCryptSecurityUUIDFromPath() is defined in path_access.c
	 */
	char * e = zuluCryptUUIDFromPath( device ) ;

	if( e == NULL ){

		puts( "UUID=\"Nil\"" ) ;
		return 1 ;
	}else{
		printf( "UUID=\"%s\"\n",e ) ;
		StringFree( e ) ;
		return 0 ;
	}
}

static void _privilegeEvelationError( const char * msg )
{
	perror( msg ) ;
	exit( 255 ) ;
}

static void _forceTerminateOnSeriousError( int sig )
{
	if( sig ){;}
	puts( "SIGSEGV caught,exiting" ) ;
	exit( 255 ) ;
}

static int _clear_dead_mappers( uid_t uid )
{
	/*
	 * zuluCryptClearDeadMappers() is defined in ../zuluCrypt-cli/bin/clear_dead_mappers.c
	 */
	zuluCryptClearDeadMappers( uid,0 ) ;
	return 0 ;
}

static int _printOpenedVolumes( uid_t uid )
{
	/*
	 * zuluCryptOpenedVolumesList() is defined in ../lib/process_mountinfo.c
	 */
	stringList_t stl ;

	zuluCryptSecurityGainElevatedPrivileges() ;

	stl = zuluCryptOpenedVolumesList( uid ) ;

	zuluCryptSecurityDropElevatedPrivileges() ;

	StringListPrintList( stl ) ;

	StringListDelete( &stl ) ;

	return 0 ;
}

int main( int argc,char * argv[] )
{
	int fd1 = -1 ;
	int fd = -1;
	const char * device ;
	const char * mapping_name ;
	char * ac ;
	char * ac_1 ;
	char * dev ;
	char action ;
	int st ;
	const char ** env = NULL ;
	ssize_t i ;

	string_t q = StringVoid ;

	stringList_t stl = StringListVoid ;
	stringList_t stx = StringListVoid ;

	struct_opts clargs ;

	uid_t uid = getuid() ;
	gid_t gid = uid ;

	struct sigaction sa ;
	memset( &sa,'\0',sizeof( struct sigaction ) ) ;
	sa.sa_handler = _forceTerminateOnSeriousError ;
	sigaction( SIGSEGV,&sa,NULL ) ;

	zuluCryptExeSetOriginalUID( uid ) ;

	/*
	 * zuluCryptDisableMetadataLocking() is defined in ../lib/create_luks.c
	 */
	zuluCryptDisableMetadataLocking() ;

	if( argc >= 2 && StringsAreEqual( argv[ 1 ],"--test" ) ){

		/*
		 * zuluCryptRunTest() is defined in test.c
		 */
		return zuluCryptRunTest() ;
	}

	/*
	 * setgroups() requires seteuid(0) ;
	 */
	_seteuid( 0 ) ;

	if( setgroups( 1,&gid ) != 0 ){

		_privilegeEvelationError( gettext( "ERROR: setgroups() failed" ) ) ;
	}
	if( setegid( uid ) != 0 ){

		_privilegeEvelationError( gettext( "ERROR: setegid() failed" ) ) ;
	}

	setlocale( LC_ALL,"" );
	bindtextdomain( "zuluCrypt-cli",TRANSLATION_PATH ) ;
	textdomain( "zuluCrypt-cli" ) ;

	memset( &clargs,'\0',sizeof( struct_opts ) ) ;

	/*
	 * zuluCryptEXEGetOpts() is defined in ./get_opts.c
	 */
	zuluCryptEXEGetOpts( argc,argv,&clargs ) ;

	/*
	 * zuluCryptSecurityConvertUID() is defined in security.c
	 */
	st = zuluCryptSecurityConvertUID( uid,clargs.uid ) ;

	if( st == -1 ){

		printf( gettext( "User is not root\n" ) ) ;
		return 114 ;
	}else{
		uid = (uid_t)st ;
	}

	/*
	 * Run with higher priority to speed things up
	 */
	setpriority( PRIO_PROCESS,0,-15 ) ;

	_setuid( 0 ) ;
	_seteuid( uid ) ;

	/*
	 * zuluCryptSetUserUIDForPrivilegeManagement() is defined in ./security.c
	 */
	zuluCryptSetUserUIDForPrivilegeManagement( uid ) ;

	/*
	 * zuluCryptSecuritySetPrivilegeElevationErrorFunction() is defined in ./security.c
	 * _privilegeEvelationError() function will be called when functions that elevate or drop privilges fail
	 */
	zuluCryptSecuritySetPrivilegeElevationErrorFunction( _privilegeEvelationError ) ;

	StringExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	StringListExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	ProcessExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	SocketExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;

	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in ./security.c
	 */
	zuluCryptSecurityDropElevatedPrivileges() ;

	if( argc == 1 ){

		zuluCryptEXEHelp();
		return 1;
	}
	if( argc == 2 ){

		ac = argv[ 1 ] ;

		if( StringAtLeastOneMatch_1( ac,"-h","--help","-help",NULL ) ){

			zuluCryptEXEHelp();

			return 0 ;
		}
		if( StringAtLeastOneMatch_1( ac,"-v","-version","--version",NULL ) ){

			printf( "%s\n",zuluCryptVersion() );

			return 0 ;
		}
		if( StringsAreEqual( ac,"--clear-dead-mount-points" ) ){

			zuluCryptClearDeadMappers( uid,1 ) ;
			return 0 ;
		}
	}

	/*
	 * this object is used as a form of memory management.It collects all string objects to make them easily deletable
	 * at the end of the function and allows a function to have easily managebale multiple exit points.
	 */
	stl = StringListInit() ;

	/*
	 * zuluCryptSecuritySanitizeTheEnvironment() is defined in ./security.c
	 */
	zuluCryptSecuritySanitizeTheEnvironment( uid,&stx ) ;

	/*
	 * clargs.env contains a copy of the inherited environment because the above function clears it because we dont need it.
	 * The copy will ultimately be passed to a plugin architecture system just in case a plugin needs it.
	 *
	 * The plugin system is managed by code in ../pluginManager/zuluCryptPluginManager.c
	 *
	 */
	clargs.env = env = StringListStringArray( stx ) ;

	q = StringListAssignString( stl,String( "" ) ) ;

	if( argc > 0 ){

		while( *argv ){

			StringMultipleAppend( q,*argv," ",NULL ) ;

			argv++ ;
		}
		StringSubChar( q,StringLength( q ) - 1,'\0' ) ;
	}

	clargs.argv = StringListContentAtFirstPlace( stl ) ;

	/*
	 * Hide "sensitive" command line arguments from ps comamnd and related tools.
	 * Best way to pass keys to zuluCrypt-cli is to use libzuluCryptPluginManager API
	 */
	#define hide( z ) strncpy( ( char * )z,"x",StringLength( q ) )

	if( clargs.key != NULL ){

		q = StringListAssignString( stl,String( clargs.key ) ) ;
		hide( clargs.key ) ;
		clargs.key = StringContent( q ) ;
	}
	if( clargs.new_key != NULL ){

		q = StringListAssignString( stl,String( clargs.new_key ) ) ;
		hide( clargs.new_key ) ;
		clargs.new_key = StringContent( q ) ;
		clargs.tcrypt_hidden_volume_key = clargs.new_key ;
	}
	if( clargs.existing_key != NULL ){

		q = StringListAssignString( stl,String( clargs.existing_key ) ) ;
		hide( clargs.existing_key ) ;
		clargs.existing_key = StringContent( q ) ;
	}
	if( clargs.device != NULL ){

		q = StringListAssignString( stl,String( clargs.device ) ) ;
		hide( clargs.device ) ;
		clargs.device = StringContent( q ) ;
	}
	if( clargs.type != NULL ){

		q = StringListAssignString( stl,String( clargs.type ) ) ;
		hide( clargs.type ) ;
		clargs.type = StringContent( q ) ;
	}
	if( clargs.rng != NULL ){

		q = StringListAssignString( stl,String( clargs.rng ) ) ;
		hide( clargs.rng ) ;
		clargs.rng = StringContent( q ) ;
	}
	for( i = 0 ; clargs.tcrypt_multiple_keyfiles[ i ] != NULL ; i++ ){

		q = StringListAssignString( stl,String( clargs.tcrypt_multiple_keyfiles[ i ] ) ) ;
		hide( clargs.tcrypt_multiple_keyfiles[ i ] ) ;
		clargs.tcrypt_multiple_keyfiles[ i ] = StringContent( q ) ;
	}

	for( i = 0 ; clargs.tcrypt_hidden_volume_multiple_keyfiles[ i ] != NULL ; i++ ){

		q = StringListAssignString( stl,String( clargs.tcrypt_hidden_volume_multiple_keyfiles[ i ] ) ) ;
		hide( clargs.tcrypt_hidden_volume_multiple_keyfiles[ i ] ) ;
		clargs.tcrypt_hidden_volume_multiple_keyfiles[ i ] = StringContent( q ) ;
	}

	zuluCryptSecurityLockMemory( stl ) ;

	action = clargs.action ;
	device = clargs.device ;

	if( action == 'C' ){

		return zuluExit( _clear_dead_mappers( uid ),stl,stx,env,NULL ) ;
	}else{
		_clear_dead_mappers( uid ) ;
	}

	/*
	 * below tests are here because they do not use -d option
	 *
	 * zuluCryptPrintPartitions() function is defined in volumes.c
	 * zuluCryptSecurityCheckPartitionPermissions() is defined in security.c
	 */
	switch( action ){

		case 'A':
		case 'N':
		case 'S': st = zuluCryptPrintPartitions( clargs.partition_number,clargs.print_partition_type,uid ) ;
			  return zuluExit( st,stl,stx,env,NULL ) ;
		case 'L': st = _printOpenedVolumes( uid ) ;
			  return zuluExit( st,stl,stx,env,NULL ) ;
	}

	if( action == '\0' ){

		return zuluExit( 130,stl,stx,env,gettext( "ERROR: \"action\" argument is missing" ) ) ;
	}
	if( device == NULL ){

		return zuluExit( 120,stl,stx,env,gettext( "ERROR: Required option( device path ) is missing for this operation" ) ) ;
	}
	if( action == 'U' ){

		st = _print_uuid_from_path( device ) ;
		return zuluExit( st,stl,stx,env,NULL ) ;
	}
	if( StringPrefixEqual( device,"UUID=" ) ){

		q = String( device ) ;
		StringRemoveString( q,"\"" ) ;
		StringSubChar( q,4,'-' ) ;

		mapping_name = StringContent( q ) ;

		/*
		 * zuluCryptEvaluateDeviceTags() is defined in path_access.c
		 */
		ac = zuluCryptEvaluateDeviceTags( "UUID",mapping_name + 5 ) ;

		if( ac != NULL ) {

			clargs.device = ac ;
			st = zuluCryptEXE( &clargs,mapping_name,uid ) ;
			StringFree( ac ) ;
			StringDelete( &q ) ;
			return zuluExit( st,stl,stx,env,NULL ) ;
		}else{
			StringDelete( &q ) ;
			return zuluExit( 110,stl,stx,env,gettext( "ERROR: Could not find any partition with the presented UUID" ) ) ;
		}
	}else{
		/*
		 * this function is defined in ../zuluCrypt-lib/file_path_security.c
		 */
		switch( zuluCryptGetDeviceFileProperties( device,&fd,&fd1,&dev,uid ) ){

			case 0 : break ;
			case 1 : return zuluExit( 111,stl,stx,env,gettext( "ERROR: Devices in /dev/shm/ path is not suppored" ) ) ;
			case 2 : return zuluExit( 112,stl,stx,env,gettext( "ERROR: Given path is a directory" ) ) ;
			case 3 : return zuluExit( 113,stl,stx,env,gettext( "ERROR: A file can have only one hard link" ) ) ;
			case 4 : return zuluExit( 113,stl,stx,env,gettext( "ERROR: Insufficient privilges to access the device" ) ) ;
			default: return zuluExit( 113,stl,stx,env,gettext( "ERROR: A non supported device encountered,device is missing or permission denied\n\
Possible reasons for getting the error are:\n1.Device path is invalid.\n2.The device has LVM or MDRAID signature" ) ) ;
		}

		if( dev == NULL ){

			if( fd1 != -1 ){

				close( fd1 ) ;
			}
			if( fd != -1 ){

				close( fd ) ;
			}
			return zuluExit( 114,stl,stx,env,gettext( "ERROR: Could not resolve path to device" ) ) ;
		}else{
			q = String( "/dev/mapper/zuluCrypt-" ) ;

			StringAppendInt( q,uid ) ;
			StringAppend( q,"-" ) ;

			st = StringPrefixMatch( dev,StringContent( q ),StringLength( q ) ) ;

			StringDelete( &q ) ;

			if( st && clargs.action == 'P' ){

				return zuluCryptEXEGetDevice( clargs.device ) ;
			}

			/*
			 * zuluCryptDeviceIsSupported() is defined in partitions.c
			 */
			if( zuluCryptDeviceIsSupported( dev,uid ) ){

				clargs.device = dev ;

				if( StringPrefixEqual( dev,"/dev/loop" ) ){

					/*
					 * zuluCryptLoopDeviceAddress_1() is defined in ../zuluCrypt-cli/create_loop_device.c
					 */
					ac_1 = zuluCryptLoopDeviceAddress_1( dev ) ;

					i = StringLastIndexOfChar_1( ac_1,'/' ) ;

					if( i != -1 ){

						mapping_name =  ac_1 + i + 1  ;
					}else{
						mapping_name =  dev  ;
					}

					st = zuluCryptEXE( &clargs,mapping_name,uid ) ;
					StringFree( ac_1 ) ;
				}else{
					i = StringLastIndexOfChar_1( dev,'/' ) ;

					if( i != -1 ){

						mapping_name =  dev + i + 1  ;
					}else{
						mapping_name =  dev  ;
					}

					st = zuluCryptEXE( &clargs,mapping_name,uid ) ;
				}
			}else{
				st = 113 ;
				puts( gettext( "ERROR: A non supported device encountered,device is missing or permission denied\n\
Possible reasons for getting the error are:\n1.Device path is invalid.\n2.The device has LVM or MDRAID signature" ) ) ;
			}

			StringFree( dev ) ;

			if( fd1 != -1 ){

				close( fd1 ) ;
			}
			if( fd != -1 ){

				close( fd ) ;
			}
			return zuluExit( st,stl,stx,env,NULL ) ;
		}
	}
}
