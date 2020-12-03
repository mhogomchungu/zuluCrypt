cmake_minimum_required(VERSION 3.0.2)

if( NOT NOKDE )
	#add_subdirectory( kwallet )
endif()

add_subdirectory( keydialog-qt )

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++14 -I${PROJECT_BINARY_DIR}/plugins -I${PROJECT_BINARY_DIR} )

if( QT5 )
	QT5_WRAP_UI( UI mainwindow.ui )
	QT5_WRAP_CPP( MOC mainwindow.h )

	add_library( plugin STATIC ${MOC} ${UI} mainwindow.cpp )

	set_target_properties( plugin PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )

	TARGET_LINK_LIBRARIES( plugin sharedObject ${Qt5Widgets_LIBRARIES} ${QtCore_LIBRARIES} zuluCryptPluginManager ${blkid} )
else()
	INCLUDE( ${QT_USE_FILE} )

	QT4_WRAP_UI( UI mainwindow.ui )
	QT4_WRAP_CPP( MOC mainwindow.h )

	add_library( plugin STATIC ${MOC} ${UI} mainwindow.cpp )

	set_target_properties( plugin PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )

	TARGET_LINK_LIBRARIES( plugin sharedObject ${QT_LIBRARIES} zuluCryptPluginManager ${blkid} )
endif()

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
