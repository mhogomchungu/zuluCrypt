
#include "includes.h"

int mount_volume( const char * mapper,const char * m_point,const char * mode,uid_t id )
{
	StrHandle * p ;
	
	StrHandle * q ;
	
	StrHandle * x ;
	
	char s[2] ;
	
	p = String( m_point ) ;		
	
	int h ;
	
	if ( mkdir( StringContent( p ), S_IRWXU  ) != 0 ){
		
		StringAppend( p, ".zc") ;
		
		if ( mkdir( StringContent( p ),S_IRWXU  ) != 0 ){
			
			close_mapper( mapper );
			
			StringDelete( p ) ;
			
			return 5 ;		
		}
	}
	
	if ( strncmp( mode, "ro",2 ) == 0 )
		q = String( ZULUCRYPTmount " -r " ) ;
	else
		q = String( ZULUCRYPTmount " -w " ) ;
	
	StringAppend( q , mapper ) ;
	
	StringAppend( q , " " );
	
	x = String( StringContent( p ) ) ;
	
	StringInsertCharString( x,'\\',"#;\"',\\`:!*?&$@(){}[]><|%~^ \n" ) ;
	
	StringAppend( q , StringContent( x ) ) ;
	
	StringAppend( q , "  2>/dev/null 1>&2 ; " ) ;
	
	StringAppend( q , ZULUCRYPTecho ) ;
	
	StringAppend( q , " $?" );
	
	execute( StringContent( q ),s,1 ) ;	
	
	if( s[0] != '0' ){
		
		remove( StringContent( p ) ) ;
		
		close_mapper( mapper );
		
		h = 4 ;
		
	}else{
		
		chown( StringContent( p ), id, id ) ;
		
		chmod( StringContent( p ), S_IRWXU ) ;
		
		h = 0 ;
	}
	
	StringDelete( p ) ;
	
	StringDelete( q ) ;
	
	StringDelete( x ) ;
	
	return h ;		
}

