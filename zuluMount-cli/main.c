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
#include "../zuluCrypt-cli/string/String.h"
#include "../zuluCrypt-cli/string/StringManage.h"
#include "../zuluCrypt-cli/string/StringList.h"
#include "../zuluCrypt-cli/lib/libzuluCrypt.h"

int zuluMountPrintMountedVolumes( uid_t uid ) ;
string_t zuluCryptGetUserHomePath( uid_t uid ) ;

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
int zuluMountMountVolume( const char * mapper,const char * m_point,const char * mode,uid_t id ) ;

static int mount_get_opts( int argc,char * argv[],const char ** action,const char ** device,
			   const char ** m_point, const char ** mode,const char ** key,const char ** key_source )
{
	int c ;
	while ( ( c = getopt( argc,argv,"shlMmUud:z:e:p:f:G:" ) ) != -1 ) {
		switch( c ){
			case 's' : *action  = "-s"   ; break ;
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

static int zuluExit( int st,string_t * p,char * q,const char * msg )
{
	if( q != NULL )
		free( q ) ;
	
	if( p != NULL )
		StringManageStringDelete( p ) ;
	
	if( msg != NULL )
		printf( "%s\n",msg ) ;
	
	return st ;
}

static int zuluMountNormalUserCanManagePartition( const char * device )
{
	string_t p = StringGetFromFile( "/etc/fstab" ) ;	
	string_t f ;
	
	stringList_t q ;
	stringList_t z ;
	
	const char * e ;
	
	size_t i ;
	size_t j ;
	int st = 1 ;
	
	if( p == StringVoid )
		return 1 ;	
	
	q = StringListStringSplit( p,'\n' ) ;
	StringDelete( &p ) ;
	
	if( q == StringListVoid )
		return 1 ;
	
	j = StringListSize( q ) ;
	
	for( i = 0 ; i < j ; i++ ){
		
		e = StringListContentAt( q,i ) ;
		
		z = StringListSplit( e,' ' ) ;
		
		if( z == StringListVoid ){
			StringListDelete( &q ) ;
			return 1 ;
		}
		
		f = StringListStringAt( z,0 ) ;
		
		if( StringEqual( f,device ) == 0 ){
			
			f = StringListStringAt( z,3 ) ;
				
			if( StringContains( f,"nouser" ) == 0 ){
				st = 1 ;
			}else if( StringContains( f,"user" ) == 0 ){
				st = 0 ;
			}else{
				/*
				 * not having either option means "nouser" since its
				 * the default option
				 */
				st = 1 ;
			}
			
			StringListMultipleDelete( &q,&z,'\0' ) ;
			
			return st ;
		}
		
		StringListDelete( &z ) ;		
	}
	
	StringListDelete( &q ) ;		
	
	return 1 ;	
}

static int zuluMountCheckDevicePermissions( const char * device,uid_t uid )
{	
	if( zuluCryptCheckIfPartitionIsSystemPartition( device ) == 1 ){
		if( zuluMountNormalUserCanManagePartition( device ) == 1 ){
			if( uid != 0 ){
				return 1 ;
			}
		}
	}
	
	return 0 ;
}

static int zuluMountMount( const char * device,const char * m_point,const char * mode,uid_t uid )
{
	int status ;
	
	string_t * z ;

	struct stat st ;
	
	uid_t org = geteuid() ;
	
	char * path ;
	char * q ;	
		
	/*
	 * Below function is defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c
	 * It checks if a device has an entry in "/etc/mtab" and return 1 if it does and 0 is it doesnt	 * 
	 */
	if( zuluCryptCheckIfMounted( device ) == 1 )
		return zuluExit( 101,NULL,NULL,"ERROR: device already mounted" ) ;
	
	if( zuluMountCheckDevicePermissions( device,uid ) == 1 )
		return zuluExit( 100,NULL,NULL,"ERROR: could not mount a system partition because it does not have \"user\" option in \"/etc/fstab\"" ) ;
	
	z = StringManageString() ;
	
	if( m_point != NULL ){
		*z = String( m_point ) ;
	}else{	
		/*
		 * Below function returns "$HOME/" and is defined in ../zuluCrypt-cli/lib/user_get_home_path.c
		 */
		*z = zuluCryptGetUserHomePath( uid ) ;
		
		if( *z == StringVoid )
			zuluExit( 102,z,NULL,"ERROR: could not get path to current user home directory" ) ;
		
		q = strrchr( device,'/' ) ;
		
		if( q == NULL )
			StringAppend( *z,device ) ;
		else
			StringAppend( *z,q + 1 ) ;
	}
		
	seteuid( uid ) ;
	
	path = ( char * ) StringContent( *z ) ;
	
	if( stat( path,&st ) == 0 ){
		seteuid( org ) ;		
		return zuluExit( 105,z,NULL,"ERROR: mount failed,mount point path already taken" ) ;			
	}
	
	if( mkdir( path,S_IRUSR | S_IWUSR | S_IXUSR ) != 0 ){
		seteuid( org ) ;		
		return zuluExit( 106,z,NULL,"ERROR: mount failed,could not create mount point" ) ;
	}
	
	path = realpath( path,NULL ) ;
	
	if( path == NULL ){
		seteuid( org ) ;		
		rmdir( path ) ;
		return zuluExit( 107,z,path,"ERROR: could not resolve mount point path" ) ;
	}
	
	seteuid( org ) ;
	
	chown( m_point,uid,uid ) ;
		
	/*
	 * zuluCryptMountVolume() defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	status = zuluCryptMountVolume( device,path,mode,uid )	;
	
	if( status == 0 ){		
		return zuluExit( 0,z,path,"SUCCESS: mount complete successfully" ) ;		
	}else{
		rmdir( m_point ) ;
		switch( status ){
			case 1 : return zuluExit( 108,z,path,"ERROR: failed to mount ntfs file system using ntfs-3g,is ntfs-3g package installed?" ) ;
			case 4 : return zuluExit( 109,z,path,"ERROR: mount failed,no or unrecognized file system" )	; 
			case 12: return zuluExit( 110,z,path,"ERROR: mount failed,could not get a lock on /etc/mtab~" ) ;	
			default: return zuluExit( 111,z,path,"ERROR: failed to mount the partition" ) ;
		}
	}
}

static int zuluMountUMount( const char * device,uid_t uid )
{
	char * m_point = NULL ;

	int status ;
		
	/*
	 * zuluCryptCheckIfMounted()  is defined in defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c 	 
	 */
	if( zuluCryptCheckIfMounted( device ) == 0 )
		return zuluExit( 127,NULL,NULL,"ERROR: device does appear to be mounted as it does not have an entry in \"/etc/mtab\"" ) ;
	
	if( zuluMountCheckDevicePermissions( device,uid ) == 1 )
		return zuluExit( 112,NULL,NULL,"ERROR: could not mount a system partition because it does not have \"user\" option in \"/etc/fstab\"" ) ;
		
	/*
	 * below function is defined in ../zuluCrypt-cli/lib/unmount_volume.c
	 */
	status = zuluCryptUnmountVolume( device,&m_point ) ;
	if( status == 0 ){
		if( m_point != NULL )
			rmdir( m_point ) ;					
		return zuluExit( 0,NULL,m_point,"SUCCESS: umount complete successfully" ) ;		
	}else{
		switch( status ) {
			case 1 : return zuluExit( 113,NULL,m_point,"ERROR: device does not exist" )  ;
			case 2 : return zuluExit( 114,NULL,m_point,"ERROR: failed to unmount,the mount point and/or one or more files are in use" )	;	
			case 4 : return zuluExit( 115,NULL,m_point,"ERROR: failed to unmount,could not get a lock on /etc/mtab~" ) ;	
			default: return zuluExit( 116,NULL,m_point,"ERROR: failed to unmount the partition" )	 ;			
		}
	}
}

static int zuluMountDeviceList( void )
{
	/*
	 * function is defined in ../zuluCrypt-cli/partitions.c
	 * 
	 * it printf() contents of "/proc/partitions" 
	 */
	return zuluCryptPrintPartitions( ALL_PARTITIONS ) ;
}

static int zuluMountMmountedList( uid_t uid )
{
	/*
	 * function is defined in print_mounted_volumes.c
	 */
	return zuluMountPrintMountedVolumes( uid ) ;
}

static int zuluMountCryptoMount( const char * device,const char * mode,uid_t uid,const char * key,const char * key_source,const char * m_point )
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
			return zuluExit( 117,NULL,NULL,"ERROR: could not get path to current user home directory" ) ;
				
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
	
	/*
	 * the function is defined in ../zuluCrypt-cli/bin/open_volume.c
	 */
	st = zuluCryptEXEOpenVolume( &opts,mapping_name,uid ) ;
	
	StringDelete( &p ) ;
	
	return st ;
}

static int zuluMountCryptoUMount( const char * device,uid_t uid )
{
	const char * mapping_name ;
	const char * e = strrchr( device,'/' ) ;
	/*
	if( is_luks( device ) == 1 && check_if_partition_is_system_partition( device ) == 1 && uid != 0 )
		return zuluExit( 200,NULL,NULL,"ERROR: insuffienct privilege to operate on a system partition" ) ;
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

static int zuluMountExe( const char * device, const char * action,const char * m_point,const char * mode,uid_t uid,const char * key,const char * key_source )
{	
	if( strcmp( action,"-m" ) == 0 )
		return zuluMountMount( device,m_point,mode,uid ) ;
	else if( strcmp( action,"-u" ) == 0 )
		return zuluMountUMount( device,uid ) ;
	else if( strcmp( action,"-M" ) == 0 )
		return zuluMountCryptoMount( device,mode,uid,key,key_source,m_point ) ;
	else if( strcmp( action,"-U" ) == 0 )
		return zuluMountCryptoUMount( device,uid ) ;
	else
		return zuluExit( 118,NULL,NULL,"ERROR: unrecognized argument encountered" ) ;	
}

static int mount_help()
{
	const char * doc = "\
options:\n\
-l -- print a list of mounted partitions\n\
-p -- print a list of partitions\n\
-m -- mount a partitions : arguments: -d partition_path -z mount_point -e mode(rw/ro)\n\
-u -- unmount a partition: arguments: -d partition_path\n\
-M -- mount a LUKS volume: arguments: -d partition_path -z mount_point -e mode(rw/ro) (-p passphrase/-f keyfile) \n\
-U -- unmount a LUKS volume: arguments: -d partition_path\n" ;

	printf( doc ) ;
	return 119 ;
}

int main( int argc,char * argv[] )
{	
	const char * action     = NULL ;
	const char * dev        = NULL ;
	const char * m_point    = NULL ;
	const char * mode       = NULL ;
	const char * key        = NULL ;
	const char * key_source = NULL ;
	
	char * device ;
	
	uid_t uid = getuid() ;
	
	int status ;
	
	string_t k = StringVoid ;
	
	if( mount_get_opts( argc,argv,&action,&dev,&m_point,&mode,&key,&key_source ) != 0 )
		return mount_help() ;
	
	if( key != NULL ){
		k = String( key ) ;
		memset( ( char * )key,'\0',StringLength( k ) ) ;
		key = StringContent( k ) ;
	}
	
	if( setuid( 0 ) != 0 )
		return zuluExit( 120,NULL,NULL,"ERROR: setuid(0) failed,check executable permissions" ) ;
	
	if( argc < 2 )
		return zuluExit( 121,NULL,NULL,"wrong number of arguments" ) ;
	
	if( action == NULL )
		return zuluExit( 122,NULL,NULL,"ERROR: action not specified" ) ;
	
	if( strcmp( action,"-s" ) == 0 ){
		/*
		 * function is defined in ../zuluCrypt-cli/bin/partitions.c
		 * it printf() devices with entries in "/etc/fstab","/etc/crypttab", and "/etc/zuluCrypttab"
		 */
		return zuluCryptPrintPartitions( SYSTEM_PARTITIONS ) ;		
	}
	
	if( strcmp( action,"-l" ) == 0 )
		return zuluMountMmountedList( uid ) ;
	
	if( strcmp( action,"-P" ) == 0 )
		return zuluMountDeviceList() ;	
	
	if( strcmp( action,"-h" ) == 0 )
		return mount_help() ;	
	
	if( dev == NULL )
		return zuluExit( 123,NULL,NULL,"ERROR: device argument missing" ) ;
		
	if( mode == NULL )
		mode = "rw" ;
	else{
		if( strcmp( mode,"ro" ) != 0 )
			if( strcmp( mode,"rw" ) != 0 )
				mode = "rw" ;
	}
	
	if( strncmp( dev,"UUID=",5 ) == 0 ){
		device = zuluCryptDeviceFromUUID( dev + 5 ) ;
		if( device != NULL ){
			status = zuluMountExe( device,action,m_point,mode,uid,key,key_source ) ;
			free( device ) ;
		}else{
			printf( "could not resolve UUID\n" ) ;
			status = 124 ;
		}
	}else if( strncmp( dev,"LABEL=",6 ) == 0 ){
		device = zuluCryptDeviceFromLabel( dev + 6 ) ;
		if( device != NULL ){
			status = zuluMountExe( device,action,m_point,mode,uid,key,key_source ) ;
			free( device ) ;
		}else{
			printf( "could not resolve LABEL\n" ) ;
			status = 125 ;
		}
	}else{
		device = realpath( dev,NULL ) ;	
		if( device != NULL ){
			status = zuluMountExe( device,action,m_point,mode,uid,key,key_source ) ;		
			free( device ) ;		
		}else{
			printf( "could not resolve path to device\n" ) ;
			status = 126 ;
		}
	}	
	
	if( k != StringVoid )
		StringDelete( &k ) ;
	
	return status ;
}
