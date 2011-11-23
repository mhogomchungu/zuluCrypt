
#include "includes.h"

int unmount_volume( const char * map )
{
	StrHandle * p ;
	
	StrHandle * q ;
	
	StrHandle * z ;
	
	struct stat st ;		
	
	char * c ;
	
	char * d = NULL ; 
	
	FILE * f ;	
	
	char buffer[256] ;
	
	int h ;
	
	if ( stat( map , &st ) != 0 )
		return 1 ;		
	
	/*
	 * Go through /etc/mtab to found out mount point so that we can delete the mount point folder
	 * when unmounting succeed 
	 */
	f = fopen( "/etc/mtab","r" ) ;
	
	while( fgets( buffer,256,f ) != NULL ){
		
		if( strncmp( buffer,map,strlen( map ) )  == 0 ){
			
			c = d = buffer + strlen( map ) + 1  ;
			
			while ( *++c != ' ' ) { ; }
			
			*c = '\0' ;	
			
			break ;
		}		
	}
	fclose( f );
	
	if ( d == NULL )
		return 3 ;			
	
	p = String( d ) ;	
	
	/*
	 * a space character in /etc/mtab file is stored as "\040".
	 * replace these characters with space character again	  
	 */	
	StringReplaceString( p,"\\040"," " ) ;	
	
	z = String( StringContent( p ) ) ;
	
	/*
	 * Escape all characters that triggers bash intelligence to prevent it
	 * from "acting crazy" when a user uses a creative path name
	 */
	StringInsertCharString( z,'\\',"#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ) ;
	
	/*
	 * mount/umount system calls do not add entries in mtab and 
	 * tools like kdiskfree do not see the volumes when they dont have
	 * entries in /etc/mtab and manually adding/removing entries 
	 * in /etc/mtab corrupts the file.
	 * 
	 * workaround is to use the mount/umount executables through popen to mount/unmount volumes and
	 * deal with popen/bash issues.
	 */
	
	q = String( ZULUCRYPTumount ) ;
	
	StringAppend( q, " " );
	
	StringAppend( q, StringContent( z ) ) ;
	
	StringAppend( q, "  1>/dev/null 2>&1 ; " ) ;
	
	StringAppend( q,ZULUCRYPTecho ) ;
	
	StringAppend( q, " $?" ) ;
	
	execute( StringContent( q ),buffer,1 ) ;
	
	if( buffer[0] != '0' )		
		h = 2 ;			
	else{
		rmdir( StringContent( p ) ) ;	
		h = 0 ;
	}		
	
	StringDelete( p ) ;	
	
	StringDelete( q ) ;
	
	StringDelete( z ) ;
	
	return h ;
}
