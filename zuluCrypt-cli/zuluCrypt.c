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

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include "zuluCrypt.h"
#include <unistd.h>
#include "String.h"
#include <fcntl.h>
#include <sys/wait.h>

#include "../executables.h"
#include "../version.h"
#include "zuluCrypt.h"

#define MAX 512 

StrHandle * get_passphrase( void )
{	
	
	//I got the big chunk of this code from: http://www.gnu.org/s/hello/manual/libc/getpass.html
	char c[2] ;
	StrHandle * p ;
	int i = 0 ;
	struct termios old, new;
			
       /* Turn echoing off and fail if we can't. */
	if (tcgetattr (1, &old) != 0)
		exit(-1);

	new = old;
	new.c_lflag &= ~ECHO;
			
	if (tcsetattr (1, TCSAFLUSH, &new) != 0)
		exit(-1);
		
	c[1] = '\0' ;
	c[0] = getchar() ;
	
	p = StringCpy( c ) ;
	
	while( ( c[0] = getchar() ) != '\n' && i < MAX ){
		
		StringCat( p, c ) ;
		i++ ;
	}
	
	(void) tcsetattr (1, TCSAFLUSH, &old);		
	
	return p ;
}

void help( void )
{
	printf("this tool should come with a documentation file, read it for more info\n");	
}

int volume_info( char * mapping_name )
{
	struct stat st ;
	char x[512] ;
	int Z ;
	
	StrHandle *s = StringCpy("/dev/mapper/zuluCrypt-") ;
	StringCat( s, mapping_name ) ;
	
	if( stat( StringCont( s ),&st) == 0 ){			
		status(mapping_name,x ,512 ) ;
		printf("%s",x);
		Z = 0 ;
	}else{
		printf("No opened volume associated with provided name\n") ;
		Z = 1 ;
	}
	
	StringDelete( s ) ;
	
	return Z ;	
}

int close_opened_volume( char * mapping_name,char * device )
{
	struct stat xt ;
	int st ;
	
	if( stat( device, &xt) == 0)
		st = close_volume( mapping_name, device ) ;
	else
		st = 3 ;
	
	switch( st ) {
	case 0 : printf("SUCCESS: volume successfully closed\n");
		break ;
	case 1 : printf("ERROR: close failed, encrypted volume with that name does not exist\n");
		break ;
			
	case 2 : printf("ERROR: close failed, the mount point and/or one or more files are in use\n");
		break ;
	case 3 : printf("ERROR: close failed, path given does not point to an encrypted device\n") ;
		break ;
	default :
		; //shouldnt get here			
	}	
	return st ;	
}

int open_volumes(int argn, char * device, char * mapping_name,int id, char * mount_point, char * mode,char *source, char * pass)
{
	StrHandle * p ;
	int st ;
	int f ;
	struct stat Q ;
	char *c;
	off_t fsize ;
	
	if (argn < 5 ){
		st = 8 ;
		goto eerr ;
	}	
	
	if (strncmp(mount_point,",\0",2)==0){
			
		st = 7 ;
		goto eerr ;			
	}		

	if (strncmp(mode,"ro",2) != 0){
		if (strncmp(mode,"rw",2) != 0){
			st = 7 ;
			goto eerr ;	
		}
	}		

	if (strncmp(mount_point,"-\0",2) ==0 ){
		mount_point = NULL ;
	}	
	
	if ( argn == 5 ){
		printf( "Enter passphrase: " ) ;

		p = get_passphrase();				

		printf("\n") ;	
		
		st = open_volume(device, mapping_name,mount_point,id,mode,StringCont( p )) ;
		
		StringDelete( p ) ;
		
	}else if ( argn == 7 ){

		if( strcmp(source,"-p") == 0 ){			
			
			st = open_volume(device, mapping_name,mount_point,id,mode, pass) ;			
		
		}else if ( strcmp(source,"-f") == 0 ){
			
			if ( stat(pass,&Q) == 0 ){			
				
				if( Q.st_size < MAX )
					fsize = Q.st_size ;
				else
					fsize = MAX ;
				
				c = ( char * ) malloc( sizeof(char) * ( fsize + 1 )  ) ;

				if( c == NULL ){
					st = 9 ;
					goto eerr ;
				}
					
				*( c + fsize ) = '\0' ;

				f = open( pass,O_RDONLY ) ;
			
				read(f,c,fsize) ;
				
				close(f);				
				
				st = open_volume(device, mapping_name,mount_point,id,mode,c) ;
				
				free( c ) ;
			}else{

				st = 6 ;
			}
		}else{
			
			st = 7 ;
		}
	}
	else{
		
		st =  8 ;			
	}

	eerr:
	
	switch ( st ){
			
		case 0 : printf("SUCCESS: Volume opened successfully\n");
			break ;			
		case 1 : printf("ERROR: No free loop device to use\n") ; 
			break ;					
		case 2 : printf("ERROR: There seem to be an open volume accociated with given address\n");
			break ;				
		case 3 : printf("ERROR: No file exist on given path\n") ; 
			break ;		
		case 4 : printf("ERROR: Wrong passphrase\n");
			break ;			
		case 5 : printf("ERROR: a file or folder already exist at mount point\n") ;
			break ;		
		case 6 : printf("ERROR: passphrase file does not exist\n");
			break ;		
		case 7 : printf("ERROR: Wrong option, run zuluCrypt with \"-h\" for help\n");
			break ;
		case 8 : printf("ERROR: Wrong number of arguments, run zuluCrypt with \"-h\" for help\n");
			break ;
		case 9 : printf("ERROR: operation failed, insuffucient system memory\n");
			break ;	
			
		default :
			;			
	}	
	return st ;
}

