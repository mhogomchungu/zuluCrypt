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
#include <sys/types.h>
#include <string.h>
 
string_t zuluCryptPluginManagerGetKeyFromModule( const char * device,const char * name,uid_t uid,const char * argv ) ;

static int zuluExit( int st,char * device,char * m_point,stringList_t stl )
{
	/*
	 * this function is defined in ../utility/string/StringList.c
	 */
	StringListClearDelete( &stl ) ;
	
	switch ( st ){
		case 0 : printf( "SUCCESS: Volume opened successfully\n" ) ;								break ;
		case 1 : printf( "ERROR: failed to mount ntfs file system using ntfs-3g,is ntfs-3g package installed?\n" ) ;		break ;
		case 2 : printf( "ERROR: there seem to be an open volume accociated with given address\n" );				break ;
		case 3 : printf( "ERROR: no file or device exist on given path\n" ) ; 							break ;
		case 4 : printf( "ERROR: volume could not be opened with the presented key\n" );					break ;
		case 5 : printf( "ERROR: could not create mount point, invalid path or path already taken\n" ) ;			break ;
		case 6 : printf( "ERROR: invalid path to key file\n" );									break ;
		case 8 : printf( "ERROR: failed to open volume,device may already be in use or couldnt find a free loop device\n" );	break ;
		case 9 : printf( "ERROR: mount point path is already taken\n" );							break ; 
		/* case 10: currently unused */
		case 11: printf( "ERROR: one or more required argument(s) for this operation is missing\n" );				break ;
		case 12: printf( "ERROR: could not get a lock on /etc/mtab~\n" );							break ;
		case 13: printf( "ERROR: wrong argument for mode, valid options are \"rw\" or \"ro\"\n" );				break ;
		case 14: printf( "ERROR: could not get enought memory to hold the key file\n" );					break ;
		case 15: printf( "ERROR: failed to open volume and failed to close the mapper, advice to do it manunally\n" );		break ;
		case 16: printf( "ERROR: could not resolve full path of mount point\n" );						break ;
		case 17: printf( "ERROR: could not resolve full path of device address\n" );						break ;
		case 18: printf( "ERROR: -O and -m options can not be used together\n" );						break ;
		case 19: printf( "ERROR: insufficient privilege to search mount point path\n" );					break ;
		case 20: printf( "ERROR: insufficient privilege to open device\n" );							break ;
		case 21: printf( "ERROR: insufficient privilege to create a mount point\n" );						break ;
		case 22: printf( "ERROR: insufficient privilege to open key file for reading\n" );					break ;
		case 23: printf( "ERROR: insufficient privilege to open device in read/write mode\n" );					break ;
		case 24: printf( "ERROR: there seem to be an opened mapper associated with the device\n" ) ;				break ;
		case 25: printf( "ERROR: could not get a passphrase from the module\n" ) ;						break ;
		case 26: printf( "ERROR: could not get passphrase in silent mode\n" );							break ;
		case 27: printf( "ERROR: insufficient memory to hold passphrase\n" );							break ;
		case 28: printf( "ERROR: insufficient privilege to open plugin or path does not exist\n" );				break ;
		case 29: printf( "ERROR: could not get a passphrase through a local socket\n" );					break ;
		case 30: printf( "ERROR: mount point error" ) ;									        break ; 		
		default: printf( "ERROR: unrecognized error with status number %d encountered\n",st );
	}
	
	if( device != NULL )
		free( device ) ;
	
	if( m_point != NULL )
		free( m_point ) ;
	
	return st ;
}

/*
 * open_volume function below can be devided into two, the first part is before the mount point folder is created and the 
 * other part is after. This function is called after the mount point is created to see if it the mount point folder
 * should be removed first before calling the above function.The above function is called directly when "open_volume"
 * function is to be exited before the mount point is created. * 
 */
static int zuluExit_1( int st,const struct_opts * opts,char * device,char * cpoint,stringList_t stl )
{
	if( opts->open_no_mount == -1 && st != 0 )
		rmdir( opts->mount_point ) ;
	return zuluExit( st,device,cpoint,stl ) ;
}

