/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

#include <mntent.h>
#include <sys/mount.h>

int unmount_volume( const char * map )
{
	StrHandle * p = NULL ;
	
	struct stat st ;		
	
	FILE * f ;	
	
	FILE * g ;
	
	int h = 3 ;
	
	struct mntent * mt ;
	
	if ( stat( map , &st ) != 0 )
		return 1 ;		
	
	/*
	 * Go through /etc/mtab to found out mount point so that we can delete the mount point folder
	 * when unmounting succeed 
	*/
	
	f = setmntent( "/etc/mtab","r" ) ;
	
	g = setmntent( "/etc/mtab-zC","w" ) ;
	
	while( ( mt = getmntent( f ) ) != NULL ){
		
		if( strncmp( mt->mnt_fsname,map, strlen( map ) ) == 0 ){
			
			h = umount( mt->mnt_dir ) ;
		
			p = String( mt->mnt_dir ) ;
		}else{
			
			addmntent( g, mt ) ;			
		}
	}

	endmntent( f ) ;
	
	endmntent( g ) ;
	
	switch( h ){
		case 0:	close_mapper( map ) ;
		
			remove( StringContent( p ) ) ;

			rename( "/etc/mtab-zC", "/etc/mtab" ) ;

			remove( "/etc/mtab-zC" ) ;

			StringDelete( p ) ;
			
			break ;
		
		case 3: break ;
			
		default:StringDelete( p ) ;
		
			h = 2 ;
			
			break ;
	}
	
	return h ;
}
