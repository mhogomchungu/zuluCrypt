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

int zuluCryptCreateVolume( const char * dev,const char * fs,const char * type,const char * pass,size_t pass_size,const char * rng )
{
	int status ;
	process_t p ;
	
	string_t m = StringVoid ;
		
	const char * device_mapper ;
	const char * mapper ;
	char * device ;
	char * e = NULL ;
	
	if ( zuluCryptPathIsNotValid( dev ) )
		return 1 ;
		
	if( strcmp( type,"luks" ) == 0 )
		if( strcmp( rng,"/dev/random" ) != 0 )
			if( strcmp( rng,"/dev/urandom" ) != 0 )
				return 2 ;
	
	device = realpath( dev,NULL ) ;
	
	if( device == NULL )
		return 3 ;
	
	m = zuluCryptCreateMapperName( device,strrchr( device,'/' ) + 1,0,CLOSE ) ;
	
	free( device ) ;

	device_mapper = StringAppendInt( m,syscall( SYS_gettid ) ) ;
	mapper = strrchr( device_mapper,'/' ) + 1 ;
	
	if( strcmp( type,"luks" ) == 0 ){
		if( zuluCryptCreateLuks( dev,pass,pass_size,rng ) != 0 )
			return zuluExit( 3,m ) ;
		if( zuluCryptOpenLuks( dev,mapper,"rw",pass,pass_size ) != 0 )
			return zuluExit( 3,m ) ; 
	}else if( strcmp( type,"plain") == 0 ){
		if( zuluCryptOpenPlain( dev,mapper,"rw",pass,pass_size ) )
			return zuluExit( 3,m ) ; 
	}else{
		return zuluExit( 2,m ) ;
	}

	p = Process( ZULUCRYPTmkfs ) ;
	
	if( strcmp( fs,"ext2" ) == 0 || strcmp( fs,"ext3" ) == 0 || strcmp( fs,"ext4" ) == 0 ){
		
		ProcessSetArgumentList( p,"-t",fs,"-m","1",device_mapper,ENDLIST ) ;
		
	}else if( strcmp( fs,"reiserfs" ) == 0 ){
		
		ProcessSetArgumentList( p,"-t",fs,"-f","-f","-q",device_mapper,ENDLIST ) ;
		
	}else if( strcmp( fs,"jfs" ) == 0 ){
		
		ProcessSetArgumentList( p,"-t",fs,"-q",device_mapper,ENDLIST ) ;
		
	}else if( strcmp( fs,"ntfs" ) == 0 ){
		
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
