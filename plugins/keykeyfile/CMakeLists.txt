cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/plugins/keykeyfile -I${PROJECT_BINARY_DIR} )

if( QT5 )
	QT5_WRAP_UI( UI mainwindow.ui )
	QT5_WRAP_CPP( MOC mainwindow.h )
	QT5_ADD_RESOURCES( ICON icon.qrc )

	add_executable( keykeyfile ${ICON} ${MOC} ${UI} main.cpp mainwindow.cpp )
	TARGET_LINK_LIBRARIES( keykeyfile sharedObject ${Qt5Widgets_LIBRARIES} ${QtCore_LIBRARIES} zuluCryptPluginManager ${blkid} )
	set_target_properties( keykeyfile PROPERTIES INSTALL_RPATH "${LIB_PATH}" )
	set_target_properties( keykeyfile PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )
else()
	INCLUDE( ${QT_USE_FILE} )

	QT4_WRAP_UI( UI mainwindow.ui )
	QT4_WRAP_CPP( MOC mainwindow.h )
	QT4_ADD_RESOURCES( ICON icon.qrc )

	add_executable( keykeyfile ${ICON} ${MOC} ${UI} main.cpp mainwindow.cpp )
	TARGET_LINK_LIBRARIES( keykeyfile sharedObject ${QT_LIBRARIES} zuluCryptPluginManager ${blkid} )
	set_target_properties( keykeyfile PROPERTIES INSTALL_RPATH "${LIB_PATH}" )
	set_target_properties( keykeyfile PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )
endif()



install( TARGETS keykeyfile RUNTIME DESTINATION "${PLUGINPATH}" )
