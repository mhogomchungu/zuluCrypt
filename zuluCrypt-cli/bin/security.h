
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


#ifndef ZULUCRYPTSECURITY
#define ZULUCRYPTSECURITY

#ifdef __cplusplus
extern "C" {
#endif	 

#include "includes.h"
/*
 * All these functions are defined in security.c
 */

/*
 * make sure the user with uid has reading permissions to path
 */
int zuluCryptSecurityCanOpenPathForReading( const char * path,uid_t uid ) ;

/*
 * make sure a user with uid has writing access to path
 */
int zuluCryptSecurityCanOpenPathForWriting( const char * path,uid_t uid ) ;

/*
 * make sure a user can create a folder at path
 */
string_t zuluCryptSecurityCreateMountPoint( const char * device,const char * m_point,uid_t uid ) ;

int zuluCryptSecurityMountPointPrefixMatch( const char * path,uid_t uid ) ;

void zuluCryptSecuritySanitizeTheEnvironment( uid_t uid,stringList_t * ) ;

string_t zuluCryptGetUserHomePath( uid_t uid ) ;

string_t zuluCryptGetUserName( uid_t uid ) ;

int zuluCryptSecurityUserOwnTheFile( const char * device,uid_t uid ) ;

void zuluCryptSecurityPrintPermissions( void ) ;

int zuluCryptSecurityDeviceIsReadable( const char * device,uid_t uid ) ;

int zuluCryptSecurityDeviceIsWritable( const char * device,uid_t uid ) ;

char * zuluCryptSecurityUUIDFromPath( const char * device ) ;

#ifdef __cplusplus
}
#endif

#endif