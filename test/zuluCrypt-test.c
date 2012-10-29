
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

#include<stdio.h>
#include<string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "../zuluCrypt-cli/pluginManager/libzuluCryptPluginManager.h"
#include "../zuluCrypt-cli/utility/process/process.h"
#include "../zuluCrypt-cli/utility/string/StringList.h"

#include "bin_path.h"

const char * luksTestVolume   = NULL ;
const char * plainTestVolume  = NULL ;
const char * mount_point      = NULL ;
const char * key              = NULL ;
const char * key1             = NULL ;
const char * zuluCryptExe     = NULL ;
const char * zuluCryptTest    = NULL ;
const char * keyfile          = NULL ;
const char * keyfile1         = NULL ;

stringList_t stl = StringListVoid ;

/*
 * lots of nothing to work around valgrind warnings
 */
void initGlobal( void )
{
	stl = StringList( "/tmp/zuluCrypt-luksTestVolume" ) ;
	
	StringListAppend( stl,"/tmp/zuluCrypt-plainTestVolume" ) ;
	StringListAppend( stl,"/tmp/zuluCrypt-MountPoint" ) ;
	
	StringListAppend( stl,"xyz" ) ;
	StringListAppend( stl,"xxx" ) ;
	
	StringListAppend( stl,ZULUCRYPTzuluCrypt ) ;
	StringListAppend( stl,ZULUCRYPTTest ) ;
	
	StringListAppend( stl,"/tmp/zuluCrypt-KeyFile" ) ;
	StringListAppend( stl,"/tmp/zuluCrypt-KeyFile1" ) ;
	
	luksTestVolume     = StringListContentAt( stl,0 ) ;
	plainTestVolume    = StringListContentAt( stl,1 ) ;
	mount_point        = StringListContentAt( stl,2 ) ;
	key                = StringListContentAt( stl,3 ) ;
	key1               = StringListContentAt( stl,4 ) ;
	zuluCryptExe       = StringListContentAt( stl,5 ) ;
	zuluCryptTest      = StringListContentAt( stl,6 ) ;
	keyfile            = StringListContentAt( stl,7 ) ;
	keyfile1           = StringListContentAt( stl,8 ) ;
}

void __print( const char * msg )
{
	printf( msg ) ;
	fflush( stdout );
}

void __printLine( void )
{
	sleep( 1 ) ;
	__print( "\n" ) ;
}

void EXIT( int st,char * msg )
{
	unlink( luksTestVolume ) ;
	unlink( plainTestVolume ) ;
	unlink( keyfile ) ;
	unlink( keyfile1 ) ;
	
	rmdir( mount_point ) ;
	
	if( msg ){
		puts( msg ) ;
		free( msg ) ;
	}
	
	StringListDelete( &stl ) ;
	exit( st ) ;
}

void createKeyFiles( void )
{
	int f = open( keyfile,O_WRONLY|O_TRUNC|O_CREAT ) ;
	int e ;
	puts( "creating a keyfile" ) ;
	
	if( f < 0 ){
		perror( "failed to create a keyfile: " ) ;
		EXIT( 1,NULL ) ;
	}else{
		e = write( f,key,strlen( key ) ) ;
		close( f ) ;
		chmod( keyfile,S_IRWXU ) ;
		if( e != 3 ){
			perror( "failed to create a keyfile: " ) ;
			EXIT( 1,NULL ) ;
		}
	}
	
	f = open( keyfile1,O_WRONLY|O_TRUNC|O_CREAT ) ;
	puts( "creating a keyfile" ) ;
	
	if( f < 0 ){
		perror( "failed to create a keyfile: " ) ;
		EXIT( 1,NULL ) ;
	}else{
		e = write( f,key1,strlen( key1 ) ) ;
		close( f ) ;
		chmod( keyfile1,S_IRWXU ) ;
		if( e != 3 ){
			perror( "failed to create a keyfile: " ) ;
			EXIT( 1,NULL ) ;
		}
	}
}

void createTestImages( void )
{
	int i ;
	int f ;
	int opt = O_WRONLY|O_TRUNC|O_CREAT ;
	char buffer[ 1024 ] = { '\0' };
	int size = 10 * 1024 ;
	char path[ 64 ] = { '\0' } ;
	
	strncpy( path,luksTestVolume,64 ) ;
	
	f = open( path,opt ) ;
	
	puts( "creating testing images" ) ;
	
	if( f < 0 ){
		perror( "failed to create testing images: " ) ;
		EXIT( 1,NULL ) ;
	}else{
		for( i = 0 ; i < size ; i++ )
			write( f,buffer,1024 ) ;
		close( f ) ;
		chmod( luksTestVolume,S_IRWXU ) ;
	}
	
	strncpy( path,plainTestVolume,64 ) ;
	
	f = open( path,opt ) ;
	
	if( f < 0 ){
		perror( "failed to create testing images: " ) ;
		EXIT( 1,NULL ) ;
	}else{
		for( i = 0 ; i < size ; i++ )
			write( f,buffer,1024 ) ;
		close( f ) ;
		chmod( plainTestVolume,S_IRWXU ) ;
	}
}

void __ProcessGetResult( process_t p )
{	
	char * e = NULL ;
	int st ;
	
	ProcessGetOutPut( p,&e,STDOUT ) ;
	
	st = ProcessExitStatus( p ) ;

	ProcessDelete( &p ) ;
	
	if( st ){
		puts( "FAILED" ) ;
		EXIT( 1,e ) ;
	}else{
		puts( "PASSED" ) ;
		if( e )
			free( e ) ;
	}
}

void __ProcessGetResultANDPrint( process_t p )
{	
	char * e = NULL ;
	int st ;
	ProcessGetOutPut( p,&e,STDOUT ) ;
	st = ProcessExitStatus( p ) ;
	
	ProcessDelete( &p ) ;
	
	if( st ){
		puts( "FAILED" ) ;
		EXIT( 1,e ) ;
	}else{
		if( e ){
			__print( e ) ;
			free( e ) ;
		}
	}
}

void createVolume( const char * device,const char * msg,const char * keysource,const char * type )
{
	process_t p ;
	__print( msg ) ;
	p = Process( zuluCryptExe ) ;
	if( strcmp( keysource,"-p" ) == 0 )
		ProcessSetArgumentList( p,"-c","-k","-d",device,"-t",type,keysource,key,'\0' ) ;
	else
		ProcessSetArgumentList( p,"-c","-k","-d",device,"-t",type,keysource,keyfile,'\0' ) ;
	ProcessStart( p ) ;
	__ProcessGetResult( p ) ;
}

void closeVolume( const char * device,const char * msg )
{	
	process_t p ;
	__print( msg ) ;
	p = Process( zuluCryptExe ) ;
	ProcessSetArgumentList( p,"-q","-d",device,'\0' ) ;
	ProcessStart( p ) ;
	__ProcessGetResult( p ) ;
}

void openVolume( const char * device,const char * msg,const char * keysource )
{
	process_t p ;
	__print( msg ) ;
	p = Process( zuluCryptExe ) ;
	if( strcmp( keysource,"-p" ) == 0 )
		ProcessSetArgumentList( p,"-o","-d",device,"-m",mount_point,keysource,key,'\0' ) ;
	else
		ProcessSetArgumentList( p,"-o","-d",device,"-m",mount_point,keysource,keyfile,'\0' ) ;
	ProcessStart( p ) ;
	__ProcessGetResult( p ) ;
}

void checkKeySlotsInUse( const char * device )
{
	process_t p ;
	__print( "check key slots in use: " ) ;
	p = Process( zuluCryptExe ) ;
	ProcessSetArgumentList( p,"-b","-d",device,'\0' ) ;
	ProcessStart( p ) ;
	__ProcessGetResultANDPrint( p ) ;
}

void addKeysToLuks( const char * device )
{
	process_t p ;
	
	__print( "add a key to a luks volume using a key and a key: " ) ;
	
	p = Process( zuluCryptExe ) ;
	
	ProcessSetArgumentList( p,"-a","-d",device,"-y",key,"-l",key1,'\0' ) ;
	ProcessStart( p ) ;
	__ProcessGetResult( p ) ;
	
	__print( "add key to luks volume using keyfile and keyfile: " ) ;
	
	p = Process( zuluCryptExe ) ;
	
	ProcessSetArgumentList( p,"-a","-d",device,"-u",keyfile,"-n",keyfile1,'\0' ) ;
	ProcessStart( p ) ;
	__ProcessGetResult( p ) ;
	
	__print( "add key to luks volume using passphrase and keyfile: " ) ;
	
	p = Process( zuluCryptExe ) ;
	
	ProcessSetArgumentList( p,"-a","-d",device,"-y",key,"-n",keyfile,'\0' ) ;
	ProcessStart( p ) ;
	__ProcessGetResult( p ) ;
	
	__print( "add key to luks volume using keyfile and passphrase: " ) ;
	
	p = Process( zuluCryptExe ) ;
	
	ProcessSetArgumentList( p,"-a","-d",device,"-u",keyfile1,"-l",key1,'\0' ) ;
	ProcessStart( p ) ;
	__ProcessGetResult( p ) ;
}

