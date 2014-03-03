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
#include <fcntl.h>
#include <sys/mount.h>
#include <unistd.h>

static inline int zuluExit( int x,string_t p )
{
	StringDelete( &p ) ;
	return x ;
}

static int _open_mapper( const open_struct_t * opts )
{
	/*
	 * zuluCryptOpenLuks() is defined in open_luks.c
	 */
	int st = zuluCryptOpenLuks( opts->device,opts->mapper_name,opts->m_opts,opts->key,opts->key_len ) ;

	if( st == 2 ){
		/*
		 * volume is not a LUKS volume,assume its a PLAIN volume
		 */
		/*
		 * zuluCryptOpenPlain_1() is defined in open_plain.c
		 */
		return zuluCryptOpenPlain_1( opts ) ;
	}else{
		return st ;
	}
}

int zuluCryptOpenVolume_0( int( *function )( const open_struct_t * ),const open_struct_t * opts )
{
	int h ;
	string_t p = StringVoid ;
	string_t q = StringVoid ;
	int mode ;
	int fd ;
	const char * mapper_1 ;
	open_struct_t opts_1 ;

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

	mapper_1 = StringMultipleAppend( p,"/",opts->mapper_name,END ) ;

	/*
	 * zuluCryptPathIsValid() is defined in is_path_valid.c
	 */
	if( zuluCryptPathIsValid( mapper_1 ) ){
		return zuluExit( 2,p ) ;
	}
	if( opts->m_flags & MS_RDONLY ){
		mode = O_RDONLY ;
	}else{
		mode = O_RDWR ;
	}

	if( StringPrefixMatch( opts->device,"/dev/",5 ) ){
		h = function( opts ) ;
	}else{
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in create_loop_device.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( opts->device,mode,&fd,&q ) ){
			memcpy( &opts_1,opts,sizeof( open_struct_t ) ) ;
			opts_1.device = StringContent( q ) ;
			h = function( &opts_1 ) ;
			close( fd ) ;
			StringDelete( &q ) ;
		}else{
			h = 1 ;
		}
	}

	switch( h ){
		case 1 : return zuluExit( 4,p ) ;
		case 2 : return zuluExit( 8,p ) ;
		case 3 : return zuluExit( 3,p ) ;
	}

	if( opts->m_point != NULL ){
		/*
		 * zuluCryptMountVolume() is defined in mount_volume.c
		 */
		h = zuluCryptMountVolume( mapper_1,opts->m_point,opts->m_flags,opts->fs_opts,opts->uid ) ;

		if( h != 0 ){
			/*
			 * zuluCryptCloseMapper() is defined in close_mapper.c
			 */
			if( zuluCryptCloseMapper( mapper_1 ) != 0 ){
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

int zuluCryptOpenPlainWithOffset( const open_struct_t * opts )
{
	/*
	 * zuluCryptOpenPlain_1() is defined in open_plain.c
	 */
	return zuluCryptOpenVolume_0( zuluCryptOpenPlain_1,opts ) ;
}

/*
 * this function tries to unlock luks and plain volumes only
 */
int zuluCryptOpenVolume_1( const open_struct_t * opts )
{
	return zuluCryptOpenVolume_0( _open_mapper,opts ) ;
}

/*
 * this function tries to unlock luks,plain and truecrypt volumes
 */
int zuluCryptOpenVolume_2( const open_struct_t * opts )
{
	int r ;
	open_struct_t opts_1 ;
	if( opts->offset != NULL ){
		r = zuluCryptOpenPlainWithOffset( opts ) ;
	}else{
		r = zuluCryptOpenVolume_1( opts ) ;
		if( r == 4 ){
			memcpy( &opts_1,opts,sizeof( open_struct_t ) ) ;
			/*
			 * try to open the volume as a normal TRUECRYPT volume.
			 */
			opts_1.volume_type = TCRYPT_NORMAL ;
			/*
			 * zuluCryptOpenTcrypt_1() is defined in open_tcrypt.c
			 */
			r = zuluCryptOpenTcrypt_1( &opts_1 ) ;
			if( r != 0 ){
				/*
				 * retry to open the volume as a hidden TRUECRYPT volume.
				 */
				opts_1.volume_type = TCRYPT_HIDDEN ;
				r = zuluCryptOpenTcrypt_1( &opts_1 ) ;
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
