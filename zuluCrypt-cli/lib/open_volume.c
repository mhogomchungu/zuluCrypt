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

static inline int zuluExit( int x,string_t p )
{
	StringDelete( &p ) ;
	return x ;
}

static int _open_mapper( const char * dev,const char * mapper,const char * mode,const char * pass,size_t pass_size )
{	
	int st ;
	/*
	 * zuluCryptVolumeIsLuks() is defined in is_luks.c
	 */
	if( zuluCryptVolumeIsLuks( dev ) ){
		/*
		 * zuluCryptOpenLuks() is defined in open_luks.c
		 */
		st = zuluCryptOpenLuks( dev,mapper,mode,pass,pass_size ) ; 
	}else{
		/*
		 * zuluCryptOpenPlain() is defined in open_plain.c
		 */
		st = zuluCryptOpenPlain( dev,mapper,mode,pass,pass_size ) ;
	}
	return st ;
}

int zuluCryptOpenVolume( const char * dev,const char * mapper,
			 const char * m_point,uid_t id,unsigned long m_opts,
			 const char * fs_opts,const char * pass,size_t pass_size ) 
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
	
	p = String( crypt_get_dir() ) ;
	
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
		h = _open_mapper( dev,mapper,mode,pass,pass_size ) ;
	}else{
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in create_loop_device.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( dev,lmode,&fd,&q ) ){
			dev = StringContent( q ) ;
			h = _open_mapper( dev,mapper,mode,pass,pass_size ) ;
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
				puts( "oo" ) ;
				puts( mapper ) ;
				h = 15 ;
			}
		}
	}
	
	return zuluExit( h,p ) ;
}
