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

/*
 * below 4 functions are defined in ./print_mounted_volumes.c
 */
int zuluMountPrintMountedVolumes( uid_t uid ) ;
void zuluMountPrintDeviceProperties( const char * entry ) ;
void zuluMountPrintDeviceProperties_1( string_t,uid_t ) ;

/*
 * All functions with "EXE" in their names are defined somewhere in ../zuluCrypt-cli/bin 
 */

static int _mount_get_opts( int argc,char * argv[],const char ** action,const char ** device,
			   const char ** m_point, const char ** fs_mode,const char ** key,
			    const char ** key_source,int * mpo,const char ** fs_opts )
{
	int c ;
	while ( ( c = getopt( argc,argv,"LntSshlPmuDd:z:e:y:p:f:G:" ) ) != -1 ) {
		switch( c ){
			case 'n' : *mpo     = 1      ; break ;
			case 'D' : *action  = "-D"   ; break ;
			case 't' : *action  = "-t"   ; break ;
			case 's' : *action  = "-s"   ; break ;
			case 'S' : *action  = "-S"   ; break ;
			case 'l' : *action  = "-l"   ; break ;
			case 'L' : *action  = "-L"   ; break ;
			case 'P' : *action  = "-P"   ; break ;
			case 'm' : *action  = "-m"   ; break ;
			case 'u' : *action  = "-u"   ; break ;
			case 'd' : *device  = optarg ; break ;
			case 'z' : *m_point = optarg ; break ;
			case 'e' : *fs_mode = optarg ; break ;
			case 'y' : *fs_opts = optarg ; break ;
			case 'p' : *key     = optarg ; 
				   *key_source = "-p"; break ;
			case 'f' : *key     = optarg ;
				   *key_source = "-f"; break ;
			case 'G' : *key     = optarg ;
				   *key_source = "-G"; break ;
			default  : return -1 ;
		}
	}
	
	return 0 ;
}

int _zuluExit( int st,string_t z,char * q,const char * msg )
{
	if( q != NULL )
		free( q ) ;
	
	StringDelete( &z ) ;
	
	if( msg != NULL )
		printf( "%s\n",msg ) ;
	
	return st ;
}

static int _zuluExit_2( int st,stringList_t z,stringList_t q,const char * msg )
{
	StringListDelete( &q ) ;
	StringListDelete( &z ) ;
	
	if( msg != NULL )
		printf( "%s\n",msg ) ;
	
	return st ;
}

int _zuluExit_1( int st,stringList_t z,char * q,const char * msg )
{
	if( q != NULL ){
		free( q ) ;
	}
	
	StringListDelete( &z ) ;
	
	if( msg != NULL )
		printf( "%s\n",msg ) ;
	
	return st ;
}

static int _zuluMountDeviceList( void )
{
	/*
	 * zuluCryptPrintPartitions() is defined in ../zuluCrypt-cli/partitions.c
	 * 
	 * it printf() contents of "/proc/partitions" 
	 */
	return zuluCryptPrintPartitions( ALL_PARTITIONS,0 ) ;
}

static int _zuluMountMountedList( uid_t uid )
{
	/*
	 * zuluMountPrintMountedVolumes() is defined in ./print_mounted_volumes.c
	 */
	return zuluMountPrintMountedVolumes( uid ) ;
}

