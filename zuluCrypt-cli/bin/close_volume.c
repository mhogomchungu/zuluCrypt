 
#include "includes.h"

int close_opened_volume(  char * mapping_name  )
{	
	 int st ;
	 
	 StrHandle * p = String(  mapping_name  ) ;
	 
	 StringReplaceCharString( p,'_',"#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ) ;
	 
	 StringPrepend( p,"/dev/mapper/zuluCrypt-" ) ;	
	 
	 st = close_volume(  StringContent(  p  )  ) ;
	 
	 switch(  st  ) {
		 case 0 : printf( "SUCCESS: volume closed successfully \n" );
		 break ;
		 case 1 : printf( "ERROR: close failed, encrypted volume with that name does not exist\n" );
		 break ;			
		 case 2 : printf( "ERROR: close failed, the mount point and/or one or more files are in use\n" );
		 break ;
		 case 3 : printf( "ERROR: close failed, volume does not have an entry in /etc/mtab\n" ) ;
		 break ;
		 default :
			 ; //shouldnt get here			
	 }	
	 
	 StringDelete(  p  ) ;
	 
	 return st ;	
}
