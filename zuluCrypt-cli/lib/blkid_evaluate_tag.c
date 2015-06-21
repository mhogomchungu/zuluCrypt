/*
 *
 *  Copyright ( c ) 2012-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
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
#include <blkid/blkid.h>

static char * _resolve_path( char * path )
{
	char * e ;
	if( path == NULL ){
		return NULL ;
	}else{
		/*
		 * zuluCryptResolvePath_3() is defined in resolve_path.c
		 */
		e = zuluCryptResolvePath_3( path ) ;
		StringFree( path ) ;
		return e ;
	}
}

char * zuluCryptDeviceFromUUID( const char * uuid )
{
	return _resolve_path( blkid_evaluate_tag( "UUID",uuid,NULL) ) ;
}

char * zuluCryptDeviceFromLabel( const char * label )
{
	return _resolve_path( blkid_evaluate_tag( "LABEL",label,NULL ) ) ;
}

char * zuluCryptDeviceFromPARTUUID( const char * PARTUUID )
{
	return _resolve_path( blkid_evaluate_tag( "PARTUUID",PARTUUID,NULL ) ) ;
}

char * zuluCryptDeviceFromPARTLABEL( const char * PARTLABEL )
{
	return _resolve_path( blkid_evaluate_tag( "PARTLABEL",PARTLABEL,NULL ) ) ;
}

char * zuluCryptUUIDFromPath_1( const char * device )
{
	const char * c = NULL ;
	char * r = NULL ;

	blkid_probe blkid = blkid_new_probe_from_filename( device ) ;

	if( blkid != NULL ){
		blkid_do_probe( blkid ) ;
		blkid_probe_lookup_value( blkid,"UUID",&c,NULL ) ;
		r = StringCopy_2( c ) ;
		blkid_free_probe( blkid ) ;
	}

	return r ;
}

int zuluCryptFileSystemIsFUSEbased( const char * device )
{
	const char * cf = NULL ;
	int st ;
	blkid_probe blkid = blkid_new_probe_from_filename( device ) ;
	if( blkid != NULL ){
		blkid_do_probe( blkid ) ;
		blkid_probe_lookup_value( blkid,"TYPE",&cf,NULL ) ;
#if 1
		st = StringAtLeastOneMatch_1( cf,"ntfs","exfat",NULL ) ;
#else
		st = StringAtLeastOneMatch_1( cf,"ntfs",NULL ) ;
#endif
		blkid_free_probe( blkid ) ;
		return st ;
	}else{
		return 0 ;
	}
}

string_t zuluCryptGetFileSystemFromDevice( const char * device )
{
	string_t st = StringVoid ;
	const char * cf = NULL ;
	blkid_probe blkid = blkid_new_probe_from_filename( device ) ;
	if( blkid != NULL ){
		blkid_do_probe( blkid ) ;
		blkid_probe_lookup_value( blkid,"TYPE",&cf,NULL ) ;
		st = String( cf ) ;
		blkid_free_probe( blkid ) ;
	}
	return st ;
}

int zuluCryptDeviceHasAgivenFileSystem( const char * device,const char * fs )
{
	const char * cf = NULL ;
	int r = 0 ;

	blkid_probe blkid = blkid_new_probe_from_filename( device ) ;

	if( blkid != NULL ){
		blkid_do_probe( blkid ) ;
		blkid_probe_lookup_value( blkid,"TYPE",&cf,NULL ) ;
		r = StringsAreEqual( cf,fs ) ;
		blkid_free_probe( blkid ) ;
	}
	return r ;
}

int zuluCryptDeviceHasEncryptedFileSystem( const char * device )
{
	const char * cf = NULL ;
	int r = 0 ;

	blkid_probe blkid = blkid_new_probe_from_filename( device ) ;

	if( blkid != NULL ){

		blkid_do_probe( blkid ) ;
		blkid_probe_lookup_value( blkid,"TYPE",&cf,NULL ) ;

		if( cf == NULL ){

			r = 1 ;
		}else{
			r = StringPrefixEqual( cf,"crypto_LUKS" ) ;
		}

		blkid_free_probe( blkid ) ;
	}

	return r ;
}
