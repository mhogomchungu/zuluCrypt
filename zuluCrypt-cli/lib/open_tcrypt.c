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

#include "includes.h"
#include <sys/syscall.h>

/*
 * this header is created at config time
 */
#include "truecrypt_support.h"

#if TRUECRYPT_CRYPTSETUP

static inline int zuluExit( int st,struct crypt_device * cd )
{
	crypt_free( cd );
	return st ;
}

/*
 * 1 is returned if a volume is a truecrypt volume.
 * 0 is returned if a volume is not a truecrypt volume or functionality is not supported
 */
int zuluCryptVolumeIsTcrypt( const char * device,const char * key,size_t key_len )
{
	struct crypt_device * cd = NULL;
	struct crypt_params_tcrypt params ;
	
	memset( &params,'\0',sizeof( struct crypt_params_tcrypt ) ) ;
	
	if( crypt_init( &cd,device ) < 0 ){
		return 0 ;
	}
	
	params.passphrase      = key ;
	params.passphrase_size = key_len ;
	params.flags           = CRYPT_TCRYPT_LEGACY_MODES ;
	
	if( crypt_load( cd,CRYPT_TCRYPT,&params ) == 0 ){
		return zuluExit( 1,cd ) ;
	}else{
		return zuluExit( 0,cd ) ;
	}
}

/*
 * 0 is returned if a volume was successfully opened.
 * 1 is returned if a volume was not successfully opened or functionality is not supported 
 */

