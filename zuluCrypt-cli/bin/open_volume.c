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
 
string_t zuluCryptPluginManagerGetKeyFromModule( const char * device,const char * name,uid_t uid,const struct_opts * opts ) ;

static void _printResult( const char * device,const char * m_point,uid_t uid,const char * mapping_name )
{
	char * e ;
	string_t p ;
	
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	p = zuluCryptCreateMapperName( device,mapping_name,uid,CLOSE ) ;
	
	/*
	 * zuluCryptGetVolumeTypeFromMapperPath() is defined in ../lib/status.c
	 */
	e = zuluCryptGetVolumeTypeFromMapperPath( StringContent( p ) ) ;
	
	if( strstr( e,"LUKS" ) != NULL ){
		printf( "SUCCESS: luks volume opened successfully\n" ) ;
	}else if( strstr( e,"PLAIN" ) != NULL ){
		printf( "SUCCESS: plain volume opened successfully\n" ) ;
	}else if( strstr( e,"TCRYPT" ) != NULL ){
		printf( "SUCCESS: tcrypt volume opened successfully\n" ) ;
	}else{
		printf( "SUCCESS: volume opened successfully\n" ) ;
	}
	
	free( e ) ;
	StringDelete( &p ) ;
	zuluCryptSecurityDropElevatedPrivileges() ;
	printf( "volume mounted at: %s\n",m_point ) ;
}

