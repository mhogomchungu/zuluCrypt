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
 
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../String.h" 
#include "../executables.h"
#include "../zuluCrypt.h"
 
#define ALL_PARTITIONS        1
#define SYSTEM_PARTITIONS     2
#define NON_SYSTEM_PARTITIONS 3

#define UUID_LENGTH 36
#define DEVICE_LENGHT 12

string_t get_passphrase(  void  ) ;
 
char * partitions(  int option  ) ;
 
int volume_info(  const char * mapper, const char * device  ) ;
 
int close_opened_volume(  char * mapping_name  ) ;
 
int open_volumes( int argn,char * device,char * mapping_name,int id,char * mount_point,char * mode,char * source,char * pass ) ;
		   
int create_volumes( int argn ,char * device,char * fs,char * mode,char * keyType,char * pass,char * rng  ) ;

int addkey( int argn,char * device,char * keyType1,char * existingKey,char * keyType2,char * newKey ) ;
		
int removekey(  int argn ,char * device, char * keyType, char * keytoremove  ) ;

//int read_file(char ** content,char * path) ;

int is_path_valid(const char * path ) ;

void replace_bash_special_chars( string_t * ) ;
