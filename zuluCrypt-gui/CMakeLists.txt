cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/zuluCrypt-gui/ )

set( UI_FILES createfile.ui
		createvolume.ui
		luksdeletekey.ui
		password.ui
		zulucrypt.ui
		createkeyfile.ui
		luksaddkey.ui
		createvolume.ui
		cryptoinfo.ui
		managevolumeheader.ui
		cryptfiles.ui
		createvolumedialog.ui
		managesystemvolumes.ui
		favorites.ui
		erasedevice.ui
		walletconfig.ui
		walletconfiginput.ui
		openvolume.ui
		tcrypt.ui
)

set( MOC_FILES
		tcrypt.h
		createfile.h
		luksaddkey.h
		createkeyfile.h
		keyfiletask.h
		luksdeletekey.h
		createvolume.h
		openvolume.h
		password_dialog.h
		zulucrypt.h
		task.h
		favorites.h
		filetask.h
		cryptoinfo.h
		erasedevice.h
		erasetask.h
		managevolumeheader.h
		cryptfiles.h
		crypttask.h
		createvolumedialog.h
		managesystemvolumes.h
		walletconfig.h
		walletconfiginput.h
		../zuluMount-gui/oneinstance.h
 )

set( SRC
	tcrypt.cpp
	createfile.cpp
	createvolume.cpp
	luksdeletekey.cpp
	openvolume.cpp
	createkeyfile.cpp
	keyfiletask.cpp
	luksaddkey.cpp
	main.cpp
	password_dialog.cpp
	zulucrypt.cpp
	task.cpp
	favorites.cpp
	filetask.cpp
	cryptoinfo.cpp
	erasedevice.cpp
	erasetask.cpp
	managevolumeheader.cpp
	cryptfiles.cpp
	crypttask.cpp
	createvolumedialog.cpp
	managesystemvolumes.cpp
	keystrength.cpp
	walletconfig.cpp
	walletconfiginput.cpp
	../zuluMount-gui/oneinstance.cpp
)

if( QT5 )
	Qt5_WRAP_UI( UI ${UI_FILES} )
	Qt5_WRAP_CPP( MOC ${MOC_FILES} )
	Qt5_ADD_RESOURCES( TRAY_RC_SRCS icon.qrc )
else()
	INCLUDE( ${QT_USE_FILE} )
	Qt4_WRAP_UI( UI ${UI_FILES} )
	Qt4_WRAP_CPP( MOC ${MOC_FILES} )
	Qt4_ADD_RESOURCES( TRAY_RC_SRCS icon.qrc )
endif()

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_executable( zuluCrypt-gui ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS}  )

target_link_libraries( zuluCrypt-gui sharedObject zuluCryptPluginManager -lQtNetwork zuluCryptMD5 )

set_target_properties( zuluCrypt-gui PROPERTIES INSTALL_RPATH "${LIB_PATH}" )

set_target_properties( zuluCrypt-gui PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic" )

if( QT5 )
	if( library_pwquality )
		TARGET_LINK_LIBRARIES( zuluCrypt-gui lxqtwallet ${Qt5Widgets_LIBRARIES} ${QtCore_LIBRARIES} ${blkid} ${library_pwquality} )
	else( library_pwquality )
		TARGET_LINK_LIBRARIES( zuluCrypt-gui lxqtwallet ${Qt5Widgets_LIBRARIES} ${QtCore_LIBRARIES} ${blkid} ${kwallet_library} )
	endif( library_pwquality )
else()
	if( library_pwquality )
		TARGET_LINK_LIBRARIES( zuluCrypt-gui lxqtwallet ${QT_LIBRARIES} ${blkid} ${library_pwquality} )
	else( library_pwquality )
		TARGET_LINK_LIBRARIES( zuluCrypt-gui lxqtwallet ${QT_LIBRARIES} ${blkid} ${kwallet_library} )
	endif( library_pwquality )
endif()

if( library_pwquality )
	TARGET_LINK_LIBRARIES( zuluCrypt-gui lxqtwallet ${QT_LIBRARIES} ${blkid} ${library_pwquality} )
else( library_pwquality )
	TARGET_LINK_LIBRARIES( zuluCrypt-gui lxqtwallet ${QT_LIBRARIES} ${blkid} ${kwallet_library} )
endif( library_pwquality )

install( TARGETS zuluCrypt-gui RUNTIME DESTINATION bin )

install( FILES ${PROJECT_BINARY_DIR}/zuluCrypt.desktop
DESTINATION share/applications

PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install( FILES zuluCrypt.png DESTINATION /usr/share/icons/hicolor/48x48/apps/ )
install( FILES zuluCrypt.png DESTINATION /usr/share/pixmaps )
#install (FILES zuluCrypt.png DESTINATION share/icons/hicolor/32x32/apps)

# desktop file section
file( WRITE ${PROJECT_BINARY_DIR}/zuluCrypt.desktop

"[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_PREFIX}/bin/zuluCrypt-gui -d %U
GenericName[en_US]=Encrypted volumes manager
GenericName=Encrypted volumes manager
Icon=zuluCrypt.png
MimeType=
Name[en_US]=ZuluCrypt
Name=ZuluCrypt
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
