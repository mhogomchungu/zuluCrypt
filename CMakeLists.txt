
cmake_minimum_required(VERSION 2.6)

#set( CMAKE_SHARED_LINKER_FLAGS "-s" )
#set( CMAKE_EXE_LINKER_FLAGS "-s" )

set( KDEINCLUDE $ENV{KDEDIR}/include )
set( KDELIB $ENV{KDEDIR}/lib )

find_file( blkid.h blkid/blkid.h )
 
if(NOT blkid.h)
	message (FATAL_ERROR "ERROR: could not find blkid.h header file?")
endif(NOT blkid.h)

message( STATUS "Found blkid.h  : " ${blkid.h} )
 
find_library( blkid libblkid.so) 
 
if(NOT blkid)	 
	message (FATAL_ERROR "ERROR: could not find libblkid.so library")
endif(NOT blkid)
 
message( STATUS "Found libblkid.so  : " ${blkid} )

ADD_SUBDIRECTORY( zuluCrypt-cli )
ADD_SUBDIRECTORY( zuluMount-gui )
ADD_SUBDIRECTORY( zuluCrypt-gui )
#ADD_SUBDIRECTORY( plugins )


find_file( kwallet_header kwallet.h PATHS ${KDEINCLUDE} )

find_library( kwallet_library libkwalletbackend.so PATHS ${KDELIB} )

if( kwallet_header )
	if( kwallet_library )
		message( STATUS "Found kwallet.h  : " ${kwallet_header} )
		message( STATUS "Found libkwalletbackend.so  : " ${kwallet_library} )	
		file( WRITE  ${PROJECT_BINARY_DIR}/can_build_kwallet.h "\n#define BUILD_KWALLET 1\n")
		ADD_SUBDIRECTORY( kwallet )	
	else( kwallet_library )
		file(WRITE  ${PROJECT_BINARY_DIR}/can_build_kwallet.h "\n#define BUILD_KWALLET 0\n")		
		message( STATUS "------------------------------\ncould not find libkwalletbackend.so,not building kwallet module\n------------------------------" )		
	endif( kwallet_library)
else( kwallet_header )
	message( STATUS "------------------------------\ncould not find kwallet.h,not building kwallet module\n------------------------------" )		
endif( kwallet_header )	

ADD_SUBDIRECTORY( test )
	 
install (FILES zuluCrypt-cli.1.bz2 DESTINATION share/man/man1)
install (FILES zuluCrypt-test.1.bz2 DESTINATION share/man/man1)
install (FILES zuluCrypt-gui.1.bz2 DESTINATION share/man/man1)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
