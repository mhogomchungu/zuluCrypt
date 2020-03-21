
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

#include<stdio.h>
#include<string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <libcryptsetup.h>

#include <sys/types.h>
#include <grp.h>

#include "../pluginManager/libzuluCryptPluginManager.h"
#include "../utility/process/process.h"

#include "bin_path.h"
#include "plugin_path.h"

static void _write( int x,const void * y,size_t z )
{
	if( write( x,y,z ) ){}
}
static void _close( int x )
{
	if( close( x ) ){}
}
static void _fchmod( int x,mode_t y )
{
	if( fchmod( x,y ) ){}
}

static const char * luksTestVolume   = "/tmp/zuluCrypt-luksTestVolume" ;
static const char * plainTestVolume  = "/tmp/zuluCrypt-plainTestVolume" ;
static const char * tcryptTestVolume = "/tmp/zuluCrypt-tcryptTestVolume" ;
static const char * vcryptTestVolume = "/tmp/zuluCrypt-vcryptTestVolume" ;
static const char * headerBackUp     = "/tmp/zuluCrypt-HeaderBackUp" ;
static const char * mount_point      = "zuluCrypt-MountPoint" ;
static const char * key              = "xyz" ;
static const char * key1             = "xxx" ;
static const char * zuluCryptExe     = ZULUCRYPTzuluCrypt ;
static const char * pluginPath       = ZULUCRYPTplugInPath;
static const char * keyfile          = "/tmp/zuluCrypt-KeyFile" ;
static const char * keyfile1         = "/tmp/zuluCrypt-KeyFile1" ;

static void _print( const char * msg )
{
	printf( "%s",msg ) ;
	fflush( stdout ) ;
}

static void _printLine( void )
{
	sleep( 1 ) ;
	_print( "\n" ) ;
}

static void EXIT( int st,char * msg )
{
	unlink( keyfile ) ;
	unlink( keyfile1 ) ;
	unlink( luksTestVolume ) ;
	unlink( plainTestVolume ) ;
	unlink( tcryptTestVolume ) ;
	unlink( vcryptTestVolume ) ;
	unlink( headerBackUp ) ;

	rmdir( mount_point ) ;

	if( msg ){
		puts( msg ) ;
		free( msg ) ;
	}

	exit( st ) ;
}

static void createKeyFiles_0( const char * keyfile )
{
	int f = open( keyfile,O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) ;
	ssize_t e ;

	puts( "creating a keyfile" ) ;

	if( f < 0 ){
		perror( "failed to create a keyfile: " ) ;
		EXIT( 1,NULL ) ;
	}else{
		_fchmod( f,S_IRWXU ) ;

		e = write( f,key,strlen( key ) ) ;
		_close( f ) ;

		if( e != 3 ){
			perror( "failed to create a keyfile: " ) ;
			EXIT( 1,NULL ) ;
		}
	}
}

static void createKeyFiles( void )
{
	createKeyFiles_0( keyfile ) ;
	createKeyFiles_0( keyfile1 ) ;
}

static void createTestImages_0( const char * path )
{
	int i ;
	int f ;
	int opt = O_WRONLY|O_TRUNC|O_CREAT ;
	char buffer[ 1024 ] = { '\0' } ;
	int size = 10 * 1024 ;

	f = open( path,opt,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) ;

	puts( "creating a testing image file" ) ;

	if( f < 0 ){
		perror( "failed to create testing images: " ) ;
		EXIT( 1,NULL ) ;
	}else{
		_fchmod( f,S_IRWXU ) ;

		for( i = 0 ; i < size ; i++ ){
			_write( f,buffer,1024 ) ;
		}
		close( f ) ;
	}
}

static void createTestImages( void )
{
	createTestImages_0( plainTestVolume ) ;
	createTestImages_0( luksTestVolume ) ;
	createTestImages_0( tcryptTestVolume ) ;
	createTestImages_0( vcryptTestVolume ) ;
}

static void _ProcessPrint( process_t p,void ( *print )( const char * ) )
{
	char * e = NULL ;

	ProcessGetOutPut( p,&e,ProcessStdOut ) ;

	if( ProcessWaitUntilFinished( &p ) != 0 ){
		puts( "FAILED" ) ;
		EXIT( 1,e ) ;
	}else{
		if( e ){
			if( print ){
				print( e ) ;
			}else{
				puts( "PASSED" ) ;
			}
			free( e ) ;
		}
	}
}

static void _ProcessGetResult( process_t p )
{
	_ProcessPrint( p,NULL ) ;
}

static void _ProcessGetResultANDPrint( process_t p )
{
	_ProcessPrint( p,_print ) ;
}

static void createHeaderBackup( const char * device,const char * msg )
{
	process_t p ;
	_print( msg ) ;
	p = Process( zuluCryptExe,"-B","-d",device,"-z",headerBackUp,NULL ) ;
	ProcessStart( p ) ;
	_ProcessGetResult( p ) ;
}

static void restoreHeaderBackup( const char * device,const char * msg )
{
	process_t p ;
	_print( msg ) ;
	p = Process( zuluCryptExe,"-R","-k","-d",device,"-z",headerBackUp,NULL ) ;
	ProcessStart( p ) ;
	_ProcessGetResult( p ) ;
}

