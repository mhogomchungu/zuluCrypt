/*
 * 
 *  Copyright ( c ) 2012
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
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

void zuluCryptEXEGetOptsSetDefault( struct_opts * stopts )
{
	memset( stopts,'\0',sizeof( struct_opts ) ) ;
	
	stopts->plugin_path = NULL ;
	stopts->device = NULL ;
	stopts->mount_point = NULL ;
	stopts->action = '\0' ;
	stopts->m_opts = NULL ;
	stopts->fs_opts = NULL ;
	stopts->key_source = NULL ;
	stopts->key = NULL ;
	stopts->fs = "ext4" ;
	stopts->type = "luks" ;
	stopts->rng = "/dev/urandom" ;
	stopts->existing_key_source = NULL ;
	stopts->existing_key = NULL ;
	stopts->new_key = NULL ;
	stopts->new_key_source = NULL ;
	stopts->partition_number = -1 ;
	stopts->dont_ask_confirmation = -1 ;
	stopts->open_no_mount = -1 ;
	stopts->argv = NULL ;
	stopts->print_partition_type = 0 ;
}

void zuluCryptEXEGetOpts( int argc,char * argv[],struct_opts * stopts )
{
	int c ;
	
	zuluCryptEXEGetOptsSetDefault( stopts ) ;
	
	while ( (c = getopt(argc,argv,"UWTJLORBXASNPkhocsarqwibEDs:m:d:p:f:e:Y:i:z:g:y:u:l:n:j:t:G:") ) != -1 ) {
		switch( c ){
			case( 'U' ) : stopts->action = 'U' 	; break ;     
			case( 'W' ) : stopts->action = 'W' 	; break ;
			case( 'E' ) : stopts->action = 'E' 	; break ;  
			case( 'D' ) : stopts->action = 'D' 	; break ; 
			case( 'X' ) : stopts->action = 'X'      ; break ;
			case( 'J' ) : stopts->action = 'J'      ; break ;
			case( 'L' ) : stopts->action = 'L'      ; break ; 
			case( 'o' ) : stopts->action = 'o'      ; break ;
			case( 'c' ) : stopts->action = 'c'      ; break ;
			case( 's' ) : stopts->action = 's'      ; break ;
			case( 'a' ) : stopts->action = 'a'      ; break ;
			case( 'r' ) : stopts->action = 'r'      ; break ;
			case( 'q' ) : stopts->action = 'q'      ; break ;
			case( 'w' ) : stopts->action = 'w'      ; break ;
			case( 'i' ) : stopts->action = 'i'      ; break ;
			case( 'b' ) : stopts->action = 'b'      ; break ;
			case( 'P' ) : stopts->action = 'P'      ; break ;
			case( 'O' ) : stopts->action = 'O'      ;
			stopts->open_no_mount = 1 ; break ;
			case( 'T' ) : stopts->print_partition_type = 1 ; 
			break;     
			case( 'B' ) : stopts->action = 'B' ;
			stopts->key_source = optarg ;
			break ;
			case( 'G' ) : stopts->plugin_path = optarg ;
			break ;
			case( 'R' ) : stopts->action = 'R' ;
			stopts->key_source = optarg ;
			break ;
			case( 'k' ) : stopts->dont_ask_confirmation = 1 ;
			break ;
			case( 'A' ) : stopts->partition_number = ALL_PARTITIONS ;
			stopts->action = 'A' ;
			break ;
			case( 'S' ) : stopts->partition_number = SYSTEM_PARTITIONS ;
			stopts->action = 'S' ;
			break ;
			case( 'N' ) : stopts->partition_number = NON_SYSTEM_PARTITIONS ;
			stopts->action = 'N' ;
			break ;
			case( 't' ) : stopts->type = optarg ;
			break ;
			case( 'm' ) : stopts->mount_point = optarg ;
			break ;
			case( 'd' ) : stopts->device = optarg ; 
			break ;
			case( 'p' ) : stopts->key_source = "-p" ; 
			stopts->key = optarg ;
			break ;
			case( 'f' ) : stopts->key_source = "-f" ;
			stopts->key = optarg ;
			break ;
			case( 'e' ) : stopts->m_opts = optarg ; 
			break ;
			case( 'Y' ) : stopts->fs_opts = optarg ; 
			break ;
			case( 'z' ) : stopts->fs = optarg ;
			break ;
			case( 'g' ) : stopts->rng = optarg ; 
			break ;
			case( 'y' ) : stopts->existing_key_source = "-p" ; 
			stopts->existing_key = optarg ;
			break ;
			case( 'u' ) : stopts->existing_key_source = "-f" ; 
			stopts->existing_key = optarg ;
			break ;
			case( 'l' ) : stopts->new_key_source = "-p" ; 
			stopts->new_key = optarg ;
			break ;
			case( 'n' ) : stopts->new_key_source = "-f" ; 
			stopts->new_key = optarg ;
			break ;
			default: printf( "run zuluCrypt-cli --help for help\n" ) ; 
		}
	}
}

