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
int open_luks( const char * device,const char * mapping_name, const char *mode,const char *source,const char * pass ) ;
int open_plain( const char * device,const char * mapping_name, const char *mode,const char *source,const char * pass,const char * cipher ) ;


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

char * status( const char * mapper )
{		
	#define SIZE 31
	char keysize[ SIZE + 1 ] ;
	char loop[512] ;
	char path[512];
	char *c ;
	char *d ;
	
	crypt_status_info csi ;
	
	struct crypt_device *cd = NULL;
	struct crypt_device *cd1 = NULL;
	struct crypt_active_device cad ;
		
	StrHandle * p ;
	StrHandle * q ;
	
	const char *type ;
	int i ;

	i = crypt_init_by_name(&cd,mapper);
	
	if( i != 0 )
		return NULL ;
	
	i = crypt_get_active_device(cd1,mapper,&cad) ;
	
	if( i != 0 )
		return NULL ;

	csi = crypt_status(cd, mapper);
	
	p = StringCpy(mapper) ;
	
	switch( csi){
		case CRYPT_INACTIVE : 	StringCat(p," is inactive.\n") ;
					c = StringContCopy(p) ;	
					crypt_free(cd);
					StringDelete(p) ;
					return c ;
					break ;
		case CRYPT_ACTIVE   : 	StringCat(p," is active.\n") ;
					break ;
		case CRYPT_BUSY     : 	StringCat(p," is active and is in use.\n") ;
					break ;
		case CRYPT_INVALID  : 	StringCat(p," is invalid.\n") ;
					c = StringContCopy(p) ;	
					crypt_free(cd);
					StringDelete(p) ;
					return c ;		
					break ;
	}	
		
	type = crypt_get_type(cd) ;	
	
	StringCat(p," type:      ");
	
	StringCat(p,type) ;

	StringCat(p,"\n cipher:    ");
	StringCat(p,crypt_get_cipher_mode(cd)) ;
	
	StringCat(p,"\n keysize:   ");
	StringCat(p,intToChar(keysize,SIZE,8 *crypt_get_volume_key_size(cd))) ;
	StringCat(p," bits");
	
	StringCat(p,"\n device:    ");
	StringCat(p,crypt_get_device_name(cd)) ;
	
	if( strncmp(crypt_get_device_name(cd),"/dev/loop",9 ) == 0){
		
		q = StringCpy(ZULUCRYPTlosetup) ;
		StringCat(q," ");
		StringCat(q,crypt_get_device_name(cd)) ;
		execute(StringCont(q),loop,510) ;
		StringDelete( q ) ;
		
		c = loop ;
		
		while( *c++ != '(') { ; }
			
		d = c ;
		
		while( *++d != ')') { ; }
		
		*d = '\0' ;
		
		i = 0 ;
	
		realpath(c,path) ;
		StringCat(p,"\n loop:      ");
		StringCat(p,path);
	}
	
	StringCat(p,"\n offset:    ");
	StringCat(p,intToChar(keysize,SIZE,crypt_get_data_offset(cd))) ;	
	StringCat(p," sectors");	
	
	StringCat(p,"\n size:      ");
	StringCat(p,intToChar(keysize,SIZE,cad.size)) ;	
	StringCat(p," sectors");
	
	StringCat(p,"\n mode:      ");
	
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
	
	crypt_free(cd);
	
	if( r == 0 )
		return 0 ;
	else 
		return 1 ;
}

int create_luks(const char * dev,const char * pass, const char *rng)
{
	int i ;
	struct crypt_device *cd = NULL;
	
	struct crypt_params_luks1 params = {
		.hash = "sha1",
		.data_alignment = 4096,
	};
	
	i =  crypt_init(&cd,dev) ;
	
	if( strcmp(rng,"/dev/random" ) == 0 )
		crypt_set_rng_type(cd, CRYPT_RNG_RANDOM);
	else 
		crypt_set_rng_type(cd, CRYPT_RNG_URANDOM);
	
	i = crypt_format(cd, CRYPT_LUKS1,"aes","cbc-essiv:sha256",NULL, NULL, 32, &params);	
	
	i = crypt_keyslot_add_by_volume_key(cd,CRYPT_ANY_SLOT ,NULL,32,pass,strlen(pass));
	
	crypt_free(cd);
	
	return i ;
}

