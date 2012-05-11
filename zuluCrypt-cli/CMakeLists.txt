 cmake_minimum_required(VERSION 2.6)  
 
 file(WRITE bin_path.h "\n#define ZULUCRYPTzuluCrypt      \"${CMAKE_INSTALL_PREFIX}/bin/zuluCrypt-cli\"\n")
 
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
	 
 find_file ( mount_1 mount/mount.h )
 
 find_file ( mount_2 libmount/libmount.h )
 
 if(NOT mount_1)
	 if(NOT mount_2)	 
	 	message (FATAL_ERROR "ERROR: could not find libmount header file(mount/mount.h or libmount/limount.h)")
	 else(mount_2)
		 message( STATUS "Found libmount/libmount.h  : " ${mount_2} )	
		 file(WRITE libmount_header.h "\n#define USE_UNSTABLE_LIBMOUNT_API 1\n#define USE_NEW_LIBMOUNT_API 1\n#include <libmount/libmount.h>\n")
	 endif(NOT mount_2)
 else(mount_1)
	 message( STATUS "Found mount/mount.h  : " ${mount_1} )	
	 file(WRITE libmount_header.h "\n#define USE_UNSTABLE_LIBMOUNT_API 1\n#include <mount/mount.h>\n")	 
 endif(NOT mount_1) 
 
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
 
 find_library( libcryptsetup libcryptsetup.so) 
 
 if(NOT libcryptsetup)	 
	 message (FATAL_ERROR "ERROR: could not find libcryptsetup.so library, is cryptsetup package installed?")
 endif(NOT libcryptsetup)	 
 
 message( STATUS "Found libcryptsetup.so : " ${libcryptsetup} ) 
 
  SET( BIN  
 bin/print_mounted_volumes.c 
 bin/create_mapper_name.c 
 bin/replace_bash_special_chars.c 
 bin/main.c 
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
 ) 
 
 SET( LIB 
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
 lib/file_encryption.c )
 
 add_definitions( -I${path_to_libcryptsetup.h}  )  
 
 #set( CMAKE_SHARED_LINKER_FLAGS "-s" )
 
 add_library( String STATIC string/String.c ) 
 
 add_library( StringList STATIC string/StringList.c )
 
 add_library( Process STATIC process/process.c )
 
 add_library( zuluCrypt SHARED ${LIB} )
 
 set_target_properties( String     PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 set_target_properties( StringList PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 set_target_properties( Process    PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 
 TARGET_LINK_LIBRARIES( zuluCrypt ${libcryptsetup} String ${blkid} ${mountLibrary} Process )
 
 set_target_properties( zuluCrypt PROPERTIES SOVERSION "2.0.0" )
 set_target_properties( zuluCrypt PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -ansi -pedantic -std=c99" )
 
 add_executable( zuluCrypt-cli ${BIN} )
 set_target_properties( zuluCrypt-cli PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIE -pthread -ansi -pedantic -std=c99" )
 set_target_properties( zuluCrypt-cli PROPERTIES LINK_FLAGS "-pie" )
 
 TARGET_LINK_LIBRARIES( zuluCrypt-cli zuluCrypt String StringList ${blkid} ) 
 
 install(TARGETS zuluCrypt-cli 
            RUNTIME DESTINATION bin
            
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
 )
 
 if( UNIX AND NOT APPLE AND NOT CMAKE_CROSSCOMPILING AND NOT EXISTS "/etc/debian_version" )
	if ( CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT LIB_SUFFIX )
	     set ( LIB_SUFFIX 64 )
	 endif( CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT LIB_SUFFIX)
 endif( UNIX AND NOT APPLE AND NOT CMAKE_CROSSCOMPILING AND NOT EXISTS "/etc/debian_version" )
  
 install(TARGETS zuluCrypt LIBRARY DESTINATION lib${LIB_SUFFIX} )
  
 install(FILES zuluCrypt.h DESTINATION include )

 
 