cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++0x -I${PROJECT_BINARY_DIR}/zuluCrypt-gui/ -I${KDEINCLUDE} ) 

set (UI_FILES createfile.ui
		createpartition.ui
		luksdeletekey.ui
		password.ui
		zulucrypt.ui
		createkeyfile.ui
		luksaddkey.ui
		openpartition.ui
		cryptoinfo.ui
		manageluksheader.ui
		cryptfiles.ui
		createvolumedialog.ui
		dialogmsg.ui
		managesystemvolumes.ui
		managedevicenames.ui
		erasedevice.ui	
		kwalletconfig.ui
)

set( MOC_FILES createfile.h
		luksaddkey.h
		createkeyfile.h
		createkeyfilethread.h		
		luksdeletekey.h 
		createpartition.h
		openpartition.h
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
 )
	
set (SRC createfile.cpp
	createpartition.cpp
	luksdeletekey.cpp
	openpartition.cpp
	createkeyfile.cpp
	createkeyfilethread.cpp	
	luksaddkey.cpp
	main.cpp
	password_dialog.cpp
	startupupdateopenedvolumes.cpp
	zulucrypt.cpp
	closeallvolumesthread.cpp
	runinthread.cpp
	miscfunctions.cpp
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
)

find_package( Qt4 REQUIRED )

INCLUDE( ${QT_USE_FILE} )

QT4_WRAP_UI( UI ${UI_FILES})

QT4_WRAP_CPP(MOC ${MOC_FILES})

QT4_ADD_RESOURCES( TRAY_RC_SRCS icon.qrc)

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

set ( msg "---------------------------------------------------------------------
optional package pwquality used to measure passphrase quality not found.
sources can be found at:https://fedorahosted.org/libpwquality/
---------------------------------------------------------------------" )

find_file ( header_pwquality pwquality.h )
find_library ( library_pwquality libpwquality.so )

if( header_pwquality )
	message ( STATUS "Found pwquality.h  : " ${header_pwquality} )
	if( library_pwquality )
		message( STATUS "Found libpwquality.so : " ${library_pwquality} )
		file(WRITE  ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 1\n")
		file(APPEND ${PROJECT_BINARY_DIR}/can_build_pwquality.h "extern \"C\"\n{\n#include <${header_pwquality}>\n}" )
	else( library_pwquality )
		file(WRITE  ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 0\n")
		message ( WARNING ${msg} )		
	endif( library_pwquality )
else( header_pwquality )
	message ( WARNING "${msg}" )
endif( header_pwquality )
				
add_executable( zuluCrypt-gui ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS})			

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

# desktop file section
file(WRITE zuluCrypt.desktop

"#!/usr/bin/env xdg-open
[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_PREFIX}/bin/zuluCrypt-gui
GenericName[en_US]=manage encypted volumes
GenericName=manage encypted volumes
Icon=${CMAKE_INSTALL_PREFIX}/share/icons/zuluCrypt.png
MimeType=
Name[en_US]=zuluCrypt
Name=zuluCrypt
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
