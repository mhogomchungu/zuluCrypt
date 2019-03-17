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
#include "../lib/includes.h"
#include <locale.h>
#include <stdio.h>
#include <libintl.h>
#include <sys/stat.h>
#include <unistd.h>

static int zuluExit( int st,string_t p )
{
	StringDelete( &p ) ;

	switch( st ) {
		case 0 : printf( gettext( "SUCCESS: volume closed successfully \n" ) ) ;							  break ;
		case 1 : printf( gettext( "ERROR: Close failed, volume is not open or was opened by a different user\n" ) ) ;  			  break ;
		case 2 : printf( gettext( "ERROR: Close failed, the mount point and/or one or more files are in use\n" ) ) ;			  break ;
		case 3 : printf( gettext( "ERROR: Close failed, volume does not have an entry in /etc/mtab\n" ) ) ;        			  break ;
		case 4 : printf( gettext( "ERROR: Close failed, could not get a lock on /etc/mtab~\n" ) ) ;  	             			  break ;
		case 5 : printf( gettext( "ERROR: Close failed, volume is unmounted but could not close mapper,advice to close it manually\n") ); break ;
		case 6 : printf( gettext( "ERROR: Close failed, could not resolve full path of device\n") ) ;					  break ;
		case 7 : printf( gettext( "ERROR: Close failed, shared mount point appear to be busy\n" ) ) ;					  break ;
		case 8 : printf( gettext( "ERROR: Close failed, shared mount point appear to belong to a different user or multiple mount points detected\n" ) ) ;		  break ;
		case 9 : printf( gettext( "ERROR: Close failed, shared mount point appear to be in an ambiguous state,advice to unmount manually\n" ) ) ; break ;
		case 10: printf( gettext( "ERROR: Close failed, multiple mount points for the volume detected\n" ) ) ; break ;

		default: printf( gettext( "ERROR: Unrecognized error with status number %d encountered\n" ),st );
	}
	return st ;
}

int zuluCryptEXECloseVolume( const char * dev,const char * mapping_name,uid_t uid )
{
	 int st ;
	 int i ;
	 string_t p = StringVoid ;
	 char * m_point = NULL ;
	 struct stat xt ;
	 const char * mapper ;

	 int r ;

	 zuluCryptSecurityGainElevatedPrivileges() ;

	 r = zuluCryptDeviceHasAgivenFileSystem( dev,zuluCryptBitLockerType() ) ;


	 if( r == 1 ){

		 p = zuluCryptBitLockerFullMapperPath( uid,dev ) ;

		 mapper = StringContent( p ) ;

		 i = stat( mapper,&xt ) ;

		 zuluCryptSecurityDropElevatedPrivileges() ;

		if( i != 0 ){

			 return zuluExit( 1,p ) ;
		}
	 }else{
		 zuluCryptSecurityDropElevatedPrivileges() ;

		 /*
		  * ZULUCRYPTlongMapperPath is set in ../constants.h
		  * zuluCryptCreateMapperName() defined in ../lib/create_mapper_name.c
		  */
		 p = zuluCryptCreateMapperName( dev,mapping_name,uid,ZULUCRYPTlongMapperPath ) ;

		 mapper = StringContent( p ) ;

		 if( stat( mapper,&xt ) != 0 ){

			 return zuluExit( 1,p ) ;
		 }
	 }

	 /*
	  * zuluCryptBindUnmountVolume() is defined in ./bind.c
	  */
	 switch( zuluCryptBindUnmountVolume( StringListVoid,mapper,uid ) ){

		 case 3 : return zuluExit( 7,p ) ;
		 case 4 : return zuluExit( 8,p ) ;
		 case 5 : return zuluExit( 9,p ) ;
		 default: ;
	 }

	 zuluCryptSecurityGainElevatedPrivileges() ;

	 /*
	  * zuluCryptCloseVolume() is defined in ../lib/close_volume.c
	  *
	  * zuluCryptReuseMountPoint() is defined in create_mount_point.c
	  */

	 if( r == 1 ){
		 if( zuluCryptReuseMountPoint() ){

			 st = zuluCryptBitLockerlock( p,NULL ) ;
		 }else{
			 st = zuluCryptBitLockerlock( p,&m_point ) ;
		 }
	 }else{
		 if( zuluCryptReuseMountPoint() ){

			 st = zuluCryptCloseVolume( mapper,NULL ) ;
		 }else{
			 st = zuluCryptCloseVolume( mapper,&m_point ) ;
		 }
	 }

	 if( st == 0 && m_point != NULL ){

		 for( i = 0 ; i < 2 ; i++ ){

			if( rmdir( m_point ) == 0 ){

				break ;
			}else{
				sleep( 1 ) ;
			}
		 }
	 }

	 StringFree( m_point ) ;

	 zuluCryptSecurityDropElevatedPrivileges() ;
	 return zuluExit( st,p ) ;
}
