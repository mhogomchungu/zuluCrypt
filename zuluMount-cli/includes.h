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
 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

#include "../zuluCrypt-cli/constants.h"
#include "../zuluCrypt-cli/bin/libzuluCrypt-exe.h"
#include "../zuluCrypt-cli/bin/includes.h"
#include "../zuluCrypt-cli/lib/libzuluCrypt.h"
#include "../zuluCrypt-cli/lib/includes.h"
#include "../zuluCrypt-cli/bin/includes.h"

int zuluMountUMount( const char * device,uid_t uid,const char * mode,int mount_point_option ) ;

int zuluMountMount( const char * device,const char * m_point,
		    const char * m_opts,const char * fs_opts,uid_t uid,
		    int mount_point_option 
		  ) ;
		    

int zuluMountCryptoMount( const char * device,const char * UUID,const char * mode,uid_t uid,
				  const char * key,const char * key_source,
				  const char * m_point,int mount_point_option,
				  stringList_t
			) ;
				  
int zuluMountCryptoUMount( const char * device,const char * UUID,uid_t uid,__attribute__((unused)) int mount_point_option ) ;

int _zuluExit( int st,string_t z,char * q,const char * msg ) ;

int _zuluExit_1( int st,stringList_t z,char * q,const char * msg ) ;

void zuluMountPartitionProperties( const char * mapper,const char * UUID,const char * device,const char * m_point ) ;
