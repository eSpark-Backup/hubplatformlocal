cmake_minimum_required(VERSION 3.2)

get_filename_component(PROJECT_ID ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_ID})

include_directories(
    ${FIRMWARE_COMMON_DIR}
    ${FIRMWARE_CORE_DIR}
)

# Update Manager Common
file(GLOB SRC_FILES
    common/*.cpp
    )

add_library(${PROJECT_NAME}_common STATIC ${SRC_FILES})

file(GLOB SRC_FILES
    *.cpp
    )
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
target_link_libraries(${PROJECT_NAME}
    ${PROJECT_NAME}_common
    service_interfaces
    hardware_manager
    common_utils
    http
    plugin_utils
    )

add_executable(ha-${PROJECT_NAME} main.cpp)
target_link_libraries(ha-${PROJECT_NAME}
    ${PROJECT_NAME}
    ${lib_name_rt}
    opkg_utils
    )

# remote interface
file(GLOB SRC_FILES
    remote_interface/*.cpp
    )
add_library( ${PROJECT_NAME}_interface SHARED ${SRC_FILES} )
target_link_libraries( ${PROJECT_NAME}_interface
    ${PROJECT_NAME}_common
    service_interfaces
    common_utils
    )

# Tests
set(TEST_LIBS_SET
    ${PROJECT_NAME}
    )

HUB_ADD_TESTS_WITH_EXTRA_DEPENDENCIES("${PROJECT_NAME}" "${TEST_LIBS_SET}" "" "")

# install
set( PACKAGE_NAME firmware-core-${PROJECT_NAME})
install( TARGETS ${PROJECT_NAME}_interface DESTINATION ${PACKAGE_NAME}/opt/firmware/lib )
install( TARGETS ha-${PROJECT_NAME} DESTINATION ${PACKAGE_NAME}/opt/firmware/bin )

if( ${HUB_TYPE} MATCHES "h2.1" )
	install( PROGRAMS ${CMAKE_SOURCE_DIR}/system/h2.1/opt/firmware/bin/update_bootloader.sh DESTINATION ${PACKAGE_NAME}/opt/firmware/bin )
	install( FILES ${CMAKE_SOURCE_DIR}/system/h2.1/opt/firmware/bootloader/u-boot-sunxi-with-spl.bin DESTINATION ${PACKAGE_NAME}/opt/firmware/bootloader )
endif()

if( ${HUB_TYPE} MATCHES "h2_secure.1" )
	install( PROGRAMS ${CMAKE_SOURCE_DIR}/system/h2_secure.1/opt/firmware/bin/update_bootloader.sh DESTINATION ${PACKAGE_NAME}/opt/firmware/bin )
	install( FILES ${CMAKE_SOURCE_DIR}/system/h2_secure.1/opt/firmware/bootloader/u-boot-sunxi-with-spl.bin DESTINATION ${PACKAGE_NAME}/opt/firmware/bootloader )
endif()
