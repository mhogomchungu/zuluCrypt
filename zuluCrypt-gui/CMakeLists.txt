cmake_minimum_required(VERSION 2.6)


set (UI_FILES createfile.ui
		createpartition.ui
		luksdeletekey.ui
		password.ui
		zulucrypt.ui
		createkeyfile.ui
		luksaddkey.ui
		openpartition.ui
		rngselector.ui
)

set( MOC_FILES createfile.h
		luksaddkey.h
		rngselector.h
		zulucryptthreads.h
		createkeyfile.h
		luksdeletekey.h 
		createpartition.h
		openpartition.h
		password_dialog.h
		zulucrypt.h
 )
	
set (SRC createfile.cpp
			createpartition.cpp
			luksdeletekey.cpp
			openpartition.cpp
			rngselector.cpp
			zulucryptthreads.cpp
			createkeyfile.cpp
			luksaddkey.cpp
			main.cpp
			password_dialog.cpp
			zulucrypt.cpp
)

add_definitions( -Wall )

find_package( Qt4 REQUIRED )

INCLUDE( ${QT_USE_FILE} )

QT4_WRAP_UI( UI ${UI_FILES})

QT4_WRAP_CPP(MOC ${MOC_FILES})

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_executable( zuluCrypt-gui ${MOC} ${UI} ${SRC})			

TARGET_LINK_LIBRARIES( zuluCrypt-gui ${QT_LIBRARIES} )

install( TARGETS zuluCrypt-gui RUNTIME DESTINATION bin )
