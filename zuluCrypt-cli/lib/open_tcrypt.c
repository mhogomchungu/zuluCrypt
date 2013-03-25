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

static inline int zuluExit_1( int st,struct crypt_device * cd,string_t xt )
{
	crypt_free( cd );
	StringDelete( &xt ) ;
	return st ;
}

/*
 * 1 is returned if a volume is a truecrypt volume.
 * 0 is returned if a volume is not a truecrypt volume or functionality is not supported
 */
int zuluCryptVolumeIsTcrypt( const char * device,const char * key,int key_source )
{
	struct crypt_device * cd = NULL;
	struct crypt_params_tcrypt params ;
	
	memset( &params,'\0',sizeof( struct crypt_params_tcrypt ) ) ;
	
	if( key_source ){;}
	
	if( crypt_init( &cd,device ) < 0 ){
		return 0 ;
	}
	
	params.passphrase      = key ;
	params.passphrase_size = StringSize( key );
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

static int _tcrypt_open_using_key( const char * device,const char * mapper,unsigned long m_opts,const char * key,int volume_type )
{
	uint32_t flags = 0 ;
	
	struct crypt_device * cd = NULL;
	struct crypt_params_tcrypt params ;
	
	memset( &params,'\0',sizeof( struct crypt_params_tcrypt ) ) ;
	
	if( m_opts & MS_RDONLY ){
		flags |= CRYPT_ACTIVATE_READONLY;
	}
	if( volume_type == TCRYPT_HIDDEN ){
		flags |= CRYPT_TCRYPT_HIDDEN_HEADER ;
	}
	if( crypt_init( &cd,device ) < 0 ){
		return 1 ;
	}

	params.passphrase       = key ;
	params.passphrase_size  = StringSize( key ) ;
	params.flags            = CRYPT_TCRYPT_LEGACY_MODES ;
	
	if( volume_type == TCRYPT_HIDDEN ){
		params.flags |= CRYPT_TCRYPT_HIDDEN_HEADER ;
	}
	if( crypt_load( cd,CRYPT_TCRYPT,&params ) != 0 ){
		return zuluExit( 1,cd ) ;
	}
	if( crypt_activate_by_volume_key( cd,mapper,NULL,0,flags ) == 0 ){
		return zuluExit( 0,cd ) ;
	}else{
		return zuluExit( 1,cd ) ;
	}
}

static int _tcrypt_open_using_keyfile( const char * device,const char * mapper,unsigned long m_opts,const char * key,int volume_type )
{
	string_t st = StringVoid ;
	int xt ;
	int fd ;
	const char * file ;
	uint32_t flags = 0 ;
	
	struct crypt_device * cd = NULL;
	struct crypt_params_tcrypt params ;
	
	if( m_opts & MS_RDONLY ){
		flags |= CRYPT_ACTIVATE_READONLY;
	}
	if( volume_type == TCRYPT_HIDDEN ){
		flags |= CRYPT_TCRYPT_HIDDEN_HEADER ;
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
		return zuluExit_1( 1,cd,st ) ;
	}

	write( fd,key,StringSize( key ) ) ;
	close( fd ) ;
	
	chown( file,0,0 ) ;
	chmod( file,S_IRWXU ) ;
	
	memset( &params,'\0',sizeof( struct crypt_params_tcrypt ) ) ;

	params.keyfiles_count = 1 ;
	params.keyfiles       = &file ;
	params.flags          = CRYPT_TCRYPT_LEGACY_MODES ;
	
	if( volume_type == TCRYPT_HIDDEN ){
		params.flags |= CRYPT_TCRYPT_HIDDEN_HEADER ;
	}
	
	xt = crypt_load( cd,CRYPT_TCRYPT,&params ) ;
	
	unlink( file ) ;
	StringDelete( &st ) ;
	
	if( xt != 0 ){
		return zuluExit_1( 1,cd,st ) ;
	}
	if( crypt_activate_by_volume_key( cd,mapper,NULL,0,flags ) == 0 ){
		return zuluExit_1( 0,cd,st ) ;
	}else{
		return zuluExit_1( 1,cd,st ) ;
	}
}

int zuluCryptOpenTcrypt( const char * device,const char * mapper,const char *key, 
			 int key_source,int volume_type,const char * m_point,
			 uid_t id,unsigned long m_opts,const char * fs_opts )
{	
	int h ;
	string_t p ;
	string_t q ;
	const char * mapper_1 ;	
	int fd ;
	int lmode ;
	
	if( m_opts & MS_RDONLY ){
		lmode = O_RDONLY ;
	}else{
		lmode = O_RDWR ;
	}
	if( StringPrefixMatch( device,"/dev/",5 ) ){
		if( key_source == TCRYPT_KEYFILE ){
			h = _tcrypt_open_using_keyfile( device,mapper,m_opts,key,volume_type ) ;
		}else{
			h = _tcrypt_open_using_key( device,mapper,m_opts,key,volume_type ) ;
		}
	}else{
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in create_loop_device.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( device,lmode,&fd,&q ) ){
			device = StringContent( q ) ;
			if( key_source == TCRYPT_KEYFILE ){
				h = _tcrypt_open_using_keyfile( device,mapper,m_opts,key,volume_type ) ;
			}else{
				h = _tcrypt_open_using_key( device,mapper,m_opts,key,volume_type ) ;
			}
			close( fd ) ;
			StringDelete( &q ) ;
		}else{
			h = 1 ;
		}
	}
	
	if( h == 0 ){
		if( m_point != NULL ){
			p = String( crypt_get_dir() ) ;
			mapper_1 = StringMultipleAppend( p,"/",mapper,END ) ;
			/*
			 * zuluCryptMountVolume() is defined in mount_volume.c
			 */
			h = zuluCryptMountVolume( mapper_1,m_point,m_opts,fs_opts,id ) ;
			if( h != 0 ){
				/*
				 * zuluCryptCloseMapper() is defined in close_mapper.c
				 */
				if( zuluCryptCloseMapper( mapper_1 ) != 0 ){
					h = 15 ;
				}
			}
			StringDelete( &p ) ;
		}
	}
	return h ;
}
#else 

