cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/plugins/luks -I${PROJECT_BINARY_DIR} )

if( QT5 )
	QT5_ADD_RESOURCES( ICON ../icon.qrc )
	add_executable( network_key ${ICON} main.cpp )
else()
	QT4_ADD_RESOURCES( ICON ../icon.qrc )
	add_executable( network_key ${ICON} main.cpp )
endif()

set_target_properties( network_key PROPERTIES INSTALL_RPATH "${LIB_PATH}" )
set_target_properties( network_key PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )

TARGET_LINK_LIBRARIES( network_key crypt_buffer plugin )


#currently not used

install( TARGETS network_key RUNTIME DESTINATION "${PLUGINPATH}" )