int zuluMountVolumeStatus( const char * device,const char * UUID,uid_t uid )
{
	char * dev = NULL ;
	int st ;
	string_t p ;
	const char * e ;
	
	if( UUID == NULL ){
		if( strncmp( device,"/dev/loop",9 ) == 0 ){
			/*
			* zuluCryptLoopDeviceAddress() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
			*/
			dev = zuluCryptLoopDeviceAddress( device ) ;
			if( dev != NULL ){
				st = zuluCryptEXEVolumeInfo( strrchr( dev,'/' ) + 1,dev,uid ) ;
				free( dev ) ;		
			}else{
				printf( "ERROR: could not get volume info,is the volume opened?\n" ) ;
				st = 1 ;
			}
		}else{
			st = zuluCryptEXEVolumeInfo( strrchr( device,'/' ) + 1,device,uid ) ;
		}
	}else{
		p = String( UUID ) ;
		StringRemoveString( p,"\"" ) ;
		e = StringSubChar( p,4,'-' ) ;
		if( strncmp( device,"/dev/loop",9 ) == 0 ){
			/*
			 * zuluCryptLoopDeviceAddress() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
			 */
			dev = zuluCryptLoopDeviceAddress( device ) ;
			if( dev != NULL ){
				st = zuluCryptEXEVolumeInfo( e,dev,uid ) ;
				free( dev ) ;		
			}else{
				printf( "ERROR: could not get volume info,is the volume opened?\n" ) ;
				st = 1 ;
			}
		}else{
			st = zuluCryptEXEVolumeInfo( e,device,uid ) ;
		}
		StringDelete( &p ) ;
	}
	
	return st ;
}

static int _zuluMountPrintDeviceProperties( const char * device,const char * UUID,uid_t uid )
{	
	string_t p ;
	string_t q ;
	
	char * dev = NULL ;
	const char * e ;
	
	if( UUID == NULL ){
		if( strncmp( device,"/dev/loop",9 ) == 0 ){
			/*
			* zuluCryptLoopDeviceAddress() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
			*/
			dev = zuluCryptLoopDeviceAddress( device ) ;
			if( dev != NULL ){
				device = dev ;
			}
		}
	
		/*
		* zuluCryptCreateMapperName() is defined in ../zuluCrypt-cli/lib/create_mapper_name.c
		*/
		q = zuluCryptCreateMapperName( device,strrchr( device,'/' ) + 1,uid,CLOSE ) ;
	}else{
		p = String( UUID ) ;
		StringRemoveString( p,"\"" ) ;
		e = StringReplaceString( p,"UUID=","UUID-" ) ;
		q = zuluCryptCreateMapperName( device,e,uid,CLOSE ) ;
		StringDelete( &p ) ;
	}
	
	e = StringContent( q ) ;
	
	/*
	 * zuluCryptGetMtabEntry() is defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c
	 */
	
	if( zuluCryptPathIsValid( e ) ){
		p = zuluCryptGetMtabEntry( e ) ;
	}else{
		p = zuluCryptGetMtabEntry( device ) ;
	}
	
	StringDelete( &q ) ;

	if( p != StringVoid ){
		/*
		* zuluMountPrintDeviceProperties() is defined in ./print_mounted_volumes.c
		*/
		zuluMountPrintDeviceProperties_1( p,uid ) ;
		StringDelete( &p ) ;
	}else{
		/*
		 * zuluMountPartitionProperties is defined in ./print_mounted_volumes.c
		 */
		zuluMountPartitionProperties( device,NULL,device,NULL ) ;
	}
	
	if( dev != NULL ){
		free( dev ) ;
	}
	return 0 ;
}

static int _zuluPartitionHasCryptoFs( const char * device )
{
	int st ;
	string_t fs ;
	char * e = NULL ;
	/*
	* zuluCryptSecurityGainElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	*/
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	if( strncmp( device,"/dev/loop",9 ) == 0 ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
		 */
		e = zuluCryptLoopDeviceAddress( device ) ;
		if( e != NULL )
			device = e ;
	}
	/*
	 * this function is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	fs = zuluCryptGetFileSystemFromDevice( device ) ;
	if( e != NULL ){
		free( e ) ;
	}
	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityDropElevatedPrivileges() ;
	if( fs == StringVoid ){
		/*
		 * no file system is found,assuming the volume is crypto_PLAIN volume
		 */	
		return 1 ;
	}else{
		/*
		 * st == 1 means the volume is cryto_LUKS
		 * st == 0 means the volume has a regular file system
		 */
		st = StringEqual( fs,"crypto_LUKS" ) ;
		StringDelete( &fs ) ;
		return st ;
	}
}

