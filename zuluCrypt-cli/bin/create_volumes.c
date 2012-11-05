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
#include "../lib/includes.h"

static int zuluExit( int st,stringList_t stl )
{
	/*
	 * this function is defined in ../string/StringList.c
	 */
	StringListClearDelete( &stl ) ;
	
	switch ( st ){
		case 0 : printf( "SUCCESS: volume created successfully\n" ) ;					break  ;
		case 1 : printf( "ERROR: invalid path to a file or device\n" ) ;				break  ;
		case 2 : printf( "ERROR: wrong option type\n" );						break  ;
		case 3 : printf( "ERROR: could not create an encrypted volume in a file or device\n" );		break  ;
		case 4 : printf( "ERROR: one or more required argument(s) for this operation is missing\n" );	break  ;
		case 5 : printf( "ERROR: wrong choice, exiting\n" );						break  ;
		case 6 : printf( "ERROR: couldnt get enought memory to hold the key file\n" ) ;			break  ;
		case 7 : printf( "ERROR: passphrases do not match\n" ) ;					break  ;
		case 8 : printf( "ERROR: invalid path to key file\n" ) ;					break  ;
		case 9 : printf( "ERROR: container file must be bigger than 3MB\n" ) ;				break  ;
		case 10: printf( "ERROR: insufficient privilege to create a volume on a system partition.\n" );
			 printf( "A system partition is a partition with an active entry in \"/etc/fstab\"," ) ;	
			 printf( "\"/etc/crypttab and \"/etc/zuluCrypttab\".\"\nRerun the tool from root's accout to proceed.\n" ) ;
														break  ;
		case 11: printf( "ERROR: %s not found \n",ZULUCRYPTmkfs ) ;					break  ;
		case 12: printf( "INFO: operation terminated per user request\n" ) ;				break  ;
		case 13: printf( "ERROR: insufficient privilege to write to device\n" ) ;			break  ;
		case 14: printf( "ERROR: insufficient privilege to create a volume in this device\n" ) ;	break  ;
		case 15: printf( "ERROR: insufficient privilege to open the file in write mode\n" ) ;		break  ;
		case 16: printf( "ERROR: there seem to be an opened mapper associated with the device\n" ) ;	break  ;
		case 17: printf( "ERROR: unable to resolve full path to device\n" ) ;				break  ;
		case 18: printf( "ERROR: can not create a volume on a mounted device\n" ) ;			break  ;
		case 19: printf( "ERROR: can not get passphrase in silent mode\n" );				break  ;
		case 20: printf( "ERROR: insufficient memory to hold passphrase\n" );				break  ;
		case 21: printf( "ERROR: insufficient memory to hold your response\n" );			break  ;
		case 22: printf( "ERROR: could not get a key from a socket\n" ) ;				break  ;
		default: printf( "ERROR: unrecognized error with status number %d encountered\n",st );
	}
	return st ;
}

static int zuluExit_1( const char * type,stringList_t stl )
{
	StringListClearDelete( &stl ) ;
	
	printf( "SUCCESS: volume created successfully\n" ) ;
	
	if( strcmp( type,"luks" ) == 0 ){
		printf( "\ncreating a backup of the volume luks header is strongly adviced.\n" ) ;
		printf( "Please read documentation on why this is important\n\n" ) ;
	}
	return 0 ;
}

