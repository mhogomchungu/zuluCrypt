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
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <termios.h>

#include "String.h"   

#define cryptsetup "/sbin/cryptsetup "
#define mount      "/bin/mount " 
#define umount     "/bin/umount "
#define mkfs       "/sbin/mkfs "
#define losetup    "/sbin/losetup "
#define e2label    "/sbin/e2label "
#define rm         "/bin/rm "
#define echo       "/bin/echo -n "


//function prototypes

int is_luks(char * device) ;
void execute( char *command , char *output, int size) ;

int add_key(char * device, char * existingkey, char * keyfile)
{
	StrHandle * p ;
	char s[2] ;
	
	p = StringCpy( echo ) ;
	StringCat( p , "\"");
	StringCat( p , existingkey ) ;
	StringCat( p , "\"");
	StringCat( p , " | " ) ;
	StringCat( p , cryptsetup "luksAddKey ") ;
	StringCat( p , device ) ;
	StringCat( p , " " ) ;
	StringCat( p , keyfile ) ;
	StringCat( p , " 2>/dev/null 1>&2 ; echo $?") ;
	
	execute(StringCont( p ), s, 1 ) ;	
	
	return s[0] - '0' ;
}

int kill_slot( char * device,char * existingkey, int slotNumber)
{	
	char s[2] ;
	
	StrHandle * p = StringCpy( echo ) ;
	StringCat( p , "\"");
	StringCat( p , existingkey ) ;
	StringCat( p , "\"");
	StringCat( p , " | " cryptsetup "luksKillSlot ") ;
	StringCat( p , device ) ;
	StringCat( p , " " ) ;
	
	s[0] = ( char ) slotNumber ; 
	s[1] = '\0' ;
	
	StringCat( p , s ) ;
	StringCat( p , " 2>&1") ;
	
	execute( StringCont( p ), s , 1 ) ;
	StringDelete( p ) ;

	//cryptsetup 1.3.1 and 1.3.0 has a bug and returns "0" when trying to kill an inactive slot, working around it
	
	if ( s[0] == '0' )      // success
		return 0 ;
	else if ( s[0] == 'K' ) // trying to kill an inactive slot
		return 1 ;
	else if ( s[0] == 'D' ) // device doesnt exist
		return 2 ;
	else if ( s[0] == 'N' )
		return 3 ;      // no key available that matched presented key
	
	return 0 ; //shouldnt get here
}

int remove_key( char * device , char * keyfile )
{
	char s[2] ;
	StrHandle * p = StringCpy(cryptsetup "luksRemoveKey ") ;
	StringCat( p , device ) ;
	StringCat( p , " " ) ;
	StringCat( p , keyfile ) ;
	StringCat( p , " 2>/dev/null 1>&2 ; echo $?") ;

	execute( StringCont( p ), s, 1 ) ;
	
	return s[0] - '0' ;	
}

int empty_slots( char * slots ,char * device )
{
	struct stat st ;
	
	int i ;
	
	char *c ;	
	
	StrHandle * p = StringCpy( cryptsetup "luksDump ") ;
	StringCat( p , device ) ;
	StringCat( p , " 1> /tmp/zuluCrypt-dump 2>&1") ;
	
	execute( StringCont( p ), NULL, 0 ) ;
	
	stat("/tmp/zuluCrypt-dump",&st) ;
	
	c = ( char * ) malloc ( sizeof(char) * st.st_size) ;
	
	i = open("/tmp/zuluCrypt-dump",O_RDONLY) ;
	
	read ( i , c , st.st_size ) ;
	
	close( i ) ;	
	
	if ( c[0] == 'D' )
		return 1 ;
	
	if ( strstr( c, "Key Slot 0: DISABLED") == NULL )
		slots[0] = '1' ;
	
	if ( strstr( c, "Key Slot 1: DISABLED") == NULL )
		slots[1] = '1' ;
		
	if ( strstr( c, "Key Slot 2: DISABLED") == NULL )
		slots[2] = '1' ;

	if ( strstr( c, "Key Slot 3: DISABLED") == NULL )
		slots[3] = '1' ;
					
	if ( strstr( c, "Key Slot 4: DISABLED") == NULL )
		slots[4] = '1' ;
							
	if ( strstr( c, "Key Slot 5: DISABLED") == NULL )
		slots[5] = '1' ;
									
	if ( strstr( c, "Key Slot 6: DISABLED") == NULL )
		slots[6] = '1' ;

	if ( strstr( c, "Key Slot 7: DISABLED") == NULL )
		slots[7] = '1' ;
	
	remove("/tmp/zuluCrypt-dump") ;
	
	return 0 ;
}

void status( char * mapping_name , char * output, int size )
{		
	StrHandle * str = StringCpy(cryptsetup " status zuluCrypt-") ;

	StringCat(str,mapping_name);
	
	execute(StringCont(str),output,size);
	
	StringDelete(str);
}

void execute( char *command , char *output, int size)
{		
	FILE *f ;
	f = popen(command, "r") ;
	int i,c  ;
	if ( output != NULL  ){
		for ( i = 0 ; i < size ; i++ ){
			if (  ( c = getc(f) ) != EOF ) 
				output[i] = ( char ) c ;
			else{
				break ;
			}			
		}
		output[i] = '\0' ;
	}	
	pclose(f);	
}

int is_luks(char * device)
{	
	char s[2] ;		

	StrHandle * str = StringCpy(cryptsetup " isLuks ");
	StringCat(str,device);	
	StringCat(str," 2>/dev/null 1>&2 ; "echo " $?") ;
	
	execute( StringCont( str ),s,1 );
		
	StringDelete( str );
		
	return s[0] - '0' ;	
}

int create_volume(char * device, char * fs,char * type, char * passphrase)
{
	StrHandle * p ;
	StrHandle * q ;
	
	struct stat st ;
	
	if ( stat( device, &st ) != 0 ){
		return 1 ;
	}
	
	q = StringCpy(mkfs " -t ") ;
	StringCat( q , fs ) ;
	StringCat( q , " " ) ;
	StringCat( q , "/dev/mapper/zuluCrypt-create-new 1>/dev/null 2>&1  ; sleep 3 ; ") ;
	
	if  (strcmp(type,"luks")  == 0 ){	
		
		p = StringCpy( echo );
		StringCat( p , "\"");
		StringCat( p , passphrase ) ;
		StringCat( p , "\"");
		
		StringCat( p , " | " cryptsetup " -q luksFormat " ) ;		
		
		StringCat( p , device ) ;
		
		execute( StringCont( p ), NULL, 0 ) ;
		
		StringDelete( p ) ;	
		
		p = StringCpy( echo );
		StringCat( p , "\"");
		StringCat( p , passphrase ) ;
		StringCat( p , "\"");
		StringCat( p , " | " cryptsetup " luksOpen ") ;

		StringCat( p , device ) ;
		StringCat( p , " zuluCrypt-create-new");
		
		execute( StringCont( p ), NULL, 0 ) ;
		
		StringDelete( p ) ;
		
		StringCat( q , cryptsetup " luksClose zuluCrypt-create-new  2>/dev/null 1>&2 ") ;		
		
		execute( StringCont( q ), NULL, 0 ) ;	
		
		StringDelete( q ) ;
		
	}else if ( strcmp(type,"plain")  == 0 ){
		
		p = StringCpy( echo );
		StringCat( p , "\"");
		StringCat( p , passphrase ) ;
		StringCat( p , "\"");
		StringCat( p , " | cryptsetup create zuluCrypt-create-new " ) ;		

		StringCat( p , device ) ;
		StringCat( p, " ") ;
				
		execute( StringCont( p ), NULL, 0) ;	
		
		StringDelete( p ) ;
		
		StringCat( q , "cryptsetup remove zuluCrypt-create-new  ") ;
		
		execute( StringCont( q ), NULL, 0 ) ;	
		
		StringDelete( q ) ;
		
	}else
		return 2 ;
	
	return 0 ;
}

int close_volume(char * mapping_name) 
{
	StrHandle * q ;
	StrHandle * a ;		

	struct stat st ;		

	int i ;
	char * c ;
	char * x ;
	char * p ;
	char * z ;
	char u[2] ;
	
	q = StringCpy("/dev/mapper/zuluCrypt-") ;
	StringCat( q , mapping_name ) ;
	
	if ( stat( StringCont( q ), &st ) != 0 ){
		StringDelete( q ) ;
		return 1 ;	
	}
	
	stat("/etc/mtab",&st);
	
	p = c = ( char * ) malloc(sizeof(char) * st.st_size) ;
	
	i = open("/etc/mtab",O_RDONLY) ;
	
	read( i,c,st.st_size) ;
	
	close(i) ;
	
	i = StringLength( q ) ;
	
	z = StringCont( q ) ;
	
	while ( 1 ){
		
		if ( strncmp(p,z,i) != 0 ){
			
			while ( *p++ != '\n' ) { ; }
			 
		}else{
			x = p = p + i + 1  ;
			while ( *++p != ' ' ) { ; }
			
			*p = '\0' ;			
			break ;						
		}
	}	
	a = StringCpy(umount " ") ;
	StringCat( a , StringCont( q ) ) ;
	StringCat( a , " 2>/dev/null 1>&2 ; "echo " $? ") ;
	
	execute( StringCont( a ), u,1 ) ;	
	
	StringDelete( a ) ;
	
	if( u[0] != '0' ){		
		StringDelete( q ) ;
		return 2 ;	
	}	
	
	if ( is_luks( StringCont( q ) ) == 0 ){
		a = StringCpy(cryptsetup " luksClose ") ;
		StringCat( a , StringCont( q ) ) ;
	}else{
		a = StringCpy(cryptsetup " remove ") ;
		StringCat( a , StringCont( q ) ) ;		
	}
	
	StringCat( a, " ; "rm " -rf ") ;
	StringCat( a, x) ;
	
	execute( StringCont( a ),NULL,0) ;
	
	StringDelete( a ) ;
	StringDelete( q ) ;
	
	free(c) ;	
	
	return 0 ;
}

