cmake_minimum_required(VERSION 2.6)

INCLUDE(FindPkgConfig)

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

add_library( lxqt-wallet-backend STATIC lxqtwallet.c )
if( CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0 )
	set_target_properties( lxqt-wallet-backend PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -Wformat-truncation=0 -s -fPIC -pedantic " )
else()
	set_target_properties( lxqt-wallet-backend PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pedantic " )
endif()
set_target_properties( lxqt-wallet-backend PROPERTIES LINK_FLAGS "-pie" )
TARGET_LINK_LIBRARIES( lxqt-wallet-backend "${GCRYPT_LIBRARY}" )
