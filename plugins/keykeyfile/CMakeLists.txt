cmake_minimum_required(VERSION 3.0.2)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/plugins/keykeyfile -I${PROJECT_BINARY_DIR} )

if( QT5 )
	QT5_ADD_RESOURCES( ICON ../icon.qrc )
	add_executable( keykeyfile ${ICON} main.cpp )
else()
	QT4_ADD_RESOURCES( ICON ../icon.qrc )
	add_executable( keykeyfile ${ICON} main.cpp )
endif()

set_target_properties( keykeyfile PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )
set_target_properties( keykeyfile PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )

TARGET_LINK_LIBRARIES( keykeyfile plugin )

install( TARGETS keykeyfile RUNTIME DESTINATION "${PLUGINPATH}" )
