/*
 *
 *  Copyright (c) 2019
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
#include <pwd.h>

const char * zuluCryptBitLockerType()
{
	return "crypto_BitLocker" ;
}

string_t zuluCryptBitLockerMapperPath( uid_t uid )
{
	return String_1( "/run/media/private/",getpwuid( uid )->pw_name,"/cryptoBitlocker",NULL ) ;
}

int zuluCryptBitLockerVolume( const char * e )
{
	return StringEndsWith_1( e,"/dislocker-file" ) ;
}

int zuluCryptBitLockerVolume_1( const char * e )
{
	stringList_t stl = StringListSplit( e,' ' ) ;
	int s = zuluCryptBitLockerVolume( StringListContentAtFirstPlace( stl ) ) ;
	StringListDelete( &stl ) ;
	return s ;
}

string_t zuluCryptBitLockerResolveMapperPath( const char * e,uid_t uid )
{
	string_t q = String( e ) ;
	string_t z = zuluCryptBitLockerMapperPath( uid ) ;

	StringRemoveString( q,StringContent( z ) ) ;
	StringRemoveString( q,"/dislocker-file" ) ;

	StringReplaceChar( q,'_','/' ) ;
	StringDelete( &z ) ;

	return q ;
}

const char * zuluCryptBitLockerCreateMapperPath( string_t e,uid_t uid )
{
	string_t st = zuluCryptBitLockerMapperPath( uid ) ;
	StringReplaceChar( e,'/','_' ) ;
	StringPrependString( e,st ) ;
	StringDelete( &st ) ;
	return StringAppend( e,"/dislocker-file" ) ;
}
