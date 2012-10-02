 
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
#include "../lib/includes.h"
#include <libcryptsetup.h>

static int zuluExit( int st,struct crypt_device * cd )
{
	switch( st ){
		case 0 : printf( "SUCCESS: header saved successfully\n" ) 						; break ;
		case 1 : printf( "SUCCESS: header restored successfully\n" ) 						; break ;
		case 2 : printf( "ERROR: presented device is not a LUKS device\n" ) 					; break ;
		case 3 : printf( "ERROR: failed to read/write header,is the volume open?\n" ) 				; break ;
		case 4 : printf( "ERROR: failed to read/write header,is the volume open?\n" )				; break ;
		case 5 : printf( "INFO: operation terminater per user request\n" ) 					; break ;
		case 6 : printf( "ERROR: path to be used to create a back up file is occupied\n" ) 			; break ;
		case 7 : printf( "ERROR: failed to restore\n" ) 							; break ;
		case 8 : printf( "ERROR: insufficient privilege to open backup header file for reading\n" ) 		; break ;
		case 9 : printf( "ERROR: invalid path to back up header file\n" ) 					; break ;
		case 10: printf( "ERROR: insufficient privilege to create a backup header in a destination folder\n" ) 	; break ;
		case 11: printf( "ERROR: invalid path to device\n" ) 							; break ;
		case 12: printf( "ERROR: argument for path to a backup  header file is missing\n" ) 			; break ;
		case 13: printf( "ERROR: argument for path to a backup  header file is missing\n" ) 			; break ;
		case 14: printf( "ERROR: only root user can restore and back up luks headers on system devices\n" )	; break ;
		case 15: printf( "ERROR: insufficient privilege to open device for writing\n" ) 			; break ;	
		case 16: printf( "ERROR: could not resolve path to device\n" ) 						; break ;	
		case 17: printf( "ERROR: backup file does not appear to contain luks header\n" ) 			; break ;
		case 18: printf( "ERROR: insufficient privilege to open device for reading\n" ) 			; break ;
		case 19: printf( "ERROR: insufficient memory to hold your responce\n" )		 			; break ;					
	}
	
	if( cd != NULL )
		crypt_free( cd );
	
	return st ;
}

static int save_header( struct crypt_device * cd,const char * device,const char * path,uid_t uid )
{
	if( zuluCryptVolumeIsLuks( device ) != 0 )
		return zuluExit( 2,cd ) ;
	
	if( crypt_header_backup( cd,NULL,path ) == 0 ){
		chown( path,uid,uid ) ;
		chmod( path,S_IRUSR ) ;
		return zuluExit( 0,cd ) ;
	}else
		return zuluExit( 4,cd ) ;
}

static int back_up_is_luks( const char * path )
{
	struct crypt_device * cd;

	int st = -1 ;
	
	if( crypt_init( &cd,path ) != 0 )
		return 2 ;
	
	if( crypt_load( cd,NULL,NULL ) != 0 )
		st = 1 ;
	else
		st = 0 ;
	
	crypt_free( cd ) ;
	
	return st ;
}

static int restore_header( struct crypt_device * cd,const char * device,const char * path,int k )
{
	char * p ;
	char * q ;
	string_t confirm ;
	const char * warn = "\
Are you sure you want to replace a header on device \"%s\" with a backup copy at \"%s\"?\n\
Type \"YES\" and press Enter to continue: " ;

	if( back_up_is_luks( path ) != 0 )
		 return zuluExit( 17,cd ) ;
	
	if( k == -1 ){
		
		p = realpath( path,NULL ) ;
		q = realpath( device,NULL ) ;
		
		printf( warn,q,p ) ;
		
		free( p ) ;
		free( q ) ;
		
		confirm = StringGetFromTerminal_1( 3 ) ;
		if( confirm != StringVoid ){
			k = StringEqual( confirm,"YES" ) ;
			StringDelete( &confirm ) ;
			if( k == 0 )
				return zuluExit( 5,cd ) ;
		}else
			return zuluExit( 19,cd ) ;
	}
	
	if( crypt_header_restore( cd,NULL,path ) == 0 )
		return zuluExit( 1,cd ) ;
	else
		return zuluExit( 7,cd ) ;
}

int zuluCryptEXESaveAndRestoreLuksHeader( const struct_opts * opts,uid_t uid,int option  )
{
	struct crypt_device * cd;
	
	const char * device = opts->device ;
	
	/*
	 * using key_key here because i do not want to introduce a key field in the structure.
	 */
	const char * path = opts->key ;	
	
	char * dev = realpath( device,NULL ) ;
	
	int k ;
	
	if( dev == NULL )
		return zuluExit( 16,NULL ) ;	
	/*
	 * zuluCryptPartitionIsSystemPartition() is defined in partitions.c
	 */
	k = zuluCryptPartitionIsSystemPartition( dev ) ;
	free( dev ) ;
	
	if( k == 1 && uid != 0 )
		return zuluExit( 14,NULL ) ;
		
	if( path == NULL ){
		if( option == LUKS_HEADER_RESTORE )			
			return zuluExit( 12,NULL ) ;
		else
			return zuluExit( 13,NULL ) ;
	}
	
	if( option == LUKS_HEADER_RESTORE ){
		switch( zuluCryptSecurityCanOpenPathForReading( path,uid ) ){
			case 1 : return zuluExit( 8,NULL ) ;
			case 2 : return zuluExit( 9,NULL ) ;		
		}
		
		switch( zuluCryptSecurityCanOpenPathForWriting( device,uid ) ){
			case 1 : return zuluExit( 15,NULL ) ;
			case 2 : return zuluExit( 11,NULL ) ;
		}
	}else{
		switch( zuluCryptSecurityCanOpenPathForReading( device,uid ) ){
			case 1 : return zuluExit( 18,NULL ) ;
			case 2 : return zuluExit( 11,NULL ) ;		
		}
		if( zuluCryptPathIsValid( path ) )
			return zuluExit( 6,NULL ) ;
		
		if( zuluCryptSecurityCanOpenPathForWriting( path,uid ) == 1 )
			return zuluExit( 10,NULL ) ;
	}	
		
	if( crypt_init( &cd,device ) != 0 )
		return zuluExit( 3,NULL ) ;
	
	switch( option ){
		case LUKS_HEADER_RESTORE : return restore_header( cd,device,path,opts->dont_ask_confirmation ) ;
		case LUKS_HEADER_SAVE    : return save_header( cd,device,path,uid ) ;
	}
	
	/*
	 * shouldnt get here.
	 */
	return 200 ; 
}