int zuluCryptEXECreateVolume( const struct_opts * opts,const char * mapping_name,uid_t uid )
{
	int i                = opts->interactive_passphrase ;
	int conf             = opts->dont_ask_confirmation ;
	const char * device  = opts->device ;
	const char * fs      = opts->fs ;
	const char * type    = opts->type ;
	const char * keyType = opts->key_source ;
	const char * pass    = opts->key ;
	const char * rng     = opts->rng ;
		
	stringList_t stl = StringListInit() ;
	
	string_t * pass_1  = StringListAssign( stl ) ;
	string_t * pass_2  = StringListAssign( stl ) ;
	string_t * content = StringListAssign( stl ) ;
	string_t * confirm = StringListAssign( stl ) ;
	string_t * mapper  = StringListAssign( stl ) ;
		
	int st  ;
	struct stat xt ;
	
	char * dev ; 
	
	int j ;
	int k ;
	
	/*
	 * This function is defined at "security.c"
	 * It makes sure the path exists and the user has atleast reading access to the path.
	 * 
	 * The importance of the function is explained where it is defined.
	 */
	switch( zuluCryptSecurityCanOpenPathForWriting( device,uid ) ){
		case 2 : return zuluExit( 1,stl ) ; break ;
		case 1 : return zuluExit( 13,stl ); break ;
	}
	
	dev = realpath( device,NULL ) ;
	
	if( dev == NULL )
		return zuluExit( 17,stl ) ;
	
	*mapper = zuluCryptCreateMapperName( dev,mapping_name,uid,CLOSE ) ;
	
	j = zuluCryptCheckOpenedMapper( StringContent( *mapper ) ) ;
	
	/*
	 * defined in ../lib/print_mounted_volumes.c
	 */
	k = zuluCryptPartitionIsMounted( dev ) ;
	
	free( dev ) ;	
	
	if( j == 1 )
		return zuluExit( 16,stl ) ;
	
	if( k == 1 )
		return zuluExit( 18,stl ) ;
			
	if( strncmp( device,"/dev/",5 ) != 0 ){
		stat( device,&xt ) ;
		if( xt.st_size < 3145728 )
			return zuluExit( 9,stl ) ;
	}
	
	/*
	 * Only root user can create volumes in system partitions.
	 * System partitions are defined as partitions with active entried in "/etc/fstab","/etc/crypttab" and "/etc/zuluCrypttab"
	 * 
	 * Active entries are entries not commented out.
	 * 
	 * zuluCryptPartitionIsSystemPartition() is defined in partitions.c
	 */	
	if( zuluCryptPartitionIsSystemPartition( device ) )
		if( uid != 0 )
			return zuluExit( 10,stl ) ;
	
	/*
	 * root's privileges required to create volumes in devices located in "/dev/" other than /dev/sdX and /dev/hdX
	 */
	if( strncmp( device,"/dev/",5 ) == 0 )
		if( strncmp( device,"/dev/hd",7 ) != 0 )
			if( strncmp( device,"/dev/sd",7 ) != 0 )
				if( uid != 0 )
					return zuluExit( 14,stl ) ;

	/*
	 * ZULUCRYPTmkfs is defined at "../constants.h"
	 * File systems are created not through file systems APIs but through mkfs.xxx executables started using exec call.
	 */
	if( zuluCryptPathIsNotValid( ZULUCRYPTmkfs ) )
		return zuluExit( 11,stl ) ;
	
	if( conf == -1 ){			
		printf( "\nThis operation will destroy all data in a device at: \"%s\"\n",device ) ;
		printf("Are you sure you want to proceed?\n" ) ;
		printf( "Type \"YES\" and press enter if you want to process: " ) ;
		
		*confirm = StringGetFromTerminal_1( 3 ) ;
		if( *confirm == StringVoid )
			return zuluExit( 21,stl ) ;
		else{
			if( !StringEqual( *confirm,"YES" ) )
				return zuluExit( 12,stl ) ;
		}
	}
	
	if( i == 1 || keyType == NULL ){
		/*
		 * Make sure the user has provided all required options
		 */
		if( fs == NULL || type == NULL || rng == NULL )
			return zuluExit( 4,stl ) ;
		
		printf( "Enter passphrase: " ) ;
		switch( StringSilentlyGetFromTerminal_1( pass_1,KEY_MAX_SIZE ) ){
			case 1 : return zuluExit( 19,stl ) ;
			case 2 : return zuluExit( 20,stl ) ;
		}
		
		printf( "\nRe enter passphrase: " ) ;
		switch( StringSilentlyGetFromTerminal_1( pass_2,KEY_MAX_SIZE ) ){
			case 1 : return zuluExit( 19,stl ) ;
			case 2 : return zuluExit( 20,stl ) ;
		}
		
		printf( "\n" ) ;
		
		if( !StringEqualString( *pass_1,*pass_2 ) ){
			st = 7 ;
		}else{				
			st = zuluCryptCreateVolume( device,fs,type,StringContent( *pass_1 ),StringLength( *pass_1 ),rng ) ;
		}				
	}else{	
		/*
		 * Make sure the user has provided all required options
		 */
		if( fs == NULL || type == NULL || pass == NULL || rng == NULL || keyType == NULL )
			return zuluExit( 4,stl ) ;
		
		/*
		 * "-p" options means a user has provided the passphrase
		 * "-f" option means a user has provided a path to where the passphrase is stored, StringGetFromFile_1 family
		 *  if functions are used to read files.One is used here to do that.
		 */
		if( strcmp( keyType,"-p" ) == 0 )
			st = zuluCryptCreateVolume( device,fs,type,pass,strlen( pass ),rng ) ;
		else if( strcmp( keyType, "-f" ) == 0 ) {
			/*
			 * function is defined at "security.c"
			 */
			switch( zuluCryptSecurityGetPassFromFile( pass,uid,content ) ){
				case 1 : return zuluExit( 8,stl )  ; 
				case 4 : return zuluExit( 15,stl ) ;
				case 2 : return zuluExit( 6,stl )  ;
				case 5 : return zuluExit( 22,stl ) ;
			}
			st = zuluCryptCreateVolume( device,fs,type,StringContent( *content ),StringLength( *content ),rng ) ;
		}else{
			st = 2 ;
		}
	}
	
	return st ? zuluExit( st,stl ) : zuluExit_1( type,stl ) ;
}

