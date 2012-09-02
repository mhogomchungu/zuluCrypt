cmake_minimum_required(VERSION 2.6)

add_subdirectory( gpg )
add_subdirectory( keykeyfile )

#keyring module is handled by CMakeLists.txt in ../zuluCrypt-cli
#add_subdirectory( keyring )

if( BUILDKWALLET )
	add_subdirectory( kwallet )
endif( BUILDKWALLET )	
