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

/*
 * Its not possible to add more keys to a volume with no empty slots or to a non luks volume
 *
 * This function checks if a volume is luks and if it has atleast one empty slot.
 */
static int zuluCryptCheckEmptySlots( const char * device )
{
	int status = 0 ;
	char * c = zuluCryptEmptySlots( device ) ;
	char * d  ;
	
	if( c == NULL ){
		/*
		 * we got here because the volume is either not luks based or the path is invalid
		 */
		return 1 ;
	}
	
	d = c - 1 ;
	
	while( *++d ){
		if( *d == '0' ){
			status = 2 ;
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
		case 0  : printf( "SUCCESS: key added successfully\n" );	                              	break ;
		case 1  : printf( "ERROR: presented key does not match any key in the volume\n" ) ;           	break ;
		case 2  : printf( "ERROR: could not open luks device\n" ) ;		                      	break ;
		case 4  : printf( "ERROR: device does not exist\n" ) ;	                                      	break ;
		case 5  : printf( "ERROR: wrong arguments\n" ) ;	                                      	break ;
		case 6  : printf( "ERROR: one or more required argument(s) for this operation is missing\n" );	break ;
		case 7  : printf( "ERROR: new passphrases do not match\n" ) ;	                              	break ;
		case 8  : printf( "ERROR: one or both keyfile(s) does not exist\n" ) ;	                      	break ;  
		case 9  : printf( "ERROR: couldnt get enought memory to hold the key file\n" ) ;	     	break ;
		case 10 : printf( "ERROR: all key slots are occupied, can not add any more keys\n" ) ;	      	break ;
		case 11 : printf( "ERROR: insufficient privilege to open a system device in read/write mode,\n\
only root user or members of group zulucrypt-write can do that\n" ) ;						break ;
		case 12 : printf( "ERROR: insufficient privilege to open key file for reading\n" );		break ;
		case 13 : printf( "ERROR: only root user can add keys to system devices\n" );			break ;
		case 14 : printf( "ERROR: can not get passphrase in silent mode\n" );				break ;
		case 15 : printf( "ERROR: insufficient memory to hold passphrase\n" );				break ;
		case 16 : printf( "ERROR: could not get a key from a socket\n" ) ;				break ;
		case 17 : printf( "ERROR: could not get elevated privilege,check binary permissions\n" ) ;	break ; 
		default : printf( "ERROR: unrecognized error with status number %d encountered\n",st );
	}
	
	return st ;
}

static int zuluExit_1( int st,const char * device,stringList_t stl )
{
	printf( "ERROR: device \"%s\" is not a luks device\n",device ) ;
	StringListClearDelete( &stl ) ;
	return st ;
}

static int zuluGetKeys( string_t * key1,string_t * key2,string_t * key3 )
{
	int st ;
	printf( "Enter an existing passphrase: " ) ;
	st = StringSilentlyGetFromTerminal_1( key1,KEY_MAX_SIZE ) ;
	if( st != 0 )
		return st ;
	printf( "\nEnter the new passphrase: " ) ;
	st = StringSilentlyGetFromTerminal_1( key2,KEY_MAX_SIZE ) ;
	if( st != 0 ){
		StringClearDelete( key1 ) ;
		return st ;
	}
	printf( "\nRe enter the new passphrase: " ) ;
	st = StringSilentlyGetFromTerminal_1( key3,KEY_MAX_SIZE ) ;
	if( st != 0 ){
		StringClearDelete( key1 ) ;
		StringClearDelete( key2 ) ;
		return st ;
	}
	
	printf( "\n" ) ;
	return 0 ;
}

/*
 * get_pass_from_file function is defined at get_pass_from_file.c * 
 */
int zuluCryptEXEAddKey( const struct_opts * opts,uid_t uid )
{
	const char * device      ;
	const char * keyType1    = opts->existing_key_source ;
	const char * existingKey = opts->existing_key ;
	const char * keyType2    = opts->new_key_source ;
	const char * newKey      = opts->new_key ;
	
	stringList_t stl = StringListInit() ;
	
	string_t * presentKey	= StringListAssign( stl ) ;
	string_t * newKey_1  	= StringListAssign( stl ) ; 
	string_t * newKey_2    	= StringListAssign( stl ) ; 
	string_t * ek          	= StringListAssign( stl ) ; 
	string_t * nk          	= StringListAssign( stl ) ; 
	string_t * dev_st	= StringListAssign( stl ) ;
	
	const char * key1 = NULL ;
	const char * key2 = NULL ;
	
	size_t len1 = 0 ;
	size_t len2 = 0 ;

	int status = 0 ;
	
	char * e = zuluCryptRealPath( opts->device ) ;
	
	if( e == NULL )
		return zuluExit( 4,stl ) ;
	
	*dev_st = StringInherit( &e ) ;
	
	device = StringContent( *dev_st ) ;
	/*
	 * This function is defined at "security.c"
	 * It makes sure the path exists and the user has atleast reading access to the path.
	 * 
	 * The importance of the function is explained where it is defined.
	 */
	switch( zuluCryptSecurityCanOpenPathForWriting( device,uid ) ){
		case 1 : return zuluExit( 11,stl ) ; break ;
		case 2 : return zuluExit( 4,stl )  ; break ;
	}
	
	if( zuluCryptSecurityGainElevatedPrivileges() ){
		if( zuluCryptVolumeIsNotLuks( device ) ){
			zuluCryptSecurityDropElevatedPrivileges() ;
			return zuluExit_1( 3,device,stl ) ;
		}
	}else{
		return zuluExit( 17,stl ) ;
	}
	
	switch( zuluCryptCheckEmptySlots( device ) ){
		case 0 : return zuluExit( 10,stl ) ;
		case 1 : return zuluExit( 2,stl )  ; 
	}
	
	if ( keyType1 == NULL && keyType2 == NULL ){
		switch( zuluGetKeys( presentKey,newKey_1,newKey_2 ) ){
			case 1 : return zuluExit( 14,stl ) ;
			case 2 : return zuluExit( 15,stl ) ;
		}
		
		if( !StringEqualString( *newKey_1,*newKey_2 ) )
			status = 7 ;
		else{	
			key1 = StringContent( *presentKey ) ;
			len1 = StringLength ( *presentKey ) ;
			key2 = StringContent( *newKey_1   ) ;
			len2 = StringLength ( *newKey_1   ) ;
			if( !zuluCryptSecurityGainElevatedPrivileges() )
				return zuluExit( 17,stl ) ;
			status = zuluCryptAddKey( device,key1,len1,key2,len2 );
		}
	}else{
		if( newKey == NULL || existingKey == NULL )
			return zuluExit( 6,stl ) ;
		if ( strcmp( keyType1, "-f" ) == 0 ){
			/*
			 * this function is defined at "security.c"
			 */
			switch( zuluCryptSecurityGetPassFromFile( existingKey,uid,ek ) ){
				case 1 : return zuluExit( 8,stl ) ; 
				case 4 : return zuluExit( 12,stl ) ;
				case 2 : return zuluExit( 9,stl );
				case 5 : return zuluExit( 16,stl ) ;
			}
			key1 = StringContent( *ek ) ;
			len1 = StringLength( *ek ) ;
		}		
		if ( strcmp( keyType2, "-f" ) == 0 ){
			/*
			 * this function is defined at "security.c.c"
			 */
			switch( zuluCryptSecurityGetPassFromFile( newKey,uid,nk ) ){
				case 1 : return zuluExit( 8,stl ) ; 
				case 4 : return zuluExit( 12,stl ) ;
				case 2 : return zuluExit( 9,stl );
				case 5 : return zuluExit( 16,stl ) ;
			}
			key2 = StringContent( *nk ) ;
			len2 = StringLength( *nk ) ;
		}		
		if ( strcmp( keyType1,"-f" ) == 0 && strcmp( keyType2,"-f" ) == 0 ){
			if( !zuluCryptSecurityGainElevatedPrivileges() )
				return zuluExit( 17,stl ) ;
			status = zuluCryptAddKey( device,key1,len1,key2,len2 ) ;
		}else if ( strcmp( keyType1,"-p" ) == 0 && strcmp( keyType2,"-p" ) == 0 ){
			key1 = existingKey ;
			len1 = strlen( existingKey ) ;
			key2 = newKey ;
			len2 = strlen( newKey ) ;
			if( !zuluCryptSecurityGainElevatedPrivileges() )
				return zuluExit( 17,stl ) ;
			status = zuluCryptAddKey( device,key1,len1,key2,len2 ) ;
		}else if ( strcmp( keyType1,"-p" ) == 0 && strcmp( keyType2,"-f" ) == 0 ){
			key1 = existingKey ;
			len1 = strlen( existingKey ) ;
			if( !zuluCryptSecurityGainElevatedPrivileges() )
				return zuluExit( 17,stl ) ;
			status = zuluCryptAddKey( device,key1,len1,key2,len2 ) ;
		}else if ( strcmp( keyType1,"-f" ) == 0 && strcmp( keyType2,"-p" ) == 0 ){
			key2 = newKey ;
			len2 = strlen( newKey ) ;
			if( !zuluCryptSecurityGainElevatedPrivileges() )
				return zuluExit( 17,stl ) ;
			status = zuluCryptAddKey( device,key1,len1,key2,len2 ) ;
		}else{
			status = 5 ;
		}
	}	
	
	/*
	 * this function is defined in check_invalid_key.c
	 */
	zuluCryptCheckInvalidKey( opts->device ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	return zuluExit( status,stl ) ;
}