static int _zuluMountPrintVolumeDeviceName( const char * device )
{	
	char * c ;
	char * e ;
	/*
	 * zuluCryptSecurityGainElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	if( strncmp( device,"/dev/loop",9 ) == 0 ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
		 */
		e = zuluCryptLoopDeviceAddress( device ) ;
		if( e != NULL ){
			c = zuluCryptVolumeDeviceName( e ) ;
			free( e ) ;
		}else{
			c = zuluCryptVolumeDeviceName( device ) ;
		}
	}else{
		/*
		* zuluCryptVolumeDeviceName() is defined in ../lib/status.c
		*/
		c = zuluCryptVolumeDeviceName( device ) ;
	}
	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityDropElevatedPrivileges() ;
	if( c == NULL ){
		printf( "ERROR: could not get device address from mapper address\n" ) ;
		return 1 ;
	}else{
		printf( "%s\n",c ) ;
		free( c ) ;
		return 0 ;
	}
}

static int _zuluMountExe( const char * device,const char * UUID,const char * action,const char * m_point,
			  const char * m_opts,const char * fs_opts,uid_t uid,const char * key,const char * key_source,
			  int mount_point_option,stringList_t stx )
{
	if( strcmp( action,"-D" ) == 0 )
		return _zuluMountPrintVolumeDeviceName( device ) ;
	
	if( strcmp( action,"-L" ) == 0 )
		return _zuluMountPrintDeviceProperties( device,UUID,uid ) ;
	
	if( strcmp( action,"-s" ) == 0 )
		return zuluMountVolumeStatus( device,UUID,uid ) ;
	
	if( strcmp( action,"-m" ) == 0 ){
		if( _zuluPartitionHasCryptoFs( device ) ){
			/*
			 * zuluMountMount() is defined in crypto_mount.c
			 */
			return zuluMountCryptoMount( device,UUID,m_opts,uid,key,key_source,m_point,mount_point_option,stx ) ;
		}else{
			/*
			 * zuluMountMount() is defined in mount.c
			 */
			return zuluMountMount( device,m_point,m_opts,fs_opts,uid,mount_point_option ) ;
		}
	}
	
	if( strcmp( action,"-u" ) == 0 ){
		if( _zuluPartitionHasCryptoFs( device ) ){
			/*
			 * zuluMountMount() is defined in crypto_umount.c
			 */
			return zuluMountCryptoUMount( device,UUID,uid,mount_point_option ) ;
		}else{
			/*
			 * zuluMountMount() is defined in umount.c
			 */
			return zuluMountUMount( device,uid,m_opts,mount_point_option ) ;
		}
	}
	
	return _zuluExit_1( 200,StringListVoid,NULL,"ERROR: unrecognized argument encountered" ) ;	
}

static int _mount_help()
{
	const char * doc3 ;
	const char * doc2 ;
	const char * doc1 = "\
options:\n\
-m -- mount a volume : arguments: -d partition_path -z mount_point -e mode(rw/ro)\n\
      -- additional arguments for crypto_LUKS and crypto_PLAIN volume, -p passphrase/-f keyfile\n";
	
	doc2 = "\
-u -- unmount a partition: arguments: -d partition_path\n\
-s -- print properties of an encrypted volume: arguments: -d partition_path\n";

      doc3 = "\
-l -- print a list of mounted partitions\n\
-L -- must be used with -d,print properties of a partition specified by d option\n\
-P -- print a list of all partitions\n\
-D -- get a device node address from its mapper path( mapper paths are usually located in /dev/mapper ). Required argument: -d\n";
	printf( "%s%s%s",doc1,doc2,doc3 ) ;
	
	return 201 ;
}

static void ExitOnMemoryExaustion( void )
{
	printf( "unexpected exiting because you have run out of memory\n" ) ;
	exit( 1 ) ;
}

