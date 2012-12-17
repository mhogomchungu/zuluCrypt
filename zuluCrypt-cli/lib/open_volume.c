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
	int st ;
	char * dev = zuluCryptVolumeDeviceName( mapper ) ;
	if( dev == NULL )
		return 1 ;
	st = strcmp( device,dev ) ;
	free( dev ) ;
	return st != 0 ;
}

static int _open_mapper( const char * dev,const char * map,const char * mode,const char * pass,size_t pass_size )
{	
	/*
	 * zuluCryptOpenLuks()   is defined in open_luks.c
	 * zuluCryptOpenTcrypt() is defined in open_tcrypt.c
	 * zuluCryptOpenPlain()  is defined in open_plain.c
	 * zuluCryptGetVolumeType() is defined in volume_type.c
	 */
	switch( zuluCryptGetVolumeType( dev,pass,pass_size ) ){
		case 1 : return zuluCryptOpenLuks(   dev,map,mode,pass,pass_size ) ; 
		case 2 : return zuluCryptOpenTcrypt( dev,map,mode,pass,pass_size ) ; 
		default: return zuluCryptOpenPlain(  dev,map,mode,pass,pass_size ) ; 
	}
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
	if( zuluCryptPathIsNotValid( dev ) ) 
		return 3 ;
	
	p = String( crypt_get_dir() ) ;
	
	mapper = StringMultipleAppend( p,"/",map,END ) ;

	/*
	 * zuluCryptPathIsValid() is defined in is_path_valid.c
	 */
	if( zuluCryptPathIsValid( mapper ) )
		return zuluExit( 2,p ) ;

	if( m_opts & MS_RDONLY ){
		lmode = O_RDONLY ;
		mode = "ro" ;
	}else{
		lmode = O_RDWR ;
		mode = "rw" ;
	}
	
	if( strncmp( dev,"/dev/",5 ) != 0 ){
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
		if( _device_is_not_sane( dev,mapper ) )
			return zuluExit( -1,p ) ;
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

