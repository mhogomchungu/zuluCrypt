cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -std=c++0x -I${PROJECT_BINARY_DIR}/zuluCrypt-gui/ ) 

set (UI_FILES createfile.ui
		createvolume.ui
		luksdeletekey.ui
		password.ui
		zulucrypt.ui
		createkeyfile.ui
		luksaddkey.ui
		createvolume.ui
		cryptoinfo.ui
		manageluksheader.ui
		cryptfiles.ui
		createvolumedialog.ui
		dialogmsg.ui
		managesystemvolumes.ui
		managedevicenames.ui
		erasedevice.ui	
		kwalletconfig.ui
		openvolume.ui
)

set( MOC_FILES createfile.h
		luksaddkey.h
		createkeyfile.h
		createkeyfilethread.h		
		luksdeletekey.h 
		createvolume.h
		openvolume.h
		password_dialog.h
		zulucrypt.h
		startupupdateopenedvolumes.h
		closeallvolumesthread.h
		runinthread.h
		managedevicenames.h
		partitionproperties.h
		volumepropertiesthread.h
		createfilethread.h
		closevolumethread.h
		checkvolumetype.h
		cryptoinfo.h	
		erasedevice.h
		erasedevicethread.h	
		manageluksheader.h
		cryptfiles.h
		cryptfilethread.h
		createvolumedialog.h
		dialogmsg.h
		managesystemvolumes.h
		kwalletconfig.h
		socketsendkey.h
		../zuluMount-gui/oneinstance.h
		openmountpointinfilemanager.h
 )
	
set (SRC createfile.cpp
	createvolume.cpp
	luksdeletekey.cpp
	openvolume.cpp
	createkeyfile.cpp
	createkeyfilethread.cpp	
	luksaddkey.cpp
	main.cpp
	password_dialog.cpp
	startupupdateopenedvolumes.cpp
	zulucrypt.cpp
	closeallvolumesthread.cpp
	runinthread.cpp
	utility.cpp
	managedevicenames.cpp
	partitionproperties.cpp
	volumepropertiesthread.cpp
	createfilethread.cpp
	closevolumethread.cpp
	checkvolumetype.cpp
	cryptoinfo.cpp
	erasedevice.cpp
	erasedevicethread.cpp
	manageluksheader.cpp
	cryptfiles.cpp
	cryptfilethread.cpp
	createvolumedialog.cpp
	dialogmsg.cpp
	managesystemvolumes.cpp
	keystrength.cpp
	userfont.cpp
	kwalletplugin.cpp
	kwalletconfig.cpp
	tablewidget.cpp
	socketsendkey.cpp
	openvolumereadonly.cpp
	zuluoptions.cpp
	openmountpointinfilemanager.cpp
	savemountpointpath.cpp
	../zuluMount-gui/oneinstance.cpp	
)

INCLUDE( ${QT_USE_FILE} )

QT4_WRAP_UI( UI ${UI_FILES} )

QT4_WRAP_CPP( MOC ${MOC_FILES} )

QT4_ADD_RESOURCES( TRAY_RC_SRCS icon.qrc )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_executable( zuluCrypt-gui ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS}  )

target_link_libraries( zuluCrypt-gui zuluCryptPluginManager -lQtNetwork zuluCryptMD5 ) 

if( library_pwquality )
	if( kwallet_library )
		TARGET_LINK_LIBRARIES( zuluCrypt-gui ${QT_LIBRARIES} ${blkid} ${library_pwquality} ${kwallet_library} )
	else( kwallet_library )
		TARGET_LINK_LIBRARIES( zuluCrypt-gui ${QT_LIBRARIES} ${blkid} ${library_pwquality} )
	endif( kwallet_library )
else( library_pwquality )
	if( kwallet_library )
		TARGET_LINK_LIBRARIES( zuluCrypt-gui ${QT_LIBRARIES} ${blkid} ${kwallet_library} )
	else( kwallet_library )
		TARGET_LINK_LIBRARIES( zuluCrypt-gui ${QT_LIBRARIES} ${blkid} )
	endif( kwallet_library )
endif( library_pwquality )
		
install( TARGETS zuluCrypt-gui RUNTIME DESTINATION bin )

install (FILES zuluCrypt.desktop 
DESTINATION share/applications

PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
)

install (FILES zuluCrypt.png DESTINATION share/icons)
#install (FILES zuluCrypt.png DESTINATION share/icons/hicolor/32x32/apps)

# desktop file section
file(WRITE zuluCrypt.desktop

"#!/usr/bin/env xdg-open
[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_PREFIX}/bin/zuluCrypt-gui -d %U
GenericName[en_US]=Encrypted volumes manager
GenericName=Encrypted volumes manager
Icon=${CMAKE_INSTALL_PREFIX}/share/icons/zuluCrypt.png
MimeType=
Name[en_US]=ZuluCrypt
Name=ZuluCrypt
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
Categories=Security;Utility;Qt;X-MandrivaLinux-System-FileTools;\n")