static int _zuluMountcheckifLVM( const char * action,const char * rpath )
{
	if( strcmp( action,"-D" ) == 0 )
		return 0 ;
	/*
	 * we currently dont support lvm volumes,for the moment treat paths to dev/dm-* as lvm volumes and refuse
	 * to work with them.
	 */
	if( strncmp( rpath,"/dev/dm-",8 ) == 0 )
		return 1 ;
	
	return 0 ;
}

static int _zuluMountDoAction( const char * device,const char * UUID,const char * action,const char * m_point,
			      const char * m_opts,uid_t uid,const char * key,const char * key_source,
			      int mount_point_option,const char * fs_opts,stringList_t stx )
{
	int fd = -1 ;
	int fd1 = -1 ;
	int status ;
	char * dev = NULL ;
	/*
	 * zuluCryptGetDeviceFileProperties is defined in ../zuluCrypt-lib/file_path_security.c
	 */
	switch( zuluCryptGetDeviceFileProperties( device,&fd,&fd1,&dev,uid ) ){
		case 0 : break ;
		case 1 : printf( "ERROR: devices in /dev/ with user access permissions are not suppored\n" ) ;			return 220 ;
		case 2 : printf( "ERROR: given path is a directory\n" ) ;  					 		return 221 ;
		case 3 : printf( "ERROR: a file can have only one hard link\n" ) ;				 		return 222 ;
		case 4 : printf( "ERROR: a non supported device encountered,device is missing or permission denied\n" ) ; 	return 223 ;
		default: printf( "ERROR: a non supported device encountered,device is missing or permission denied\n" ) ; 	return 224 ;
	}
	
	if( dev == NULL ){
		printf( "ERROR: a non supported device encountered,device is missing or permission denied\n" ) ;
		return 224 ;
	}
		
	if( _zuluMountcheckifLVM( action,dev ) ){
		printf( "ERROR: this device looks like an lvm device,these devices are currently not supported\n" ) ;
		status = 226 ;
	}else{
		status = _zuluMountExe( dev,UUID,action,m_point,m_opts,fs_opts,uid,key,key_source,mount_point_option,stx ) ;
	}
	
	free( dev ) ;
	
	if( fd1 != -1 ){
		close( fd ) ;
	}
	if( fd != -1 ){
		close( fd ) ;
	}
	return status ;
}

static void _privilegeEvelationError( const char * msg )
{
	puts( msg ) ;
	exit( 255 ) ;
}

