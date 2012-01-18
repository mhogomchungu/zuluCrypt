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

int check_partition( const char * device )
{
	char * c = partitions( SYSTEM_PARTITIONS ) ;	
	char * d = strstr( c , device ) ;	
	free( c ) ;
	if ( d == NULL )
		return -1 ;
	else
		return 1 ;	
}

int create_volumes( int argn,char * device,char * fs,char * mode,char * keyType,char * pass,char * rng  )
{
	string_t pass_1 ;
	string_t pass_2 ;
	string_t content ;
	
	char Y ;
	int st  ;
	struct stat xt ;
	
	if( is_path_valid( device ) == -1 ){
		st = 1 ;
		goto out ;
	}	
	stat( device, &xt ) ;	
	if( strncmp( device,"/dev/",5 ) != 0 && xt.st_size < 3145728 ){
		st = 9 ;
		goto out ;		
	}
	if ( check_partition( device ) == 1  ){		
		st = 10 ;
		goto out ;		
	}		
	if( is_path_valid( ZULUCRYPTmkfs ) == -1 ){		
		st = 11 ;
		goto out ;		
	}
	
	if(  argn == 5  ){
		printf( "ARE YOU SURE YOU WANT TO CREATE/OVERWRITE: \"%s\" ? Type \"Y\" if you are\n",device );		
		Y = getchar() ;		
		if (  Y != 'Y' )
			st = 5 ;
		else{			
			getchar();    //get rid of "\n" still in stdin buffer				
			printf( "Enter passphrase: " ) ;			
			pass_1 = get_passphrase(  );			
			printf( "\nRe enter passphrase: " ) ;			
			pass_2 = get_passphrase(  );				
			printf( "\n" ) ;			
			if(  StringCompare( pass_1 , pass_2 ) != 0  ){				
				st = 7 ;
			}else{				
				if( strcmp( mode,"luks" ) == 0  ){					
					printf( "enter 1 to use \"/dev/random\" device when generating the key  (  more secure but slower  )\n" ) ;
					printf( "enter 2 to use \"/dev/urandom\" device when generating the key (  secure enought and faster  )\n" ) ;
					Y = getchar() ;
					getchar() ;					
					if(  Y == '1' )
						st = create_volume( device,fs,mode,StringContent( pass_1 ),StringLength( pass_1 ),"/dev/random" );
					else if (  Y == '2'  )
						st = create_volume( device,fs,mode,StringContent( pass_1 ),StringLength( pass_1 ),"/dev/urandom" );
					else{
						st = 5 ;
						goto out ;
					}
				}else{
					st = create_volume( device,fs,mode,StringContent( pass_1 ),StringLength( pass_1 ),"NULL" ) ;						
				}		
			}
			StringDelete( pass_1 ) ;
			StringDelete( pass_2 ) ;				
		}		
	}else if (  argn == 8  ){		
		if(  strcmp( rng,"/dev/random" ) != 0 )
			if(  strcmp( rng,"/dev/urandom" ) != 0 ){
				st = 2 ;
				goto out ;
			}			
			if(  strcmp(  keyType, "-p"  ) == 0  ) {			
				
				st = create_volume( device,fs,mode,pass,strlen( pass ),rng ) ;			
				
			}else if(  strcmp(  keyType, "-f"  ) == 0  ) {
				switch( StringGetFromFile( &content,pass ) ){
					case 1 : st = 8 ; goto out ; 
					case 3 : st = 6 ; goto out ;
				}
				st = create_volume( device,fs,mode,StringContent( content ),StringLength( content ),rng ) ;					
				StringDelete( content ) ;				
			}else{
				st = 2 ;			
			}
	}else{
		st = 4 ;			
	}	
	out:	
	switch ( st ){
		case 0 : printf( "SUCCESS: volume created successfully\n" ) ;
		break  ;
		case 1 : printf( "ERROR: invalid path to a file or device\n" ) ;
		break  ;
		case 2 : printf( "ERROR: Wrong option type\n" );
		break  ;
		case 3 : printf( "ERROR: could not create an encrypted volume in a file or device\n" );
		break  ;	
		case 4 : printf( "ERROR: Wrong number of arguments\n" );
		break  ;
		case 5 : printf( "ERROR: Wrong choice, exiting\n" );
		break  ;
		case 7 : printf( "ERROR: passphrases do not match\n" ) ;
		break  ;	
		case 8 : printf( "ERROR: invalid path to key file\n" ) ;
		break  ;
		case 9 : printf( "ERROR: container file must be bigger than 3MB\n" ) ;
		break  ;
		case 10: printf( "ERROR: creating volumes on system partitions is not allowed.\n" );
		printf( "System partitions have active entries in /etc/fstab and /etc/crypttab\n" ) ;
		break  ;
		case 11 : printf( "ERROR: %s not found \n",ZULUCRYPTmkfs ) ;
		break  ;
		case 6 : printf( "ERROR: couldnt get enought memory to hold the key file\n" ) ;
		break  ;
		default:
			;
	}
	return st ;
}

