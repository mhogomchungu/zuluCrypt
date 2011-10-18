 cmake_minimum_required(VERSION 2.6)

 add_executable( zuluCrypt-cli zuluCrypt.c String.c )
 
 add_library( zuluCrypt SHARED libzuluCrypt.c String.c )
 
 TARGET_LINK_LIBRARIES( zuluCrypt cryptsetup ) 
 
 TARGET_LINK_LIBRARIES( zuluCrypt-cli zuluCrypt ) 
 
 install(TARGETS zuluCrypt-cli 
            RUNTIME DESTINATION bin
            
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
 )
 
  install(TARGETS zuluCrypt
            LIBRARY DESTINATION lib
 )
  
  install(FILES zuluCrypt.h DESTINATION include )
