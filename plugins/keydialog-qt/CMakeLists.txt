cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++0x -I${PROJECT_BINARY_DIR}/plugins/keydialog-qt -I${PROJECT_BINARY_DIR} ) 

INCLUDE( ${QT_USE_FILE} )

QT4_WRAP_UI( UI mainwindow.ui ../../zuluCrypt-gui/dialogmsg.ui )

QT4_WRAP_CPP( MOC mainwindow.h ../../zuluCrypt-gui/socketsendkey.h ../../zuluCrypt-gui/dialogmsg.h )

QT4_ADD_RESOURCES( ICON icon.qrc )

add_executable( keydialog-qt ${ICON} ${MOC} ${UI} main.cpp mainwindow.cpp ../../zuluCrypt-gui/socketsendkey.cpp ../../zuluCrypt-gui/dialogmsg.cpp )

TARGET_LINK_LIBRARIES( keydialog-qt ${QT_LIBRARIES} ${zuluPluginManager} ${blkid} ) 

set_target_properties( keydialog-qt PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )

install( TARGETS keydialog-qt RUNTIME DESTINATION "${PLUGINPATH}" )
