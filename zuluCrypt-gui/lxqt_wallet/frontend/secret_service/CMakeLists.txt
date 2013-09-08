
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

include_directories( ${LIBSECRET_INCLUDE_DIRS} )
set( SECRETSERVICE_LIBRARY "secretService" )
add_library( secretService STATIC ../secret_service.c )
target_link_libraries( secretService ${LIBSECRET_LIBRARIES} )
set_target_properties( secretService PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic " )
