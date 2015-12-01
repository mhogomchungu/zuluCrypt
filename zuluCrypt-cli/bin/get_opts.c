/*
 *
 *  Copyright ( c ) 2012-2015
 *  name : Francis Banyikwa
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
#include <unistd.h>
#include <stdio.h>

void zuluCryptEXEGetOptsSetDefault( struct_opts * stopts )
{
	static char random[] = "/dev/urandom" ;
	static char type[]   = "luks" ;

	memset( stopts,'\0',sizeof( struct_opts ) ) ;

	stopts->fs   = "ext4" ;
	stopts->rng  = random ;
	stopts->type = type ;

	stopts->open_mount = 1 ;
	stopts->ask_confirmation = 1 ;
}

void zuluCryptEXEGetOpts( int argc,char * argv[],struct_opts * stopts )
{
	int c ;

	int k = 0 ;
	int n = 0 ;

	zuluCryptEXEGetOptsSetDefault( stopts ) ;

	while ( ( c = getopt( argc,argv,"MHZCUWTJLORBXASNPkhocsarqwibEDs:m:d:p:f:e:Y:i:z:g:y:u:l:n:j:t:G:F:V:K:" ) ) != -1 ) {

		switch( c ){

			case( 'H' ) : stopts->action = 'H' 	; break ;
			case( 'C' ) : stopts->action = 'C' 	; break ;
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

			case( 'O' ) : stopts->action = 'O' ;
			stopts->open_mount = 0 ;
			break ;
			case( 'T' ) : stopts->print_partition_type = 1 ;
			break;
			case( 'M' ) : stopts->share = 1 ;
			break ;
			stopts->share = 1 ;
			break ;
			case( 'Z' ) : stopts->print_partition_type = 2 ;
			break;
			case( 'B' ) : stopts->action = 'B' ;
			stopts->key_source = optarg ;
			break ;
			case( 'G' ) : stopts->plugin_path = optarg ;
			break ;
			case( 'R' ) : stopts->action = 'R' ;
			stopts->key_source = optarg ;
			break ;
			case( 'k' ) : stopts->ask_confirmation = 0 ;
			break ;
			/*
			 * ZULUCRYPTallPartitions,ZULUCRYPTsystemPartitions and ZULUCRYPTnonSystemPartitions
			 * are set in ../constants.h
			 */
			case( 'A' ) : stopts->partition_number = ZULUCRYPTallPartitions ;
			stopts->action = 'A' ;
			break ;
			case( 'S' ) : stopts->partition_number = ZULUCRYPTsystemPartitions ;
			stopts->action = 'S' ;
			break ;
			case( 'N' ) : stopts->partition_number = ZULUCRYPTnonSystemPartitions ;
			stopts->action = 'N' ;
			break ;
			case( 't' ) : stopts->type = optarg ;
			break ;
			case( 'K' ) : stopts->uid = optarg ;
			break ;
			case( 'm' ) : stopts->mount_point = optarg ;
			break ;
			case( 'd' ) : stopts->device = optarg ;
			break ;
			case( 'p' ) : stopts->key_source = "-p" ;
			stopts->key = optarg ;
			break ;
			case( 'F' ) :
				if( k < TRUECRYPT_MAX_KEYFILES ){
					/*
					* TRUECRYPT_MAX_KEYFILES is set at libzuluCrypt-exe.h
					*/
					stopts->tcrypt_multiple_keyfiles[ k ] = optarg ;
					k++ ;
				}
			break ;
			case( 'f' ) : stopts->key_source = "-f" ;
			stopts->key = optarg ;
			break ;
			case( 'e' ) :
				stopts->m_opts = optarg ;
				stopts->tcrypt_hidden_volume_size = optarg ;
			break ;
			case( 'Y' ) : stopts->fs_opts = optarg ;
			break ;
			case( 'z' ) : stopts->fs = optarg ;
			stopts->back_up_file_path = optarg ;
			break ;
			case( 'g' ) : stopts->rng = optarg ;
			break ;
			case( 'y' ) : stopts->existing_key_source = "-p" ;
			stopts->existing_key = optarg ;
			break ;
			case( 'u' ) :
				stopts->existing_key_source = "-f" ;
				stopts->existing_key = optarg ;
			break ;
			case( 'V' ) :
				if( n < TRUECRYPT_MAX_KEYFILES ){
					/*
					* TRUECRYPT_MAX_KEYFILES is set at libzuluCrypt-exe.h
					*/
					stopts->tcrypt_hidden_volume_multiple_keyfiles[ n ] = optarg ;
					n++ ;
				}
			break ;
			case( 'l' ) : stopts->new_key_source = "-p" ;
			stopts->new_key = optarg ;
			stopts->tcrypt_hidden_volume_key = optarg ;
			break ;
			case( 'n' ) : stopts->new_key_source = "-f" ;
			stopts->new_key = optarg ;
			break ;
			default: printf( "run zuluCrypt-cli --help for help\n" ) ;
		}
	}
}