void char_from_numbers( char * buffer,int size, int k )
{
	char *c ;
	
	c = &buffer[ size - 1 ] ;
	
	do{		
		*c =  k % 10 + '0' ;
		c-- ;			
	}while( ( k = k / 10 ) != 0 )	;
}

//doesnt work,dont think its necessary
int create_file(char * name, char *random_device , char * size,uid_t id )
{
	struct stat st;
	double k,j,l,n ; ;
	//int  r ;
	char c ;
	char Z[3] ={ '0','0','0'} ;
	
	StrHandle * p ;
	
	if( random_device == NULL )
		return 10 ;
	
	if ( fork() == 0 ){		
		p = StringCpy(ZULUCRYPTdd " if=");
		StringCat( p , random_device ) ;
		StringCat( p , " of=") ;
		StringCat( p , name ) ;
		StringCat( p , " bs=1024 count=");
		StringCat( p , size) ;
		StringCat( p , " 2>/dev/null 1>&2") ;
		
		popen( StringCont( p ),"r") ;
		
		StringDelete( p ) ;
		
		chown(name,id,id);
		chmod(name,S_IRWXU);
	}else{	
		c = size[strlen(size)-1] ;
		size[strlen(size)-1] = '\0' ;
		n = atoll(size) ;	

		switch( c ){
			case 'K' : n = n * 1024 ; break;
			case 'M' : n = n * 1024 * 1024  ; break ;
			case 'G' : n = n * 1024 * 1024 * 1024   ; break ;
			default: ;
		}
		
		do{			
			stat(name,&st);			
			k =(double) st.st_size;
			sleep(2);
			stat(name,&st);			
			j = (double) st.st_size;
			l =  ( (  j  /  n ) * 100 )   ;

			write( 1 , "\rpercentage complete: ",22);
			
			char_from_numbers(Z,3,(int)l) ;
			
			write( 1, Z ,3 ) ;
			write( 1, "%",1) ;
			
		}while( k != j );

		printf("\n") ;
	}
	
	return 0 ;
}

void partitions(StrHandle *partitions, StrHandle * fstab_partitions, StrHandle * non_system_partitions)
{
	char *c,*d ;
	char buffer[512];
	char uuid[64];
	char label[64];
	
	int i ;
	
	StrHandle * command ;
		
	FILE *f ;
	FILE *z ;
	
	f = fopen("/proc/partitions","r") ;
	
	fgets(buffer,512,f ) ;
	fgets(buffer,512,f ) ;
	
	while ( fgets(buffer,512,f ) != NULL ){
		
		c = buffer ;
		
		while( *++c != '\n' ) { ; }
	
		*c = '\0' ;
		
		d = c ;
		
		while( *--d != ' ' ) { ; }
		
		d++ ;		
		
		if(strlen( d ) == 3 )
			continue ;
		StringCat( partitions, "/dev/");
		StringCat( partitions, d ) ;
		StringCat( partitions, "\n" ) ;
	}
		
	StringDelete( non_system_partitions ) ;
	
	non_system_partitions = StringCpy( StringCont( partitions ) ) ;
	
	fclose(f);
	
	f = fopen("/etc/fstab","r");
	
	while ( fgets(buffer,512,f ) != NULL ){	
		
		if( strncmp( buffer , "/dev/", 5 ) == 0 ){
			
			c = buffer ;
			
			while ( *++c != ' ' ) { ; }
			
			*c = '\0' ;
			
			StringCat( fstab_partitions, buffer ) ;		
			StringCat( fstab_partitions, "\n");	
			
			i = StringPosString( non_system_partitions,buffer) ;
			
			StringStringRemove( non_system_partitions, i , strlen(buffer) + 1 ) ;
			
		}else if ( strncmp(buffer ,"UUID",4) == 0 ){
			
			c = buffer + 5 ;
			d = uuid ;
			
			while ( ( *d++ = *c++ ) != ' ' ) { ; }
			
			*--d = '\0' ;
			
			command = StringCpy(ZULUCRYPTblkid) ;
			StringCat( command , " -U " ) ;
			StringCat( command , uuid);
			
			z = popen( StringCont( command ), "r" ) ;
			
			fgets( buffer, 512, z ) ;
			
			pclose( z ) ;
			
			c = buffer ;
			
			while ( *++c != '\n' ) { ; }
			
			*c = '\0' ;
			
			StringCat( fstab_partitions, buffer ) ;	
			
			StringCat( fstab_partitions, "\n");
			
			StringDelete( command ) ;	
			
			i = StringPosString( non_system_partitions,buffer) ;
			
			StringStringRemove( non_system_partitions, i , strlen(buffer) + 1) ;
			
		}else if ( strncmp(buffer ,"LABEL",5) == 0 ){
			
			c = buffer + 6 ;
			d = label ;
			
			if ( *c == '\"'){
				c++ ;
				while ( ( *d++ = *c++ ) != '\"' ) { ; } 
			}else
				while ( ( *d++ = *c++ ) != ' ' ) { ; }
			
			*--d = '\0' ;

			command = StringCpy(ZULUCRYPTblkid) ;
			StringCat( command , " -L " ) ;
			StringCat( command , label);
			
			z = popen( StringCont( command ), "r" ) ;
			
			fgets( buffer, 512, z ) ;
			
			pclose( z ) ;
			
			c = buffer ;
			
			while ( *++c != '\n' ) { ; }
			
			*c = '\0' ;
			
			StringCat( fstab_partitions, buffer ) ;	
			
			StringCat( fstab_partitions, "\n");
			
			StringDelete( command ) ;
			
			i = StringPosString( non_system_partitions,buffer) ;
			
			StringStringRemove( non_system_partitions, i , strlen(buffer) + 1) ;
		}		
	}
}

int create_volumes(int argn ,char *device, char *fs, char * mode, char * keyType, char * pass )
{
	StrHandle * p ;
	StrHandle * q ;
	StrHandle * k ;
	char Y ;
	int st ;
	struct stat xt ;
	char *c ;
	int z ;
	off_t fsize ;
	
	p = StringCpy("");
	q = StringCpy("");
	k = StringCpy("");	
	
	partitions( p, q, k ) ;
	
	if ( strstr( StringCont( p ) , device ) != NULL ){
		
		printf("ERROR: creating volumes on system partitions is not allowed.\n");
		printf("System partitions have active entries in /etc/fstab") ;
		StringDelete( p ) ;
		StringDelete( q ) ;
		StringDelete( k ) ;
		return 7 ;
	}		
	StringDelete( k ) ;
	StringDelete( p ) ;
	StringDelete( q ) ;
	
	if( argn == 5 ){
		printf("ARE YOU SURE YOU WANT TO CREATE/OVERWRITE: \"%s\" ? Type \"Y\" if you are\n",device);
		
		scanf("%c",&Y);
		
		if ( Y != 'Y')
			st = 5 ;
		else{			
			getchar();    //get rid of "\n" still in stdin buffer	
			
			printf("Enter passphrase: ") ;

			p = get_passphrase();
			
			printf("\nRe enter passphrase: ") ;

			q = get_passphrase();	
			
			printf("\n") ;
			
			if(strcmp(StringCont( p ),StringCont( q )) != 0){

				st = 3 ;
			}else{
				st = create_volume(device,fs,mode,StringCont( p )) ;
				StringDelete( q ) ;
				StringDelete( p ) ;			
			}
		}
			
	}else if ( argn == 7 ){
		
		if( strcmp( keyType, "-p" ) == 0 ) {			

			st = create_volume(device,fs,mode,pass) ;			
			
		}else if( strcmp( keyType, "-f" ) == 0 ) {
			
			if( stat( pass, &xt) == 0 ) {
			
				if( xt.st_size < MAX )
					fsize = xt.st_size ;
				else
					fsize = MAX ;
				
				c = ( char *) malloc ( sizeof(char) * ( fsize + 1 ) ) ;
				
				*( c + fsize  ) = '\0' ;
			
				z = open(pass , O_RDONLY ) ;
			
				read( z, c, fsize ) ;
			
				close( z ) ;				
				
				st = create_volume(device,fs,mode,c) ;
				
				free( c ) ;
			}else{
				st = 1 ;
			}				
		}else{
			st = 6 ;			
		}
	}else{
		st = 4 ;			
	}
	
	switch ( st ){
		case 0 : printf("SUCCESS: volume successfully created\n") ;
			break  ;
		case 1 : printf("ERROR: File path given does not point to a file or partition\n") ;
			break  ;
		case 2 : printf("ERROR: Unrecognized volume type.\n");
			break  ;
		case 3 : printf("ERROR: passphrases do not match\n") ;
			break  ;
		case 4 : printf("ERROR: Wrong number of arguments\n");
			break  ;	
		case 5 : printf("ERROR: Wrong choice, exiting\n");
			break  ;
		case 6 : printf("ERROR: Wrong option type\n");
			break  ;
		default:
			;
	}	
	return st ;
}

void delete_file( char * file )
{
	int i, j, k  ;
	struct stat st ;	
	char X = 'X' ;
	
	stat(file, &st) ;

	k = open("/dev/urandom",O_RDONLY);
	i = open( file, O_WRONLY ) ;
	
	for( j = 0 ; j < st.st_size ; j ++ ){
		read(  k , &X, 1 ) ;
		write( i , &X, 1 ) ;	
	}
	close( i ) ;
	close( k ) ;
	remove( file ) ;	
}

