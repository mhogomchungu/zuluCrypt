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
#include <signal.h>
#include "../zuluCrypt-cli/lib/includes.h"
#include "../zuluCrypt-cli/bin/includes.h"
#include <sys/resource.h>
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
	if( seteuid( uid ) ){}
}

static void _setuid( uid_t uid )
{
	if( setuid( uid ) ){}
}

/*
 * All functions with "EXE" in their names are defined somewhere in ../zuluCrypt-cli/bin
 */

static int _mount_get_opts( int argc,char * argv[],ARGS * args )
{
	int c ;
	int k = 0 ;

	while( ( c = getopt( argc,argv,"cEMLnASNshlPmuDd:z:e:Y:p:f:G:o:F:t:B:b:K:" ) ) != -1 ){

		switch( c ){

			case 'M' : args->share   = 1      ; break ;
			case 'n' : args->mpo     = 1      ; break ;
			case 'E' : args->action  = "-E"   ; break ;
			case 'D' : args->action  = "-D"   ; break ;
			case 's' : args->action  = "-s"   ; break ;
			case 'l' : args->action  = "-l"   ; break ;
			case 'L' : args->action  = "-L"   ; break ;
			case 'P' : args->action  = "-P"   ; break ;
			case 'A' : args->action  = "-A"   ; break ;
			case 'S' : args->action  = "-S"   ; break ;
			case 'N' : args->action  = "-N"   ; break ;
			case 'm' : args->action  = "-m"   ; break ;
			case 'u' : args->action  = "-u"   ; break ;
			case 'c' : args->action  = "-c"   ; break ;
			case 'B' : args->action  = "-B"   ;
			           args->m_point = optarg ; break ;
			case 'b' : args->action  = "-b"   ;
				   args->m_point = optarg ; break ;
			case 't' : args->type    = optarg ; break ;
			case 'o' : args->offset  = optarg ; break ;
			case 'd' : args->device  = optarg ; break ;
			case 'z' : args->m_point = optarg ; break ;
			case 'e' : args->m_opts  = optarg ; break ;
			case 'K' : args->u_id    = optarg ; break ;
			case 'Y' : args->fs_opts = optarg ; break ;
			case 'p' : args->key     = optarg ;
				   args->key_source = "-p"; break ;
			case 'f' : args->key     = optarg ;
				   args->key_source = "-f"; break ;
			case 'G' : args->key     = optarg ;
				   args->key_source = "-G"; break ;
			case 'F' :
				   if( k < TRUECRYPT_MAX_KEYFILES ){

					   /*
					    * TRUECRYPT_MAX_KEYFILES is set at ../zuluCrypt-cli/bin/libzuluCrypt-exe.h
					    */

					args->tcrypt_multiple_keyfiles[ k ] = optarg ;
					k++ ;
				   }
				   break ;
			default  : return -1 ;
		}
	}

	return 0 ;
}

int _zuluExit( int st,string_t z,char * q,const char * msg )
{
	StringFree( q ) ;

	StringDelete( &z ) ;

	if( msg != NULL ){

		printf( "%s\n",msg ) ;
	}
	return st ;
}

static int _zuluExit_2( int st,stringList_t z,stringList_t q,const char * msg )
{
	zuluCryptSecurityUnlockMemory( z ) ;

	StringListMultipleDelete( &q,&z,NULL ) ;

	if( msg != NULL ){

		printf( "%s\n",msg ) ;
	}
	return st ;
}

int _zuluExit_1( int st,stringList_t z,char * q,const char * msg )
{
	StringFree( q ) ;

	zuluCryptSecurityUnlockMemory( z ) ;

	StringListDelete( &z ) ;

	if( msg != NULL ){

		printf( "%s\n",msg ) ;
	}

	return st ;
}

static int _zuluMountDeviceList( uid_t uid )
{
	/*
	 * zuluCryptPrintPartitions() is defined in ../zuluCrypt-cli/partitions.c
	 * ZULUCRYPTallPartitions is set in ../zuluCrypt-cli/constants.h
	 */
	return zuluCryptPrintPartitions( ZULUCRYPTallPartitions,0,uid ) ;
}