int open_volume(char *device, char * mapping_name, char *m_point, uid_t id,char * mode, char *passphrase) 
{
	char status[2] ;	
	StrHandle * p ;	
	StrHandle * q ;
	StrHandle * z ;
	char label[20];
	struct stat st ;
	
	int luks = is_luks( device ) ;	
	
	if ( strncmp(device,"/dev/",5) != 0 ){
	
		execute(losetup " -f 2>/dev/null 1>&2 ; " echo"  $? ",status,1) ;
		if ( status[0] != '0' ){
			return 1 ;	
		}
	}

	z = StringCpy("/dev/mapper/zuluCrypt-");
	StringCat( z , mapping_name ) ;
	
	if( stat( StringCont( z ), &st ) == 0 ){
		StringDelete( z ) ;
		return 2 ;
	}
	
	StringDelete( z ) ;
	
	if( stat( device , &st ) != 0 ){		 
		return 3 ;
	}
	
	p = StringCpy( echo );
	StringCat( p , "\"");
	StringCat( p , passphrase ) ;
	StringCat( p , "\"");
	StringCat( p , " | " ) ;
		
	if ( luks == 0 ){	
		
		if ( strncmp( mode, "ro",2 ) == 0 )		
			StringCat( p ,"cryptsetup -r luksOpen ") ;
		else
			StringCat( p ,"cryptsetup luksOpen ") ;
		
		StringCat( p , device ) ;
		StringCat( p , " zuluCrypt-" ) ;
		StringCat( p , mapping_name ) ;
		StringCat( p , " 1>/dev/null 2>&1") ;		
		
	}else{
		if ( strncmp( mode, "ro",2 ) == 0 )		
			StringCat( p ,cryptsetup " -r create zuluCrypt-");
		else
			StringCat( p ,cryptsetup "  create zuluCrypt-");
		
		StringCat( p, mapping_name ) ;
		StringCat( p, " " ) ;
		StringCat( p, device ) ;
		StringCat( p , " 1>/dev/null 2>&1") ;				
	}	

	execute( StringCont( p ), NULL, 0 ) ;			

	StringDelete( p ) ;
	
	if ( m_point == NULL )	
		p = StringCpy( "/mnt/" ) ;
	else{
		p = StringCpy(m_point) ;
		StringCat( p ,"/");
	}
	
	z = StringCpy(e2label " /dev/mapper/zuluCrypt-") ;
	StringCat( z , mapping_name ) ;
	StringCat( z , " 1>/dev/null 2>&1 ; " echo " $?" ) ;
	
	execute( StringCont( z ), label, 1 ) ;	
	
	StringDelete( z ) ;
	if( label[0] == '1' ) {
		
		if ( luks == 0 ){
			z = StringCpy( cryptsetup " luksClose zuluCrypt-") ;
			StringCat( z , mapping_name ) ;
			StringCat( z , "  2>/dev/null 1>&2");
			execute( StringCont( z ),NULL,0 ) ;
			StringDelete( z ) ;
			
			return 4 ;
		}else{			
			z = StringCpy( cryptsetup " remove zuluCrypt-") ;
			StringCat( z , mapping_name ) ;
			StringCat( z , " 2>/dev/null 1>&2");			
			execute( StringCont( z ),NULL,0 ) ;			
			StringDelete( z ) ;
			
			//plain volume open failed, lets try to reopen the plain volume using legacy option.
			//legacy mode is with option -c aes-cbc-plain
			
			z = StringCpy( echo );
			StringCat( z , passphrase ) ;
			StringCat( z , " | " ) ;
			
			if ( strncmp( mode, "ro",2 ) == 0 )		
				StringCat( z ,cryptsetup " -r -c aes-cbc-plain create zuluCrypt-");
			else
				StringCat( z ,cryptsetup " -c aes-cbc-plain  zuluCrypt-");
			
			StringCat( z, mapping_name ) ;
			StringCat( z, " " ) ;
			StringCat( z, device ) ;
			StringCat( z , " 1>/dev/null 2>&1") ;
			
			execute( StringCont( z ), NULL, 0 ) ;			

			StringDelete( z ) ;
			
			z = StringCpy(e2label " /dev/mapper/zuluCrypt-") ;
			StringCat( z , mapping_name ) ;
			StringCat( z , " 1>/dev/null 2>&1 ; " echo " $?" ) ;
	
			execute( StringCont( z ), label, 1 ) ;	
	
			StringDelete( z ) ;
			
			if( label[0] == '1' ){
				z = StringCpy( cryptsetup " remove zuluCrypt-") ;
				StringCat( z , mapping_name ) ;
				StringCat( z , " 2>/dev/null 1>&2");			
				execute( StringCont( z ),NULL,0 ) ;			
				StringDelete( z ) ;				
				return 4 ;
			}
		}	
	}		

	z = StringCpy(e2label " /dev/mapper/zuluCrypt-") ;
	
	StringCat( z , mapping_name ) ;		

	execute( StringCont( z ), label, 20 ) ;
	StringDelete( z ) ;
	

	if( label[0] == '\n' ) {		
		
		StringCat( p , mapping_name ) ;
		
	}else{			
		*( strchr( label,'\n') ) = '\0' ;			
		StringCat(p,label) ;		
	}
	
	if ( mkdir( StringCont( p ), S_IRWXU  ) != 0 ){
		StringCat( p, ".zc") ;
		
		if ( mkdir( StringCont( p ),S_IRWXU  ) != 0 ){
			StringDelete( p ) ;
			return 5 ;		
		}
	}
	
	z = StringCpy("/dev/mapper/zuluCrypt-") ;
	
	StringCat( z , mapping_name ) ;
	
	if ( strncmp( mode, "ro",2 ) == 0 )
		q = StringCpy(mount " -r ") ;
	else
		q = StringCpy(mount " -w ") ;
	
	StringCat( q , StringCont( z ) ) ;

	StringCat( q , " ");

	StringCat( q , StringCont( p ) ) ;
	
	execute(StringCont( q ),NULL,0) ;
	
	sleep(2) ;
	
	chown( StringCont( p ), id, id ) ;
	
	chmod( StringCont( p ), S_IRWXU ) ;
	
	StringDelete( q ) ;
	StringDelete( p ) ;
	StringDelete( z ) ;
	
	return 0 ;	
}

