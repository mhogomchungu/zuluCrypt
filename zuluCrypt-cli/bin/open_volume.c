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

int open_volumes( int argn,char * device,char * mapping_name,int id,char * mount_point,char * mode,char * source,char * pass )
{
	string_t passphrase  ;	
	string_t m_name  ;	
	string_t m_point  ;
	
	int st ;
	
	struct stat xt ;

	m_name = String( mapping_name ) ;
	m_point = String( mount_point );

	if ( argn != 5 && argn != 7  ){
		st = 11 ;
		goto eerr ;
	}	
	if( strlen( mount_point ) == 1 ){
		if (  strcmp( mount_point,"," ) == 0 ){
			st = 10 ;
			goto eerr ;			
		}
	}	
	if( stat( mount_point,&xt ) == 0 ){		
		st = 9 ;
		goto eerr ;
	}	
	if ( strncmp( mode,"ro",2 ) != 0 ){
		if ( strncmp( mode,"rw",2 ) != 0 ){
			st = 10 ;
			goto eerr ;	
		}
	}		
	StringReplaceCharString( m_name,'_',"#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ) ;
	
	StringPrepend( m_name,"/dev/mapper/zuluCrypt-" ) ;

	while( StringEndsWithChar( m_point , '/' ) == 0 )
		StringRemoveRight( m_point,1 );
		
	if ( mkdir( StringContent( m_point ), S_IRWXU  ) != 0 ){		
		st = 5 ;			
		goto eerr ;
	
	}	
	if (  argn == 5  ){
		printf(  "Enter passphrase: "  ) ;		
		passphrase = get_passphrase(  );	
		printf( "\n" ) ;	
		st = open_volume( device,StringContent( m_name ),StringContent( m_point ),id,mode,StringContent( passphrase ),"-p" ) ;
		StringDelete( passphrase ) ;
	}else if (  argn == 7  ){
		st = open_volume( device,StringContent( m_name ),StringContent( m_point ),id,mode,pass,source ) ;	
	}else{
		st =  11 ;			
	}
	eerr:
	if( st == 0 )
		 printf( "SUCCESS: Volume opened successfully\n" );
	else{		
		remove( StringContent( m_point ) ) ;
		switch (  st  ){
			case 1 : printf( "ERROR: no free loop device to use\n" ) ; 
				break ;					
			case 2 : printf( "ERROR: there seem to be an open volume accociated with given address\n" );
				break ;				
			case 3 : printf( "ERROR: no file or device exist on given path\n" ) ; 
				break ;		
			case 4 : printf( "ERROR: wrong passphrase\n" );		
				break ;			
			case 5 : printf( "ERROR ccould not create mount point, invalid path or path already taken\n" ) ;
				break ;		
			case 6 : printf( "ERROR: passphrase file does not exist\n" );
				break ;	
			case 11 : printf( "ERROR: wrong number of arguments, run zuluCrypt with \"-h\" for help\n" );
				break ;
			case 8 : printf( "ERROR: failed to open volume\n" );
				break ;	
			case 10 : printf( "ERROR: \",\" ( comma ) is not a valid mount point\n" );
				break ;
			case 9 :  printf( "ERROR: mount point path is already taken\n" );		
				break ;	
			case 12 :  printf( "ERROR: could not get a lock on /etc/mtab~\n" );		
				 break ;	
			default :
				;
		}
	}	
	StringDelete( m_name ) ;
	StringDelete( m_point ) ;
	return st ;
}
