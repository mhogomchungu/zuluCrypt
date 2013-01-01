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

static inline const char * _mapping_name( const char * m )
{
	const char * e = strrchr( m,'/' ) ;
	return e == NULL ? m : e + 1 ;
}

int zuluMountCryptoMount( const char * device,const char * mode,uid_t uid,
			  const char * key,const char * key_source,
			  const char * m_point,int mount_point_option,
			  stringList_t stx 
			)
{
	int st ;
	/*
	 * the struct is declared in ../zuluCrypt-cli/bin/libzuluCrypt-exe.h
	 */
	struct_opts opts ;
	
	const char * mapping_name ;
	char * path = NULL ;
	
	if( strncmp( device,"/dev/loop",9 ) == 0 ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ../zuluCrypt-cli/create_loop_device.c
		 */
		path = zuluCryptLoopDeviceAddress( device ) ;
		if( path == NULL ){
			return 20 ;
		}else{
			mapping_name = _mapping_name( path ) ;
		}
	}else{
		mapping_name = _mapping_name( device ) ;
	}
	
	if( key_source != NULL ){
		if( strcmp( key_source,"-G" ) == 0 ){
			opts.plugin_path = key ;
		}
	}
	
	/*
	 * zuluCryptEXEGetOptsSetDefault() is defined in ../zuluCrypt-cli/bin/get_opts.c
	 */
	zuluCryptEXEGetOptsSetDefault( &opts ) ;
	
	opts.mount_point = m_point ;
	opts.open_no_mount = -1 ;
	opts.device = device ;
	opts.m_opts = mode ;
	opts.key = key ;
	opts.key_source = key_source ;
	opts.mount_point_option = mount_point_option ;
	opts.env = StringListStringArray( stx ) ;
	
	/*
	 * zuluCryptEXEOpenVolume() is defined in ../zuluCrypt-cli/bin/open_volume.c
	 */
	st = zuluCryptEXEOpenVolume( &opts,mapping_name,uid ) ;
	
	if( path != NULL ){
		free( path ) ;
	}
	free( ( char * )opts.env ) ;
	return st ;
}