static void createVolume( const char * device,const char * msg,const char * keysource,const char * type,const char * arg1,const char * arg2 )
{
	process_t p ;
	_print( msg ) ;
	p = Process( zuluCryptExe,NULL ) ;
	if( strcmp( keysource,"-p" ) == 0 ){
		ProcessSetArgumentList( p,"-c","-k","-d",device,"-t",type,keysource,key,arg1,arg2,NULL ) ;
	}else{
		ProcessSetArgumentList( p,"-c","-k","-d",device,"-t",type,keysource,keyfile,arg1,arg2,NULL ) ;
	}
	ProcessStart( p ) ;
	_ProcessGetResult( p ) ;
}

static void closeVolume( const char * device,const char * msg )
{
	process_t p ;
	_print( msg ) ;
	p = Process( zuluCryptExe,"-q","-d",device,NULL ) ;
	ProcessStart( p ) ;
	_ProcessGetResult( p ) ;
}

static void openVolume( const char * device,const char * msg,const char * keysource,const char * arg1,const char * arg2 )
{
	process_t p ;
	_print( msg ) ;
	p = Process( zuluCryptExe,NULL ) ;
	if( strcmp( keysource,"-p" ) == 0 ){
		ProcessSetArgumentList( p,"-o","-d",device,"-m",mount_point,keysource,key,arg1,arg2,NULL ) ;
	}else{
		ProcessSetArgumentList( p,"-o","-d",device,"-m",mount_point,keysource,keyfile,arg1,arg2,NULL ) ;
	}
	ProcessStart( p ) ;
	_ProcessGetResult( p ) ;
}

static void checkKeySlotsInUse( const char * device )
{
	process_t p ;
	_print( "check key slots in use: " ) ;
	p = Process( zuluCryptExe,"-b","-d",device,NULL ) ;
	ProcessStart( p ) ;
	_ProcessGetResultANDPrint( p ) ;
}

static void addKeysToLuks( const char * device )
{
	process_t p ;

	_print( "add a key to a luks volume using a key and a key: " ) ;

	p = Process( zuluCryptExe,"-a","-d",device,"-y",key,"-l",key1,NULL ) ;

	ProcessStart( p ) ;
	_ProcessGetResult( p ) ;

	_print( "add key to luks volume using keyfile and keyfile: " ) ;

	p = Process( zuluCryptExe,"-a","-d",device,"-u",keyfile,"-n",keyfile1,NULL ) ;

	ProcessStart( p ) ;
	_ProcessGetResult( p ) ;

	_print( "add key to luks volume using passphrase and keyfile: " ) ;

	p = Process( zuluCryptExe,"-a","-d",device,"-y",key,"-n",keyfile,NULL ) ;

	ProcessStart( p ) ;
	_ProcessGetResult( p ) ;

	_print( "add key to luks volume using keyfile and passphrase: " ) ;

	p = Process( zuluCryptExe,"-a","-d",device,"-u",keyfile1,"-l",key1,NULL ) ;

	ProcessStart( p ) ;
	_ProcessGetResult( p ) ;
}

static void removeKeysFromLuksVolume( const char * device )
{
	process_t p ;
	_print( "remove a key from a luks volume using a key: " ) ;
	p = Process( zuluCryptExe,"-r","-d",device,"-p",key,NULL ) ;
	ProcessStart( p ) ;
	_ProcessGetResult( p ) ;

	_print( "remove a key from a luks volume using a keyfile: " ) ;
	p = Process( zuluCryptExe,"-r","-d",device,"-f",keyfile,NULL ) ;
	ProcessStart( p ) ;
	_ProcessGetResult( p ) ;
}

static void checkForOpenedMappers( void )
{
	char * d ;

	int st = 1 ;

	struct dirent * entry ;

	DIR * dir = opendir( crypt_get_dir() ) ;

	printf( "check if there are no opened mappers: " ) ;

	if( dir == NULL ){
		printf( "failed to complete the test\n" ) ;
		EXIT( 1,NULL ) ;
	}

	while( ( entry = readdir( dir ) ) != NULL ){

		d = strstr( entry->d_name,"zuluCrypt-" ) ;
		if( d != NULL ){
			if( st ){
				printf( "FAILED\n" ) ;
				st = 0 ;
			}
			st = 0 ;
			printf( "found opened mapper: /dev/mapper/%s\n",entry->d_name ) ;
		}
	}

	closedir( dir ) ;

	if( st ){
		printf( "PASSED\n" ) ;
	}
}

static void openVolumeWithPlugIn( const char * device,const char * msg )
{
	process_t p ;
	_print( msg ) ;
	p = Process( zuluCryptExe,"-o","-d",device,"-m",mount_point,"-G",pluginPath,NULL ) ;
	ProcessStart( p ) ;
	_ProcessGetResult( p ) ;
}

