project(cored)

set( service_name "core" )

add_subdirectory( house_modes )
add_subdirectory( model )

include_directories(
    ${FIRMWARE_CORE_DIR}
    ${FIRMWARE_COMMON_DIR}
    ${FIRMWARE_COMMON_GENERATED_DIR}
)

link_directories(${LIBRARIES_DIR})

set( BINARY_NAME ha-${PROJECT_NAME} )

file(GLOB SRC_FILES
    *.cpp *.h
    db_modifier/*.cpp
    script_events/*.cpp
    script_events/resources/*.cpp
)

list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
target_link_libraries(${PROJECT_NAME}
        ${lib_name_rt}
        base_utils
        common_utils
        system_utils
        service_interfaces
        ${service_name}d_house_modes
        ${service_name}d_model
)

add_executable(${BINARY_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(${BINARY_NAME} ${PROJECT_NAME})

# remote interface
file( GLOB SRC_FILES
    remote_interface/*.cpp
    ${FIRMWARE_COMMON_DIR}/utils/utils.cpp
    ${FIRMWARE_COMMON_DIR}/utils/unit_converter.cpp
    device_adding_wizard.cpp
    what_happened.cpp
    )

add_library( ${service_name}d_interface SHARED ${SRC_FILES} )
target_link_libraries( ${service_name}d_interface
    ${service_name}d_house_modes
    ${service_name}d_model
    base_utils
    service_interfaces
    tiny
    jsonrpc
    dbclient
    device_model
    common_utils
)

# lua
set( SRC_FILES bindings/core_lua_binding.cpp )
add_library( lua_${service_name} MODULE ${SRC_FILES} )
target_link_libraries( lua_${service_name}
    lua_binding_interface
    cored_interface
    ${service_name}d_model
    uid_interface
)

file( GLOB EXTRA_TEST_SOURCES tests/fakes/*.cpp tests/fixtures/*.cpp )
HUB_ADD_TESTS_WITH_EXTRA_DEPENDENCIES( ${PROJECT_NAME} cored_interface "" "${EXTRA_TEST_SOURCES}" )

# fill settings.cfg file
CONFIGURE_CFG_FILES(CURRENT_GENERATED_CFG_DIR ${SOURCES_ROOT}/system/common/opt/firmware/etc settings.cfg)

set( PACKAGE_NAME firmware-core-${PROJECT_NAME})
install( TARGETS ${BINARY_NAME} DESTINATION ${PACKAGE_NAME}/opt/firmware/bin )
install( TARGETS ${service_name}d_interface DESTINATION ${PACKAGE_NAME}/opt/firmware/lib )
install( FILES ${CURRENT_GENERATED_CFG_DIR}/settings.cfg DESTINATION ${PACKAGE_NAME}/opt/firmware/etc )

set( LUA_PACKAGE_NAME firmware-core-lua_${PROJECT_NAME})
install( TARGETS lua_${service_name} DESTINATION ${LUA_PACKAGE_NAME}/opt/firmware/lib )
