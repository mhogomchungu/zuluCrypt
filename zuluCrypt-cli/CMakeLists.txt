 cmake_minimum_required(VERSION 2.6)  
 
 file(WRITE  ${PROJECT_BINARY_DIR}/bin_path.h "\n#define ZULUCRYPTzuluCrypt      \"${CMAKE_INSTALL_PREFIX}/bin/zuluCrypt-cli\"\n")
 file(APPEND ${PROJECT_BINARY_DIR}/bin_path.h "\n#define zuluMount	         \"${CMAKE_INSTALL_PREFIX}/bin/zuluMount-cli\"\n")
 file(APPEND ${PROJECT_BINARY_DIR}/bin_path.h "\n#define ZULUCRYPTTest           \"${CMAKE_INSTALL_PREFIX}/bin/zuluCrypt-testKey\"\n")
 
 add_definitions( -I${PROJECT_BINARY_DIR} ) 
  
 SET( BIN  
 bin/close_volume.c 
 bin/add_key.c
 bin/volume_info.c
 bin/open_volume.c 
 bin/partitions.c 
 bin/security.c
 bin/check_invalid_key.c
 bin/create_volumes.c 
 bin/remove_key.c
 bin/write_device_with_junk.c
 bin/save_and_restore_luks_header.c
 bin/crypt_file.c
 bin/check_opened_mapper.c
 bin/get_opts.c
 bin/help.c
 bin/mount_flags.c 
 bin/file_encryption.c
 bin/clear_dead_mappers.c
 ) 
 
 SET( BINMount
 ../zuluMount-cli/print_mounted_volumes.c
 bin/partitions.c
 ../zuluMount-cli/main.c
 ../zuluMount-cli/mount.c
 ../zuluMount-cli/umount.c
 ../zuluMount-cli/crypto_mount.c
 ../zuluMount-cli/crypto_umount.c 
 )
 
 SET( LIB 
 lib/print_mounted_volumes.c  
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
 lib/canonicalize/canonicalize.c
 lib/create_tcrypt.c
)
 	 
 add_library( Socket STATIC utility/socket/socket.c ) 
 
 #add_library( String STATIC string/String.c string/StringManage.c ) 
 add_library( String STATIC utility/string/String.c ) 
 
 add_library( StringList STATIC utility/string/StringList.c )
 
 add_library( Process STATIC utility/process/process.c )
 
 TARGET_LINK_LIBRARIES( Process -pthread ) 
 
 add_library( zuluCrypt-exe SHARED ${BIN} )
 
 add_library( zuluCrypt-exe-static STATIC ${BIN} )
 
 add_library( zuluCrypt SHARED ${LIB} )
 
 add_library( zuluCrypt-static STATIC ${LIB} )
 
 add_library( zuluCryptPluginManager SHARED pluginManager/zuluCryptPluginManager.c )
 
 add_library( zuluCryptPluginManager-static STATIC pluginManager/zuluCryptPluginManager.c )
 
 TARGET_LINK_LIBRARIES( zuluCryptPluginManager Socket Process String ${blkid} )
 
 TARGET_LINK_LIBRARIES( zuluCryptPluginManager-static Socket Process String ${blkid} )
 
 set_target_properties( zuluCryptPluginManager PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread -pedantic " )
 
 set_target_properties( zuluCryptPluginManager-static PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -pedantic " )
 
 set_target_properties( zuluCryptPluginManager PROPERTIES SOVERSION ${LIB_PLUGIN_VERSION} )
 
 set_target_properties( Socket        PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
 
 set_target_properties( String        PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
 set_target_properties( StringList    PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
 set_target_properties( Process       PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
 set_target_properties( zuluCrypt-exe PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
 set_target_properties( zuluCrypt     PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
 
 set_target_properties( zuluCrypt-exe-static PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
 set_target_properties( zuluCrypt-static     PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
 
 set_target_properties( zuluCrypt     PROPERTIES SOVERSION ${LIB_VERSION} )
 set_target_properties( zuluCrypt-exe PROPERTIES SOVERSION ${LIB_VERSION} )
 
 TARGET_LINK_LIBRARIES( zuluCrypt     ${cryptsetup_lib} String StringList ${blkid} ${mountLibrary} Process ) 
 TARGET_LINK_LIBRARIES( zuluCrypt-exe zuluCrypt zuluCryptPluginManager ) 
 
 TARGET_LINK_LIBRARIES( zuluCrypt-static     ${cryptsetup_lib} String StringList ${blkid} ${mountLibrary} Process ) 
 TARGET_LINK_LIBRARIES( zuluCrypt-exe-static zuluCrypt-static zuluCryptPluginManager-static )
 
 TARGET_LINK_LIBRARIES( zuluCrypt-exe zuluCrypt )
 
 add_executable( zuluMount-cli ../zuluMount-cli ${BINMount} )
 
 set_target_properties( zuluMount-cli PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
 set_target_properties( zuluMount-cli PROPERTIES LINK_FLAGS "-pie" )
 TARGET_LINK_LIBRARIES( zuluMount-cli zuluCrypt-exe zuluCrypt ) 
 
 add_executable( zuluCrypt-cli  bin/main.c  )
 set_target_properties( zuluCrypt-cli PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
 set_target_properties( zuluCrypt-cli PROPERTIES LINK_FLAGS "-pie" )
 
 TARGET_LINK_LIBRARIES( zuluCrypt-cli zuluCrypt-exe ) 
 
 set_target_properties( zuluCrypt-cli PROPERTIES INSTALL_RPATH "${LIB_PATH}" )
 set_target_properties( zuluMount-cli PROPERTIES INSTALL_RPATH "${LIB_PATH}" )
  
 if( KEYRINGSUPPORT )
	add_executable( keyring  ../plugins/keyring/keyring.c )
	set_target_properties( keyring PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread  -pedantic -std=c99" )
	set_target_properties( keyring PROPERTIES LINK_FLAGS "-pie" )
	TARGET_LINK_LIBRARIES( keyring zuluCryptPluginManager ${keyring_library} )
	install( TARGETS keyring RUNTIME DESTINATION "${PLUGINPATH}" )	
 endif( KEYRINGSUPPORT )
 
 install(TARGETS zuluCrypt-cli RUNTIME DESTINATION bin            
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
 )
 
 install(TARGETS zuluMount-cli RUNTIME DESTINATION bin 
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
 )

 add_executable( zuluCrypt-test ../test/zuluCrypt-test.c )

 add_executable( zuluCrypt-testKey ../test/zuluCrypt-testKey.c )
 
 set_target_properties( zuluCrypt-test PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
 set_target_properties( zuluCrypt-test PROPERTIES LINK_FLAGS "-pie" )

 TARGET_LINK_LIBRARIES( zuluCrypt-test zuluCryptPluginManager StringList ${cryptsetup_lib} )
 
 set_target_properties( zuluCrypt-testKey PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
 set_target_properties( zuluCrypt-testKey PROPERTIES LINK_FLAGS "-pie" )
 
 TARGET_LINK_LIBRARIES( zuluCrypt-testKey zuluCryptPluginManager )
 
 file( WRITE ${PROJECT_BINARY_DIR}/libzuluCrypt.pc 
 "prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}
libdir=${LIB_PATH}
includedir=${CMAKE_INSTALL_PREFIX}/include
 
Name: libzuluCrypt
Description: a zuluCrypt library,a front end to cryptsetup
Version: ${LIB_VERSION}
Libs: -L${LIB_PATH} -lcryptsetup -lzuluCrypt -lzuluCrypt-exe -lzuluCryptPluginManager -lblkid -lmount
Cflags: -I${CMAKE_INSTALL_PREFIX}/include
\n")
 
 install(TARGETS zuluCrypt-test RUNTIME DESTINATION bin            
 PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
 )
 
 install( TARGETS zuluCrypt-testKey RUNTIME DESTINATION "${PLUGINPATH}" )
 
 # ${LIB_PATH} is set in ../CMakeLists.txt
 install(TARGETS zuluCryptPluginManager LIBRARY DESTINATION ${LIB_PATH} )
 
 install(TARGETS zuluCryptPluginManager-static ARCHIVE DESTINATION ${LIB_PATH} )
 
 install(TARGETS zuluCrypt LIBRARY DESTINATION ${LIB_PATH} )
  
 install(TARGETS zuluCrypt-exe LIBRARY DESTINATION ${LIB_PATH} )
 
 install(TARGETS zuluCrypt-exe-static ARCHIVE DESTINATION ${LIB_PATH} )
 
 install(TARGETS zuluCrypt-static ARCHIVE DESTINATION ${LIB_PATH} ) 
 
 install(FILES lib/libzuluCrypt.h              		 DESTINATION include/zuluCrypt )
 install(FILES bin/libzuluCrypt-exe.h		         DESTINATION include/zuluCrypt )
 install(FILES pluginManager/libzuluCryptPluginManager.h DESTINATION include/zuluCrypt )
 install(FILES ${PROJECT_BINARY_DIR}/libzuluCrypt.pc     DESTINATION ${LIB_PATH}/pkgconfig/ PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )

 