
cmake_minimum_required(VERSION 2.6)

set( KDEINCLUDE $ENV{KDEDIR}/include )
set( KDELIB $ENV{KDEDIR}/lib )

find_file( kwallet_header kwallet.h PATHS ${KDEINCLUDE} )

find_library( kwallet_library libkwalletbackend.so PATHS ${KDELIB} )

if( kwallet_header )
	if( kwallet_library )
		message( STATUS "Found kwallet.h  : " ${kwallet_header} )
		message( STATUS "Found libkwalletbackend.so  : " ${kwallet_library} )		
		ADD_SUBDIRECTORY( kwallet )	
	else( kwallet_library )
		message( STATUS "------------------------------\ncould not find libkwalletbackend.so,not building kwallet module\n------------------------------" )		
	endif( kwallet_library)
else( kwallet_header )
	message( STATUS "------------------------------\ncould not find kwallet.h,not building kwallet module\n------------------------------" )		
endif( kwallet_header )	

