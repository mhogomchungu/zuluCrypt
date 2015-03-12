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
#include <libcryptsetup.h>

/*
 * This function is responsible for creating a mapper name,the mapper name will show up at "/dev/mapper" if the volume
 * is successfully opened.
 *
 * mapping_name is taken from a path to a volume presented by the user and then "zuluCrypt-XXX-YYYY-ZZZZ-AAAA is added to it to give
 * a mapper name unique to zuluCrypt.
 *
 * XXX is the UID of the user that run the program,YYYY tells if the volume uses UUID or not."NAAN" pads the volume length
 * to make sure the length of "zuluCrypt-XXX-YYYY" is always the same regardless of what the user provided.
 *
 * XXX is there for security reason.It makes sure one user can not manage another user's mappers
 *
 * ZZZZ is taken from the last component of the device path.
 *
 * AAAA is hash of the string to prevent collissions if two different volumes that ends with the same ZZZZ are used.
 *
 * A successfully constructed "ZULUCRYPTshortMapperPath mapper path" will look like "zuluCrypt-500-NAAN-mdraid-2355849641"
 */

string_t zuluCryptCreateMapperName( const char * device,const char * mapping_name,uid_t uid,int i )
{
	string_t p ;
	unsigned long z ;
	char * e ;
	/*
	 * ZULUCRYPTshortMapperPath is set in ../constants.h
	 */
	if( i == ZULUCRYPTshortMapperPath ){
		p = String( "zuluCrypt-" ) ;
	}else{
		/*
		 * zuluCryptMapperPrefix() is defined in include.h
		 */
		p = String_1( zuluCryptMapperPrefix(),"/zuluCrypt-",NULL ) ;
	}

	StringAppendInt( p,uid ) ;

	if( StringPrefixEqual( mapping_name,"UUID-" ) ){

		StringMultipleAppend( p,"-",mapping_name,"-",NULL ) ;
		z = StringJenkinsOneAtATimeHash( mapping_name ) ;
	}else{
		StringMultipleAppend( p,"-NAAN-",mapping_name,"-",NULL ) ;

		if( StringPrefixEqual( device,"/dev/loop" ) ){

			/*
			* zuluCryptLoopDeviceAddress_1() is defined in ./create_loop_device.c
			*/
			e = zuluCryptLoopDeviceAddress_1( device ) ;

			if( e != NULL ){
				z = StringJenkinsOneAtATimeHash( e ) ;
				StringFree( e ) ;
			}else{
				z = StringJenkinsOneAtATimeHash( device ) ;
			}
		}else{
			z = StringJenkinsOneAtATimeHash( device ) ;
		}
	}

	StringAppendInt( p,z ) ;

	/*
	 * cryptsetup 1.4.1 and previous have a bug and its triggered when the mapper has one or more bash
	 * special characters, this functions substitute bash special characters for an underscore to
	 * work around the bug.
	 */
	StringReplaceCharString( p,'_',BASH_SPECIAL_CHARS ) ;
	return p ;
}

static const char * _zuluCryptMapperPrefix = NULL ;

const char * zuluCryptMapperPrefix( void )
{
	if( _zuluCryptMapperPrefix == NULL ){
		_zuluCryptMapperPrefix = crypt_get_dir() ;
		return _zuluCryptMapperPrefix ;
	}else{
		return _zuluCryptMapperPrefix ;
	}
}

