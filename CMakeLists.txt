
cmake_minimum_required(VERSION 2.6)

#set( CMAKE_SHARED_LINKER_FLAGS "-s" )
#set( CMAKE_EXE_LINKER_FLAGS "-s" )

set ( zuluPluginManager "${PROJECT_BINARY_DIR}/zuluCrypt-cli/libzuluCryptPluginManager.so" ) 

INCLUDE(FindPkgConfig)

pkg_check_modules( GNOME_KEYRING gnome-keyring-1 )
pkg_check_modules( CRYPTSETUP libcryptsetup )
pkg_check_modules( BLKID blkid ) 
pkg_check_modules( PWQUALITY pwquality ) 
pkg_check_modules( LIBMOUNT mount ) 

set( PGR_VERSION "version  : 4.5.3" )
file( WRITE ${PROJECT_BINARY_DIR}/version.h "
#ifndef ZULUCRYPT_VERSION
#define ZULUCRYPT_VERSION
#define VERSION_STRING \"${PGR_VERSION}\\ncopyright: (c)2011,2012 Ink Francis aka mhogomchungu\\nlicense  : GPLv2+\"
#endif
\n" )

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
		file( WRITE ${PROJECT_BINARY_DIR}/libmount_header.h "\n#define USE_UNSTABLE_LIBMOUNT_API 1\n#define USE_NEW_LIBMOUNT_API 0\n#include <mount/mount.h>\n")
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

if( LIB_SUFFIX )
	set ( LIB_PATH "${CMAKE_INSTALL_PREFIX}/${LIB_SUFFIX}" )
else( LIB_SUFFIX )
	if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		set ( LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib64" )
	else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		set ( LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib" )
	endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
endif( LIB_SUFFIX )
 
set( PLUGINPATH "${LIB_PATH}/zuluCrypt" )

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

ADD_SUBDIRECTORY( zuluCrypt-cli )

if( NOT NOGUI ) 
	
find_package( Qt4 COMPONENTS QtCore QtNetwork QtGui REQUIRED )
find_package( KDE4 QUIET )

file( WRITE  ${PROJECT_BINARY_DIR}/can_build_kwallet.h "\n#define BUILD_KWALLET 0\n" )

if( KDE4_FOUND )
	add_definitions( -I${KDE4_INCLUDE_DIR} )
	find_file( kwallet_header kwallet.h )
	find_library( kwallet_library libkwalletbackend.so )
	if( kwallet_library )
		if( kwallet_header )
			file( WRITE  ${PROJECT_BINARY_DIR}/can_build_kwallet.h "\n#define BUILD_KWALLET 1\n" )
			set( BUILDKWALLET "true" )
		endif( kwallet_header )
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
else( PWQUALITY_FOUND )	
	file( WRITE ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 0\n" )
endif( PWQUALITY_FOUND )

ADD_SUBDIRECTORY( plugins )
ADD_SUBDIRECTORY( zuluMount-gui )
ADD_SUBDIRECTORY( zuluCrypt-gui )

endif( NOT NOGUI )

message ( STATUS "---------------------------------------------------------------------------" )
message ( STATUS "optional functionality that will be build" )

if( NOT NOGUI )
	message( STATUS "GUI front ends,zuluCrypt-gui and zuluMount-gui" )
endif( NOT NOGUI )
	
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

if( NOGUI )
	message( STATUS "GUI front ends,zuluCrypt-gui and zuluMount-gui" )
endif( NOGUI )

if( NOT BUILDKWALLET )
	message( STATUS "kde kwallet support" )
endif( NOT BUILDKWALLET )

if( NOT KEYRINGSUPPORT )
	message( STATUS "gnome-keyring support" )
endif( NOT KEYRINGSUPPORT )

if( NOT BUILDPWQUALITY )
	message( STATUS "pwquality library used to measure passphrase strength" )
	message( STATUS "sources can be found at: https://fedorahosted.org/libpwquality/" )	
endif( NOT BUILDPWQUALITY )
message ( STATUS "---------------------------------------------------------------------------" )

install ( FILES zuluCrypt-cli.1.bz2 DESTINATION share/man/man1 )
install ( FILES zuluCrypt-test.1.bz2 DESTINATION share/man/man1 )
install ( FILES zuluCrypt-gui.1.bz2 DESTINATION share/man/man1 )

install ( SCRIPT "${CMAKE_SOURCE_DIR}/PostInstall.cmake")

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY )

add_custom_target( uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )
