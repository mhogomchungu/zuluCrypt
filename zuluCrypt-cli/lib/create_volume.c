/*
 * 
 *  Copyright (c) 2011
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

#include <sys/syscall.h>

static inline int zuluExit( int st,string_t m )
{
	StringDelete( &m ) ;
	return st ;
}

static int _create_volume( const char * dev,const char * fs,const char * type,const char * pass,size_t pass_size,const char * rng )
{
	int status ;
	process_t p ;
	
	string_t m = StringVoid ;
	
	const char * device_mapper ;
	const char * mapper ;
	char * device ;
	char * e = NULL ;
	
	if ( zuluCryptPathIsNotValid( dev ) ){
		return 1 ;
	}
	if( StringsAreEqual( type,"luks" ) ){
		if( StringsAreNotEqual( rng,"/dev/random" ) ){
			if( StringsAreNotEqual( rng,"/dev/urandom" ) ){
				return 2 ;
			}
		}
	}
	
	device = zuluCryptRealPath( dev ) ;
		
	if( device == NULL ){
		return 3 ;
	}
	
	/*
	 * ZULUCRYPTlongMapperPath is set in ../constants.h
	 * zuluCryptCreateMapperName() is defined in create_mapper_name.c
	 */
	m = zuluCryptCreateMapperName( device,strrchr( device,'/' ) + 1,0,ZULUCRYPTlongMapperPath ) ;
	
	free( device ) ;
	
	device_mapper = StringAppendInt( m,syscall( SYS_gettid ) ) ;
	mapper = strrchr( device_mapper,'/' ) + 1 ;
	
	if( StringsAreEqual( type,"luks" ) ){
		if( zuluCryptCreateLuks( dev,pass,pass_size,rng ) != 0 ){
			return zuluExit( 3,m ) ;
		}
		if( zuluCryptOpenLuks( dev,mapper,"rw",pass,pass_size ) != 0 ){
			return zuluExit( 3,m ) ; 
		}
	}else if( StringsAreEqual( type,"plain") ){
		if( zuluCryptOpenPlain( dev,mapper,"rw",pass,pass_size ) ){
			return zuluExit( 3,m ) ; 
		}
	}else if( StringsAreEqual( type,"tcrypt" ) ){
		if( zuluCryptCreateTCrypt( dev,pass,pass_size,rng ) != 0 ){
			return zuluExit( 3,m ) ;
		}
		if( zuluCryptOpenTcrypt( dev,mapper,"rw",pass,pass_size ) != 0 ){
			return zuluExit( 3,m ) ;
		}
	}else{
		return zuluExit( 2,m ) ;
	}
	
	p = Process( ZULUCRYPTmkfs ) ;
	
	if( StringsAreEqual( fs,"ext2" ) || StringsAreEqual( fs,"ext3" ) || StringsAreEqual( fs,"ext4" ) ){
		
		ProcessSetArgumentList( p,"-t",fs,"-m","1",device_mapper,ENDLIST ) ;
		
	}else if( StringsAreEqual( fs,"reiserfs" ) ){
		
		ProcessSetArgumentList( p,"-t",fs,"-f","-f","-q",device_mapper,ENDLIST ) ;
		
	}else if( StringsAreEqual( fs,"jfs" ) ){
		
		ProcessSetArgumentList( p,"-t",fs,"-q",device_mapper,ENDLIST ) ;
		
	}else if( StringsAreEqual( fs,"ntfs" ) ){
		
		ProcessSetArgumentList( p,"-t",fs,"-f",device_mapper,ENDLIST ) ;
		
	}else{
		ProcessSetArgumentList( p,"-t",fs,device_mapper,ENDLIST ) ;
		
		/*
		 * unhandled fs are processed here.They are given 60 seconds to accomplish their task
		 * and are assumed to be running in interactive more and are blocked waiting for user input
		 * when they fail to return in time and hence are killed since we cant get to them from GUI 
		 */
		
		ProcessSetOptionTimeout( p,60,SIGKILL ) ;
	}
	
	ProcessStart( p ) ;
	
	status = ProcessExitStatus( p ) ;
	
	if( status ){
		ProcessGetOutPut( p,&e,STDERROR ) ;
		if( e ){
			puts( e ) ;
			free( e ) ;
		}
	}
	
	zuluCryptCloseMapper( device_mapper );
	
	ProcessDelete( &p ) ;
	
	return status == 0 ? zuluExit( 0,m ) : zuluExit( 3,m ) ;
}

int zuluCryptCreateVolume( const char * dev,const char * fs,const char * type,const char * pass,size_t pass_size,const char * rng )
{
	string_t st ;
	int fd ;
	int r ;
	if( strncmp( dev,"/dev/",5 ) == 0 ){
		return _create_volume( dev,fs,type,pass,pass_size,rng ) ;
	}else{
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in ./create_loop.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( dev,O_RDWR,&fd,&st ) ){
			r = _create_volume( StringContent( st ),fs,type,pass,pass_size,rng ) ;
			StringDelete( &st ) ;
			close( fd ) ;
			return r ;
		}else{
			return 3 ;
		}
	}
}
