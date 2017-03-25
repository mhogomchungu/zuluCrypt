cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/zuluPolkit/ )
include_directories( ${PROJECT_BINARY_DIR}/zuluPolkit/ )

set( MOC_FILES
	zulupolkit.h
	../zuluCrypt-gui/task.h
 )

set( SRC
	main.cpp
	zulupolkit.cpp
)

if( QT5 )
	Qt5_WRAP_CPP( MOC ${MOC_FILES} )
else()
	INCLUDE( ${QT_USE_FILE} )
	Qt4_WRAP_CPP( MOC ${MOC_FILES} )
endif()

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_executable( zuluPolkit ${MOC} ${SRC} )

set_target_properties( zuluPolkit PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )

set_target_properties( zuluPolkit PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic" )

if( QT5 )
	target_link_libraries( zuluPolkit ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} )
else()
	target_link_libraries( zuluPolkit ${QT_LIBRARIES} )
endif()

install( TARGETS zuluPolkit RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