static void checkIfDeviceIsLuks( const char * device )
{
	int st ;
	process_t p = Process( zuluCryptExe,"-i","-d",device,NULL ) ;
	ProcessStart( p ) ;
	st = ProcessWaitUntilFinished( &p ) ;

	if( st ){
		_print( "check if a luks volume is a luks volume: PASSED\n" ) ;
	}
}

static int _loop_device_module_is_not_present( void )
{
	struct stat stlsmod ;
	int st = 0 ;
	process_t p ;
	const char * lsmod = "/sbin/lsmod" ;
	char * output = NULL ;
	if( stat( lsmod,&stlsmod ) != 0 ){
		return 0 ;
	}
	p = Process( lsmod,NULL ) ;
	ProcessStart( p ) ;
	ProcessGetOutPut( p,&output,ProcessStdOut ) ;
	if( output ){
		if( strstr( output,"\nloop" ) == NULL ){
			st = 1 ;
		}
		free( output ) ;
	}
	ProcessWaitUntilFinished( &p ) ;
	return st ;
}

int zuluCryptRunTest( void )
{
	uid_t uid  = getuid() ;
	struct stat st ;

	int r = seteuid( 0 ) ;

	r = setgid( uid ) ;
	r = setgroups( 1,&uid ) ;
	r = setegid( uid ) ;
	r = setuid( uid ) ;

	if( r ){}

	if( _loop_device_module_is_not_present() ){
		printf( "\nWARNING: \"loop\" kernel module does not appear to be loaded,\n" ) ;
		printf( "tests and opening of encrypted containers in files will fail if the module was not built into the kernel\n\n" ) ;
	}

	createTestImages() ;
	createKeyFiles() ;

	_printLine() ;
	createVolume( luksTestVolume,"create a luks type volume using a key: ","-p","luks",NULL,NULL ) ;

	_printLine() ;
	checkIfDeviceIsLuks( luksTestVolume ) ;

	_printLine() ;
	createHeaderBackup( luksTestVolume,"create luks header backup: " ) ;

	_printLine() ;
	restoreHeaderBackup( luksTestVolume,"restore luks header from backup: " ) ;

	_printLine() ;
	createVolume( plainTestVolume,"create a plain type volume using a key: ","-p","plain",NULL,NULL ) ;

	_printLine() ;
	createVolume( tcryptTestVolume,"create a tcrypt type volume using a key: ","-p","tcrypt",NULL,NULL ) ;

	_printLine() ;
	createVolume( vcryptTestVolume,"create a vcrypt type volume with pim=10 using a key: ","-p","vcrypt","-g","/dev/urandom.aes.xts-plain64.256.sha512.10" ) ;

	_printLine() ;
	openVolume( plainTestVolume,"open a plain volume with a key: ","-p",NULL,NULL ) ;
	closeVolume( plainTestVolume,"closing a plain volume: " ) ;

	_printLine() ;
	openVolume( plainTestVolume,"open a plain volume with a keyfile: ","-f",NULL,NULL ) ;
	closeVolume( plainTestVolume,"closing a plain volume: " ) ;

	_printLine() ;
	openVolume( tcryptTestVolume,"open a tcrypt volume with a key: ","-p",NULL,NULL ) ;
	closeVolume( tcryptTestVolume,"closing a tcrypt volume: " ) ;

	_printLine() ;
	openVolume( vcryptTestVolume,"open a vcrypt volume with a key: ","-p","-t","vcrypt.10" ) ;
	closeVolume( vcryptTestVolume,"closing a vcrypt volume: " ) ;

	_printLine() ;
	if( stat( ZULUCRYPTTestPlugin,&st ) != 0 ){
		_print( "test plugin not found,skip plain volume opening with a plugin\n" ) ;
	}else{
		openVolumeWithPlugIn( plainTestVolume,"open a plain volume using a plugin: " ) ;
		closeVolume( plainTestVolume,"closing a plain volume: " ) ;
	}

	_printLine() ;
	openVolume( luksTestVolume,"open a luks volume with a key: ","-p",NULL,NULL ) ;
	closeVolume( luksTestVolume,"closing a luks volume: " ) ;

	_printLine() ;
	openVolume( luksTestVolume,"open a luks volume with a keyfile: ","-f",NULL,NULL ) ;
	closeVolume( luksTestVolume,"closing a luks volume: " ) ;

	_printLine() ;
	if( stat( ZULUCRYPTTestPlugin,&st ) != 0 ){
		_print( "test plugin not found,skip luks volume opening with a plugin\n" ) ;
	}else{
		openVolumeWithPlugIn( luksTestVolume,"open a luks volume using a plugin: " ) ;
		closeVolume( luksTestVolume,"closing a luks volume: " ) ;
	}

	_printLine() ;
	checkKeySlotsInUse( luksTestVolume ) ;

	_printLine() ;
	addKeysToLuks( luksTestVolume ) ;

	_printLine() ;
	checkKeySlotsInUse( luksTestVolume ) ;

	_printLine() ;
	removeKeysFromLuksVolume( luksTestVolume ) ;
	checkKeySlotsInUse( luksTestVolume ) ;

	_printLine() ;
	checkForOpenedMappers() ;

	EXIT( 0,NULL ) ;
	return 0 ;
}
