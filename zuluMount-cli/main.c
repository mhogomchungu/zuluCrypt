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

#include "../zuluCrypt-cli/bin/getopt.h"
#include "../zuluCrypt-cli/constants.h"
#include "../zuluCrypt-cli/bin/libzuluCrypt-exe.h"
#include "../zuluCrypt-cli/string/String.h"
#include "../zuluCrypt-cli/lib/libzuluCrypt.h"

#ifdef __STDC__
char * realpath( const char * path, char * resolved_path ) ;
int seteuid(uid_t uid);
#endif

int mount_print_mounted_volumes( uid_t uid ) ;
char * device_from_label( const char * ) ;
char * device_from_uuid( const char * ) ;

static int mount_get_opts( int argc,char * argv[],const char ** action,const char ** device,
			   const char ** m_point, const char ** mode,const char ** key,const char ** key_source )
{
	int c ;
	while ( ( c = getopt( argc,argv,"shlMmUud:z:e:p:f:" ) ) != -1 ) {
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
			default  : return -1 ;
		}
	}
	
	return 0 ;
}

static int mount_return( int st,string_t p,char * q,const char * msg )
{
	if( q != NULL )
		free( q ) ;
	
	if( p != NULL )
		StringDelete( &p ) ;
	
	if( msg != NULL )
		printf( "%s",msg ) ;
	
	return st ;
}

static int inkmount( const char * device,const char * m_point,const char * mode,uid_t uid )
{
	int status ;
	
	string_t p = NULL ;
	
	struct stat st ;
	
	uid_t org = geteuid() ;
	
	char * path ;
	char * q ;
	
	struct passwd * pass ;

	/*
	 * below functin checks the device if it has an entry in "/etc/fstab","/etc/crypttab" and "/etc/zuluCrypttab".
	 * 1 is returned if an entry is found and 0 is returned if an entry is not found.
	 *
	 * The function is defined in ../zuluCrypt-cli/bin/partitions.c 
	 */
	if( check_if_partition_is_system_partition( device ) == 1 && uid != 0 )				
			return mount_return( 200,NULL,NULL,"ERROR: insuffienct privilege to operate on a system partition\n" ) ;
		
	/*
	 * Below function is defined in ../zuluCrypt-cli/lib/print_mounted_volumes.c
	 * It checks if a device has an entry in "/etc/mtab" and return 1 if it does and 0 is it doesnt	 * 
	 */
	if( check_if_mounted( device ) == 1 )
		return mount_return( 2,NULL,NULL,"ERROR: device already mounted\n" ) ;

	if( m_point != NULL ){
		p = String( m_point ) ;
	}else{		
		pass = getpwuid( uid ) ;
		
		if( pass == NULL )
			mount_return( 3,NULL,NULL,"ERROR: could not get path to current user home directory\n" ) ;
				
		p = String( pass->pw_dir ) ;
		
		q = strrchr( device,'/' ) ;
		
		if( q == NULL )
			StringMultipleAppend( p,"/",device,'\0' ) ;
		else
			StringAppend( p,q ) ;
	}

	m_point = StringContent( p ) ;
	
	seteuid( uid ) ;
	
	if( stat( m_point,&st ) == 0 )
		mount_return( 4,p,NULL,"ERROR: mount failed,mount point path already taken\n" ) ;
			
	if( mkdir( m_point,S_IRUSR | S_IWUSR | S_IXUSR ) != 0 )
		mount_return( 5,p,NULL,"ERROR: mount failed,could not create mount point\n" ) ;
			
	seteuid( org ) ;
	
	chown( m_point,uid,uid ) ;
		
	path = realpath( m_point,NULL ) ;
	
	if( path == NULL )
		mount_return( 6,p,NULL,"ERROR: could not resolve mount point path\n" ) ;
			
	/*
	 * below function is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	status = mount_volume( device,path,mode,uid )	;
	
	if( status == 0 ){		
		return mount_return( 0,p,path,"SUCCESS: mount complete successfully\n" ) ;		
	}else{
		rmdir( m_point ) ;
		switch( status ){
			case 1 : return mount_return( 7,p,path,"ERROR: failed to mount ntfs file system using ntfs-3g,is ntfs-3g package installed?\n" ) ;
			case 4 : return mount_return( 7,p,path,"ERROR: mount failed,no or unrecognized file system\n" )	; 
			case 12: return mount_return( 7,p,path,"ERROR: mount failed,could not get a lock on /etc/mtab~\n" ) ;	
			default: return mount_return( 7,p,path,"ERROR: failed to mount the partition\n" ) ;
		}
	}
}

static int inkumount( const char * device,uid_t uid )
{
	char * m_point = NULL ;

	int status ;
	
	if( check_if_partition_is_system_partition( device ) == 1 && uid != 0 )
		return mount_return( 200,NULL,NULL,"ERROR: insuffienct privilege to operate on a system partition\n" ) ;
			
	if( check_if_mounted( device ) == 0 )
		return mount_return( 200,NULL,NULL,"ERROR: device not mounted\n" ) ;
		
	/*
	 * below function is defined in ../zuluCrypt-cli/lib/unmount_volume.c
	 */
	status = unmount_volume( device,&m_point ) ;
	if( status == 0 ){
		if( m_point != NULL )
			rmdir( m_point ) ;					
		return mount_return( 0,NULL,m_point,"SUCCESS: umount complete successfully\n" ) ;		
	}else{
		switch( status ) {
			case 1 : return mount_return( 3,NULL,m_point,"ERROR: device does not exist\n" )  ;
			case 2 : return mount_return( 3,NULL,m_point,"ERROR: failed to unmount,the mount point and/or one or more files are in use\n" )	;	
			case 4 : return mount_return( 3,NULL,m_point,"ERROR: failed to unmount,could not get a lock on /etc/mtab~\n" ) ;	
			default: return mount_return( 3,NULL,m_point,"ERROR: failed to unmount the partition\n" )	 ;			
		}
	}
}

