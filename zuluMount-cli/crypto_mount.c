/*
 *
 *  Copyright (c) 2012
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

int zuluMountCryptoMount( ARGS * args )
{
	const char * type       = args->type       ;
	const char * offset     = args->offset     ;
	const char * device     = args->device     ;
	const char * UUID       = args->uuid       ;
	const char * mode       = args->m_opts     ;
	uid_t        uid        = args->uid        ;
	const char * key        = args->key        ;
	const char * key_source = args->key_source ;
	const char * m_point    = args->m_point    ;
	const char * fs_opts    = args->fs_opts    ;
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
				mapping_name = path + StringLastIndexOfChar_1( path,'/' ) + 1 ;
			}
		}else{
			mapping_name = device + StringLastIndexOfChar_1( device,'/' ) + 1 ;
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
	opts.device             = device ;
	opts.m_opts             = mode ;
	opts.key                = key ;
	opts.key_source         = key_source ;
	opts.mount_point_option = mount_point_option ;
	opts.share              = share ;
	opts.fs_opts            = fs_opts ;
	opts.env                = StringListStringArray( args->env ) ;
	opts.offset             = offset ;
	opts.type               = type ;

	memcpy( opts.tcrypt_multiple_keyfiles,args->tcrypt_multiple_keyfiles,
		sizeof( args->tcrypt_multiple_keyfiles ) ) ;

	/*
	 * zuluCryptEXEOpenVolume() is defined in ../zuluCrypt-cli/bin/open_volume.c
	 */
	st = zuluCryptEXEOpenVolume( &opts,mapping_name,uid ) ;

	StringDelete( &str ) ;

	StringFree( opts.env ) ;

	StringFree( path ) ;

	return st ;
}
