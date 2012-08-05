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
#include "../zuluCrypt-cli/bin/libzuluCrypt-exe.h"
#include "../zuluCrypt-cli/string/String.h"
#include "../zuluCrypt-cli/lib/libzuluCrypt.h"

#ifdef __STDC__
char * realpath( const char * path, char * resolved_path ) ;
int seteuid(uid_t uid);
#endif

int mount_print_mounted_volumes( uid_t uid ) ;
int mount_print_partitions( void ) ;
char * device_from_label( const char * ) ;
char * device_from_uuid( const char * ) ;

static int mount_get_opts( int argc,char * argv[],const char ** action,const char ** device, const char ** m_point, const char ** mode,const char ** key,const char ** key_source )
{
	int c ;
	while ( ( c = getopt( argc,argv,"hlMmUud:z:e:p:f:" ) ) != -1 ) {
		switch( c ){
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

static int inkmount( const char * device,const char * m_point,const char * mode,uid_t uid )
{
	int status ;
	
	string_t p = NULL ;
	
	struct stat st ;
	
	uid_t org = geteuid() ;
	
	char * path ;
	char * q ;
	
	struct passwd * pass ;

	if( check_if_mounted( device ) == 1 ){
		printf( "ERROR: device already mounted\n" ) ;
		return 2 ;
	}

	if( m_point != NULL ){
		p = String( m_point ) ;
	}else{		
		pass = getpwuid( uid ) ;
		
		if( pass == NULL ){
			printf( "ERROR: could not get path to current user home directory\n" ) ;
			return 3 ;
		}
		
		p = String( pass->pw_dir ) ;
		
		q = strrchr( device,'/' ) ;
		
		if( q == NULL )
			StringMultipleAppend( p,"/",device,'\0' ) ;
		else
			StringAppend( p,q ) ;
	}

	m_point = StringContent( p ) ;
	
	seteuid( uid ) ;
	
	if( stat( m_point,&st ) == 0 ){
		printf( "ERROR: mount failed,mount point path already taken or insuffienct privilege to mount point path\n" ) ;
		StringDelete( &p ) ;
		return 4 ;
	}
	
	if( mkdir( m_point,S_IRUSR | S_IWUSR | S_IXUSR ) != 0 ){
		printf( "ERROR: mount failed,could not create mount point\n" ) ;
		StringDelete( &p ) ;		
		return 5 ;
	}
	
	seteuid( org ) ;
	
	chown( m_point,uid,uid ) ;
		
	path = realpath( m_point,NULL ) ;
	
	if( path == NULL ){
		printf( "ERROR: could not resolve mount point path\n" ) ;
		remove( m_point ) ;
		StringDelete( &p ) ;		
		return 6 ;	
	}
		
	status = mount_volume( device,path,mode,uid )	;
	
	if( status == 0 ){
		free( path ) ;
		StringDelete( &p ) ;		
		printf( "SUCCESS: mount complete successfully\n" ) ;
		return 0 ;
	}else{
		StringDelete( &p ) ;		
		free( path ) ;
		remove( m_point ) ;
		
		switch( status ){
			case 1 : printf( "ERROR: failed to mount ntfs file system using ntfs-3g,is ntfs-3g package installed?\n" ) 	; break ;
			case 4 : printf( "ERROR: mount failed,no or unrecognized file system\n" )					; break ;
			case 12: printf( "ERROR: mount failed,could not get a lock on /etc/mtab~\n" ) 					; break ;	
			default: printf( "ERROR: failed to mount the partition\n" )							; break ;
		}
		return 7 ;
	}
}

static int inkumount( const char * device )
{
	char * m_point = NULL ;

	int status ;
	
	if( check_if_mounted( device ) == 0 ){
		printf( "ERROR: device not mounted\n" ) ;
		return 2 ;
	}
	
	status = unmount_volume( device,&m_point ) ;
	if( status == 0 ){
		printf( "SUCCESS: umount complete successfully\n" ) ;
		if( m_point != NULL ){
			remove( m_point ) ;
			free( m_point ) ;
		}
		return 0 ;
	}else{
		if( m_point != NULL )
			free( m_point ) ;
		
		switch( status ) {
			case 1 : printf( "ERROR: device does not exist\n" ) 							; break ;
			case 2 : printf( "ERROR: failed to unmount,the mount point and/or one or more files are in use\n" )	; break ;					
			case 4 : printf( "ERROR: failed to unmount,could not get a lock on /etc/mtab~\n" ) 			; break ;	
			default: printf( "ERROR: failed to unmount the partition\n" )						; break ;			
		}
		
		return 3 ;
	}
}

static int device_list( void )
{
	return mount_print_partitions() ;
}

static int mounted_list( uid_t uid )
{
	return mount_print_mounted_volumes( uid ) ;
}

static int crypto_mount( const char * device,const char * mode,uid_t uid,const char * key,const char * key_source,const char * m_point )
{
	struct passwd * pass ;
	
	string_t p = NULL;
	
	int st ;
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
		pass = getpwuid( uid ) ;
		
		if( pass == NULL ){
			printf( "ERROR: could not get path to current user home directory\n" ) ;
			return 3 ;
		}
		
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
	
	st = open_volumes( &opts,mapping_name,uid ) ;
	
	StringDelete( &p ) ;
	
	return st ;
}

static int crypto_umount( const char * device,uid_t uid )
{
	const char * mapping_name ;
	const char * e = strrchr( device,'/' ) ;
	
	if( e == NULL)
		mapping_name = device ;
	else
		mapping_name = e + 1 ;
	
	return close_opened_volume( device,mapping_name,uid ) ;	
}

static int exe( const char * device, const char * action,const char * m_point,const char * mode,uid_t uid,const char * key,const char * key_source )
{	
	if( strcmp( action,"-m" ) == 0 )
		return inkmount( device,m_point,mode,uid ) ;
	else if( strcmp( action,"-u" ) == 0 )
		return inkumount( device ) ;
	else if( strcmp( action,"-M" ) == 0 )
		return crypto_mount( device,mode,uid,key,key_source,m_point ) ;
	else if( strcmp( action,"-U" ) == 0 )
		return crypto_umount( device,uid ) ;
	else{
		printf( "ERROR: unrecognized argument encountered\n" ) ;		
		return 150 ;
	}	
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
	
	if( setuid( 0 ) != 0 ){
		printf( "ERROR: setuid(0) failed,check executable permissions\n" ) ;
		return 120 ;
	}
	
	if( argc < 2 ){
		printf( "wrong number of arguments\n" ) ;
		exit( 100 ) ;
	}
	
	if( action == NULL ){
		printf( "ERROR: action not specified\n" ) ;
		return 150 ;
	}
	
	if( strcmp( action,"-l" ) == 0 )
		return mounted_list( uid ) ;
	
	if( strcmp( action,"-P" ) == 0 )
		return device_list() ;	
	
	if( strcmp( action,"-h" ) == 0 )
		return mount_help() ;	
	
	if( dev == NULL ){
		printf( "ERROR: device argument missing\n" ) ;
		return 160 ;
	}
		
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
