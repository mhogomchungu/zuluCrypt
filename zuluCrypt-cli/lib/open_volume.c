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

/*
 * here,we check if the path we send to cryptsetup is the one cryptsetup used to create
 * a mapper.The check is there to guard against the underlying device being changed under us
 * in an attempt to exploit an suid program using the library
 */
static inline int _device_is_not_sane( const char * device,const char * mapper )
{
	int st = 1 ;
	/*
	 * zuluCryptVolumeDeviceName() is defined in ./status.c
	 */
	char * dev = zuluCryptVolumeDeviceName( mapper ) ;
	char * dev_1 ;
	
	if( dev == NULL ){
		return 1 ;
	}
	if( StringPrefixMatch( device,"/dev/loop",9 ) ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in create_loop_device.c
		 */
		dev_1 = zuluCryptLoopDeviceAddress( device ) ;
		if( dev_1 != NULL ){
			st = strcmp( dev_1,dev ) ;
			free( dev_1 ) ;
		}
	}else{
		st = strcmp( device,dev ) ;
	}
	free( dev ) ;
	return st != 0 ;
}

static int _open_mapper( const char * dev,const char * map,const char * mode,const char * pass,size_t pass_size )
{	
	int st ;
	/*
	 * zuluCryptVolumeIsLuks() is defined in is_luks.c
	 */
	if( zuluCryptVolumeIsLuks( dev ) ){
		/*
		 * zuluCryptOpenLuks() is defined in open_luks.c
		 */
		st = zuluCryptOpenLuks( dev,map,mode,pass,pass_size ) ; 
	}else{
		/*
		 * zuluCryptOpenTcrypt() is defined in open_tcrypt.c
		 */
		st = zuluCryptOpenTcrypt( dev,map,mode,pass,pass_size ) ;
		if( st != 0 ){
			/*
			 * zuluCryptOpenPlain() is defined in open_plain.c
			 */
			st = zuluCryptOpenPlain( dev,map,mode,pass,pass_size ) ; 
		}
	}
	return st ;
}

int zuluCryptOpenVolume( const char * dev,const char * map,
			 const char * m_point,uid_t id,unsigned long m_opts,
			 const char * fs_opts,const char * pass,size_t pass_size ) 
{
	int h ;
	string_t p = StringVoid ;
	string_t q = StringVoid ;
	int lmode ;
	int fd ;
	const char * mapper ;
	const char * mode ;
		
	/*
	 * zuluCryptPathIsNotValid() is defined in is_path_valid.c
	 */
	if( zuluCryptPathIsNotValid( dev ) ){
		return 3 ;
	}
	
	p = String( crypt_get_dir() ) ;
	
	mapper = StringMultipleAppend( p,"/",map,END ) ;

	/*
	 * zuluCryptPathIsValid() is defined in is_path_valid.c
	 */
	if( zuluCryptPathIsValid( mapper ) ){
		return zuluExit( 2,p ) ;
	}
	if( m_opts & MS_RDONLY ){
		lmode = O_RDONLY ;
		mode = "ro" ;
	}else{
		lmode = O_RDWR ;
		mode = "rw" ;
	}
	
	if( !StringPrefixMatch( dev,"/dev/",5 ) ){
		/*
		 * zuluCryptAttachLoopDeviceToFile() is defined in create_loop_device.c
		 */
		if( zuluCryptAttachLoopDeviceToFile( dev,lmode,&fd,&q ) ){
			dev = StringContent( q ) ;
			h = _open_mapper( dev,map,mode,pass,pass_size ) ;
			close( fd ) ;
			StringDelete( &q ) ;
		}else{
			h = 1 ;
		}
	}else{
		h = _open_mapper( dev,map,mode,pass,pass_size ) ;
	}
	
	switch( h ){
		case 1 : return zuluExit( 4,p ) ;
		case 2 : return zuluExit( 8,p ) ; 
		case 3 : return zuluExit( 3,p ) ;
	}
	
	if( h == 0 ){
		if( _device_is_not_sane( dev,mapper ) ){
			zuluCryptCloseMapper( map ) ;
			return zuluExit( -1,p ) ;
		}
	}
	
	if( m_point != NULL ){
		/*
		 * zuluCryptMountVolume() is defined in mount_volume.c
		 */
		h = zuluCryptMountVolume( mapper,m_point,m_opts,fs_opts,id ) ;
	
		if( h != 0 ){
			/*
			 * zuluCryptCloseMapper() is defined in close_mapper.c
			 */
			if( zuluCryptCloseMapper( map ) != 0 ){
				h = 15 ;
			}
		}
	}
	
	return zuluExit( h,p ) ;
}
