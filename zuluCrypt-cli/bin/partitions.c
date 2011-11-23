/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

char * partitions( int option )
{
	char * b ;
	
	char * c ;
	
	char * d ;
	
	char buffer[512];
	char uuid[36];
	char label[16];
	char device[12] ;
	
	StrHandle * command ;
	StrHandle * all ;
	StrHandle * system ;
	StrHandle * non_system ;
	
	FILE *f ;
	FILE *z ;
	
	f = fopen( "/proc/partitions","r" ) ;
	
	fgets( buffer,1,f  ) ;
	fgets( buffer,1,f  ) ;
	
	all = String( "" );	
	
	strcpy( device, "/dev/" ) ;
	
	b = device + 5 ;
	
	while (  fgets( buffer,512,f  ) != NULL  ){
		
		c = buffer ;
		
		while(  *c++ != '\n'  ) { ; }
		
		*c = '\0' ;
		
		d = c ;
		
		while(  *--d != ' '  ) { ; }
		
		d++ ;		
		
		if( strlen(  d  ) == 4 || (  strncmp(  d, "hd", 2  ) != 0 && strncmp(  d, "sd", 2 ) != 0  )  )
			continue ;
		
		strcpy( b , d ) ;
		
		StringAppend(  all, device );
	}
	
	fclose( f );	
	
	if(  option == ALL_PARTITIONS  )
		return StringDeleteHandle(  all  ) ;
	
	non_system = all ;
	
	system = String( "" );
	
	f = fopen( "/etc/fstab","r" );
	
	while (  fgets( buffer,512,f  ) != NULL  ){	
		
		if(  strncmp(  buffer , "/dev/", 5  ) == 0  ){
			
			c = buffer ;
			
			while (  *++c != ' '  ) { ; }
			
			*c++ = '\n' ;
			
			*c = '\0' ;			
			
			StringAppend(  system, buffer  ) ;		
			
			StringRemoveStringString( non_system, buffer ) ;
			
		}else if (  strncmp( buffer ,"UUID",4 ) == 0  ){
			
			c = buffer + 5 ;
			d = uuid ;
			
			while (  (  *d++ = *c++  ) != ' '  ) { ; }
			
			*--d = '\0' ;
			
			command = String( ZULUCRYPTblkid ) ;
			StringAppend(  command , " -U "  ) ;
			StringAppend(  command , uuid );
			
			z = popen(  StringContent(  command  ), "r"  ) ;			
			
			fgets(  buffer, 512, z  ) ;
			
			pclose(  z  ) ;
			
			StringDelete(  command  ) ;
			
			c = buffer ;
			
			while (  *c++ != '\n'  ) { ; }
			
			*c = '\0' ;
			
			StringAppend(  system, buffer  ) ;	
			
			StringRemoveStringString( non_system, buffer ) ;			
			
		}else if (  strncmp( buffer ,"LABEL",5 ) == 0  ){
			
			c = buffer + 6 ;
			d = label ;
			
			if (  *c == '\"' ){
				c++ ;
				while (  (  *d++ = *c++  ) != '\"'  ) { ; } 
			}else
				while (  (  *d++ = *c++  ) != ' '  ) { ; }
				
				*--d = '\0' ;
			
			command = String( ZULUCRYPTblkid ) ;
			StringAppend(  command , " -L "  ) ;
			StringAppend(  command , label );
			
			z = popen(  StringContent(  command  ), "r"  ) ;
			
			fgets(  buffer, 512, z  ) ;
			
			pclose(  z  ) ;
			
			StringDelete(  command  ) ;
			
			c = buffer ;
			
			while (  *c++ != '\n'  ) { ; }
			
			*c = '\0' ;
			
			StringAppend(  system, buffer  ) ;	
			
			StringRemoveStringString( non_system, buffer ) ;
		}		
	}
	
	fclose( f ) ;	
	
	f = fopen( "/etc/crypttab","r" );
	
	if(  f != NULL  ){
		
		while (  fgets( buffer,512,f  ) != NULL  ){	
			
			if( buffer[0] == '#' )
				continue ;
			
			if( buffer[0] == '\n' )
				continue ;
			
			c = buffer ;
			
			while(  *++c != '/'  ) { ; }
			
			d = c ;
			
			while(  *++d != ' '  ) { ; }
			
			*d++ = '\n' ;
			
			*d = '\0' ;
			
			StringAppend(  system, buffer  ) ;	
			
			StringRemoveStringString( non_system, buffer ) ;
		}
		
		fclose( f ) ;
	}	
	
	if(  option == SYSTEM_PARTITIONS  ){
		StringDelete(  non_system  ) ;
		return StringDeleteHandle(  system  ) ;
	}else{
		StringDelete(  system  ) ;
		return StringDeleteHandle(  non_system  ) ;		
	}
}
