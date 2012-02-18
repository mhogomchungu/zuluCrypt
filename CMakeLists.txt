
cmake_minimum_required(VERSION 2.6)

set( CMAKE_SHARED_LINKER_FLAGS "-s" )
set( CMAKE_EXE_LINKER_FLAGS "-s" )

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

ADD_SUBDIRECTORY( zuluCrypt-gui )

ADD_SUBDIRECTORY( test )
	 
install (FILES zuluCrypt-cli.1.bz2 
DESTINATION share/man/man1
)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)