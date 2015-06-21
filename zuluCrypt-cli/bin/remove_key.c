/*
 *
 *  Copyright (c) 2011-2015
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

#include "includes.h"
#include <stdio.h>
#include <libintl.h>

static int _zuluCryptExECheckEmptySlots( const char * device )
{
	int status = 0 ;
	char * c  ;
	char * d  ;

	zuluCryptSecurityGainElevatedPrivileges() ;
	c = zuluCryptEmptySlots( device ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;

	if( c == NULL ){
		return 1 ;
	}

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
	zuluCryptSecurityUnlockMemory( stl ) ;
	/*
	 * this function is defined in ../string/StringList.c
	 */
	StringListClearDelete( &stl ) ;

	switch ( st ){
		case 0 : printf( gettext( "SUCCESS: Key removed successfully\n" ) ) ;					break ;
		case 2 : printf( gettext( "ERROR: There is no key in the volume that match the presented key\n" ) ) ;	break ;
		case 3 : printf( gettext( "ERROR: Could not open the volume\n" ) ) ;					break ;
		case 4 : printf( gettext( "ERROR: Insufficient privilege to open a system device,\
only root user or members of group zulucrypt can do that\n" ) ) ;						break ;
		case 5 : printf( gettext( "ERROR: Could not open the volume in write mode\n" ) ) ;			break ;
		case 6 : printf( gettext( "ERROR: Insufficient memory to hold your response\n" ) ) ;			break ;
		case 7 : printf( gettext( "INFO: Operation terminated per user request\n" ) ) ;				break ;
		case 8 : printf( gettext( "ERROR: Can not get passphrase in silent mode\n" ) ) ;			break ;
		case 9 : printf( gettext( "ERROR: Insufficient memory to hold passphrase\n" ) ) ;			break ;
		case 10: printf( gettext( "ERROR: One or more required argument(s) for this operation is missing\n" ) );break ;
		case 11: printf( gettext( "ERROR: Keyfile does not exist\n" ) ) ;					break ;
		case 12: printf( gettext( "ERROR: Could not get enough memory to open the key file\n" ) ) ;		break ;
		case 13: printf( gettext( "ERROR: Insufficient privilege to open key file for reading\n" ) ) ;		break ;
		case 14: printf( gettext( "ERROR: Could not get a key from a socket\n" ) ) ;				break ;
		default: printf( gettext( "ERROR: Unrecognized error with status number %d encountered\n" ),st );
	}
	return st ;
}

static int zuluExit_1( int st,const char * device,stringList_t stl )
{
	printf( gettext( "ERROR: Device \"%s\" is not a luks device\n" ),device ) ;
	StringListClearDelete( &stl ) ;
	return st ;
}

int zuluCryptEXERemoveKey( const struct_opts * opts,uid_t uid )
{
	int ask_confirmation     = opts->ask_confirmation ;
	const char * device      = opts->device ;
	const char * keyType     = opts->key_source ;
	const char * keytoremove = opts->key ;

	stringList_t stl = StringListInit() ;

	string_t * pass    =  StringListAssign( stl ) ;
	string_t * confirm =  StringListAssign( stl ) ;

	int status = 0 ;

	const char * key ;
	size_t       key_size ;

	/*
	 * zuluCryptPartitionIsSystemPartition() is defined in ./partitions.c
	 */
	if( zuluCryptPartitionIsSystemPartition( device,uid ) ){
		if( !zuluCryptUserIsAMemberOfAGroup( uid,"zulucrypt" ) ){
			return zuluExit( 4,stl ) ;
		}
	}

	/*
	 * zuluCryptCanOpenPathForWriting is defined in path_access.c
	 */
	status = zuluCryptCanOpenPathForWriting( device,uid ) ;
	/*
	 * 1-permissions denied
	 * 2-invalid path
	 * 3-shenanigans
	 * 4-common error
	 */
	switch( status ){
		case 0 : break ;
		case 1 : return zuluExit( 5,stl ) ;
		case 2 : return zuluExit( 5,stl ) ;
		case 3 : return zuluExit( 5,stl ) ;
		case 4 : return zuluExit( 5,stl ) ;
		default: return zuluExit( 5,stl ) ;
	}

	if( _zuluCryptExECheckEmptySlots( device ) == 3 ){
		if( ask_confirmation ){
			printf( gettext( "WARNING: There is only one key in the volume and all data in it will be lost if you continue.\n" ) ) ;
			printf( gettext( "Do you still want to continue? Type \"YES\" if you do: " ) ) ;
			*confirm = StringGetFromTerminal_1( 3 ) ;
			if( *confirm == StringVoid ){
				return zuluExit( 6,stl ) ;
			}
			if( !StringEqual( *confirm,gettext( "YES" ) ) ){
				return zuluExit( 7,stl ) ;
			}
		}
	}

	if( keyType == NULL ){

		printf( gettext( "Enter a key to be removed: " ) ) ;
		/*
		 * ZULUCRYPT_KEY_MAX_SIZE is set in ../constants.h
		 */
		switch( StringSilentlyGetFromTerminal_1( pass,ZULUCRYPT_KEY_MAX_SIZE ) ){
			case 1 : return zuluExit( 8,stl ) ;
			case 2 : return zuluExit( 9,stl ) ;
		}

		printf( "\n" ) ;
		key = StringContent( *pass ) ;
		key_size = StringLength( *pass ) ;
		zuluCryptSecurityLockMemory_1( *pass ) ;
	}else{
		if( keyType == NULL || keytoremove == NULL ){
			return zuluExit( 10,stl ) ;
		}
		if( StringsAreEqual( keyType,"-f" ) ){
			/*
			 * zuluCryptGetPassFromFile() is defined at path_access.c"
			 */
			switch( zuluCryptGetPassFromFile( keytoremove,uid,pass ) ){
				case 1 : return zuluExit( 11,stl )  ;
				case 2 : return zuluExit( 12,stl )  ;
				case 4 : return zuluExit( 13,stl ) ;
				case 5 : return zuluExit( 14,stl ) ;
			}
			key = StringContent( *pass ) ;
			key_size = StringLength( *pass ) ;
			zuluCryptSecurityLockMemory_1( *pass ) ;
		}else if( StringsAreEqual( keyType, "-p" ) ){
			key = keytoremove ;
			key_size = StringSize( keytoremove ) ;
		}else{
			return zuluExit( 10,stl ) ;
		}
	}

	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptRemoveKey() is defined in ../lib/remove_key.c
	 */
	status = zuluCryptRemoveKey( device,key,key_size ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;

	if( status == 1 ){
		status = zuluExit_1( status,device,stl ) ;
	}else{
		status = zuluExit( status,stl ) ;
	}

	/*
	 * zuluCryptCheckInvalidKey() is defined in check_invalid_key.c
	 */
	zuluCryptCheckInvalidKey( opts->device ) ;
	return status ;
}
