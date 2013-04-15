cmake_minimum_required(VERSION 2.6)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wall -pedantic -std=c++0x -I${PROJECT_BINARY_DIR}/plugins/kwallet/ -I${PROJECT_BINARY_DIR} -I${KDEINCLUDE} ) 

INCLUDE( ${QT_USE_FILE} )

QT4_WRAP_CPP( MOC wallet.h ../../zuluCrypt-gui/socketsendkey.h )

add_executable( kwallet ${MOC} main.cpp wallet.cpp ../../zuluCrypt-gui/socketsendkey.cpp ../../zuluCrypt-gui/zuluoptions.cpp ../../zuluCrypt-gui/kwalletplugin.cpp )

TARGET_LINK_LIBRARIES( kwallet ${kwallet_library} zuluCryptPluginManager ${blkid} ) 
set_target_properties( kwallet PROPERTIES INSTALL_RPATH "${LIB_PATH}" )
set_target_properties( kwallet PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )

install( TARGETS kwallet RUNTIME DESTINATION "${PLUGINPATH}" )
