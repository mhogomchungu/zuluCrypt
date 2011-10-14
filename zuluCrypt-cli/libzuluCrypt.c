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
#include <libcryptsetup.h>

#include "String.h"   

#include "../executables.h"

//function prototypes

int is_luks(const char * device) ;
void execute( const char *command ,char *output, int size) ;
int mount_volume(const char *mapping_name,const char *m_point,const char * mode,uid_t id) ;
char * sanitize(const char *c ) ;
char * intToChar(char * x, int y,int z) ;

int add_key(const char * device, const char * existingkey,const  const char * newkey)
{
	struct crypt_device *cd = NULL;

	int i ;
	
	if( is_luks(device) == 1)
		return 8 ;
		
	i = crypt_init(&cd,device) ;
	
	if( i != 0 )
		return 7 ;
	
	i = crypt_load(cd, CRYPT_LUKS1, NULL) ;
	
	if( i != 0 )
		return 7 ;
	
	i = crypt_keyslot_add_by_passphrase(cd,CRYPT_ANY_SLOT,existingkey,strlen(existingkey),newkey,strlen(newkey)) ;
	
	if ( i < 0 )
		return 3 ;
	else
		return 0 ;	
	
	crypt_free(cd);	
}

int remove_key( const char * device , const char * pass )
{
	struct crypt_device *cd = NULL;

	int i ;
	
	if( is_luks(device) == 1)
		return 1 ;
		
	i = crypt_init(&cd,device) ;
	
	if( i != 0 )
		return 3 ;
	
	i = crypt_load(cd, CRYPT_LUKS1, NULL) ;
	
	if( i != 0 )
		return 3 ;

	i =  crypt_activate_by_passphrase(cd,"zuluCrypt-deleteKey",CRYPT_ANY_SLOT,pass,strlen(pass), 0);

	crypt_deactivate(cd,"zuluCrypt-deleteKey");
	
	if ( i < 0 ){
		crypt_free(cd);
		return 2 ;
	}
	
	i = crypt_keyslot_destroy(cd,i) ;
	
	crypt_free(cd);	
	
	if ( i < 0 )
		return 3 ;	
	else
		return i ;
}

int empty_slots( char * slots ,const char * device )
{
	struct crypt_device *cd = NULL;
	crypt_keyslot_info cki ;
	
	int i ;
	int j ;
	int k ;
	
	if( is_luks(device) == 1)
		return 1 ;
		
	i = crypt_init(&cd,device) ;
	
	if( i != 0 )
		return 2 ;
	
	i = crypt_load(cd, CRYPT_LUKS1, NULL) ;
	
	if( i != 0 )
		return 2 ;
	
	k = crypt_keyslot_max(CRYPT_LUKS1) ;
	
	for( j = 0 ; j < k ; j++){
		
		cki = crypt_keyslot_status(cd, j);
		
		switch ( cki ){
			case CRYPT_SLOT_INACTIVE :   slots[j] = '0' ; break ;
			case CRYPT_SLOT_ACTIVE :     slots[j] = '1' ; break ;
			case CRYPT_SLOT_INVALID :    slots[j] = '2' ; break ;
			case CRYPT_SLOT_ACTIVE_LAST: slots[j] = '3' ; break ;			
		}		
	}
	slots[j] = '\0' ;
	return 0 ;
}

char * intToChar(char * x, int y,int z)
{
	char *c =  x + y  ;
	
	*c = '\0' ;
	
	c-- ;
	
	do{
		*c-- = z % 10 + '0' ;
		
		z = z / 10 ;		
		
	}while( z != 0 ) ;
	
	return ++c ;
}