int addkey(int argn,char * device, char *keyType1, char * existingKey, char * keyType2, char * newKey)
{
	StrHandle * p ;
	StrHandle * q ;
	StrHandle * n ;
	
	struct stat st1 ;
	int status = 0 ;
	int z ;
	char * c ;
	
	off_t fsize ;
	
	if ( argn == 3 ){		
		
		printf("Enter an existing passphrase: ") ;
		
		p = get_passphrase() ;
		
		printf("\n") ;				
			
		printf("Enter the new passphrase: ") ;
		
		q = get_passphrase() ;
		
		printf("\n") ;	
			
		printf("Re enter the new passphrase: ") ;
		
		n = get_passphrase() ;
		
		printf("\n") ;
		
		if (strcmp( StringCont( q ), StringCont( n ) ) != 0){
			
			status = 2 ;
		}else{
		
			z = open("/tmp/.zuluCrypt-tmp",O_WRONLY | O_CREAT | O_TRUNC ) ;

			chown("/tmp/.zuluCrypt-tmp",0,0) ;
			chmod("/tmp/.zuluCrypt-tmp",S_IRWXU) ;
		
			write(z,StringCont( q ),strlen(StringCont( q ))) ;
		
			close( z ) ;
			
			status = add_key( device,StringCont( p ), "/tmp/.zuluCrypt-tmp" ) ;
			
			delete_file("/tmp/.zuluCrypt-tmp") ;				

			StringDelete( p ) ;			
			StringDelete( q ) ;	
			StringDelete( n ) ;	
		}
	}else if( argn == 7 ){			
		
		if ( strcmp( keyType1, "-f" ) == 0 ){			

			if( stat( existingKey, &st1) == 0 ) {
			
				if( st1.st_size < MAX )
					fsize = st1.st_size ;
				else
					fsize = MAX ;
				
				c = ( char *) malloc ( sizeof(char) * ( fsize + 1 )) ;
				
				*( c + fsize ) = '\0' ;
			
				z = open(existingKey, O_RDONLY ) ;
			
				read( z, c, fsize ) ;
			
				close( z ) ;				
				
			}else{
				status = 3 ;
				goto ouch ;
			}
		}
		
		if ( strcmp( keyType2, "-p" ) == 0){			
			
			z = open("/tmp/.zuluCrypt-tmp",O_WRONLY | O_CREAT | O_TRUNC ) ;
			
			chown("/tmp/.zuluCrypt-tmp",0,0) ;
			chmod("/tmp/.zuluCrypt-tmp",S_IRWXU) ;

			write( z,newKey,strlen(newKey)) ;
		
			close( z ) ;		
		}
		
		if ( strcmp(keyType1,"-f") == 0 && strcmp(keyType2,"-f") == 0 ){
			
			status = add_key( device, c, newKey) ;
			
			free( c ) ;
			
		}else if (strcmp(keyType1,"-p") == 0 && strcmp(keyType2,"-p") == 0 ){
			
			status = add_key(device, existingKey, "/tmp/.zuluCrypt-tmp" ) ;
						
			delete_file("/tmp/.zuluCrypt-tmp") ;	
			
		}else if (strcmp(keyType1,"-p") == 0 && strcmp(keyType2,"-f") == 0 ){
						
			status = add_key( device, existingKey, newKey) ;
						
		}else if (strcmp(keyType1,"-f") == 0 && strcmp(keyType2,"-p") == 0 ){			
					
			status = add_key( device, c, "/tmp/.zuluCrypt-tmp") ;	
			
			delete_file("/tmp/.zuluCrypt-tmp") ;	
	
			free( c ) ;
		}else{			
			status = 5 ;
		}
	}else{
		status = 6 ;		
	}
	
	ouch:
	
	switch ( status ){
		case 0 : printf("SUCCESS: key added successfully\n");
		break ;		
		case 1 : printf("ERROR: presented key does not match any key in the volume\n") ;
		break ;
		case 2 : printf("ERROR: new passphrases do not match\n") ;
		break ;
		case 3 : printf("ERROR: key file containing a key in the volume does not exist\n") ;
		break ;  
		case 4 : printf("ERROR: device does not exist\n");
		break ;
		case 5 : printf("ERROR: Wrong arguments\n") ;
		break ;
		case 6 : printf("ERROR: Wrong number of arguments\n") ;
		break ;
	
		default :
			;		
	}
	return status ;
}

int killslot(int argn, char * device, char * keyType, char * existingkey, char * s)
{
	int status, i, d ;
	char * c ;
	struct stat st ;
	StrHandle * p ;
	off_t fsize ;
	
	int slotNumber = s[0]  ;
	
	if ( argn == 3 ){
		
		printf("Enter an existing passphrase: ") ;
		
		p = get_passphrase() ;
		
		printf("\n") ;
		
		printf("Enter a slot number to remove a key on: ") ;
		
		d = ( char ) getchar() ;
		
		getchar() ; //remove the new line character from stdin buffer		

		status = kill_slot( device, StringCont( p ), d ) ;
		
		StringDelete( p ) ;
		
	}else if ( argn == 6 ){
	
		if( strcmp( keyType, "-p" ) == 0 ){		

			status =  kill_slot(device, existingkey , slotNumber ) ;	
			
		}else if ( strcmp( keyType, "-f" ) == 0 ){
		
			if ( stat( existingkey,&st ) != 0 ){
				return 4 ;
			}
			if( st.st_size < MAX )
				fsize = st.st_size ;
			else
				fsize = MAX ;
			
			c = ( char * ) malloc ( sizeof( char ) * ( fsize + 1 ) ) ;
		
			*( c + fsize ) = '\0' ;
			
			i = open( existingkey, O_RDONLY ) ;
		
			write( i , c , fsize ) ;
		
			close( i ) ;		
			
			status = kill_slot( device, c , slotNumber ) ;
		
			free( c ) ;				
			
		}else{
			status = 5 ;		
		}
	}else{		
		status = 6 ;		
	}	
	switch ( status ){
		case 0 : printf("SUCCESS: slot successfully killed\n");
		break ;
		case 1 : printf("ERROR: slot to be killed is inactive/empty\n") ;
		break ;
		case 2 : printf("ERROR: the device does not exist\n") ;
		break ;
		case 3 : printf("ERROR: presented key does not match any key in the volume\n") ;
		break ;  
		case 4 : printf("ERROR: key file does not exist\n");
		break ;
		case 5 : printf("ERROR: Wrong arguments\n") ;
		break ;
		case 6 : printf("ERROR: Wrong number of arguments\n") ;
		break ;
		default :
			;		
	}	
	return status ;
}

