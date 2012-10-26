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

int zuluMountPrintMountedVolumes( uid_t uid ) ;
string_t zuluCryptGetUserHomePath( uid_t uid ) ;
char * zuluCryptGetMountPointFromPath( const char * path ) ;
string_t zuluCryptCreateMapperName( const char * device,const char * mapping_name,uid_t uid,int i ) ;
void zuluMountPartitionProperties( const char * mapper,const char * device,const char * m_point ) ;
int zuluCryptVolumeIsNotLuks( const char * dev ) ;

#ifdef __STDC__
char * realpath( const char * path, char * resolved_path ) ;
int seteuid( uid_t uid );
int getopt(int argc, char * const argv[], const char *optstring);
extern char *optarg;
extern int optind, opterr, optopt;
#endif

int mount_print_mounted_volumes( uid_t uid ) ;
char * zuluCryptDeviceFromUUID( const char * ) ;
char * zuluCryptDeviceFromLabel( const char * ) ;
int _zuluMountMountVolume( const char * mapper,const char * m_point,const char * mode,uid_t id ) ;

static int _mount_get_opts( int argc,char * argv[],const char ** action,const char ** device,
			   const char ** m_point, const char ** mode,const char ** key,const char ** key_source,int * mpo )
{
	int c ;
	while ( ( c = getopt( argc,argv,"ntSshlPMmUud:z:e:p:f:G:" ) ) != -1 ) {
		switch( c ){
			case 'n' : *mpo     = 1      ; break ;
			case 't' : *action  = "-t"   ; break ;
			case 's' : *action  = "-s"   ; break ;
			case 'S' : *action  = "-S"   ; break ;
			case 'U' : *action  = "-U"   ; break ;
			case 'M' : *action  = "-M"   ; break ;
			case 'l' : *action  = "-l"   ; break ;
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

static int _zuluMountPartitionAccess( const char * device,const char * mode,uid_t uid )
{
	/*
	 * this function is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	string_t p = zuluCryptGetMountOptionsFromFstab( device,3 ) ;
	
	if( p == StringVoid )
		return 0 ;
	
	int st = 0 ;
	int ro      = StringContains( p,"ro" ) ;
	int nouser  = StringContains( p,"nouser" ) ;
	int defaulT = StringContains( p,"default" ) ;
	int user    = StringContains( p,"user" ) ;
	
	if( ro && strstr( mode,"rw" ) != NULL )
		st = 1 ;
	else if( nouser || defaulT ){
		if( uid != 0 )
			st = 2 ;
	}else if( user ){
		st = 3 ;
	}else if( !nouser && !defaulT && !user && uid != 0 ){
		st = 2 ;
	}
	
	StringDelete( &p ) ;
	
	return st ;
}

static int _zuluMountMount( const char * device,const char * m_point,const char * mode,uid_t uid,int mount_point_option )
{
	int status ;
	
	string_t z = StringVoid ;

	char * path = NULL ;
	char * q ;
	const char * m_path ;
		
	/*
	 * below function is defined in ../zuluCrypt-cli/bin/security.c
	 */
	if( zuluCryptSecurityCanOpenPathForReading( device,uid ) != 0 )
		return _zuluExit( 109,z,path,"ERROR: invalid path to device or insuffienct previlege to access it" ) ;
	
	/*
	 * Below function is defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c
	 * It checks if a device has an entry in "/etc/mtab" and return 1 if it does and 0 is it doesnt	 * 
	 */
	if( zuluCryptPartitionIsMounted( device ) )
		return _zuluExit( 101,z,path,"ERROR: device already mounted" ) ;
	
	switch( _zuluMountPartitionAccess( device,mode,uid ) ){
		case 1 : return _zuluExit( 102,z,path,"ERROR: \"/etc/fstab\" entry for this partition requires it to be mounted read only" ) ;
		case 2 : return _zuluExit( 103,z,path,"ERROR: \"/etc/fstab\" entry for this partition requires only root user to mount it" ) ;		
	}
	
	if( m_point != NULL ){
		z = String( m_point ) ;
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
	
	/*
	 * zuluCryptSecurityCreateMountPoint() defined in ../zuluCrypt-cli/bin/security.c
	 * zuluCryptPathIsNotValid() is defined in ../zuluCrypt-cli/lib/is_path_valid.c
	 */
	
	m_path = StringContent( z ) ;
	
	if( zuluCryptPathIsNotValid( m_path ) ){
		if( zuluCryptSecurityCreateMountPoint( m_path,uid ) != 0 )
			return _zuluExit( 105,z,path,"ERROR: could not create mount point,invalid path" ) ;
	}else{
		if( !mount_point_option )
			return _zuluExit( 106,z,path,"ERROR: could not create mount point,invalid path or path already taken" ) ;
	}
	
	path = realpath( m_path,NULL ) ;
	
	if( path == NULL ){
		rmdir( path ) ;
		return _zuluExit( 107,z,path,"ERROR: could not resolve mount point path" ) ;
	}

	chown( path,uid,uid ) ;
		
	/*
	 * zuluCryptMountVolume() defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */

	status = zuluCryptMountVolume( device,path,mode,uid )	;
	
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

static int _zuluMountUMount( const char * device,uid_t uid,const char * mode,int mount_point_option )
{
	char * m_point = NULL ;

	int status ;
		
	/*
	 * zuluCryptPartitionIsMounted()  is defined in defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c 	 
	 */
	if( !zuluCryptPartitionIsMounted( device ) )
		return _zuluExit( 100,StringVoid,m_point,"ERROR: device does not appear to be mounted as it does not have an entry in \"/etc/mtab\"" ) ;
	
	if( _zuluMountPartitionAccess( device,mode,uid ) == 2 )
		return _zuluExit( 101,StringVoid,m_point,"ERROR: \"/etc/fstab\" entry for this partition requires only root user to unmount it" ) ;
		
	/*
	 * below function is defined in ../zuluCrypt-cli/lib/unmount_volume.c
	 */
	status = zuluCryptUnmountVolume( device,&m_point ) ;
	if( status == 0 ){
		if( m_point != NULL )
			if( !mount_point_option )
				if( !zuluCryptPartitionIsSystemPartition( device ) )
					rmdir( m_point ) ;
		return _zuluExit( 0,StringVoid,m_point,"SUCCESS: umount complete successfully" ) ;
	}else{
		switch( status ) {
			case 1 : return _zuluExit( 102,StringVoid,m_point,"ERROR: device does not exist" )  ;
			case 2 : return _zuluExit( 103,StringVoid,m_point,"ERROR: failed to unmount,the mount point and/or one or more files are in use" )	;	
			case 4 : return _zuluExit( 104,StringVoid,m_point,"ERROR: failed to unmount,could not get a lock on /etc/mtab~" ) ;	
			default: return _zuluExit( 105,StringVoid,m_point,"ERROR: failed to unmount the partition" )	 ;			
		}
	}
}

static int _zuluMountDeviceList( void )
{
	/*
	 * function is defined in ../zuluCrypt-cli/partitions.c
	 * 
	 * it printf() contents of "/proc/partitions" 
	 */
	return zuluCryptPrintPartitions( ALL_PARTITIONS ) ;
}

static int _zuluMountMmountedList( uid_t uid )
{
	/*
	 * function is defined in print_mounted_volumes.c
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
	
	opts.plugin_path = NULL ;
	
	if( key_source != NULL )
		if( strcmp( key_source,"-G" ) == 0 )
			opts.plugin_path = key ;
	
	opts.mount_point = StringContent( p ) ;
	opts.interactive_passphrase = -1 ;
	opts.open_no_mount = -1 ;
	opts.device = device ;
	opts.mode = mode ;
	opts.key = key ;
	opts.key_source = key_source ;
	opts.mount_point_option = mount_point_option ;
	
	/*
	 * the function is defined in ../zuluCrypt-cli/bin/open_volume.c
	 */
	st = zuluCryptEXEOpenVolume( &opts,mapping_name,uid ) ;
	
	StringDelete( &p ) ;
	
	return st ;
}

static int _zuluMountCryptoUMount( const char * device,uid_t uid,int mount_point_option )
{
	mount_point_option = 0 ;
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
	 * the function is defined in ../zuluCrypt-cli/bin/close_volume.c
	 */
	return zuluCryptEXECloseVolume( device,mapping_name,uid ) ;
}

int zuluMountVolumeStatus( const char * device,uid_t uid )
{
	return zuluCryptEXEVolumeInfo( strrchr( device,'/' ) + 1,device,uid ) ;
}

static int _zuluMiniCryptoProperties( const char * device,uid_t uid )
{	
	string_t p = zuluCryptCreateMapperName( device,strrchr( device,'/' ) + 1,uid,CLOSE ) ;
	int st ;
	char * d ;
	
	if( p == StringVoid ){
		printf( "Nil\t0\t0\t0\t\n" ) ;
		st = 1 ;
	}else{
		/*
		 * this function is defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c
		 */
		d = zuluCryptGetMountPointFromPath( StringContent( p ) ) ;
		
		if( d == NULL ){
			printf( "Nil\t0\t0\t0\t\n" ) ;
			st = 1 ;
		}else{
			/*
			 * this function is defined in print_mounted_volumes.c
			 */
			zuluMountPartitionProperties( device,StringContent( p ),d ) ;
			free( d ) ;
			st = 0 ;
		}
		
		StringDelete( &p ) ;
	}	
	
	return st ;
}

static int _zuluMiniProperties( const char * device,uid_t uid )
{
	char * d ;
	/*
	 * this function is defined in ../zuluCrypt-cli/lib/is_luks.c
	 */
	if( zuluCryptVolumeIsNotLuks( device ) ){
		
		/*
		 * volume is not LUKS, assuming its a plain type or a regular partition
		 */
		d = zuluCryptGetMountPointFromPath( device ) ;
		
		if( d != NULL ){
			zuluMountPartitionProperties( device,device,d ) ;
			free( d ) ;
			return 0 ;
		}else{
			/*
			 * volume is not LUKS but doesnt look like its a regular partition, assume its plain volume
			 */
 			return _zuluMiniCryptoProperties( device,uid ) ;	
		}
	}else{
		/*
		 * volume is LUKS
		 */
		return _zuluMiniCryptoProperties( device,uid ) ;
	}	
}

static int _zuluMountExe( const char * device,const char * action,const char * m_point,
			  const char * mode,uid_t uid,const char * key,const char * key_source,
			  int mount_point_option )
{	
	if( strcmp( action,"-m" ) == 0 )
		return _zuluMountMount( device,m_point,mode,uid,mount_point_option ) ;
	else if( strcmp( action,"-u" ) == 0 )
		return _zuluMountUMount( device,uid,mode,mount_point_option ) ;
	else if( strcmp( action,"-M" ) == 0 )
		return _zuluMountCryptoMount( device,mode,uid,key,key_source,m_point,mount_point_option ) ;
	else if( strcmp( action,"-U" ) == 0 )
		return _zuluMountCryptoUMount( device,uid,mount_point_option ) ;
	else if( strcmp( action,"-s" ) == 0 )
		return zuluMountVolumeStatus( device,uid ) ;
	else if( strcmp( action,"-t" ) == 0 )
		return _zuluMiniProperties( device,uid ) ;
	else
		return _zuluExit( 200,StringVoid,NULL,"ERROR: unrecognized argument encountered" ) ;	
}

static int _mount_help()
{
	const char * doc = "\
options:\n\
-n -- when the option is used with -m,the mount point will be auto created if absent\
   -- error willl be produced if the mount point is present and the option is ommited\
   -- when used with -u,the mount point will not get auto deleted\
-l -- print a list of mounted partitions\n\
-p -- print a list of partitions\n\
-m -- mount a partitions : arguments: -d partition_path -z mount_point -e mode(rw/ro)\n\
-u -- unmount a partition: arguments: -d partition_path\n\
-M -- mount a LUKS/PLAIN volume: arguments: -d partition_path -z mount_point -e mode(rw/ro) (-p passphrase/-f keyfile) \n\
-U -- unmount a LUKS volume: arguments: -d partition_path\n" ;

	printf( doc ) ;
	return 201 ;
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
	char * e ;
	uid_t uid = getuid() ;
	
	int status ;
	
	string_t k = StringVoid ;
	
	if( _mount_get_opts( argc,argv,&action,&dev,&m_point,&mode,&key_argv,&key_source,&mount_point_option ) != 0 )
		return _mount_help() ;
	
	if( key_argv != NULL ){
		k = String( key_argv ) ;
		e = ( char * ) key_argv ;
		memset( e,'\0',StringLength( k ) ) ;
		e[ 0 ] = 'x' ;
		key = StringContent( k ) ;
	}
	
	if( setuid( 0 ) != 0 )
		return _zuluExit( 210,k,NULL,"ERROR: setuid(0) failed,check executable permissions" ) ;
	
	if( argc < 2 )
		return _zuluExit( 211,k,NULL,"wrong number of arguments" ) ;
	
	if( action == NULL )
		return _zuluExit( 212,k,NULL,"ERROR: action not specified" ) ;
	
	if( strcmp( action,"-S" ) == 0 ){
		/*
		 * function is defined in ../zuluCrypt-cli/bin/partitions.c
		 * it printf() devices with entries in "/etc/fstab","/etc/crypttab", and "/etc/zuluCrypttab"
		 */
		return _zuluExit( zuluCryptPrintPartitions( SYSTEM_PARTITIONS ),k,NULL,NULL ) ;
	}
		
	if( strcmp( action,"-l" ) == 0 )
		return _zuluExit( _zuluMountMmountedList( uid ),k,NULL,NULL ) ;
	
	if( strcmp( action,"-P" ) == 0 )
		return _zuluExit( _zuluMountDeviceList(),k,NULL,NULL ) ;
	
	if( strcmp( action,"-h" ) == 0 )
		return _zuluExit( _mount_help(),k,NULL,NULL ) ;
	
	if( dev == NULL )
		return _zuluExit( 213,k,NULL,"ERROR: device argument missing" ) ;
		
	if( mode == NULL )
		mode = "rw" ;
	
	if( strncmp( dev,"UUID=",5 ) == 0 ){
		device = zuluCryptDeviceFromUUID( dev + 5 ) ;
		if( device != NULL ){
			status = _zuluMountExe( device,action,m_point,mode,uid,key,key_source,mount_point_option ) ;
			free( device ) ;
		}else{
			printf( "could not resolve UUID\n" ) ;
			status = 214 ;
		}
	}else if( strncmp( dev,"LABEL=",6 ) == 0 ){
		device = zuluCryptDeviceFromLabel( dev + 6 ) ;
		if( device != NULL ){
			status = _zuluMountExe( device,action,m_point,mode,uid,key,key_source,mount_point_option ) ;
			free( device ) ;
		}else{
			printf( "could not resolve LABEL\n" ) ;
			status = 215 ;
		}
	}else{
		device = realpath( dev,NULL ) ;
		if( device != NULL ){
			status = _zuluMountExe( device,action,m_point,mode,uid,key,key_source,mount_point_option ) ;
			free( device ) ;
		}else{
			printf( "could not resolve path to device\n" ) ;
			status = 216 ;
		}
	}	
	
	return _zuluExit( status,k,NULL,NULL ) ;
}
