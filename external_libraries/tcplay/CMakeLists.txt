cmake_minimum_required (VERSION 2.6)

project (tcplay)
set (VERSION_MAJOR 3)
set (VERSION_MINOR 3)

find_package (PkgConfig)
include (CheckLibraryExists)

set (SRCS_COMMON tcplay.c crc32.c safe_mem.c io.c hdr.c humanize.c crypto.c generic_xts.c)
set (SRCS_LINUX  crypto-gcrypt.c)

set (CFLAGS_LINUX  "-D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE")
set (CFLAGS_WARN "-w")
set (CFLAGS_DEBUG  "-O0 -g3 -DDEBUG")
set (CFLAGS_OPT    "-O3")
set (CFLAGS_VER    "-DMAJ_VER=${VERSION_MAJOR} -DMIN_VER=${VERSION_MINOR}")
set (CFLAGS_COMMON "-std=c99 -fPIC  ${CFLAGS_LINUX} ${CFLAGS_WARN} ${CFLAGS_VER}")
if ($ENV{DEBUG})
	set (CFLAGS_COMMON "${CFLAGS_COMMON} ${CFLAGS_DEBUG}")
else()
	set (CFLAGS_COMMON "${CFLAGS_COMMON} ${CFLAGS_OPT}")
endif()

include_directories(${PROJECT_BINARY_DIR})

pkg_check_modules(DEVMAPPER devmapper)
pkg_check_modules(UUID uuid)
pkg_check_modules(UUID-OSSP ossp-uuid)

find_library(GCRYPT_LIB gcrypt)

if (NOT DEVMAPPER_FOUND)
	message(FATAL_ERROR "Could not find the devmapper library")
endif()

if (NOT UUID_FOUND)
	if (NOT UUID-OSSP_FOUND)
		message(FATAL_ERROR "Could not find the uuid library")
	else()
	        file( WRITE ${PROJECT_BINARY_DIR}/uuid_source.h "\n#define TCPLAY_USE_OSSP_UUID 1\n" )
	endif()
else()
        file( WRITE ${PROJECT_BINARY_DIR}/uuid_source.h "\n#define TCPLAY_USE_OSSP_UUID 0\n" )
endif()

if (NOT GCRYPT_LIB)
	message(FATAL_ERROR "Could not find the gcrypt library")
else()
	set (GCRYPT_LDFLAGS "-lgcrypt -lgpg-error")
	set (GCRYPT_CFLAGS  "")
endif()

set (CMAKE_REQUIRED_LIBRARIES gpg-error)
# gcrypt>=1.5.0 required
check_library_exists(gcrypt gcry_kdf_derive "" HAVE_GCRYPT_PBKDF)
set (CMAKE_REQUIRED_LIBRARIES "")

if (HAVE_GCRYPT_PBKDF)
	set (SRCS_PBKDF pbkdf2-gcrypt.c)
	set (LDFLAGS_PBKDF "")
	set (CFLAGS_PBKDF  "")
	set (PBKDF_BACKEND gcrypt)
else()
	pkg_check_modules(OPENSSL openssl>=1.0.0a)
	if (NOT OPENSSL_FOUND)
		message(FATAL_ERROR "Could not find a gcrypt with gcry_kdf_derive() nor OpenSSL >= 1.0.0a")
	endif()
	set (SRCS_PBKDF pbkdf2-openssl.c)
	set (LDFLAGS_PBKDF ${OPENSSL_LDFLAGS})
	set (CFLAGS_PBKDF  ${OPENSSL_CFLAGS})
	set (PBKDF_BACKEND openssl)
endif()


if (NOT (DEFINED LIB_SUFFIX))
	message(STATUS "")
	message(STATUS "LIB_SUFFIX variable is not defined. It will be autodetected now.")
	message(STATUS "You can set it manually with -DLIB_SUFFIX=<value> (e.g. 64).")
	if (CMAKE_SIZEOF_VOID_P EQUAL 8)
		message(STATUS "\nSetting LIB_SUFFIX=64\n")
		set (LIB_SUFFIX "64")
	else()
		message(STATUS "\nSetting LIB_SUFFIX=\n")
		set (LIB_SUFFIX "")
	endif()
endif()

if (NOT (DEFINED SBIN_DIR))
  set (SBIN_DIR "sbin")
endif()

include_directories(${PROJECT_BINARY_DIR})

