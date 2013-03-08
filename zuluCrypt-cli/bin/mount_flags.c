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
#include "includes.h"

int zuluCryptMountFlagsAreNotCorrect( const char * mode,uid_t uid,unsigned long * flags ) 
{
	unsigned long flg = 0 ;
	
	int user_has_no_access ;
	if( uid == 0 ){
		user_has_no_access = 0 ;
	}else{
		/*
		 * zuluCryptUserIsAMemberOfAGroup() is defined in security.c
		 */
		user_has_no_access = !zuluCryptUserIsAMemberOfAGroup( uid,"zulumount" ) ;
	}
	if( mode == NULL ){
		flg |= MS_NODEV | MS_NOSUID | MS_NOEXEC | MS_RELATIME ;
		*flags = flg ;
		return 0 ;
	}
	if( StringHasComponent( mode,"ro" ) ){
		flg |= MS_RDONLY;
	}
	if( StringHasComponent( mode,"dev" ) ){
		if( user_has_no_access ){
			return 1 ;
		}
	}else{
		flg |= MS_NODEV ;
	}
	if( zuluCryptUserIsAMemberOfAGroup( uid,"zulumount-exec" ) ){
		/*
		 * user is a member of a group,mount volume with exec option by default
		 */
		if( StringHasComponent( mode,"noexec" ) ){
			/*
			 * user with access wish to mount a volume without it
			 */
			flg |= MS_NOEXEC ;
		}
	}else{
		if( StringHasComponent( mode,"exec" ) ){
			if( user_has_no_access ){
				return 1 ;
			}
		}else{
			flg |= MS_NOEXEC ;
		}
	}
	if( StringHasComponent( mode,"suid" ) ){
		if( user_has_no_access ){
			return 1 ;
		}
	}else{
		flg |= MS_NOSUID ;
	}
	if( strstr( mode,"bind" ) != NULL ){
		if( user_has_no_access ){
			return 1 ;
		}
		flg |= MS_BIND ;
	}
	if( StringHasComponent( mode,"mandlock" ) ){
		if( user_has_no_access ){
			return 1 ;
		}
		flg |= MS_MANDLOCK ;
	}
	if( StringHasComponent( mode,"move" ) ){
		if( user_has_no_access ){
			return 1 ;
		}
		flg |= MS_MOVE ;
	}
	if( StringHasComponent( mode,"noatime" ) ){
		if( user_has_no_access ){
			return 1 ;
		}
		flg |= MS_NOATIME ;
	}
	if( StringHasComponent( mode,"strictatime" ) ){
		if( user_has_no_access ){
			return 1 ;
		}
		flg |= MS_STRICTATIME ;
	}
	if( flg & MS_NOATIME ){
		/*
		 * MS_NOATIME flag is set by user,use it instead of MS_RELATIME
		 */
		;
	}else if( flg & MS_STRICTATIME ){
		/*
		 *  MS_STRICTATIME flag is set by user,use it instead of MS_RELATIME 
		 */
		;
	}else{
		/*
		 * MS_NOATIME flag not set,autoset MS_RELATIME flag as the default flag
		 */
		flg |= MS_RELATIME ;
	}
#if 0
	/*
	 * done check for this one since its a default option set above
	 */
	if( StringHasComponent( mode,"relatime" ) ){
		
		if( user_has_no_access ){
			return 1 ;
		}
		flg |= MS_RELATIME ;
	}
#endif	
	if( StringHasComponent( mode,"nodiratime" ) ){
		if( user_has_no_access ){
			return 1 ;
		}
		flg |= MS_NODIRATIME ;
	}
	if( StringHasComponent( mode,"remount" ) ){
		if( user_has_no_access ){
			return 1 ;
		}
		flg |= MS_REMOUNT ;
	}
	if( StringHasComponent( mode,"silent" ) ){
		if( user_has_no_access ){
			return 1 ;
		}
		flg |= MS_SILENT ;
	}
	if( StringHasComponent( mode,"synchronous" ) ){
		if( user_has_no_access ){
			return 1 ;
		}
		flg |= MS_SYNCHRONOUS ;
	}
	*flags = flg ;
	return 0 ;
}
