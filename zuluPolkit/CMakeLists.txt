cmake_minimum_required(VERSION 3.0.2)

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR}/zuluPolkit/ )
include_directories( ${PROJECT_BINARY_DIR}/zuluPolkit/ )

set( MOC_FILES
	zulupolkit.h
 )

set( SRC
	main.cpp
	zulupolkit.cpp
	../zuluCrypt-gui/executablesearchpaths.cpp
)

if( QT5 )
	Qt5_WRAP_CPP( MOC ${MOC_FILES} )
else()
	INCLUDE( ${QT_USE_FILE} )
	Qt4_WRAP_CPP( MOC ${MOC_FILES} )
endif()

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_executable( zuluPolkit ${MOC} ${SRC} )

set_target_properties( zuluPolkit PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )

set_target_properties( zuluPolkit PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic" )

if( QT5 )
        target_link_libraries( zuluPolkit ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} mhogomchungu_task )
else()
        target_link_libraries( zuluPolkit ${QT_LIBRARIES} mhogomchungu_task )
endif()

file( WRITE ${PROJECT_BINARY_DIR}/org.zulucrypt.zulupolkit.policy
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<!DOCTYPE policyconfig PUBLIC
 \"-//freedesktop//DTD PolicyKit Policy Configuration 1.0//EN\"
 \"http://www.freedesktop.org/standards/PolicyKit/1/policyconfig.dtd\">
<policyconfig>

  <action id=\"org.zulucrypt.zulupolkit\">
    <message>Authentication is required to complete requested operation.</message>
    <icon_name>zuluCrypt</icon_name>
    <defaults>
      <allow_any>auth_admin</allow_any>
      <allow_inactive>auth_admin</allow_inactive>
      <allow_active>auth_admin</allow_active>
    </defaults>
    <annotate key=\"org.freedesktop.policykit.exec.path\">${CMAKE_INSTALL_PREFIX}/bin/zuluPolkit</annotate>
  </action>

</policyconfig>
")

install( FILES ${PROJECT_BINARY_DIR}/org.zulucrypt.zulupolkit.policy DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/polkit-1/actions )
install( TARGETS zuluPolkit RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
