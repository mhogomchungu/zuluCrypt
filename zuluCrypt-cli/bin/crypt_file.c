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

static int msg( int st )
{
	switch( st ){
		case 0 : printf( "SUCCESS: encrypted file created successfully\n")		; break ;
		case 1 : printf( "SUCCESS: decrepted file created successfully\n" )  		; break ;
		case 2 : printf( "ERROR: could not open key file for reading\n" )  		; break ;
		case 3 : printf( "ERROR: missing key source\n")					; break ;
		case 4 : printf( "ERROR: could not open encryption mapper\n")			; break ;
		case 5 : printf( "ERROR: file or folder already exist at destination address\n"); break ; 
		case 6 : printf( "ERROR: invalid path to source\n")				; break ;
		case 7 : printf( "ERROR: could not resolve path to destination file\n")		; break ;
				
	}
	
	return st ;
}

static uint64_t create_dest_file( const char * source, const char * dest,uid_t uid )
{
	uint64_t data_written = 0 ;
	uint64_t size ;
	size_t len ;
	int fd ;
	char data[ SIZE ] ;
	struct stat st ;
	
	memset( data,0,SIZE ) ;
	
	stat( source,&st ) ;
	
	size = st.st_size ;

	fd = open( dest,O_CREAT | O_WRONLY ) ;

	while( 1 ){
		if( size - data_written <= SIZE ){
			write( fd,data,size - data_written ) ;
			break ;
		}
		
		data_written += SIZE ;
		
		len = SIZE ;
		do{
			len = len - write( fd,data,len ) ;		
		}while( len != 0 ) ;	
	}
	
	close( fd ) ;
	chmod( dest,S_IRWXU ) ;
	chown( dest,uid,uid ) ;
	return st.st_size ;
}

int open_mapper( const char * dest,uid_t uid,const char * mapper,const char * passphrase,const char * type,string_t * xt )
{
	int st ;
	const char * map ;
	const char * key ;
	uint64_t key_len ;
	
	char * rpath ;
	
	string_t q ;	
	string_t p ;
	
	rpath = realpath( dest,NULL ) ;
	
	if( rpath == NULL )
		return 7 ;	
	
	if( strcmp( type,"-p" ) == 0 ){
		p = String( passphrase ) ;
	}else if( strcmp( type,"-f" ) == 0 ){		
		p = StringGetFromFile( passphrase ) ;		
		if( p == NULL )
			return 2 ;		
	}else{
		return 3 ;
	}

	q = create_mapper_name( rpath,mapper,uid,OPEN ) ;
	
	key = StringContent( p ) ;
	key_len = StringLength( p ) ;
	map = StringContent( q ) ;
	
	if( open_plain( rpath,map,"rw",key,key_len,"cbc-essiv:sha256" ) == 0 )
		st = 0 ;
	else
		st = 4 ;
	
	*xt = q ;
	StringDelete( &p ) ;
	return st ;
}

static void close_mapper_crypt( const char * path,const char * mapper,uid_t uid )
{	
	char * rpath = realpath( path,NULL ) ;
	
	if( rpath == NULL )
		return ;
	
	string_t z = create_mapper_name( rpath,mapper,uid,CLOSE ) ;
	
	if( z != NULL ){
		close_mapper( StringContent( z ) ) ;	
		StringDelete( &z ) ;
	}
	
	free( rpath ) ;
}

static void crypt( const char * cin,const char * cout,uint64_t data_size )
{
	char buffer[ SIZE ] ;	
	
	int in = open( cin,O_RDONLY ) ;
	int out = open( cout,O_WRONLY ) ;

	uint64_t data_written = 0 ;
	
	int size ;
	while( 1 ) {
		
		size = read( in,buffer,SIZE );
		if( size <= 0 ){
			printf("done\n");
			break;
			
		}
		
		write( out,buffer,size );
		
		lseek( in,size,SEEK_SET ) ;
		
	}	
	
	close( in ) ;
	close( out ) ;
	
}

int encrypt_file( const char * source,const char * mapper,const char * dest,const char * passphrase,const char * type,uid_t uid )
{
	string_t q ;
	
	uint64_t file_size ;	
	
	int st ;

	if( is_path_valid( dest ) == 0 )
		return msg( 5 ) ;
	
	if( is_path_valid( source ) != 0 )
		return msg( 6 ) ;

	file_size = create_dest_file( source,dest,uid ) ;

	st = open_mapper( dest,uid,mapper,passphrase,type,&q ) ;
	
	if( st != 0 ){
		remove( dest ) ;
		return msg( st ) ;
	}
	
	StringPrepend( q,"/" ) ;
	StringPrepend( q,crypt_get_dir() ) ;
	
	crypt( source,StringContent( q ),file_size ) ;
	
	close_mapper_crypt( dest,mapper,uid ) ;
	
	StringDelete( &q ) ;
	
	return msg( 0 ) ;
	
}

int decrypt_file( const char * source,const char * mapper,const char * dest,const char * passphrase,const char * type,uid_t uid )
{
	string_t q ;
	
	uint64_t file_size ;	
	
	int st ;
	
	if( is_path_valid( dest ) == 0 )
		return msg( 5 ) ;
	
	if( is_path_valid( source ) != 0 )
		return msg( 6 ) ;
	
	file_size = create_dest_file( source,dest,uid ) ;
	
	st = open_mapper( source,uid,mapper,passphrase,type,&q ) ;
	
	if( st != 0 ){
		remove( dest ) ;
		return msg( st ) ;
	}
	
	StringPrepend( q,"/" ) ;
	StringPrepend( q,crypt_get_dir() ) ;
	
	crypt( StringContent( q ),dest,file_size ) ;
	
	close_mapper_crypt( source,mapper,uid ) ;
	
	StringDelete( &q ) ;
	
	return msg( 1 ) ;
	
}

