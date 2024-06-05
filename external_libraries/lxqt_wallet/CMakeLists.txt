if(${CMAKE_VERSION} VERSION_GREATER "3.26")
    cmake_minimum_required(VERSION 3.5.0)
else()
    cmake_minimum_required(VERSION 3.0.2)
endif()

find_package( PkgConfig )

if( WIN32 )
    add_library( lxqt-wallet-backend STATIC backend/fake_wallet_backend.c )

    set_target_properties( lxqt-wallet-backend PROPERTIES COMPILE_FLAGS "-Wall -s -pedantic" )
else()
    add_subdirectory( backend )
endif()

add_subdirectory( frontend )


