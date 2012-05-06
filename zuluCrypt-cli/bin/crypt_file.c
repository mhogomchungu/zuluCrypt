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

#define SIZE 512

#define DECRYPT 1
#define ENCRYPT 0

/*
 *  routines for encrypting and decrypting stand alone files.
 *  
 *  Basic idea of the operation:
 * 
 *  steps taken when creating an encrypted file.
 *  1. Create a destination file with the size a multple of 512
 *  2. Create a plain mapper and attach it to destination file.
 *  3. Copy contents of source file to destination file through the mapper.
 *  4. Resize the destination file to the size of the source.
 * 
 *  For reasons i am not sure of,things work as expected when the file attached to the mapper
 *  has a file size that is a multiple of 512
 * 
 */

static int msg( int st )
{
	switch( st ){
		case 0 : printf( "SUCCESS: encrypted file created successfully\n")		; break ;
		case 1 : printf( "SUCCESS: decrypted file created successfully\n" )  		; break ;
		case 2 : printf( "ERROR: could not open key file for reading\n" )  		; break ;
		case 3 : printf( "ERROR: missing key source\n")					; break ;
		case 4 : printf( "ERROR: could not open encryption mapper\n")			; break ;
		case 5 : printf( "ERROR: file or folder already exist at destination address\n"); break ; 
		case 6 : printf( "ERROR: invalid path to source\n")				; break ;
		case 7 : printf( "ERROR: could not resolve path to destination file\n")		; break ;
		case 8 : printf( "ERROR: passphrases do not match\n")				; break ;
		case 9 : printf( "ERROR: required argument is missing\n")			; break ;
		case 10: printf( "ERROR: insufficient privilege to create destination file\n")	; break ;				
	}	
	return st ;
}

/*
 * write data from one file to another through the mapper
 */
static void crypt( const char * cin,const char * cout )
{
	char buffer[ SIZE ] ;	
	
	int in = open( cin,O_RDONLY ) ;
	int out = open( cout,O_WRONLY ) ;

	size_t size ;
	while( ( size = read( in,buffer,SIZE ) ) > 0 )
		write( out,buffer,size );
	
	close( in ) ;
	close( out ) ;	
}

static int pad_path( const char * source,const char * dest )
{
	char buffer[ SIZE ] ;
	int fd ;
	
	uint64_t size ;
	uint64_t size_1 ;
	
	struct stat st ;
	
	stat( source,&st ) ;
	
	size = st.st_size ;
	
	/*
	 *  we want the file attached to the mapper to have a file size that is a multiple of 512.
	 *  why? not sure but i suspect its to allow reading/writing in blocks of 512 bytes,this is the default
	 *  block size used in cryptsetup if i remember the docs correctly
	 * 
	 */
	while( size % SIZE != 0 )
		size++ ;

	memset( buffer,0,SIZE ) ;
	
	fd = open( dest,O_WRONLY | O_CREAT ) ;
	
	size_1 = 0 ;
	
	while( 1 ){
		write( fd,buffer,SIZE );
		
		size_1 += SIZE ;
		
		if( size_1 == size )
			break ;
	}
	
	close( fd ) ;
	
	return 0 ;	
}

static int shrink_file( const char * source,const char * dest )
{
	struct stat st ;
	stat( source,&st ) ;
	
	return truncate( dest,st.st_size ) ;	
}

static int crypt_opt( const struct_opts * opts,const char * mapper,uid_t uid,int opt )
{
	string_t q ;
	string_t p ;
	
	char * rpath ;
	const char * x ;
	
	const char * source	= opts->device ;
	const char * dest  	= opts->mode ;
	const char * passphrase = opts->key ;
	const char * type 	= opts->key_source ;
	int i 			= opts->interactive_passphrase ;
	
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
	
	pad_path( source,dest ) ;
	
	rpath = realpath( dest,NULL ) ;
	
	if( rpath == NULL ){
		remove( dest ) ;
		return msg( 7 ) ;	
	}
	
	q = create_mapper_name( rpath,mapper,uid,OPEN ) ;	
	
	if( open_plain( dest,StringContent( q ),"rw",StringContent( p ),StringLength( p ),"cbc-essiv:sha256" ) != 0 ){
		remove( dest ) ;
		return msg( 4 ) ;	
	}

	StringPrepend( q,"/" ) ;
	x = StringPrepend( q,crypt_get_dir() ) ;	
	
	if( opt == DECRYPT )
		crypt( x,dest ) ;
	else
		crypt( source,x ) ;
	
	close_mapper( x ) ;
	
	shrink_file( source,dest ) ;	
	
	StringDelete( &q ) ;
	StringDelete( &p ) ;	
	
	free( rpath ) ;	
	
	chown( dest,uid,uid ) ;
	chmod( dest,S_IRUSR | S_IWUSR ) ;
	
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
