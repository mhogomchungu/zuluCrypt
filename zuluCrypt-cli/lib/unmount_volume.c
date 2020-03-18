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

#include "includes.h"

#include <mntent.h>
#include <sys/mount.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

static int _unmount_rest( const char * m_dir )
{
	return umount( m_dir ) ;
}

static int _unmount_fuse( const char * m_dir )
{
	/*
	 * ZULUCRYPTumount is defined in ../constants.h
	 */
	return ProcessExecute( ZULUCRYPTumount,m_dir,NULL ) ;
}

static int _unmount( int( *function )( const char * m_dir ),const char * m_dir )
{
	int h ;
	int i ;

	/*
	 * try 5 times on one second intervals to umount the volume.
	 * Trying to unmount more than once seem to be necessary sometimes
	 */
	for( i = 0 ; i < 5 ; i++ ){

		h = function( m_dir ) ;

		if( h == 0 ){

			break ;
		}else{
			sleep( 1 ) ;
		}
	}

	return h ;
}

static int _zuluCryptUnmountVolume_0( string_t st,char ** m_point,char ** m_point_2 )
{
	int h ;

	stringList_t stl = StringListStringSplit( st,' ' ) ;

	StringListIterator it = StringListBegin( stl ) ;

	/*
	 * zuluCryptDecodeMountEntry() is defined in mount_volume.c
	 */
	const char * mout_point = zuluCryptDecodeMountEntry( *( it + 1 ) ) ;

	if( StringContains( *( it + 2 ),"fuse" ) ){

		/*
		 * Dont know whats going on but FUSE based file systems do not seem to work with umount()
		 */
		h = _unmount( _unmount_fuse,mout_point ) ;

		if( h == 32 ){

			/*
			 * errno value of 32 means "broken pipe, we will get this when trying
			 * to unmount a file system on a device that is no longer attached
			 */
			h = _unmount( _unmount_rest,mout_point ) ;
		}
	}else{
		h = _unmount( _unmount_rest,mout_point ) ;
	}

	if( h == 0 && m_point != NULL ){

		*m_point = StringCopy_2( mout_point ) ;
	}

	if( m_point_2 != NULL ){

		*m_point_2 = StringCopy_2( mout_point ) ;
	}

	StringListDelete( &stl ) ;

	return h ;
}

static stringList_t _get_mount_entries( const char * device )
{
	/*
	 * zuluCryptGetMountEntry() is defined in mountinfo.c
	 */
	stringList_t stl = zuluCryptGetMoutedList() ;

	StringListIterator it ;
	StringListIterator end ;

	string_t st = String_1( device," ",NULL ) ;

	StringListGetIterators( stl,&it,&end ) ;

	while( it != end ){

		if( StringStartsWith_1( *it,st ) ){

			it++ ;
		}else{
			StringListRemoveAt_1( stl,it,&end ) ;
		}
	}

	StringDelete( &st ) ;

	return stl ;
}

int zuluCryptUnmountVolume( const char * device,char ** m_point )
{
	int h = 3 ;

	char * e ;

	size_t s ;

	stringList_t stl ;

	StringListIterator it ;
	StringListIterator end ;

	ssize_t r ;

	string_t st ;
	string_t xt ;

	if( zuluCryptNoPartitionLoopDevice( device ) ){

		/*
		 * zuluCryptLoopDeviceAddress() is defined in create_loop_device.c
		 */

		e = zuluCryptLoopDeviceAddress( device ) ;

		if( e == NULL ){

			return h ;
		}else{
			stl = _get_mount_entries( e ) ;
			StringFree( e ) ;
		}
	}else{
		stl = _get_mount_entries( device ) ;
	}

	s = StringListSize( stl ) ;

	if( s == 0 ){

		/*
		 * volume appear to not be mounted.
		 */

	}else if( s == 1 ){

		/*
		 * there is only one mount point for the volume,unmount it normally
		 */

		h = _zuluCryptUnmountVolume_0( StringListStringAtFirstPlace( stl ),m_point,NULL ) ;
	}else{
		/*
		 * There are multiple mount points for the same volume.
		 *
		 * Try to figure out which one among the mount points is ours and then try
		 * first to unmount the rest of them.
		 */

		r = StringListHasSequence( stl," /run/media/private/" ) ;

		if( r == -1 ){

			/*
			 * Probable reason for getting here is if a user use a home mount point path,
			 * we dont know the path because we dont know the user we are serving
			 * and hence we bail out with an error.
			 */
			h = 10 ;
		}else{
			/*
			 * We got our mount point,take it out of the list to use it last
			 */
			st = StringListDetachAt( stl,r ) ;

			StringListGetIterators( stl,&it,&end ) ;

			while( it != end ){

				xt = *it ;

				it++ ;

				if( _zuluCryptUnmountVolume_0( xt,NULL,&e ) != 0 ){

					printf( "Failed to unmount third party mount point: \"%s\"\n",e ) ;

					StringFree( e ) ;
					/*
					 * Failed to unmount one of the extra mount points,
					 * bail out with an error.
					 */
					h = 10 ;

					break ;
				}
			}

			if( h != 10 ){

				/*
				 * Attempt to unmount our mount point last.
				 */
				h = _zuluCryptUnmountVolume_0( st,m_point,NULL ) ;
			}

			StringDelete( &st ) ;
		}
	}

	if( h != 0 && h != 3 && h != 4 && h != 1 && h != 10 ){

		h = 2 ;
	}

	StringListDelete( &stl ) ;

	return h ;
}