static int device_list( void )
{
	/*
	 * function is defined in ../zuluCrypt-cli/partitions.c
	 * 
	 * it printf() contents of "/proc/partitions" 
	 */
	return print_partitions( ALL_PARTITIONS ) ;
}

static int mounted_list( uid_t uid )
{
	/*
	 * function is defined in print_mounted_volumes.c
	 */
	return mount_print_mounted_volumes( uid ) ;
}

static int crypto_mount( const char * device,const char * mode,uid_t uid,const char * key,const char * key_source,const char * m_point )
{
	struct passwd * pass ;
	
	string_t p = NULL;
	
	int st ;
	/*
	 * the struct is declared in ../zuluCrypt-cli/bin/libzuluCrypt-exe.h
	 */
	struct_opts opts ;
	
	const char * mapping_name ;
	const char * e = strrchr( device,'/' ) ;
	
	if( check_if_partition_is_system_partition( device ) == 1 && uid != 0 )
		return mount_return( 200,NULL,NULL,"ERROR: insuffienct privilege to operate on a system partition\n" ) ;
	
	if( e == NULL)
		mapping_name = device ;
	else
		mapping_name = e + 1 ;
	
	if( m_point != NULL ){
		p = String( m_point ) ;
	}else{		
		pass = getpwuid( uid ) ;
		
		if( pass == NULL )
			return mount_return( 3,NULL,NULL,"ERROR: could not get path to current user home directory\n" ) ;
				
		p = String( pass->pw_dir ) ;
		
		e = strrchr( device,'/' ) ;
		
		if( e == NULL )
			StringMultipleAppend( p,"/",device,'\0' ) ;
		else
			StringAppend( p,e ) ;
	}
	
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
	st = open_volumes( &opts,mapping_name,uid ) ;
	
	StringDelete( &p ) ;
	
	return st ;
}

