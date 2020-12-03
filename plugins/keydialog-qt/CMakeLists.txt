cmake_minimum_required(VERSION 3.0.2)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/plugins/keydialog-qt -I${PROJECT_BINARY_DIR} )

if( QT5 )
	QT5_WRAP_UI( UI mainwindow.ui )
	QT5_WRAP_CPP( MOC mainwindow.h )
	QT5_ADD_RESOURCES( ICON icon.qrc )

	add_executable( keydialog-qt ${ICON} ${MOC} ${UI} main.cpp mainwindow.cpp )
	TARGET_LINK_LIBRARIES( keydialog-qt sharedObject ${Qt5Widgets_LIBRARIES} ${QtCore_LIBRARIES} zuluCryptPluginManager ${blkid} )
	set_target_properties( keydialog-qt PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )
	set_target_properties( keydialog-qt PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )
else()
	INCLUDE( ${QT_USE_FILE} )
	QT4_WRAP_UI( UI mainwindow.ui )
	QT4_WRAP_CPP( MOC mainwindow.h )
	QT4_ADD_RESOURCES( ICON icon.qrc )

	add_executable( keydialog-qt ${ICON} ${MOC} ${UI} main.cpp mainwindow.cpp )
	TARGET_LINK_LIBRARIES( keydialog-qt sharedObject ${QT_LIBRARIES} zuluCryptPluginManager ${blkid} )
	set_target_properties( keydialog-qt PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )
	set_target_properties( keydialog-qt PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )
endif()

install( TARGETS keydialog-qt RUNTIME DESTINATION "${PLUGINPATH}" )
