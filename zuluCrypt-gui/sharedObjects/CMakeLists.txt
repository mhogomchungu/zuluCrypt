cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

set ( SRC
	../dialogmsg.cpp
	../userfont.cpp
	../tablewidget.cpp
	../socketsendkey.cpp 
	../openvolumereadonly.cpp 
	../savemountpointpath.cpp
	../utility.cpp
)

set ( MOC_FILES
	../dialogmsg.h
	../socketsendkey.h 
)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -std=c++0x -I${PROJECT_BINARY_DIR}/zuluCrypt-gui/sharedObjects -I${PROJECT_BINARY_DIR}) 
INCLUDE( ${QT_USE_FILE} )

set ( UI_FILES 	../dialogmsg.ui )

QT4_WRAP_UI( UI ${UI_FILES} )

QT4_WRAP_CPP( MOC ${MOC_FILES} )

add_library( sharedObject STATIC ${MOC} ${SRC} ${UI_FILES} )
set_target_properties( sharedObject PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -pedantic " ) 
set_target_properties( sharedObject PROPERTIES LINK_FLAGS "-pie" )
