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

#include "../string/String.h"
#include "../executables.h"
#include "../version.h"

//function prototypes

int add_key(const char * device,const char * existingkey,size_t existingkey_size,const char * newkey,size_t newkey_size) ;
	    
int remove_key( const char * device ,const char * pass,size_t pass_size ) ;
			    
char * empty_slots( const char * device ) ;
			    
char * status( const char * mapper ) ;
			    
void execute( const char *command ,char *output,int size) ;
		     
int is_luks(const char * dev) ;
		     
int close_mapper( const char * mapper ) ;
		     
int create_luks(const char * dev,const char * pass,size_t pass_size,const char * rng) ;
		       
int create_volume(const char * dev,const char * fs,const char * type,const char * pass,size_t pass_size,const char * rng) ;
			   
int close_volume(const char * map, char ** mount_point) ;
			   
int mount_volume(const char * mapping_name,const char * m_point,const char * mode,uid_t id) ;
		       
int open_luks( const char * device,const char * mapping_name,const char * mode,const char * pass,size_t pass_size ) ;
			
int open_plain( const char * device,const char * mapping_name,const char * mode,const char * pass,size_t pass_size,const char * cipher ) ;
		   
int open_volume(const char * dev,const char * map,const char * m_point,uid_t id,const char * mode,const char * pass,size_t pass_size) ;			
		     
const char * version(void) ; 

int unmount_volume( const char * map, char ** mount_point ) ;

char * volume_device_name( const char * mapper ) ;

//defined in ../bin/is_path_valid.c
int is_path_valid(const char * path ) ;