/*
 *
 *  Copyright (c) 2011-2015
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

#include "luks_slot_status.h"
#include "includes.h"
#include <libcryptsetup.h>
#include <fcntl.h>
#include <unistd.h>

static char * zuluExit( char * c,struct crypt_device * cd )
{
	if( cd != NULL ){

		crypt_free( cd ) ;
	}

	return c ;
}

static const char * _crypt_init( struct crypt_device ** cd,
				 const char * device,
				 const resolve_path_t * opts )
{
	const char * type ;

	if( opts ){}

	if( crypt_init( cd,device ) != 0 ){

		return NULL ;
	}

	if( crypt_load( *cd,NULL,NULL ) != 0 ){

		return NULL ;
	}

	type = crypt_get_type( *cd ) ;

	if( type == NULL ){

		return zuluExit( NULL,*cd ) ;
	}else{
		return type ;
	}
}

static char * _empty_slots( const char * device,const resolve_path_t * opts )
{
	struct crypt_device * cd ;

	int j ;
	int k ;

	string_t p ;

	const char * type = _crypt_init( &cd,device,opts ) ;

	if( type == NULL ){

		return zuluExit( NULL,cd ) ;
	}

	k = crypt_keyslot_max( type ) ;

	if( k < 0 ){

		return zuluExit( NULL,cd ) ;
	}

	p = StringEmpty() ;

	for( j = 0 ; j < k ; j++ ){

		switch( crypt_keyslot_status( cd,j ) ){

			case CRYPT_SLOT_INACTIVE   : StringAppend( p,"0" ) ; break ;
			case CRYPT_SLOT_ACTIVE     : StringAppend( p,"1" ) ; break ;
			case CRYPT_SLOT_INVALID    : StringAppend( p,"2" ) ; break ;
			case CRYPT_SLOT_ACTIVE_LAST: StringAppend( p,"3" ) ; break ;
			default : ;
		}
	}

	return zuluExit( StringDeleteHandle( &p ),cd ) ;
}

char * zuluCryptEmptySlots( const char * device )
{
	/*
	 * resolve_path_t is defined in includes.h
	 */
	resolve_path_t opts ;

	memset( &opts,'\0',sizeof( opts ) ) ;

	opts.device        = device ;
	opts.open_mode     = O_RDONLY ;
	opts.error_value_1 = NULL ;

	/*
	 * zuluCryptResolveDevicePath_1() is defined in resolve_path.c
	 */
	return zuluCryptResolveDevicePath_1( _empty_slots,&opts ) ;
}

#if SUPPORT_crypt_keyslot_get_pbkdf

const char * _to_string( unsigned int s )
{
	static char buffer[ 1024 ] ;
	snprintf( buffer,sizeof( buffer ),"%d",( int )s ) ;
	return buffer ;
}

static void _get_slot_property( string_t q,int j,const char * type,struct crypt_device * cd )
{
	size_t key_size ;

	struct crypt_pbkdf_type pbkdf ;

	if( crypt_keyslot_get_pbkdf( cd,j,&pbkdf ) == 0 ){

		StringMultipleAppend( q,"PBKDF type: ",pbkdf.type,"\n",NULL ) ;

		if( StringsAreEqual( type,CRYPT_LUKS1 ) ){

			StringMultipleAppend( q,"PBKDF hash: ",pbkdf.hash,"\n",NULL ) ;

			StringMultipleAppend( q,"PBKDF iterations: ",_to_string( pbkdf.iterations ),NULL ) ;

		}else if( StringsAreEqual( type,CRYPT_LUKS2 ) ){

			const char * e = crypt_keyslot_get_encryption( cd,j,&key_size ) ;

			key_size *= 8 ;

			if( e ){

				StringMultipleAppend( q,"Cipher: ",e,"\n",NULL ) ;
			}else{
				StringMultipleAppend( q,"Cipher: ","N/A","\n",NULL ) ;
			}

			StringMultipleAppend( q,"Cipher key: ",_to_string( ( unsigned int )key_size )," bits\n",NULL ) ;

			switch( crypt_keyslot_get_priority( cd,j ) ) {

				case CRYPT_SLOT_PRIORITY_INVALID :
					StringAppend( q,"Priority: Invalid\n" ) ;
					break ;
				case CRYPT_SLOT_PRIORITY_IGNORE :
					StringAppend( q,"Priority: Ignore\n" ) ;
					break ;
				case CRYPT_SLOT_PRIORITY_NORMAL :
					StringAppend( q,"Priority: Normal\n" ) ;
					break ;
				case CRYPT_SLOT_PRIORITY_PREFER :
					StringAppend( q,"Priority: Prefer\n" ) ;
					break ;
				default:
					StringAppend( q,"Priority: Unknown\n" ) ;
					break ;
			}

			StringMultipleAppend( q,"Time cost: ",_to_string( pbkdf.iterations ),"\n",NULL ) ;

			StringMultipleAppend( q,"Memory: ",_to_string( pbkdf.max_memory_kb ),"\n",NULL ) ;

			StringMultipleAppend( q,"Threads: ",_to_string( pbkdf.parallel_threads ),NULL ) ;
		}

		StringAppend( q,"\n\n" ) ;
	}
}

