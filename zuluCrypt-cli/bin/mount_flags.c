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

#include <sys/mount.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int zuluCryptUserIsAMemberOfAGroup( uid_t uid,const char * groupname ) ;

static inline int _has_no_access( uid_t uid )
{
	return uid == 0 ? 0 : !zuluCryptUserIsAMemberOfAGroup( uid,"zulucrypt-root" ) ;
}

int zuluCryptMountFlagsAreNotCorrect( const char * mode,uid_t uid,unsigned long * flags ) 
{
	unsigned long flg = 0 ;
	
	if( strstr( mode,"ro" ) == 0 )
		flg = MS_RDONLY;
	
	if( strstr( mode,"dev" ) != NULL ){
		if( _has_no_access( uid ) )
			return 1 ;
	}else{
		flg |= MS_NODEV ;
	}
	if( strstr( mode,"exec" ) != NULL ){
		if( _has_no_access( uid ) ) 
			return 1 ;
	}else{
		flg |= MS_NOEXEC ;
	}
	if( strstr( mode,"suid" ) != NULL ){
		if( _has_no_access( uid ) )
			return 1 ;
	}else{
		flg |= MS_NOSUID ;
	}
	if( strstr( mode,"bind" ) != NULL ){
		flg |= MS_BIND ;
	}
	if( strstr( mode,"mandlock" ) != NULL ){
		flg |= MS_MANDLOCK ;
	}
	if( strstr( mode,"move" ) != NULL ){
		flg |= MS_MOVE ;
	}
	if( strstr( mode,"noatime" ) != NULL ){
		flg |= MS_NOATIME ;
	}
	if( strstr( mode,"nodiratime" ) != NULL ){
		flg |= MS_NODIRATIME ;
	}
	if( strstr( mode,"relAtime" ) != NULL ){
		flg |= MS_RELATIME ;
	}
	if( strstr( mode,"remount" ) != NULL ){
		flg |= MS_REMOUNT ;
	}
	if( strstr( mode,"silent" ) != NULL ){
		flg |= MS_SILENT ;
	}
	if( strstr( mode,"strictatime" ) != NULL ){
		flg |= MS_STRICTATIME ;
	}
	if( strstr( mode,"synchronous" ) != NULL ){
		flg |= MS_SYNCHRONOUS ;
	}
	*flags = flg ;
	return 0 ;
}
