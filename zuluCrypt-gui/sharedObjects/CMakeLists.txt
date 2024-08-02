if(${CMAKE_VERSION} VERSION_GREATER "3.26")
    cmake_minimum_required(VERSION 3.5.0)
else()
    cmake_minimum_required(VERSION 3.0.2)
endif()

set( SRC
	../dialogok.cpp
	../dialogmsg.cpp
	../tablewidget.cpp
	../utility.cpp
	../readonlywarning.cpp
	../plugin.cpp
	../favorites.cpp
	../executablesearchpaths.cpp
	../systemsignalhandler.cpp
	../debugwindow.cpp
	../tcrypt.cpp
	../favorites.cpp
	../openvolume.cpp
	../secrets.cpp
	../favorites2.cpp
	../../zuluMount-gui/oneinstance.cpp
	../../zuluMount-gui/monitor_mountinfo.cpp
	../../zuluMount-gui/zulumounttask.cpp
	../../zuluMount-gui/volumeproperty.cpp
)

set( SHARED_OBJ_MOC_FILES
	../dialogok.h
	../dialogmsg.h
	../readonlywarning.h
	../plugin.h
	../utility.h
	../debugwindow.h
	../tcrypt.h
	../utility.h
	../openvolume.h
	../favorites2.h
	../../zuluMount-gui/oneinstance.h
	../../zuluMount-gui/monitor_mountinfo.h
)

set( UI_FILES
	 ../dialogmsg.ui
	 ../readonlywarning.ui
	 ../plugin.ui
	 ../dialogok.ui
	 ../debugwindow.ui
	../tcrypt.ui
	../openvolume.ui
	../favorites2.ui
	../openvolume.ui
 )

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -I${PROJECT_BINARY_DIR}/zuluCrypt-gui/sharedObjects -I${PROJECT_BINARY_DIR})

if(BUILD_WITH_QT6)

	add_library( sharedObject STATIC ${SHARED_OBJ_MOC_FILES} ${SRC} ${UI_FILES} icon.qrc )

	if( BUILDKWALLET )
		target_link_libraries( sharedObject -lgcrypt lxqt-wallet ${kwallet_library} -lblkid ${Qt6Network_LIBRARIES} mhogomchungu_task )
	else()
		target_link_libraries( sharedObject -lgcrypt lxqt-wallet -lblkid ${Qt6Network_LIBRARIES} mhogomchungu_task )
	endif()
else()
	Qt5_WRAP_UI( UI ${UI_FILES} )
	Qt5_WRAP_CPP( MOC ${SHARED_OBJ_MOC_FILES} )
	Qt5_ADD_RESOURCES( ICON icon.qrc )

	add_library( sharedObject STATIC ${MOC} ${SRC} ${UI_FILES} ${ICON} )

	if( BUILDKWALLET )
		target_link_libraries( sharedObject -lgcrypt lxqt-wallet ${kwallet_library} -lblkid ${Qt5Network_LIBRARIES} mhogomchungu_task )
	else()
		target_link_libraries( sharedObject -lgcrypt lxqt-wallet -lblkid ${Qt5Network_LIBRARIES} mhogomchungu_task )
	endif()
endif()

set_target_properties( sharedObject PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -pedantic" )
set_target_properties( sharedObject PROPERTIES LINK_FLAGS "-pie" )
