if(${CMAKE_VERSION} VERSION_GREATER "3.26")
    cmake_minimum_required(VERSION 3.5.0)
else()
    cmake_minimum_required(VERSION 3.0.2)
endif()

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/plugins/tomb -I${PROJECT_BINARY_DIR} )

if( QT5 )
	QT5_ADD_RESOURCES( ICON ../icon.qrc )
	add_executable( tomb ${ICON} main.cpp )
else()
	QT4_ADD_RESOURCES( ICON ../icon.qrc )
	add_executable( tomb ${ICON} main.cpp )
endif()

set_target_properties( tomb PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )
set_target_properties( tomb PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )

TARGET_LINK_LIBRARIES( tomb plugin )

install( TARGETS tomb RUNTIME DESTINATION "${PLUGINPATH}" )
