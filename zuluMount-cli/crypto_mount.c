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

int zuluMountCryptoMount( ARGS * args )
{
	const char * device     = args->device     ;
	const char * UUID       = args->uuid       ;
	const char * mode       = args->m_opts     ;
	uid_t        uid        = args->uid        ;
	const char * key        = args->key        ;
	const char * key_source = args->key_source ;
	const char * m_point    = args->m_point    ;
	int mount_point_option  = args->mpo        ;
	int share               = args->share      ;
	int st ;
	/*
	 * the struct is declared in ../zuluCrypt-cli/bin/libzuluCrypt-exe.h
	 */
	struct_opts opts ;
	
	const char * mapping_name ;
	char * path = NULL ;
	
	string_t str = StringVoid ;
	
	if( UUID == NULL ){
		if( StringPrefixEqual( device,"/dev/loop" ) ){
			/*
			* zuluCryptLoopDeviceAddress_1() is defined in ../zuluCrypt-cli/create_loop_device.c
			*/
			path = zuluCryptLoopDeviceAddress_1( device ) ;
			if( path == NULL ){
				return 20 ;
			}else{
				mapping_name = _mapping_name( path ) ;
			}
		}else{
			mapping_name = _mapping_name( device ) ;
		}
	}else{
		str = String( UUID ) ;
		StringRemoveString( str,"\"" ) ;
		mapping_name = StringReplaceString( str,"UUID=","UUID-" ) ;
	}
	
	/*
	 * zuluCryptEXEGetOptsSetDefault() is defined in ../zuluCrypt-cli/bin/get_opts.c
	 */
	zuluCryptEXEGetOptsSetDefault( &opts ) ;
	
	if( StringPrefixEqual( key_source,"-G" ) ){
		opts.plugin_path = key ;
	}
	
	opts.mount_point        = m_point ;
	opts.open_no_mount      = -1 ;
	opts.device             = device ;
	opts.m_opts             = mode ;
	opts.key                = key ;
	opts.key_source         = key_source ;
	opts.mount_point_option = mount_point_option ;
	opts.share              = share ; 
	opts.env                = StringListStringArray( args->env ) ;
	opts.tcrypt_hidden_volume = args->tcrypt_hidden_volume ;
	
	/*
	 * zuluCryptEXEOpenVolume() is defined in ../zuluCrypt-cli/bin/open_volume.c
	 */
	st = zuluCryptEXEOpenVolume( &opts,mapping_name,uid ) ;
	
	free( ( char * ) opts.env ) ;
	
	StringDelete( &str ) ;
	
	if( path != NULL ){
		free( path ) ;
	}

	return st ;
}
