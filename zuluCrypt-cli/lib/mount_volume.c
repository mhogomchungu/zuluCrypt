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

#include <sys/mount.h>
#include <mntent.h>
#include <blkid/blkid.h>

int mount_volume( const char * mapper,const char * m_point,const char * mode,uid_t id )
{
	StrHandle * p = NULL ;
	
	StrHandle * q ;
	
	unsigned long mountflags = 0 ;
	
	struct mntent mt  ;
	
	blkid_probe blkid ;
	
	char s[4] ;
	
	int h ;
	
	const char * fs ;	
	
	FILE * f ;
	
	blkid = blkid_new_probe_from_filename( mapper ) ;
	
	blkid_do_probe( blkid );
	
	h = blkid_probe_lookup_value( blkid , "TYPE", &fs, NULL ) ;		
	
	if( h != 0 ){
		/*you will land here if:
		 * 
		 *1. you attempt to open a plain volume with a wrong passphrase
		 *2. you attempt to open a volume without a file system,or unrecognized file system  
		 */
		blkid_free_probe( blkid );		
		
		close_mapper( mapper ) ; 
		
		return 4 ;
	}
	
	q = String( fs ) ;
	
	blkid_free_probe( blkid );
	
	fs = StringContent( q ) ;
		
	mt.mnt_freq = 0 ;
	
	mt.mnt_passno = 0 ;
	
	mt.mnt_fsname = ( char * ) mapper ;
	
	mt.mnt_dir =  ( char * ) m_point ;	
	
	mt.mnt_type = ( char * ) fs ;
	
	if ( strcmp( mode, "ro" ) == 0 )
		mountflags = MS_RDONLY ;
		
	if( strcmp( fs, "vfat" ) == 0 ){
		
		p = String( "dmask=007,uid=" ) ;

		StringAppend( p ,intToString( s, 10, id ) ) ;
		
		StringAppend( p , ",gid=" ) ;
		
		StringAppend( p ,intToString( s, 10, id ) );		
				
		h = mount( mapper, m_point,fs,mountflags,StringContent( p ) ) ;	
		
		StringPrepend( p ,"," ) ;
		
		StringPrepend( p , mode ) ;
	}else{		
		p = String( mode ) ;
		
		h = mount( mapper, m_point,fs,mountflags,NULL) ;		
		
		chown( m_point, id, id ) ;
		
		chmod( m_point, S_IRWXU ) ;		
	}
	
	if( h == 0 ){
		
		mt.mnt_opts = ( char * ) StringContent( p ) ;
		
		f = setmntent( "/etc/mtab","a" ) ;
		
		addmntent( f, &mt ) ;		
			
		endmntent( f ) ;

	}else{		
		close_mapper( mapper ) ; 
		
		h = 4 ;
	}	
	
	StringDelete( p ) ;
	
	StringDelete( q ) ;
	
	return h ;
}

