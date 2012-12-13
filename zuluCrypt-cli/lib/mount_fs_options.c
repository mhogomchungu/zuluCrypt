 
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

#include "../bin/includes.h"

static inline int _fileSystemIsNotSupported( const char * fs )
{
	if(     strcmp( fs,"ntfs" ) == 0  || 
		strcmp( fs,"vfat" ) == 0  || 
		strcmp( fs,"fat" ) == 0   ||
		strcmp( fs,"msdos" ) == 0 ||
		strcmp( fs,"umsdos" ) == 0 ||
		strcmp( fs,"affs" ) == 0 || 
		strcmp( fs,"hfs" ) == 0  ||
		strcmp( fs,"iso9660" ) == 0 ||
		strcmp( fs,"udf" ) == 0  ||
		strcmp( fs,"ext2" ) == 0 ||
		strcmp( fs,"ext3" ) == 0 ||
		strcmp( fs,"ext4" ) == 0 ){
			return 0 ;
		}else{
			return 1 ;
		}
}

static inline int _userIsAllowed( uid_t uid )
{
	if( uid == 0 || zuluCryptUserIsAMemberOfAGroup( uid,"zulucrypt-root" ) )
		return 1 ;
	else
		return 0 ;
}

static inline int _fileSystemOptionsAreAllowed( const char * fs,const char * fs_opts )
{
	if( fs == NULL ){
		;
	}
	
	if( fs_opts == NULL ){
		;
	}
	return 0 ;
}

int zuluCryptFsOptionsAreNotAllowed( uid_t uid,const char * fs_opts,string_t s )
{
	const char * fs = StringContent( s ) ;
	
	if( fs == NULL )
		return 0 ;
	if( _fileSystemIsNotSupported( fs ) ){
		if( _userIsAllowed( uid ) ){
			return 0 ;
		}else{
			return 1 ;
		}
	}
	if( _fileSystemOptionsAreAllowed( fs,fs_opts ) ){
		return 0 ;
	}else{
		if( _userIsAllowed( uid ) ){
			return 1 ;
		}else{
			return 0 ;
		}
	}
		
	return 0 ;
}
