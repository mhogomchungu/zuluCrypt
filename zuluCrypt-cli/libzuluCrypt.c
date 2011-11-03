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
#include <stdint.h>
#include <libcryptsetup.h>
#include "String.h"   
#include <sys/types.h>
#include <errno.h>

#include "../zuluCrypt-gui/executables.h"

//function prototypes

int add_key(const char * device,
	    const char * existingkey,
	    const char * newkey) ;

int remove_key( const char * device ,
		const char * pass ) ;

char * empty_slots( const char * device ) ;

char * intToChar(char * x,
		 int y,
		 int z) ;

char * status( const char * mapper ) ;


char * sanitize(const char *c ) ;

void execute( const char *command ,
	      char *output,
	      int size) ;

int is_luks(const char * dev) ;

int close_mapper( const char * mapper ) ;

int create_luks(const char * dev,
		const char * pass,
		const char * rng) ;

int create_volume(const char * dev,
		  const char * fs,
		  const char * type,
		  const char * pass,
		  const char * rng) ;

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

		
struct crypt_device *cd;

int add_key(const char * device,
	    const char * existingkey,
	    const char * newkey)
{
	int status ;
	
	int i ;
	
	if( is_luks(device) == 1){		
		status =  3 ;
		goto out ;
	}
		
	i = crypt_init(&cd,device) ;
	
	if( i != 0 ){		
		status =  2 ;
		goto out ;
	}
	
	i = crypt_load(cd, CRYPT_LUKS1, NULL) ;
	
	if( i != 0 ){		
		status =  2 ;
		goto out ;
	}
	
	i = crypt_keyslot_add_by_passphrase(cd,
					   CRYPT_ANY_SLOT,
					   existingkey,
					   strlen(existingkey),
					   newkey,
					   strlen(newkey)) ;
						   
	if ( i < 0 )
		status =  1 ;
	else
		status = 0 ;	
	
	out:
	
	crypt_free(cd);
	
	return status ;
}

int remove_key( const char * device ,
		const char * pass )
{
	int i ;	
	int status ;
	
	if( is_luks(device) == 1)
		return 1 ;	
	
	i = crypt_init(&cd,device) ;
	
	if( i != 0 ){
		status =  3 ;
		goto out ;
	}
	
	i = crypt_load(cd, CRYPT_LUKS1, NULL) ;
	
	if( i != 0 ){
		status =  3 ;
		goto out ;
	}

	i =  crypt_activate_by_passphrase(cd,
					 "zuluCrypt-deleteKey",
					 CRYPT_ANY_SLOT,
					 pass,
					 strlen(pass),
					 0);
	if ( i < 0 ){
		status = 2 ;
		goto out ;
	}
	
	crypt_deactivate(cd,"/dev/mapper/zuluCrypt-deleteKey");
	
	i = crypt_keyslot_destroy(cd,i) ;
	
	if ( i < 0 )
		status = 2 ;	
	else
		status = 0 ;
	
	out:
	crypt_free(cd);		
	
	return status ;
}

char * empty_slots( const char * device )
{
	crypt_keyslot_info cki ;
	
	int i ;
	int j ;
	int k ;
	
	char *slot ;
	
	if( is_luks(device) == 1)
		return NULL ;
	
	i = crypt_init(&cd,device) ;
	
	if( i != 0 )
		return NULL ;
	
	i = crypt_load(cd, CRYPT_LUKS1, NULL) ;
	
	if( i != 0 )
		return NULL ;
	
	k = crypt_keyslot_max(CRYPT_LUKS1) ;
	
	slot = ( char * ) malloc( sizeof( char ) * ( k + 1 ) ) ;
	
	for( j = 0 ; j < k ; j++){
		
		cki = crypt_keyslot_status(cd, j);
		
		switch ( cki ){
			case CRYPT_SLOT_INACTIVE :   slot[j] = '0' ; break ;
			case CRYPT_SLOT_ACTIVE :     slot[j] = '1' ; break ;
			case CRYPT_SLOT_INVALID :    slot[j] = '2' ; break ;
			case CRYPT_SLOT_ACTIVE_LAST: slot[j] = '3' ; break ;			
		}		
	}
	
	slot[j] = '\0' ;	

	crypt_free(cd);
	
	return slot ;
}

