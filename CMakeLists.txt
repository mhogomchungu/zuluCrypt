
cmake_minimum_required(VERSION 2.6)

#set( CMAKE_SHARED_LINKER_FLAGS "-s" )
#set( CMAKE_EXE_LINKER_FLAGS "-s" )

set ( zuluPluginManager "${PROJECT_BINARY_DIR}/zuluCrypt-cli/libzuluCryptPluginManager.so" ) 

INCLUDE(FindPkgConfig)

find_package( Qt4 COMPONENTS QtCore QtNetwork QtGui REQUIRED )
#add_definitions( "${Qt4_CFLAGS}" )

#find_package( KDE4 )

pkg_check_modules( GNOME_KEYRING gnome-keyring-1 )
pkg_check_modules( CRYPTSETUP libcryptsetup )
pkg_check_modules( BLKID blkid ) 
pkg_check_modules( PWQUALITY pwquality ) 
pkg_check_modules( LIBMOUNT mount ) 

if( NOT BLKID_FOUND )
	message ( FATAL_ERROR "ERROR: could not find blkid package" )
else( NOT BLKID_FOUND )
	#add_definitions( "${BLKID_CFLAGS}" )
	find_library( blkid_lib libblkid.so )
	if( blkid_lib )
		set( blkid "${blkid_lib}" )
	else( blkid_lib )
		message ( FATAL_ERROR "ERROR: could not find libblkid.so" )
	endif( blkid_lib )
endif( NOT BLKID_FOUND )
 
if( NOT LIBMOUNT_FOUND )
	message ( FATAL_ERROR "ERROR: could not find libmount package" )
else( NOT LIBMOUNT_FOUND )
	#add_definitions( "${LIBMOUNT_CFLAGS}" )
	find_library( mountLibrary libmount.so )
	if( NOT mountLibrary )
		message ( FATAL_ERROR "ERROR: could not find libblkid.so" )
	endif( NOT mountLibrary )
	
	find_file( newAPI libmount/libmount.h PATHS ${LIBMOUNT_INCLUDE_DIRS} )
	
	if( newAPI )
		file( WRITE ${PROJECT_BINARY_DIR}/libmount_header.h "\n#define USE_UNSTABLE_LIBMOUNT_API 1\n#define USE_NEW_LIBMOUNT_API 1\n#include <libmount/libmount.h>\n")
	else( newAPI )
		file( WRITE ${PROJECT_BINARY_DIR}/libmount_header.h "\n#define USE_UNSTABLE_LIBMOUNT_API 1\n#include <mount/mount.h>\n")
	endif( newAPI )
endif( NOT LIBMOUNT_FOUND )

if( NOT CRYPTSETUP_FOUND )
	message ( FATAL_ERROR "ERROR: could not find cryptsetup package" )
else( NOT CRYPTSETUP_FOUND )
	#add_definitions( "${CRYPTSETUP_CFLAGS}" )
	find_library( cryptsetup_lib libcryptsetup.so )
	if( NOT cryptsetup_lib )
		message ( FATAL_ERROR "ERROR: could not find libcryptsetup.so" )
	endif( NOT cryptsetup_lib )
endif( NOT CRYPTSETUP_FOUND )
	

if( UNIX AND NOT APPLE AND NOT CMAKE_CROSSCOMPILING AND NOT EXISTS "/etc/debian_version" )
	if ( CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT LIB_SUFFIX )
	     set ( LIB_SUFFIX 64 )
	endif( CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT LIB_SUFFIX)
endif( UNIX AND NOT APPLE AND NOT CMAKE_CROSSCOMPILING AND NOT EXISTS "/etc/debian_version" )
 
set( PLUGINPATH "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/zuluCrypt" )

file( WRITE ${PROJECT_BINARY_DIR}/plugin_path.h "\n#define ZULUCRYPTpluginPath      \"${PLUGINPATH}/\"\n" )

if( GNOME_KEYRING_FOUND )
	find_file( keyring_header gnome-keyring.h PATHS ${GNOME_KEYRING_INCLUDE_DIRS} )
	find_library( keyring_library libgnome-keyring.so )
	if( keyring_header )
		if( keyring_library )
			add_definitions( ${GNOME_KEYRING_CFLAGS} )
			set( KEYRINGSUPPORT "true" )
		endif( keyring_library )
	endif( keyring_header )
endif( GNOME_KEYRING_FOUND )


set( KDEINCLUDE $ENV{KDEDIR}/include )
set( KDELIB $ENV{KDEDIR}/lib )
set( KDE4_FOUND "true" )
if( KDE4_FOUND )
	#add_definitions( ${KDE4_DEFINITIONS} )
	find_file( kwallet_header kwallet.h PATHS ${KDE4_INCLUDE_DIRS} ${KDE4_INCLUDE_DIR} )
	find_library( kwallet_library libkwalletbackend.so )
	if( kwallet_library )
		if( kwallet_header )
			add_definitions( -I${KDEINCLUDE} ) 
			file( WRITE  ${PROJECT_BINARY_DIR}/can_build_kwallet.h "\n#define BUILD_KWALLET 1\n" )
			set( BUILDKWALLET "true" )
		else ( kwallet_header )
			file( WRITE  ${PROJECT_BINARY_DIR}/can_build_kwallet.h "\n#define BUILD_KWALLET 0\n" )
		endif( kwallet_header )
	else( kwallet_library )
		file( WRITE  ${PROJECT_BINARY_DIR}/can_build_kwallet.h "\n#define BUILD_KWALLET 0\n" )
	endif( kwallet_library)
endif( KDE4_FOUND )

if( PWQUALITY_FOUND )
	find_file ( header_pwquality pwquality.h PATHS ${PWQUALITY_INCLUDE_DIRS} )
	find_library ( library_pwquality libpwquality.so )
	set( BUILDPWQUALITY "true" )
	if( library_pwquality )
		if( header_pwquality )
			file( WRITE ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 1\n" )
			file( APPEND ${PROJECT_BINARY_DIR}/can_build_pwquality.h "extern \"C\"\n{\n#include <${header_pwquality}>\n}" )
		else( header_pwquality )
			file( WRITE ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 0\n" )
		endif( header_pwquality )
	else( library_pwquality )
		file( WRITE ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 0\n" )
	endif( library_pwquality )
endif( PWQUALITY_FOUND )

ADD_SUBDIRECTORY( zuluCrypt-cli )
ADD_SUBDIRECTORY( zuluMount-gui )
ADD_SUBDIRECTORY( zuluCrypt-gui )
ADD_SUBDIRECTORY( plugins )

#ADD_SUBDIRECTORY( test )

message ( STATUS "---------------------------------------------------------------------------" )
message ( STATUS "optional functionality that will be build" )
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
message ( STATUS "optional functionality that will NOT be build" )
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

install ( FILES zuluCrypt-cli.1.bz2 DESTINATION share/man/man1 )
install ( FILES zuluCrypt-test.1.bz2 DESTINATION share/man/man1 )
install ( FILES zuluCrypt-gui.1.bz2 DESTINATION share/man/man1 )

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY )

add_custom_target( uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )
