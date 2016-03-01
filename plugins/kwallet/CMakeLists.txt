cmake_minimum_required(VERSION 2.6)

if( QT5 )
	#QT5_WRAP_CPP( MOC wallet.h )
else()
	find_package(KDE4 QUIET)
	if(KDE4_FOUND)
		find_library( LIBKWALLETBACKEND_ libkwalletbackend.so )
		find_library( LIBQTCORE_ libQtCore.so )
		find_library( LIBQTGUI_ libQtGui.so )
		find_library( LIBKDEUI_ libkdeui.so )
		find_library( LIBKDECORE_ libkdecore.so )

		if( NOT LIBKWALLETBACKEND_ )
			find_library( LIBKWALLETBACKEND_ libkwalletbackend.so.4 )
		endif()

		if( LIBKWALLETBACKEND_ )
			if( LIBQTCORE_ )
				if( LIBQTGUI_ )
					if( LIBKDEUI_ )
						if( LIBKDECORE_ )
							include(KDE4Defaults)
							include_directories(${KDE4_INCLUDES})
							add_definitions(-I${KDE4_INCLUDE_DIR})

							add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/plugins/kwallet/ -I${PROJECT_BINARY_DIR} -I${KDEINCLUDE} )

							INCLUDE( ${QT_USE_FILE} )
							QT4_WRAP_CPP( MOC wallet.h )

							add_executable( kwallet ${MOC} main.cpp wallet.cpp )

							TARGET_LINK_LIBRARIES( kwallet ${LIBQTCORE_} ${LIBQTGUI_} ${LIBKDECORE_} ${LIBKDEUI_} sharedObject ${LIBKWALLETBACKEND_} zuluCryptPluginManager )
							set_target_properties( kwallet PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )
							set_target_properties( kwallet PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )

							install( TARGETS kwallet RUNTIME DESTINATION "${PLUGINPATH}" )
						endif()
					endif()
				endif()
			endif()
		endif()
	endif()
endif()


