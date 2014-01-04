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

static inline int _open_mapper( const char * dev,const char * offset,const char * mapper,
				const char * mode,const char * pass,size_t pass_size,int x,int y )
{
	/*
	 * zuluCryptOpenLuks() is defined in open_luks.c
	 */
	int st = zuluCryptOpenLuks( dev,mapper,mode,pass,pass_size ) ;

	if( st == 2 ){
		/*
		 * volume is not a LUKS volume,assume its a PLAIN volume
		 */
		/*
		 * zuluCryptOpenPlain_1() is defined in open_plain.c
		 */
		return zuluCryptOpenPlain_1( dev,offset,mapper,mode,pass,pass_size,x,y ) ;
	}else{
		return st ;
	}
}

int zuluCryptOpenVolume_0( int( *function )( const char *,const char *,const char *,const char *,const char *,size_t,int,int ),
			   const char * dev,const char * offset,const char * mapper,
			   const char * m_point,uid_t id,unsigned long m_opts,
			   const char * fs_opts,const char * pass,size_t pass_size,int volume_type,int key_source )
{
	int h ;
	string_t p = StringVoid ;
	string_t q = StringVoid ;
	int lmode ;
	int fd ;
	const char * mode ;
	const char * mapper_1 ;
	/*
	 * zuluCryptPathIsNotValid() is defined in is_path_valid.c
	 */
	if( zuluCryptPathIsNotValid( dev ) ){
		return 3 ;
	}

	/*
	 * zuluCryptMapperPrefix() is defined in create_mapper_name.c
	 */
	p = String( zuluCryptMapperPrefix() ) ;

	mapper_1 = StringMultipleAppend( p,"/",mapper,END ) ;

	/*
	 * zuluCryptPathIsValid() is defined in is_path_valid.c
	 */
	if( zuluCryptPathIsValid( mapper_1 ) ){
		return zuluExit( 2,p ) ;
	}
	if( m_opts & MS_RDONLY ){
		lmode = O_RDONLY ;
		mode = "ro" ;
	}else{
		lmode = O_RDWR ;
		mode = "rw" ;
	}

	if( StringPrefixMatch( dev,"/dev/",5 ) ){
		h = function( dev,offset,mapper,mode,pass,pass_size,volume_type,key_source ) ;
	}else{
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in create_loop_device.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( dev,lmode,&fd,&q ) ){
			dev = StringContent( q ) ;
			h = function( dev,offset,mapper,mode,pass,pass_size,volume_type,key_source ) ;
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

	if( m_point != NULL ){
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
	}

	return zuluExit( h,p ) ;
}


int zuluCryptOpenVolume( const char * dev,const char * mapper,
			 const char * m_point,uid_t id,unsigned long m_opts,
			 const char * fs_opts,const char * pass,size_t pass_size )
{
	return zuluCryptOpenVolume_0( _open_mapper,dev,NULL,mapper,m_point,id,m_opts,fs_opts,pass,pass_size,0,0 ) ;
}

int zuluCryptOpenPlainWithOffset( const char * dev,const char * offset,const char * mapper,
				  const char * m_point,uid_t id,unsigned long m_opts,
				  const char * fs_opts,const char * pass,size_t pass_size )
{
	/*
	 * zuluCryptOpenPlain_1() is defined in open_plain.c
	 */
	return zuluCryptOpenVolume_0( zuluCryptOpenPlain_1,dev,offset,mapper,m_point,id,m_opts,fs_opts,pass,pass_size,0,0 ) ;
}
