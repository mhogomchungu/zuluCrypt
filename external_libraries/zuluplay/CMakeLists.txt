cmake_minimum_required (VERSION 2.6)

project (zuluplay)
set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)

find_package (PkgConfig)
include (CheckLibraryExists)

INCLUDE(GNUInstallDirs)

set (SRCS_COMMON tcplay.c tcplay_api.c crc32.c safe_mem.c io.c hdr.c humanize.c crypto.c generic_xts.c)
set (SRCS_LINUX  crypto-gcrypt.c)

set (CFLAGS_LINUX  "-D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D_DEFAULT_SOURCE")
set (CFLAGS_WARN "-Wall -W -Wno-unused-parameter -Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith -Wold-style-definition -Wreturn-type -Wwrite-strings -Wswitch -Wshadow -Wunused-parameter -Wchar-subscripts	-Winline -Wnested-externs")
set (CFLAGS_DEBUG  "-O0 -g3 -DDEBUG")
set (CFLAGS_OPT    "-O3")
set (CFLAGS_VER    "-DMAJ_VER=${VERSION_MAJOR} -DMIN_VER=${VERSION_MINOR}")
set (CFLAGS_COMMON "-std=c99 -fPIE ${CFLAGS_LINUX} ${CFLAGS_WARN} ${CFLAGS_VER}")
set (CFLAGS_STATIC_COMMON "-std=c99 -fPIC ${CFLAGS_LINUX} ${CFLAGS_WARN} ${CFLAGS_VER}")
set (CFLAGS_SHARED_COMMON "-std=c99 -fPIC ${CFLAGS_LINUX} ${CFLAGS_WARN} ${CFLAGS_VER}")

if ($ENV{DEBUG})
	set (CFLAGS_COMMON "${CFLAGS_COMMON} ${CFLAGS_DEBUG}")
else()
	set (CFLAGS_COMMON "${CFLAGS_COMMON} ${CFLAGS_OPT}")
endif()

pkg_check_modules(DEVMAPPER devmapper)
pkg_check_modules(UUID uuid)
find_library(GCRYPT_LIB gcrypt)

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
	message(FATAL_ERROR "Could not find development files of libgcrypt")

	pkg_check_modules(OPENSSL openssl>=1.0.0a)
	if (NOT OPENSSL_FOUND)
		message(FATAL_ERROR "Could not find a gcrypt with gcry_kdf_derive() nor OpenSSL >= 1.0.0a")
	endif()
	set (SRCS_PBKDF pbkdf2-openssl.c)
	set (LDFLAGS_PBKDF ${OPENSSL_LDFLAGS})
	set (CFLAGS_PBKDF  ${OPENSSL_CFLAGS})
	set (PBKDF_BACKEND openssl)
endif()

if(ZULUPLAY_STATIC_ONLY)

