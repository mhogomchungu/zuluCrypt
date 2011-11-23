
#include "includes.h"

int volume_info(  const char * mapper  )
{
	char * output ;	
	
	int xt ;
	
	struct stat st;
	
	StrHandle * p = String(  mapper  ) ;
	
	/*
	 * It appears that cryptsetup creates two mappers when a volume name has bash special characters in them.
	 * One mapper will have the special character(bogus mapper) and another with '_'(the one in use)
	 * making it hard to predict what mapper will be created and used.
	 * 
	 * Manual substitution before cryptsetup does it will give us a better control of cryptsetup behavior. 
	 */
	StringReplaceCharString( p,'_',"#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ) ;
	
	StringPrepend( p,"/dev/mapper/zuluCrypt-" ) ;
	
	if(  stat(  StringContent( p ),&st ) != 0  ) {
		
		printf( "%s is inactive\n",StringContent( p ) ) ;
		
		xt =  1 ;
	}else{
		
		output = status(  StringContent( p )  ) ;
		
		printf( "%s\n",output );
		
		free( output ) ;
		
		xt = 0 ;
	}
	
	StringDelete( p );
	
	return xt ;
}