int zuluCryptEXEOpenVolume( const struct_opts * opts,const char * mapping_name,uid_t uid )
{
	int nmp                  = opts->open_no_mount ;
	int i                    = opts->interactive_passphrase ;
	const char * dev         = opts->device ;
	const char * mount_point = opts->mount_point ;
	const char * mode        = opts->mode ;
	const char * source      = opts->key_source ;
	const char * pass        = opts->key ;
	const char * plugin_path = opts->plugin_path ;
	const char * argv      	 = opts->argv ;

	stringList_t stl = StringListInit() ;
	
	string_t * passphrase =  StringListAssign( stl ) ;	
	string_t * m_name     =  StringListAssign( stl ) ;	
	string_t * data       =  StringListAssign( stl ) ; 
	
	const char * cpass ;	
	const char * cname ;
	
	char * device = NULL ;
	char * cpoint = NULL ;
	
	size_t len ;
	int st = 0 ;
	
	/*
	 * This function is defined at "is_path_valid.c"
	 * It makes sure the path exists and the user has atleast reading access to the path.
	 * 
	 * The importance of the function is explained where it is defined.
	 */
	switch( zuluCryptSecurityCanOpenPathForReading( dev,uid ) ){
		case 0 : break ;
		case 1 : return zuluExit( 20,device,cpoint,stl ) ;
		default: return zuluExit( 3,device,cpoint,stl ) ;
	}
	
	if( mode == NULL )
		mode = "rw" ;
	
	if( strstr( mode,"ro" ) == NULL )
		if ( strstr( mode,"rw" ) == NULL )
			return zuluExit( 13,device,cpoint,stl ) ;
		
	if( strstr( mode,"rw" ) != NULL ){
		switch( zuluCryptSecurityCanOpenPathForWriting( dev,uid ) ){
			case 0 : break ;
			case 1 : return zuluExit( 23,device,cpoint,stl ) ;
			default: return zuluExit( 3,device,cpoint,stl ) ;
		}
	}
	
	device = realpath( dev,NULL ) ;
	if( device == NULL )
		return zuluExit( 17,device,cpoint,stl ) ;
	
	if( nmp == 1 && mount_point != NULL )
		return zuluExit( 18,device,cpoint,stl ) ;
	
	if( nmp == -1 ){
		if( mount_point == NULL )
			return zuluExit( 11,device,cpoint,stl ) ;
	
		/*
		 * zuluCryptSecurityCreateMountPoint() is defined in security.c
		 */
		switch( zuluCryptSecurityCreateMountPoint( mount_point,uid ) ){
			case 2 : return zuluExit( 5,device,cpoint,stl ) ;
			case 1 : return zuluExit( 21,device,cpoint,stl ) ;
		}
	
		cpoint = realpath( mount_point,NULL ) ;
		if( cpoint == NULL )
			return zuluExit_1( 16,opts,device,cpoint,stl ) ;
	}		

	/*
	 * This function is defined at "../lib/create_mapper_name.c"
	 * 
	 * Explanation for what it does is explained where it is defined. 
	 */
	*m_name = zuluCryptCreateMapperName( device,mapping_name,uid,OPEN ) ;

	cname = StringContent( *m_name ) ;
	
	/*
	 *  zuluCryptCheckOpenedMapper() is defined in check_opened_mapper.c 
	 */
	if( zuluCryptCheckOpenedMapper( cname ) == 1 ){
		if( cpoint != NULL )
			rmdir( cpoint ) ;
		
		return zuluExit( 24,device,cpoint,stl ) ;
	}
	
	if( plugin_path != NULL ){
		
		if( strstr( plugin_path,"/" ) != NULL )
			if( zuluCryptSecurityCanOpenPathForReading( plugin_path,uid ) != 0 )
				return zuluExit_1( 28,opts,device,cpoint,stl ) ;
				
		/*
		 * zuluCryptPluginManagerGetKeyFromModule is defined in ../pluginManager/zuluCryptPluginManager.c
		 */
		*passphrase = zuluCryptPluginManagerGetKeyFromModule( device,plugin_path,uid,argv ) ;
		
		if( *passphrase == StringVoid )
			return zuluExit_1( 25,opts,device,cpoint,stl ) ;
		
		cpass = StringContent( *passphrase ) ;
		len = StringLength( *passphrase ) ;
		
		st = zuluCryptOpenVolume( device,cname,cpoint,uid,mode,cpass,len ) ;
	
	}else if( i == 1 || source == NULL ){
		
		printf( "Enter passphrase: " ) ;
		switch( StringSilentlyGetFromTerminal_1( passphrase,KEY_MAX_SIZE ) ){
			case 1 : return zuluExit_1( 26,opts,device,cpoint,stl ) ;
			case 2 : return zuluExit_1( 27,opts,device,cpoint,stl ) ;
		}
		printf( "\n" ) ;
		cpass = StringContent( *passphrase ) ;
		len = StringLength( *passphrase ) ;
		st = zuluCryptOpenVolume( device,cname,cpoint,uid,mode,cpass,len ) ;
	}else{
		if( source == NULL || pass == NULL )
			return zuluExit_1( 11,opts,device,cpoint,stl ) ;
		
		if( strcmp( source,"-p" ) == 0 ){
			cpass = pass ;
			len = strlen( pass ) ;
			st = zuluCryptOpenVolume( device,cname,cpoint,uid,mode,cpass,len ) ;
		}else if( strcmp( source,"-f" ) == 0 ){
			/*
			 * function is defined at "security.c"
			 */
			switch( zuluCryptSecurityGetPassFromFile( pass,uid,data ) ){
				case 1 : return zuluExit_1( 6,opts,device,cpoint,stl ) ;
				case 2 : return zuluExit_1( 14,opts,device,cpoint,stl ) ; 
				case 4 : return zuluExit_1( 22,opts,device,cpoint,stl ) ;
				case 5 : return zuluExit_1( 29,opts,device,cpoint,stl ) ;
			}
			cpass = StringContent( *data ) ;
			len = StringLength( *data ) ;
			st = zuluCryptOpenVolume( device,cname,cpoint,uid,mode,cpass,len ) ;
		}
	}		
	
	zuluCryptCheckInvalidKey( opts->device ) ;
	
	return zuluExit_1( st,opts,device,cpoint,stl );	
}
