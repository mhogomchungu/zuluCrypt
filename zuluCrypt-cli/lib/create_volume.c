/*
 *
 *  Copyright (c) 2011-2015
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

#include <sys/syscall.h>
#include <libcryptsetup.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <limits.h>

int zuluCryptOpenPlain_2( const char * device,
			const char * mapper,
			const char * mode,
			const char * key,
			size_t key_len,
			const char * rng ) ;

static int zuluExit( int st,string_t m )
{
	StringDelete( &m ) ;
	return st ;
}

int zuluCryptCreateFileSystemInAVolume( const char * fs,const char * device_mapper )
{
	int r ;
	char * e = NULL ;
	process_t p ;

	if( StringsAreEqual( fs,"none" ) ){

		return 0 ;
	}
	/*
	 * zulucryptFileSystemIsSupported() is defined in mount_fs_options.c
	 */
	if( zulucryptFileSystemIsSupported( fs ) == 0 ){

		return 1 ;
	}

	p = Process( ZULUCRYPTmkfs,NULL ) ;

	if( StringAtLeastOneMatch_1( fs,"ext2","ext3","ext4",NULL ) ){

		ProcessSetArgumentList( p,"-t",fs,"-m","1",device_mapper,NULL ) ;

	}else if( StringsAreEqual( fs,"reiserfs" ) ){

		ProcessSetArgumentList( p,"-t",fs,"-f","-f","-q",device_mapper,NULL ) ;

	}else if( StringsAreEqual( fs,"jfs" ) ){

		ProcessSetArgumentList( p,"-t",fs,"-q",device_mapper,NULL ) ;

	}else if( StringsAreEqual( fs,"ntfs" ) ){

		ProcessSetArgumentList( p,"-t",fs,"-f",device_mapper,NULL ) ;

	}else if( StringsAreEqual( fs,"xfs" ) ){

		ProcessSetArgumentList( p,"-t",fs,"-f",device_mapper,NULL ) ;

	}else{
		ProcessSetArgumentList( p,"-t",fs,device_mapper,NULL ) ;

		/*
		 * unhandled fs are processed here.They are given 60 seconds to accomplish their task
		 * and are assumed to be running in interactive more and are blocked waiting for user input
		 * when they fail to return in time and hence are killed since we cant get to them from GUI
		 */

		ProcessSetOptionTimeout( p,60,SIGKILL ) ;
	}

	ProcessStart( p ) ;

	r = ProcessExitStatus( p ) ;

	if( r != 0 ){

		ProcessGetOutPut( p,&e,ProcessStdError ) ;

		if( e ){

			puts( e ) ;

			StringFree( e ) ;
		}
	}

	ProcessCleanUp( &p ) ;

	return r ;
}

typedef struct{

	const char * fs ;
	const char * type ;
	const char * pass ;
	size_t       pass_size ;
	const char * rng ;

}arguments ;

static int _create_volume( const char * device,const resolve_path_t * opts )
{
	int r ;

	const char * mapper ;

	const arguments * args = opts->args ;

	string_t m = String( "/zuluCrypt-create-volume-" ) ;
	mapper = StringAppendInt( m,(u_int64_t)syscall( SYS_gettid ) ) + 1 ;

	if( StringsAreEqual( args->type,"plain" ) ){

		if( zuluCryptOpenPlain_2( device,mapper,"rw",args->pass,args->pass_size,args->rng ) != 0 ){

			return zuluExit( 3,m ) ;
		}
	}else{
		if( StringAtLeastOneMatch_1( args->type,"luks","luks1",NULL ) ){

			if( zuluCryptCreateLuks( device,args->pass,args->pass_size,args->rng ) != 0 ){

				return zuluExit( 3,m ) ;
			}

		}else if( StringsAreEqual( args->type,"luks2" ) ){

			if( zuluCryptCreateLuks2( device,args->pass,args->pass_size,args->rng ) != 0 ){

				return zuluExit( 3,m ) ;
			}
		}else{
			return zuluExit( 3,m ) ;
		}

		if( zuluCryptOpenLuks( device,mapper,"rw",args->pass,args->pass_size ) != 0 ){

			return zuluExit( 3,m ) ;
		}
	}

	mapper = StringPrepend( m,crypt_get_dir() ) ;

	r = zuluCryptCreateFileSystemInAVolume( args->fs,mapper ) ;
	/*
	 * zuluCryptCloseMapper() is defined in close_mapper.c
	 */
	zuluCryptCloseMapper( mapper ) ;

	if( r == 0 ){
		
		return zuluExit( 0,m ) ;
	}else{
		return zuluExit( 3,m ) ;
	}
}

int zuluCryptCreateVolume( const char * device,const char * fs,
			   const char * type,const char * pass,
			   size_t pass_size,const char * rng )
{
	/*
	 * resolve_path_t is defined in includes.h
	 */
	resolve_path_t opts ;
	arguments args ;

	memset( &opts,'\0',sizeof( opts ) ) ;
	memset( &args,'\0',sizeof( args ) ) ;

	args.fs        = fs ;
	args.type      = type ;
	args.pass      = pass ;
	args.pass_size = pass_size ;
	args.rng       = rng ;

	opts.device       = device ;
	opts.args         = &args ;
	opts.open_mode    = O_RDWR ;
	opts.error_value  = 3 ;

	/*
	 * zuluCryptResolveDevicePath() is defined in resolve_path.c
	 */
	return zuluCryptResolveDevicePath( _create_volume,&opts ) ;
}