static int _tcrypt_open_using_key( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{
	uint32_t flags = 0 ;
	
	struct crypt_device * cd = NULL;
	struct crypt_params_tcrypt params ;
	
	memset( &params,'\0',sizeof( struct crypt_params_tcrypt ) ) ;
	
	if( StringHasComponent( mode,"ro" ) ){
		flags |= CRYPT_ACTIVATE_READONLY;
	}
	if( crypt_init( &cd,device ) < 0 ){
		return 1 ;
	}
	
	params.passphrase       = pass ;
	params.passphrase_size  = pass_size ;
	params.flags            = CRYPT_TCRYPT_LEGACY_MODES ;
	
	if( crypt_load( cd,CRYPT_TCRYPT,&params ) != 0 ){
		return zuluExit( 1,cd ) ;
	}
	if( crypt_activate_by_volume_key( cd,mapper,NULL,0,flags ) == 0 ){
		return zuluExit( 0,cd ) ;
	}else{
		return zuluExit( 1,cd ) ;
	}
}

static int _tcrypt_open_using_keyfile( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{
	string_t st ;
	int xt ;
	int fd ;
	const char * file ;
	uint32_t flags = 0 ;
	
	struct crypt_device * cd = NULL;
	struct crypt_params_tcrypt params ;
	
	if( StringHasComponent( mode,"ro" ) ){
		flags |= CRYPT_ACTIVATE_READONLY;
	}
	if( crypt_init( &cd,device ) < 0 ){
		return 1 ;
	}
	
	/*
	 * ZULUCRYPTtempFolder is set in ../constants.h
	 */
	mkdir( ZULUCRYPTtempFolder,S_IRWXU ) ;
	chown( ZULUCRYPTtempFolder,0,0 ) ;
	
	st = String( ZULUCRYPTtempFolder"/open_tcrypt-" ) ;
	file = StringAppendInt( st,syscall( SYS_gettid ) ) ;
	fd = open( file,O_WRONLY | O_CREAT ) ;
	
	if( fd == -1 ){
		return zuluExit( 1,cd ) ;
	}

	write( fd,pass,pass_size ) ;
	close( fd ) ;
	
	memset( &params,'\0',sizeof( struct crypt_params_tcrypt ) ) ;

	params.keyfiles_count = 1 ;
	params.keyfiles       = &file ;
	params.flags          = CRYPT_TCRYPT_LEGACY_MODES ;
	
	xt = crypt_load( cd,CRYPT_TCRYPT,&params ) ;
	
	unlink( file ) ;
	StringDelete( &st ) ;
	
	if( xt != 0 ){
		return zuluExit( 1,cd ) ;
	}
	if( crypt_activate_by_volume_key( cd,mapper,NULL,0,flags ) == 0 ){
		return zuluExit( 0,cd ) ;
	}else{
		return zuluExit( 1,cd ) ;
	}
}

int zuluCryptOpenTcrypt( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{	
	/*
	 * An explanation for the below branches is explained in open_volume.c
	 */
	if( StringSize( mapper ) > 20 ){
		if( StringPrefixMatch( mapper,"zuluCryptKeyFromPass",20 ) ){
			return _tcrypt_open_using_key( device,mapper+20,mode,pass,pass_size ) ;
		}else if( StringPrefixMatch( mapper,"zuluCryptKeyFromFile",20 ) ){
			return _tcrypt_open_using_keyfile( device,mapper+20,mode,pass,pass_size ) ;
		}
	}
	return _tcrypt_open_using_key( device,mapper,mode,pass,pass_size ) ;
}
#else 

#if TRUECRYPT_TCPLAY

static int _tcrypt_open_using_key( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{
	tc_api_opts api_opts ;
	int r ;
	
	if( pass_size ){;}
	if( mode ){;}
	
	memset( &api_opts,'\0',sizeof( api_opts ) ) ;
	
	api_opts.tc_device      = device ;
	api_opts.tc_passphrase  = pass   ;
	api_opts.tc_map_name    = mapper ;
	
	if( tc_api_init( 0 ) == TC_OK ){
		r = tc_api_map_volume( &api_opts );
		tc_api_uninit() ;
		return r == TC_OK ? 0 : 1 ;
	}else{
		return 1 ;
	}
}


static int _tcrypt_open_using_keyfile( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{
	string_t st ;
	int fd ;
	const char * file ;
	const char * keyfiles[ 2 ] ;
	
	tc_api_opts api_opts ;
	int r = TC_ERR;
	
	if( pass_size ){;}
	if( mode ){;}
	
	/*
	 * ZULUCRYPTtempFolder is set in ../constants.h
	 */
	mkdir( ZULUCRYPTtempFolder,S_IRWXU ) ;
	chown( ZULUCRYPTtempFolder,0,0 ) ;
	
	st = String( ZULUCRYPTtempFolder"/open_tcrypt-" ) ;
	file = StringAppendInt( st,syscall( SYS_gettid ) ) ;
	fd = open( file,O_WRONLY | O_CREAT ) ;
	
	if( fd == -1 ){
		return 1 ;
	}
	
	write( fd,pass,pass_size ) ;
	close( fd ) ;
	
	memset( &api_opts,'\0',sizeof( api_opts ) ) ;
	api_opts.tc_device      = device ;
	api_opts.tc_map_name    = mapper ;
	api_opts.tc_keyfiles    = keyfiles ;
	keyfiles[ 0 ] = file ;
	keyfiles[ 1 ] = NULL ;
	
	if( tc_api_init( 0 ) == TC_OK ){
		r = tc_api_map_volume( &api_opts );
		tc_api_uninit() ;
	}
	
	unlink( file ) ;
	StringDelete( &st ) ;
	
	return r == TC_OK ? 0 : 1 ;
}

/*
 * 1 is returned if a volume is a truecrypt volume.
 * 0 is returned if a volume is not a truecrypt volume or functionality is not supported
 */
int zuluCryptVolumeIsTcrypt( const char * device,const char * key,size_t key_len )
{
	tc_api_opts api_opts ;
	tc_api_volinfo volinfo ;
	int r = TC_ERR;
	
	if( key_len ) {;}
	
	memset( &api_opts,'\0',sizeof( api_opts ) ) ;
	
	api_opts.tc_device      = device ;
	api_opts.tc_passphrase  = key   ;
	
	if( tc_api_init( 0 ) == TC_OK ){
		r = tc_api_info_volume( &api_opts,&volinfo ) ;
		tc_api_uninit() ;
	}
	return r == TC_OK ;
}

/*
 * 0 is returned if a volume was successfully opened.
 * 1 is returned if a volume was not successfully opened or functionality is not supported 
 */
int zuluCryptOpenTcrypt( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{	
	/*
	 * An explanation for the below branches is explained in open_volume.c
	 */
	if( StringSize( mapper ) > 20 ){
		if( StringPrefixMatch( mapper,"zuluCryptKeyFromPass",20 ) ){
			return _tcrypt_open_using_key( device,mapper+20,mode,pass,pass_size ) ;
		}else if( StringPrefixMatch( mapper,"zuluCryptKeyFromFile",20 ) ){
			return _tcrypt_open_using_keyfile( device,mapper+20,mode,pass,pass_size ) ;
		}
	}
	return _tcrypt_open_using_key( device,mapper,mode,pass,pass_size ) ;
}

#else
/*
 * 1 is returned if a volume is a truecrypt volume.
 * 0 is returned if a volume is not a truecrypt volume or functionality is not supported
 */
int zuluCryptVolumeIsTcrypt( const char * device,const char * key,size_t key_len )
{
	if( device ){;}
	if( key ){;}
	if( key_len ){;}
	return 0 ;
}

/*
 * 0 is returned if a volume was successfully opened.
 * 1 is returned if a volume was not successfully opened or functionality is not supported 
 */
int zuluCryptOpenTcrypt( const char * device,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{
	if( device ){;}
	if( mapper ){;}
	if( mode )  {;}
	if( pass )  {;}
	if( pass_size ) {;}
	return 1 ;
}
#endif
#endif