static int crypto_umount( const char * device,uid_t uid )
{
	const char * mapping_name ;
	const char * e = strrchr( device,'/' ) ;
	
	if( check_if_partition_is_system_partition( device ) == 1 && uid != 0 )
		return mount_return( 200,NULL,NULL,"ERROR: insuffienct privilege to operate on a system partition\n" ) ;
	
	if( e == NULL)
		mapping_name = device ;
	else
		mapping_name = e + 1 ;
	
	/*
	 * the function is defined in ../zuluCrypt-cli/bin/close_volume.c
	 */
	return close_opened_volume( device,mapping_name,uid ) ;	
}

static int exe( const char * device, const char * action,const char * m_point,const char * mode,uid_t uid,const char * key,const char * key_source )
{	
	if( strcmp( action,"-m" ) == 0 )
		return inkmount( device,m_point,mode,uid ) ;
	else if( strcmp( action,"-u" ) == 0 )
		return inkumount( device,uid ) ;
	else if( strcmp( action,"-M" ) == 0 )
		return crypto_mount( device,mode,uid,key,key_source,m_point ) ;
	else if( strcmp( action,"-U" ) == 0 )
		return crypto_umount( device,uid ) ;
	else
		return mount_return( 150,NULL,NULL,"ERROR: unrecognized argument encountered\n" ) ;	
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
	return 170 ;
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
	uid_t org ;
	
	int status ;
	
	struct stat st ;
	
	if( mount_get_opts( argc,argv,&action,&dev,&m_point,&mode,&key,&key_source ) != 0 )
		return mount_help() ;
	
	if( setuid( 0 ) != 0 )
		return mount_return( 120,NULL,NULL,"ERROR: setuid(0) failed,check executable permissions\n" ) ;
	
	if( argc < 2 )
		return mount_return( 100,NULL,NULL,"wrong number of arguments\n" ) ;
	
	if( action == NULL )
		return mount_return( 150,NULL,NULL,"ERROR: action not specified\n" ) ;
	
	if( strcmp( action,"-s" ) == 0 ){
		/*
		 * function is defined in ../zuluCrypt-cli/bin/partitions.c
		 * it printf() devices with entries in "/etc/fstab","/etc/crypttab", and "/etc/zuluCrypttab"
		 */
		return print_partitions( SYSTEM_PARTITIONS ) ;		
	}
	
	if( strcmp( action,"-l" ) == 0 )
		return mounted_list( uid ) ;
	
	if( strcmp( action,"-P" ) == 0 )
		return device_list() ;	
	
	if( strcmp( action,"-h" ) == 0 )
		return mount_help() ;	
	
	if( dev == NULL )
		return mount_return( 160,NULL,NULL,"ERROR: device argument missing\n" ) ;
		
	if( mode == NULL )
		mode = "rw" ;
	else{
		if( strcmp( mode,"ro" ) != 0 )
			if( strcmp( mode,"rw" ) != 0 )
				mode = "rw" ;
	}
	
	if( strncmp( dev,"UUID=",5 ) == 0 ){
		device = device_from_uuid( dev + 5 ) ;
		if( device != NULL ){
			status = exe( device,action,m_point,mode,uid,key,key_source ) ;
			free( device ) ;
		}else{
			printf( "could not resolve UUID\n" ) ;
			status = 130 ;
		}
	}else if( strncmp( dev,"LABEL=",6 ) == 0 ){
		device = device_from_label( dev + 6 ) ;
		if( device != NULL ){
			status = exe( device,action,m_point,mode,uid,key,key_source ) ;
			free( device ) ;
		}else{
			printf( "could not resolve LABEL\n" ) ;
			status = 140 ;
		}
	}else{		
		org = geteuid() ;		
		seteuid( uid ) ;		
		status = stat( dev,&st ) ;
		seteuid( org ) ;		
		
		if( status != 0 ){			
			printf( "invalid path to device or insuffienct privilege to access it\n" ) ;
			status = 160 ;
		}else{		
			device = realpath( dev,NULL ) ;		
			status = exe( device,action,m_point,mode,uid,key,key_source ) ;		
			free( device ) ;		
		}
	}	
	
	return status ;
}
