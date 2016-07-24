
/*
 *
 *  Copyright (c) 2014-2015
 *  name : Francis Banyikwa
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>
#include <dirent.h>
#include <sys/mman.h>
#include <locale.h>
#include <libintl.h>
#include <locale.h>

#include "lxqtwallet.h"
#include "version.h"

#define PASSWORD_SIZE         512
#define WALLET_NAME_SIZE      512

#define APPLICATION_NAME "zuluSafe"

#define _file( x ) x,strlen( x ) + 1

#define StringsAreEqual( x,y ) strcmp( x,y ) == 0
#define StringsAreNotEqual( x,y ) strcmp( x,y ) != 0

static void _write( int x,const void * y,size_t z )
{
	if( write( x,y,z ) ){;}
}
static void _read( int x,void * y,size_t z )
{
	if( read( x,y,z ) ){;}
}

static void _help( void )
{
	const char * help2 ;
	const char * help1 ;

	help1 = gettext( "\n\n\
This is a simple tool for secure storage of files in a wallet.Added files to the wallet\n\
will be stored in a secured file located at \"~/.config/lxqt/wallets/zuluSafe/YYY.lwt\" where YYY is\n\
wallet name\n\n" ) ;

	help2 = gettext( "\
To add a file to a wallet,run            : zuluSafe-cli --add    <path to file>\n\
To delete a file in a wallet,run         : zuluSafe-cli --delete <file name>\n\
To get a file from the wallet,run        : zuluSafe-cli --get    <file name>\n\
To add all files in a folder,run         : zuluSafe-cli --add-all <folder path>\n\
To get a list of files in the wallet,run : zuluSafe-cli --list\n\
To get all files from the wallet,run     : zuluSafe-cli --get-all\n\
To get a list of wallets,run             : zuluSafe-cli --wallets\n" ) ;

	printf( "\n%s%s\n%s",VERSION_STRING,help1,help2 ) ;
}

static void _clearKeyBoardBuffer( void )
{
	int c ;
	while( 1 ){
		c = getchar() ;
		if( c == '\n' || c == EOF ){
			break ;
		}
	}
}

static void _getInputFromUser( char * buffer,size_t size,size_t * len )
{
	char c ;
	char * e = buffer ;
	const char * f = e + size ;

	while( e != f ){
		c = getchar() ;
		if( c == '\n' || c == EOF ){
			break ;
		}else{
			*e++ = c ;
		}
	}

	*e = '\0' ;

	if( len != NULL ){
		*len = e - buffer ;
	}
}

static int _terminalEchoOff( struct termios * old,struct termios * new )
{
	if( tcgetattr ( 1,old ) != 0 ){
		return 1 ;
	}

	*new = *old;
	new->c_lflag &= ~ECHO;

	if( tcsetattr ( 1,TCSAFLUSH,new ) != 0 ){
		return 1 ;
	}else{
		return 0 ;
	}
}

static int _getPassWordFromUser( char * password,size_t size,size_t * len )
{
	struct termios old ;
	struct termios new ;

	if( _terminalEchoOff( &old,&new ) == 1 ){
		puts( gettext( "failed to read password" ) ) ;
		return 1 ;
	}else{
		_getInputFromUser( password,size,len ) ;
		tcsetattr( 1,TCSAFLUSH,&old );
		return 0 ;
	}
}

static int _open_wallet( lxqt_wallet_t * wallet,const char * password,size_t password_length,const char * wallet_name )
{
	lxqt_wallet_error r = lxqt_wallet_open( wallet,password,password_length,wallet_name,APPLICATION_NAME ) ;
	if( r != lxqt_wallet_no_error ){
		puts( gettext( "wrong password,failed to open wallet" ) ) ;
		return 1 ;
	}else{
		return 0 ;
	}
}

static int _openWallet( lxqt_wallet_t * wallet )
{
	size_t password_length = 0 ;
	lxqt_wallet_error r ;

	int c ;

	char password[ PASSWORD_SIZE + 1 ] ;
	char password_1[ PASSWORD_SIZE + 1 ] ;

	char wallet_name[ WALLET_NAME_SIZE + 1 ] ;

	printf( gettext( "enter wallet name: " ) ) ;
	_getInputFromUser( wallet_name,WALLET_NAME_SIZE,NULL ) ;

	if( lxqt_wallet_exists( wallet_name,APPLICATION_NAME ) != 0 ){
		printf( gettext( "wallet \"%s\" does not exist,do you want to create it?(y/n): " ),wallet_name ) ;
		c = getchar() ;
		_clearKeyBoardBuffer() ;
		if( c == 'y' ){
			printf( gettext( "enter wallet password: " ) ) ;
			_getPassWordFromUser( password,PASSWORD_SIZE,&password_length ) ;
			puts( "" ) ;
			printf( gettext( "re enter wallet password: " ) ) ;
			_getPassWordFromUser( password_1,PASSWORD_SIZE,NULL ) ;
			puts( "" ) ;
			if( StringsAreNotEqual( password,password_1 ) ){
				puts( gettext( "passwords did not match" ) ) ;
				return 1 ;
			}else{
				r = lxqt_wallet_create( password,password_length,wallet_name,APPLICATION_NAME ) ;
				if( r != lxqt_wallet_no_error ){
					puts( gettext( "failed to create wallet" ) ) ;
					return 1 ;
				}
			}
		}else{
			puts( gettext( "volume not created per user request" ) ) ;
			return 1 ;
		}
	}else{
		printf( gettext( "enter wallet password: " ) ) ;
		_getPassWordFromUser( password,PASSWORD_SIZE,&password_length ) ;
		puts( "" ) ;
	}

	return _open_wallet( wallet,password,password_length,wallet_name ) ;
}

static int _printListOfManagedFiles( lxqt_wallet_t wallet )
{
	lxqt_wallet_iterator_t iter ;

	memset( &iter,'\0',sizeof( iter ) ) ;

	while( lxqt_wallet_iter_read_value( wallet,&iter ) ){
		puts( iter.entry.key ) ;
	}

	return 0 ;
}

static const char * _fileName( const char * filePath )
{
	const char * e = strrchr( filePath,'/' ) ;
	if( e == NULL ){
		return filePath ;
	}else{
		return e + 1 ;
	}
}

static int _addFileToWallet_1( lxqt_wallet_t wallet,int fd,const char * filePath,const struct stat * st )
{
	lxqt_wallet_error r ;
	char * e ;
	const char * fileName = _fileName( filePath ) ;
	e = malloc( st->st_size ) ;
	if( e == NULL ){
		puts( gettext( "failed to allocate memory" ) ) ;
		return 1 ;
	}else{
		_read( fd,e,st->st_size ) ;
		r = lxqt_wallet_add_key( wallet,_file( fileName ),e,st->st_size ) ;
		free( e ) ;
		if( r != lxqt_wallet_no_error ){
			puts( gettext( "failed to add file to the wallet" ) ) ;
			return 1 ;
		}else{
			return 0 ;
		}
	}
}

static int _addFileToWallet( lxqt_wallet_t wallet,const char * filePath )
{
	lxqt_wallet_error r ;
	struct stat st ;
	int fd ;
	void * map ;
	int k = 1 ;
	const char * fileName = _fileName( filePath ) ;

	if( lxqt_wallet_wallet_has_key( wallet,_file( fileName ) ) ){
		printf( gettext( "wallet already has \"%s\" entry\n" ),fileName ) ;
		return 1 ;
	}else{
		fd = open( filePath,O_RDONLY ) ;
		if( fd == -1 ){
			puts( gettext( "failed to open file for reading" ) ) ;
			return 1 ;
		}else{
			fstat( fd,&st ) ;
			map = mmap( 0,st.st_size,PROT_READ,MAP_PRIVATE,fd,0 ) ;
			if( map == MAP_FAILED ){
				k = _addFileToWallet_1( wallet,fd,fileName,&st ) ;
			}else{
				r = lxqt_wallet_add_key( wallet,_file( fileName ),map,st.st_size ) ;
				munmap( map,st.st_size ) ;
				if( r != lxqt_wallet_no_error ){
					puts( gettext( "failed to add file to the wallet" ) ) ;
					k = 1 ;
				}else{
					k = 0 ;
				}
			}

			close( fd ) ;
			return k ;
		}
	}
}

static int _deleteFileFromWallet( lxqt_wallet_t wallet,const char * filePath )
{
	const char * fileName = _fileName( filePath ) ;
	lxqt_wallet_delete_key( wallet,_file( fileName ) ) ;
	return 0 ;
}

static int _getFileFromWallet( lxqt_wallet_t wallet,const char * filePath )
{
	int fd ;
	int r ;
	struct stat st ;
	lxqt_wallet_key_values_t k ;
	const char * fileName = _fileName( filePath ) ;

	r = lxqt_wallet_read_key_value( wallet,_file( fileName ),&k ) ;
	if( r != 1 ){
		puts( gettext( "file not found in the wallet" ) ) ;
		return 1 ;
	}else{
		if( stat( filePath,&st ) == 0 ){
			printf( gettext( "path ./\"%s\" already occupied\n" ),fileName ) ;
			return 1 ;
		}else{
			fd = open( filePath,O_WRONLY|O_CREAT,0644 ) ;
			if( fd == -1 ){
				puts( gettext( "failed to open file for writing" ) ) ;
				return 1 ;
			}else{
				_write( fd,k.key_value,k.key_value_size ) ;
				close( fd ) ;
				return 0 ;
			}
		}
	}
}

static int _getAllFilesFromWallet( lxqt_wallet_t wallet )
{
	lxqt_wallet_iterator_t iter ;

	memset( &iter,'\0',sizeof( iter ) ) ;

	while( lxqt_wallet_iter_read_value( wallet,&iter ) ){
		_getFileFromWallet( wallet,iter.entry.key ) ;
	}

	return 0 ;
}

static int _addAllFilesToTheWallet( lxqt_wallet_t wallet,const char * path )
{
	struct stat st ;
	DIR * dir = opendir( path ) ;
	struct dirent * entry ;
	char path_1[ PATH_MAX ] ;

	if( dir == NULL ){
		puts( gettext( "failed to open directory for reading" ) ) ;
		return 1 ;
	}else{
		while( ( entry = readdir( dir ) ) != NULL ){
			snprintf( path_1,PATH_MAX,"%s/%s",path,entry->d_name ) ;
			if( stat( path_1,&st ) == 0 && S_ISREG( st.st_mode ) ){
				_addFileToWallet( wallet,path_1 ) ;
			}
		}
		return 0 ;
	}
}

static int _printWalletList( void )
{
	int len = 0 ;
	int k   ;
	char * c ;
	char ** e = lxqt_wallet_wallet_list( APPLICATION_NAME,&len ) ;

	for( k = 0 ; k < len ; k++ ){
		c = *( e + k ) ;
		puts( c ) ;
		free( c ) ;
	}

	free( e ) ;
	return 0 ;
}

int main( int argc,char * argv[] )
{
	lxqt_wallet_t wallet = 0 ;

	const char * path ;
	const char * action ;
	int r = 1 ;

	if( argc == 1 ){
		_help() ;
		return 0 ;
	}

	action = argv[ 1 ] ;

	if( argc == 2 ){
		if( StringsAreEqual( action,"-h" ) || StringsAreEqual( action,"--help" ) || StringsAreEqual( action,"-help" )
			|| StringsAreEqual( action,"-v" ) || StringsAreEqual( action,"--version" ) ){
			_help() ;
			return 0 ;
		}else if( StringsAreEqual( action,"--wallets" ) ){
			return _printWalletList() ;
		}else if( StringsAreEqual( action,"--list" ) || StringsAreEqual( action,"--get-all" ) ){
			;
		}else{
			_help() ;
			return 1 ;
		}
	}

	if( _openWallet( &wallet ) ){
		return 1 ;
	}else{
		if( StringsAreEqual( action,"--list" ) ){
			r = _printListOfManagedFiles( wallet ) ;
		}else if( StringsAreEqual( action,"--get-all" ) ){
			r = _getAllFilesFromWallet( wallet ) ;
		}else{
			if( argc != 3 ){
				_help() ;
			}else{
				path = argv[ 2 ] ;
				if( StringsAreEqual( action,"--add" ) ){
					r = _addFileToWallet( wallet,path ) ;
				}else if( StringsAreEqual( action,"--delete" ) ){
					r = _deleteFileFromWallet( wallet,path ) ;
				}else if( StringsAreEqual( action,"--get" ) ){
					r = _getFileFromWallet( wallet,path ) ;
				}else if( StringsAreEqual( action,"--add-all" ) ){
					r = _addAllFilesToTheWallet( wallet,path ) ;
				}else{
					puts( "unknown command" ) ;
					r = 1 ;
				}
			}
		}

		lxqt_wallet_close( &wallet ) ;
		return r ;
	}
}
