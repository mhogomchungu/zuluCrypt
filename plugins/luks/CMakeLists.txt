if(${CMAKE_VERSION} VERSION_GREATER "3.26")
    cmake_minimum_required(VERSION 3.5.0)
else()
    cmake_minimum_required(VERSION 3.0.2)
endif()

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/plugins/luks -I${PROJECT_BINARY_DIR} )

if(BUILD_WITH_QT6)
	add_executable( luks ../icon.qrc main.cpp )
else()
	QT5_ADD_RESOURCES( ICON ../icon.qrc )
	add_executable( luks ${ICON} main.cpp )
endif()

set_target_properties( luks PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )
set_target_properties( luks PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )

TARGET_LINK_LIBRARIES( luks plugin )

install( TARGETS luks RUNTIME DESTINATION "${PLUGINPATH}" )
