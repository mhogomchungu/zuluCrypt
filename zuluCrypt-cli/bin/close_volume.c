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

#include "includes.h"

static int zuluExit( int st,string_t p )
{
	StringDelete( &p ) ;
	
	switch( st ) {
		case 0 : printf( "SUCCESS: volume closed successfully \n" );								  break ;		 
		case 1 : printf( "ERROR: close failed, encrypted volume with that name does not exist\n" );      			  break ;
		case 2 : printf( "ERROR: close failed, the mount point and/or one or more files are in use\n" );			  break ;
		case 3 : printf( "ERROR: close failed, volume does not have an entry in /etc/mtab\n" ) ;        			  break ;
		case 4 : printf( "ERROR: close failed, could not get a lock on /etc/mtab~\n" ) ;  	             			  break ;
		case 5 : printf( "ERROR: close failed, volume is unmounted but could not close mapper,advice to close it manually\n");	  break ;
		case 6 : printf( "ERROR: close failed, could not resolve full path of device\n");					  break ;
		case 7 : printf( "ERROR: close failed, could not get elevated privileges,check binary permissions\n" );			  break ;
		default: printf( "ERROR: unrecognized error with status number %d encountered\n",st );
	}	
	return st ;
}

int zuluCryptEXECloseVolume( const char * dev,const char * mapping_name,uid_t uid )
{	
	 int st ;
	 string_t p = StringVoid ;
	 char * m_point = NULL ;
	 
	 /*
	  * zuluCryptCreateMapperName() defined in "../lib/create_mapper_name.c" 
	  */
	 p = zuluCryptCreateMapperName( dev,mapping_name,uid,CLOSE ) ;
	 
	 if( !zuluCryptSecurityPathIsValid( StringContent( p ),uid ) )
		 return zuluExit( 1,p ) ;
	 
	 if( !zuluCryptSecurityGainElevatedPrivileges() )
		 return zuluExit( 7,p ) ;
	 
	 st = zuluCryptCloseVolume( StringContent( p ),&m_point ) ;
	
	 if( st == 0 ){
		if( m_point != NULL ){
			remove( m_point ) ;
			free( m_point ) ;
		}
	 }
	 zuluCryptSecurityDropElevatedPrivileges() ;
	 return zuluExit( st,p ) ;
}