static char * _slots_status( const char * device,const resolve_path_t * opts )
{
	struct crypt_device * cd ;

	int j ;
	int k ;

	string_t q ;

	const char * type = _crypt_init( &cd,device,opts ) ;

	if( type == NULL ){

		return zuluExit( NULL,cd ) ;
	}

	k = crypt_keyslot_max( type ) ;

	if( k < 0 ){

		return zuluExit( NULL,cd ) ;
	}

	k = crypt_keyslot_max( type ) ;

	if( k < 0 ){

		return zuluExit( NULL,cd ) ;
	}

	crypt_keyslot_info info ;

	q = String_1( type," header information\n\n",NULL ) ;

	for( j = 0 ; j < k ; j++ ){

		info = crypt_keyslot_status( cd,j ) ;

		if( info == CRYPT_SLOT_ACTIVE || info == CRYPT_SLOT_ACTIVE_LAST ) {

			StringMultipleAppend( q,"Slot Number: ",_to_string( ( unsigned int )j ),"\n",NULL ) ;

			StringAppend( q,"Slot Status: Active\n" ) ;

			_get_slot_property( q,j,type,cd ) ;

		}else if( info == CRYPT_SLOT_INACTIVE ){

			StringMultipleAppend( q,"Slot Number: ",_to_string( ( unsigned int )j ),"\n",NULL ) ;

			StringAppend( q,"Slot Status: Inactive\n\n" ) ;

		}else if( info == CRYPT_SLOT_INVALID ){

			StringMultipleAppend( q,"Slot Number: ",_to_string( ( unsigned int )j ),"\n",NULL ) ;

			StringAppend( q,"Slot Status: Invalid\n\n" ) ;

		}else if( info == CRYPT_SLOT_UNBOUND ){

			StringMultipleAppend( q,"Slot Number: ",_to_string( ( unsigned int )j ),"\n",NULL ) ;

			StringAppend( q,"Slot Status: Unbound\n\n" ) ;
		}else{
			StringMultipleAppend( q,"Slot Number: ",_to_string( ( unsigned int )j ),"\n",NULL ) ;

			StringAppend( q,"Slot Status: Unknown\n\n" ) ;
		}
	}

	return zuluExit( StringDeleteHandle( &q ),cd ) ;
}

#else

static char * _slots_status( const char * device,const resolve_path_t * opts )
{
	if( device && opts ){}

	return NULL ;
}

#endif

char * zuluCryptSlotsStatus( const char * device )
{
	/*
	 * resolve_path_t is defined in includes.h
	 */
	resolve_path_t opts ;

	memset( &opts,'\0',sizeof( opts ) ) ;

	opts.device        = device ;
	opts.open_mode     = O_RDONLY ;
	opts.error_value_1 = NULL ;

	/*
	 * zuluCryptResolveDevicePath_1() is defined in resolve_path.c
	 */
	return zuluCryptResolveDevicePath_1( _slots_status,&opts ) ;
}
