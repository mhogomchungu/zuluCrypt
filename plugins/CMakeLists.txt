if(${CMAKE_VERSION} VERSION_GREATER "3.26")
    cmake_minimum_required(VERSION 3.26.0)
else()
    cmake_minimum_required(VERSION 3.0.2)
endif()

if( NOT NOKDE )
	#add_subdirectory( kwallet )
endif()

add_subdirectory( keydialog-qt )

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -I${PROJECT_BINARY_DIR}/plugins -I${PROJECT_BINARY_DIR} )

if( CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.0.0)
	set( CMAKE_CXX_STANDARD 14 )
	MESSAGE( STATUS "Setting C++ version to C++14" )
else()
	set( CMAKE_CXX_STANDARD 14 )
	MESSAGE( STATUS "Setting C++ version to C++14" )
endif()

set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)

if(BUILD_WITH_QT6)

	add_library( plugin STATIC mainwindow.h mainwindow.ui mainwindow.cpp )

	TARGET_LINK_LIBRARIES( plugin sharedObject ${Qt6Widgets_LIBRARIES} ${Qt6Core_LIBRARIES} )
else()
	QT5_WRAP_UI( UI mainwindow.ui )
	QT5_WRAP_CPP( MOC mainwindow.h )

	add_library( plugin STATIC ${MOC} ${UI} mainwindow.cpp )

	TARGET_LINK_LIBRARIES( plugin sharedObject ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} )
endif()

set_target_properties( plugin PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )

TARGET_LINK_LIBRARIES( plugin sharedObject zuluCryptPluginManager ${blkid} )

add_subdirectory( gpg )
add_subdirectory( keykeyfile )
add_subdirectory( steghide )
#add_subdirectory( luks )
add_subdirectory( tomb )
add_subdirectory( hmac )
#add_subdirectory( generic_header )

#add_subdirectory( network_key )

#keyring module is handled by CMakeLists.txt in ../zuluCrypt-cli
#add_subdirectory( keyring )