void removeKeysFromLuksVolume( const char * device )
{
	process_t p ;
	__print( "remove a key from a luks volume using a key: " ) ;
	p = Process( zuluCryptExe ) ;
	ProcessSetArgumentList( p,"-r","-d",device,"-p",key,'\0' ) ;
	ProcessStart( p ) ;
	__ProcessGetResult( p ) ;
	
	__print( "remove a key from a luks volume using a keyfile: " ) ;
	p = Process( zuluCryptExe ) ;
	ProcessSetArgumentList( p,"-r","-d",device,"-f",keyfile,'\0' ) ;
	ProcessStart( p ) ;
	__ProcessGetResult( p ) ;
}

void checkForOpenedMappers( void )
{
	char * d ;
	
	int st = 1 ;
	
	struct dirent * entry ;
	
	DIR * dir = opendir( "/dev/mapper" ) ;
	
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
	
	if( st )
		printf( "PASSED\n" ) ;
}

void openVolumeWithPlugIn( const char * device,const char * msg )
{
	process_t p ;
	__print( msg ) ;
	p = Process( zuluCryptExe ) ;
	ProcessSetArgumentList( p,"-o","-d",device,"-m",mount_point,"-G",zuluCryptTest,'\0' ) ;
	ProcessStart( p ) ;
	__ProcessGetResult( p ) ;
}

void checkIfDeviceIsLuks( const char * device )
{
	int st ;
	process_t p = Process( zuluCryptExe ) ;
	ProcessSetArgumentList( p,"-i","-d",device,'\0' ) ;
	ProcessStart( p ) ;
	st = ProcessExitStatus( p ) ;
	ProcessDelete( &p ) ;
	
	if( st )
		__print( "check if a luks volume is a luks volume: PASSED\n" ) ;
}

int runTest( void )
{	
	createTestImages() ;
	createKeyFiles() ;
	
	__printLine() ;
	createVolume( plainTestVolume,"create a plain type volume using a key: ","-p","plain" ) ;
		
	__printLine() ;
	createVolume( luksTestVolume,"create a luks type volume using a key: ","-p","luks" ) ;
	
	__printLine() ;
	checkIfDeviceIsLuks( luksTestVolume ) ;
	
	__printLine() ;
	openVolume( plainTestVolume,"open a plain volume with a key: ","-p" );
	closeVolume( plainTestVolume,"closing a plain volume: " ) ;
	
	__printLine() ;
	openVolume( plainTestVolume,"open a plain volume with a keyfile: ","-f" );
	closeVolume( plainTestVolume,"closing a plain volume: " ) ;
		
	__printLine() ;
	openVolumeWithPlugIn( plainTestVolume,"open a plain volume using a plugin: " ) ; 
	closeVolume( plainTestVolume,"closing a plain volume: " ) ;
	
	__printLine() ;
	openVolume( luksTestVolume,"open a luks volume with a key: ","-p" );
	closeVolume( luksTestVolume,"closing a luks volume: " ) ;
	
	__printLine() ;
	openVolume( luksTestVolume,"open a luks volume with a keyfile: ","-f" );
	closeVolume( luksTestVolume,"closing a luks volume: " ) ;
		
	__printLine() ;
	openVolumeWithPlugIn( luksTestVolume,"open a luks volume using a plugin: " ) ; 
	closeVolume( luksTestVolume,"closing a luks volume: " ) ;
	
	__printLine() ;
	checkKeySlotsInUse( luksTestVolume ) ;
	
	__printLine() ;
	addKeysToLuks( luksTestVolume ) ;
	
	__printLine() ;
	checkKeySlotsInUse( luksTestVolume ) ;
	
	__printLine() ;
	removeKeysFromLuksVolume( luksTestVolume ) ;
	checkKeySlotsInUse( luksTestVolume ) ;
	
	__printLine() ;
	checkForOpenedMappers() ;
	
	EXIT( 0,NULL ) ;
	return 0 ;
}

int main( void )
{
	initGlobal() ;
	return runTest() ; 
}

 
