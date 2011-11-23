/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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
	StrHandle * p ;
	
	StrHandle * q ;
	
	StrHandle * z ;
	
	int st ;
	
	struct stat xt ;
	
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
	
	if(  stat (  mount_point,&xt ) != 0 ){
		
		st = 9 ;
		goto eerr ;
	}
	
	if ( strncmp( mode,"ro",2 ) != 0 ){
		if ( strncmp( mode,"rw",2 ) != 0 ){
			st = 10 ;
			goto eerr ;	
		}
	}	
	
	q = String(  mapping_name  ) ;
	
	StringReplaceCharString( q,'_',"#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ) ;
	
	StringPrepend( q,"/dev/mapper/zuluCrypt-" ) ;
	
	z = String( mount_point );
	
	StringAppend( z,"/" );
	
	StringAppend( z,mapping_name );
	
	if (  argn == 5  ){
		printf(  "Enter passphrase: "  ) ;
		
		p = get_passphrase(  );				
		
		printf( "\n" ) ;	
		
		st = open_volume( device,StringContent( q ),StringContent( z ),id,mode,StringContent(  p  ),"-p" ) ;
		
		StringDelete(  p  ) ;
		
	}else if (  argn == 7  ){
		
		st = open_volume( device,StringContent( q ),StringContent( z ),id,mode,pass,source ) ;			
		
	}else{
		
		st =  11 ;			
	}
	
	StringDelete(  q  ) ;
	
	StringDelete(  z  ) ;
	
	eerr:
	
	switch (  st  ){
		
		case 0 : printf( "SUCCESS: Volume opened successfully\n" );
		break ;			
		case 1 : printf( "ERROR: No free loop device to use\n" ) ; 
		break ;					
		case 2 : printf( "ERROR: There seem to be an open volume accociated with given address\n" );
		break ;				
		case 3 : printf( "ERROR: No file exist on given path\n" ) ; 
		break ;		
		case 4 : printf( "ERROR: Wrong passphrase\n" );
		break ;			
		case 5 : printf( "ERROR: a file or folder already exist at mount point\n" ) ;
		break ;		
		case 6 : printf( "ERROR: passphrase file does not exist\n" );
		break ;	
		case 11 : printf( "ERROR: Wrong number of arguments, run zuluCrypt with \"-h\" for help\n" );
		break ;
		case 8 : printf( "ERROR: failed to open volume\n" );
		break ;	
		case 10 : printf( "ERROR: \",\" ( comma ) is not a valid mount point\n" );
		break ;
		case 9 :  printf( "ERROR: mount point path does not exist\n" );
		break ;	
		
		default :
			;			
	}	
	return st ;
}