static int _zuluMountNonSystemDeviceList( uid_t uid )
{
	/*
	 * zuluCryptPrintPartitions() is defined in ../zuluCrypt-cli/partitions.c
	 * ZULUCRYPTallPartitions is set in ../zuluCrypt-cli/constants.h
	 */
	return zuluCryptPrintPartitions( ZULUCRYPTnonSystemPartitions,0,uid ) ;
}

static int _zuluMountSystemDeviceList( uid_t uid )
{
	/*
	 * zuluCryptPrintPartitions() is defined in ../zuluCrypt-cli/partitions.c
	 * ZULUCRYPTallPartitions is set in ../zuluCrypt-cli/constants.h
	 */
	return zuluCryptPrintPartitions( ZULUCRYPTsystemPartitions,0,uid ) ;
}

static int _zuluMountMountedList( uid_t uid )
{
	/*
	 * zuluMountPrintVolumesProperties() is defined in volume_status.c
	 */
	return zuluMountPrintVolumesProperties( uid ) ;
}

static string_t _zuluCryptGetFileSystemFromDevice( const char * device )
{
	string_t st ;

	zuluCryptSecurityGainElevatedPrivileges() ;
	st = zuluCryptGetFileSystemFromDevice( device ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;

	return st ;
}

static int _zuluPartitionHasCryptoFs( const char * device )
{
	int r ;
	/*
	 * zuluCryptSecurityGainElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityGainElevatedPrivileges() ;

	/*
	 * zuluCryptDeviceHasEncryptedFileSystem() is defined in ../zuluCrypt-cli/lib/blkid_evaluate_tag.c
	 */
	r = zuluCryptDeviceHasEncryptedFileSystem( device ) ;

	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityDropElevatedPrivileges() ;

	return r ;
}

static int _zuluMountPrintVolumeDeviceName( const char * device )
{
	char * c ;

	/*
	 * zuluCryptSecurityGainElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	* zuluCryptVolumeDeviceName() is defined in ../lib/status.c
	*/
	c = zuluCryptVolumeDeviceName( device ) ;

	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityDropElevatedPrivileges() ;

	if( c == NULL ){

		return 1 ;
	}else{
		printf( "%s\n",c ) ;
		StringFree( c ) ;
		return 0 ;
	}
}

/*
 * This is an emergency function,to be used when a device was unplugged uncleanly causing the device
 * old path to be "locked" as pluggin in the device again will give it a different path
 */
static int _checkUnmount( const char * device,uid_t uid )
{
	stringList_t stx ;
	stringList_t stl ;

	ssize_t index ;
	int r ;
	char * m_point = NULL ;
	string_t st ;
	const char * g ;

	if( StringPrefixEqual( device,"/dev/mapper/zuluCrypt-" ) ){
		/*
		 * encrypted volumes are handled someplace else
		 */
		return 0 ;
	}
	/*
	 * zuluCryptGetMoutedList() is defined in ../lib/process_mountinfo.c
	 */
	stx = zuluCryptGetMoutedList() ;

	st = String( device ) ;
	index = StringListHasStartSequence( stx,StringAppend( st," " ) ) ;
	StringDelete( &st ) ;

	if( index != -1 ){

		st = StringListStringAt( stx,(size_t)index ) ;
		stl = StringListStringSplit( st,' ' ) ;

		device = StringListContentAtFirstPlace( stl ) ;
		/*
		 * zuluCryptBindUnmountVolume() is defined in ../zuluCrypt-cli/bin/bind.c
		 */
		r = zuluCryptBindUnmountVolume( stx,device,uid ) ;

		if( !( r == 3 || r == 4 ) ){

			st = StringListStringAtSecondPlace( stl ) ;
			/*
			 * zuluCryptDecodeMountEntry() is defined in ../zuluCrypt-cli/lib/mount_volume.c
			 */
			g  = zuluCryptDecodeMountEntry( st ) ;
			/*
			 * zuluCryptMountPointPrefixMatch() is defined in ../zuluCrypt-cli/lib/create_mount_point.c
			 */
			r = zuluCryptMountPointPrefixMatch( g,uid,NULL ) ;

			if( r == 1 ){

				zuluCryptSecurityGainElevatedPrivileges() ;
				/*
				 * zuluCryptUnmountVolume() is defined in ../zuluCrypt-cli/lib/unmount_volume.c
				 */
				if( zuluCryptUnmountVolume( device,&m_point ) == 0 ){

					if( m_point != NULL ){

						rmdir( m_point ) ;
						StringFree( m_point ) ;
					}
				}
				zuluCryptSecurityDropElevatedPrivileges() ;
			}
		}

		StringListDelete( &stl ) ;
	}else{
		/*
		 * Either the volume is not mounted or is encrypted.
		 * If it is encrypted,then it is handled someplace else
		 */
		;
	}

	StringListDelete( &stx ) ;

	return 0 ;
}

static int _zuluMountExe( ARGS * args )
{
	const char * device = args->device ;
	const char * action = args->action ;
	const char * uuid   = args->uuid   ;
	const char * offset = args->offset ;
	uid_t        user_id = args->user_id ;

	string_t st ;

	int r ;

	if( StringsAreEqual( action,"-L" ) ){

		return zuluMountPrintDeviceProperties( device,uuid,user_id ) ;
	}

	if( StringsAreEqual( action,"-s" ) ){

		st = _zuluCryptGetFileSystemFromDevice( device ) ;

		zuluCryptSecurityGainElevatedPrivileges() ;

		r = zuluCryptDeviceManagedByDislocker( device,user_id ) ;

		zuluCryptSecurityDropElevatedPrivileges() ;

		if( StringContains( st,zuluCryptBitLockerType() ) && r ){

			r = zuluMountPrintBitLockerProperties( device,user_id ) ;

		}else if( offset != NULL ||
			  st == StringVoid ||
			  StringsAreEqual_2( st,"Nil" ) ||
			  StringStartsWith( st,"crypto_" ) ){

			r = zuluMountVolumeStatus( device,uuid,user_id ) ;
		}else{
			r = zuluMountUnEncryptedVolumeStatus( device,NULL,NULL ) ;
		}

		StringDelete( &st ) ;

		return r ;
	}

	if( StringsAreEqual( action,"-m" ) ){

		if( offset != NULL || _zuluPartitionHasCryptoFs( device ) ){
			/*
			 * zuluMountMount() is defined in crypto_mount.c
			 */
			return zuluMountCryptoMount( args ) ;
		}else{
			/*
			 * zuluMountMount() is defined in mount.c
			 */
			return zuluMountMount( args ) ;
		}
	}

	if( StringsAreEqual( action,"-u" ) ){

		if( offset != NULL || _zuluPartitionHasCryptoFs( device ) ){
			/*
			 * zuluMountMount() is defined in crypto_umount.c
			 */
			return zuluMountCryptoUMount( args ) ;
		}else{
			/*
			 * zuluMountMount() is defined in umount.c
			 */
			return zuluMountUMount( args ) ;
		}
	}

	return _zuluExit_1( 200,StringListVoid,NULL,gettext( "ERROR: unrecognized argument encountered" ) ) ;
}

static int _mount_help( void )
{
	const char * doc6 ;
	const char * doc5 ;
	const char * doc4 ;
	const char * doc3 ;
	const char * doc2 ;
	const char * doc1 = gettext( "\
options:\n\
-m -- mount a volume : arguments: -d volume_path -z mount_point -e mode(rw/ro)\n\
      -- additional arguments for crypto_LUKS,crypto_PLAIN,crypto_TCRYPT volumes, -p passphrase/-f keyfile\n\
-z -- mount point component to append to \"/run/media/private/$USER/\"\n\
-Y -- file system options\n\
-e -- mount options\n" ) ;

	doc2 = gettext( "\
-u -- unmount a volume: arguments: -d volume_path\n\
-s -- print properties of a volume: arguments: -d partition_path\n\
-M -- this option will create a mount point in \"/run/media/private/$USER\" and a publicly accessible \"mirror\" in \"/run/media/public/\'\n" ) ;

	doc3 = gettext( "\
-l -- print expanded list of all volumes\n\
-L -- must be used with -d,print properties of a volume specified by d option\n\
-P -- print a list of all volumes\n\
-D -- get a device node address from its mapper path( mapper paths are usually located in /dev/mapper ). Required argument: -d\n" ) ;

	doc4 = gettext( "\
-A -- print a list of all volumes\n\
-S -- print a list of system volumes\n\
-N -- print a list of non system volumes\n\
-E -- print a list of mounted volumes\n\
-t -- to unlock a volume as VeraCrypt volume,use \"-t vera\"\n" ) ;

	doc5= gettext( "\
-o -- offset in sectors on where the volume starts in the volume.The volume is assumed to be plain type with this option \
and the option must be given when -u or -s arguments are used with a volume opened with this option\n\
-F -- path to truecrypt multiple keyfiles.Keyfiles are separated by \":\" character\n\n" ) ;

	doc6= gettext( "\
examples:\n\
mount a volume  : zuluMount-cli -m -d /dev/sdc1\n\
unmount a volume: zuluMount-cli -u -d /dev/sdc1\n\
mount and encrypted volume with a key \"xyz\" : zuluMount-cli -m -d /dev/sdc2 -p xyz\n" ) ;

      printf( "%s%s%s%s%s%s",doc1,doc2,doc3,doc4,doc5,doc6 ) ;

	return 201 ;
}

static void ExitOnMemoryExaustion( void )
{
	printf( gettext( "Unexpected exiting because you have run out of memory\n" ) ) ;
	exit( 1 ) ;
}

static int _zuluMountDoAction( ARGS * args )
{
	int fd = -1 ;
	int fd1 = -1 ;
	int status ;
	char * dev = NULL ;
	const char * msg = gettext( "ERROR: A non supported device encountered,device is missing or permission denied\n\
Possible reasons for getting the error are:\n1.Device path is invalid.\n2.The device has LVM or MDRAID signature\n"  ) ;
	/*
	 * zuluCryptGetDeviceFileProperties is defined in ../zuluCrypt-lib/file_path_security.c
	 */
	switch( zuluCryptGetDeviceFileProperties( args->device,&fd,&fd1,&dev,args->uid ) ){

		case 0 : args->device = dev ;
			 /*
			  * zuluCryptDeviceIsSupported() is defined in ../zuluCrypt-cli/bin/volumes.c
			  */
			 if( zuluCryptDeviceIsSupported( dev,args->uid ) ){

				status = _zuluMountExe( args ) ;
			 }else{
				printf( "%s",msg ) ;
				status = 224 ;
			 }
			 if( dev != NULL ){

				 StringFree( dev ) ;
			 }
			 if( fd1 != -1 ){

				 close( fd ) ;
			 }
			 if( fd != -1 ){

				 close( fd ) ;
			 }
			 return status ;
		case 1 : printf( gettext( "ERROR: Devices in /dev/shm path is not suppored\n" ) ) ;	return 220 ;
		case 2 : printf( gettext( "ERROR: Given path is a directory\n" ) ) ;  			return 221 ;
		case 3 : printf( gettext( "ERROR: A file can have only one hard link\n" ) ) ;		return 222 ;
		case 4 : printf( gettext( "ERROR: Insufficient privilges to access the device\n" ) ) ; 	return 223 ;
		default: printf( "%s",msg ) ; 								return 224 ;
	}
}

static void _privilegeEvelationError( const char * msg )
{
	puts( msg ) ;
	exit( 255 ) ;
}

static void _forceTerminateOnSeriousError( int sig )
{
	if( sig ){;}
	puts( "SIGSEGV caught,exiting" ) ;
	exit( 255 ) ;
}

static int _create_mount_point( const char * label,const char * m_opts,uid_t uid )
{
	/*
	 * zuluCryptCreateMountPoint() is defined in ../zuluCrypt-cli/bin/create_mount_point.c
	 */
	string_t st = zuluCryptCreateMountPoint( NULL,label,m_opts,uid ) ;

	int r ;

	if( st != StringVoid ){

		r = 0 ;

		StringDelete( &st ) ;
	}else{
		r = 1 ;
	}

	return r ;
}

static int _delete_mount_point( const char * m_path,uid_t uid )
{
	string_t st ;

	int r = 0 ;

	/*
	 * zuluCryptReuseMountPoint() is defined in ../zuluCrypt-cli/bin/create_mount_point.c
	 */

	if( !zuluCryptReuseMountPoint() ){

		/*
		 * zuluCryptGetUserName() is defined in ../zuluCrypt-cli/lib/user_home_path.c
		 */
		st = zuluCryptGetUserName( uid ) ;

		if( StringPrefixEqual( m_path,StringPrepend( st,"/run/media/private/" ) ) ){

			zuluCryptSecurityGainElevatedPrivileges() ;

			r = rmdir( m_path ) ;

			zuluCryptSecurityDropElevatedPrivileges() ;

		}else{
			zuluCryptSecurityDropElevatedPrivileges() ;

			r = rmdir( m_path ) ;
		}

		StringDelete( &st ) ;
	}

	return r ;
}

int main( int argc,char * argv[] )
{
	char * action ;
	char * device ;
	string_t * k ;
	stringList_t stl ;
	stringList_t stx ;
	int status ;

	int i ;

	uid_t uid = getuid() ;
	gid_t gid = getgid() ;

	uid_t user_id ;

	/*
	 * ARGS structure is declared in ./includes.h
	 */
	ARGS args ;

	struct sigaction sa ;
	memset( &sa,'\0',sizeof( struct sigaction ) ) ;
	sa.sa_handler = _forceTerminateOnSeriousError ;
	sigaction( SIGSEGV,&sa,NULL ) ;

	setlocale( LC_ALL,"" ) ;
	bindtextdomain( "zuluMount-cli",TRANSLATION_PATH ) ;
	textdomain( "zuluMount-cli" );

	zuluCryptExeSetOriginalUID( uid ) ;

	if( argc < 2 ){

		return _mount_help() ;
	}
	if( argc == 2 ){

		action = argv[ 1 ] ;

		if( StringAtLeastOneMatch_1( action,"-h","--help","-help",NULL ) ){

			return _mount_help() ;
		}
		if( StringAtLeastOneMatch_1( action,"-v","-version","--version",NULL ) ){

			printf( "%s\n",zuluCryptVersion() );
			return 0 ;
		}
		if( StringsAreEqual( action,"--clear-dead-mount-points" ) ){

			zuluCryptClearDeadMappers( uid,1 ) ;
			return 0 ;
		}
	}

	memset( &args,'\0',sizeof( args ) ) ;

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
	if( _mount_get_opts( argc,argv,&args ) != 0 ){

		return _mount_help() ;
	}

	if( zuluCryptSecurityConvertUID( uid,args.u_id,&user_id ) ){

		puts( gettext( "ERROR: user is not root privileged" ) ) ;
		return 255 ;
	}

	args.uid     = uid ;
	args.user_id = user_id ;


	/*
	 * Run with higher priority to speed things up
	 */
	setpriority( PRIO_PROCESS,0,-15 ) ;
	_setuid( 0 ) ;
	_seteuid( uid ) ;

	/*
	 * zuluCryptDisableMetadataLocking() is defined in ..zuluCrypt-cli/lib/create_luks.c
	 */
	zuluCryptDisableMetadataLocking() ;

	/*
	 * zuluCryptClearDeadMappers() is defined in ../zuluCrypt-cli/bin/clear_dead_mapper.c
	 */
	zuluCryptClearDeadMappers( user_id,0 ) ;

	/*
	 * zuluCryptSecuritySetPrivilegeElevationErrorFunction() is defined in ../zuluCrypt-cli/bin/security.c
	 * _privilegeEvelationError() function will be called when functions that elevate or drop privileges fail
	 */
	zuluCryptSecuritySetPrivilegeElevationErrorFunction( _privilegeEvelationError ) ;

	/*
	 * zuluCryptSetUserUIDForPrivilegeManagement() is defined in ../zuluCrypt-bin/security.c
	 */
	zuluCryptSetUserUIDForPrivilegeManagement( user_id ) ;
	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityDropElevatedPrivileges() ;

	StringExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	StringListExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	SocketExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	ProcessExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;

	stl = StringListInit() ;
	/*
	 * zuluCryptSecuritySanitizeTheEnvironment() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecuritySanitizeTheEnvironment( user_id,&stx ) ;

	#define _hide( z ) strncpy( ( char * )z,"x",StringLength( *k ) )

	if( args.device != NULL ){

		k = StringListAssign( stl ) ;
		*k = String( args.device ) ;
		_hide( args.device ) ;
		args.device = StringContent( *k ) ;
	}

	if( args.key != NULL ){

		k = StringListAssign( stl ) ;
		*k = String( args.key ) ;
		_hide( args.key ) ;
		args.key = StringContent( *k ) ;
	}

	if( args.type != NULL ){

		k = StringListAssign( stl ) ;
		*k = String( args.type ) ;
		_hide( args.type ) ;
		args.type = StringContent( *k ) ;
	}

	for( i = 0 ; args.tcrypt_multiple_keyfiles[ i ] != NULL ; i++ ){

		k = StringListAssign( stl ) ;
		*k = String( args.tcrypt_multiple_keyfiles[ i ] ) ;
		_hide( args.tcrypt_multiple_keyfiles[ i ] ) ;
		args.tcrypt_multiple_keyfiles[ i ] = StringContent( *k ) ;
	}

	zuluCryptSecurityLockMemory( stl ) ;

	if( args.action == NULL ){

		return _zuluExit_2( 212,stl,stx,gettext( "ERROR: Action not specified" ) ) ;
	}
	if( StringsAreEqual( args.action,"-E" ) ){

		return _zuluExit_2(  zuluMountprintAListOfMountedVolumes( user_id ),stl,stx,NULL ) ;
	}
	if( StringsAreEqual( args.action,"-c" ) ){

		return _zuluExit_2( _checkUnmount( args.device,user_id ),stl,stx,NULL ) ;
	}
	if( StringsAreEqual( args.action,"-l" ) ){

		return _zuluExit_2( _zuluMountMountedList( user_id ),stl,stx,NULL ) ;
	}
	if( StringsAreEqual( args.action,"-P" ) || StringsAreEqual( args.action,"-A" ) ){

		return _zuluExit_2( _zuluMountDeviceList( uid ),stl,stx,NULL ) ;
	}
	if( StringsAreEqual( args.action,"-S" ) ){

		return _zuluExit_2( _zuluMountSystemDeviceList( user_id ),stl,stx,NULL ) ;
	}
	if( StringsAreEqual( args.action,"-N" ) ){
		return _zuluExit_2( _zuluMountNonSystemDeviceList( user_id ),stl,stx,NULL ) ;
	}
	if( StringsAreEqual( args.action,"-B" ) ){

		return _zuluExit_2( _create_mount_point( args.m_point,args.m_opts,args.user_id ),stl,stx,NULL ) ;
	}
	if( StringsAreEqual( args.action,"-b" ) ){

		return _zuluExit_2( _delete_mount_point( args.m_point,args.user_id ),stl,stx,NULL ) ;
	}
	if( StringsAreEqual( args.action,"-h" ) ){

		return _zuluExit_2( _mount_help(),stl,stx,NULL ) ;
	}
	if( args.device == NULL ){

		return _zuluExit_2( 213,stl,stx,gettext( "ERROR: device argument missing" ) ) ;
	}
	if( StringsAreEqual( args.action,"-D" ) ){

		return _zuluMountPrintVolumeDeviceName( args.device ) ;
	}
	if( args.m_opts == NULL ){

		args.m_opts = "rw" ;
	}

	args.env = stx ;

	/*
	 * zuluCryptEvaluateDeviceTags() is defined in ../zuluCrypt-cli/bin/path_access.c
	 */
	if( StringPrefixEqual( args.device,"UUID=" ) ){

		device = zuluCryptEvaluateDeviceTags( "UUID",args.device + 5 ) ;

		if( device != NULL ){

			args.uuid = args.device ;
			args.device = device ;
			status = _zuluMountDoAction( &args ) ;
			StringFree( device ) ;
		}else{
			printf( gettext( "Could not resolve UUID\n" ) ) ;
			status = 214 ;
		}
	}else if( StringPrefixEqual( args.device,"LABEL=" ) ){

		device = zuluCryptEvaluateDeviceTags( "LABEL",args.device + 6 ) ;

		if( device != NULL ){

			args.device = device ;
			status = _zuluMountDoAction( &args ) ;
			StringFree( device ) ;
		}else{
			printf( gettext( "Could not resolve LABEL\n" ) ) ;
			status = 215 ;
		}
	}else{
		status = _zuluMountDoAction( &args ) ;
	}

	return _zuluExit_2( status,stl,stx,NULL ) ;
}
