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
#include <libcryptsetup.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 * this header is created at config time
 */
#include "truecrypt_support.h"

#if TRUECRYPT_CRYPTSETUP

static inline int zuluExit( int st,struct crypt_device * cd )
{
	crypt_free( cd ) ;
	return st ;
}

static inline int zuluExit_1( int st,struct crypt_device * cd,string_t xt )
{
	crypt_free( cd ) ;
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
	}else{
		params.passphrase      = key ;
		params.passphrase_size = StringSize( key );
		params.flags           = CRYPT_TCRYPT_LEGACY_MODES ;

		if( crypt_load( cd,CRYPT_TCRYPT,&params ) == 0 ){
			return zuluExit( 1,cd ) ;
		}else{
			return zuluExit( 0,cd ) ;
		}
	}
}

/*
 * 0 is returned if a volume was successfully opened.
 * 1 is returned if a volume was not successfully opened or functionality is not supported
 */

static int _tcrypt_open_using_key( const char * device,const char * mapper,unsigned long m_opts,
				   const char * key,size_t key_len,int volume_type,int system_header )
{
	uint32_t flags = 0 ;

	struct crypt_device * cd = NULL ;
	struct crypt_params_tcrypt params ;

	if( crypt_init( &cd,device ) < 0 ){
		return 1 ;
	}else{
		memset( &params,'\0',sizeof( struct crypt_params_tcrypt ) ) ;

		params.passphrase       = key ;
		params.passphrase_size  = key_len ;
		params.flags            = system_header ;

		if( volume_type == TCRYPT_HIDDEN ){
			params.flags |= CRYPT_TCRYPT_LEGACY_MODES | CRYPT_TCRYPT_HIDDEN_HEADER ;
		}else{
			params.flags |= CRYPT_TCRYPT_LEGACY_MODES ;
		}
		if( crypt_load( cd,CRYPT_TCRYPT,&params ) != 0 ){
			return zuluExit( 1,cd ) ;
		}
		if( m_opts & MS_RDONLY ){
			flags |= CRYPT_ACTIVATE_READONLY ;
		}
		if( crypt_activate_by_volume_key( cd,mapper,NULL,0,flags ) == 0 ){
			return zuluExit( 0,cd ) ;
		}else{
			return zuluExit( 1,cd ) ;
		}
	}
}

static int _tcrypt_open_using_keyfile( const char * device,const char * mapper,unsigned long m_opts,
				       const char * key,size_t key_len,int volume_type,int system_header )
{
	string_t st = StringVoid ;
	int xt ;
	int fd ;
	const char * file ;
	uint32_t flags = 0 ;

	struct stat statstr ;

	struct crypt_device * cd = NULL;
	struct crypt_params_tcrypt params ;

	if( crypt_init( &cd,device ) < 0 ){
		return 1 ;
	}

	#define path_does_not_exist( x ) stat( x,&statstr ) != 0

	if( path_does_not_exist( "/run" ) ){
		mkdir( "/run",S_IRWXU ) ;
		chown( "/run",0,0 ) ;
	}
	if( path_does_not_exist( "/run/zuluCrypt" ) ){
		mkdir( "/run/zuluCrypt",S_IRWXU ) ;
		chown( "/run/zuluCrypt",0,0 ) ;
	}

	st = String( "/run/zuluCrypt/open_tcrypt-" ) ;
	file = StringAppendInt( st,syscall( SYS_gettid ) ) ;
	fd = open( file,O_WRONLY | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP |S_IROTH ) ;

	if( fd == -1 ){
		return zuluExit_1( 1,cd,st ) ;
	}

	write( fd,key,key_len ) ;
	close( fd ) ;

	chown( file,0,0 ) ;
	chmod( file,S_IRWXU ) ;

	memset( &params,'\0',sizeof( struct crypt_params_tcrypt ) ) ;

	params.keyfiles_count = 1 ;
	params.keyfiles       = &file ;
	params.flags          = system_header ;

	if( volume_type == TCRYPT_HIDDEN ){
		params.flags |= CRYPT_TCRYPT_LEGACY_MODES | CRYPT_TCRYPT_HIDDEN_HEADER ;
	}else{
		params.flags |= CRYPT_TCRYPT_LEGACY_MODES ;
	}

	xt = crypt_load( cd,CRYPT_TCRYPT,&params ) ;

	/*
	 * zuluCryptDeleteFile() is defined in open_path_security.c
	 */
	zuluCryptDeleteFile( file ) ;

	if( xt != 0 ){
		return zuluExit_1( 1,cd,st ) ;
	}
	if( m_opts & MS_RDONLY ){
		flags |= CRYPT_ACTIVATE_READONLY;
	}
	if( crypt_activate_by_volume_key( cd,mapper,NULL,0,flags ) == 0 ){
		return zuluExit_1( 0,cd,st ) ;
	}else{
		return zuluExit_1( 1,cd,st ) ;
	}
}

