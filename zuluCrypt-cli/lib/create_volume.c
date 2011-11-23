
#include "includes.h"

int create_volume( const char * dev,const char * fs,const char * type,const char * pass,const char * rng )
{
	StrHandle * q ;
	
	StrHandle * p ;
	
	const char * c ;
	
	struct stat st ;
	
	int status ;
	
	if ( stat( dev, &st ) != 0 ){
		return 1 ;
	}	
	
	if( strcmp( type,"luks" ) == 0 )
		if(  strcmp( rng,"/dev/random" ) != 0 )
			if( strcmp( rng,"/dev/urandom" ) != 0 )
				return 2 ;
			
			if( strncmp( fs,"ext3",4 ) != 0 )
				if( strncmp( fs,"ext4",4 ) != 0 )
					if( strncmp( fs,"vfat",4 ) != 0 )
						return 2 ;
					
					c = strrchr( dev,'/') ;
				
				if( c == NULL )
					p = String( dev ) ;
				else
					p = String( ++c ) ;
				
				StringReplaceCharString( p,'_',"#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ) ;
				
				StringPrepend( p,"/dev/mapper/zuluCrypt-create-" ) ;
				
				if( strcmp( type,"luks" )  == 0 ){
					
					status = create_luks( dev,pass,rng ) ;		
					
					if( status != 0 )
						return 3 ;
					
					status = open_luks( dev,StringContent( p ),"rw","-p",pass ) ;
					
					if( status != 0 )
						return 3 ;
					
				}else if( strcmp( type,"plain") == 0 ){
					
					status = open_plain( dev,StringContent( p ),"rw","-p",pass,"cbc-essiv:sha256" ) ;
					
					if( status != 0 )
						return 3 ;		
				}else{
					return 2 ;
				}
				
				q = String( ZULUCRYPTmkfs );
				
				StringAppend( q , " -t ") ;
				
				StringAppend( q , fs ) ;
				
				if( strcmp( fs,"vfat") == 0 )
					StringAppend( q , " " ) ;
				else
					StringAppend( q , " -m 1 " ) ;
				
				StringAppend( q , StringContent( p ) ) ;
				
				StringAppend( q , " 1>/dev/null 2>&1" ) ;
				
				execute( StringContent( q ),NULL,0 ) ;
				
				close_mapper( StringContent( p ) );
				
				StringDelete( p ) ;
				
				StringDelete( q ) ;
				
				return 0 ;	
}

