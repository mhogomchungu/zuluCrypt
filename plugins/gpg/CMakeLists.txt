if(${CMAKE_VERSION} VERSION_GREATER "3.26")
    cmake_minimum_required(VERSION 3.5.0)
else()
    cmake_minimum_required(VERSION 3.0.2)
endif()

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++17 -I${PROJECT_BINARY_DIR}/plugins/gpg -I${PROJECT_BINARY_DIR} )

if(BUILD_WITH_QT6)
	add_executable( gpg ../icon.qrc main.cpp )
else()
	QT5_ADD_RESOURCES( ICON ../icon.qrc )
	add_executable( gpg ${ICON} main.cpp )
endif()

set_target_properties( gpg PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )
set_target_properties( gpg PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )

TARGET_LINK_LIBRARIES( gpg plugin )

install( TARGETS gpg RUNTIME DESTINATION "${PLUGINPATH}" )
