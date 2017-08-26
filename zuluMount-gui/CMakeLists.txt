cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -std=c++14 -I${PROJECT_BINARY_DIR}/zuluMount-gui/ -I${PROJECT_BINARY_DIR})

include_directories( ${PROJECT_BINARY_DIR}/zuluMount-gui/ )


set (UI_FILES 	zulumount.ui
		keydialog.ui
		mountpartition.ui
		deviceoffset.ui
		mountoptions.ui
		veracryptpimdialog.ui
		../zuluCrypt-gui/tcrypt.ui
		../zuluCrypt-gui/favorites.ui
		../zuluCrypt-gui/openvolume.ui
)

set(MOC_FILES 	zulumount.h
		keydialog.h
		mountpartition.h
		oneinstance.h
		events.h
		monitor_mountinfo.h
		deviceoffset.h
		mountoptions.h
		veracryptpimdialog.h
		../zuluCrypt-gui/tcrypt.h
		../zuluCrypt-gui/utility.h
		../zuluCrypt-gui/favorites.h
		../zuluCrypt-gui/openvolume.h
 )

set(SRC 	main.cpp
		events.cpp
		zulumount.cpp
		keydialog.cpp
		zulumounttask.cpp
		mountpartition.cpp
		oneinstance.cpp
		monitor_mountinfo.cpp
		deviceoffset.cpp
		mountoptions.cpp
		siritask.cpp
		volumeproperty.cpp
		../zuluCrypt-gui/tcrypt.cpp
		veracryptpimdialog.cpp
		../zuluCrypt-gui/favorites.cpp
		../zuluCrypt-gui/openvolume.cpp
		../zuluCrypt-gui/secrets.cpp
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

	TARGET_LINK_LIBRARIES( zuluMount-gui sharedObject lxqt-wallet  ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES} ${blkid} zuluCryptPluginManager )

	set_target_properties( zuluMount-gui PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )
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

	TARGET_LINK_LIBRARIES( zuluMount-gui sharedObject lxqt-wallet ${QT_LIBRARIES} ${blkid} zuluCryptPluginManager )

	set_target_properties( zuluMount-gui PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )
endif()

set_target_properties( zuluMount-gui PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic " )

install( TARGETS zuluMount-gui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )

install ( FILES ${PROJECT_BINARY_DIR}/zuluMount.desktop
DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications

PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install( FILES zuluMount.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps/ )
install( FILES zuluMount.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons )
install( FILES zuluMount.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps )
install( FILES zuluMount.nicolas.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps/ )
install( FILES zuluMount.nicolas.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps )
install( FILES zuluMount.papirus.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps/ )
install( FILES zuluMount.papirus.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps )
install( FILES zuluMount.papirus.dark.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps/ )
install( FILES zuluMount.papirus.dark.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps )
#install ( FILES zuluMount.png DESTINATION share/icons/hicolor/32x32/apps )

# desktop file section

file( WRITE ${PROJECT_BINARY_DIR}/zuluMount.desktop

"[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_FULL_BINDIR}/zuluMount-gui -d %U
GenericName[en_US]=Encrypted volume and Partition mounter
GenericName=Encrypted volumes and partition mounter
Icon=zuluMount
Name[en_US]=ZuluMount
Name=ZuluMount
NoDisplay=false
StartupNotify=true
Terminal=false
Type=Application
Categories=Security;Utility;Qt;X-MandrivaLinux-System-FileTools;\n")
