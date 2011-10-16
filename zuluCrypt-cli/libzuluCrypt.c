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

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mount.h>
#include <fcntl.h>
//#include <termios.h>
//#include <libcryptsetup.h>
#include <stdint.h>

#include "String.h"   

#include "../executables.h"

//function prototypes

int add_key(const char * device,
	    const char * existingkey,
	    const char * newkey) ;

int remove_key( const char * device ,
		const char * pass ) ;

int empty_slots( char * slots ,
		 const char * device ) ;

char * intToChar(char * x,
		 int y,
		 int z) ;

char * status( const char * mapper ) ;


char * sanitize(const char *c ) ;

void execute( const char *command ,
	      char *output,
	      int size) ;

int is_luks(const char * dev) ;


int create_luks(const char * dev,
		const char * pass,
		const char *rng) ;


int create_volume(const char * dev,
		  const char * fs,
		  const char * type,
		  const char * pass,
		  const char *rng) ;


int close_volume(const char * map) ;


int mount_volume(const char * mapping_name,
		 const char * m_point,
		 const char * mode,uid_t id) ;

int open_luks( const char * device,
	       const char * mapping_name,
	       const char * mode,
	       const char * source,
	       const char * pass ) ;

int open_plain( const char * device,
		const char * mapping_name,
		const char * mode,
		const char * source,
		const char * pass,
		const char * cipher ) ;

int open_volume(const char * dev,
		const char * map,
		const char * m_point,
		uid_t id,
		const char * mode,
		const char * pass,
		const char * source) ;		
		
		
#define CRYPT_PLAIN "PLAIN" 
#define CRYPT_LUKS1 "LUKS1" 		
#define CRYPT_ANY_SLOT -1
#define CRYPT_RNG_URANDOM 0
#define CRYPT_RNG_RANDOM  1
		
struct crypt_device *cd ;

struct crypt_active_device {
	uint64_t offset;	/* offset in sectors */
	uint64_t iv_offset;	/* IV initilisation sector */
	uint64_t size;		/* active device size */
	uint32_t flags;		/* activation flags */
};

struct crypt_params_plain {
	const char *hash; /* password hash function */
	uint64_t offset;  /* offset in sectors */
	uint64_t skip;    /* IV initilisation sector */
};

struct crypt_params_luks1 {
	const char *hash;      /* hash used in LUKS header */
	size_t data_alignment; /* in sectors, data offset is multiple of this */
};

void * handle = NULL ;

int ( * ptr_crypt_init ) ( struct crypt_device **, const char * ) ;
	
int ( * ptr_crypt_load ) ( struct crypt_device *, const char *, void * ) ;
	
int ( * ptr_crypt_keyslot_add_by_passphrase ) ( struct crypt_device *,
						int,const char *, 
						size_t, const char *,
						size_t ) ;
	
void ( *ptr_crypt_free ) ( struct crypt_device * ) ;
	


	
int ( * ptr_crypt_activate_by_passphrase) ( struct crypt_device * ,
						    const char *,
					            int,const char *,
						    size_t,uint32_t) ;
	
int ( * ptr_crypt_deactivate ) ( struct crypt_device * ,
				    const char * ) ;			    				    
	 
int ( * ptr_crypt_keyslot_destroy ) ( struct crypt_device *, int ) ;
		
int ( * ptr_crypt_keyslot_max ) ( const char * ) ;	

typedef enum {
	CRYPT_SLOT_INVALID,
	CRYPT_SLOT_INACTIVE,
	CRYPT_SLOT_ACTIVE,
	CRYPT_SLOT_ACTIVE_LAST
} crypt_keyslot_info;

crypt_keyslot_info ( * ptr_crypt_keyslot_status ) ( struct crypt_device *, int ) ;

typedef enum {
	CRYPT_INVALID,
	CRYPT_INACTIVE,
	CRYPT_ACTIVE,
	CRYPT_BUSY
} crypt_status_info;
crypt_status_info ( * ptr_crypt_status) (struct crypt_device *, const char *);

int ( * ptr_crypt_init_by_name ) ( struct crypt_device **, const char *) ;
	
int ( * ptr_crypt_get_active_device ) ( struct crypt_device *,
						const char *,
						struct crypt_active_device * ) ;
						
const char * ( * ptr_crypt_get_type ) ( struct crypt_device * ) ;
	
const char * ( * ptr_crypt_get_cipher_mode ) ( struct crypt_device * ) ;
	
