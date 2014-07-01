cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/zuluMount-gui/ -I${PROJECT_BINARY_DIR})

set (UI_FILES 	mainwindow.ui
		keydialog.ui
		mountpartition.ui
		deviceoffset.ui
		mountoptions.ui
		../zuluCrypt-gui/tcrypt.ui
)

set(MOC_FILES 	mainwindow.h
		keydialog.h
		mountpartition.h
		oneinstance.h
		events.h
		monitor_mountinfo.h
		deviceoffset.h
		mountoptions.h
		../zuluCrypt-gui/tcrypt.h
 )

set(SRC 	main.cpp
		events.cpp
		mainwindow.cpp
		keydialog.cpp
		../zuluCrypt-gui/task.cpp
		zulumounttask.cpp
		mountpartition.cpp
		oneinstance.cpp
		monitor_mountinfo.cpp
		deviceoffset.cpp
		mountoptions.cpp
		volumeentryproperties.cpp
		../zuluCrypt-gui/tcrypt.cpp

)

if( QT5 )
	find_package( Qt5Widgets REQUIRED )
	find_package( Qt5Core REQUIRED )

	set( CMAKE_INCLUDE_CURRENT_DIR ON )
	include_directories( ${Qt5Widgets_INCLUDE_DIRS} )

	add_definitions( ${Qt5Widgets_DEFINITIONS} )
	QT5_WRAP_UI( UI ${UI_FILES} )
	QT5_WRAP_CPP(MOC ${MOC_FILES} )
	QT5_ADD_RESOURCES( TRAY_RC_SRCS icon.qrc )

	INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

	add_executable( zuluMount-gui ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS} )

	TARGET_LINK_LIBRARIES( zuluMount-gui sharedObject lxqtwallet  ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${blkid} zuluCryptPluginManager -lQt5Network )

	set_target_properties( zuluMount-gui PROPERTIES INSTALL_RPATH "${LIB_PATH}" )
else()
	find_package( Qt4 REQUIRED )

	add_definitions( -I${Qt4_INCLUDE_DIR} )
	add_definitions( -I${QT_HEADERS_DIR} )

	INCLUDE( ${QT_USE_FILE} )
	INCLUDE( ${QT_USE_FILE} )

	INCLUDE( ${QT_USE_FILE} )
	QT4_WRAP_UI( UI ${UI_FILES} )
	QT4_WRAP_CPP(MOC ${MOC_FILES} )
	QT4_ADD_RESOURCES( TRAY_RC_SRCS icon.qrc )

	INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

	add_executable( zuluMount-gui ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS} )

	TARGET_LINK_LIBRARIES( zuluMount-gui sharedObject lxqtwallet ${QT_LIBRARIES} ${blkid} zuluCryptPluginManager -lQtNetwork )

	set_target_properties( zuluMount-gui PROPERTIES INSTALL_RPATH "${LIB_PATH}" )
endif()

set_target_properties( zuluMount-gui PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic " )

install( TARGETS zuluMount-gui RUNTIME DESTINATION bin )

install ( FILES ${PROJECT_BINARY_DIR}/zuluMount.desktop
DESTINATION share/applications

PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install( FILES zuluMount.png DESTINATION /usr/share/icons/hicolor/48x48/apps/ )
install( FILES zuluMount.png DESTINATION /usr/share/pixmaps )

#install ( FILES zuluMount.png DESTINATION share/icons/hicolor/32x32/apps )

# desktop file section
file( WRITE ${PROJECT_BINARY_DIR}/zuluMount.desktop

"[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_PREFIX}/bin/zuluMount-gui -d %U
GenericName[en_US]=Encrypted volume and Partition mounter
GenericName=Encrypted volumes and partition mounter
Icon=zuluMount.png
MimeType=
Name[en_US]=ZuluMount
Name=ZuluMount
NoDisplay=false
Path=
StartupNotify=true
Terminal=false
Type=Application
X-DBUS-ServiceName=
X-DBUS-StartupType=
X-KDE-SubstituteUID=false
X-KDE-Username=
Categories=Security;Utility;Qt;X-MandrivaLinux-System-FileTools;\n")
