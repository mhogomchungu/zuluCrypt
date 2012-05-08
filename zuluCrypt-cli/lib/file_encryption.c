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

#define KEY_SIZE 100

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
 *  2. The encrypted file will have an addition size of 512 added to the beginning of thhe encrypted file.This is the header
 *     of the encrypted file.
 * 
 *     offset 0 to 100 contains the size of plain text in a C string format. If for example the plain text file has 69 bytes in it,
 *     the header at offset 0 will contain "69\0".Only characters from '0' to '9' are allowed and they must be null terminated.
 * 
 *     offset 100 to 200 will contain atmost 100 characters used in creating the encrypted file.
 *     If the key is less than 100 bytes in length,then the remaining length is NULL padded.
 * 
 *  If for example, a 100 bytes file is to be encrypted,the encrypted file will have a file size of 1024 bytes. First, the 100 bytes
 *  will be padded to 512 and then 512 bytes will be added to store the size of the plain text file,useful when decrypting the file. 
 * 
 */

static string_t crypt_mapper( const char * path,const char * key,uint64_t key_len )
{
	string_t p = StringIntToString( getpid() ) ;
	StringPrepend( p,"zuluCrypt-" ) ;
	
	if( open_plain( path,StringContent( p ),"rw",key,key_len ) != 0 ){
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
	 * Read the first 512 bytes bytes from the encrypted file.
	 * The content will be in plain text because we are reading from the mapper
	 */
	read( f_in,buffer,SIZE ) ;
	
	/*
	 * Go to offset 100 and compare its content against presented key.
	 * Them being the same means the mapper was opened with the right passphrase since there
	 * isrecognizable pattern.
	 * 
	 */
	
	if( key_len <= KEY_SIZE ){
		if( memcmp( buffer + KEY_SIZE,key,key_len ) != 0 )
			return return_status( 2,f_in,f_out,p ) ;
	}else{
		if( memcmp( buffer + KEY_SIZE,key,KEY_SIZE ) != 0 )
			return return_status( 2,f_in,f_out,p ) ;
	}
	
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

int encrypt_file( const char * source,const char * dest,const char * key,uint64_t key_len )
{
	string_t p ;
	string_t q ;
	
	char buffer[ SIZE ] ;
	char memkey[ KEY_SIZE ] ;
	
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
	 * set offset of the header to contain at most 100 characters from the key.Useful when checking if
	 * the volume is opened with right key.
	 */
	memset( memkey,0,KEY_SIZE ) ;
	
	if( key_len <= KEY_SIZE )
		memcpy( memkey,key,key_len ) ;
	else
		memcpy( memkey,key,KEY_SIZE ) ;	
	
	lseek( f_out,KEY_SIZE,SEEK_SET ) ;	
	
	write( f_out,memkey,KEY_SIZE ) ;
	
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

