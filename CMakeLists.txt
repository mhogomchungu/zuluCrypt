
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

#set( CMAKE_SHARED_LINKER_FLAGS "-s" )
#set( CMAKE_EXE_LINKER_FLAGS "-s" )

INCLUDE(FindPkgConfig)

if( NOT NOGNOME ) 
	pkg_check_modules( GNOME_KEYRING gnome-keyring-1 )
endif( NOT NOGNOME )

pkg_check_modules( CRYPTSETUP libcryptsetup )
pkg_check_modules( BLKID blkid ) 
pkg_check_modules( PWQUALITY pwquality ) 
pkg_check_modules( LIBMOUNT mount ) 
pkg_check_modules( TCPLAY tcplay ) 

set( PGR_VERSION "4.6.3" )
set( LIB_VERSION "1.2.0" )
set( LIB_PLUGIN_VERSION "1.0.0" )

file( WRITE ${PROJECT_BINARY_DIR}/version.h "
#ifndef ZULUCRYPT_VERSION
#define ZULUCRYPT_VERSION
#define VERSION_STRING \"version  : ${PGR_VERSION}\\ncopyright: 2011-2013 Ink Francis,mhogomchungu@gmail.com\\nlicense  : GPLv2+\"
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

if( TCPLAY_FOUND )
	if( NOT TCPLAY_VERSION VERSION_LESS "1.0.0" )
		set( BUILD_TCPLAY "true" )
		file( WRITE ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TRUECRYPT_CREATE 1\n#include <tcplay_api.h>\n" )
		add_definitions( -I${TCPLAY_INCLUDEDIR} ) 
		add_definitions( -I${TCPLAY_CFLAGS} ) 
		SET( TCPLAY_LIB "-ltcplay" )
	elseif( NOT TCPLAY_VERSION VERSION_LESS "1.0.0" )
		file( WRITE ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TRUECRYPT_CREATE 0\n" )
	endif( NOT TCPLAY_VERSION VERSION_LESS "1.0.0" )
else( TCPLAY_FOUND )	
	file( WRITE ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TRUECRYPT_CREATE 0\n" )	
endif( TCPLAY_FOUND )	
	
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

if( NOT NOKDE ) 
	find_package( KDE4 QUIET )
endif( NOT NOKDE )

file( WRITE  ${PROJECT_BINARY_DIR}/can_build_kwallet.h "\n#define BUILD_KWALLET 0\n" )

if( KDE4_FOUND )
	add_definitions( -I${KDE4_INCLUDE_DIR} )
	find_file( kwallet_header kwallet.h )
	#
	# alternative way to set the wallet library is by using -L${KDE4_LIB_DIR} -lkwalletbackend
	#
	find_library( kwallet_library libkwalletbackend.so )
	if( kwallet_library )
		if( kwallet_header )
			file( WRITE  ${PROJECT_BINARY_DIR}/can_build_kwallet.h "\n#define BUILD_KWALLET 1\n#include<kwallet.h>\n" )
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

ADD_SUBDIRECTORY( zuluCrypt-gui/md5 )
ADD_SUBDIRECTORY( zuluCrypt-gui )
ADD_SUBDIRECTORY( plugins )
ADD_SUBDIRECTORY( zuluMount-gui )

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

if( UDEVSUPPORT STREQUAL "true" )
	message( STATUS "udev support will be enabled" )
endif( UDEVSUPPORT STREQUAL "true" )
		
if( BUILD_TCPLAY )
	message( STATUS "tcplay adds the ability to create truecrypt compatible encrypted containers" )
endif( BUILD_TCPLAY )

if( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
	message( STATUS "version of cryptsetup is atleast 1.6.0,support for opening of truecrypt compatible containers is enabled" )
endif( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )

message ( STATUS "---------------------------------------------------------------------------" )
message ( STATUS "---------------------------------------------------------------------------" )
message ( STATUS "optional functionality that will NOT be build" )

if( NOGUI )
	message( STATUS "GUI front ends,zuluCrypt-gui and zuluMount-gui\n" )
endif( NOGUI )

if( NOT BUILDKWALLET )
	message( STATUS "kde kwallet support\n" )
endif( NOT BUILDKWALLET )

if( NOT KEYRINGSUPPORT )
	message( STATUS "gnome-keyring support\n" )
endif( NOT KEYRINGSUPPORT )

if( NOT BUILDPWQUALITY )
	message( STATUS "pwquality library used to measure passphrase strength" )
	message( STATUS "sources can be found at: https://fedorahosted.org/libpwquality/\n" )
endif( NOT BUILDPWQUALITY )

if( NOT UDEVSUPPORT STREQUAL "true" )
	message( STATUS "udev support,please see build instructions on how to enable it\n" )
endif( NOT UDEVSUPPORT STREQUAL "true" )
		
if( NOT BUILD_TCPLAY )
	message( STATUS "tcplay.Package not found or found version is too old(found version is less than 1.0.0)" )
	message( STATUS "You wont be able to create truecrypt compatible encrypted containers" )
	message( STATUS "tcplay project is hosted at: https://github.com/bwalex/tc-play\n" )	
endif( NOT BUILD_TCPLAY )

if( NOT CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
	message( STATUS "version of cryptsetup is less than 1.6.0,support for opening of truecrypt compatible containers is disabled" )
endif( NOT CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
	
message ( STATUS "---------------------------------------------------------------------------" )
message ( STATUS "\n" )

if( BUILD_TCPLAY )
	if( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
		file( WRITE  ${PROJECT_BINARY_DIR}/truecrypt_support.h "\n#define TRUECRYPT_CRYPTSETUP 1\n#define TRUECRYPT_TCPLAY 1" )
#		message ( STATUS "truecrypt volumes will be opened using cryptsetup\n" )
	else( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
		file( WRITE ${PROJECT_BINARY_DIR}/truecrypt_support.h "\n#define TRUECRYPT_CRYPTSETUP 0\n#define TRUECRYPT_TCPLAY 1" )
#		message ( STATUS "truecrypt volumes will be opened using tcplay\n" )
	endif( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
	file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support.h "\n#include <tcplay_api.h>" )
else( BUILD_TCPLAY )
	if( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
		file( WRITE ${PROJECT_BINARY_DIR}/truecrypt_support.h "\n#define TRUECRYPT_CRYPTSETUP 1\n#define TRUECRYPT_TCPLAY 0" )
#		message ( STATUS "truecrypt volumes will be opened using cryptsetup\n" )
	else( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
		file( WRITE ${PROJECT_BINARY_DIR}/truecrypt_support.h "\n#define TRUECRYPT_CRYPTSETUP 0\n#define TRUECRYPT_TCPLAY 0" )		
	endif( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
endif( BUILD_TCPLAY )

#overdide the above for now
if( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
	file( WRITE ${PROJECT_BINARY_DIR}/truecrypt_support.h "\n#define TRUECRYPT_CRYPTSETUP 1\n#define TRUECRYPT_TCPLAY 0" )
else( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
	file( WRITE ${PROJECT_BINARY_DIR}/truecrypt_support.h "\n#define TRUECRYPT_CRYPTSETUP 0\n#define TRUECRYPT_TCPLAY 0" )
endif( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
	

if( UDEVSUPPORT STREQUAL "true" )
	
	file( WRITE ${PROJECT_BINARY_DIR}/udev_support.h "\n#define UDEV_SUPPORT 1\n")
	
	message ( STATUS "udev will be consulted when deciding if a volume is system or not" )
	message ( STATUS "if a partition does not have an entry in fstab,udev will be consulted before a partition is considered non system" )
	message ( STATUS "udev has a bad habit of identifiying partitions as internal when they are not,please read its documentation" )
	message ( STATUS "when it happens" )	
else( UDEVSUPPORT STREQUAL "true" )
	
	file( WRITE ${PROJECT_BINARY_DIR}/udev_support.h "\n#define UDEV_SUPPORT 0\n")
	
	message ( STATUS "udev will NOT be consulted when deciding if a volume is system or not." )	
	message ( STATUS "This means ONLY partitions with an entry in fstab will be considered as system partition" )
	message ( STATUS "Please read section 5.0 of the FAQ located at: http://code.google.com/p/zulucrypt/wiki/FAQ" )
	message ( STATUS "before continuing.It is very important to make sure all partitions that are to be considered system" )
	message ( STATUS "have an entry in fstab before continuing." )
	message ( STATUS "Please make this known to your users if you are a distributor and udev support is off\n" )	
endif( UDEVSUPPORT STREQUAL "true" )

install ( FILES zuluCrypt-cli.1.bz2 DESTINATION share/man/man1 )
install ( FILES zuluCrypt-gui.1.bz2 DESTINATION share/man/man1 )
install ( FILES zuluMount-cli.1.bz2 DESTINATION share/man/man1 )
install ( FILES zuluMount-gui.1.bz2 DESTINATION share/man/man1 )
install ( FILES zuluCrypt-test.1.bz2 DESTINATION share/man/man1 )

install ( SCRIPT "${CMAKE_SOURCE_DIR}/PostInstall.cmake")

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY )

add_custom_target( uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )
