cmake_minimum_required( VERSION 3.0 )

project( tasks )

add_definitions( -Wextra -Wall -pedantic )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)

INCLUDE(CMakeDependentOption)

if( QT5 )

	find_package( Qt5Core REQUIRED )
	QT5_WRAP_CPP( MOC_LIBRARY task.hpp )
	include_directories( ${Qt5Core_INCLUDE_DIRS} )

	add_library( tasks STATIC ${MOC_LIBRARY} )

	TARGET_LINK_LIBRARIES( tasks ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} )
else()
	find_package( Qt4 REQUIRED )

	add_definitions( -I${Qt4_INCLUDE_DIR} )
	add_definitions( -I${QT_HEADERS_DIR} )
	INCLUDE( ${QT_USE_FILE} )
	QT4_WRAP_CPP( MOC_LIBRARY task.hpp )

	add_library( tasks STATIC ${MOC_LIBRARY} )

	TARGET_LINK_LIBRARIES( tasks -lQtCore )
endif()

set_target_properties( tasks PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic " )
