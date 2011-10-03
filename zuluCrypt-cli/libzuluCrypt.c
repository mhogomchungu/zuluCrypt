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

#include "../executables.h"

//function prototypes

int is_luks(char * device) ;
void execute( char *command , char *output, int size) ;
int mount_volume(char *mapping_name,char *m_point,char * mode,uid_t id) ;
char * sanitize(char *c ) ;

int add_key(char * dev, char * ek, char * keyfile)
{
	StrHandle * p ;
	char s[2] ;
	
	char * device = sanitize( dev ) ;
	char * existingkey = sanitize( ek ) ;
	
	p = StringCpy( ZULUCRYPTecho ) ;
	//StringCat( p , "\"");
	StringCat( p , existingkey ) ;
	//StringCat( p , "\"");
	StringCat( p , " | " ) ;
	StringCat( p , ZULUCRYPTcryptsetup) ;
	StringCat( p , " luksAddKey ") ;
	StringCat( p , device ) ;
	StringCat( p , " " ) ;
	StringCat( p , keyfile ) ;
	StringCat( p , " 2>/dev/null 1>&2 ; echo $?") ;
	
	execute(StringCont( p ), s, 1 ) ;	
	
	StringDelete( p ) ;
	free( device ) ;
	free( existingkey ) ;
	
	return s[0] - '0' ;
}

