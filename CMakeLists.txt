
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

add_definitions( -Wno-unused-result -D_DEFAULT_SOURCE )
include_directories( ${PROJECT_BINARY_DIR} )

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY )

add_custom_target( uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )

add_definitions( -I${PROJECT_BINARY_DIR}/zuluCrypt-gui/lxqt_wallet/frontend )

INCLUDE(CMakeDependentOption)
INCLUDE(FindPkgConfig)

# options
option(NOGUI "Don't build the GUI" OFF)
cmake_dependent_option(WITH_PWQUALITY "Build with pwquality support" ON
	"NOT NOGUI" OFF)
option(WITH_TCPLAY "Build with tcplay support" ON)
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
if ( WITH_TCPLAY )
	pkg_check_modules( TCPLAY tcplay )
endif ( WITH_TCPLAY )

set( PGR_VERSION "4.7.6" )
set( LIB_VERSION "1.2.0" )
set( LIB_PLUGIN_VERSION "1.0.0" )

file( WRITE ${PROJECT_BINARY_DIR}/version.h "
#ifndef ZULUCRYPT_VERSION
#define ZULUCRYPT_VERSION
#define VERSION_STRING \"Version  : ${PGR_VERSION}\\nCopyright: 2011-2015 Francis Banyikwa,mhogomchungu@gmail.com\\nLicense  : GPLv2+\"
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
file( APPEND ${PROJECT_BINARY_DIR}/plugin_path.h "\n#define ZULUCRYPTTestPlugin      \"${PLUGINPATH}/zuluCrypt-testKey\"\n" )

if( WITH_TCPLAY )

	if( TCPLAY_FOUND )

		if( TCPLAY_VERSION VERSION_GREATER "2.0.0" )
			set( BUILD_TCPLAY "true" )
			set( TCPLAY_NEW_API "true" )
			set( TCPLAY_LIB "-ltcplay" )
			add_definitions( -I${TCPLAY_INCLUDEDIR} )
			add_definitions( -I${TCPLAY_CFLAGS} )

			if( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
				file( WRITE  ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TRUECRYPT_CREATE 1\n#include <tcplay_api.h>\n" )
				file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TCPLAY_NEW_API 1\n" )
			else()
				file( WRITE  ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TRUECRYPT_CREATE 0\n#include <tcplay_api.h>\n" )
				file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TCPLAY_NEW_API 0\n" )
			endif()

			if( CRYPTSETUP_VERSION VERSION_GREATER "1.6.6" )
				file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define VERACRYPT_CREATE 1\n" )
			else()
				file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define VERACRYPT_CREATE 0\n" )
			endif()
		else()
			if( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
				SET( STATIC_TCPLAY "true" )

				file( WRITE  ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TRUECRYPT_CREATE 1\n#include \"tcplay_api.h\"\n" )
				file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TCPLAY_NEW_API 1\n" )

				if( CRYPTSETUP_VERSION VERSION_GREATER "1.6.6" )
					file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define VERACRYPT_CREATE 1\n" )
				else()
					file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define VERACRYPT_CREATE 0\n" )
				endif()
			else()

				file( WRITE  ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TCPLAY_NEW_API 0\n" )
				file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TRUECRYPT_CREATE 0\n" )
				file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define VERACRYPT_CREATE 0\n" )

			endif()

		endif()
	else()
		if( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
			SET( STATIC_TCPLAY "true" )

			file( WRITE  ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TRUECRYPT_CREATE 1\n#include \"tcplay_api.h\"\n" )
			file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TCPLAY_NEW_API 1\n" )

			if( CRYPTSETUP_VERSION VERSION_GREATER "1.6.6" )
				file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define VERACRYPT_CREATE 1\n" )
			else()
				file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define VERACRYPT_CREATE 0\n" )
			endif()
		else()

			file( WRITE  ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TCPLAY_NEW_API 0\n" )
			file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TRUECRYPT_CREATE 0\n" )
			file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define VERACRYPT_CREATE 0\n" )

		endif()
	endif()
else()
	file( WRITE  ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TCPLAY_NEW_API 0\n" )
	file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define TRUECRYPT_CREATE 0\n" )
	file( APPEND ${PROJECT_BINARY_DIR}/truecrypt_support_1.h "\n#define VERACRYPT_CREATE 0\n" )
endif()

if( STATIC_TCPLAY )

	set( BUILD_TCPLAY "true" )
	set( TCPLAY_NEW_API "true" )
	ADD_SUBDIRECTORY( external_libraries/tc-play )
	add_definitions( -I${PROJECT_SOURCE_DIR}/external_libraries/tc-play )
	message ( STATUS "---------------------------------------------------------------------------" )
	message ( STATUS "version of cryptsetup is atleast 1.6.0 but version of tcplay is less than 2.1.0 or not found." )
	message ( STATUS "will add tcplay functionality using internal version" )
	message ( STATUS "---------------------------------------------------------------------------" )
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
ADD_SUBDIRECTORY( zuluCrypt-gui/lxqt_wallet )
ADD_SUBDIRECTORY( zuluSafe )

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
		include_directories( ${Qt5Widgets_INCLUDE_DIRS} )

		add_definitions( ${Qt5Widgets_DEFINITIONS} )
	else()
		find_package( Qt4 REQUIRED QtCore QtGui QtNetwork )

		add_definitions( -I${Qt4_INCLUDE_DIR} )
		add_definitions( -I${QT_HEADERS_DIR} )

		INCLUDE( ${QT_USE_FILE} )
		INCLUDE( ${QT_USE_FILE} )
	endif()

	ADD_SUBDIRECTORY( zuluCrypt-gui/md5 )
	ADD_SUBDIRECTORY( zuluCrypt-gui/sharedObjects )
	ADD_SUBDIRECTORY( zuluCrypt-gui )
	ADD_SUBDIRECTORY( plugins )
	ADD_SUBDIRECTORY( zuluMount-gui )

endif( NOT NOGUI )

file( WRITE ${PROJECT_BINARY_DIR}/locale_path.h "\n#define TRANSLATION_PATH \"${CMAKE_INSTALL_PREFIX}/share/zuluCrypt/translations/\"\n")

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

if( TCPLAY_NEW_API )
	message( STATUS "version of tcplay is greater or equal to 2.0.0,support for management of truecrypt header backup will be enabled" )
else()
	message( STATUS "version of tcplay is less than 2.0.0,support for management of truecrypt header backup will be disabled" )
endif()

if( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
	message( STATUS "version of cryptsetup is atleast 1.6.0,support for opening of truecrypt compatible containers is enabled" )
endif( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )

if( CRYPTSETUP_VERSION VERSION_GREATER "1.6.3" )
	file( WRITE ${PROJECT_BINARY_DIR}/support_whirlpool.h "\n#define SUPPORT_WHIRLPOOL 1\n" )
else()
	file( WRITE ${PROJECT_BINARY_DIR}/support_whirlpool.h "\n#define SUPPORT_WHIRLPOOL 0\n" )
endif()

if( CRYPTSETUP_VERSION VERSION_GREATER "1.6.6" )
	file( WRITE ${PROJECT_BINARY_DIR}/veracrypt_support.h "\n#define VERACRYPT_SUPPORT 1\n" )
	message( STATUS "version of cryptsetup is greater than 1.6.6,support for opening of veracrypt containers is enabled" )
	if( TCPLAY_VERSION VERSION_GREATER "2.0.0" )
		message( STATUS "version of tcplay is atleast 2.1.0,support for creation of veracrypt volumes is enabled." )
	endif()
else()
	message( STATUS "version of cryptsetup is less than 1.6.7,support for opening of veracrypt containers is disabled" )
	file( WRITE ${PROJECT_BINARY_DIR}/veracrypt_support.h "\n#define VERACRYPT_SUPPORT 0\n" )
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

if( NOT BUILD_TCPLAY )
	#message( STATUS "cryptsetup version is too old(<1.6.0)" )
	#message( STATUS "You wont be able to create truecrypt compatible encrypted containers" )
	#message( STATUS "tcplay project is hosted at: https://github.com/bwalex/tc-play\n" )
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
	file( WRITE ${PROJECT_BINARY_DIR}/truecrypt_support.h "\n#define TRUECRYPT_CRYPTSETUP 1\n#define TRUECRYPT_TCPLAY 0\n" )
else( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )
	file( WRITE ${PROJECT_BINARY_DIR}/truecrypt_support.h "\n#define TRUECRYPT_CRYPTSETUP 0\n#define TRUECRYPT_TCPLAY 0\n" )
endif( CRYPTSETUP_VERSION VERSION_GREATER "1.5.1" )

if( CRYPTSETUP_VERSION VERSION_GREATER "1.4.0" )
	file( WRITE ${PROJECT_BINARY_DIR}/luks_external_header.h "\n#define LUKS_EXTERNAL_HEADER 1\n" )
else( CRYPTSETUP_VERSION VERSION_GREATER "1.4.0" )
	file( WRITE ${PROJECT_BINARY_DIR}/truecrypt_support.h "\n#define LUKS_EXTERNAL_HEADER 0\n" )
endif( CRYPTSETUP_VERSION VERSION_GREATER "1.4.0" )

if( UDEVSUPPORT )

	file( WRITE ${PROJECT_BINARY_DIR}/udev_support.h "\n#define UDEV_SUPPORT 1\n")

	message ( STATUS "udev will be consulted when deciding if a volume is system or not." )
	message ( STATUS "please read \"udev_support\" file for more information about udev support and associated problems.\n\n" )

else( UDEVSUPPORT )

	file( WRITE ${PROJECT_BINARY_DIR}/udev_support.h "\n#define UDEV_SUPPORT 0\n")

	message ( STATUS "udev will NOT be consulted when deciding if a volume is system or not." )
	message ( STATUS "please read \"udev_support\" file for more information about udev support and how to enable it." )
	message ( STATUS "It is probably a good idea to enable it if you are creating a package for distribution.\n\n" )

endif( UDEVSUPPORT )

install ( FILES zuluCrypt-cli.1 DESTINATION share/man/man1 )
install ( FILES zuluCrypt-gui.1 DESTINATION share/man/man1 )
install ( FILES zuluMount-cli.1 DESTINATION share/man/man1 )
install ( FILES zuluMount-gui.1 DESTINATION share/man/man1 )
install ( FILES zuluSafe-cli.1  DESTINATION share/man/man1 )
install ( FILES docs/zuluCrypt.pdf  DESTINATION share/zuluCrypt )

file( WRITE ${PROJECT_BINARY_DIR}/pdf_path.h "\n#define PDF_PATH \" ${CMAKE_INSTALL_PREFIX}/share/zuluCrypt/zuluCrypt.pdf\"\n" )

#install ( FILES translations/zuluMount/de_DE.qm DESTINATION share/zuluCrypt/translations/zuluMount-gui )
#install ( FILES translations/zuluCrypt/de_DE.qm DESTINATION share/zuluCrypt/translations/zuluCrypt-gui )

