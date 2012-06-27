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
#include "../bin/libzuluCrypt-exe.h"
#include "../bin/bash_special_chars.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SIZE 512


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
 *  create a container file, attach a mapper to the container file and copy contents ofthe file to be encrypted to the container file through
 *  the mapper .
 * 
 *  The container file has a simple format.
 *  1. The file will be a multiple of 512,if the plain text file has a file size that is not a multiple of 512,then
 *     encrypted file will be padded to the next file size of 512 mupltiple.
 * 
 *  2. The encrypted file will have an addition size of 512 bytes added to the beginning of the encrypted file.This is the header
 *     of the encrypted file.
 * 
 *     100 bytes of data are read from "/dev/urandom" and added at offset 100 and offset 200.These two 100 bytes data chunks will bo
 *     compared to check if the decryption key is the same as encrypting key.The offsett should math if thhe keys are the same.
 * 
 *     The first 100 bytes will contain the size of the load and the format should be a format atoll() can correctly undestand. ie
 *     the size must be a null terminated string of digits representing the size of the load.
 * 
 *      
 *  If for example, a 100 bytes file is to be encrypted,the encrypted file will have a file size of 1024 bytes. First, the 100 bytes
 *  will be padded to 512 and then 512 bytes will be added to store the size of the plain text file,useful when decrypting the file. 
 * 
 */

static string_t crypt_mapper( const char * path,const char * key,uint64_t key_len )
{
	const char * mapper ;
	
	char * q = strrchr( path,'/' ) ;
	
	string_t p = String( crypt_get_dir() ) ;
	
	size_t s = StringLength( p ) + 1 ;
	
	StringAppend( p,"/zuluCrypt-" ) ;
	
	if( q == NULL )
		mapper = StringAppend( p,path ) + s ;
	else
		mapper = StringAppend( p,q + 1 ) + s ;
	
	replace_bash_special_chars( p ) ;
	
	if( open_plain( path,mapper,"rw",key,key_len ) != 0 ){
		StringDelete( &p ) ;
		return NULL ;
	}

	return p ;
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
int decrypt_file( const char * source,const char * dest,const char * key,uint64_t key_len )
{	
	struct stat st ;
	
	char buffer[ SIZE ] ;
	
	uint64_t size ;
	uint64_t len ;
	uint64_t i ;
	
	int f_in ;
	int f_out = -1 ;
	
	int test ;
	
	/*
	 * attach a mapper to the file containing encrypted data
	 */
	string_t p = crypt_mapper( source,key,key_len ) ;
	
	if( p == NULL )
		return 1 ;
	
	f_in = open( StringContent( p ),O_RDONLY ) ;
	
	/*
	 * 100 bytes from offset 100 and 100 bytes from offset 200 are supposed to be te same if
	 * the right key is used.
	 */
	read( f_in,buffer,SIZE ) ;
	
	if( memcmp( buffer + 100,buffer + 200,100 ) != 0 )
		return return_status( 2,f_in,f_out,p ) ;
		
	/*
	 * get the size of encrypted data
	 */
	size = atoll( buffer ) ;
	
	/*
	 * Make sure the size of the encrypted file is with in expected range.
	 * It being out of range means either a wrong encrypted key was used or the encrypted file is corrupted.
	 * 
	 *  Because the padding can be up to 511 bytes and the header takes 512 bytes, the encrypted file will be
	 *  larger and the different will be >= 512 and < 1024.
	 */
	stat( source,&st ) ;
	
	test = st.st_size - size ;
	
	if( test < SIZE || test >= ( SIZE * 2 ) )
		return return_status( 2,f_in,f_out,p ) ;
	
	f_out = open( dest,O_WRONLY | O_CREAT ) ;
	
	if( size <= SIZE ){
		read( f_in,buffer,size ) ;
		write( f_out,buffer,size ) ;
	}else{
		len = size / SIZE ;
		
		for( i = 0 ; i < len ; i++ ){
		
			read( f_in,buffer,SIZE ) ;
			write( f_out,buffer,SIZE ) ;
		}
		
		len = size - ( i * SIZE ) ;
		
		read( f_in,buffer,len ) ;
		write( f_out,buffer,len ) ;
	}
	
	return return_status( 0,f_in,f_out,p ) ;	
}

/*
 * function responsible for creating an encrypted file
 */
int encrypt_file( const char * source,const char * dest,const char * key,uint64_t key_len )
{
	string_t p ;
	string_t q ;
	
	char buffer[ SIZE ] ;
	
	int f_in ;
	int f_out ;
	
	uint64_t size ;
	uint64_t size_1 ;
	
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
	 * write the size of plain text file to the encrypted file. This information is important when decrypting the data 
	 * because it tells us how much padding was applied if any.
	 * 
	 */
	write( f_out,StringContent( q ),StringLength( q ) ) ;
	write( f_out,'\0',StringLength( q ) + 1 ) ;
	
	/*
	 * write the same 100 byte random data in two locations to be used to check the decrypting key during decryption.	 * 
	 */
	f_in = open( "/dev/urandom",O_RDONLY ) ;
	read( f_in,buffer,100 ) ;
	close( f_in ) ;
	
	lseek( f_out,100,SEEK_SET ) ;	
	
	write( f_out,buffer,100 ) ;
	write( f_out,buffer,100 ) ;
	
	/*
	 * set the beginning of the payload,The cypher text will start at byte 512.
	 */
	lseek( f_out,SIZE,SEEK_SET ) ;	
	
	f_in = open( source,O_RDONLY ) ;
	
	/*
	 * Copy over plain text data to the "shell" file through the mapper, creating an encrypted file.
	 */	
	while( read( f_in,buffer,SIZE ) > 0 )
		write( f_out,buffer,SIZE ) ;
	
	close( f_in ) ;
	close( f_out ) ;
	
	close_mapper( mapper ) ;
	
	StringDelete( &q ) ;
	StringDelete( &p ) ;	
	
	return 0 ;	
}