char * intToChar(char * x, int y,int z)
{
	char *c =  x + y  ;
	
	*c-- = '\0' ;	
	
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
	
	struct crypt_device *cd1 = NULL;
	
	crypt_status_info csi ;
	
	struct crypt_active_device cad ;	
	
	StrHandle * p ;
	StrHandle * q ;
	
	const char *type ;
	int i ;
	
	i = crypt_init_by_name(&cd,mapper);

	//if( i != 0 ){
	//	c = NULL ;
	//	goto out ;
	//}
	
	i = crypt_get_active_device(cd1,mapper,&cad) ;
	
	//if( i != 0 ){
	//	c = NULL ;
	//	goto out ;
	//}

	csi = crypt_status(cd, mapper);
	
	p = String(mapper) ;
	
	switch( csi ){
		case CRYPT_INACTIVE :
			StringAppend(p," is inactive.\n") ; 	
			goto out ;
		case CRYPT_ACTIVE   : 
			StringAppend(p," is active.\n") ;
			break ;
		case CRYPT_BUSY     : 	
			StringAppend(p," is active and is in use.\n") ;
			break ;
		case CRYPT_INVALID  : 
			StringAppend(p," is invalid.\n") ;	
			goto out ;
	}	
		
	type = crypt_get_type(cd) ;	
	
	StringAppend(p," type:      ");
	
	StringAppend(p,type) ;

	StringAppend(p,"\n cipher:    ");
	StringAppend(p,crypt_get_cipher_mode(cd)) ;
	
	StringAppend(p,"\n keysize:   ");
	StringAppend(p,intToChar(keysize,SIZE,8 * crypt_get_volume_key_size(cd))) ;
	StringAppend(p," bits");
	
	StringAppend(p,"\n device:    ");
	StringAppend(p,crypt_get_device_name(cd)) ;
	
	if( strncmp(crypt_get_device_name(cd),"/dev/loop",9 ) == 0){
		
		q = String(ZULUCRYPTlosetup) ;
		StringAppend(q," ");
		StringAppend(q,crypt_get_device_name(cd)) ;
		execute(StringContent(q),loop,510) ;
		StringDelete( q ) ;
		
		c = loop ;
		
		while( *c++ != '(') { ; }
			
		d = c ;
		
		while( *++d != ')') { ; }
		
		*d = '\0' ;		
	
		realpath(c,path) ;
		StringAppend(p,"\n loop:      ");
		StringAppend(p,path);
	}
	
	StringAppend(p,"\n offset:    ");
	StringAppend(p,intToChar(keysize,SIZE,crypt_get_data_offset(cd))) ;	
	StringAppend(p," sectors");	
	
	StringAppend(p,"\n size:      ");
	StringAppend(p,intToChar(keysize,SIZE,cad.size)) ;	
	StringAppend(p," sectors");
	
	StringAppend(p,"\n mode:      ");
	
	if( cad.flags == 1 )
		StringAppend(p,"readonly");
	else
		StringAppend(p,"read/write");			
	
	out:
	crypt_free(cd);	
	crypt_free(cd1);	
	return StringDeleteHandle(p) ;
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

void execute( const char *command ,
	      char *output,
	      int size)
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

int create_luks(const char * dev,
		const char * pass,
		const char * rng)
{
	int status ;
	
	struct stat st;
	
	struct crypt_params_luks1 params = {
		.hash = "sha1",
		.data_alignment = 4096,
	};	
	
	if( stat(dev,&st) != 0 )
		return 4 ;
	
	status =  crypt_init(&cd,dev) ;
	
	if ( status != 0 ){
		status = 1 ;
		goto out ;
	}
	
	if( strcmp(rng,"/dev/random" ) == 0 )
		crypt_set_rng_type(cd, CRYPT_RNG_RANDOM);
	else 
		crypt_set_rng_type(cd, CRYPT_RNG_URANDOM);
	
	status = crypt_format(cd,
			      CRYPT_LUKS1,
			      "aes",
			      "cbc-essiv:sha256",
			       NULL,
			       NULL,
		               32,
		               &params);	
	
	if ( status != 0 ){
		status = 2 ;
		goto out ;
	}
	
	status = crypt_keyslot_add_by_volume_key(cd,
						CRYPT_ANY_SLOT ,
						NULL,
						32,
						pass,
						strlen(pass));
	
	if ( status != 0 ){
		status = 3 ;
		goto out ;
	}
	
	status = 0 ;
	
	out:
	
	crypt_free(cd);
	
	return status ;
}

int create_volume(const char * dev,
		  const char * fs,
		  const char * type,
		  const char * pass,
		  const char * rng)
{
	StrHandle * q ;
	
	struct stat st ;
	
	int status ;
	
	char * fsys = sanitize( fs ) ;
	
	if ( stat( dev, &st ) != 0 ){
		return 1 ;
	}	
	
	if( strcmp(type,"luks") == 0 )
		if( strcmp(rng,"/dev/random") != 0)
			if( strcmp(rng,"/dev/urandom") != 0)
				return 2 ;
	
	if( strncmp(fs,"ext3",4) != 0)
		if( strncmp(fs,"ext4",4) != 0)
			if( strncmp(fs,"vfat",4) != 0)
				return 2 ;
	
	if(  strcmp(type,"luks")  == 0){
	
		status = create_luks(dev,pass,rng) ;		

		if( status != 0 )
			return 3 ;
		
		status = open_luks(dev,"zuluCrypt-create-new","rw","-p",pass ) ;
		
		if( status != 0 )
			return 3 ;
		
	}else if( strcmp(type,"plain") == 0 ){
		
		status = open_plain(dev,"zuluCrypt-create-new","rw","-p",pass,"cbc-essiv:sha256" ) ;
		
		if( status != 0 )
			return 3 ;		
	}else{
		return 2 ;
	}
		
	q = String(ZULUCRYPTmkfs );
	StringAppend( q , " -t ") ;
	StringAppend( q , fsys ) ;
	StringAppend( q , " " ) ;
	StringAppend( q , "/dev/mapper/zuluCrypt-create-new 1>/dev/null 2>&1 ") ;
	
	execute(StringContent(q),NULL,0) ;
	
	StringDelete( q ) ;
		
	free( fsys ) ;		
	
	/*
	 * I have on occassion seen this not being closed, assuming its due to my slow
	 * computer and hence putting a few seconds pause just in case
	 */
	sleep( 2 ) ;
	close_mapper("/dev/mapper/zuluCrypt-create-new");
	
	return 0 ;	
}

int close_mapper( const char * mapper )
{
	int i = crypt_deactivate(NULL, mapper);

	if(i == 0)
		return 0;
	else
		return 2 ;	
}

int unmount_volume( const char * map )
{
	StrHandle * p ;
	
	StrHandle * q ;
	
	struct stat st ;		

	char * mount_point = NULL ;
	
	char *c ;
	
	FILE *f ;	
	
	char buffer[256] ;
	
	if ( stat( map , &st ) != 0 )
		return 1 ;		
	
	f = fopen("/etc/mtab","r") ;
	while( fgets(buffer,256,f) != NULL ){
		
		if( strncmp(buffer,map,strlen(map) )  == 0 ){
			
			mount_point = c = buffer + strlen(map) + 1  ;
			while ( *++c != ' ' ) { ; }
			
			*c = '\0' ;	
			
			break ;
		}		
	}
	fclose(f);
	
	if ( mount_point == NULL )
		return 3 ;			

	p = String(mount_point) ;	
	
	/*
	 * space character in /etc/mtab file is stored as \040
	 * replace these characters with space again	 * 
	 */	
	StringReplaceString(p,"\\040"," ") ;	
	
	mount_point = sanitize(StringContent(p)) ;		

	/*
	 * mount/umount system calls do not add entries in mtab and 
	 * tools like kdiskfree do not see the volumes when they dont have
	 * entries in /etc/mtab and manually adding/removing entries 
	 * in /etc/mtab corrupts the file.
	 * workaround is to use the mount/umount executables to mount/unmount volumes. 
	 */
	
	q = String(ZULUCRYPTumount) ;
	StringAppend(q, " ");
	StringAppend(q, mount_point) ;
	StringAppend(q, "  1>/dev/null 2>&1 ; ") ;
	StringAppend(q,ZULUCRYPTecho) ;
	StringAppend(q, " $?") ;
	execute(StringContent(q),buffer,1) ;
	StringDelete( q ) ;
	
	if(buffer[0] != '0'){		
		StringDelete( p ) ;
		free(mount_point);
		return 2 ;		
	}
	
	rmdir( StringContent(p) ) ;	
	free(mount_point);	
	StringDelete( p ) ;	
	return 0 ;
}

int close_volume(const char * map) 
{		
	int i ;
	
	i = 0 ;
	
	i = unmount_volume( map ) ;
	
	if( i != 0 )
		return i ;	
	
	i = close_mapper( map ) ;		
	
	if( i == 1 )
		return 4 ;
	else
		return 0 ;
}

int mount_volume(const char * mapper,
		 const char * m_point,
		 const char * mode,uid_t id)
{
	StrHandle *p ;
	StrHandle *q ;
	
	char *mount_point ;
	char *sane_mapper ;
	
	char s[2] ;
	
	p = String(m_point) ;		
	
	if ( mkdir( StringContent( p ), S_IRWXU  ) != 0 ){
		StringAppend( p, ".zc") ;
		
		if ( mkdir( StringContent( p ),S_IRWXU  ) != 0 ){
			close_mapper(mapper);
			StringDelete( p ) ;
			return 5 ;		
		}
	}

	if ( strncmp( mode, "ro",2 ) == 0 )
		q = String(ZULUCRYPTmount " -r ") ;
	else
		q = String(ZULUCRYPTmount " -w ") ;
	
	sane_mapper = sanitize( mapper ) ;
	
	StringAppend( q , sane_mapper ) ;
	
	free( sane_mapper ) ;

	StringAppend( q , " ");

	mount_point = sanitize( StringContent( p ) ) ;
	
	StringAppend( q , mount_point ) ;
	
	StringAppend( q , "  2>/dev/null 1>&2 ; ") ;
	StringAppend( q , ZULUCRYPTecho ) ;
	StringAppend( q , " $?");
	
	execute(StringContent( q ),s,1) ;	
	
	if( s[0] != '0' ){
		
		remove( StringContent( p ) ) ;
		StringDelete( q ) ;
		StringDelete( p ) ;
		free(mount_point ) ;	
		
		close_mapper(mapper);
		
		return 4 ;
	}
		
	chown( StringContent( p ), id, id ) ;
	
	chmod( StringContent( p ), S_IRWXU ) ;
	
	StringDelete( q ) ;
	StringDelete( p ) ;
	free(mount_point ) ;
	
	return 0 ;		
}

int open_luks( const char * device,
	       const char * mapper,
	       const char * mode,
	       const char * source,
	       const char * pass )
{
	struct stat st ;
	uint32_t flags = 0;
	int i;
	
	const char * c ;	
	
	if( stat( device, &st) != 0 )
		return 3 ;
	
	c = strrchr(mapper,'/');
	
	/*
	 * second argument of "crypt_activate_by_passphrase" and "crypt_activate_by_keyfile"
	 * takes a name, not a path, below code will take the name or the last segment of the path
	 * making it irrelevant what a user passes to this function. 
	 * 
	 * blablabla will be taken as is
	 *\/dev/mapper/blablabla will be taken as blablabla only.These two functions will add they
	 *first part of the address.
	 * 
	 */	
	
	if( c == NULL )
		c = mapper ;
	else
		c++ ;
	
	i = crypt_init(&cd, device) ;
	
	if ( i != 0 ){
		crypt_free(cd);	
		return 2 ;
	}

	i = crypt_load(cd, CRYPT_LUKS1, NULL) ;
	
	if ( i != 0 ){
		crypt_free(cd);
		return 2 ;
	}
	
	if(strcmp(mode,"ro") == 0 )
		flags = 1 ;
	else
		flags = 0 ;
	
	if(strcmp(source,"-p")==0){
		
		i = crypt_activate_by_passphrase(cd,
						c,
						CRYPT_ANY_SLOT,
						pass,
						strlen(pass),
						flags);
	}else{
		if ( stat(pass,&st) != 0 ){
			crypt_free(cd);
			return 1 ;			
		}			
		
		i = crypt_activate_by_keyfile(cd,
					     c,
					     CRYPT_ANY_SLOT,
					     pass,
					     st.
					     st_size,
					     flags) ;
	}
	
	crypt_free(cd);
	
	if( i == 0 )
		return 0 ;
	else
		return 1 ;
}

int open_plain( const char * device,
		const char * mapper,
		const char * mode,
		const char * source,
		const char * pass,
		const char * cipher )
{
	int i ;
	
	int flags ;
	
	const char *c ;
	
	struct crypt_params_plain params = {
		.hash = "ripemd160",
		.skip = 0,
		.offset = 0,
	};
	
	struct stat st;
	
	if( stat(device,&st) != 0 )
		return 3 ;

	c = strrchr(mapper,'/');
	
	if( c == NULL )
		c = mapper ;
	else
		c++ ;
	
	if(strcmp(mode,"ro") == 0 )
		flags = 1 ;
	else
		flags = 0 ;

	i = crypt_init(&cd, device) ;	
	
	i = crypt_format(cd,
			CRYPT_PLAIN,
			"aes",
			cipher,
			NULL,
			NULL,
			32,
			&params);

	if(strcmp(source,"-p")==0){
		
		i = crypt_activate_by_passphrase(cd,
						c,
						CRYPT_ANY_SLOT,
						pass,
						strlen(pass),
						flags);
	}else{			
		if( stat(pass,&st) != 0){
			crypt_free(cd);
			return 4 ;
		}		
		i = crypt_activate_by_keyfile(cd,
					     c,
					     CRYPT_ANY_SLOT,
					     pass,
					     st.st_size,
					     flags) ;
	}
	crypt_free(cd);
	
	if ( i == 0 )
		return 0 ;
	else
		return 2 ;
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
	StrHandle * p = NULL ;
	struct stat st ;
	int h ;
	int luks;
	
	if( stat( dev , &st ) != 0 ){		 
		return 3 ;
	}
	
	if( strcmp( source,"-f") == 0 )
		if( stat( pass, &st ) != 0 )
			return 6 ;
	
	if(stat(map,&st) == 0 )
		return 2 ;	

	if ( strncmp(dev,"/dev/",5) != 0 ){
	
		p = String( ZULUCRYPTlosetup  ) ;
		StringAppend(p ," -f 2>/dev/null 1>&2 ;" );
		StringAppend(p , ZULUCRYPTecho ) ;
		StringAppend(p , "  $? " ) ;
		execute(StringContent( p ),status,1) ;
		StringDelete( p ) ;
		
		if ( status[0] != '0' )
			return 1 ;			
	}
	
	luks = is_luks( dev ) ;		
	
	if( luks == 0 )
		h = open_luks( dev,map,mode,source,pass ) ;
	else
		h = open_plain( dev,map,mode,source,pass,"cbc-essiv:sha256" ) ;
	
	switch ( h ){
		case 3 : goto out ;
		case 2 : h = 8 ; 
			 goto out ;
	}
	
	h = mount_volume(map,m_point,mode,id ) ;	
	
	if( h == 4 ){
		/*
		 * udisk seem to crash when mount/unmount happen too quickly, give it room to breath.
		 */
		sleep(2) ;
		open_plain( dev,map,mode,source,pass,"cbc-plain" ) ;		
		
		h = mount_volume(map,m_point,mode,id ) ;
	}
	out:
	return h ;	
}

