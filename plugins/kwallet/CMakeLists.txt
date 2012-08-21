cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++0x -I${PROJECT_BINARY_DIR}/plugins/kwallet/ -I${PROJECT_BINARY_DIR} -I${KDEINCLUDE} ) 

find_package( Qt4 REQUIRED )

INCLUDE( ${QT_USE_FILE} )

QT4_WRAP_CPP( MOC wallet.h )

add_executable( kwallet ${MOC} main.cpp wallet.cpp )

TARGET_LINK_LIBRARIES( kwallet ${kwallet_library} ) 

set_target_properties( kwallet PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )

install( TARGETS kwallet RUNTIME DESTINATION "/etc/zuluCrypt/modules" )
