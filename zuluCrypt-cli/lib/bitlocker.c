/*
 *
 *  Copyright (c) 2019
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

#include "includes.h"
#include <pwd.h>
#include <sys/mount.h>

const char * zuluCryptBitLockerType()
{
	return "crypto_BitLocker" ;
}

string_t zuluCryptBitLockerMapperPath( uid_t uid )
{
	return String_1( "/run/media/private/",getpwuid( uid )->pw_name,"/cryptoBitlocker",NULL ) ;
}

string_t zuluCryptBitLockerMapperName( const char * e )
{
	string_t st ;

	if( StringPrefixEqual( e,"/dev/loop" ) ){

		st = zuluCryptLoopDeviceAddress_2( e ) ;
	}else{
		st = String( e ) ;
	}

	StringReplaceChar( st,'/','_' ) ;

	return st ;
}

string_t zuluCryptBitLockerFullMapperPath( uid_t uid,const char * e )
{
	string_t a = zuluCryptBitLockerMapperPath( uid ) ;
	string_t b = zuluCryptBitLockerMapperName( e ) ;

	StringAppendString( a,b ) ;

	StringAppend( a,"/dislocker-file" ) ;

	StringDelete( &b ) ;

	return a ;
}

int zuluCryptBitLockerVolume( const char * e )
{
	return StringEndsWith_1( e,"/dislocker-file" ) ;
}

int zuluCryptBitLockerVolume_1( const char * e )
{
	stringList_t stl = StringListSplit( e,' ' ) ;
	int s = zuluCryptBitLockerVolume( StringListContentAtFirstPlace( stl ) ) ;
	StringListDelete( &stl ) ;
	return s ;
}

string_t zuluCryptBitLockerResolveMapperPath( const char * e,uid_t uid )
{
	string_t q = String( e ) ;
	string_t z = zuluCryptBitLockerMapperPath( uid ) ;

	StringRemoveString( q,StringContent( z ) ) ;
	StringRemoveString( q,"/dislocker-file" ) ;

	StringReplaceChar( q,'_','/' ) ;
	StringDelete( &z ) ;

	return q ;
}

const char * zuluCryptBitLockerCreateMapperPath( string_t e,uid_t uid )
{
	string_t st = zuluCryptBitLockerMapperPath( uid ) ;
	StringReplaceChar( e,'/','_' ) ;
	StringPrependString( e,st ) ;
	StringDelete( &st ) ;
	return StringAppend( e,"/dislocker-file" ) ;
}

int zuluCryptBitLockerlock( string_t mapperPath,char ** mount_point )
{
	char * e = NULL ;

	int r = zuluCryptUnmountVolume( StringContent( mapperPath ),&e ) ;

	int s ;

	const char * m ;

	if( r == 0 ){

		m = StringRemoveString( mapperPath,"/dislocker-file" ) ;

		s = ProcessExecute( ZULUCRYPTumount,m,NULL ) ; ;

		if( s == 0 ){

			*mount_point = e ;
		}else{
			StringFree( e ) ;
		}

		return s ;
	}else{
		return r ;
	}
}

int zuluCryptBitLockerUnlock( const open_struct_t * opts,string_t * xt )
{
	int r ;

	string_t st = String_1( opts->mapper_path,opts->mapper_name,NULL ) ;

	process_t p = Process( "/usr/bin/dislocker-fuse",opts->device,"-u \"Abc123@ 2\"","--",StringContent( st ),NULL ) ;


	ProcessStart( p ) ;

	//int m = ProcessWrite( p,opts->key,opts->key_len ) ;

	//printf( "--%d:%d",m,opts->key_len ) ;

	//ProcessCloseStdWrite( p ) ;

	r = ProcessExitStatus( p ) ;

	char * e = "dsdsd" ;
	ProcessGetOutPut( p,&e,ProcessStdOut ) ;

	printf( "---**--%s----\n",e ) ;

	printf( "---%d---\n",r ) ;
	if( r == 0 ){

		StringAppend( st,"/dislocker-file" ) ;
		*xt = st ;
		return 0 ;
	}else{
		puts( "ddd" ) ;
		StringDelete( &st ) ;
		return 4 ;
	}
}
