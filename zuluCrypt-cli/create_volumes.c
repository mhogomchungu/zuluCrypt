
#include "includes.h"

int create_volumes( int argn,char * device,char * fs,char * mode,char * keyType,char * pass,char * rng  )
{
	StrHandle * p ;
	StrHandle * q ;
	
	char Y ;
	int st ;
	struct stat xt ;
	char * c ;
	int z ;
	
	if(  stat(  device, &xt  ) != 0  ){
		st = 1 ;
		goto out ;
	}else if(  strncmp(  device,"/dev/",5 ) != 0 && xt.st_size < 3145728  ){
		st = 9 ;
		goto out ;		
	}	
	
	c = partitions(  SYSTEM_PARTITIONS  ) ;
	
	if (  strstr(  c , device  ) != NULL  ){		
		st = 10 ;
		free (  c  ) ;
		goto out ;		
	}	
	
	free (  c  ) ;
	
	if(  argn == 5  ){
		printf( "ARE YOU SURE YOU WANT TO CREATE/OVERWRITE: \"%s\" ? Type \"Y\" if you are\n",device );
		
		Y = getchar(  ) ;
		
		if (  Y != 'Y' )
			st = 5 ;
		else{			
			getchar(  );    //get rid of "\n" still in stdin buffer	
			
			printf( "Enter passphrase: " ) ;
			
			p = get_passphrase(  );
			
			printf( "\nRe enter passphrase: " ) ;
			
			q = get_passphrase(  );	
			
			printf( "\n" ) ;			
			
			if(  StringCompare(  p , q  ) != 0  ){
				
				st = 7 ;
			}else{
				
				if( strcmp( mode,"luks" ) == 0  ){
					
					printf( "enter 1 to use \"/dev/random\" device when generating the key  (  more secure but slower  )\n" ) ;
					printf( "enter 2 to use \"/dev/urandom\" device when generating the key (  secure enought and faster  )\n" ) ;
					
					Y = getchar(  ) ;
					getchar(  ) ;
					
					if(  Y == '1' )
						st = create_volume( device,fs,mode,StringContent(  p  ),"/dev/random" );
					else if (  Y == '2'  )
						st = create_volume( device,fs,mode,StringContent(  p  ),"/dev/urandom" );
					else{
						st = 5 ;
						goto out ;
					}
				}else{
					st = create_volume( device,fs,mode,StringContent(  p  ),"NULL" ) ;						
				}		
			}
			StringDelete(  p  ) ;
			StringDelete(  q  ) ;				
		}
		
	}else if (  argn == 8  ){
		
		if(  strcmp( rng,"/dev/random" ) != 0 )
			if(  strcmp( rng,"/dev/urandom" ) != 0 ){
				st = 2 ;
				goto out ;
			}
			
			if(  strcmp(  keyType, "-p"  ) == 0  ) {			
				
				st = create_volume( device,fs,mode,pass,rng ) ;			
				
			}else if(  strcmp(  keyType, "-f"  ) == 0  ) {
				
				if(  stat(  pass, &xt ) == 0  ) {
					
					c = (  char * ) malloc (  sizeof( char ) * (  xt.st_size + 1  )  ) ;
					
					if(  c == NULL  ){
						st = 6 ;
						goto out ;
					}
					
					*(  c + xt.st_size   ) = '\0' ;
					
					z = open( pass , O_RDONLY  ) ;
					
					read(  z, c, xt.st_size  ) ;
					
					close(  z  ) ;				
					
					st = create_volume( device,fs,mode,c,rng ) ;
					
					free(  c  ) ;
				}else{
					st = 8 ;
				}				
			}else{
				st = 2 ;			
			}
	}else{
		st = 4 ;			
	}
	
	out:
	
	switch (  st  ){
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
		case 6 : printf( "ERROR: couldnt get requested memory to open the key file.\n" );
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
		default:
			;
	}
	return st ;
}

