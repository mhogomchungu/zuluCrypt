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

int addkey( int argn,char * device,char * keyType1,char * existingKey,char * keyType2,char * newKey )
{
	string_t * presentKey ;
	string_t * newKey_1 ;
	string_t * newKey_2 ;
	
	struct stat st ;
	int status = 0 ;
	char * c = NULL ;
	char * d = NULL ;
	char * e = NULL ;	
	
	if(  stat( device,&st ) != 0  ){		
		status = 4 ;
		goto out ;
	}
	
	e = empty_slots( device ) ;
	
	if( e == NULL ){		
		status = 2 ;
		goto out ;		
	}
	
	d = strchr( e, '0' ) ;
	
	if( d == NULL ){		
		status = 10 ;
		goto out ;		
	}
	
	if (  argn == 3  ){		
		printf( "Enter an existing passphrase: " ) ;		
		presentKey = get_passphrase(  ) ;		
		printf( "\n" ) ;		
		printf( "Enter the new passphrase: " ) ;		
		newKey_1 = get_passphrase(  ) ;		
		printf( "\n" ) ;		
		printf( "Re enter the new passphrase: " ) ;		
		newKey_2 = get_passphrase(  ) ;		
		printf( "\n" ) ;
		
		if(  StringCompare( newKey_1 , newKey_2 ) != 0  )			
			status = 7 ;
		else			
			status = add_key( device,StringContent( presentKey ), StringContent( newKey_1 ) ) ;			
		
		StringDelete( presentKey ) ;			
		StringDelete( newKey_1 ) ;	
		StringDelete( newKey_2 ) ;
		
	}else if(  argn == 7  ){		
		if ( strcmp(  keyType1, "-f"  ) == 0 ){			
			switch( read_file( &c,existingKey ) ){
				case 1 : status = 8 ; goto out ; 
				case 2 : status = 9 ; goto out ;
			}
		}		
		if ( strcmp(  keyType2, "-f"  ) == 0 ){				
			switch( read_file( &d,newKey ) ){
				case 1 : status = 8 ; goto out ; 
				case 2 : status = 9 ; goto out ; 		
			}
		}		
		if (  strcmp( keyType1,"-f" ) == 0 && strcmp( keyType2,"-f" ) == 0  ){
			status = add_key(  device, c, d ) ;			
			free(  c  ) ;
			free(  d  ) ;			
		}else if ( strcmp( keyType1,"-p" ) == 0 && strcmp( keyType2,"-p" ) == 0  ){
			
			status = add_key( device, existingKey, newKey  ) ;	
			
		}else if ( strcmp( keyType1,"-p" ) == 0 && strcmp( keyType2,"-f" ) == 0  ){
			
			status = add_key(  device, existingKey, d ) ;			
			free(  d  ) ;	
			
		}else if ( strcmp( keyType1,"-f" ) == 0 && strcmp( keyType2,"-p" ) == 0  ){			
			
			status = add_key(  device, c, newKey ) ;	
			
			free(  c  ) ;
		}else{			
			status = 5 ;
		}
	}else{
		status = 6 ;		
	}	
	out:	
	free( e ) ;	
	switch (  status  ){
		case 0 : printf( "SUCCESS: key added successfully\n" );
		break ;		
		case 1 : printf( "ERROR: presented key does not match any key in the volume\n" ) ;
		break ;
		case 2 : printf( "ERROR: could not open luks device, quiting\n" ) ;
		break ;
		case 3 : printf( "ERROR: device \"%s\" is not a luks device\n",device ) ;
		break ;
		case 4 : printf( "ERROR: device does not exist\n" ) ;
		break ;
		case 5 : printf( "ERROR: Wrong arguments\n" ) ;
		break ;
		case 6 : printf( "ERROR: Wrong number of arguments\n" ) ;
		break ;			
		case 7 : printf( "ERROR: new passphrases do not match\n" ) ;
		break ;
		case 8 : printf( "ERROR: one or both keyfile( s ) does not exist\n" ) ;
		break ;  
		case 9 : printf( "ERROR: Run out of memory\n" ) ;
		break ;
		case 10 : printf( "ERROR: all key slots are occupied, can not add any more keys\n" ) ;
		break ;
		default :
			;		
	}
	return status ;
}