int removekey( int argn , char * device, char * keyType, char * keytoremove )
{
	StrHandle *p;
	int status, z ;
	struct stat st ;
	
	if ( argn == 3 ){
		
		printf("Enter the passphrase of the key you want to delete: ") ;
		
		p = get_passphrase() ;
		
		printf("\n") ;
		
		z = open("/tmp/.zuluCrypt-tmp",O_WRONLY | O_CREAT | O_TRUNC ) ;
			
		chown("/tmp/.zuluCrypt-tmp",0,0) ;
		chmod("/tmp/.zuluCrypt-tmp",S_IRWXU) ;

		write( z, StringCont( p ) ,StringLength( p )) ;
		
		close( z ) ;
		
		status = remove_key( device,"/tmp/.zuluCrypt-tmp" ) ;
		
		StringDelete( p ) ;
			
		delete_file("/tmp/.zuluCrypt-tmp");
		
	}else if ( argn == 5 ){
		
		if( strcmp(keyType, "-f") == 0 ){
			
			if ( stat(keytoremove,&st) == 0 )
				status = remove_key(device, keytoremove );
			else
				status = 5 ;
			
		}else if( strcmp(keyType, "-p") == 0 ) {
			
			z = open("/tmp/.zuluCrypt-tmp",O_WRONLY | O_CREAT | O_TRUNC ) ;
			
			chown("/tmp/.zuluCrypt-tmp",0,0) ;
			chmod("/tmp/.zuluCrypt-tmp",S_IRWXU) ;
			
			write( z, keytoremove ,strlen(keytoremove)) ;			
			
			close( z ) ;
		
			status = remove_key( device,"/tmp/.zuluCrypt-tmp" ) ;
		
			delete_file("/tmp/.zuluCrypt-tmp");			
		}
	}else
		status = 6 ;
	
	switch ( status ){
		case 0 : printf("SUCCESS: key successfully removed\n");
		break ;
		//case 1 : printf("") ;
		//break ;
		case 2 : printf("ERROR: there is no key in the volume that match the presented key\n") ;
		break ;
		//case 3 : printf("") ;
		//break ;  
		case 4 : printf("ERROR: device does not exist\n");
		break ;
		case 5 : printf("ERROR: keyfile does not exist\n") ;
		break ;
		case 6 : printf("ERROR: Wrong number of arguments\n") ;
		break ;
		default :
			;		
	}		
	return status ;	
}

