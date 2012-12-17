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
void zuluMountPartitionProperties( const char * mapper,const char * device,const char * m_point ) ;
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

int _zuluMountPartitionAccess( const char * device,const char * m_opts,uid_t uid,stringList_t stl )
{
	/*
	 * this function is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	/*
	 * MOUNTOPTIONS constant is defined in ../zuluCrypt-cli/lib/includes.h
	 */
	string_t p ;
	int ro      ;
	int nouser  ;
	int defaulT ;
	int user    ;
	int users   ;
	
	if( zuluCryptPartitionIsSystemPartition( device ) )
		if( uid != 0 )
			return 4 ;
	
	p = StringListStringAt( stl,MOUNTOPTIONS ) ;
	
	if( p == StringVoid )
		return 0 ;

	ro      = StringContains( p,"ro" ) ;
	nouser  = StringContains( p,"nouser" ) ;
	defaulT = StringContains( p,"defaults" ) ;
	users   = StringContains( p,"users" );
	user    = StringContains( p,"user" ) ;

	if( ro && strstr( m_opts,"rw" ) != NULL )
		return 1 ;
	/*
	 * zuluCryptUserIsAMemberOfAGroup() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	if( uid == 0 || zuluCryptUserIsAMemberOfAGroup( uid,"zulucrypt" ) )
		return 0 ;
	if( nouser || defaulT ){
		return 2 ;
	}else if( user ){
		return 3 ;
	}else if( !nouser && !defaulT && !user && !users && uid != 0 ){
		return 2 ;
	}

	return 0 ;
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

int zuluMountVolumeStatus( const char * device,uid_t uid )
{
	return zuluCryptEXEVolumeInfo( strrchr( device,'/' ) + 1,device,uid ) ;
}

static int _zuluMountPrintDeviceProperties( const char * device,uid_t uid )
{	
	string_t p ;
	string_t q = zuluCryptCreateMapperName( device,strrchr( device,'/' ) + 1,uid,CLOSE ) ;
	const char * e = StringContent( q ) ;
	
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
		return 0 ;
	}else{
		/*
		 * zuluMountPartitionProperties is defined in ./print_mounted_volumes.c
		 */
		zuluMountPartitionProperties( device,device,NULL ) ;
		return 0 ;
	}
}

static int _zuluPartitionHasCryptoFs( const char * device )
{
	int st ;
	/*
	 * this function is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	string_t fs = zuluCryptGetFileSystemFromDevice( device ) ;
	
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

static int _zuluMountExe( const char * device,const char * action,const char * m_point,
			  const char * m_opts,const char * fs_opts,uid_t uid,const char * key,const char * key_source,
			  int mount_point_option )
{
	if( strcmp( action,"-D" ) == 0 )
		return _zuluMountPrintVolumeDeviceName( device ) ;
	
	if( strcmp( action,"-L" ) == 0 )
		return _zuluMountPrintDeviceProperties( device,uid ) ;
	
	if( strcmp( action,"-s" ) == 0 )
		return zuluMountVolumeStatus( device,uid ) ;
	
	if( strcmp( action,"-m" ) == 0 ){
		if( _zuluPartitionHasCryptoFs( device ) ){
			/*
			 * zuluMountMount() is defined in crypto_mount.c
			 */
			return zuluMountCryptoMount( device,m_opts,uid,key,key_source,m_point,mount_point_option ) ;
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
			return zuluMountCryptoUMount( device,uid,mount_point_option ) ;
		}else{
			/*
			 * zuluMountMount() is defined in umount.c
			 */
			return zuluMountUMount( device,uid,m_opts,mount_point_option ) ;
		}
	}
	
	return _zuluExit( 200,StringVoid,NULL,"ERROR: unrecognized argument encountered" ) ;	
}

static int _mount_help()
{
	const char * doc3 ;
	const char * doc2 ;
	const char * doc1 = "\
options:\n\
-m -- mount a volume : arguments: -d partition_path -z mount_point -e mode(rw/ro)\n\
	-- additional arguments for crypto_LUKS and crypto_PLAIN volume, -p passphrase/-f keyfile\n\
	-- if \"-n\" option is also set,the mount point folder will be used if present\n\
	-- if \"-n\" option is not set,an error will be produced if mount point folder is present\n\
	NOTE: presence or absence of -n is ineffective if the partition has an entry in fstab and the mount point is the same as that in fstab\n" ;
	
	doc2 = "\
-u -- unmount a partition: arguments: -d partition_path\n\
	-- if \"-n\" is set,the mount point folder will not be autodeleted\n\
	-- if \"-n\" is not set the mount point folder will be autodeleted\n\
	NOTE: a mount folder will not be autodeleted if the partition has an entry in fstab and the mount point is the same as that in fstab\n";

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

static int _zuluMountDoAction( const char * device,const char * action,const char * m_point,
			      const char * m_opts,uid_t uid,const char * key,const char * key_source,
			      int mount_point_option,const char * fs_opts )
{
	int status ;
	char * dev ;
	
	if( strncmp( device,"/dev/",5 ) != 0 ){
		/*
		 * check if a user has permission to access a volume in a file
		 */
		
		/*
		 * zuluCryptSecurityPathIsValid() is defined in zuluCrypt-cli/bin/security.c 
		 */
		if( !zuluCryptSecurityPathIsValid( device,uid ) ){
			printf( "ERROR: failed to resolve path to device\n" ) ;
			return 217 ;
		}
	}
	
	dev = zuluCryptRealPath( device ) ;
	
	if( dev == NULL ){
		printf( "ERROR: failed to resolve path to device\n" ) ;
		return 217 ;
	}
	
	if( _zuluMountcheckifLVM( action,dev ) ){
		printf( "ERROR: this device looks like an lvm device,these devices are currently not supported\n" ) ;
		status = 218 ;
	}else{
		if( strncmp( dev,"/dev/",5 ) != 0 ){
			/*
			* zuluCryptSecurityCanOpenPathForReading() is defined in ../zuluCrypt-cli/bin/security.c
			*/
			if( zuluCryptSecurityCanOpenPathForReading( dev,uid ) != 0 ){
				printf( "insuffienct privilege to access a volume file\n" ) ;
				status = 217 ;
			}else{
				status = _zuluMountExe( dev,action,m_point,m_opts,fs_opts,uid,key,key_source,mount_point_option ) ;
			}
		}else{
			status = _zuluMountExe( dev,action,m_point,m_opts,fs_opts,uid,key,key_source,mount_point_option ) ;
		}
	}
	
	free( dev ) ;
	
	return status ;
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
	uid_t uid ;
	string_t k = StringVoid ;
	int status ;
	
	/*
	 * global_variable_user_uid is a global variable defined in ../zuluCrypt-cli/bin/security.c 
	 * and declared in ../zuluCrypt-cli/bin/includes.
	 */
	uid = global_variable_user_uid = getuid() ;
		
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
	
	if( _mount_get_opts( argc,argv,&action,&dev,&m_point,&m_opts,&key_argv,&key_source,&mount_point_option,&fs_opts ) != 0 )
		return _mount_help() ;
	
	if( key_argv != NULL ){
		k = String( key_argv ) ;
		strncpy( ( char * ) key_argv,"x",StringLength( k ) ) ;
		key = StringContent( k ) ;
	}
	
	if( setuid( 0 ) != 0 )
		return _zuluExit( 210,k,NULL,"ERROR: setuid(0) failed,check executable permissions" ) ;
	
	if( action == NULL )
		return _zuluExit( 212,k,NULL,"ERROR: action not specified" ) ;
	
	if( strcmp( action,"-S" ) == 0 ){
		/*
		 * zuluCryptPrintPartitions() is defined in ../zuluCrypt-cli/bin/partitions.c
		 * it printf() devices with entries in "/etc/fstab","/etc/crypttab", and "/etc/zuluCrypttab"
		 */
		return _zuluExit( zuluCryptPrintPartitions( SYSTEM_PARTITIONS,0 ),k,NULL,NULL ) ;
	}
		
	if( strcmp( action,"-l" ) == 0 )
		return _zuluExit( _zuluMountMountedList( uid ),k,NULL,NULL ) ;
	
	if( strcmp( action,"-P" ) == 0 )
		return _zuluExit( _zuluMountDeviceList(),k,NULL,NULL ) ;
	
	if( strcmp( action,"-h" ) == 0 )
		return _zuluExit( _mount_help(),k,NULL,NULL ) ;
	
	if( dev == NULL )
		return _zuluExit( 213,k,NULL,"ERROR: device argument missing" ) ;
		
	if( m_opts == NULL )
		m_opts = "rw" ;
	
	/*
	 * zuluCryptSecurityEvaluateDeviceTags() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	if( strncmp( dev,"UUID=",5 ) == 0 ){
		device = zuluCryptSecurityEvaluateDeviceTags( "UUID",dev + 5 ) ;
		if( device != NULL ){
			status = _zuluMountDoAction( device,action,m_point,m_opts,uid,key,key_source,mount_point_option,fs_opts ) ;
			free( device ) ;
		}else{
			printf( "could not resolve UUID\n" ) ;
			status = 214 ;
		}
	}else if( strncmp( dev,"LABEL=",6 ) == 0 ){
		device = zuluCryptSecurityEvaluateDeviceTags( "LABEL",dev + 6 ) ;
		if( device != NULL ){
			status = _zuluMountDoAction( device,action,m_point,m_opts,uid,key,key_source,mount_point_option,fs_opts ) ;
			free( device ) ;
		}else{
			printf( "could not resolve LABEL\n" ) ;
			status = 215 ;
		}
	}else{
		status = _zuluMountDoAction( dev,action,m_point,m_opts,uid,key,key_source,mount_point_option,fs_opts ) ;
	}
	
	return _zuluExit( status,k,NULL,NULL ) ;
}
