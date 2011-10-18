
cmake_minimum_required(VERSION 2.6)

ADD_SUBDIRECTORY( zuluCrypt-cli )

ADD_SUBDIRECTORY( zuluCrypt-gui )

install (FILES zuluCrypt.desktop 
	DESTINATION share/applications

	#PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
)

install (FILES zuluCrypt.png DESTINATION share/icons)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)