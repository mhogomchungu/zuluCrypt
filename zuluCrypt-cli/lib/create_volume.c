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

int zuluCryptCreateFileSystemInAVolume( const char * fs,const char * device_mapper )
{
	int status ;
	char * e = NULL ;
	
	process_t p = Process( ZULUCRYPTmkfs ) ;
	
	if( StringsAreEqual( fs,"ext2" ) || StringsAreEqual( fs,"ext3" ) || StringsAreEqual( fs,"ext4" ) ){
		
		ProcessSetArgumentList( p,"-t",fs,"-m","1",device_mapper,ENDLIST ) ;
		
	}else if( StringsAreEqual( fs,"reiserfs" ) ){
		
		ProcessSetArgumentList( p,"-t",fs,"-f","-f","-q",device_mapper,ENDLIST ) ;
		
	}else if( StringsAreEqual( fs,"jfs" ) ){
		
		ProcessSetArgumentList( p,"-t",fs,"-q",device_mapper,ENDLIST ) ;
		
	}else if( StringsAreEqual( fs,"ntfs" ) ){
		
		ProcessSetArgumentList( p,"-t",fs,"-f",device_mapper,ENDLIST ) ;
		
	}else if( StringsAreEqual( fs,"xfs" ) ){
		
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
	
	ProcessDelete( &p ) ;
	return status ;
}

static int _create_volume( const char * dev,const char * fs,const char * type,const char * pass,size_t pass_size,const char * rng )
{
	size_t len ;
	int status ;
	
	string_t m = StringVoid ;
	
	const char * device_mapper ;
	const char * mapper ;
	
	if ( zuluCryptPathIsNotValid( dev ) ){
		return 1 ;
	}
		
	m = String( crypt_get_dir() ) ;
	len = StringLength( m )   ;
	
	StringAppend( m,"/zuluCrypt-" ) ;
	device_mapper = StringAppendInt( m,syscall( SYS_gettid ) ) ;
	mapper = device_mapper + len + 1 ;
		
	if( StringsAreEqual( type,"luks" ) ){
		if( StringsAreNotEqual( rng,"/dev/random" ) ){
			if( StringsAreNotEqual( rng,"/dev/urandom" ) ){
				return zuluExit( 2,m ) ; 
			}
		}
		if( zuluCryptCreateLuks( dev,pass,pass_size,rng ) != 0 ){
			return zuluExit( 3,m ) ;
		}
		if( zuluCryptOpenLuks( dev,mapper,"rw",pass,pass_size ) != 0 ){
			return zuluExit( 3,m ) ; 
		}
	}else if( StringsAreEqual( type,"plain") ){
		if( zuluCryptOpenPlain( dev,mapper,"rw",pass,pass_size ) != 0 ){
			return zuluExit( 3,m ) ; 
		}
	}else{
		return zuluExit( 2,m ) ;
	}
	
	status = zuluCryptCreateFileSystemInAVolume( fs,device_mapper ) ;
	/*
	 * zuluCryptCloseMapper() is defined in close_mapper.c
	 */
	zuluCryptCloseMapper( device_mapper );
	
	return status == 0 ? zuluExit( 0,m ) : zuluExit( 3,m ) ;
}

int zuluCryptCreateVolume( const char * dev,const char * fs,const char * type,const char * pass,size_t pass_size,const char * rng )
{
	string_t st ;
	int fd ;
	int r ;
	if( StringPrefixMatch( dev,"/dev/",5 ) ){
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