add_executable(tcplay-bin main.c ${SRCS_COMMON} ${SRCS_LINUX} ${SRCS_PBKDF})
set_target_properties(tcplay-bin PROPERTIES OUTPUT_NAME tcplay)
set_target_properties(tcplay-bin PROPERTIES COMPILE_FLAGS "${CFLAGS_COMMON} ${DEVMAPPER_CFLAGS} ${UUID_CFLAGS} ${UUID-OSSP_CFLAGS}  ${GCRYPT_CFLAGS}")
target_link_libraries(tcplay-bin ${DEVMAPPER_LDFLAGS} ${UUID_LDFLAGS} ${UUID-OSSP_LDFLAGS} ${GCRYPT_LDFLAGS} ${LDFLAGS_PBKDF})


add_library(tcplay-so SHARED tcplay_api.c ${SRCS_COMMON} ${SRCS_LINUX} ${SRCS_PBKDF})
set_target_properties(tcplay-so PROPERTIES OUTPUT_NAME tcplay)
set_target_properties(tcplay-so PROPERTIES SOVERSION ${VERSION_MAJOR}.${VERSION_MINOR})
set_target_properties(tcplay-so PROPERTIES COMPILE_FLAGS "${CFLAGS_COMMON} ${DEVMAPPER_CFLAGS} ${UUID_CFLAGS} ${UUID-OSSP_CFLAGS} ${GCRYPT_CFLAGS}")
set_target_properties(tcplay-so PROPERTIES LINK_FLAGS "-Wl,--version-script=${PROJECT_SOURCE_DIR}/tcplay.map")
# XXX: revist linking libraries against so. Seems to be more common practice nowadays
target_link_libraries(tcplay-so ${DEVMAPPER_LDFLAGS} ${UUID_LDFLAGS} ${UUID-OSSP_LDFLAGS} ${GCRYPT_LDFLAGS} ${LDFLAGS_PBKDF})


add_library(tcplay-static STATIC tcplay_api.c ${SRCS_COMMON} ${SRCS_LINUX} ${SRCS_PBKDF})
set_target_properties(tcplay-static PROPERTIES OUTPUT_NAME tcplay)
set_target_properties(tcplay-static PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
set_target_properties(tcplay-static PROPERTIES COMPILE_FLAGS "${CFLAGS_COMMON} ${DEVMAPPER_CFLAGS} ${UUID_CFLAGS} ${UUID-OSSP_CFLAGS} ${GCRYPT_CFLAGS}")
target_link_libraries(tcplay-static ${DEVMAPPER_LDFLAGS} ${UUID_LDFLAGS} ${UUID-OSSP_LDFLAGS} ${GCRYPT_LDFLAGS} ${LDFLAGS_PBKDF})


# Generate pkg-config file tcplay.pc
file(WRITE ${PROJECT_BINARY_DIR}/tcplay.pc "prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}
libdir=${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}
includedir=${CMAKE_INSTALL_PREFIX}/include

Name: libtcplay
Description: tcplay as a library
Version: ${VERSION_MAJOR}.${VERSION_MINOR}
Requires.private: devmapper uuid
Libs: -L\${libdir} -ltcplay
Libs.private: -lgpg-error -lgcrypt
Cflags: -I\${includedir}
")


# Installation targets
#install(TARGETS tcplay-bin tcplay-static tcplay-so
#	RUNTIME DESTINATION ${SBIN_DIR}      COMPONENT bin
#	LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT lib
#	ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT lib-dev
#)

#install(FILES ${PROJECT_BINARY_DIR}/tcplay.pc DESTINATION lib${LIB_SUFFIX}/pkgconfig COMPONENT lib-dev)

#install(FILES tcplay_api.h DESTINATION include        COMPONENT lib-dev)
#install(FILES tcplay.3     DESTINATION share/man/man3 COMPONENT lib-dev)

#install(FILES tcplay.8     DESTINATION share/man/man8 COMPONENT bin)




# Optional CPack magic
set (CPACK_RPM_COMPONENT_INSTALL 1)
set (CPACK_DEB_COMPONENT_INSTALL 1)

set (CPACK_PACKAGE_NAME "tcplay")
set (CPACK_PACKAGE_CONTACT "Alex Hornung <alex@alexhornung.com>")
set (CPACK_PACKAGE_VENDOR  "Alex Hornung <alex@alexhornung.com>")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "tcplay is a free (BSD-licensed), pretty much fully featured (including multiple keyfiles, cipher cascades, hidden volumes, etc) and stable TrueCrypt implementation.")
set (CPACK_PACAKGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_PATCH "0")
set (CPACK_PACKAGE_LICENSE "2-clause BSD")

include(CPack)
cpack_add_component(bin DISPLAY_NAME bin REQUIRED INSTALL_TYPES all)
cpack_add_component(lib DISPLAY_NAME lib REQUIRED INSTALL_TYPES all)
cpack_add_component(lib-dev DISPLAY_NAME lib-dev REQUIRED INSTALL_TYPES all DEPENDS lib)

