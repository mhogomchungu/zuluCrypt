cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)


add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -I${PROJECT_BINARY_DIR}/zuluCrypt-gui/ ) 

add_library( MD5 STATIC md5.c )
set_target_properties( MD5 PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -pedantic " ) 
