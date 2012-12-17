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

int zuluMountCryptoMount( const char * device,const char * mode,uid_t uid,
				  const char * key,const char * key_source,
				  const char * m_point,int mount_point_option )
{
	int st ;
	/*
	 * the struct is declared in ../zuluCrypt-cli/bin/libzuluCrypt-exe.h
	 */
	struct_opts opts ;
	
	const char * mapping_name ;
	const char * e = strrchr( device,'/' ) ;
	
	/*
	 * zuluCryptEXEGetOptsSetDefault() is defined in ../zuluCrypt-cli/bin/get_opts.c
	 */
	zuluCryptEXEGetOptsSetDefault( &opts ) ;
	
	if( e == NULL)
		mapping_name = device ;
	else
		mapping_name = e + 1 ;
	
	if( key_source != NULL ){
		if( strcmp( key_source,"-G" ) == 0 ){
			opts.plugin_path = key ;
		}
	}
		
	opts.mount_point = m_point ;
	opts.open_no_mount = -1 ;
	opts.device = device ;
	opts.m_opts = mode ;
	opts.key = key ;
	opts.key_source = key_source ;
	opts.mount_point_option = mount_point_option ;
	
	/*
	 * zuluCryptEXEOpenVolume() is defined in ../zuluCrypt-cli/bin/open_volume.c
	 */
	st = zuluCryptEXEOpenVolume( &opts,mapping_name,uid ) ;
	
	return st ;
}
