
cmake_minimum_required(VERSION 2.6)

#set( CMAKE_SHARED_LINKER_FLAGS "-s" )
#set( CMAKE_EXE_LINKER_FLAGS "-s" )

find_package( Qt4 REQUIRED )

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

find_file( keyring_header gnome-keyring.h PATHS /usr/include/gnome-keyring-1 /usr/include /usr/local/include /usr/local/include/glib-2.0 /usr/include/glib-2.0 /usr/lib/glib-2.0/include /usr/lib64/glib-2.0/include )
find_library( keyring_library libgnome-keyring.so )

if( keyring_header )
	if( keyring_library )
		add_definitions( -I/usr/include/gnome-keyring-1 -I/usr/include -I/usr/local/include -I/usr/local/include/glib-2.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/lib64/glib-2.0/include )
		set( KEYRINGSUPPORT "true" )
	endif( keyring_library )
endif( keyring_header )

find_file( kwallet_header kwallet.h PATHS ${KDEINCLUDE} )

find_library( kwallet_library libkwalletbackend.so PATHS ${KDELIB} )

if( kwallet_header )
	if( kwallet_library )
		set( BUILDKWALLET "true" )
		add_definitions( -I${KDEINCLUDE} ) 			
		message( STATUS "Found kwallet.h  : " ${kwallet_header} )
		message( STATUS "Found libkwalletbackend.so  : " ${kwallet_library} )	
		file( WRITE  ${PROJECT_BINARY_DIR}/can_build_kwallet.h "\n#define BUILD_KWALLET 1\n" )
		ADD_SUBDIRECTORY( kwallet )
	else( kwallet_library )
		file(WRITE  ${PROJECT_BINARY_DIR}/can_build_kwallet.h "\n#define BUILD_KWALLET 0\n")		
	endif( kwallet_library)
else( kwallet_header )
	file(WRITE  ${PROJECT_BINARY_DIR}/can_build_kwallet.h "\n#define BUILD_KWALLET 0\n")			
endif( kwallet_header )	

find_file ( header_pwquality pwquality.h )
find_library ( library_pwquality libpwquality.so )

if( header_pwquality )
	if( library_pwquality )
		set( BUILDPWQUALITY "true" )
		message ( STATUS "Found pwquality.h  : " ${header_pwquality} )		
		message( STATUS "Found libpwquality.so : " ${library_pwquality} )
		file( WRITE  ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 1\n")
		file( APPEND ${PROJECT_BINARY_DIR}/can_build_pwquality.h "extern \"C\"\n{\n#include <${header_pwquality}>\n}" )
	else( library_pwquality )
		file( WRITE  ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 0\n")
	endif( library_pwquality )
else( header_pwquality )
	file( WRITE  ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 0\n")	
endif( header_pwquality )

ADD_SUBDIRECTORY( zuluCrypt-cli )
ADD_SUBDIRECTORY( zuluMount-gui )
ADD_SUBDIRECTORY( zuluCrypt-gui )
ADD_SUBDIRECTORY( test )

message ( STATUS "---------------------------------------------------------------------------" )
message ( STATUS "external functionality that will be build" )
if( BUILDKWALLET )
	message( STATUS "kde kwallet support" )
endif( BUILDKWALLET )

if( KEYRINGSUPPORT )
	message( STATUS "gnome-keyring support" )
endif( KEYRINGSUPPORT )	

if( BUILDPWQUALITY )
	message( STATUS "pwquality library support to give passphrase strength measure" )
endif( BUILDPWQUALITY )	
message ( STATUS "---------------------------------------------------------------------------" )

message ( STATUS "---------------------------------------------------------------------------" )
message ( STATUS "external functionality that will NOT be build" )
if( NOT BUILDKWALLET )
	message( STATUS "kde kwallet support" )
endif( NOT BUILDKWALLET )

if( NOT KEYRINGSUPPORT )
	message( STATUS "gnome-keyring support" )
endif( NOT KEYRINGSUPPORT )	

if( NOT BUILDPWQUALITY )
	message( STATUS "pwquality library used to measure passphrase strength" )
	message( STATUS "sources can be found at:https://fedorahosted.org/libpwquality/" )	
endif( NOT BUILDPWQUALITY )	
message ( STATUS "---------------------------------------------------------------------------" )

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
