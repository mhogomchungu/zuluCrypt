 cmake_minimum_required(VERSION 2.6)

 file(WRITE  ${PROJECT_BINARY_DIR}/bin_path.h "\n#define ZULUCRYPTzuluCrypt      \"${CMAKE_INSTALL_FULL_BINDIR}/zuluCrypt-cli\"\n")
 file(APPEND ${PROJECT_BINARY_DIR}/bin_path.h "\n#define zuluPolkitPath	         \"${CMAKE_INSTALL_FULL_BINDIR}/zuluPolkit\"\n")
 file(APPEND ${PROJECT_BINARY_DIR}/bin_path.h "\n#define zuluMountPath	         \"${CMAKE_INSTALL_FULL_BINDIR}/zuluMount-cli\"\n")
 file(APPEND ${PROJECT_BINARY_DIR}/bin_path.h "\n#define ZULUCRYPTplugInPath     \"${PLUGINPATH}/zuluCrypt-testKey\"\n")

 add_definitions( -I${PROJECT_BINARY_DIR} )

 SET( BIN
 bin/close_volume.c
 bin/add_key.c
 bin/volume_info.c
 bin/open_volume.c
 bin/volumes.c
 bin/security.c
 bin/check_invalid_key.c
 bin/create_volumes.c
 bin/remove_key.c
 bin/write_device_with_junk.c
 bin/save_and_restore_volume_header.c
 bin/crypt_file.c
 bin/check_opened_mapper.c
 bin/get_opts.c
 bin/help.c
 bin/mount_flags.c
 bin/file_encryption.c
 bin/clear_dead_mappers.c
 bin/bind.c
 bin/create_mount_point.c
 bin/path_access.c
 bin/test.c
 ../zuluMount-cli/volume_status.c
 )

 SET( BINMount
 ../zuluMount-cli/volume_status.c
 ../zuluMount-cli/main.c
 ../zuluMount-cli/mount.c
 ../zuluMount-cli/umount.c
 ../zuluMount-cli/crypto_mount.c
 ../zuluMount-cli/crypto_umount.c
 )

 SET( LIB
 lib/bitlocker.c
 lib/mountinfo.c
 lib/create_mapper_name.c
 lib/is_path_valid.c
 lib/version.c
 lib/close_volume.c
 lib/add_key.c
 lib/remove_key.c
 lib/empty_slots.c
 lib/status.c
 lib/create_luks.c
 lib/is_luks.c
 lib/create_volume.c
 lib/close_mapper.c
 lib/open_luks.c
 lib/open_plain.c
 lib/open_volume.c
 lib/mount_volume.c
 lib/unmount_volume.c
 lib/user_home_path.c
 lib/create_loop_device.c
 lib/blkid_evaluate_tag.c
 lib/open_tcrypt.c
 lib/volume_type.c
 lib/mount_fs_options.c
 lib/real_path.c
 lib/file_path_security.c
 lib/create_tcrypt.c
 lib/parse_fstab.c
 lib/resolve_paths.c
)

 add_library( Socket STATIC utility/socket/socket.c )

 #add_library( String STATIC string/String.c string/StringManage.c )
 add_library( String STATIC utility/string/String.c )

 add_library( StringList STATIC utility/string/StringList.c )

 add_library( Process STATIC utility/process/process.c )

 TARGET_LINK_LIBRARIES( Process -pthread )

 add_library( zuluCrypt-exe SHARED ${BIN} )

 #add_library( zuluCrypt-exe-static STATIC ${BIN} )

 add_library( zuluCrypt SHARED ${LIB} )

 #add_library( zuluCrypt-static STATIC ${LIB} )

 add_library( zuluCryptPluginManager SHARED pluginManager/zuluCryptPluginManager.c )

 #add_library( zuluCryptPluginManager-static STATIC pluginManager/zuluCryptPluginManager.c )

 TARGET_LINK_LIBRARIES( zuluCryptPluginManager Socket Process String )

 #TARGET_LINK_LIBRARIES( zuluCryptPluginManager-static Socket Process String )

 set_target_properties( zuluCryptPluginManager PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread -pedantic " )

 #set_target_properties( zuluCryptPluginManager-static PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -pedantic " )

 set_target_properties( zuluCryptPluginManager PROPERTIES SOVERSION ${LIB_PLUGIN_VERSION} )

 set_target_properties( Socket        PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )

 set_target_properties( String        PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
 set_target_properties( StringList    PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
 set_target_properties( Process       PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
 set_target_properties( zuluCrypt-exe PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
 set_target_properties( zuluCrypt     PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )

 #set_target_properties( zuluCrypt-exe-static PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
 #set_target_properties( zuluCrypt-static     PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )

 set_target_properties( zuluCrypt     PROPERTIES SOVERSION ${LIB_VERSION} )
 set_target_properties( zuluCrypt-exe PROPERTIES SOVERSION ${LIB_VERSION} )

 if( STATIC_ZULUPLAY )
        TARGET_LINK_LIBRARIES( zuluCrypt     String StringList Process tcplay-static ${cryptsetup_lib} ${blkid} ${uuid_lib} ${devmapper_lib} -lgcrypt )
 else()
        TARGET_LINK_LIBRARIES( zuluCrypt     String StringList Process ${cryptsetup_lib} ${blkid} ${uuid_lib} ${devmapper_lib} ${TCPLAY_LIB} -lgcrypt -ltcplay )
 endif()

 set_target_properties( zuluCrypt PROPERTIES LINK_FLAGS "-pie -Wl,-z,relro -Wl,-z,now" )

 TARGET_LINK_LIBRARIES( zuluCrypt-exe zuluCrypt zuluCryptPluginManager )

 set_target_properties( zuluCrypt-exe PROPERTIES LINK_FLAGS "-pie -Wl,-z,relro -Wl,-z,now" )

 #TARGET_LINK_LIBRARIES( zuluCrypt-static String StringList Process )
 #TARGET_LINK_LIBRARIES( zuluCrypt-exe-static zuluCrypt-static zuluCryptPluginManager-static )

 TARGET_LINK_LIBRARIES( zuluCrypt-exe zuluCrypt )

 add_executable( zuluMount-cli ../zuluMount-cli ${BINMount} )

 set_target_properties( zuluMount-cli PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
 set_target_properties( zuluMount-cli PROPERTIES LINK_FLAGS "-pie -Wl,-z,relro -Wl,-z,now" )
 TARGET_LINK_LIBRARIES( zuluMount-cli zuluCrypt-exe zuluCrypt )

 add_executable( zuluCrypt-cli  bin/main.c  )
 set_target_properties( zuluCrypt-cli PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
 set_target_properties( zuluCrypt-cli PROPERTIES LINK_FLAGS "-pie -Wl,-z,relro -Wl,-z,now" )

 TARGET_LINK_LIBRARIES( zuluCrypt-cli zuluCrypt-exe zuluCrypt )

 set_target_properties( zuluCrypt-cli PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )
 set_target_properties( zuluMount-cli PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )

 #zuluCryptKeyServer not used
 #add_executable( zuluCryptKeyServer ../plugins/network_key/server.c )
 #set_target_properties( zuluCryptKeyServer PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -pedantic " )
 #TARGET_LINK_LIBRARIES( zuluCryptKeyServer lxqtwallet crypt_buffer )

 if( NOT NOSECRETSUPPORT )

	pkg_check_modules( LIBSECRET libsecret-1 )

	if( LIBSECRET_FOUND )
		add_definitions( ${LIBSECRET_CFLAGS} )
		add_executable( keyring  ../plugins/keyring/keyring.c )
		set_target_properties( keyring PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread  -pedantic" )
		set_target_properties( keyring PROPERTIES LINK_FLAGS "-pie" )
		TARGET_LINK_LIBRARIES( keyring zuluCryptPluginManager ${keyring_library} ${blkid} ${LIBSECRET_LIBRARIES} )
		set_target_properties( keyring PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )
		install( TARGETS keyring RUNTIME DESTINATION "${PLUGINPATH}" )
	endif()
 endif()

 if( USE_POLKIT )
    install(TARGETS zuluCrypt-cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
    install(TARGETS zuluMount-cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
 else()
    install(TARGETS zuluCrypt-cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID )
    install(TARGETS zuluMount-cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID )
 endif()





 add_executable( zuluCrypt-testKey ../plugins/test/zuluCrypt-testKey.c )

 set_target_properties( zuluCrypt-testKey PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
 set_target_properties( zuluCrypt-testKey PROPERTIES LINK_FLAGS "-pie" )

 TARGET_LINK_LIBRARIES( zuluCrypt-testKey zuluCryptPluginManager ${blkid} )
 set_target_properties( zuluCrypt-testKey PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )
 #set_target_properties( zuluCrypt-test PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )

 file( WRITE ${PROJECT_BINARY_DIR}/libzuluCrypt.pc
 "prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}
libdir=${CMAKE_INSTALL_FULL_LIBDIR}
includedir=${CMAKE_INSTALL_FULL_INCLUDEDIR}

Name: libzuluCrypt
Description: a zuluCrypt library,a front end to cryptsetup
Version: ${LIB_VERSION}
Libs: -L${CMAKE_INSTALL_FULL_LIBDIR} -lcryptsetup -lzuluCrypt -lzuluCrypt-exe -lzuluCryptPluginManager -lblkid -lmount ${TCPLAY_LIB} -L${TCPLAY_LIBDIR} -L${BLKID_LIBDIR} -L${CRYPTSETUP_LIBDIR}
Cflags: -I${CMAKE_INSTALL_FULL_INCLUDEDIR}
\n")

 #install(TARGETS zuluCrypt-test RUNTIME DESTINATION bin
 #PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
 #)

 install( TARGETS zuluCrypt-testKey RUNTIME DESTINATION "${PLUGINPATH}" )

 install(TARGETS zuluCryptPluginManager LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

 #install(TARGETS zuluCryptPluginManager-static ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} )

 install(TARGETS zuluCrypt LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

 install(TARGETS zuluCrypt-exe LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

 #install(TARGETS zuluCrypt-exe-static ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} )

 #install(TARGETS zuluCrypt-static ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} )

 install(FILES lib/libzuluCrypt.h              		 DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zuluCrypt )
 install(FILES bin/libzuluCrypt-exe.h		         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zuluCrypt )
 install(FILES pluginManager/libzuluCryptPluginManager.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zuluCrypt )
 install(FILES ${PROJECT_BINARY_DIR}/libzuluCrypt.pc     DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig/ PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )


