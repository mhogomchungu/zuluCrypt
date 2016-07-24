
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)
cmake_policy(SET CMP0002 OLD)

set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS ON)

project(zuluCrypt)

INCLUDE(GNUInstallDirs)

set( PGR_VERSION "5.0.0" )
set( LIB_VERSION "1.2.0" )
set( LIB_PLUGIN_VERSION "1.0.0" )

#add_definitions( -D_DEFAULT_SOURCE -fstack-protector-all -D_FORTIFY_SOURCE=2 --param ssp-buffer-size=4 )
add_definitions( -D_DEFAULT_SOURCE -fstack-protector-all --param ssp-buffer-size=4 )

include_directories( ${PROJECT_BINARY_DIR} )

if( LIB_SUFFIX )
	set( CMAKE_INSTALL_LIBDIR "${LIB_SUFFIX}" )
	set( CMAKE_INSTALL_FULL_LIBDIR "${CMAKE_INSTALL_PREFIX}/${LIB_SUFFIX}" )
endif()

# These variables are used but they dont always appear to be set and we set them if we found them to be not set
if( NOT CMAKE_INSTALL_DOCDIR )
	set( CMAKE_INSTALL_DOCDIR "share/doc/zuluCrypt" )
endif()

if( NOT CMAKE_INSTALL_MANDIR )
	set( CMAKE_INSTALL_MANDIR "share/man" )
endif()

if( NOT CMAKE_INSTALL_DATADIR )
	set( CMAKE_INSTALL_DATADIR "share" )
endif()

if( NOT DEFINED QT5 )
    set( QT5 "true" )
endif()

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY )

add_custom_target( uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )

INCLUDE(CMakeDependentOption)
INCLUDE(FindPkgConfig)

# options
option(NOGUI "Don't build the GUI" OFF)
cmake_dependent_option(WITH_PWQUALITY "Build with pwquality support" ON
	"NOT NOGUI" OFF)
option(NOGNOME "Build without gnome support" OFF)
cmake_dependent_option(NOKDE "Build without kde support" OFF
	"NOT NOGUI" ON)
option(UDEVSUPPORT "Build without udev support" ON)

if( NOT NOGNOME )
	pkg_check_modules( LIBSECRET libsecret-1 )
else( NOT NOGNOME )
	set( NOSECRETSUPPORT "true" )
endif( NOT NOGNOME )

if( NOKDE )
	set( NOKDESUPPORT "true" )
endif( NOKDE )

pkg_check_modules( CRYPTSETUP libcryptsetup )
pkg_check_modules( BLKID blkid )
if ( WITH_PWQUALITY )
	pkg_check_modules( PWQUALITY pwquality )
endif ( WITH_PWQUALITY )

