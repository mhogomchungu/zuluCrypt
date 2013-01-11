/*
 * 
 *  Copyright ( c ) 2011
 *  name : mhogo mchungu 
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

static int zuluCryptEXEGetDevice( const char * device )
{
	/*
	 * zuluCryptVolumeDeviceName() is defined in ../lib/status.c
	 */
	char * c = NULL ;
	int st = 1 ;
	if( zuluCryptSecurityGainElevatedPrivileges() ){
		/*
		 * zuluCryptVolumeDeviceName() is defined in ../lib/status.c
		 */
		c = zuluCryptVolumeDeviceName( device ) ;
		if( c == NULL ){
			printf( "ERROR: could not get device address from mapper address\n" ) ;
			st = 1 ;
		}else{
			printf( "%s\n",c ) ;
			free( c ) ;
			st = 0 ;
		}
		zuluCryptSecurityDropElevatedPrivileges() ;
	}
	return st ;
}

static int zuluCryptEXECheckIfLuks( const char * device )
{
	int status ;
	if( !zuluCryptSecurityGainElevatedPrivileges() ){
		printf( "ERROR: unable to gain elevated privilges,check binary permissions\n" ) ;
		return 2 ;
	}
	
	/*
	 * this zuluCryptVolumeIsLuks() is defined in ../lib/is_luks.c
	 */
	status = zuluCryptVolumeIsLuks( device ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	if( status )
		printf( "device is a luks volume\n" ) ;
	else
		printf( "device is not a luks volume\n" ) ;
	
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
		printf( "ERROR: key argument is missing\n" ) ;
		return 1 ;
	}
	
	if( source == NULL ){
		printf( "ERROR: key source argument is missing\n" ) ;
		return 1 ;
	}
	
	if( !zuluCryptSecurityGainElevatedPrivileges() ){
		printf( "ERROR: unable to gain elevated privilges,check binary permissions\n" ) ;
		return 1 ;
	}
	
	if( strcmp( source,"-p" ) == 0 ){
		zuluCryptSecurityGainElevatedPrivileges() ;
		/*
		 * zuluCryptGetVolumeType() is defined in ../lib/volume_type.c
		 */
		if( zuluCryptGetVolumeType( device,key,strlen( key ) ) == 2 ){
			printf( "\"%s\" is a tcrypt device\n",device ) ;
			st = 0 ;
		}
		zuluCryptSecurityDropElevatedPrivileges() ;
	}else if( strcmp( source,"-f" ) == 0 ){
		/*
		 * zuluCryptSecurityGetPassFromFile() is defined in security.c
		 */
		if( zuluCryptSecurityGetPassFromFile( key,uid,&st_key ) == 0 ){
			key = StringContent( st_key ) ;
			key_len = StringLength( st_key ) ;
			zuluCryptSecurityGainElevatedPrivileges() ;
			if( zuluCryptGetVolumeType( device,key,key_len ) == 2 ){
				printf( "\"%s\" is a tcrypt device\n",device ) ;
				StringDelete( &st_key ) ;
				st =  0 ;
			}else{
				printf( "\"%s\" is a not tcrypt device\n",device ) ;
				StringDelete( &st_key ) ;
				st = 1 ;
			}
			zuluCryptSecurityDropElevatedPrivileges() ;
		}else{
			printf( "\"%s\" is not a tcrypt device\n",device ) ;
			st = 1 ;
		}
	}else{
		/*
		* shouldnt get here
		*/
		printf( "\"%s\" is not a tcrypt device\n",device ) ;
		st = 1 ;
	}
	
	return st  ;
}

static int zuluCryptEXECheckEmptySlots( const char * device )
{
	int status ;
	char * c  ;
	/*
	 * this zuluCryptSecurityPathIsValid() is defined in ../security.c
	 */
	if( !zuluCryptSecurityPathIsValid( device,-1 ) ){
		printf( "path \"%s\" does not point to a device\n",device ) ;
		status = 1 ;
	}else{
		if( !zuluCryptSecurityGainElevatedPrivileges() ){
			printf( "ERROR: unable to gain elevated privilges,check binary permissions\n" ) ;
			return 3 ;
		}
		/*
		 * zuluCryptEmptySlots() is defined in ../lib/empty_slots.c
		 */
		c = zuluCryptEmptySlots( device ) ;
		if( c == NULL ){
			printf( "device \"%s\" is not a luks device\n",device ) ;
			status = 2 ;
		}else{
			printf( "%s\n",c ) ;
			status = 0 ;
			free( c ) ;
		}
		zuluCryptSecurityDropElevatedPrivileges() ;
	}
	return status ;
}

static int zuluCryptEXECheckUUID( const char * device )
{
	printf( "%s\n",device ) ;
	return 0 ;
}

