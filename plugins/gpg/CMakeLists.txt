cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/plugins/gpg -I${PROJECT_BINARY_DIR} )

if( QT5 )
	QT5_WRAP_UI( UI ../utility/mainwindow.ui )
	QT5_WRAP_CPP( MOC ../utility/mainwindow.h ../utility/getKey.h )
	QT5_ADD_RESOURCES( ICON ../utility/icon.qrc )

	add_executable( gpg ${ICON} ${MOC} ${UI} main.cpp ../utility/mainwindow.cpp ../utility/getKey.cpp )
	set_target_properties( gpg PROPERTIES INSTALL_RPATH "${LIB_PATH}" )
	set_target_properties( gpg PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )
	TARGET_LINK_LIBRARIES( gpg sharedObject ${Qt5Widgets_LIBRARIES} ${QtCore_LIBRARIES} zuluCryptPluginManager ${blkid} )
else()
	INCLUDE( ${QT_USE_FILE} )

	QT4_WRAP_UI( UI ../utility/mainwindow.ui )
	QT4_WRAP_CPP( MOC ../utility/mainwindow.h ../utility/getKey.h )
	QT4_ADD_RESOURCES( ICON ../utility/icon.qrc )

	add_executable( gpg ${ICON} ${MOC} ${UI} main.cpp ../utility/mainwindow.cpp ../utility/getKey.cpp )
	set_target_properties( gpg PROPERTIES INSTALL_RPATH "${LIB_PATH}" )
	set_target_properties( gpg PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )
	TARGET_LINK_LIBRARIES( gpg sharedObject ${QT_LIBRARIES} zuluCryptPluginManager ${blkid} )
endif()

install( TARGETS gpg RUNTIME DESTINATION "${PLUGINPATH}" )