char * status( const char * device )
{		
	#define SIZE 31
	char keysize[ SIZE + 1 ] ;
	char loop[512] ;
	char *c ;
	char *d ;
	//crypt_status_info csi ;
	struct crypt_device *cd = NULL;
	struct crypt_device *cd1 = NULL;
	struct crypt_active_device cad ;
	
	StrHandle * p ;
	StrHandle * q ;
	
	const char *type ;
	int i ;

	i = crypt_init_by_name(&cd,device);
	
	if( i != 0 )
		return NULL ;
	
	i = crypt_get_active_device(cd1,device,&cad) ;
	
	if( i != 0 )
		return NULL ;
	
	type = crypt_get_type(cd) ;	
	
	p = StringCpy("type:\t\t");
	
	StringCat(p,type) ;

	StringCat(p,"\ncipher:\t\t");
	StringCat(p,crypt_get_cipher_mode(cd)) ;
	
	StringCat(p,"\nkeysize:\t");
	StringCat(p,intToChar(keysize,SIZE,8 *crypt_get_volume_key_size(cd))) ;
	StringCat(p," bits");
	
	StringCat(p,"\ndevice:\t\t");
	StringCat(p,crypt_get_device_name(cd)) ;
	
	if( strncmp(crypt_get_device_name(cd),"/dev/loop",9 ) == 0){
		
		q = StringCpy(ZULUCRYPTlosetup) ;
		StringCat(q," ");
		StringCat(q,crypt_get_device_name(cd)) ;
		execute(StringCont(q),loop,511) ;
		StringDelete( q ) ;
		
		c = loop ;
		
		while( *c++ != '(') { ; }
			
		d = c ;
		
		while( *++d != ')') { ; }
		
		*d = '\0' ;
		
		StringCat(p,"\nloop:\t\t");
		StringCat(p,c);
	}
	
	StringCat(p,"\noffset:\t\t");
	StringCat(p,intToChar(keysize,SIZE,crypt_get_data_offset(cd))) ;	
	StringCat(p," sectors");	
	
	StringCat(p,"\nsize:\t\t");
	StringCat(p,intToChar(keysize,SIZE,cad.size)) ;	
	StringCat(p," sectors");
	
	StringCat(p,"\nmode:\t\t");
	
	if( cad.flags == 1 )
		StringCat(p,"readonly");
	else
		StringCat(p,"read/write");	
		
	c = StringContCopy(p) ;	
	crypt_free(cd);
	StringDelete(p) ;
	return c ;
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

char * sanitize(const char *c )
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

void execute( const char *command , char *output, int size)
{		
	FILE *f ;
	int i,c  ;
	//log outputs of command
	//printf("%s\n",command);
	//int d ;
	//d = open("/home/ink/zzz",O_WRONLY | O_APPEND | O_CREAT ) ;
	//write(d,command,strlen(command)) ;
	//write(d,"\n",1) ;
	//close(d);
	
	f = popen(command, "r") ;
	
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

int is_luks(const char * dev)
{		
	struct crypt_device *cd = NULL;
	int r;

	r = crypt_init(&cd, dev) ;	
	
	if( r == 0 )
		r = crypt_load(cd, CRYPT_LUKS1, NULL);
	
	if( r == 0 )
		return 0 ;
	else 
		return 1 ;
}

int create_volume(const char * dev, const char * fs,const char * type, const char * pass, const char *rng)
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
		StringCat( p , " ");
		StringCat( p , passphrase ) ;
		StringCat( p , " | " ) ;
		
		StringCat( p , ZULUCRYPTcryptsetup ) ;
		
		if(strcmp(rng,"/dev/random") == 0)
			StringCat( p , " --use-random " ) ;
		else
			StringCat( p , " --use-urandom " ) ;
		
		StringCat( p , " -q luksFormat " ) ;		
		
		StringCat( p , device ) ;
		
		execute( StringCont( p ), NULL, 0 ) ;
		
		StringDelete( p ) ;	
		
		p = StringCpy( ZULUCRYPTecho );
		StringCat( p , " ");
		StringCat( p , passphrase ) ;
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
		StringCat( p , " ");
		StringCat( p , passphrase ) ;
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

int close_volume(const char * map,const char * device) 
{
	StrHandle * q ;
	StrHandle * a ;		

	struct stat st ;		

	int i ;
	char * c ;
	char * x ;
	char * p ;
	const char * z ;
	char u[2] ;
	
	char * mount_point ;
	char * mapping_name ;
	
	q = StringCpy("/dev/mapper/zuluCrypt-") ;
	
	StringCat( q , map ) ;
	
	if ( stat( StringCont( q ), &st ) != 0 ){
		
		StringDelete( q ) ;
		
		return 1 ;
	}
	
	mapping_name = sanitize( StringCont( q ) ) ;	
	
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
	
	StringDelete( q ) ;
	
	free(c) ;
	
	mount_point = sanitize( x ) ;
	
	a = StringCpy(ZULUCRYPTumount) ;
	StringCat( a , " ") ;
	StringCat( a , mount_point ) ;
	StringCat( a , " 2>/dev/null 1>&2 ; ") ;
	StringCat( a , ZULUCRYPTecho) ;
	StringCat( a , " $? ") ;	
	
	execute( StringCont( a ), u,1 ) ;	
	
	StringDelete( a ) ;	
	
	if( u[0] != '0' ){
		free(mapping_name);
		free(mount_point) ;
		return 2 ;	
	}	

	if ( is_luks( device ) == 0 ){
		a = StringCpy(ZULUCRYPTcryptsetup ) ;
		StringCat( a ," luksClose ") ;
		StringCat( a , mapping_name ) ;
	}else{
		a = StringCpy(ZULUCRYPTcryptsetup ) ;
		StringCat( a ," remove ") ;
		StringCat( a , mapping_name ) ;		
	}
	
	StringCat( a, " ; ") ;
	StringCat( a, ZULUCRYPTrm ) ;
	StringCat( a," -rf ") ;
	StringCat( a, mount_point) ;
	
	execute( StringCont( a ),NULL,0) ;
	
	StringDelete( a ) ;
		
	free(mapping_name);
	free(mount_point);		
	
	return 0 ;
}

int mount_volume(const char *mapping_name,const char *m_point,const char * mode,uid_t id)
{
	StrHandle *p ;
	StrHandle *z ;
	StrHandle *q ;
	
	char *mount_point ;
	
	char s[2] ;

	if ( m_point == NULL )	
		p = StringCpy( "/mnt/" ) ;
	else{
		p = StringCpy(m_point) ;
		StringCat( p ,"/");
	}
	
	StringCat( p , mapping_name ) ;
	
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

	mount_point = sanitize( StringCont( p ) ) ;
	
	StringCat( q , mount_point ) ;
	
	StringCat( q , "  2>/dev/null 1>&2 ; ") ;
	StringCat( q , ZULUCRYPTecho ) ;
	StringCat( q , " $?");
	
	execute(StringCont( q ),s,1) ;	
	
	if( s[0] != '0' ){
		
		remove( StringCont( p ) ) ;
		StringDelete( q ) ;
		StringDelete( p ) ;
		StringDelete( z ) ;
		free(mount_point ) ;
		return 4 ;
	}
		
	chown( StringCont( p ), id, id ) ;
	
	chmod( StringCont( p ), S_IRWXU ) ;
	
	StringDelete( q ) ;
	StringDelete( p ) ;
	StringDelete( z ) ;
	free(mount_point ) ;
	
	return 0 ;		
}
int open_volume(const char *dev, const char * map, const char *m_point, uid_t id,const char * mode, const char *pass) 
{
	char status[2] ;	
	StrHandle * p ;	
	StrHandle * z ;
	struct stat st ;
	int h ;
	int luks;
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
	
	luks = is_luks( dev ) ;	

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
	StringCat( p , " ");
	StringCat( p , passphrase ) ;
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
		
		StringCat( p , " 1>/dev/null 2>&1 ") ;	
	
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
				StringCat( z , " ");
				StringCat( z , passphrase ) ;
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