file( WRITE ${PROJECT_BINARY_DIR}/version_1.h "
#ifndef THIS_VERSION
#define THIS_VERSION \"${PGR_VERSION}\"
#endif
\n" )

file( WRITE ${PROJECT_BINARY_DIR}/version.h "
#ifndef ZULUCRYPT_VERSION
#define ZULUCRYPT_VERSION
#define VERSION_STRING \"Version  : ${PGR_VERSION}\\nCopyright: 2011-2016 Francis Banyikwa,mhogomchungu@gmail.com\\nLicense  : GPLv2+\"
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

if( NOT CRYPTSETUP_FOUND )
	message ( FATAL_ERROR "ERROR: could not find cryptsetup package" )
else( NOT CRYPTSETUP_FOUND )
	#add_definitions( "${CRYPTSETUP_CFLAGS}" )
	find_library( cryptsetup_lib libcryptsetup.so )
	if( NOT cryptsetup_lib )
		message ( FATAL_ERROR "ERROR: could not find libcryptsetup.so" )
	endif( NOT cryptsetup_lib )
endif( NOT CRYPTSETUP_FOUND )

find_file( GCRYPT_INCLUDE_FILE gcrypt.h )

find_library( GCRYPT_LIBRARY gcrypt )

if( NOT GCRYPT_INCLUDE_FILE )
	MESSAGE( FATAL_ERROR "could not find gcrypt header file" )
else()
	MESSAGE( STATUS "found gcrypt header file: ${GCRYPT_INCLUDE_FILE}" )
endif()

if( NOT GCRYPT_LIBRARY )
	MESSAGE( FATAL_ERROR "could not find gcrypt library(libgcrypt.so)" )
else()
	MESSAGE( STATUS "found gcrypt library: ${GCRYPT_LIBRARY}" )
endif()

file( WRITE  ${PROJECT_BINARY_DIR}/install_prefix.h "\n#define INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}/\"\n" )

if( REUSEMOUNTPOINT )
	file( WRITE  ${PROJECT_BINARY_DIR}/reuse_mount_point.h "\n#define REUSE_MOUNT_POINT 1\n" )
else()
	file( WRITE  ${PROJECT_BINARY_DIR}/reuse_mount_point.h "\n#define REUSE_MOUNT_POINT 0\n" )
endif()


set( PLUGINPATH "${CMAKE_INSTALL_FULL_LIBDIR}/zuluCrypt" )

file( WRITE ${PROJECT_BINARY_DIR}/plugin_path.h "\n#define ZULUCRYPTpluginPath      \"${PLUGINPATH}/\"\n" )
file( APPEND ${PROJECT_BINARY_DIR}/plugin_path.h "\n#define ZULUCRYPTTestPlugin      \"${PLUGINPATH}/zuluCrypt-testKey\"\n" )

set( BUILD_TCPLAY "true" )
set( TCPLAY_NEW_API "true" )
set( STATIC_TCPLAY "true" )

file( WRITE  ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TRUECRYPT_CREATE 1\n#include \"tcplay_api.h\"\n" )
file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TCPLAY_NEW_API 1\n" )
file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define VERACRYPT_CREATE 1\n" )

file( WRITE ${PROJECT_BINARY_DIR}/tcplay_support.h "\n#define TCPLAY_OPEN 1\n#include \"tcplay_api.h\"\n" )
file( WRITE ${PROJECT_BINARY_DIR}/veracrypt_support.h "\n#define VERACRYPT_SUPPORT 1\n" )
file( WRITE  ${PROJECT_BINARY_DIR}/truecrypt_support.h "\n#define TRUECRYPT_CRYPTSETUP 1\n#define TRUECRYPT_TCPLAY 1" )

ADD_SUBDIRECTORY( external_libraries/tc-play )
add_definitions( -I${PROJECT_SOURCE_DIR}/external_libraries/tc-play )
include_directories( ${PROJECT_SOURCE_DIR}/external_libraries/tc-play )

message ( STATUS "---------------------------------------------------------------------------" )
message ( STATUS "internal version of tcplay will be used to create and open TrueCrypt and VeraCrypt volumes." )
message ( STATUS "internal version of tcplay will be used to create and restore TrueCrypt volume headers." )
message ( STATUS "creation and restoration of VeraCrypt volume headers is currently disabled." )
message ( STATUS "---------------------------------------------------------------------------" )

if( SHARE_MOUNT_PREFIX )
    if( "${SHARE_MOUNT_PREFIX}" STREQUAL "default" )
	file( WRITE  ${PROJECT_BINARY_DIR}/share_mount_prefix_path.h "\n#define SHARE_MOUNT_PREFIX \"/run/media/public\"\n" )
    elseif( "${SHARE_MOUNT_PREFIX}" STREQUAL "" )
	file( WRITE  ${PROJECT_BINARY_DIR}/share_mount_prefix_path.h "\n#define SHARE_MOUNT_PREFIX \"/run/media/public\"\n" )
    else()
	file( WRITE  ${PROJECT_BINARY_DIR}/share_mount_prefix_path.h "\n#define SHARE_MOUNT_PREFIX \"${SHARE_MOUNT_PREFIX}\"\n" )
    endif()
else()
	file( WRITE  ${PROJECT_BINARY_DIR}/share_mount_prefix_path.h "\n#define SHARE_MOUNT_PREFIX \"/run/media/public\"\n" )
endif()

if( HOMEMOUNTPREFIX )
	file( WRITE  ${PROJECT_BINARY_DIR}/mount_prefix_path.h "\n#define USE_HOME_PATH_AS_MOUNT_PREFIX 1\n" )
else()
	file( WRITE  ${PROJECT_BINARY_DIR}/mount_prefix_path.h "\n#define USE_HOME_PATH_AS_MOUNT_PREFIX 0\n" )
endif()

add_library( crypt_buffer STATIC plugins/network_key/crypt_buffer.c zuluCrypt-cli/utility/socket/socket.c )
set_target_properties( crypt_buffer PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
TARGET_LINK_LIBRARIES( crypt_buffer -lgcrypt )

ADD_SUBDIRECTORY( zuluCrypt-cli )

if( NOT DEFINED INTERNAL_LXQT_WALLET )
	set( INTERNAL_LXQT_WALLET "false" )
endif()

if( INTERNAL_LXQT_WALLET )

	message ( STATUS "---------------------------------------------------------------------------" )
	message ( STATUS "lxqt wallet support will be provided by an internal library" )
	message ( STATUS "---------------------------------------------------------------------------" )

	include_directories( zuluCrypt-gui/lxqt_wallet )
	include_directories( zuluCrypt-gui/lxqt_wallet/frontend )
	include_directories( zuluCrypt-gui/lxqt_wallet/backend )

	add_subdirectory( zuluCrypt-gui/lxqt_wallet )
else()
	pkg_check_modules( LXQT_WALLET lxqt-wallet )

	if( NOT LXQT_WALLET_FOUND )

		message ( STATUS "ERROR: could not find lxqt_wallet package" )
		message ( STATUS "---------------------------------------------------------------------------" )
		message ( STATUS "lxqt wallet support will be provided by an internal library" )
		message ( STATUS "---------------------------------------------------------------------------" )

		include_directories( zuluCrypt-gui/lxqt_wallet )
		include_directories( zuluCrypt-gui/lxqt_wallet/frontend )
		include_directories( zuluCrypt-gui/lxqt_wallet/backend )

		add_subdirectory( zuluCrypt-gui/lxqt_wallet )
	else()
		if( LXQT_WALLET_VERSION VERSION_GREATER "2.0.0" )

			include_directories( ${LXQT_WALLET_INCLUDEDIR} )
			link_directories( ${LXQT_WALLET_LIBDIR} )

			message ( STATUS "---------------------------------------------------------------------------" )
			message ( STATUS "lxqt wallet support will be provided by an external libary" )
			message ( STATUS "---------------------------------------------------------------------------" )
		else()
			message ( STATUS "---------------------------------------------------------------------------" )
			message ( STATUS "lxqt wallet support will be provided by an internal library" )
			message ( STATUS "because external library is too old(<= 2.0.0)" )

			message ( STATUS "---------------------------------------------------------------------------" )

			include_directories( zuluCrypt-gui/lxqt_wallet )
			include_directories( zuluCrypt-gui/lxqt_wallet/frontend )
			include_directories( zuluCrypt-gui/lxqt_wallet/backend )

			add_subdirectory( zuluCrypt-gui/lxqt_wallet )
		endif()
	endif()
endif()

add_subdirectory( zuluSafe )

if( NOT NOGUI )

	if( WITH_PWQUALITY AND PWQUALITY_FOUND )
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
	else( WITH_PWQUALITY AND PWQUALITY_FOUND )
		file( WRITE ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 0\n" )
	endif( WITH_PWQUALITY AND PWQUALITY_FOUND )

	if( QT5 )
		find_package( Qt5Widgets REQUIRED )
		find_package( Qt5Core REQUIRED )
		find_package( Qt5Network REQUIRED )

		set( CMAKE_INCLUDE_CURRENT_DIR ON )
		include_directories( ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} )

		add_definitions( ${Qt5Widgets_DEFINITIONS} )
	else()
		find_package( Qt4 REQUIRED QtCore QtGui QtNetwork )

		add_definitions( -I${Qt4_INCLUDE_DIR} )
		add_definitions( -I${QT_HEADERS_DIR} )

		INCLUDE( ${QT_USE_FILE} )
        endif()

        ADD_SUBDIRECTORY( zuluCrypt-gui/sharedObjects )
	ADD_SUBDIRECTORY( zuluCrypt-gui )
	ADD_SUBDIRECTORY( plugins )
	ADD_SUBDIRECTORY( zuluMount-gui )

else( NOT NOGUI )
	if( QT5 )
	#adding this one just to silence a warning
	endif()
endif( NOT NOGUI )

file( WRITE ${PROJECT_BINARY_DIR}/locale_path.h "\n#define TRANSLATION_PATH \"${CMAKE_INSTALL_FULL_DATADIR}/zuluCrypt/translations/\"\n")

message ( STATUS "---------------------------------------------------------------------------" )
message ( STATUS "optional functionality that will be build" )

if( NOT NOGUI )
	message( STATUS "GUI front ends,zuluCrypt-gui and zuluMount-gui" )
endif( NOT NOGUI )

if( BUILDPWQUALITY )
	message( STATUS "pwquality library support to give passphrase strength measure" )
endif( BUILDPWQUALITY )

if( UDEVSUPPORT )
	message( STATUS "udev support will be enabled" )
endif( UDEVSUPPORT )

if( BUILD_TCPLAY )
	message( STATUS "tcplay adds the ability to create truecrypt compatible encrypted containers" )
endif( BUILD_TCPLAY )

if( CRYPTSETUP_VERSION VERSION_GREATER "1.6.3" )
	file( WRITE ${PROJECT_BINARY_DIR}/support_whirlpool.h "\n#define SUPPORT_WHIRLPOOL 1\n" )
else()
	file( WRITE ${PROJECT_BINARY_DIR}/support_whirlpool.h "\n#define SUPPORT_WHIRLPOOL 0\n" )
endif()

if( CRYPTSETUP_VERSION VERSION_GREATER "1.6.0" )
	file( WRITE ${PROJECT_BINARY_DIR}/check_tcrypt.h "\n#define CHECK_TCRYPT 1\n" )
else()
	file( WRITE ${PROJECT_BINARY_DIR}/check_tcrypt.h "\n#define CHECK_TCRYPT 0\n" )
endif()

message ( STATUS "---------------------------------------------------------------------------" )
message ( STATUS "---------------------------------------------------------------------------" )
message ( STATUS "optional functionality that will NOT be build" )

if( NOGUI )
	message( STATUS "GUI front ends,zuluCrypt-gui and zuluMount-gui\n" )
endif( NOGUI )

if( NOT BUILDPWQUALITY )
	message( STATUS "pwquality library used to measure passphrase strength" )
	message( STATUS "sources can be found at: https://fedorahosted.org/libpwquality/\n" )
endif( NOT BUILDPWQUALITY )

if( NOT UDEVSUPPORT )
	message( STATUS "udev support\n" )
endif( NOT UDEVSUPPORT )

message ( STATUS "---------------------------------------------------------------------------" )
message ( STATUS "\n" )

if( CRYPTSETUP_VERSION VERSION_GREATER "1.4.0" )
	file( WRITE ${PROJECT_BINARY_DIR}/luks_external_header.h "\n#define LUKS_EXTERNAL_HEADER 1\n" )
else( CRYPTSETUP_VERSION VERSION_GREATER "1.4.0" )
	file( WRITE ${PROJECT_BINARY_DIR}/luks_external_header.h "\n#define LUKS_EXTERNAL_HEADER 0\n" )
endif( CRYPTSETUP_VERSION VERSION_GREATER "1.4.0" )

if( UDEVSUPPORT )

	file( WRITE ${PROJECT_BINARY_DIR}/udev_support.h "\n#define UDEV_SUPPORT 1\n")

	message ( STATUS "udev will be consulted when deciding if a volume is system or not." )
	message ( STATUS "please read \"udev_support\" file for more information about udev support and associated problems.\n" )

else( UDEVSUPPORT )

	file( WRITE ${PROJECT_BINARY_DIR}/udev_support.h "\n#define UDEV_SUPPORT 0\n")

	message ( STATUS "udev will NOT be consulted when deciding if a volume is system or not." )
	message ( STATUS "please read \"udev_support\" file for more information about udev support and how to enable it." )
	message ( STATUS "It is probably a good idea to enable it if you are creating a package for distribution.\n" )

endif( UDEVSUPPORT )

if( NOT NOGUI )
	if( QT5 )
		message ( STATUS "---------------------------------------------------------------------------" )
		message ( STATUS "Building GUI components using Qt5" )
		message ( STATUS "---------------------------------------------------------------------------\n\n" )
	else()
		message ( STATUS "---------------------------------------------------------------------------" )
		message ( STATUS "Building GUI components using Qt4" )
		message ( STATUS "---------------------------------------------------------------------------\n\n" )
	endif()
else()
	message ( STATUS "---------------------------------------------------------------------------" )
	message ( STATUS "Not building GUI components" )
	message ( STATUS "---------------------------------------------------------------------------\n\n" )
endif()

install ( FILES zuluCrypt-cli.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
install ( FILES zuluCrypt-gui.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
install ( FILES zuluMount-cli.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
install ( FILES zuluMount-gui.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
install ( FILES zuluSafe-cli.1  DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
install ( FILES docs/zuluCrypt.pdf  DESTINATION ${CMAKE_INSTALL_DOCDIR} )

file( WRITE ${PROJECT_BINARY_DIR}/pdf_path.h "\n#define PDF_PATH \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR}/zuluCrypt.pdf\"\n" )

#install ( FILES translations/zuluMount/de_DE.qm DESTINATION share/zuluCrypt/translations/zuluMount-gui )
install ( FILES translations/zuluCrypt/de_DE.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluCrypt-gui )
install ( FILES translations/zuluCrypt/fr_FR.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluCrypt-gui )
install ( FILES translations/zuluCrypt/en_US.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluCrypt-gui )

install ( FILES translations/zuluMount/en_US.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluMount-gui )
install ( FILES translations/zuluMount/fr_FR.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluMount-gui )
