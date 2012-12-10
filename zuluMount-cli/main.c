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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

#include "../zuluCrypt-cli/constants.h"
#include "../zuluCrypt-cli/bin/libzuluCrypt-exe.h"
#include "../zuluCrypt-cli/bin/includes.h"
#include "../zuluCrypt-cli/lib/libzuluCrypt.h"
#include "../zuluCrypt-cli/lib/includes.h"
#include "../zuluCrypt-cli/bin/includes.h"

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
			   const char ** m_point, const char ** mode,const char ** key,const char ** key_source,int * mpo )
{
	int c ;
	while ( ( c = getopt( argc,argv,"LntSshlPmud:z:e:p:f:G:" ) ) != -1 ) {
		switch( c ){
			case 'n' : *mpo     = 1      ; break ;
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
			case 'e' : *mode    = optarg ; break ;
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

static int _zuluExit( int st,string_t z,char * q,const char * msg )
{
	if( q != NULL )
		free( q ) ;
	
	StringDelete( &z ) ;
	
	if( msg != NULL )
		printf( "%s\n",msg ) ;
	
	return st ;
}

static int _zuluMountPartitionAccess( const char * mode,uid_t uid,stringList_t stl )
{
	/*
	 * this function is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	/*
	 * MOUNTOPTIONS constant is defined in ../zuluCrypt-cli/lib/includes.h
	 */
	string_t p = StringListStringAt( stl,MOUNTOPTIONS ) ;
	int ro      ;
	int nouser  ;
	int defaulT ;
	int user    ;
	int users   ;

	if( p == StringVoid )
		return 0 ;
	
	ro      = StringContains( p,"ro" ) ;
	nouser  = StringContains( p,"nouser" ) ;
	defaulT = StringContains( p,"defaults" ) ;
	users   = StringContains( p,"users" );
	user    = StringContains( p,"user" ) ;

	if( ro && strstr( mode,"rw" ) != NULL )
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

static int _zuluMountMount( const char * device,const char * m_point,const char * mode,uid_t uid,int mount_point_option )
{
	int status ;
	int mount_point_from_fstab = 0 ;
	string_t z = StringVoid ;

	char * path = NULL ;
	char * q ;
	const char * m_path ;
	
	stringList_t stl = StringListVoid ;
	/*
	 * zuluCryptPathIsNotValid() is defined in ../zuluCrypt-cli/lib/is_path_valid.c
	 */
	if( zuluCryptPathIsNotValid( device ) )
		return _zuluExit( 101,z,path,"ERROR: invalid path to device" ) ;
	
	/*
	 * zuluCryptPartitionIsMounted is defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c
	 * It checks if a device has an entry in "/etc/mtab" and return 1 if it does and 0 is it doesnt
	 */
	if( zuluCryptPartitionIsMounted( device ) )
		return _zuluExit( 101,z,path,"ERROR: device already mounted" ) ;
	
	/*
	 * zuluCryptGetFstabEntryList() is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	stl = zuluCryptGetFstabEntryList( device ) ;
	
	status = _zuluMountPartitionAccess( mode,uid,stl ) ;
	if( status == 1 ){
		StringListDelete( &stl ) ;
		return _zuluExit( 102,z,path,"ERROR: \"/etc/fstab\" entry for this partition requires it to be mounted read only" ) ;
	}else if( status == 2 ){
		StringListDelete( &stl ) ;
		return _zuluExit( 103,z,path,"ERROR: \"/etc/fstab\" entry for this partition requires only root user or members of group zulucrypt to mount it" ) ;
	}
		
	if( m_point != NULL ){
		StringListDelete( &stl ) ;
		z = String( m_point ) ;
	}else{
		z = StringListCopyStringAt( stl,MOUNTPOINT ) ;
		StringListDelete( &stl ) ;
		if( z != StringVoid ){
			mount_point_from_fstab = 1 ;
		}else{
			/*
			* Below function returns "$HOME/" and is defined in ../zuluCrypt-cli/lib/user_get_home_path.c
			*/
			z = zuluCryptGetUserHomePath( uid ) ;
		
			if( z == StringVoid )
				_zuluExit( 104,z,path,"ERROR: could not get path to current user home directory" ) ;
		
			q = strrchr( device,'/' ) ;
		
			if( q == NULL )
				StringAppend( z,device ) ;
			else
				StringAppend( z,q + 1 ) ;
		}
	}
	
	/*
	 * zuluCryptSecurityCreateMountPoint() is defined in ../zuluCrypt-cli/bin/security.c
	 * zuluCryptPathIsNotValid() is defined in ../zuluCrypt-cli/lib/is_path_valid.c
	 */
	
	m_path = StringContent( z ) ;
	
	if( zuluCryptPathIsNotValid( m_path ) ){
		/*
		 * Mount folder does not exit,try to create it
		 */
		if( zuluCryptSecurityCreateMountPoint( m_path,uid ) != 0 ){
			return _zuluExit( 105,z,path,"ERROR: could not create mount point,invalid path or permission denied" ) ;
		}else{
			chown( m_path,uid,uid ) ;
		}
	}else{
		if( mount_point_from_fstab ){
			/*
			 * The mount point exists and we are trying to mount a system partition on a path present in fstab,just use the folder
			 */
			;
		}else{
			/*
			 * mount folder exist when we expect it not to and a option to auto use is not set,complain and exit
			 */
			if( !mount_point_option ){
				return _zuluExit( 106,z,path,"ERROR: could not create mount point,invalid path or path already taken" ) ;
			}
		}
	}
	
	path = realpath( m_path,NULL ) ;
	if( path == NULL )
		return _zuluExit( 107,z,path,"ERROR: could not resolve mount point path" ) ;
	
	/*
	 * zuluCryptMountVolume() defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	status = zuluCryptMountVolume( device,path,mode,uid ) ;
	
	if( status == 0 ){
		return _zuluExit( 0,z,path,"SUCCESS: mount complete successfully" ) ;
	}else{
		rmdir( path ) ;
		switch( status ){
			case 1 : return _zuluExit( 108,z,path,"ERROR: failed to mount ntfs file system using ntfs-3g,is ntfs-3g package installed?" ) ;
			case 4 : return _zuluExit( 109,z,path,"ERROR: mount failed,no or unrecognized file system" )	; 
			case 12: return _zuluExit( 110,z,path,"ERROR: mount failed,could not get a lock on /etc/mtab~" ) ;	
			default: return _zuluExit( 111,z,path,"ERROR: failed to mount the partition" ) ;
		}
	}
}

static int _zuluMountUserHasAccessToMountPoint( const char * device,uid_t uid )
{
	char * e ;
	int st = 0 ;
	/*
	 * zuluCryptSecurityPathIsValid() is defined in zuluCrypt-cli/lib/print_mounted_volumes.c 
	 */
	char * m_point = zuluCryptGetMountPointFromPath( device ) ;
	if( m_point != NULL ){
		e = strrchr( m_point,'/' ) ;
		if( e != NULL ){
			*e = '\0' ;
			/*
			 * zuluCryptSecurityPathIsValid() is defined in zuluCrypt-cli/bin/security.c
			 */
			st = zuluCryptSecurityPathIsValid( m_point,uid ) ;
		}
		free( m_point ) ;
	}
	
	return st ;
}

static int _zuluMountUMount( const char * device,uid_t uid,const char * mode,int mount_point_option )
{
	char * m_point = NULL ;
	stringList_t stl = StringListVoid ;
	int status ;
	string_t st ;
	/*
	 * zuluCryptPartitionIsMounted()  is defined in defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c 
	 */
	if( !zuluCryptPartitionIsMounted( device ) )
		return _zuluExit( 100,StringVoid,m_point,"ERROR: device does not appear to be mounted" ) ;
	
	/*
	 * zuluCryptGetFstabEntryList() is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	stl = zuluCryptGetFstabEntryList( device ) ;
	
	if( _zuluMountPartitionAccess( mode,uid,stl ) == 2 ){
		StringListDelete( &stl ) ;
		return _zuluExit( 101,StringVoid,m_point,"ERROR: \"/etc/fstab\" entry for this partition requires only root user or members of group zulucrypt to unmount it" ) ;
	}
	
	/*
	 * MOUNTOPTIONS and MOUNTPOINT constants are defined in ../zuluCrypt-cli/lib/includes.h
	 */
	st = StringListStringAt( stl,MOUNTOPTIONS ) ;
	if( !StringContains( st,"users" ) ){
		if( !_zuluMountUserHasAccessToMountPoint( device,uid ) ){
			return _zuluExit( 106,st,m_point,"ERROR: insuffienct privilege to unmount the volume,another user appear to have mount it" ) ;
		}
	}
	
	st = StringListCopyStringAt( stl,MOUNTPOINT ) ;
	StringListDelete( &stl ) ;
	
	/*
	 * zuluCryptUnmountVolume() is defined in ../zuluCrypt-cli/lib/unmount_volume.c
	 */
	status = zuluCryptUnmountVolume( device,&m_point ) ;
	if( status == 0 ){
		if( m_point != NULL ){
			if( !StringEqual( st,m_point ) ){
				if( !mount_point_option ){
					rmdir( m_point ) ;
				}
			}
		}
		return _zuluExit( 0,st,m_point,"SUCCESS: umount complete successfully" ) ;
	}else{
		switch( status ) {
			case 1 : return _zuluExit( 102,st,m_point,"ERROR: device does not exist" )  ;
			case 2 : return _zuluExit( 103,st,m_point,"ERROR: failed to unmount,the mount point and/or one or more files are in use" )	;	
			case 4 : return _zuluExit( 104,st,m_point,"ERROR: failed to unmount,could not get a lock on /etc/mtab~" ) ;	
			default: return _zuluExit( 105,st,m_point,"ERROR: failed to unmount the partition" )	 ;			
		}
	}
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

static int _zuluMountCryptoMount( const char * device,const char * mode,uid_t uid,
				  const char * key,const char * key_source,
				  const char * m_point,int mount_point_option )
{
	string_t p = StringVoid ;
	
	int st ;
	/*
	 * the struct is declared in ../zuluCrypt-cli/bin/libzuluCrypt-exe.h
	 */
	struct_opts opts ;
	
	const char * mapping_name ;
	const char * e = strrchr( device,'/' ) ;
	
	/*
	 * zuluCryptEXEGetOptsSetDefault() is defined in ../zuluCrypt-cli/bin/get_opts.c
	 */
	zuluCryptEXEGetOptsSetDefault( &opts ) ;
	
	if( e == NULL)
		mapping_name = device ;
	else
		mapping_name = e + 1 ;
	
	if( m_point != NULL ){
		p = String( m_point ) ;
	}else{	
		p = zuluCryptGetUserHomePath( uid ) ;
		
		if( p == StringVoid )
			return _zuluExit( 100,p,NULL,"ERROR: could not get path to current user home directory" ) ;
				
		e = strrchr( device,'/' ) ;
		
		if( e == NULL )
			StringAppend( p,device ) ;
		else
			StringAppend( p,e + 1 ) ;
	}
	
	if( key_source != NULL )
		if( strcmp( key_source,"-G" ) == 0 )
			opts.plugin_path = key ;
	
	opts.mount_point = StringContent( p ) ;
	opts.open_no_mount = -1 ;
	opts.device = device ;
	opts.mode = mode ;
	opts.key = key ;
	opts.key_source = key_source ;
	opts.mount_point_option = mount_point_option ;
	
	/*
	 * zuluCryptEXEOpenVolume() is defined in ../zuluCrypt-cli/bin/open_volume.c
	 */
	st = zuluCryptEXEOpenVolume( &opts,mapping_name,uid ) ;
	
	StringDelete( &p ) ;
	
	return st ;
}

static int _zuluMountCryptoUMount( const char * device,uid_t uid,__attribute__((unused)) int mount_point_option )
{
	const char * mapping_name ;
	const char * e = strrchr( device,'/' ) ;
	/*
	if( is_luks( device ) == 1 && check_if_partition_is_system_partition( device ) == 1 && uid != 0 )
		return _zuluExit( 200,NULL,NULL,"ERROR: insuffienct privilege to operate on a system partition" ) ;
	*/
	if( e == NULL)
		mapping_name = device ;
	else
		mapping_name = e + 1 ;
	
	/*
	 * zuluCryptEXECloseVolume() is defined in ../zuluCrypt-cli/bin/close_volume.c
	 */
	return zuluCryptEXECloseVolume( device,mapping_name,uid ) ;
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

static int _zuluMountExe( const char * device,const char * action,const char * m_point,
			  const char * mode,uid_t uid,const char * key,const char * key_source,
			  int mount_point_option )
{
	if( strcmp( action,"-L" ) == 0 )
		return _zuluMountPrintDeviceProperties( device,uid ) ;
	
	if( strcmp( action,"-s" ) == 0 )
		return zuluMountVolumeStatus( device,uid ) ;
	
	if( strcmp( action,"-m" ) == 0 ){
		if( _zuluPartitionHasCryptoFs( device ) ){
			return _zuluMountCryptoMount( device,mode,uid,key,key_source,m_point,mount_point_option ) ;
		}else{
			return _zuluMountMount( device,m_point,mode,uid,mount_point_option ) ;
		}
	}
	
	if( strcmp( action,"-u" ) == 0 ){
		if( _zuluPartitionHasCryptoFs( device ) ){
			return _zuluMountCryptoUMount( device,uid,mount_point_option ) ;
		}else{
			return _zuluMountUMount( device,uid,mode,mount_point_option ) ;
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
-p -- print a list of all partitions\n" ;      
	printf( "%s%s%s",doc1,doc2,doc3 ) ;
	
	return 201 ;
}

static void ExitOnMemoryExaustion( void )
{
	printf( "unexpected exiting because you have run out of memory\n" ) ;
	exit( 1 ) ;
}

static int _zuluMountcheckifLVM( const char * path,const char * rpath )
{
	/*
	 * zuluCryptGetFileSystemFromDevice() is defined in ../zuluCrypt-cli/lib/mount_volume.c 
	 */
	
	/*
	 * in this function,we are making an assumption that a /dev/dm-X path with a file system is a lvm device
	 */
	string_t st = zuluCryptGetFileSystemFromDevice( path ) ;
	if( st != StringVoid ){
		if( strncmp( rpath,"/dev/dm-",8 ) == 0 ){
			return 1 ;
		}
	}
	
	return 0 ;
}

static int _zuluMountDoAction( const char * device,const char * action,const char * m_point,
			      const char * mode,uid_t uid,const char * key,const char * key_source,
			      int mount_point_option )
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
	
	dev = realpath( device,NULL ) ;
	
	if( dev == NULL ){
		printf( "ERROR: failed to resolve path to device\n" ) ;
		return 217 ;
	}
	
	if( _zuluMountcheckifLVM( device,dev ) ){
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
				status = _zuluMountExe( dev,action,m_point,mode,uid,key,key_source,mount_point_option ) ;
			}
		}else{
			status = _zuluMountExe( dev,action,m_point,mode,uid,key,key_source,mount_point_option ) ;
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
	const char * mode       = NULL ;
	const char * key        = NULL ;
	const char * key_source = NULL ;
	const char * key_argv   = NULL ;
	int mount_point_option = 0 ;
	char * device ;

	uid_t uid ;
	
	int status ;
	
	string_t k = StringVoid ;
	
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
	
	uid = getuid() ;
	
	StringExitOnMemoryExaustion( &ExitOnMemoryExaustion ) ;
	StringListExitOnMemoryExaustion( &ExitOnMemoryExaustion ) ;
	
	if( _mount_get_opts( argc,argv,&action,&dev,&m_point,&mode,&key_argv,&key_source,&mount_point_option ) != 0 )
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
		
	if( mode == NULL )
		mode = "rw" ;
	
	/*
	 * zuluCryptDeviceFromUUID() and zuluCryptDeviceFromLabel() are defined in ../zuluCrypt-cli/lib/blkid_evaluate_tag.c
	 */
	if( strncmp( dev,"UUID=",5 ) == 0 ){
		device = zuluCryptDeviceFromUUID( dev + 5 ) ;
		if( device != NULL ){
			status = _zuluMountDoAction( device,action,m_point,mode,uid,key,key_source,mount_point_option ) ;
			free( device ) ;
		}else{
			printf( "could not resolve UUID\n" ) ;
			status = 214 ;
		}
	}else if( strncmp( dev,"LABEL=",6 ) == 0 ){
		device = zuluCryptDeviceFromLabel( dev + 6 ) ;
		if( device != NULL ){
			status = _zuluMountDoAction( device,action,m_point,mode,uid,key,key_source,mount_point_option ) ;
			free( device ) ;
		}else{
			printf( "could not resolve LABEL\n" ) ;
			status = 215 ;
		}
	}else{
		status = _zuluMountDoAction( dev,action,m_point,mode,uid,key,key_source,mount_point_option ) ;
	}
	
	return _zuluExit( status,k,NULL,NULL ) ;
}