static int _tcrypt_open_using_keyfile_1( const char * device,const char * mapper,unsigned long m_opts,
				       const char * keyfile,int volume_type,int system_header )
{
	uint32_t flags = 0 ;

	struct crypt_device * cd = NULL;
	struct crypt_params_tcrypt params ;

	if( crypt_init( &cd,device ) < 0 ){
		return 1 ;
	}else{
		memset( &params,'\0',sizeof( struct crypt_params_tcrypt ) ) ;

		params.keyfiles_count = 1 ;
		params.keyfiles       = &keyfile ;
		params.flags          = system_header ;

		if( volume_type == TCRYPT_HIDDEN ){
			params.flags |= CRYPT_TCRYPT_LEGACY_MODES | CRYPT_TCRYPT_HIDDEN_HEADER ;
		}else{
			params.flags |= CRYPT_TCRYPT_LEGACY_MODES ;
		}
		if( crypt_load( cd,CRYPT_TCRYPT,&params ) != 0 ){
			return zuluExit( 1,cd ) ;
		}
		if( m_opts & MS_RDONLY ){
			flags |= CRYPT_ACTIVATE_READONLY;
		}
		if( crypt_activate_by_volume_key( cd,mapper,NULL,0,flags ) == 0 ){
			return zuluExit( 0,cd ) ;
		}else{
			return zuluExit( 1,cd ) ;
		}
	}
}

static int _open_tcrypt( const char * device,const char * mapper,unsigned long m_opts,
			 const char * key,size_t key_len,int volume_type,int key_source )
{
	int h ;
	if( key_source == TCRYPT_KEYFILE ){
		h = _tcrypt_open_using_keyfile( device,mapper,m_opts,key,key_len,volume_type,0 ) ;
	}else if( key_source == TCRYPT_KEYFILE_FILE ){
		h = _tcrypt_open_using_keyfile_1( device,mapper,m_opts,key,volume_type,0 ) ;
	}else{
		h = _tcrypt_open_using_key( device,mapper,m_opts,key,key_len,volume_type,0 ) ;
	}

	if( h != 0 ){
		/*
		 * failed to opened the volume,assuming its because its a system volume and try again
		 */
		if( key_source == TCRYPT_KEYFILE ){
			h = _tcrypt_open_using_keyfile( device,mapper,m_opts,key,key_len,volume_type,CRYPT_TCRYPT_SYSTEM_HEADER ) ;
		}else if( key_source == TCRYPT_KEYFILE_FILE ){
			h = _tcrypt_open_using_keyfile_1( device,mapper,m_opts,key,volume_type,CRYPT_TCRYPT_SYSTEM_HEADER ) ;
		}else{
			h = _tcrypt_open_using_key( device,mapper,m_opts,key,key_len,volume_type,CRYPT_TCRYPT_SYSTEM_HEADER ) ;
		}
	}

	return h ;
}

int zuluCryptOpenTcrypt( const char * device,const char * mapper,const char * key,size_t key_len,
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
		h = _open_tcrypt( device,mapper,m_opts,key,key_len,volume_type,key_source ) ;
	}else{
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in create_loop_device.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( device,lmode,&fd,&q ) ){
			device = StringContent( q ) ;

			h = _open_tcrypt( device,mapper,m_opts,key,key_len,volume_type,key_source ) ;

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

/*
 * We wil get here if truecrypt support is not enabled through cryptsetup or tcplay
 */

/*
 * 1 is returned if a volume is a truecrypt volume.
 * 0 is returned if a volume is not a truecrypt volume or functionality is not supported
 */
int zuluCryptVolumeIsTcrypt( const char * device,const char * key,int key_source )
{
	if( 0 && device && key && key_source ){;}
	return 0 ;
}

/*
 * 0 is returned if a volume was successfully opened.
 * 1 is returned if a volume was not successfully opened or functionality is not supported
 */
int zuluCryptOpenTcrypt( const char * device,const char * mapper,const char * key,size_t key_len,
			 int key_source,int volume_type,const char * m_point,
			 uid_t id,unsigned long m_opts,const char * fs_opts )
{
	if( 0 && device && mapper && key && key_source && volume_type
		&& m_point && id && m_opts && fs_opts && key_len ){;}
	return 1 ;
}

#endif
