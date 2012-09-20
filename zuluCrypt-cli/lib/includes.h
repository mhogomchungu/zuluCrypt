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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libcryptsetup.h>   
#include <stdlib.h>

#include "../string/String.h"
#include "../string/StringManage.h"
#include "../constants.h"
#include "../version.h"
#include "libzuluCrypt.h"
#include "../bin/bash_special_chars.h"

#ifdef __STDC__
char * realpath( const char * path, char * resolved_path ) ;
#endif

string_t zuluCryptCreateMapperName( const char * device,const char * mapping_name,uid_t uid,int i ) ;

/*
 * this function checks if path exists or not.
 * return 1 if the path exists
 * return 0 if it does not
 */
int zuluCryptPathIsValid( const char * path ) ;

/*
 * this function checks if a path exists or not
 * return 1 if the path does not exists
 * return 0 if the path exist
 */
int zuluCryptPathIsNotValid( const char * path ) ;

/*
 * this function is deined in ../lib/print_mountd_volumes.c
 * return 1 if mtab is a file and is located at /etc
 * return 0 otherwise
 */
int zuluCryptMtabIsAtEtc( void ) ;

/*
 * this is defined in print_mounted_volumes.c
 */
int zuluCryptPartitionIsMounted( const char * device ) ;
