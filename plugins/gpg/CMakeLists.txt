cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++0x -I${PROJECT_BINARY_DIR}/plugins/gpg -I${PROJECT_BINARY_DIR} ) 

INCLUDE( ${QT_USE_FILE} )

QT4_WRAP_UI( UI mainwindow.ui ../../zuluCrypt-gui/dialogmsg.ui )

QT4_WRAP_CPP( MOC mainwindow.h getgpgkey.h ../../zuluCrypt-gui/dialogmsg.h ../../zuluCrypt-gui/socketsendkey.h )

QT4_ADD_RESOURCES( ICON icon.qrc )

add_executable( gpg ${ICON} ${MOC} ${UI} main.cpp mainwindow.cpp getgpgkey.cpp ../../zuluCrypt-gui/dialogmsg.cpp ../../zuluCrypt-gui/socketsendkey.cpp )

TARGET_LINK_LIBRARIES( gpg ${QT_LIBRARIES} ${zuluPluginManager} ${blkid} ) 

set_target_properties( gpg PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )

install( TARGETS gpg RUNTIME DESTINATION "${PLUGINPATH}" )
