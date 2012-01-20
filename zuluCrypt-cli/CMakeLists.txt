 cmake_minimum_required(VERSION 2.6)  
 
 find_file ( mntent mntent.h )
 
 if(NOT mntent)
	 message (FATAL_ERROR "ERROR: could not find mntent.h header file")
 endif(NOT mntent)
	 
 message( STATUS "Found mntent.h  : " ${mntent} )
 
 find_file ( mount_sys sys/mount.h )
 
 if(NOT mount_sys)
	 message (FATAL_ERROR "ERROR: could not find sys/mount.h header file")
 endif(NOT mount_sys)
	 
 message( STATUS "Found sys/mount.h  : " ${mount_sys} )
	 
 find_file ( mount mount/mount.h )
 
 if(NOT mount)
	 message (FATAL_ERROR "ERROR: could not find mount/mount.h header file")
 endif(NOT mount)
 
 message( STATUS "Found mount/mount.h  : " ${mount} )
 
 find_library( mountLibrary libmount.so )
 
 if(NOT mountLibrary)	 
	 message (FATAL_ERROR "ERROR: could not find libmount.so library")
 endif(NOT mountLibrary)
	 
 message( STATUS "Found libmount.so  : " ${mountLibrary} ) 
 
 find_file ( libcryptsetup.h libcryptsetup.h ) 
 
 if(NOT libcryptsetup.h)
	 message (FATAL_ERROR "ERROR: could not find libcryptsetup.h header file, is the development package of cryptsetup installed?")
 endif(NOT libcryptsetup.h)
	 
 message( STATUS "Found libcryptsetup.h  : " ${libcryptsetup.h} )   
 
 find_path ( path_to_libcryptsetup.h libcryptsetup.h )
 
 add_definitions( -I${path_to_libcryptsetup.h} -D_FILE_OFFSET_BITS=64 -Wall ) 
 
 #message( STATUS "Looking for libcryptsetup.so" )
 
 find_library( libcryptsetup libcryptsetup.so) 
 
 if(NOT libcryptsetup)	 
	 message (FATAL_ERROR "ERROR: could not find libcryptsetup.so library, is cryptsetup package installed?")
 endif(NOT libcryptsetup)	 
 
 message( STATUS "Found libcryptsetup.so : " ${libcryptsetup} ) 
 
 add_library( String STATIC String.c )
 
 add_library( zuluCrypt SHARED bin/is_path_valid.c lib/version.c lib/close_volume.c lib/execute_commands.c lib/add_key.c lib/remove_key.c lib/empty_slots.c lib/status.c lib/create_luks.c lib/is_luks.c lib/create_volume.c lib/close_mapper.c lib/open_luks.c lib/open_plain.c lib/open_volume.c lib/mount_volume.c lib/unmount_volume.c)
 
 add_executable( zuluCrypt-cli bin/replace_bash_special_chars.c bin/zuluCrypt.c bin/close_volume.c bin/add_key.c bin/volume_info.c bin/open_volume.c bin/partitions.c bin/create_volumes.c bin/remove_key.c) 
 
 TARGET_LINK_LIBRARIES( zuluCrypt ${libcryptsetup} String ${blkid} ${mountLibrary} )
 
 set_target_properties( zuluCrypt PROPERTIES SOVERSION "1.1.0" )
 
 TARGET_LINK_LIBRARIES( zuluCrypt-cli zuluCrypt String ${blkid} ) 
 
 install(TARGETS zuluCrypt-cli 
            RUNTIME DESTINATION bin
            
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
 )
 
  install(TARGETS zuluCrypt
            LIBRARY DESTINATION lib
 )
  
  install(FILES zuluCrypt.h DESTINATION include )

  