int main( int argc,char * argv[] )
{	
	const char * action     = NULL ;
	const char * dev        = NULL ;
	const char * m_point    = NULL ;
	const char * m_opts     = NULL ;
	const char * key        = NULL ;
	const char * key_source = NULL ;
	const char * key_argv   = NULL ;
	const char * fs_opts    = NULL ;
	int mount_point_option = 0 ;
	char * device ;
	string_t * k ;
	stringList_t stl ;
	stringList_t stx ;
	int status ;
	
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
	setuid( 0 ) ;
	seteuid( uid ) ;
	
	/*
	 * zuluCryptClearDeadMappers() is defined in ../zuluCrypt-cli/bin/clear_dead_mapper.c
	 */
	zuluCryptClearDeadMappers( uid ) ;
	
	/*
	 * zuluCryptSecuritySetPrivilegeElevationErrorFunction() is defined in ../zuluCrypt-cli/bin/security.c
	 * _privilegeEvelationError() function will be called when functions that elevate or drop privilges fail
	 */
	zuluCryptSecuritySetPrivilegeElevationErrorFunction( _privilegeEvelationError ) ;
	
	/*
	 * zuluCryptGetUserUIDForPrivilegeManagement() is defined in ../zuluCrypt-bin/security.c
	 */
	zuluCryptGetUserUIDForPrivilegeManagement( uid ) ;
	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	if( argc < 2 )
		return _mount_help() ;
	
	if( argc == 2 ){
		action = argv[ 1 ] ;
		if ( strcmp( action,"-h" ) == 0 || strcmp( action,"--help" ) == 0 || strcmp( action,"-help" ) == 0 ){
			return _mount_help() ;
		}
		if ( strcmp( action,"-v" ) == 0 || strcmp( action,"-version" ) == 0 || strcmp( action,"--version" ) == 0 ){
			printf( "%s\n",zuluCryptVersion() );
			return 0 ;
		}
	}
	
	StringExitOnMemoryExaustion( &ExitOnMemoryExaustion ) ;
	StringListExitOnMemoryExaustion( &ExitOnMemoryExaustion ) ;
	
	stl = StringListInit() ; 
	/*
	 * zuluCryptSecuritySanitizeTheEnvironment() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecuritySanitizeTheEnvironment( global_variable_user_uid,&stx ) ;
	
	if( _mount_get_opts( argc,argv,&action,&dev,&m_point,&m_opts,&key_argv,&key_source,&mount_point_option,&fs_opts ) != 0 )
		return _mount_help() ;
	
	if( key_argv != NULL ){
		k = StringListAssign( stl ) ;
		*k = String( key_argv ) ;
		strncpy( ( char * ) key_argv,"x",StringLength( *k ) ) ;
		key = StringContent( *k ) ;
	}
	
	if( action == NULL )
		return _zuluExit_2( 212,stl,stx,"ERROR: action not specified" ) ;
	
	if( strcmp( action,"-S" ) == 0 ){
		/*
		 * zuluCryptPrintPartitions() is defined in ../zuluCrypt-cli/bin/partitions.c
		 * it printf() devices with entries in "/etc/fstab","/etc/crypttab", and "/etc/zuluCrypttab"
		 */
		return _zuluExit_2( zuluCryptPrintPartitions( SYSTEM_PARTITIONS,0 ),stl,stx,NULL ) ;
	}
		
	if( strcmp( action,"-l" ) == 0 )
		return _zuluExit_2( _zuluMountMountedList( uid ),stl,stx,NULL ) ;
	
	if( strcmp( action,"-P" ) == 0 )
		return _zuluExit_2( _zuluMountDeviceList(),stl,stx,NULL ) ;
	
	if( strcmp( action,"-h" ) == 0 )
		return _zuluExit_2( _mount_help(),stl,stx,NULL ) ;
	
	if( dev == NULL )
		return _zuluExit_2( 213,stl,stx,"ERROR: device argument missing" ) ;
		
	if( m_opts == NULL )
		m_opts = "rw" ;
	
	/*
	 * zuluCryptSecurityEvaluateDeviceTags() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	if( strncmp( dev,"UUID=",5 ) == 0 ){
		device = zuluCryptSecurityEvaluateDeviceTags( "UUID",dev + 5 ) ;
		if( device != NULL ){
			status = _zuluMountDoAction( device,dev,action,m_point,m_opts,uid,key,key_source,mount_point_option,fs_opts,stx ) ;
			free( device ) ;
		}else{
			printf( "could not resolve UUID\n" ) ;
			status = 214 ;
		}
	}else if( strncmp( dev,"LABEL=",6 ) == 0 ){
		device = zuluCryptSecurityEvaluateDeviceTags( "LABEL",dev + 6 ) ;
		if( device != NULL ){
			status = _zuluMountDoAction( device,NULL,action,m_point,m_opts,uid,key,key_source,mount_point_option,fs_opts,stx ) ;
			free( device ) ;
		}else{
			printf( "could not resolve LABEL\n" ) ;
			status = 215 ;
		}
	}else{
		status = _zuluMountDoAction( dev,NULL,action,m_point,m_opts,uid,key,key_source,mount_point_option,fs_opts,stx ) ;
	}
			
	return _zuluExit_2( status,stl,stx,NULL ) ;
}
