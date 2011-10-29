
cmake_minimum_required(VERSION 2.6)

ADD_SUBDIRECTORY( zuluCrypt-cli )

ADD_SUBDIRECTORY( zuluCrypt-gui )

ADD_SUBDIRECTORY( test )

install (FILES zuluCrypt-cli.1.bz2 
DESTINATION share/man/man1
)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)