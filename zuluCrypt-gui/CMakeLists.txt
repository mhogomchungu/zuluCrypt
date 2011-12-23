cmake_minimum_required(VERSION 2.6)


set (UI_FILES createfile.ui
		createpartition.ui
		luksdeletekey.ui
		password.ui
		zulucrypt.ui
		createkeyfile.ui
		luksaddkey.ui
		openpartition.ui
)

set( MOC_FILES createfile.h
		luksaddkey.h
		createkeyfile.h
		luksdeletekey.h 
		createpartition.h
		openpartition.h
		password_dialog.h
		zulucrypt.h
		startupupdateopenedvolumes.h
		closeallvolumesthread.h
		additemtotablethread.h
		runinthread.h
 )
	
set (SRC createfile.cpp
	createpartition.cpp
	luksdeletekey.cpp
	openpartition.cpp
	zulucryptthreads.cpp
	createkeyfile.cpp
	luksaddkey.cpp
	main.cpp
	password_dialog.cpp
	startupupdateopenedvolumes.cpp
	zulucrypt.cpp
	closeallvolumesthread.cpp
	additemtotablethread.cpp
	runinthread.cpp
)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall ) 

find_package( Qt4 REQUIRED )

INCLUDE( ${QT_USE_FILE} )

QT4_WRAP_UI( UI ${UI_FILES})

QT4_WRAP_CPP(MOC ${MOC_FILES})

QT4_ADD_RESOURCES( TRAY_RC_SRCS icon.qrc)

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_executable( zuluCrypt-gui ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS})			

TARGET_LINK_LIBRARIES( zuluCrypt-gui ${QT_LIBRARIES} ${blkid})

install( TARGETS zuluCrypt-gui RUNTIME DESTINATION bin )

install (FILES zuluCrypt.desktop 
DESTINATION share/applications

#PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
)

install (FILES zuluCrypt.png DESTINATION share/icons)


