cmake_minimum_required(VERSION 2.6)

if( NOT NOKDE )
	add_subdirectory( kwallet )
endif()

add_subdirectory( gpg )
add_subdirectory( keykeyfile )
add_subdirectory( keydialog-qt )
add_subdirectory( steghide )
add_subdirectory( luks )
add_subdirectory( tomb )

#keyring module is handled by CMakeLists.txt in ../zuluCrypt-cli
#add_subdirectory( keyring )