add_library(zuluplay-static STATIC tcplay_api.c ${SRCS_COMMON} ${SRCS_LINUX} ${SRCS_PBKDF})
set_target_properties(zuluplay-static PROPERTIES OUTPUT_NAME zuluplay)
set_target_properties(zuluplay-static PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
set_target_properties(zuluplay-static PROPERTIES COMPILE_FLAGS "${CFLAGS_STATIC_COMMON} ${DEVMAPPER_CFLAGS} ${UUID_CFLAGS} ${GCRYPT_CFLAGS}")

else()

add_executable(zuluplay-bin main.c ${SRCS_COMMON} ${SRCS_LINUX} ${SRCS_PBKDF})
set_target_properties(zuluplay-bin PROPERTIES OUTPUT_NAME zuluplay)
set_target_properties(zuluplay-bin PROPERTIES COMPILE_FLAGS "${CFLAGS_COMMON} ${DEVMAPPER_CFLAGS} ${UUID_CFLAGS} ${GCRYPT_CFLAGS}")
target_link_libraries(zuluplay-bin  ${GCRYPT_LIB} ${DEVMAPPER_LDFLAGS} ${UUID_LDFLAGS} ${GCRYPT_LDFLAGS} ${LDFLAGS_PBKDF})

add_library(zuluplay-static STATIC tcplay_api.c ${SRCS_COMMON} ${SRCS_LINUX} ${SRCS_PBKDF})
set_target_properties(zuluplay-static PROPERTIES OUTPUT_NAME zuluplay)
set_target_properties(zuluplay-static PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
set_target_properties(zuluplay-static PROPERTIES COMPILE_FLAGS "${CFLAGS_STATIC_COMMON} ${DEVMAPPER_CFLAGS} ${UUID_CFLAGS} ${GCRYPT_CFLAGS}")

add_library(zuluplay-so SHARED ${SRCS_COMMON} ${SRCS_LINUX} ${SRCS_PBKDF})
set_target_properties(zuluplay-so PROPERTIES OUTPUT_NAME zuluplay)
set_target_properties(zuluplay-so PROPERTIES SOVERSION ${VERSION_MAJOR}.${VERSION_MINOR})
set_target_properties(zuluplay-so PROPERTIES COMPILE_FLAGS "${CFLAGS_SHARED_COMMON} ${DEVMAPPER_CFLAGS} ${UUID_CFLAGS} ${GCRYPT_CFLAGS}")
set_target_properties(zuluplay-so PROPERTIES LINK_FLAGS "-Wl,--version-script=${PROJECT_SOURCE_DIR}/tcplay.map")
# XXX: revist linking libraries against so. Seems to be more common practice nowadays
target_link_libraries(zuluplay-so ${GCRYPT_LIB} ${DEVMAPPER_LDFLAGS} ${UUID_LDFLAGS} ${GCRYPT_LDFLAGS} ${LDFLAGS_PBKDF})

# Generate pkg-config file zuluplay.pc
file(WRITE ${PROJECT_BINARY_DIR}/zuluplay.pc "prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}
libdir=${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
includedir=${CMAKE_INSTALL_PREFIX}/include

Name: libzuluplay
Description: zuluplay as a library
Version: ${VERSION_MAJOR}.${VERSION_MINOR}
Requires.private: devmapper uuid
Libs: -L\${libdir} -lzuluplay
Libs.private: -lgpg-error -lgcrypt
Cflags: -I\${includedir}
")


# Installation targets
install(TARGETS zuluplay-bin zuluplay-so
	RUNTIME DESTINATION bin                     COMPONENT bin
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib-dev
)

install(FILES ${PROJECT_BINARY_DIR}/zuluplay.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig COMPONENT lib-dev)

install(FILES zuluplay_api.h DESTINATION include        COMPONENT lib-dev)
install(FILES zuluplay.3     DESTINATION share/man/man3 COMPONENT lib-dev)

install(FILES zuluplay.8     DESTINATION share/man/man8 COMPONENT bin)

# Optional CPack magic
set (CPACK_RPM_COMPONENT_INSTALL 1)
set (CPACK_DEB_COMPONENT_INSTALL 1)

set (CPACK_PACKAGE_NAME "zuluplay")
set (CPACK_PACKAGE_CONTACT "Francis Banyikwa <mhogomchungu@gmail.com>")
set (CPACK_PACKAGE_VENDOR  "Francis Banyikwa <mhogomchungu@gmail.com>")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "zuluplay is a free (BSD-licensed), pretty much fully featured (including multiple keyfiles, cipher cascades, hidden volumes, etc) and stable TrueCrypt implementation.")
set (CPACK_PACAKGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_PATCH "0")
set (CPACK_PACKAGE_LICENSE "2-clause BSD")

include(CPack)
cpack_add_component(bin DISPLAY_NAME bin REQUIRED INSTALL_TYPES all)
cpack_add_component(lib DISPLAY_NAME lib REQUIRED INSTALL_TYPES all)
cpack_add_component(lib-dev DISPLAY_NAME lib-dev REQUIRED INSTALL_TYPES all DEPENDS lib)

endif()