int check_system_tools(void)
{	
	struct stat st ;
	StrHandle * p ;
	
	if( stat(ZULUCRYPTblkid,&st) == 0 && stat(ZULUCRYPTcryptsetup,&st) == 0 \
		&& stat(ZULUCRYPTdd,&st) == 0 && stat(ZULUCRYPTe2label,&st) ==0 \
		&& stat(ZULUCRYPTecho,&st) ==0 && stat(ZULUCRYPTlosetup,&st) ==0 \
		&& stat(ZULUCRYPTmkfs,&st) ==0 && stat(ZULUCRYPTmount,&st) ==0 \
		&& stat(ZULUCRYPTrm,&st) ==0 && stat(ZULUCRYPTumount,&st) ==0 ){
		
		return 0 ;
	}	
	p = StringCpy(ZULUCRYPTcryptsetup) ;
	StringCat( p , "\n" ) ;
	StringCat( p , ZULUCRYPTblkid) ;
	StringCat( p , "\n" ) ;	
	StringCat( p , ZULUCRYPTdd) ;
	StringCat( p , "\n" ) ;
	StringCat( p , ZULUCRYPTe2label) ;
	StringCat( p , "\n" ) ;
	StringCat( p , ZULUCRYPTecho) ;
	StringCat( p , "\n" ) ;
	StringCat( p , ZULUCRYPTlosetup) ;
	StringCat( p , "\n" ) ;
	StringCat( p , ZULUCRYPTmkfs) ;
	StringCat( p , "\n" ) ;
	StringCat( p , ZULUCRYPTmount) ;
	StringCat( p , "\n" ) ;
	StringCat( p , ZULUCRYPTrm) ;
	StringCat( p , "\n" ) ;
	StringCat( p , ZULUCRYPTumount) ;

	printf("this program can not work as expected on your system ");
	printf("because one or more of the following tools are either not present ") ;
	printf("or not where they are expected to be.\n%s\n",StringCont( p ));
	
	StringDelete( p ) ;
	
	return 1 ;
}
int main( int argc , char *argv[])
{
	char * action = argv[1] ;
	
	char * device = argv[2] ;

	StrHandle *p,*q,*z ;
	uid_t id ;
	
	int status ;
	
	char *  mapping_name ;
	char * c ;
	
	char slots[7] = {'0','0','0','0','0','0','0'};;
	
	id = getuid();	
	
	setuid(0);
	
	if( check_system_tools() == 1 )
		return 100 ;
	
	if ( argc < 2 ){
		help();
		return 10 ;
	}
	if ( strcmp( action, "-h" ) == 0 || strcmp( action, "--help" ) == 0 || strcmp( action, "-help" ) == 0 ){			
		help();	
		return 10 ;
	}
	
	if ( strcmp( action, "-v" ) == 0 || strcmp( action, "-version" ) == 0 || strcmp( action, "--version" ) == 0 ){		
		printf(VERSION_STRING) ;
		printf("\n");
		return 10 ;
	}
	
	if ( argc < 3 ){
		help();
		return 10 ;
	}
		
	if ( (c = strrchr(device,'/')) != NULL) {
		mapping_name =  c + 1  ;	
		
	}else{
		mapping_name =  device  ;			
	}		
	
	if( strcmp( action, "isLuks" ) == 0 ){
	
		status =  is_luks( device ) ;
		
	}else if ( strcmp( action, "status" ) == 0 ){			

		status = volume_info( mapping_name ) ;
		
	}else if ( strcmp( action, "close" ) == 0 ){			

		status =  close_opened_volume( mapping_name,device ) ;
		
	}else if ( strcmp( action, "open" ) == 0 ){
		
		status =  open_volumes(argc,device,mapping_name,id,argv[3],argv[4],argv[5],argv[6] ) ;		
		
	}else if(strcmp(action,"create") == 0 ){

		status =  create_volumes(argc ,device,argv[3],argv[4],argv[5],argv[6] ) ;	
		
	}else if(strcmp(action,"addkey") == 0 ){
		
		status =  addkey(argc,device,argv[3],argv[4],argv[5],argv[6]) ;
		
	}else if(strcmp(action,"killslot") == 0 ){
		
		status =  killslot(argc, device,argv[3],argv[4],argv[5] ) ;
	
	}else if(strcmp(action,"removekey") == 0 ){
				
		status =  removekey(argc, device, argv[3],argv[4] );
	
	}else if (strcmp(action,"createfile") == 0 ){
		
		status =  create_file(device,argv[3],argv[4],id) ; 
		
	}else if ( strcmp(action,"partitions") == 0 ){
		
		p = StringCpy("");
		q = StringCpy("");
		z = StringCpy("");
		
		partitions( p, q ,z ) ;
		
		if( argv[2][0] == '1' ) {
			status = 0 ;
			printf("%s",StringCont(p)) ;
		}else if ( argv[2][0] == '2' ){
			status = 0 ;
			printf("%s",StringCont(q)) ;
		}else if ( argv[2][0] == '3' ){
			status = 0 ;
			printf("%s",StringCont(z)) ;
		}else{
			printf("wrong argument\n");
			status = 1 ;
		}
		
		StringDelete( p ) ;
		StringDelete( q ) ;
		StringDelete( z ) ;	
		
		return status ;
	}else if(strcmp(action,"emptyslots") == 0 ){
		
		if ( empty_slots( slots , device ) == 0 ){			
			printf("%s\n",slots ) ;
			status =  0 ;
		}else{
			printf("ERROR: given device does not exist\n") ;
			status =  1 ;
		}
			
	}else{
		printf("ERROR: Wrong argument\n") ;
		status =  10 ;
	}
	
	return status ; 		
}