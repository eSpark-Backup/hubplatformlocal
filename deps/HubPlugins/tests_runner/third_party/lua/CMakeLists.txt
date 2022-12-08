cmake_minimum_required(VERSION 3.2)

file( GLOB SRC_FILES *.c )

add_library( lua SHARED ${SRC_FILES} )

