/*
 *
 *  Copyright (c) 2011
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
#include <fcntl.h>
#include <sys/mount.h>
#include <unistd.h>

static inline int zuluExit( int x,string_t p )
{
	StringDelete( &p ) ;
	return x ;
}

int zuluCryptOpenVolume_0( int( *function )( const open_struct_t * ),const open_struct_t * opts )
{
	int h ;

	string_t p ;

	const char * mapper ;

	/*
	 * zuluCryptPathIsNotValid() is defined in is_path_valid.c
	 */
	if( zuluCryptPathIsNotValid( opts->device ) ){
		return 3 ;
	}
	/*
	 * zuluCryptMapperPrefix() is defined in create_mapper_name.c
	 */
	p = String( zuluCryptMapperPrefix() ) ;

	mapper = StringMultipleAppend( p,"/",opts->mapper_name,NULL ) ;

	/*
	 * zuluCryptPathIsValid() is defined in is_path_valid.c
	 */
	if( zuluCryptPathIsValid( mapper ) ){
		return zuluExit( 2,p ) ;
	}

	h = function( opts ) ;

	switch( h ){
		case 1 : return zuluExit( 4,p ) ;
		case 2 : return zuluExit( 8,p ) ;
		case 3 : return zuluExit( 3,p ) ;
	}

	if( opts->m_point != NULL ){
		/*
		 * zuluCryptMountVolume() is defined in mount_volume.c
		 */
		h = zuluCryptMountVolume( mapper,opts->m_point,opts->m_flags,opts->fs_opts,opts->uid ) ;

		if( h != 0 ){
			/*
			 * zuluCryptCloseMapper() is defined in close_mapper.c
			 */
			if( zuluCryptCloseMapper( mapper ) != 0 ){
				h = 15 ;
			}
		}
	}

	return zuluExit( h,p ) ;
}

int zuluCryptOpenVolume( const char * dev,const char * mapper,const char * m_point,uid_t uid,
			 unsigned long m_flags,const char * fs_opts,const char * key,size_t key_len )
{
	open_struct_t opts ;

	memset( &opts,'\0',sizeof( open_struct_t ) ) ;

	opts.device      = dev ;
	opts.mapper_name = mapper ;
	opts.m_point     = m_point ;
	opts.uid         = uid ;
	opts.m_flags     = m_flags ;
	opts.fs_opts     = fs_opts ;
	opts.key         = key ;
	opts.key_len     = key_len ;

	if( m_flags & MS_RDONLY ){
		opts.m_opts = "ro" ;
	}else{
		opts.m_opts = "rw" ;
	}

	return zuluCryptOpenVolume_1( &opts ) ;
}

static int _open_mapper( const open_struct_t * opts )
{
	int r ;
	/*
	 * zuluCryptVolumeIsLuks() is defined in is_luks.c
	 */
	if( opts->luks_detached_header ){

		/*
		 * zuluCryptOpenLuks() is defined in open_luks.c
		 */
		r = zuluCryptOpenLuks_1( opts ) ;
		
	}else if( zuluCryptVolumeIsLuks( opts->device ) ){

		if( opts->veraCrypt_volume ){
			/*
			 * an attempt was made to unlock a volume as a VeraCrypt volume
			 * but the volume is a LUKS volume.
			 */
			return 1 ;
		}
		/*
		 * zuluCryptOpenLuks_2() is defined in open_luks.c
		 */
		r = zuluCryptOpenLuks_2( opts ) ;
		if( r != 0 ){
			/*
			 * just assumed wrong password when a volume fail to unlock
			 */
			r = 1 ;
		}
	}else{
		/*
		 * zuluCryptOpenPlain_1() is defined in open_plain.c
		 */
		r = zuluCryptOpenPlain_1( opts ) ;
	}

	return r ;
}

/*
 * this function tries to unlock luks and plain volumes only
 */
int zuluCryptOpenVolume_1( const open_struct_t * opts )
{
	return zuluCryptOpenVolume_0( _open_mapper,opts ) ;
}

static int _open_tcrypt_0( open_struct_t * opts,int system )
{
	int r ;

	opts->volume_type = TCRYPT_NORMAL ;

	opts->tcrypt_system = system ;
	/*
	 * zuluCryptOpenTcrypt_1() is defined in open_tcrypt.c
	 */
	r = zuluCryptOpenTcrypt_1( opts ) ;

	if( r != 0 ){

		opts->volume_type = TCRYPT_HIDDEN ;
		r = zuluCryptOpenTcrypt_1( opts ) ;
	}
	return r ;
}

static int _open_tcrypt( open_struct_t * opts )
{
	if( _open_tcrypt_0( opts,0 ) ){

		return _open_tcrypt_0( opts,1 ) ;
	}else{
		return 0 ;
	}
}

/*
 * this function tries to unlock luks,plain and truecrypt volumes
 */
int zuluCryptOpenVolume_2( const open_struct_t * opts )
{
	int r ;
	open_struct_t opts_1 ;
	string_t zt = StringVoid ;
	const char * keyfile ;

	if( opts->offset != NULL ){
		/*
		 * zuluCryptOpenPlain_1() is defined in open_plain.c
		 */
		r = zuluCryptOpenVolume_0( zuluCryptOpenPlain_1,opts ) ;
	}else{
		r = zuluCryptOpenVolume_1( opts ) ;

		if( r == 4 && zuluCryptVolumeIsNotLuks( opts->device ) ){

			memcpy( &opts_1,opts,sizeof( open_struct_t ) ) ;

			if( opts_1.key_source == TCRYPT_KEYFILE ){
				/*
				 * zuluCryptCreateKeyFile() is defined in open_tcrypt.c
				 */
				zt = zuluCryptCreateKeyFile( opts_1.key,opts_1.key_len,"keyfile" ) ;

				if( zt != StringVoid ){
					keyfile = StringContent( zt ) ;
					opts_1.tcrypt_keyfiles_count = 1 ;
					opts_1.tcrypt_keyfiles       = &keyfile ;
					opts_1.key = "" ;
					opts_1.key_len = 0 ;

					r = _open_tcrypt( &opts_1 ) ;
					/*
					 * zuluCryptDeleteFile() is defined in file_path_security.c
					 */
					zuluCryptDeleteFile( keyfile ) ;
					StringDelete( &zt ) ;
				}else{
					r = -1 ;
				}
			}else{
				r = _open_tcrypt( &opts_1 ) ;
			}
		}
		if( r == 0 || r == -1 ){
			;
		}else{
			r = 4 ;
		}
	}
	return r ;
}