int kill_slot( char * dev,char * ek, int slotNumber)
{	
	char s[2] ;
		
	char * device = sanitize( dev ) ;
	char * existingkey = sanitize( ek ) ;
	
	StrHandle * p = StringCpy( ZULUCRYPTecho ) ;
	//StringCat( p , "\"");
	StringCat( p , existingkey ) ;
	//StringCat( p , "\"");
	StringCat( p , " | " ) ;
	StringCat( p , ZULUCRYPTcryptsetup ) ;
	StringCat( p , " luksKillSlot ") ;
	StringCat( p , device ) ;
	StringCat( p , " " ) ;
	
	s[0] = ( char ) slotNumber ; 
	s[1] = '\0' ;
	
	StringCat( p , s ) ;
	StringCat( p , " 2>&1") ;        //< -------- read error values from stderr
	
	execute( StringCont( p ), s , 1 ) ;
	StringDelete( p ) ;

	free( device ) ;
	free( existingkey ) ;
	
	//cryptsetup return not very informative error numbers, stderr is more useful
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

int remove_key( char * dev , char * keyfile )
{
	char s[2] ;
	
	char * device = sanitize( dev ) ;

	StrHandle * p = StringCpy(ZULUCRYPTcryptsetup ) ;
	StringCat( p , " luksRemoveKey ") ;
	StringCat( p , device ) ;
	StringCat( p , " " ) ;
	StringCat( p , keyfile ) ;
	StringCat( p , " 2>/dev/null 1>&2 ; echo $?") ;

	execute( StringCont( p ), s, 1 ) ;
	
	free( device ) ;
	StringDelete( p ) ;
	
	return s[0] - '0' ;	
}

int empty_slots( char * slots ,char * dev )
{
	struct stat st ;
	
	int i ;
	
	char *c ;	
	
	char * device = sanitize( dev ) ;
	
	StrHandle * p = StringCpy( ZULUCRYPTcryptsetup) ;
	StringCat( p , " luksDump ") ;
	StringCat( p , device ) ;
	StringCat( p , " 1> /tmp/zuluCrypt-dump 2>&1") ;
	
	execute( StringCont( p ), NULL, 0 ) ;
	
	StringDelete( p ) ;
	
	free( device ) ;
	
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

void status( char * map , char * output, int size )
{		
	char * mapping_name = sanitize( map ) ;
	
	StrHandle * str = StringCpy(ZULUCRYPTcryptsetup ) ;
	StringCat(str," status zuluCrypt-") ;

	StringCat(str,mapping_name);
	
	execute(StringCont(str),output,size);
	
	StringDelete(str);
}

/*
 * execute function here uses popen, popen opens bash and pass the command to it and bash execute it.
 * 
 * This function escapes all characters that triggers bash intelligence.  
 * 
 * Without this function, a passphrase with a single quote for example will cause bash to expect a 
 * second one and will error out.
 * 
 * Without this function, a passphrase with an interesting character sequence will cause bash to run
 * a command.
 */

char * sanitize(char *c )
{
	char *n="#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ;	
	
	char *d ;	
	
	char *f ;
	
	int count = 0 ;
	
	int i,j ;
	
	int z = strlen( c ) ;
	int k = strlen( n ) ;
	
	for ( i = 0 ; i < z ; i++ ){
		
		for( j = 0 ; j < k ; j++ ){
			
			if( c[i] == n[j] ){
				count++ ;
				break ;
			}
		}		
	}
	
	f = d = (char * ) malloc(sizeof(char) * ( z + count + 1 ) ) ;	

	for ( i = 0 ; i < z ; i++ ){
		
		for( j = 0 ; j < k ; j++ ){
			
			if( c[i] == n[j] ){
				*f++ = '\\' ;
				//*f++ = n[j] ;
				break ;
			}
		}
		
		*f++ = c[i] ;
	}
	*f = '\0' ;

	return d ;
}

void execute( char *command , char *output, int size)
{		
	FILE *f ;
	
	//log outputs of command
	//printf("%s\n",command);
	
	//i = open("/home/ink/zzz",O_WRONLY | O_APPEND | O_CREAT ) ;
	//write(i,command,strlen(command)) ;
	//write(i,"\n",1) ;
	//close(i);
	
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

int is_luks(char * dev)
{	
	char s[2] ;		

	char * device = sanitize( dev ) ;

	StrHandle * str = StringCpy(ZULUCRYPTcryptsetup ) ;
	StringCat(str," isLuks ");
	StringCat(str,device);	
	StringCat(str," 2>/dev/null 1>&2 ; ") ;
	StringCat(str,ZULUCRYPTecho) ;
	StringCat(str," $?") ;
	
	execute( StringCont( str ),s,1 );
		
	StringDelete( str );
		
	free( device ) ;
	
	return s[0] - '0' ;	
}

int create_volume(char * dev, char * fs,char * type, char * pass)
{
	StrHandle * p ;
	StrHandle * q ;
	
	char *device ;
	char * passphrase ;
	
	struct stat st ;
	
	if ( stat( dev, &st ) != 0 ){
		return 1 ;
	}
	
	if( strncmp(fs,"ext3",4) != 0)
		if( strncmp(fs,"ext4",4) != 0)
			if( strncmp(fs,"vfat",4) != 0)
				return 6 ;
			
	device = sanitize( dev ) ;
	passphrase = sanitize( pass ) ;
	
	q = StringCpy(ZULUCRYPTmkfs );
	StringCat( q , " -t ") ;
	StringCat( q , fs ) ;
	StringCat( q , " " ) ;
	StringCat( q , "/dev/mapper/zuluCrypt-create-new 1>/dev/null 2>&1  ; ") ;
	
	if  (strcmp(type,"luks")  == 0 ){	
		
		p = StringCpy( ZULUCRYPTecho );
		//StringCat( p , "\"");
		StringCat( p , passphrase ) ;
		//StringCat( p , "\"");
		
		StringCat( p , " | " ) ;
		StringCat( p , ZULUCRYPTcryptsetup ) ;
		StringCat( p , " -q luksFormat " ) ;		
		
		StringCat( p , device ) ;
		
		execute( StringCont( p ), NULL, 0 ) ;
		
		StringDelete( p ) ;	
		
		p = StringCpy( ZULUCRYPTecho );
		//StringCat( p , "\"");
		StringCat( p , passphrase ) ;
		//StringCat( p , "\"");
		StringCat( p , " | " ) ;
		StringCat( p , ZULUCRYPTcryptsetup ) ;
		StringCat( p , " luksOpen ") ;

		StringCat( p , device ) ;
		StringCat( p , " zuluCrypt-create-new");
		
		execute( StringCont( p ), NULL, 0 ) ;
		
		StringDelete( p ) ;
		
		StringCat( q , ZULUCRYPTcryptsetup ) ;
		StringCat( q , " luksClose zuluCrypt-create-new  2>/dev/null 1>&2 ") ;		
		
		execute( StringCont( q ), NULL, 0 ) ;	
		
		StringDelete( q ) ;
		
	}else if ( strcmp(type,"plain")  == 0 ){
		
		p = StringCpy( ZULUCRYPTecho );
		//StringCat( p , "\"");
		StringCat( p , passphrase ) ;
		//StringCat( p , "\"");
		StringCat( p , " | " ) ;
		StringCat( p , ZULUCRYPTcryptsetup ) ;
		StringCat( p , " create zuluCrypt-create-new " ) ;		

		StringCat( p , device ) ;
		StringCat( p, " ") ;
				
		execute( StringCont( p ), NULL, 0) ;	
		
		StringDelete( p ) ;
		
		StringCat( q , "cryptsetup remove zuluCrypt-create-new  ") ;
		
		execute( StringCont( q ), NULL, 0 ) ;	
		
		StringDelete( q ) ;
		
	}else
		return 2 ;
	
	free( device ) ;
	free( passphrase );
	
	return 0 ;
}

int close_volume(char * mapping_name,char * device) 
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
	
	StringSanitize( q ) ;

	a = StringCpy(ZULUCRYPTumount) ;
	StringCat( a , " ") ;
	StringCat( a , StringCont( q ) ) ;
	StringCat( a , " 2>/dev/null 1>&2 ; "ZULUCRYPTecho " $? ") ;	
	
	execute( StringCont( a ), u,1 ) ;	
	
	StringDelete( a ) ;
	
	if( u[0] != '0' ){		
		StringDelete( q ) ;
		return 2 ;	
	}	

	if ( is_luks( device ) == 0 ){
		a = StringCpy(ZULUCRYPTcryptsetup ) ;
		StringCat( a ," luksClose ") ;
		StringCat( a , StringCont( q ) ) ;
	}else{
		a = StringCpy(ZULUCRYPTcryptsetup ) ;
		StringCat( a ," remove ") ;
		StringCat( a , StringCont( q ) ) ;		
	}
	
	StringCat( a, " ; ") ;
	StringCat( a, ZULUCRYPTrm ) ;
	StringCat( a," -rf ") ;
	StringCat( a, x) ;
	
	execute( StringCont( a ),NULL,0) ;
	
	StringDelete( a ) ;
	StringDelete( q ) ;
	
	free(c) ;	
	
	return 0 ;
}

int mount_volume(char *mapping_name,char *m_point,char * mode,uid_t id)
{
	StrHandle *p ;
	StrHandle *z ;
	StrHandle *q ;
	
	char s[2] ;
	
	if ( m_point == NULL )	
		p = StringCpy( "/mnt/" ) ;
	else{
		p = StringCpy(m_point) ;
		StringCat( p ,"/");
	}
	
	StringCat( p , mapping_name ) ;
	
	StringSanitize( p ) ;	
	
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
		q = StringCpy(ZULUCRYPTmount " -r ") ;
	else
		q = StringCpy(ZULUCRYPTmount " -w ") ;
	
	StringCat( q , StringCont( z ) ) ;

	StringCat( q , " ");

	StringCat( q , StringCont( p ) ) ;
	
	StringCat( q , "  2>/dev/null 1>&2 ; ") ;
	StringCat( q , ZULUCRYPTecho ) ;
	StringCat( q , "$?");
	
	execute(StringCont( q ),s,1) ;	
	
	if( s[0] != '0' ){
		
		remove( StringCont( p ) ) ;
		StringDelete( q ) ;
		StringDelete( p ) ;
		StringDelete( z ) ;
		return 4 ;
	}
		
	chown( StringCont( p ), id, id ) ;
	
	chmod( StringCont( p ), S_IRWXU ) ;
	
	StringDelete( q ) ;
	StringDelete( p ) ;
	StringDelete( z ) ;
	
	return 0 ;		
}
int open_volume(char *dev, char * map, char *m_point, uid_t id,char * mode, char *pass) 
{
	char status[2] ;	
	StrHandle * p ;	
	StrHandle * z ;
	struct stat st ;
	int h ;
	
	char *device ;
	char *mapping_name ;
	char *passphrase ;
	
	if( stat( dev , &st ) != 0 ){		 
		return 3 ;
	}
	
	if ( strncmp(dev,"/dev/",5) != 0 ){
	
		z = StringCpy( ZULUCRYPTlosetup  ) ;
		StringCat(z ," -f 2>/dev/null 1>&2 ;" );
		StringCat(z , ZULUCRYPTecho ) ;
		StringCat(z , "  $? " ) ;
		execute(StringCont( z ),status,1) ;
		StringDelete( z ) ;
		
		if ( status[0] != '0' ){
			return 1 ;	
		}
	}
	
	int luks = is_luks( dev ) ;	

	device = sanitize( dev ) ;	
	
	z = StringCpy("/dev/mapper/zuluCrypt-");
	StringCat( z , map ) ;
	
	if( stat( StringCont( z ), &st ) == 0 ){
		StringDelete( z ) ;
		return 2 ;
	}
	
	StringDelete( z ) ;	
	
	mapping_name = sanitize( map ) ;
	passphrase = sanitize( pass ) ;
	
	p = StringCpy( ZULUCRYPTecho );
	//StringCat( p , "\"");
	StringCat( p , passphrase ) ;
	//StringCat( p , "\"");
	StringCat( p , " | " ) ;
		
	if ( luks == 0 ){	
		
		if ( strncmp( mode, "ro",2 ) == 0 ){		
			StringCat( p ,ZULUCRYPTcryptsetup ) ;		
			StringCat( p ," -r luksOpen ") ;
		}else{		
			StringCat( p ,ZULUCRYPTcryptsetup ) ;
			StringCat( p ," luksOpen ") ;
		}
		StringCat( p , device ) ;
		StringCat( p , " zuluCrypt-" ) ;
		StringCat( p , mapping_name ) ;
		StringCat( p , " ") ;		
		
	}else{
		if ( strncmp( mode, "ro",2 ) == 0 ){		
			StringCat( p ,ZULUCRYPTcryptsetup ) ;
			StringCat( p ," -r create zuluCrypt-");
		}
		else{
			StringCat( p ,ZULUCRYPTcryptsetup ) ;
			StringCat( p ,"  create zuluCrypt-");
			
		}
		
		StringCat( p, mapping_name ) ;
		StringCat( p, " " ) ;
		StringCat( p, device ) ;
		StringCat( p , " 1>/dev/null 2>&1") ;				
	}	

	execute( StringCont( p ), status, 1 ) ;			

	StringDelete( p ) ;
	
	if( status[0] == '3' ){
		free( device );
		free( mapping_name );
		free( passphrase );	
		return 9 ;
	}
	
	h = mount_volume(mapping_name,m_point,mode,id ) ;	
	
	if( h != 0 ) {
		
		if ( luks == 0 ){
			z = StringCpy( ZULUCRYPTcryptsetup ) ;
			StringCat( z , " luksClose zuluCrypt-") ;
			StringCat( z , mapping_name ) ;
			StringCat( z , "  2>/dev/null 1>&2");
			execute( StringCont( z ),NULL,0 ) ;
			StringDelete( z ) ;
			
		}else{			
			z = StringCpy( ZULUCRYPTcryptsetup ) ;
			StringCat( z , " remove zuluCrypt-") ;
			StringCat( z , mapping_name ) ;
			StringCat( z , " 2>/dev/null 1>&2");			
			execute( StringCont( z ),NULL,0 ) ;			
			StringDelete( z ) ;
			
			if ( h == 4 ){
				//plain volume open failed, lets try to reopen the plain volume using legacy option.
				//legacy mode is with option -c aes-cbc-plain
				//sleep( 2 ) ;
				z = StringCpy( ZULUCRYPTecho );
				//StringCat( z , "\"");
				StringCat( z , passphrase ) ;
				//StringCat( z , "\"");
				StringCat( z , " | " ) ;
			
				if ( strncmp( mode, "ro",2 ) == 0 ){
					StringCat( z ,ZULUCRYPTcryptsetup ) ;
					StringCat( z , " -r -c aes-cbc-plain create zuluCrypt-");
				}else{
					StringCat( z ,ZULUCRYPTcryptsetup ) ;
					StringCat( z , " -c aes-cbc-plain create zuluCrypt-");
				}
				StringCat( z, mapping_name ) ;
				StringCat( z, " " ) ;
				StringCat( z, device ) ;
				StringCat( z , " 1>/dev/null 2>&1") ;			
			
				execute( StringCont( z ), NULL, 0 ) ;			

				StringDelete( z ) ;
			
				h = mount_volume(mapping_name,m_point,mode,id ) ;
				
				if( h == 4 ) {
				
					z = StringCpy( ZULUCRYPTcryptsetup ) ;
					StringCat( z , " remove zuluCrypt-") ;
					StringCat( z , mapping_name ) ;
					StringCat( z , " 1>/dev/null 2>&1");			
					execute( StringCont( z ),NULL,0 ) ;			
					StringDelete( z ) ;				
				}
			}	
		}	
	}

	free( device );
	free( mapping_name );
	free( passphrase );
	return h ;	
}

