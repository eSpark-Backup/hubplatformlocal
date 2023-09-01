cmake_minimum_required(VERSION 3.2)

set(PROJECT tests_runner)

project(${PROJECT} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(third_party/lua)

file(GLOB SRC_FILES *.cpp)

add_executable(${PROJECT} ${SRC_FILES})

target_include_directories(${PROJECT} PRIVATE third_party/lua)
target_include_directories(${PROJECT} PRIVATE third_party/json)

target_link_libraries(${PROJECT} lua)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_link_libraries(${PROJECT} stdc++fs)
endif()
