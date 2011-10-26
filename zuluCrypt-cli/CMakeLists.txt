 cmake_minimum_required(VERSION 2.6)  
 
 message( STATUS "Looking for libcryptsetup.h" )
 
 find_file ( libcryptsetup.h libcryptsetup.h ) 
 
 if(NOT libcryptsetup.h)
	 message (FATAL_ERROR "ERROR: could not find libcryptsetup.h header file, is the development package of cryptsetup installed?")
 endif(NOT libcryptsetup.h)
	 
 message( STATUS ${libcryptsetup.h} " - found" )   
 
 find_path ( path_to_libcryptsetup.h libcryptsetup.h )
 
 add_definitions( -I${path_to_libcryptsetup.h} -D_FILE_OFFSET_BITS=64 -Wall ) 
 
 message( STATUS "Looking for libcryptsetup.so" )
 
 find_library( libcryptsetup.so libcryptsetup.so) 
 
 if(NOT libcryptsetup.so)	 
	 message (FATAL_ERROR "ERROR: could not find libcryptsetup.so library, is cryptsetup package installed?")
 endif(NOT libcryptsetup.so)	 
 
 message( STATUS ${libcryptsetup.so} " - found" ) 
 
 add_executable( zuluCrypt-cli zuluCrypt.c String.c )
 
 add_library( zuluCrypt SHARED libzuluCrypt.c String.c )
 
 TARGET_LINK_LIBRARIES( zuluCrypt ${libcryptsetup.so} ) 
 
 TARGET_LINK_LIBRARIES( zuluCrypt-cli zuluCrypt ) 
 
 install(TARGETS zuluCrypt-cli 
            RUNTIME DESTINATION bin
            
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
 )
 
  install(TARGETS zuluCrypt
            LIBRARY DESTINATION lib
 )
  
  install(FILES zuluCrypt.h DESTINATION include )

  