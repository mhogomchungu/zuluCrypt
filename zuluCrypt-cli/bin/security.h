
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
 * set the function to be called when an attempt to evelate or downgrade privileges fail
 * zuluCryptSecurityPrivilegeElevationError global variable is defined in security.c
 */
void ( *zuluCryptSecurityPrivilegeElevationError )( const char * ) ;

void zuluCryptSecuritySetPrivilegeElevationErrorFunction( void ( * ) ( const char * ) ) ;

void zuluCryptSecuritySanitizeTheEnvironment( uid_t uid,stringList_t * ) ;

int zuluCryptSecurityUserOwnTheFile( const char * device,uid_t uid ) ;

void zuluCryptSecurityPrintPermissions( void ) ;

void zuluCryptSetUserUIDForPrivilegeManagement( uid_t ) ;

void zuluCryptSecurityLockMemory( stringList_t stl ) ;

void zuluCryptSecurityUnlockMemory( stringList_t stl ) ;

void zuluCryptSecurityLockMemory_1( string_t st ) ;

void zuluCryptSecurityUnlockMemory_1( string_t st ) ;

#ifdef __cplusplus
}
#endif

#endif