int create_volume(const char * dev, const char * fs,const char * type, const char * pass, const char *rng)
{
	StrHandle * q ;
	
	struct stat st ;
	
	int k ;
	
	if ( stat( dev, &st ) != 0 ){
		return 1 ;
	}
	
	if( strncmp(fs,"ext3",4) != 0)
		if( strncmp(fs,"ext4",4) != 0)
			if( strncmp(fs,"vfat",4) != 0)
				return 6 ;				
	
	if(  strcmp(type,"luks")  == 0){
	
		k = create_luks(dev,pass,rng) ;
		
		k = open_luks(dev,"create-new","rw","-p",pass ) ;
		
	}else{
		k =  open_plain(dev,"create-new","rw","-p",pass,"cbc-essiv:sha256" ) ;		
	}		
		
	q = StringCpy(ZULUCRYPTmkfs );
	StringCat( q , " -t ") ;
	StringCat( q , fs ) ;
	StringCat( q , " " ) ;
	StringCat( q , "/dev/mapper/zuluCrypt-create-new 1>/dev/null 2>&1 ") ;
	
	execute(StringCont(q),NULL,0) ;
	
	StringDelete( q ) ;
	
	crypt_deactivate(NULL,"zuluCrypt-create-new");
		
	return 0 ;	
}

