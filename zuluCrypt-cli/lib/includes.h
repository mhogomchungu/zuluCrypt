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

#include "../String.h"
#include "../executables.h"
#include "../version.h"

//function prototypes

int add_key(const char * device,const char * existingkey,const char * newkey) ;
	    
int remove_key( const char * device ,const char * pass ) ;
			    
char * empty_slots( const char * device ) ;
			    
char * status( const char * mapper ) ;
			    
void execute( const char *command ,char *output,int size) ;
		     
int is_luks(const char * dev) ;
		     
int close_mapper( const char * mapper ) ;
		     
int create_luks(const char * dev,const char * pass,const char * rng) ;
		       
int create_volume(const char * dev,const char * fs,const char * type,const char * pass,const char * rng) ;
			   
int close_volume(const char * map) ;
			   
int mount_volume(const char * mapping_name,const char * m_point,const char * mode,uid_t id) ;
		       
int open_luks( const char * device,const char * mapping_name,const char * mode,const char * source,const char * pass ) ;
			
int open_plain( const char * device,const char * mapping_name, const char * mode,const char * source,const char * pass, const char * cipher ) ;
		   
int open_volume(const char * dev,const char * map,const char * m_point,uid_t id,const char * mode, const char * pass,const char * source) ;			
		     
char * version(void) ; 

int unmount_volume( const char * map ) ;