#if TRUECRYPT_TCPLAY

/*
 * TRUECRYPT_TCPLAY functionality is not enabled,this code is broken and doesnt compile
 * We are living it here just in case we want to use the functionality in the future
 */
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


static int _tcrypt_open_using_keyfile( tcrpt_str * str )
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
	
	write( fd,str->passphrase,StringSize( str->passphrase ) ;
	close( fd ) ;
	
	memset( &api_opts,'\0',sizeof( api_opts ) ) ;
	api_opts.tc_device      = str->device ;
	api_opts.tc_map_name    = str->mapping_name ;
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
int zuluCryptVolumeIsTcrypt( const char * device,const char * key,int key_source )
{
	tc_api_opts api_opts ;
	tc_api_volinfo volinfo ;
	int r = TC_ERR;
	
	if( key_len ) {;}
	
	memset( &api_opts,'\0',sizeof( api_opts ) ) ;
	
	api_opts.tc_device      = str->device ;
	api_opts.tc_passphrase  = str->passphrase   ;
	
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
int zuluCryptVolumeIsTcrypt( const char * device,const char * key,int key_source )
{	
	if( str->key_files == NULL ){
		return _tcrypt_open_using_key( str ) ;
	}else{
		return _tcrypt_open_using_keyfile( str ) ;
	}
}

#else
/*
 * 1 is returned if a volume is a truecrypt volume.
 * 0 is returned if a volume is not a truecrypt volume or functionality is not supported
 */
int zuluCryptVolumeIsTcrypt( const char * device,const char * key,int key_source )
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
int zuluCryptOpenTcrypt( const char * device,const char * mapper,const char * mode,const char * key,int key_source ) ;
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