int close_volume(const char * map,const char * device) 
{
	StrHandle * q ;
	StrHandle * p ;
	
	struct stat st ;		

	char * mount_point ;
	char * c ;
	
	FILE *f ;
	
	char buffer[256] ;
	
	q = StringCpy("/dev/mapper/zuluCrypt-") ;
	
	StringCat( q , map ) ;
	
	p = StringCpy( StringCont( q ) ) ;
	
	if ( stat( StringCont( q ), &st ) != 0 ){
		
		StringDelete( q ) ;
		
		return 1 ;
	}	
	
	f = fopen("/etc/mtab","r") ;
	
	while( fgets(buffer,256,f) != NULL ){
		
		if( strncmp(buffer,StringCont( q ),StringLength( q ) )  == 0 ){
			
			mount_point = c = buffer + StringLength( q ) + 1  ;
			
			while ( *++c != ' ' ) { ; }
			
			*c = '\0' ;	
			
			break ;
		}		
	}
	
	mount_point = sanitize(mount_point) ;
	
	fclose(f);
	
	StringDelete( q ) ;

	/*
	 * mount/umount system calls do not add entries in mtab and tools like kdiskfree do not see the volumes.
	 * workaround is to use the mount/umount executables to mount/unmount volumes.	 * 
	 */
	
	q = StringCpy(ZULUCRYPTumount) ;
	StringCat(q, " ");
	StringCat(q, mount_point) ;
	StringCat(q, "  ; ") ;
	StringCat(q,ZULUCRYPTecho) ;
	StringCat(q, " $?") ;
		
	execute(StringCont(q),buffer,1) ;
	
	if(buffer[0] != '0'){
		StringDelete( q ) ;
		return 2 ;
	}
	
	StringDelete( q ) ;
	
	q = StringCpy(ZULUCRYPTrm) ;
	StringCat(q, " -rf ");
	StringCat(q, mount_point) ;

	execute(StringCont(q),NULL,0) ;
	
	crypt_deactivate(NULL, StringCont(p));
	
	free(mount_point);
	StringDelete( p ) ;
	StringDelete( q ) ;
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
		
		p = StringCpy("zuluCrypt-");
		StringCat(p,mapping_name) ;
		
		crypt_deactivate(NULL,StringCont(p)) ;
		
		StringDelete( p ) ;
		
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

int open_luks( const char * device,const char * mapping_name, const char *mode,const char *source,const char *pass )
{
	struct stat st ;
	struct crypt_device *cd = NULL;
	uint32_t flags = 0;
	int i;
	
	StrHandle *p = StringCpy("zuluCrypt-") ;
	StringCat(p,mapping_name) ;
	
	i = crypt_init(&cd, device) ;
	
	if ( i != 0 ){
		crypt_free(cd);	
		return 10 ;
	}
	
	i = crypt_load(cd, CRYPT_LUKS1, NULL) ;
	
	if ( i != 0 ){
		crypt_free(cd);	
		return 10 ;
	}
	
	if(strcmp(mode,"ro") == 0 )
		flags = 1 ;
	else
		flags = 0 ;
	
	if(strcmp(source,"-p")==0){
		
		i = crypt_activate_by_passphrase(cd, StringCont(p),CRYPT_ANY_SLOT, pass, strlen(pass), flags);

	}else{
		if ( stat(pass,&st) != 0 ){
			crypt_free(cd);
			StringDelete(p);
			return 6 ;			
		}			
		
		i = crypt_activate_by_keyfile(cd,StringCont(p),CRYPT_ANY_SLOT,pass,st.st_size,flags) ;
	}
		
	crypt_free(cd);
	StringDelete(p);

	return i ;
}

int open_plain( const char * device,const char * mapping_name, const char *mode,const char *source,const char *pass, const char *cipher )
{
	int i ;
	int flags ;
	
	struct stat st;
	
	StrHandle *p = StringCpy("zuluCrypt-") ;
	StringCat(p,mapping_name) ;
	
	struct crypt_device *cd = NULL;
	
	struct crypt_params_plain params = {
		.hash = "ripemd160",
		.skip = 0,
		.offset = 0,
	};
	
	if(strcmp(mode,"ro") == 0 )
		flags = 1 ;
	else
		flags = 0 ;

	i = crypt_init(&cd, device) ;
	
	i = crypt_format(cd, CRYPT_PLAIN,"aes",cipher,NULL, NULL,32,&params);
	
	if(strcmp(source,"-p")==0){
		
		i = crypt_activate_by_passphrase(cd,StringCont(p),CRYPT_ANY_SLOT,pass,strlen(pass),flags);
	}else{
			
		stat(pass,&st) ;
		
		i = crypt_activate_by_keyfile(cd,StringCont(p),CRYPT_ANY_SLOT,pass,st.st_size,flags) ;
	}
	
	crypt_free(cd);
	StringDelete(p);
	
	return i ;
}

int open_volume(const char *dev, const char * map, const char *m_point, uid_t id,const char * mode, const char *pass,const char * source) 
{
	char status[2] ;	
	StrHandle * p ;	
	struct stat st ;
	int h ;
	int luks;
	
	if( stat( dev , &st ) != 0 ){		 
		return 3 ;
	}
	
	p = StringCpy("/dev/mapper/zuluCrypt-");
	StringCat(p,map);
	
	
	if(stat(StringCont(p),&st) == 0 ){
		StringDelete(p) ;
		return 2 ;
	}

	StringDelete(p) ;
	
	if ( strncmp(dev,"/dev/",5) != 0 ){
	
		p = StringCpy( ZULUCRYPTlosetup  ) ;
		StringCat(p ," -f 2>/dev/null 1>&2 ;" );
		StringCat(p , ZULUCRYPTecho ) ;
		StringCat(p , "  $? " ) ;
		execute(StringCont( p ),status,1) ;
		StringDelete( p ) ;
		
		if ( status[0] != '0' ){
			return 1 ;	
		}
	}
	
	luks = is_luks( dev ) ;	

	if( luks == 0 )
		h = open_luks( dev,map,mode,source,pass ) ;
	else
		h = open_plain( dev,map,mode,source,pass,"cbc-essiv:sha256" ) ;

	//if(h != 0 )	
	//	return 9 ;
	
	h = mount_volume(map,m_point,mode,id ) ;	
	
	if( h == 4 ){
		
		p = StringCpy("zuluCrypt-");
		StringCat(p,map);
	
		crypt_deactivate(NULL,StringCont(p));
	
		StringDelete(p);	
		
		open_plain( dev,map,mode,source,pass,"cbc-plain" ) ;
		
		h = mount_volume(map,m_point,mode,id ) ;	
	}
	
	return h ;	
}