static int zuluExit( int st,const char * device,char * m_point,stringList_t stl,uid_t uid,const char * mapping_name )
{
	zuluCryptSecurityDropElevatedPrivileges() ;
	/*
	 * this function is defined in ../utility/string/StringList.c
	 */
	StringListClearDelete( &stl ) ;
	
	switch ( st ){
		case 0 : _printResult( device,m_point,uid,mapping_name ) ;								break ;
		case -1: printf( "ERROR: failed to mount a filesystem,invalid mount option or permission denied\n" ) ;			break ;
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
		case 21: printf( "ERROR: mount point path already taken.\n" );								break ;
		case 22: printf( "ERROR: insufficient privilege to open key file for reading\n" );					break ;
		case 23: printf( "ERROR: volume could not be opened\n" );								break ;
		case 24: printf( "ERROR: there seem to be an opened mapper associated with the device\n" ) ;				break ;
		case 25: printf( "ERROR: could not get a passphrase from the module\n" ) ;						break ;
		case 26: printf( "ERROR: could not get passphrase in silent mode\n" );							break ;
		case 27: printf( "ERROR: insufficient memory to hold passphrase\n" );							break ;
		case 28: printf( "ERROR: insufficient privilege to open plugin or path does not exist\n" );				break ;
		case 29: printf( "ERROR: could not get a passphrase through a local socket\n" );					break ;
		case 30: printf( "ERROR: mount point error" ) ;									        break ; 
		case 31: printf( "ERROR: insufficient privilege to mount the device with given options\n" ) ;				break ;
		case 32: printf( "ERROR: ERROR: could not get elevated privilege,check binary permissions\n" ) ;			break ;
		case 33: printf( "ERROR: only root user can perform this operation\n" ) ;						break ;
		default: printf( "ERROR: unrecognized error with status number %d encountered\n",st );
	}
		
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
static int zuluExit_1( int st,const struct_opts * opts,const char * device,char * cpoint,stringList_t stl,uid_t uid,const char * mapping_name )
{
	if( opts->open_no_mount == -1 && st != 0 ){
		zuluCryptSecurityGainElevatedPrivileges() ;
		rmdir( cpoint ) ;
		zuluCryptSecurityDropElevatedPrivileges() ;
	}
	return zuluExit( st,device,cpoint,stl,uid,mapping_name ) ;
}

int zuluCryptEXEOpenVolume( const struct_opts * opts,const char * mapping_name,uid_t uid )
{
	int nmp                  = opts->open_no_mount ;
	const char * device      = opts->device ;
	const char * mount_point = opts->mount_point ;
	const char * m_opts      = opts->m_opts ;
	const char * source      = opts->key_source ;
	const char * pass        = opts->key ;
	const char * plugin_path = opts->plugin_path ;
	const char * fs_opts     = opts->fs_opts ;
	
	stringList_t stl = StringListInit() ;
	
	/*
	 * Below is a form of memory management.All strings are collected in a stringlist object to easily delete them
	 * when the function returns.This allows for the function to have multiple exit points without risks of leaking
	 * memory from manually examining each exit point to make sure all strings are deleted or go with multiple goto
	 * code deleting blocks to take into account different exit points. 
	 */
	string_t * passphrase =  StringListAssign( stl ) ;
	string_t * m_name     =  StringListAssign( stl ) ;
	string_t * data       =  StringListAssign( stl ) ; 
	string_t st_mpoint ;
	
	const char * cpass ;
	const char * cname ;
	
	char * cpoint = NULL ;
	
	size_t len ;
	int st = 0 ;
	
	unsigned long m_flags ;
	
	if( m_opts == NULL )
		m_opts = "rw" ;
	
	if( strstr( m_opts,"ro" ) == NULL )
		if ( strstr( m_opts,"rw" ) == NULL )
			return zuluExit( 13,device,cpoint,stl,uid,mapping_name ) ;
		
	/*
	 * zuluCryptMountFlagsAreNotCorrect() is defined in ./mount_flags.c
	 */
	if( zuluCryptMountFlagsAreNotCorrect( m_opts,uid,&m_flags ) )
		return zuluExit( 31,device,cpoint,stl,uid,mapping_name ) ; 
	
	if( strstr( m_opts,"rw" ) != NULL ){
		/*
		 * zuluCryptSecurityDeviceIsWritable() is defined in security.c
		 */
		st = zuluCryptSecurityDeviceIsWritable( device,uid ) ;
	}else{
		/*
		 * zuluCryptSecurityDeviceIsReadable() is defined in security.c
		 */
		st = zuluCryptSecurityDeviceIsReadable( device,uid ) ;
	}
	
	/*
	 * 1-permissions denied
	 * 2-invalid path
	 * 3-shenanigans
	 * 4-common error 
	 */
	switch( st ){
		case 0 :  break ;
		case 1 :  return zuluExit( 20,device,cpoint,stl,uid,mapping_name ) ;
		case 2 :  return zuluExit( 20,device,cpoint,stl,uid,mapping_name ) ;
		case 3 :  return zuluExit( 20,device,cpoint,stl,uid,mapping_name ) ;
		case 4 :  return zuluExit( 20,device,cpoint,stl,uid,mapping_name ) ;
		default:  return zuluExit( 20,device,cpoint,stl,uid,mapping_name ) ;
	}
	
	if( nmp == 1 ){
		if( uid != 0 ){
			return zuluExit( 33,device,cpoint,stl,uid,mapping_name ) ;
		}
		if( mount_point != NULL ){
			return zuluExit( 18,device,cpoint,stl,uid,mapping_name ) ;
		}
	}
	
	if( nmp == -1 ){
		/*
		* zuluCryptSecurityCreateMountPoint() is defined in security.c
		*/
		st_mpoint = zuluCryptSecurityCreateMountPoint( device,mount_point,uid ) ;
		cpoint = StringDeleteHandle( &st_mpoint ) ;
		if( cpoint == NULL ){
			return zuluExit( 21,device,cpoint,stl,uid,mapping_name ) ;
		}	
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
		
		return zuluExit_1( 24,opts,device,cpoint,stl,uid,mapping_name ) ;
	}
	
	if( plugin_path != NULL ){
		/*
		 * zuluCryptPluginManagerGetKeyFromModule is defined in ../pluginManager/zuluCryptPluginManager.c
		 */
		*passphrase = zuluCryptPluginManagerGetKeyFromModule( device,plugin_path,uid,opts ) ;
		
		if( *passphrase == StringVoid )
			return zuluExit_1( 25,opts,device,cpoint,stl,uid,mapping_name ) ;
		
		cpass = StringContent( *passphrase ) ;
		len = StringLength( *passphrase ) ;
		if( !zuluCryptSecurityGainElevatedPrivileges() )
			return zuluExit_1( 30,opts,device,cpoint,stl,uid,mapping_name ) ;
		st = zuluCryptOpenVolume( device,cname,cpoint,uid,m_flags,fs_opts,cpass,len ) ;
	
	}else if( source == NULL ){
		printf( "Enter passphrase: " ) ;
		switch( StringSilentlyGetFromTerminal_1( passphrase,KEY_MAX_SIZE ) ){
			case 1 : return zuluExit_1( 26,opts,device,cpoint,stl,uid,mapping_name ) ;
			case 2 : return zuluExit_1( 27,opts,device,cpoint,stl,uid,mapping_name ) ;
		}
		printf( "\n" ) ;
		cpass = StringContent( *passphrase ) ;
		len = StringLength( *passphrase ) ;
		if( !zuluCryptSecurityGainElevatedPrivileges() )
			return zuluExit_1( 30,opts,device,cpoint,stl,uid,mapping_name ) ;
		st = zuluCryptOpenVolume( device,cname,cpoint,uid,m_flags,fs_opts,cpass,len ) ;
	}else{
		if( source == NULL || pass == NULL )
			return zuluExit_1( 11,opts,device,cpoint,stl,uid,mapping_name ) ;
		if( strcmp( source,"-p" ) == 0 ){
			cpass = pass ;
			len = strlen( pass ) ;
			if( !zuluCryptSecurityGainElevatedPrivileges() )
				return zuluExit_1( 30,opts,device,cpoint,stl,uid,mapping_name ) ;
			st = zuluCryptOpenVolume( device,cname,cpoint,uid,m_flags,fs_opts,cpass,len ) ;
		}else if( strcmp( source,"-f" ) == 0 ){
			/*
			 * function is defined at "security.c"
			 */
			switch( zuluCryptSecurityGetPassFromFile( pass,uid,data ) ){
				case 1 : return zuluExit_1( 6,opts,device,cpoint,stl,uid,mapping_name ) ;
				case 2 : return zuluExit_1( 14,opts,device,cpoint,stl,uid,mapping_name ) ; 
				case 4 : return zuluExit_1( 22,opts,device,cpoint,stl,uid,mapping_name ) ;
				case 5 : return zuluExit_1( 29,opts,device,cpoint,stl,uid,mapping_name ) ;
			}
			cpass = StringContent( *data ) ;
			len = StringLength( *data ) ;
			if( !zuluCryptSecurityGainElevatedPrivileges() )
				return zuluExit_1( 30,opts,device,cpoint,stl,uid,mapping_name ) ;
			st = zuluCryptOpenVolume( device,cname,cpoint,uid,m_flags,fs_opts,cpass,len ) ;
		}
	}		
	
	zuluCryptCheckInvalidKey( opts->device ) ;
	return zuluExit_1( st,opts,device,cpoint,stl,uid,mapping_name );
}
