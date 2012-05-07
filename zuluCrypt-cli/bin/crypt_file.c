/*
 * 
 *  Copyright (c) 2012
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

#include "includes.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SIZE 512

#define DECRYPT 1
#define ENCRYPT 0

/*
 *  routines for encrypting and decrypting stand alone files.
 *  
 *  Basic idea of the operation:
 * 
 *  basic concept:
 *  
 *  to create an encrypted file:
 * 
 *  create a "shell" file, attach a mapper to it and then copy the content of plain text file
 *  to the shell file through the mapper to create a cypher text file.
 * 
 *  to create a decrypted file:
 * 
 *  create a "shell" file, attach a mapper to the encrypted file and copy contents of encrypted file through
 *  the mapper to the "shell" file and the shell file will contain decrypted data.
 * 
 *  The encrypted file has a simple format.
 *  1. The file will be a multiple of 512,if the plain text file has a file size that is not a multiple of 512,then
 *     encrypted file will be padded to the next file size of 512 mupltiple.
 * 
 *  2. The encrypted file will have an addition size of 512 added to the beginning to contain the size of the encrypted data.
 * 
 *  If for example, a 100 bytes file is to be encrypted,the encrypted file will have a file size of 1024 bytes. First, the 100 bytes
 *  will be padded to 512 and then 512 bytes will be added to store the size of the plain text file,useful when decrypting the file. 
 * 
 */

static int msg( int st )
{
	switch( st ){
		case 0 : printf( "SUCCESS: encrypted file created successfully\n")				; break ;
		case 1 : printf( "SUCCESS: decrypted file created successfully\n" )  				; break ;
		case 2 : printf( "ERROR: could not open key file for reading\n" )  				; break ;
		case 3 : printf( "ERROR: missing key source\n")							; break ;
		case 4 : printf( "ERROR: could not open encryption mapper\n")					; break ;
		case 5 : printf( "ERROR: file or folder already exist at destination address\n")		; break ; 
		case 6 : printf( "ERROR: invalid path to source\n")						; break ;
		case 7 : printf( "ERROR: could not resolve path to destination file\n")				; break ;
		case 8 : printf( "ERROR: passphrases do not match\n")						; break ;
		case 9 : printf( "ERROR: required argument is missing\n")					; break ;
		case 10: printf( "ERROR: insufficient privilege to create destination file\n")			; break ;
		case 11: printf( "ERROR: inconsistency in the encrypted file detected,wrong passphrase?\n")	; break ;
	}	
	return st ;
}

static string_t crypt_mapper( const char * path,const char * key,uint64_t key_len )
{
	string_t p = StringIntToString( getpid() ) ;
	StringPrepend( p,"zuluCrypt-" ) ;
	
	if( open_plain( path,StringContent( p ),"rw",key,key_len,"cbc-essiv:sha256" ) != 0 ){
		StringDelete( &p ) ;
		return NULL ;
	}
	
	StringPrepend( p,"/" ) ;
	StringPrepend( p,crypt_get_dir() ) ;
	
	return p ;
}

/*
 * function responsible for creating an encrypted file
 */
static int encrypt_path( const char * source,const char * dest,const char * key,uint64_t key_len )
{
	string_t p ;
	string_t q ;
	
	char buffer[ SIZE ] ;
	
	int f_in ;
	int f_out ;
	
	uint64_t size ;
	uint64_t size_1 ;
	
	size_t len ;
	
	const char * mapper ;
	
	struct stat st ;
	
	stat( source,&st ) ;
	
	size = st.st_size ;
	
	/*
	 * make sure the encrypted file is a multiple of 512, important because data will be read/written in chunks of 512 bytes.
	 */
	while( size % SIZE != 0 )
		size++ ;
	
	/*
	 * add 512 bytes to encrypted file, the exta space will be used to store the content size of the data to be encrypted.
	 */
	size += SIZE ;
	
	memset( buffer,0,SIZE ) ;
	
	f_out = open( dest,O_WRONLY | O_CREAT ) ;
	
	size_1 = 0 ;
	
	/*
	 * create a file to be used to store encrypted data.
	 */
	while( 1 ){
		write( f_out,buffer,SIZE );
		
		size_1 += SIZE ;
		
		if( size_1 == size )
			break ;
	}
	
	close( f_out ) ;
	
	/*
	 * attach a mapper to the file that will contain encrypted data
	 */
	p = crypt_mapper( dest,key,key_len ) ;
	if( p == NULL ){
		remove( dest ) ;
		return 1 ;
	}
	
	mapper = StringContent( p ) ;
	
	f_out = open( mapper,O_WRONLY ) ;
	
	q = StringIntToString( st.st_size ) ;
	
	/*
	 * write the size of plain text file to the encrypted file. This information when decrypting the data because it tells us
	 * how much data is in the volume. We cant know this by looking at the file size because it can be padded and searching for 
	 * NULL character to look for end point will not work with contents with NULL characters in them.
	 */
	write( f_out,StringContent( q ),StringLength( q ) ) ;
	write( f_out,'\0',StringLength( q ) + 1 ) ;
	
	/*
	 * set the beginning of the payload,The cypher text will start at byte 512.
	 */
	lseek( f_out,SIZE,SEEK_SET ) ;	
	
	f_in = open( source,O_RDONLY ) ;
	
	/*
	 * Copy over plain text data to the "shell" file through the mapper, creating an encrypted file.
	 */	
	while( 1 ){
		len = read( f_in,buffer,SIZE ) ;
		if( len <= 0 )
			break ;
		
		write( f_out,buffer,len ) ;
	}
	
	close( f_in ) ;
	close( f_out ) ;
	
	close_mapper( mapper ) ;
	
	StringDelete( &q ) ;
	StringDelete( &p ) ;	
	
	return 0 ;	
}

