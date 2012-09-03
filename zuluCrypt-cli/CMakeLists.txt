 cmake_minimum_required(VERSION 2.6)  
 
 file(WRITE  ${PROJECT_BINARY_DIR}/bin_path.h "\n#define ZULUCRYPTzuluCrypt      \"${CMAKE_INSTALL_PREFIX}/bin/zuluCrypt-cli\"\n")
 file(APPEND ${PROJECT_BINARY_DIR}/bin_path.h "\n#define zuluMount	         \"${CMAKE_INSTALL_PREFIX}/bin/zuluMount-cli\"\n")
 
 add_definitions( -I${PROJECT_BINARY_DIR} ) 
 
 find_file ( mntent mntent.h PATHS /usr/lib /usr/lib64 /usr/lib/include /usr/lib64/include /usr/include/gnome-keyring-1 /usr/include /usr/local/include /usr/local/include/glib-2.0 /usr/include/glib-2.0 /usr/lib/glib-2.0/include /usr/lib64/glib-2.0/include ) 
  
 if(NOT mntent)
	 message (FATAL_ERROR "ERROR: could not find mntent.h header file")
 endif(NOT mntent)
	 
 message( STATUS "Found mntent.h  : " ${mntent} )
 
 find_file ( mount_sys sys/mount.h PATHS /usr/lib /usr/lib64 /usr/lib/include /usr/lib64/include /usr/include/gnome-keyring-1 /usr/include /usr/local/include /usr/local/include/glib-2.0 /usr/include/glib-2.0 /usr/lib/glib-2.0/include /usr/lib64/glib-2.0/include )
 
 if(NOT mount_sys)
	 message (FATAL_ERROR "ERROR: could not find sys/mount.h header file")
 endif(NOT mount_sys)
	 
 message( STATUS "Found sys/mount.h  : " ${mount_sys} )
	 
 find_file ( mount_1 mount/mount.h PATHS /usr/lib /usr/lib64 /usr/lib/include /usr/lib64/include /usr/include/gnome-keyring-1 /usr/include /usr/local/include /usr/local/include/glib-2.0 /usr/include/glib-2.0 /usr/lib/glib-2.0/include /usr/lib64/glib-2.0/include ) 
  
  find_file ( mount_2 libmount/libmount.h PATHS /usr/lib /usr/lib64 /usr/lib/include /usr/lib64/include /usr/include/gnome-keyring-1 /usr/include /usr/local/include /usr/local/include/glib-2.0 /usr/include/glib-2.0 /usr/lib/glib-2.0/include /usr/lib64/glib-2.0/include ) 
   
 if(NOT mount_1)
	 if(NOT mount_2)	 
	 	message (FATAL_ERROR "ERROR: could not find libmount header file(mount/mount.h or libmount/limount.h)")
	 else(mount_2)
		 message( STATUS "Found libmount/libmount.h  : " ${mount_2} )	
		 file(WRITE ${PROJECT_BINARY_DIR}/libmount_header.h "\n#define USE_UNSTABLE_LIBMOUNT_API 1\n#define USE_NEW_LIBMOUNT_API 1\n#include <libmount/libmount.h>\n")
	 endif(NOT mount_2)
 else(mount_1)
	 message( STATUS "Found mount/mount.h  : " ${mount_1} )	
	 file(WRITE ${PROJECT_BINARY_DIR}/libmount_header.h "\n#define USE_UNSTABLE_LIBMOUNT_API 1\n#include <mount/mount.h>\n")	 
 endif(NOT mount_1) 
 
 find_library( mountLibrary libmount.so )
 
 if(NOT mountLibrary)	 
	 message (FATAL_ERROR "ERROR: could not find libmount.so library")
 endif(NOT mountLibrary)
	 
 message( STATUS "Found libmount.so  : " ${mountLibrary} ) 
 
 find_file ( libcryptsetup.h libcryptsetup.h PATHS /include /usr/lib /usr/lib64 /usr/lib/include /usr/lib64/include /usr/include/gnome-keyring-1 /usr/include /usr/local/include /usr/local/include/glib-2.0 /usr/include/glib-2.0 /usr/lib/glib-2.0/include /usr/lib64/glib-2.0/include )
  
 if(NOT libcryptsetup.h)
	 message (FATAL_ERROR "ERROR: could not find libcryptsetup.h header file, is the development package of cryptsetup installed?")
 endif(NOT libcryptsetup.h)
	 
 message( STATUS "Found libcryptsetup.h  : " ${libcryptsetup.h} )   
 
 find_library( libcryptsetup libcryptsetup.so) 
 
 if(NOT libcryptsetup)	 
	 message (FATAL_ERROR "ERROR: could not find libcryptsetup.so library, is cryptsetup package installed?")
 endif(NOT libcryptsetup)	 
 
 message( STATUS "Found libcryptsetup.so : " ${libcryptsetup} ) 
 
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
 ) 
 
 SET( BINMount
 ../zuluMount-cli/print_mounted_volumes.c
 bin/partitions.c
 ../zuluMount-cli/main.c
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
 lib/file_encryption.c 
 lib/user_home_path.c 
)
 	 
 add_library( Socket STATIC socket/socket.c ) 
 
 add_library( String STATIC string/String.c ) 
 
 add_library( StringList STATIC string/StringList.c )
 
 add_library( Process STATIC process/process.c )
 
 add_library( zuluCrypt-exe SHARED ${BIN} )
 
 add_library( zuluCrypt-exe-static STATIC ${BIN} )
 
 add_library( zuluCrypt SHARED ${LIB} )
 
 add_library( zuluCrypt-static STATIC ${LIB} )
 
 add_library( zuluCryptPluginManager SHARED pluginManager/zuluCryptPluginManager.c )
 
 add_library( zuluCryptPluginManager-static STATIC pluginManager/zuluCryptPluginManager.c )
 
 TARGET_LINK_LIBRARIES( zuluCryptPluginManager Socket Process String ${blkid} )
 
 TARGET_LINK_LIBRARIES( zuluCryptPluginManager-static Socket Process String ${blkid} )
 
 set_target_properties( zuluCryptPluginManager PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 
 set_target_properties( zuluCryptPluginManager-static PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 
 set_target_properties( zuluCryptPluginManager PROPERTIES SOVERSION "1.0.0" )
 
 set_target_properties( Socket        PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 
 set_target_properties( String        PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 set_target_properties( StringList    PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 set_target_properties( Process       PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 set_target_properties( zuluCrypt-exe PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 set_target_properties( zuluCrypt     PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 
 set_target_properties( zuluCrypt-exe-static PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 set_target_properties( zuluCrypt-static     PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 
 set_target_properties( zuluCrypt     PROPERTIES SOVERSION "3.0.0" )
 set_target_properties( zuluCrypt-exe PROPERTIES SOVERSION "3.0.0" )
 
 TARGET_LINK_LIBRARIES( zuluCrypt     ${libcryptsetup} String StringList ${blkid} ${mountLibrary} Process ) 
 TARGET_LINK_LIBRARIES( zuluCrypt-exe zuluCrypt zuluCryptPluginManager ) 
 
 TARGET_LINK_LIBRARIES( zuluCrypt-static     ${libcryptsetup} String StringList ${blkid} ${mountLibrary} Process ) 
 TARGET_LINK_LIBRARIES( zuluCrypt-exe-static zuluCrypt-static zuluCryptPluginManager-static )
 
 TARGET_LINK_LIBRARIES( zuluCrypt-exe zuluCrypt )
 
 add_executable( zuluMount-cli ../zuluMount-cli ${BINMount} )
 
 set_target_properties( zuluMount-cli PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -ansi -pedantic -std=c99" )
 set_target_properties( zuluMount-cli PROPERTIES LINK_FLAGS "-pie" )
 TARGET_LINK_LIBRARIES( zuluMount-cli zuluCrypt-exe ) 
 
 add_executable( zuluCrypt-cli  bin/main.c  )
 set_target_properties( zuluCrypt-cli PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -ansi -pedantic -std=c99" )
 set_target_properties( zuluCrypt-cli PROPERTIES LINK_FLAGS "-pie" )
 
 TARGET_LINK_LIBRARIES( zuluCrypt-cli zuluCrypt-exe ) 
 
 if( KEYRINGSUPPORT )
	add_executable( keyring  ../plugins/keyring/keyring.c )
	set_target_properties( keyring PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -ansi -pedantic -std=c99" )
	set_target_properties( keyring PROPERTIES LINK_FLAGS "-pie" )
	TARGET_LINK_LIBRARIES( keyring zuluCryptPluginManager ${keyring_library} )
	install( TARGETS keyring RUNTIME DESTINATION "${PLUGINPATH}" )	
	message( STATUS "found gnome-keyring/gnome-keyring.h  ${keyring_header}" )
	message( STATUS "found libgnome-keyring.so  ${keyring_library}" )
 endif( KEYRINGSUPPORT )
 
 install(TARGETS zuluCrypt-cli RUNTIME DESTINATION bin            
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
 )
 
 install(TARGETS zuluMount-cli RUNTIME DESTINATION bin 
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
 )
 
 install(TARGETS zuluCryptPluginManager LIBRARY DESTINATION lib${LIB_SUFFIX} )
 
 install(TARGETS zuluCryptPluginManager-static ARCHIVE DESTINATION lib${LIB_SUFFIX} )
 
 install(TARGETS zuluCrypt LIBRARY DESTINATION lib${LIB_SUFFIX} )
  
 install(TARGETS zuluCrypt-exe LIBRARY DESTINATION lib${LIB_SUFFIX} )
 
 install(TARGETS zuluCrypt-exe-static ARCHIVE DESTINATION lib${LIB_SUFFIX} )
 
 install(TARGETS zuluCrypt-static ARCHIVE DESTINATION lib${LIB_SUFFIX} ) 
 
 install(FILES lib/libzuluCrypt.h              		 DESTINATION include/zuluCrypt )
 install(FILES bin/libzuluCrypt-exe.h		         DESTINATION include/zuluCrypt )
 install(FILES pluginManager/libzuluCryptPluginManager.h DESTINATION include/zuluCrypt )
 
 
 