static int zuluCryptEXE( struct_opts * clargs,const char * mapping_name,uid_t uid )
{	
	switch( clargs->action ){
		case 'W' : return zuluCryptEXECheckIfTcrypt( clargs,uid ) ;
		case 'B' : return zuluCryptEXESaveAndRestoreLuksHeader( clargs,uid,LUKS_HEADER_SAVE ) ;
		case 'R' : return zuluCryptEXESaveAndRestoreLuksHeader( clargs,uid,LUKS_HEADER_RESTORE ) ;
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
	printf("ERROR!!!!!!!!!!: cli option missed!\n" );
	return 200 ; /* shouldnt get here */	
}

static int zuluExit( int st,stringList_t stl,stringList_t stx,char * const * env,const char * msg )
{
	StringListClearDelete( &stl ) ;
	StringListDelete( &stx ) ;
	
	if( env != NULL )
		free( ( char * )env ) ;
	if( msg != NULL )
		printf( "%s\n",msg ) ;
	return st ;
}

static void ExitOnMemoryExaustion( void )
{
	printf( "unexpected exiting because you have run out of memory\n" ) ;
	exit( 1 ) ;
}

static int _print_uuid_from_path( const char * device )
{
	/*
	 * zuluCryptSecurityUUIDFromPath() is defined in ./security.c
	 */
	char * e = zuluCryptSecurityUUIDFromPath( device ) ;
	if( e == NULL ){
		puts( "UUID=\"Nil\"" ) ;
		return 1 ;
	}else{
		printf( "UUID=\"%s\"\n",e ) ;
		free( e ) ;
		return 0 ;
	}
}

static void _privilegeEvelationError( const char * msg )
{
	perror( msg ) ;
	exit( 255 ) ;
}

static int _clear_dead_mappers( uid_t uid )
{
	zuluCryptClearDeadMappers( uid ) ;
	return 0 ;
}

int main( int argc,char * argv[] )
{
	int fd1 = -1 ;
	int fd = -1;
	const char * device ;
	const char * mapping_name ;
	char * ac ;
	char * dev ;
	char action ;
	int st ;
	char * const * env ;
	
	string_t q = StringVoid ;
	
	stringList_t stl ;
	stringList_t stx ;
	
	struct_opts clargs ;
	
	uid_t uid = getuid() ;
	gid_t gid = getgid() ;
	/*
	 * setgroups() requires seteuid(0) ;
	 */
	
	seteuid( 0 ) ;
	if( setgroups( 1,&gid ) != 0 ){
		_privilegeEvelationError( "ERROR: setgroups() failed" ) ;
	}
	if( setegid( uid ) != 0 ){
		_privilegeEvelationError( "ERROR: setegid() failed" ) ;
	}
	seteuid( uid ) ;
	
	/*
	 * zuluCryptSecuritySetPrivilegeElevationErrorFunction() is defined in ./security.c
	 * _privilegeEvelationError() function will be called when functions that elevate or drop privilges fail
	 */
	zuluCryptSecuritySetPrivilegeElevationErrorFunction( _privilegeEvelationError ) ;
	
	/*
	 * zuluCryptGetUserUIDForPrivilegeManagement() is defined in ./security.c
	 */
	zuluCryptGetUserUIDForPrivilegeManagement( uid ) ;
	
	memset( &clargs,'\0',sizeof( struct_opts ) ) ;
	
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
		if ( strcmp( ac,"-h" ) == 0 || strcmp( ac,"--help" ) == 0 || strcmp( ac,"-help" ) == 0 ){
			zuluCryptEXEHelp();	
			return 0 ;
		}
		if ( strcmp( ac,"-v" ) == 0 || strcmp( ac,"-version" ) == 0 || strcmp( ac,"--version" ) == 0 ){
			printf( "%s\n",zuluCryptVersion() );
			return 0 ;
		}
	}
	
	/*
	 * zuluCryptEXEGetOpts() is defined in ./get_opts.c
	 */
	zuluCryptEXEGetOpts( argc,argv,&clargs );
	
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
			StringMultipleAppend( q,*argv," ",'\0' ) ;
			argv++ ;
		}
		StringSubChar( q,StringLength( q ) - 1,'\0' ) ;
	}
	
	clargs.argv = StringListContentAt( stl,0 );
	
	/*
	 * Hide "sensitive" command line arguments from ps comamnd and related tools.
	 * Best way to pass keys to zuluCrypt-cli is to use libzuluCryptPluginManager API
	 */
	if( clargs.key != NULL ){
		q = StringListAssignString( stl,String( clargs.key ) ) ;
		strncpy( ( char * )clargs.key,"x",StringLength( q ) ) ;
		clargs.key = StringContent( q ) ;
	}
	
	if( clargs.new_key != NULL ){
		q = StringListAssignString( stl,String( clargs.new_key ) ) ;
		strncpy( ( char * )clargs.new_key,"x",StringLength( q ) ) ;
		clargs.new_key = StringContent( q ) ;
	}
	
	if( clargs.existing_key != NULL ){
		q = StringListAssignString( stl,String( clargs.existing_key ) ) ;
		strncpy( ( char * )clargs.existing_key,"x",StringLength( q ) ) ;
		clargs.existing_key = StringContent( q ) ;
	}		
	
	if( clargs.device != NULL ){
		q = StringListAssignString( stl,String( clargs.device ) ) ;
		strncpy( ( char * )clargs.device,"x",StringLength( q ) ) ;
		clargs.device = StringContent( q ) ;
	}	
	
	action = clargs.action ;
	device = clargs.device ;
	
	/*
	 * below tests are here because they do not use -d option
	 * 
	 * zuluCryptPrintPartitions() function is defined in partitions.c 
	 * zuluCryptSecurityCheckPartitionPermissions() is defined in security.c
	 * zuluCryptPrintOpenedVolumes() is defined in ../lib/print_open_volumes.c
	 */
	switch( action ){
		case 'C': return zuluExit( _clear_dead_mappers( uid ),stl,stx,env,NULL ) ;
		case 'A':
		case 'N':
		case 'S': st = zuluCryptPrintPartitions( clargs.partition_number,clargs.print_partition_type ) ; 
			  return zuluExit( st,stl,stx,env,NULL ) ;
		case 'L': zuluCryptSecurityGainElevatedPrivileges() ;
			  st = zuluCryptPrintOpenedVolumes( uid ) ; 
			  zuluCryptSecurityDropElevatedPrivileges() ;
			  return zuluExit( st,stl,stx,env,NULL ) ;
	}
	
	/*
	 * zuluCryptClearDeadMappers() is defined in clear_dead_mapper.c
	 */
	zuluCryptClearDeadMappers( uid ) ;
	
	if( action == '\0' )
		return zuluExit( 130,stl,stx,env,"ERROR: \"action\" argument is missing" ) ;
	
	if( device == NULL )
		return zuluExit( 120,stl,stx,env,"ERROR: required option( device path ) is missing for this operation" ) ;
	
	if( action == 'U' ){
		st = _print_uuid_from_path( device ) ;
		return zuluExit( st,stl,stx,env,NULL ) ;
	}
	
	if( strncmp( device,"UUID=",5 ) == 0 ){

		q = String( device ) ;
		StringRemoveString( q,"\"" ) ;
		StringSubChar( q,4,'-' ) ;
		
		mapping_name = StringContent( q ) ;
		
		/*
		 * zuluCryptSecurityEvaluateDeviceTags() is defined in ./security.c
		 */
		ac = zuluCryptSecurityEvaluateDeviceTags( "UUID",mapping_name + 5 ) ;
		
		if( ac != NULL ) {
			clargs.device = ac ;
			st = zuluCryptEXE( &clargs,mapping_name,uid );
			free( ac ) ;
			StringDelete( &q ) ;
			return zuluExit( st,stl,stx,env,NULL ) ;
		}else{
			StringDelete( &q ) ;
			return zuluExit( 110,stl,stx,env,"ERROR: could not find any partition with the presented UUID") ;
		}
	}else{
		/*
		 * this function is defined in ../zuluCrypt-lib/file_path_security.c
		 */
		switch( zuluCryptGetDeviceFileProperties( device,&fd,&fd1,&dev,uid ) ){
			case 0 : break ;
			case 1 : return zuluExit( 111,stl,stx,env,"ERROR: devices in /dev/ with user access permissions are not suppored" ) ;
			case 2 : return zuluExit( 112,stl,stx,env,"ERROR: given path is a directory" ) ;   
			case 3 : return zuluExit( 113,stl,stx,env,"ERROR: a file can have only one hard link" ) ;
			case 4 : return zuluExit( 113,stl,stx,env,"ERROR: insufficient privilges to access the device" ) ;
			default: return zuluExit( 113,stl,stx,env,"ERROR: a non supported device encountered,device is missing or permission denied" ) ;
		}
		
		if( dev == NULL ){
			if( fd1 != -1 )
				close( fd1 ) ;
			if( fd != -1 ){
				close( fd ) ;
			}
			return zuluExit( 114,stl,stx,env,"ERROR: could not resolve path to device" ) ; 
		}
		
		clargs.device = dev ;
		
		if( ( ac = strrchr( device,'/' ) ) != NULL ){
			mapping_name =  ac + 1  ;
		}else{
			mapping_name =  device  ;
		}
	}
	
	st = zuluCryptEXE( &clargs,mapping_name,uid ) ;
	
	free( dev ) ;
	if( fd1 != -1 )
		close( fd1 ) ;
	if( fd != -1 ){
		close( fd ) ;
	}
	return zuluExit( st,stl,stx,env,NULL ) ;
} 