static int return_status( int st,int f_in,int f_out,string_t p )
{
	if( f_out != -1 )
		close( f_out ) ;
	
	close( f_in ) ;
	
	close_mapper( StringContent( p ) ) ;
	
	StringDelete( &p ) ;
	
	return st ;
}

/*
 * function responsible for creating a decrypted file
 */
static int decrypt_path( const char * source,const char * dest,const char * key,uint64_t key_len )
{	
	struct stat st ;
	
	char buffer[ SIZE ] ;
	
	uint64_t size ;
	uint64_t len ;
	
	const char * mapper ;
	
	int f_in ;
	int f_out = -1 ;
	
	int test ;
	
	/*
	 * attach a mapper to the file containing encrypted data
	 */
	string_t p = crypt_mapper( source,key,key_len ) ;
	
	if( p == NULL )
		return 1 ;
	
	mapper = StringContent( p ) ;
	
	f_in = open( mapper,O_RDONLY ) ;
	
	/*
	 * Read the first 512 bytes bytes from the encrypted file.
	 * The content will be in plain text because we are reading from the mapper
	 */
	read( f_in,buffer,SIZE ) ;
	
	/*
	 * get the size of encrypted data
	 */
	size = atoll( buffer ) ;
	
	/*
	 * Do a bit of sanity check to check for passphrase correctness.
	 * The diffence btw encrypted file and plain text file must be btw 0 and 1023
	 * 
	 * A value out of range means a wrong passphrase was used and random data was read and atoll failed to 
	 * give expected value.
	 * 
	 * 1023 = 512 + 511
	 * The first 512 is for thhe offset to store plain text file size.
	 * 
	 * The 511 is the maximum bytes we can add to encrypted file to make it devisible by 512	 * 
	 */
	stat( source,&st ) ;
	
	test = st.st_size - size ;
	
	if( test < 0 || test > ( SIZE * 2 ) - 1 )
		return return_status( 2,f_in,f_out,p ) ;
	
	len = 0 ;
	
	f_out = open( dest,O_WRONLY | O_CREAT ) ;
	
	/*
	 * read the content of the encrypted file through the mapper and write them to a file in plain text
	 */
	while( 1 ){
		
		read( f_in,buffer,SIZE ) ;
		
		if( size - len <= SIZE ){
			write( f_out,buffer,size - len ) ;
			break ;
		}
		
		write( f_out,buffer,SIZE ) ;
		
		len += SIZE ;
	}
	
	return return_status( 0,f_in,f_out,p ) ;	
}

static int crypt_opt( const struct_opts * opts,const char * mapper,uid_t uid,int opt )
{
	string_t q ;
	string_t p ;

	int st ;
	
	const char * source	= opts->device ;
	const char * dest  	= opts->mode ;
	const char * passphrase = opts->key ;
	const char * type 	= opts->key_source ;
	int i 			= opts->interactive_passphrase ;
	
	//remove( dest ) ;
	
	if( dest == NULL )
		return msg( 9 ) ;
	
	if( source == NULL )
		return msg( 9 ) ;
	
	if( is_path_valid( dest ) == 0 )
		return msg( 5 ) ;
	
	if( can_open_path_for_writing( dest,uid ) == 1 )
		return msg( 10 ) ;		
	
	if( is_path_valid( source ) != 0 )
		return msg( 6 ) ;
	
	if( i == 1 ){
		printf( "Enter passphrase: " ) ;			
		p = get_passphrase();			
		printf( "\nRe enter passphrase: " ) ;			
		q = get_passphrase();				
		printf( "\n" ) ;			
		if( StringCompare( p,q ) != 0 ){
			StringDelete( &p ) ;
			StringDelete( &q ) ;
			return msg( 8 ) ; 
		}else{
			StringDelete( &q ) ;
		}
	}else{
		if( type == NULL )
			return msg( 9 ) ;
		if( strcmp( type,"-p" ) == 0 ){
			p = String( passphrase ) ;
		}else if( strcmp( type,"-f" ) == 0 ){		
			p = StringGetFromFile( passphrase ) ;		
			if( p == NULL )
				return msg( 2 ) ;		
		}else{
			return msg( 3 ) ;
		}
	}		
	
	if( opt == ENCRYPT )
		st = encrypt_path( source,dest,StringContent( p ),StringLength( p ) ) ;
	else
		st = decrypt_path( source,dest,StringContent( p ),StringLength( p ) ) ;	
	
	StringDelete( &p ) ;
	
	switch( st ){
		case 1 : return msg( 4 ) ;
		case 2 : return msg( 11 ) ;
	}
	
	chmod( dest,S_IRUSR | S_IWUSR ) ;
	chown( dest,uid,uid ) ;
	
	if( opt == 1 )
		return msg( 1 ) ;
	else
		return msg( 0 ) ;
}

int decrypt_file( const struct_opts * opts,const char *mapper,uid_t uid ) 
{
	return crypt_opt( opts,mapper,uid,DECRYPT ) ;
}

int encrypt_file( const struct_opts * opts,const char *mapper,uid_t uid )
{
	return crypt_opt( opts,mapper,uid,ENCRYPT ) ;		
}
