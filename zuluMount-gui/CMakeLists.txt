if(${CMAKE_VERSION} VERSION_GREATER "3.26")
    cmake_minimum_required(VERSION 3.5.0)
else()
    cmake_minimum_required(VERSION 3.0.2)
endif()

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -I${PROJECT_BINARY_DIR}/zuluMount-gui/ -I${PROJECT_BINARY_DIR})

include_directories( ${PROJECT_BINARY_DIR}/zuluMount-gui/ )


set (UI_FILES 	zulumount.ui
		keydialog.ui
		mountpartition.ui
		veracryptpimdialog.ui
		../zuluCrypt-gui/tcrypt.ui
		../zuluCrypt-gui/favorites2.ui
		../zuluCrypt-gui/openvolume.ui
)

set(MOC_FILES 	zulumount.h
		keydialog.h
		mountpartition.h
		oneinstance.h
		events.h
		monitor_mountinfo.h
		veracryptpimdialog.h
		../zuluCrypt-gui/tcrypt.h
		../zuluCrypt-gui/utility.h
		../zuluCrypt-gui/favorites2.h
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
		siritask.cpp
		volumeproperty.cpp
		../zuluCrypt-gui/tcrypt.cpp
		veracryptpimdialog.cpp
		../zuluCrypt-gui/favorites2.cpp
		../zuluCrypt-gui/openvolume.cpp
		../zuluCrypt-gui/secrets.cpp
)

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

set_target_properties( zuluMount-gui PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic " )

install( TARGETS zuluMount-gui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )

install ( FILES ${PROJECT_BINARY_DIR}/zuluMount.desktop
DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
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
MimeType=application/x-raw-disk-image;
Categories=Security;Utility;Qt;X-MandrivaLinux-System-FileTools;\n")
