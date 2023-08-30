cmake_minimum_required(VERSION 3.5)

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE "Release" )
    message( STATUS "CMAKE_BUILD_TYPE was not specified in command line parameters. Sorry, but we set it for you to the ${CMAKE_BUILD_TYPE} value." )
endif()

set( BUILD_GTEST "on" )
set( googleTestDir "gtest" )

add_subdirectory( ${googleTestDir} )

file( GLOB TEST_SOURCE_FILES
    *.cpp
    moc/*.cpp
    moc/utils/*.cpp
    moc/utils/*.c
    moc/cc/*.cpp
    moc/fc/*.cpp
    fixtures/*.cpp
    fakes/*.cpp
    accessors/*.cpp
    unittests/*.cpp
    )

add_executable(check_znet
    ${googleTestDir}
    ${TEST_SOURCE_FILES}
    )

target_compile_options(check_znet
    PUBLIC -g3 -O0 -Wreturn-type
    )

target_link_libraries(check_znet
    znet
    gtest
    gmock_main
    gmock
    )

target_include_directories(check_znet
    PUBLIC ${googleTestDir}/googlemock/include
    PUBLIC ./../include
    PUBLIC ./../src
    PUBLIC ./../../datastructs
    PUBLIC ./../../system_time
    PUBLIC ./../../serial_port
    PUBLIC ./../..
    PRIVATE moc
    PRIVATE fixtures
    PRIVATE accessors
    PRIVATE fakes
    )

target_compile_definitions(check_znet PRIVATE BUILD_ZNET_LIBRARY=TRUE) #allow access to private znet headers

enable_testing()
add_test(NAME check_znet COMMAND check_znet)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose DEPENDS check_znet)
set_tests_properties(check_znet PROPERTIES ENVIRONMENT "GTEST_BREAK_ON_FAILURE=1")
