cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

add_library( zuluCryptMD5 STATIC md5.c )
set_target_properties( zuluCryptMD5 PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -pedantic " ) 
set_target_properties( zuluCryptMD5 PROPERTIES LINK_FLAGS "-pie" )
