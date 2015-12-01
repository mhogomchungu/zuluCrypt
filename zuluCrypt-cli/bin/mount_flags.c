/*
 *
 *  Copyright (c) 2012-2015
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

#include <sys/mount.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "includes.h"

#define MOUNT_WITH_NOEXEC_BY_DEFAULT 0

static int _user_has_no_access( uid_t uid )
{
	if( uid == 0 ){

		return 0 ;
	}else{
		/*
		 * zuluCryptUserIsAMemberOfAGroup() is defined in security.c
		 */
		return !zuluCryptUserIsAMemberOfAGroup( uid,"zulumount" ) ;
	}
}

int zuluCryptMountFlagsAreNotCorrect( const char * mode,uid_t uid,unsigned long * flags )
{
	unsigned long flg = 0 ;

	if( mode == NULL ){

		flg |= MS_NODEV | MS_NOSUID | MS_NOEXEC | MS_RELATIME ;
		*flags = flg ;
		return 0 ;
	}
	if( StringHasComponent( mode,"ro" ) ){

		flg |= MS_RDONLY ;
	}
	if( StringHasComponent( mode,"dev" ) ){

		if( _user_has_no_access( uid ) ){

			return 1 ;
		}
	}else{
		flg |= MS_NODEV ;
	}
#if MOUNT_WITH_NOEXEC_BY_DEFAULT
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

			if( _user_has_no_access( uid ) ){

				return 1 ;
			}
		}else{
			flg |= MS_NOEXEC ;
		}
	}
#else
	if( StringHasComponent( mode,"noexec" ) ){
		/*
		* user with access wish to mount a volume without it
		*/
		flg |= MS_NOEXEC ;
	}
#endif
	if( StringHasComponent( mode,"suid" ) ){

		if( _user_has_no_access( uid ) ){

			return 1 ;
		}
	}else{
		flg |= MS_NOSUID ;
	}
	if( StringHasComponent( mode,"bind" ) ){

		if( _user_has_no_access( uid ) ){

			return 1 ;
		}
		flg |= MS_BIND ;
	}
	if( StringHasComponent( mode,"mandlock" ) ){

		if( _user_has_no_access( uid ) ){

			return 1 ;
		}
		flg |= MS_MANDLOCK ;
	}
	if( StringHasComponent( mode,"move" ) ){

		if( _user_has_no_access( uid ) ){

			return 1 ;
		}
		flg |= MS_MOVE ;
	}
	if( StringHasComponent( mode,"noatime" ) ){

		if( _user_has_no_access( uid ) ){

			return 1 ;
		}
		flg |= MS_NOATIME ;
	}
	if( StringHasComponent( mode,"strictatime" ) ){

		if( _user_has_no_access( uid ) ){

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

		if( _user_has_no_access( uid ) ){

			return 1 ;
		}
		flg |= MS_RELATIME ;
	}
#endif
	if( StringHasComponent( mode,"nodiratime" ) ){

		if( _user_has_no_access( uid ) ){

			return 1 ;
		}
		flg |= MS_NODIRATIME ;
	}
	if( StringHasComponent( mode,"remount" ) ){

		if( _user_has_no_access( uid ) ){

			return 1 ;
		}
		flg |= MS_REMOUNT ;
	}
	if( StringHasComponent( mode,"silent" ) ){

		if( _user_has_no_access( uid ) ){

			return 1 ;
		}
		flg |= MS_SILENT ;
	}
	if( StringHasComponent( mode,"synchronous" ) ){

		if( _user_has_no_access( uid ) ){

			return 1 ;
		}
		flg |= MS_SYNCHRONOUS ;
	}
	*flags = flg ;
	return 0 ;
}
