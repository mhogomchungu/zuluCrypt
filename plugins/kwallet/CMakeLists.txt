cmake_minimum_required(VERSION 2.6)

if( QT5 )
	#QT5_WRAP_CPP( MOC wallet.h )
else()
	find_package(KDE4 QUIET)
	if(KDE4_FOUND)
		include(KDE4Defaults)
		include_directories(${KDE4_INCLUDES})
		add_definitions(-I${KDE4_INCLUDE_DIR})
		add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++0x -I${PROJECT_BINARY_DIR}/plugins/kwallet/ -I${PROJECT_BINARY_DIR} -I${KDEINCLUDE} )

		INCLUDE( ${QT_USE_FILE} )
		QT4_WRAP_CPP( MOC wallet.h )

		find_library( LIBKWALLETBACKEND libkwalletbackend.so )

		add_executable( kwallet ${MOC} main.cpp wallet.cpp )

		TARGET_LINK_LIBRARIES( kwallet -lQtCore -lQtGui -lkdeui -lkdecore sharedObject ${LIBKWALLETBACKEND} zuluCryptPluginManager )
		set_target_properties( kwallet PROPERTIES INSTALL_RPATH "${LIB_PATH}" )
		set_target_properties( kwallet PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )

		install( TARGETS kwallet RUNTIME DESTINATION "${PLUGINPATH}" )
	endif()
endif()