int ( * ptr_crypt_get_volume_key_size ) ( struct crypt_device * ) ;
	
	
const char * ( * ptr_crypt_get_device_name ) ( struct crypt_device * ) ;
	
uint64_t  ( * ptr_crypt_get_data_offset ) ( struct crypt_device * ) ;
	
void ( * ptr_crypt_free ) ( struct crypt_device * ) ;

void ( * ptr_crypt_set_rng_type ) (struct crypt_device *, int ) ;

int ( * ptr_crypt_activate_by_keyfile )(struct crypt_device *,
				        const char *,
				        int,const char *,
				        size_t,
				        uint32_t );

int ( * ptr_crypt_format)(struct crypt_device *,
			 const char *,
			 const char *,
			 const char *,
			 const char *,
			 const char *,
			 size_t,
			 void *);

int ( * ptr_crypt_keyslot_add_by_volume_key)(struct crypt_device *,
					    int,
					    const char *,
					    size_t,
					    const char *,
					    size_t);

void handle_init(void)
{

	if( ( handle = dlopen("/usr/local/lib/libcryptsetup.so",RTLD_LAZY | RTLD_LOCAL ) ) == NULL )
		if( ( handle = dlopen("/usr/lib/libcryptsetup.so",RTLD_LAZY | RTLD_LOCAL ) ) == NULL )
			if( ( handle = dlopen("/lib/libcryptsetup.so",RTLD_LAZY | RTLD_LOCAL ) ) == NULL )
				return ;
				  
	ptr_crypt_init = dlsym( handle , "crypt_init" ) ;
	
	ptr_crypt_load = dlsym( handle , "crypt_load" ) ;	
	
	ptr_crypt_keyslot_add_by_passphrase = dlsym( handle, "crypt_keyslot_add_by_passphrase" ) ;
	
	ptr_crypt_free = dlsym( handle , "crypt_free" ) ;
	
	ptr_crypt_deactivate = dlsym( handle , "crypt_deactivate" ) ;
	
	ptr_crypt_keyslot_destroy = dlsym( handle , "crypt_keyslot_destroy" ) ;
	
	ptr_crypt_keyslot_max = dlsym( handle , "crypt_keyslot_max" ) ;
	
	ptr_crypt_keyslot_status = dlsym( handle , "crypt_keyslot_status" ) ; 
	
	ptr_crypt_init_by_name = dlsym( handle , "crypt_init_by_name" ) ;
	
	ptr_crypt_get_active_device = dlsym( handle , "crypt_get_active_device" ) ;
		
	ptr_crypt_status = dlsym( handle , "crypt_status" ) ;

	ptr_crypt_get_type = dlsym( handle , "crypt_get_type" ) ;

	ptr_crypt_get_cipher_mode = dlsym( handle , "crypt_get_cipher_mode" ) ;

	ptr_crypt_get_volume_key_size = dlsym( handle , "crypt_get_volume_key_size" ) ;
	
	ptr_crypt_get_device_name = dlsym( handle , "crypt_get_device_name" ) ;
	
	ptr_crypt_get_data_offset = dlsym( handle , "crypt_get_data_offset" ) ;
	
	ptr_crypt_free = dlsym( handle , "crypt_free" ) ;
	
	ptr_crypt_set_rng_type = dlsym( handle , "crypt_set_rng_type" ) ;
	
	ptr_crypt_format = dlsym( handle , "crypt_format" );
	
	ptr_crypt_keyslot_add_by_volume_key = dlsym( handle , "crypt_keyslot_add_by_volume_key" );
	
	ptr_crypt_activate_by_keyfile = dlsym( handle, "crypt_activate_by_keyfile") ;
	
	ptr_crypt_activate_by_passphrase = dlsym( handle, "crypt_activate_by_passphrase");
}


int add_key(const char * device, const char * existingkey,const  const char * newkey)
{
	int status ;
	
	int i ;
	
	handle_init() ;
	
	if( handle == NULL )		
		return 40 ;
	
	
	if( is_luks(device) == 1){		
		status =  3 ;
		goto out ;
	}
		
	i = (*ptr_crypt_init)(&cd,device) ;
	
	if( i != 0 ){		
		status =  2 ;
		goto out ;
	}
	i = (*ptr_crypt_load)(cd, CRYPT_LUKS1, NULL) ;
	
	if( i != 0 ){		
		status =  2 ;
		goto out ;
	}
	i = (*ptr_crypt_keyslot_add_by_passphrase)(cd,CRYPT_ANY_SLOT,
						   existingkey,
						   strlen(existingkey),
						   newkey,strlen(newkey)) ;
						   
	if ( i < 0 )
		status =  1 ;
	else
		status = 0 ;	
	
	out:
	
	(*ptr_crypt_free)(cd);
	dlclose(handle);	
	
	return status ;
}

