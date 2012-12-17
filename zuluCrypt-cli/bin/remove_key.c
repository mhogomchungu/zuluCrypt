/*
 * 
 *  Copyright (c) 2011
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

static int zuluCryptExECheckEmptySlots( const char * device )
{
	int status = 0 ;
	char * c = zuluCryptEmptySlots( device ) ;
	char * d  ;
	
	if( c == NULL )
		return 1 ;
	d = c - 1 ;
	while( *++d ){
		if( *d == '3' ){
			status = 3 ;
			break ;
		}
	}	
	free( c ) ;
	return status ;
}

static int zuluExit( int st,stringList_t stl )
{
	/*
	 * this function is defined in ../string/StringList.c
	 */
	StringListClearDelete( &stl ) ;
	
	switch ( st ){
		case 0 : printf( "SUCCESS: key removed successfully\n" );										break ;
		case 2 : printf( "ERROR: there is no key in the volume that match the presented key\n" ) ;						break ;
		case 3 : printf( "ERROR: could not open device\n" ) ;											break ;  
		case 5 : printf( "ERROR: keyfile does not exist\n" ) ;											break ;
		case 6 : printf( "ERROR: one or more required argument(s) for this operation is missing\n" ) ;						break ;
		case 7 : printf( "ERROR: could not get enough memory to open the key file\n" ) ;							break ;
		case 10: printf( "ERROR: device does not exist\n" );											break ;
		case 11: printf( "INFO: operation terminated per user request\n" );									break ;
		case 12: printf( "ERROR: insufficient privilege to open a system device in read/write mode,\
only root user or members of group zulucrypt-write can do that\n" ) ;											break ;
		case 13: printf( "ERROR: insufficient privilege to open key file for reading\n" );							break ;
		case 14: printf( "ERROR: only root user can remove keys from system devices\n" );							break ;
		case 15: printf( "ERROR: can not get passphrase in silent mode\n" );									break ;
		case 16: printf( "ERROR: insufficient memory to hold passphrase\n" );									break ;
		case 17: printf( "ERROR: insufficient memory to hold your response\n" );								break ;
		case 18: printf( "ERROR: could not get a key from a socket\n" ) ;									break ;
		case 19 : printf( "ERROR: could not get elevated privilege,check binary permissions\n" ) ;						break ;	 
		default: printf( "ERROR: unrecognized error with status number %d encountered\n",st );
	}		
	return st ;
}

static int zuluExit_1( int st,const char * device,stringList_t stl )
{
	printf( "ERROR: device \"%s\" is not a luks device\n",device ) ;
	StringListClearDelete( &stl ) ;
	return st ;
}

int zuluCryptEXERemoveKey( const struct_opts * opts,uid_t uid ) 
{
	int k                    = opts->dont_ask_confirmation ;
	const char * device      = opts->device ;
	const char * keyType     = opts->key_source ;
	const char * keytoremove = opts->key ;
		
	stringList_t stl = StringListInit() ;
	
	string_t * pass    =  StringListAssign( stl ) ;
	string_t * confirm =  StringListAssign( stl ) ;
	
	int status = 0 ;
		
	/*
	* check_partition is defined in partitions.c
	
	if( zuluCryptPartitionIsSystemPartition( device ) && uid != 0 )
		return zuluExit( 14,stl ) ;
	*/
	/*
	 * This function is defined at "is_path_valid.c"
	 * It makes sure the path exists and the user has atleast reading access to the path.
	 * 
	 * The importance of the function is explained where it is defined.
	 */
	switch( zuluCryptSecurityCanOpenPathForWriting( device,uid ) ){
		case 2 : return zuluExit( 10,stl ); break ;
		case 1 : return zuluExit( 12,stl ); break ;
	}
	
	if( !zuluCryptSecurityGainElevatedPrivileges() )
		return zuluExit( 19,stl ) ;
	if( zuluCryptExECheckEmptySlots( device ) == 3 ){
		if( k != 1 ){
			printf( "WARNING: there is only one key in the volume and all data in it will be lost if you continue.\n" );
			printf( "Do you still want to continue? Type \"YES\" if you do: " );
			*confirm = StringGetFromTerminal_1( 3 ) ;
			if( *confirm == StringVoid ){
				zuluCryptSecurityDropElevatedPrivileges() ;
				return zuluExit( 17,stl ) ;
			}
			if( !StringEqual( *confirm,"YES" ) ){
				zuluCryptSecurityDropElevatedPrivileges() ;
				return zuluExit( 11,stl ) ;
			}
		}
	}
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	if ( keyType == NULL ){
	
		printf( "Enter a key to be removed: " ) ;
		switch( StringSilentlyGetFromTerminal_1( pass,KEY_MAX_SIZE ) ){
			case 1 : return zuluExit( 15,stl ) ;
			case 2 : return zuluExit( 16,stl ) ;
		}
		
		printf( "\n" ) ;
		
		status = zuluCryptRemoveKey( device,StringContent( *pass ),StringLength( *pass ) ) ;
	}else{
		if( keyType == NULL || keytoremove == NULL )
			return zuluExit( 6,stl ) ;
		
		if( strcmp( keyType,"-f" ) == 0 ){
			/*
			 * function is defined at security.c"
			 */
			switch( zuluCryptSecurityGetPassFromFile( keytoremove,uid,pass ) ){
				case 1 : return zuluExit( 5,stl )  ; 
				case 2 : return zuluExit( 7,stl )  ;
				case 4 : return zuluExit( 13,stl ) ;
				case 5 : return zuluExit( 18,stl ) ;
			}
			if( !zuluCryptSecurityGainElevatedPrivileges() )
				return zuluExit( 19,stl ) ;
			status = zuluCryptRemoveKey( device,StringContent( *pass ),StringLength( *pass ) ) ;
		}else if( strcmp( keyType, "-p" ) == 0 ) {
			if( !zuluCryptSecurityGainElevatedPrivileges() )
				return zuluExit( 19,stl ) ;
			status = zuluCryptRemoveKey( device,keytoremove,strlen( keytoremove ) ) ;
		}
	}
	
	if( status == 1 )
		status = zuluExit_1( status,device,stl ) ;
	else
		status = zuluExit( status,stl ) ; 
	
	zuluCryptCheckInvalidKey( opts->device ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	return status ;
}

 