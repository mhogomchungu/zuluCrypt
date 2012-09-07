cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++0x -I${PROJECT_BINARY_DIR}/zuluMount-gui/ -I${PROJECT_BINARY_DIR}) 

set (UI_FILES 	../zuluCrypt-gui/dialogmsg.ui
		mainwindow.ui
		keydialog.ui
		mountpartition.ui
)

set(MOC_FILES 	mainwindow.h
		keydialog.h
		managepartitionthread.h
		mountpartition.h
		../zuluCrypt-gui/dialogmsg.h
		../plugins/kwallet/zulusocket.h		
 )
	
set (SRC 	main.cpp
		mainwindow.cpp
		keydialog.cpp
		managepartitionthread.cpp
		mountpartition.cpp
		../zuluCrypt-gui/dialogmsg.cpp
		../zuluCrypt-gui/userfont.cpp
		../zuluCrypt-gui/tablewidget.cpp
		../plugins/kwallet/zulusocket.cpp		
)

find_package( Qt4 REQUIRED )

INCLUDE( ${QT_USE_FILE} )

QT4_WRAP_UI( UI ${UI_FILES})

QT4_WRAP_CPP(MOC ${MOC_FILES})

QT4_ADD_RESOURCES( TRAY_RC_SRCS icon.qrc)

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_executable( zuluMount-gui ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS} )			

TARGET_LINK_LIBRARIES( zuluMount-gui ${QT_LIBRARIES} ${blkid} -lQtNetwork )

install( TARGETS zuluMount-gui RUNTIME DESTINATION bin )

install (FILES zuluMount.desktop 
DESTINATION share/applications

PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
)

install (FILES zuluMount.png DESTINATION share/icons)

# desktop file section
file(WRITE zuluMount.desktop

"#!/usr/bin/env xdg-open
[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_PREFIX}/bin/zuluMount-gui
GenericName[en_US]=partition mounter
GenericName=partition mounter
Icon=${CMAKE_INSTALL_PREFIX}/share/icons/zuluMount.png
MimeType=
Name[en_US]=zuluMount
Name=zuluMount
NoDisplay=false
Path=
StartupNotify=true
Terminal=false
TerminalOptions=
Type=Application
X-DBUS-ServiceName=
X-DBUS-StartupType=
X-KDE-SubstituteUID=false
X-KDE-Username=
Categories=Security;Utility;Qt;\n")