int remove_key( const char * device , const char * pass )
{
	int i ;	
	int status ;
	
	if( is_luks(device) == 1)
		return 1 ;	
	
	handle_init() ;
	
	if(  handle  == NULL )
		return 40 ;
	
	i = (*ptr_crypt_init)(&cd,device) ;
	
	if( i != 0 ){
		status =  3 ;
		goto out ;
	}
	
	i = (*ptr_crypt_load)(cd, CRYPT_LUKS1, NULL) ;
	
	if( i != 0 ){
		status =  3 ;
		goto out ;
	}

	i =  (*ptr_crypt_activate_by_passphrase)(cd,
						 "zuluCrypt-deleteKey",
						 CRYPT_ANY_SLOT,
						 pass,
					         strlen(pass),
						 0);

	(*ptr_crypt_deactivate)(cd,"zuluCrypt-deleteKey");
	
	if ( i < 0 ){
		status = 2 ;
		goto out ;
	}
	
	i = (*ptr_crypt_keyslot_destroy)(cd,i) ;
	

	
	if ( i < 0 )
		status = 3 ;	
	else
		status = 0 ;
	
	out:
	(*ptr_crypt_free)(cd);	
	
	dlclose(handle);
	
	return status ;
}

int empty_slots( char * slots ,const char * device )
{
	struct crypt_device *cd = NULL;
	crypt_keyslot_info cki ;
	
	int i ;
	int j ;
	int k ;
	int status ;
	
	handle_init() ;
	
	if( is_luks(device) == 1)
		return 1 ;
	
	if(  handle  == NULL )
		return 40 ;

	i = (*ptr_crypt_init)(&cd,device) ;
	
	if( i != 0 ){
		status = 2 ;
		goto out ;
	}
	
	i = (*ptr_crypt_load)(cd, CRYPT_LUKS1, NULL) ;
	
	if( i != 0 ){
		status = 2 ;
		goto out ;
	}
	
	k = (*ptr_crypt_keyslot_max)(CRYPT_LUKS1) ;
	
	for( j = 0 ; j < k ; j++){
		
		cki = (*ptr_crypt_keyslot_status)(cd, j);
		
		switch ( cki ){
			case CRYPT_SLOT_INACTIVE :   slots[j] = '0' ; break ;
			case CRYPT_SLOT_ACTIVE :     slots[j] = '1' ; break ;
			case CRYPT_SLOT_INVALID :    slots[j] = '2' ; break ;
			case CRYPT_SLOT_ACTIVE_LAST: slots[j] = '3' ; break ;			
		}		
	}
	slots[j] = '\0' ;
	
	out:
	(*ptr_crypt_free)(cd);
	dlclose(handle);
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
	char *c = NULL ;
	char *d = NULL ;
	
	crypt_status_info csi ;
	
	struct crypt_device *cd1 = NULL;
	struct crypt_active_device cad ;	
	
	StrHandle * p ;
	StrHandle * q ;
	
	const char *type ;
	int i ;
	
	handle_init() ;
	
	if( handle  == NULL )
		return NULL ;		

	i = (*ptr_crypt_init_by_name)(&cd,mapper);

	//if( i != 0 ){
	//	c = NULL ;
	//	goto out ;
	//}
	
	i = (*ptr_crypt_get_active_device)(cd1,mapper,&cad) ;
	
	//if( i != 0 ){
	//	c = NULL ;
	//	goto out ;
	//}

	csi = (*ptr_crypt_status)(cd, mapper);
	
	p = StringCpy(mapper) ;
	
	switch( csi){
		case CRYPT_INACTIVE : 	StringCat(p," is inactive.\n") ; goto out ;	break ;
		case CRYPT_ACTIVE   : 	StringCat(p," is active.\n") ;			break ;
		case CRYPT_BUSY     : 	StringCat(p," is active and is in use.\n") ;	break ;
		case CRYPT_INVALID  : 	StringCat(p," is invalid.\n") ;	 goto out ;	break ;
	}	
		
	type = (*ptr_crypt_get_type)(cd) ;	
	
	StringCat(p," type:      ");
	
	StringCat(p,type) ;

	StringCat(p,"\n cipher:    ");
	StringCat(p,(*ptr_crypt_get_cipher_mode)(cd)) ;
	
	StringCat(p,"\n keysize:   ");
	StringCat(p,intToChar(keysize,SIZE,8 * (*ptr_crypt_get_volume_key_size)(cd))) ;
	StringCat(p," bits");
	
	StringCat(p,"\n device:    ");
	StringCat(p,(*ptr_crypt_get_device_name)(cd)) ;
	
	if( strncmp((*ptr_crypt_get_device_name)(cd),"/dev/loop",9 ) == 0){
		
		q = StringCpy(ZULUCRYPTlosetup) ;
		StringCat(q," ");
		StringCat(q,(*ptr_crypt_get_device_name)(cd)) ;
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
	StringCat(p,intToChar(keysize,SIZE,(*ptr_crypt_get_data_offset)(cd))) ;	
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
	StringDelete(p) ;
	
	out:
	
	(*ptr_crypt_free)(cd);	
	dlclose(handle) ;
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

	handle_init() ;
	
	if(  handle  == NULL )
		return 3 ;
	
	r = (*ptr_crypt_init)(&cd, dev) ;	
	
	if( r == 0 )
		r = (*ptr_crypt_load)(cd, CRYPT_LUKS1, NULL);	
	
	(*ptr_crypt_free)(cd);
	
	dlclose(handle) ;
	
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
	
	handle_init() ;
	
	if(  handle  == NULL )
		return 40 ;
	
	i =  (*ptr_crypt_init)(&cd,dev) ;
	
	if( strcmp(rng,"/dev/random" ) == 0 )
		(*ptr_crypt_set_rng_type)(cd, CRYPT_RNG_RANDOM);
	else 
		(*ptr_crypt_set_rng_type)(cd, CRYPT_RNG_URANDOM);
	
	i = (*ptr_crypt_format)(cd, CRYPT_LUKS1,"aes","cbc-essiv:sha256",NULL, NULL, 32, &params);	
	
	i = (*ptr_crypt_keyslot_add_by_volume_key)(cd,CRYPT_ANY_SLOT ,NULL,32,pass,strlen(pass));
	
	(*ptr_crypt_free)(cd);
	dlclose(handle) ;
	return i ;
}

int create_volume(const char * dev, const char * fs,const char * type, const char * pass, const char *rng)
{
	StrHandle * q ;
	
	struct stat st ;
	
	int k ;
	
	char * fsys = sanitize( fs ) ;
	
	if ( stat( dev, &st ) != 0 ){
		return 1 ;
	}
	
	if( strcmp(rng,"/dev/random") != 0)
		if( strcmp(rng,"/dev/urandom") != 0)
			return 6 ;
	
	if( strncmp(fs,"ext3",4) != 0)
		if( strncmp(fs,"ext4",4) != 0)
			if( strncmp(fs,"vfat",4) != 0)
				return 6 ;
	
	if(  strcmp(type,"luks")  == 0){
	
		k = create_luks(dev,pass,rng) ;
		
		k = open_luks(dev,"create-new","rw","-p",pass ) ;
		
	}else if( strcmp(type,"luks") == 0 ){
		k =  open_plain(dev,"create-new","rw","-p",pass,"cbc-essiv:sha256" ) ;		
	}else{
		return 6 ;
	}
		
	q = StringCpy(ZULUCRYPTmkfs );
	StringCat( q , " -t ") ;
	StringCat( q , fsys ) ;
	StringCat( q , " " ) ;
	StringCat( q , "/dev/mapper/zuluCrypt-create-new 1>/dev/null 2>&1 ") ;
	
	execute(StringCont(q),NULL,0) ;
	
	StringDelete( q ) ;
		
	free( fsys ) ;	
	
	handle_init() ;	
	
	(*ptr_crypt_deactivate)(NULL,"zuluCrypt-create-new");

	dlclose(handle) ;
	
	return 0 ;	
}

int close_volume(const char * map) 
{
	StrHandle * q ;
	StrHandle * p ;
	
	struct stat st ;		

	char * mount_point = NULL ;
	char * c ;
	
	FILE *f ;
	
	char buffer[256] ;
	
	handle_init() ;
	
	if( handle == NULL )		
		return 3 ;	
	
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
	
	if ( mount_point == NULL ){
		StringDelete(q) ;
		return 10 ;
	}
	mount_point = sanitize(mount_point) ;
	
	fclose(f);
	
	StringDelete( q ) ;

	/*
	 * mount/umount system calls do not add entries in mtab and 
	 * tools like kdiskfree do not see the volumes when they dont have
	 * entries in /etc/mtab and manually adding/removing entries 
	 * in /etc/mtab corrupts the file.
	 * workaround is to use the mount/umount executables to mount/unmount volumes. 
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

	(*ptr_crypt_deactivate)(NULL, StringCont(p));
	
	dlclose(handle);	
	
	StringDelete( p ) ;
	
	free(mount_point);	
	
	StringDelete( q ) ;
	
	return 0 ;
}

int mount_volume(const char * mapping_name,
		 const char * m_point,
		 const char * mode,uid_t id)
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
		
		handle_init();
		(*ptr_crypt_deactivate)(NULL,StringCont(p)) ;
		dlclose(handle);
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

int open_luks( const char * device,
	       const char * mapping_name,
	       const char * mode,
	       const char * source,
	       const char * pass )
{
	struct stat st ;
	uint32_t flags = 0;
	int i;
	
	StrHandle *p ;
	
	handle_init() ;
	
	if( handle == NULL )
		return 40 ;

	p = StringCpy("zuluCrypt-") ;
	
	StringCat(p,mapping_name) ;
	
	i = (*ptr_crypt_init)(&cd, device) ;
	
	if ( i != 0 ){
		(*ptr_crypt_free)(cd);	
		dlclose(handle) ;
		return 10 ;
	}

	i = (*ptr_crypt_load)(cd, CRYPT_LUKS1, NULL) ;
	
	if ( i != 0 ){
		(*ptr_crypt_free)(cd);
		dlclose(handle);
		return 10 ;
	}
	
	if(strcmp(mode,"ro") == 0 )
		flags = 1 ;
	else
		flags = 0 ;
	
	if(strcmp(source,"-p")==0){
		
		i = (*ptr_crypt_activate_by_passphrase)(cd,
							StringCont(p),
							CRYPT_ANY_SLOT,
							pass,
							strlen(pass),
							flags);

	}else{
		if ( stat(pass,&st) != 0 ){
			(*ptr_crypt_free)(cd);
			dlclose(handle);
			StringDelete(p);
			return 6 ;			
		}			
		
		i = (*ptr_crypt_activate_by_keyfile)(cd,
						     StringCont(p),
						     CRYPT_ANY_SLOT,
						     pass,
						     st.
						     st_size,
						     flags) ;
	}
	
	(*ptr_crypt_free)(cd);
	dlclose(handle) ;
	StringDelete(p);
	
	return i ;
}

int open_plain( const char * device,
		const char * mapping_name,
		const char * mode,
		const char * source,
		const char * pass,
		const char * cipher )
{
	int i ;
	
	int flags ;
	
	struct crypt_params_plain params = {
		.hash = "ripemd160",
		.skip = 0,
		.offset = 0,
	};
	
	struct stat st;
	
	StrHandle *p ;
	
	handle_init() ;
	
	if( handle == NULL )
		return 40 ;
	
	p = StringCpy("zuluCrypt-") ;
	StringCat(p,mapping_name) ;
	
	if(strcmp(mode,"ro") == 0 )
		flags = 1 ;
	else
		flags = 0 ;

	i = (*ptr_crypt_init)(&cd, device) ;
	
	i = (*ptr_crypt_format)(cd,
				CRYPT_PLAIN,
				"aes"
				,cipher,
				NULL,
				NULL,
				32,
				&params);
	
	if(strcmp(source,"-p")==0){
		
		i = (*ptr_crypt_activate_by_passphrase)(cd,
							StringCont(p),
							CRYPT_ANY_SLOT,
							pass,
							strlen(pass),
							flags);
	}else{
			
		stat(pass,&st) ;
		
		i = (*ptr_crypt_activate_by_keyfile)(cd,
						     StringCont(p),
						     CRYPT_ANY_SLOT,
						     pass,
						     st.st_size,
						     flags) ;
	}
	
	
	(*ptr_crypt_free)(cd);
	StringDelete(p);
	dlclose(handle) ;
	return i ;
}

int open_volume(const char * dev,
		const char * map,
		const char * m_point,
		uid_t id,
		const char * mode,
		const char * pass,
		const char * source) 
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
	
		handle_init();
		
		(*ptr_crypt_deactivate)(NULL,StringCont(p));
	
		dlclose(handle);
		
		StringDelete(p);		

		open_plain( dev,map,mode,source,pass,"cbc-plain" ) ;		
		
		h = mount_volume(map,m_point,mode,id ) ;	
	}
	
	return h ;	
}

