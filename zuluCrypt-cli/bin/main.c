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
	char * c = zuluCryptVolumeDeviceName( device ) ;
	if( c == NULL ){
		printf( "ERROR: could not get device address from mapper address\n" ) ;
		return 1 ;
	}else{
		printf( "%s\n",c ) ;
		free( c ) ;
		return 0 ;
	}
}

static int zuluCryptEXECheckIfLuks( const char * device )
{
	/*
	 * this zuluCryptVolumeIsLuks() is defined in ../lib/is_luks.c
	 */
	int status = zuluCryptVolumeIsLuks( device ) ;
	
	if( status )
		printf( "\"%s\" is a luks device\n",device ) ;
	else
		printf( "\"%s\" is not a luks device\n",device ) ;
	
	return status ;
}

static int zuluCryptEXECheckIfTcrypt( struct_opts * clargs,uid_t uid )
{
	string_t st_key     = StringVoid ;
	const char * device = clargs->device ;
	const char * key    = clargs->key ;
	const char * source = clargs->key_source ;
	size_t key_len ;
	
	if( key == NULL ){
		printf( "ERROR: key argument is missing" ) ;
		return 1 ;
	}
	
	if( source == NULL ){
		printf( "ERROR: key source argument is missing" ) ;
		return 1 ;
	}
	
	if( strcmp( source,"-p" ) == 0 ){
		/*
		 * zuluCryptGetVolumeType() is defined in ../lib/volume_type.c
		 */
		if( zuluCryptGetVolumeType( device,key,strlen( key ) ) == 2 ){
			printf( "\"%s\" is a tcrypt device\n",device ) ;
			return 0 ;
		}
	}else if( strcmp( source,"-f" ) == 0 ){
		/*
		 * zuluCryptSecurityGetPassFromFile() is defined in security.c
		 */
		if( zuluCryptSecurityGetPassFromFile( key,uid,&st_key ) == 0 ){
			key = StringContent( st_key ) ;
			key_len = StringLength( st_key ) ;
			if( zuluCryptGetVolumeType( device,key,key_len ) == 2 ){
				printf( "\"%s\" is a tcrypt device\n",device ) ;
				StringDelete( &st_key ) ;
				return 0 ;
			}else{
				printf( "\"%s\" is a not tcrypt device\n",device ) ;
				StringDelete( &st_key ) ;
				return 1 ;
			}
		}else{
			printf( "\"%s\" is not a tcrypt device\n",device ) ;
			return 1 ;
		}
	}else{
		/*
		* shouldnt get here
		*/
		printf( "\"%s\" is not a tcrypt device\n",device ) ;
		return 1 ;
	}
	
	/*
	 * shouldnt get here
	 */
	printf( "\"%s\" is not a tcrypt device\n",device ) ;
	return 1 ;
}

static int zuluCryptEXECheckEmptySlots( const char * device )
{
	int status ;
	char * c  ;
	/*
	 * this zuluCryptPathIsNotValid() is defined in ../lib/is_path_valid.c
	 */
	if( zuluCryptPathIsNotValid( device ) ){
		printf( "path \"%s\" does not point to a device\n",device ) ;
		status = 1 ;
	}else{
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

static int zuluExit( int st,stringList_t stl,const char * msg )
{
	StringListClearDelete( &stl ) ;
	
	if( msg != NULL )
		printf( "%s\n",msg ) ;
	return st ;
}

static void ExitOnMemoryExaustion( void )
{
	printf( "unexpected exiting because you have run out of memory\n" ) ;
	exit( 1 ) ;
}

int main( int argc,char * argv[] )
{
	const char * device ;
	const char * mapping_name ;
	char * ac ;
	char action ;
	int st ;
	uid_t uid ;
	string_t q = StringVoid ;
	stringList_t stl = StringListVoid ;
	struct_opts clargs ;
	
	memset( &clargs,'\0',sizeof( struct_opts ) ) ;
	
	uid = getuid();
	
	StringExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	StringListExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	ProcessExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	SocketExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	
	if( setuid( 0 ) != 0 ){
		printf( "ERROR: could not setuid(0),check the executable permission\n" ) ;
		return 255 ;
	}
	
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
	
	zuluCryptEXEGetOpts( argc,argv,&clargs );
	
	stl = StringListInit() ;
	
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
	 */
	switch( action ){
		case 'A':
		case 'N':
		case 'S': st = zuluCryptPrintPartitions( clargs.partition_number,clargs.print_partition_type ) ; 
			  return zuluExit( st,stl,NULL ) ;
		case 'L': st = zuluCryptPrintOpenedVolumes( uid ) ; 
			  return zuluExit( st,stl,NULL ) ;
	}
	
	if( action == '\0' )
		return zuluExit( 130,stl,"ERROR: \"action\" argument is missing\n" ) ;
	
	if( device == NULL )
		return zuluExit( 120,stl,"ERROR: required option( device path ) is missing for this operation\n" ) ;
	
	if( strncmp( device,"UUID=",5 ) == 0 ){

		q = String( device ) ;
		StringRemoveString( q,"\"" ) ;
		StringSubChar( q,4,'-' ) ;
		
		mapping_name = StringContent( q ) ;
		
		/*
		 * zuluCryptDeviceFromUUID() is defined in ./partitions.c
		 */
		ac = zuluCryptDeviceFromUUID( mapping_name + 5 ) ;
		
		if( ac != NULL ) {
			clargs.device = ac ;
			st = zuluCryptEXE( &clargs,mapping_name,uid );
			free( ac ) ;
			StringDelete( &q ) ;
			return zuluExit( st,stl,NULL ) ;
		}else{
			StringDelete( &q ) ;
			return zuluExit( 110,stl,"ERROR: could not find any partition with the presented UUID\n") ;
		}
	}else{
		if ( ( ac = strrchr( device,'/' ) ) != NULL ) {
			mapping_name =  ac + 1  ;
		}else{
			mapping_name =  device  ;
		}
	}

	st = zuluCryptEXE( &clargs,mapping_name,uid ) ;
	
	return zuluExit( st,stl,NULL ) ;
} 
