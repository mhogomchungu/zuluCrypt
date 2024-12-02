if(${CMAKE_VERSION} VERSION_GREATER "3.26")
    cmake_minimum_required(VERSION 3.26.0)
else()
    cmake_minimum_required(VERSION 3.0.2)
endif()

project( tasks )

set( LIB_VERSION "1.2.3" )
set( SO_VERSION "1.0.0" )

add_definitions( -Wextra -Wall -pedantic )

if( CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.0.0)
	set( CMAKE_CXX_STANDARD 14 )
else()
	set( CMAKE_CXX_STANDARD 14 )
endif()

set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)

INCLUDE( GNUInstallDirs )

INCLUDE( CMakeDependentOption )

if(BUILD_WITH_QT6)

	find_package( Qt6Core REQUIRED )

	include_directories( ${Qt6Core_INCLUDE_DIRS} )

	if( MCHUNGU_TASK_INSTALL )

		add_library( mhogomchungu_task SHARED task.hpp )
		set_target_properties(mhogomchungu_task PROPERTIES SOVERSION "${SO_VERSION}")
	else()
		add_library( mhogomchungu_task STATIC task.hpp )
	endif()

	set_target_properties( mhogomchungu_task PROPERTIES LINKER_LANGUAGE CXX )

	set_target_properties( mhogomchungu_task PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic " )

	target_link_libraries( mhogomchungu_task ${Qt6Core_LIBRARIES} )

	if( DEBUG )
		add_executable( example example.cpp main.cpp example.h )
		target_link_libraries( example mhogomchungu_task )
	endif()
else()
	find_package( Qt5Core REQUIRED )

	QT5_WRAP_CPP( MOC_LIBRARY task.hpp )

	include_directories( ${Qt5Core_INCLUDE_DIRS} )

	if( MCHUNGU_TASK_INSTALL )

		add_library( mhogomchungu_task SHARED ${MOC_LIBRARY} )
		set_target_properties(mhogomchungu_task PROPERTIES SOVERSION "${SO_VERSION}")
	else()
		add_library( mhogomchungu_task STATIC ${MOC_LIBRARY} )
	endif()

	set_target_properties( mhogomchungu_task PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic " )

	target_link_libraries( mhogomchungu_task ${Qt5Core_LIBRARIES} )

	if( DEBUG )
		QT5_WRAP_CPP( MOC_EXE example.h )
		add_executable( example example.cpp main.cpp ${MOC_EXE} )
		target_link_libraries( example mhogomchungu_task )
	endif()
endif()

file( WRITE ${PROJECT_BINARY_DIR}/mhogomchungu_task.pc
"prefix=${CMAKE_INSTALL_PREFIX}
libdir=${CMAKE_INSTALL_FULL_LIBDIR}
includedir=${CMAKE_INSTALL_FULL_INCLUDEDIR}/mhogomchungu

Name: mhogomchungu_task
Description: A Qt5 library that offers task based programming using modern C++
Version: ${LIB_VERSION}
Libs: -L${CMAKE_INSTALL_FULL_LIBDIR} -l${Qt5Core_LIBRARIES}
Cflags: -I${CMAKE_INSTALL_FULL_INCLUDEDIR}/mhogomchungu
\n")

if( MCHUNGU_TASK_INSTALL )
	install( FILES ${PROJECT_BINARY_DIR}/mhogomchungu_task.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig/ PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
	install( FILES task.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mhogomchungu )
	install( TARGETS mhogomchungu_task LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )


	# uninstall target
	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
		IMMEDIATE @ONLY )

	add_custom_target( uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )
endif()
