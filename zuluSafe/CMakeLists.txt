
 cmake_minimum_required(VERSION 2.6)

 include_directories( {PROJECT_BINARY_DIR} )

 add_executable( zuluSafe-cli zuluSafe.c )

 find_file( GCRYPT_INCLUDE_FILE gcrypt.h )

 find_library( GCRYPT_LIBRARY gcrypt )

 if( NOT GCRYPT_INCLUDE_FILE )
	MESSAGE( FATAL_ERROR "could not find gcrypt header file" )
 else()
	MESSAGE( STATUS "found gcrypt header file: ${GCRYPT_INCLUDE_FILE}" )
 endif()

 set_target_properties( zuluSafe-cli PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
 set_target_properties( zuluSafe-cli PROPERTIES LINK_FLAGS "-pie" )
 TARGET_LINK_LIBRARIES( zuluSafe-cli lxqt-wallet "${GCRYPT_LIBRARY}" )

 install(TARGETS zuluSafe-cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
