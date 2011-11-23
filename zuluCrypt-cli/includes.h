 
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "String.h" 
#include "executables.h"
#include "zuluCrypt.h"
 
#define ALL_PARTITIONS        1
#define SYSTEM_PARTITIONS     2
#define NON_SYSTEM_PARTITIONS 3

StrHandle * get_passphrase(  void  ) ;
 
char * partitions(  int option  ) ;
 
int volume_info(  const char * mapper  ) ;
 
int close_opened_volume(  char * mapping_name  ) ;
 
int open_volumes( int argn,char * device,char * mapping_name,int id,char * mount_point,char * mode,char * source,char * pass ) ;
		   
int create_volumes( int argn ,char * device,char * fs,char * mode,char * keyType,char * pass,char * rng  ) ;

int addkey( int argn,char * device,char * keyType1,char * existingKey,char * keyType2,char * newKey ) ;
		
int removekey(  int argn ,char * device, char * keyType, char * keytoremove  ) ;