cmake_minimum_required(VERSION 3.5)

set(PUBLIC_HEADER_FILES
	lib/znet_release_info.h

	esp32/include/znet/znet_defs.h
	esp32/include/znet/znet_errors.h
	esp32/include/znet/znet.h

	esp32/include/znet/IFile.hpp
	esp32/include/znet/znet_command_class.hpp
	esp32/include/znet/znet_error_message.hpp
	esp32/include/znet/znet.hpp
	esp32/include/znet/znet_node.hpp
	esp32/include/znet/znet_notification.hpp
	esp32/include/znet/iznet_owner.hpp
	esp32/include/znet/znet_utils.hpp
	esp32/include/znet/znet_door_condition.hpp
	esp32/include/znet/znet_requests_manager.hpp
	esp32/include/znet/znet_schedule_polling_handler.hpp
	esp32/include/znet/znet_default_date_time_provider.hpp

	esp32/include/znet/converters/cc_converter.hpp
	esp32/include/znet/converters/barrier_operator_signal_converter.hpp
	esp32/include/znet/converters/barrier_operator_state_converter.hpp
	esp32/include/znet/converters/central_scene_attributes_converter.hpp
	esp32/include/znet/converters/door_lock.hpp
	esp32/include/znet/converters/generic_types_converter.hpp
	esp32/include/znet/converters/multilevel_sensor_converter.hpp
	esp32/include/znet/converters/resources.hpp
	esp32/include/znet/converters/security_modes_converter.hpp
	esp32/include/znet/converters/signed_configuration_parameter_converter.hpp
	esp32/include/znet/converters/specific_types_converter.hpp
	esp32/include/znet/converters/thermostat_setpoint_converter.hpp
	esp32/include/znet/converters/thermostat_operating_state_converter.hpp
	esp32/include/znet/converters/user_code.hpp
	esp32/include/znet/converters/zwave_plus_icon_types_converter.hpp
	esp32/include/znet/converters/manufacturer_names_converter.hpp
	esp32/include/znet/converters/color_switch_converter.hpp
	esp32/include/znet/converters/time_zones_converter.hpp
	esp32/include/znet/converters/configuration_format_converter.hpp
	esp32/include/znet/converters/mode_converter.hpp
	esp32/include/znet/converters/mode_status_converter.hpp
	esp32/include/znet/converters/agi_profile_converter.hpp
	esp32/include/znet/converters/agi_commands_converter.hpp

	esp32/include/znet/enums/barrier_operator_state.hpp
	esp32/include/znet/enums/barrier_operator_signal.hpp
	esp32/include/znet/enums/central_scene_attributes.hpp
	esp32/include/znet/enums/door_lock.hpp
	esp32/include/znet/enums/generic_type.hpp
	esp32/include/znet/enums/multilevel_sensor.hpp
	esp32/include/znet/enums/thermostat_operating_state.hpp
	esp32/include/znet/enums/user_code.hpp
	esp32/include/znet/enums/zwave_plus_icon_types.hpp
	esp32/include/znet/enums/configuration_format.hpp

	esp32/include/znet/cc_0x20_basic.hpp
	esp32/include/znet/cc_0x25_switch.hpp
	esp32/include/znet/cc_0x26_switch_multi_level.hpp
	esp32/include/znet/cc_0x5e_zwave_plus_info.hpp
	esp32/include/znet/cc_0x2b_scene_activation.hpp
	esp32/include/znet/cc_0x2c_scene_actuator_conf.hpp
	esp32/include/znet/cc_0x4e_schedule_entry_lock.hpp
	esp32/include/znet/cc_0x30_binary_sensor.hpp
	esp32/include/znet/cc_0x31_sensor_multilevel.hpp
	esp32/include/znet/cc_0x32_meter.hpp
	esp32/include/znet/cc_0x33_color_switch.hpp
	esp32/include/znet/cc_0x40_thermostat_mode.hpp
	esp32/include/znet/cc_0x42_thermostat_operating_state.hpp
	esp32/include/znet/cc_0x43_thermostat_setpoint.hpp
	esp32/include/znet/cc_0x44_thermostat_fan_mode.hpp
	esp32/include/znet/cc_0x45_thermostat_fan_state.hpp
	esp32/include/znet/cc_0x47_thermostat_setback.hpp
	esp32/include/znet/cc_0x59_association_grp_info.hpp
	esp32/include/znet/cc_0x5b_central_scene.hpp
	esp32/include/znet/cc_0x60_multi_channel.hpp
	esp32/include/znet/cc_0x62_door_lock.hpp
	esp32/include/znet/cc_0x63_user_code.hpp
	esp32/include/znet/cc_0x6f_entry_control.hpp
	esp32/include/znet/cc_0x8b_time_parameters.hpp
	esp32/include/znet/cc_0x8a_time.hpp
	esp32/include/znet/cc_0x66_barrier_operator.hpp
	esp32/include/znet/cc_0x70_configuration.hpp
	esp32/include/znet/cc_0x71_notification.hpp
	esp32/include/znet/cc_0x72_manufacturer_specific.hpp
	esp32/include/znet/cc_0x79_sound_switch.hpp
	esp32/include/znet/cc_0x80_battery.hpp
	esp32/include/znet/cc_0x81_clock.hpp
	esp32/include/znet/cc_0x84_wakeup.hpp
	esp32/include/znet/cc_0x85_association.hpp
	esp32/include/znet/cc_0x87_indicator.hpp
	esp32/include/znet/cc_0x8e_multi_channel_association.hpp
	esp32/include/znet/cc_0x9c_alarm_sensor.hpp
	esp32/include/znet/cc_0x9d_alarm_silence.hpp
	esp32/include/znet/znet_command_class_visitor.hpp
	)

set(HEADER_FILES
#datastructs
	datastructs
#esp32
	esp32/src
#lib
	lib/cc/Transport-Encapsulation/transport_service
	lib/fc
	lib
#libs2
	libs2
#multitasking
	multitasking
#serial_port
	serial_port
#system_time
	system_time
#current
	${CMAKE_CURRENT_SOURCE_DIR}
)

set(SOURCE_FILES
	datastructs/array_bin_search.c
	datastructs/buffers.c
	datastructs/heap.c
	datastructs/lists.c

	esp32/src/znet_chrono.c
	esp32/src/znet_backup.c
	esp32/src/znet_cmd_alarm_sensor.c
	esp32/src/znet_cmd_alarm_silence.c
	esp32/src/znet_cmd_association.c
	esp32/src/znet_cmd_association_group_info.c
	esp32/src/znet_cmd_basic.c
	esp32/src/znet_cmd_battery.c
	esp32/src/znet_cmd_barrier_operator.c
	esp32/src/znet_cmd_binary_sensor.c
	esp32/src/znet_cmd_binary_switch.c
	esp32/src/znet_cmd_central_scene.c
	esp32/src/znet_cmd_clock.c
	esp32/src/znet_cmd_color_switch.c
	esp32/src/znet_cmd_configuration.c
	esp32/src/znet_cmd_device_reset_locally.c
	esp32/src/znet_cmd_door_lock.c
	esp32/src/znet_cmd_user_code.c
	esp32/src/znet_cmd_entry_control.c
	esp32/src/znet_cmd_hail.c
	esp32/src/znet_cmd_inclusion_controller.c
	esp32/src/znet_cmd_manufacturer_specific.c
	esp32/src/znet_cmd_meter.c
	esp32/src/znet_cmd_multichannel_association.c
	esp32/src/znet_cmd_multichannel.c
	esp32/src/znet_cmd_multilevel_sensor.c
	esp32/src/znet_cmd_multilevel_switch.c
	esp32/src/znet_cmd_no_operation.c
	esp32/src/znet_cmd_node_provisioning.c
	esp32/src/znet_cmd_notification.c
	esp32/src/znet_cmd_powerlevel.c
	esp32/src/znet_cmd_scene.c
	esp32/src/znet_cmd_security.c
	esp32/src/znet_cmd_sound_switch.c
	esp32/src/znet_cmd_thermostat_fan_mode.c
	esp32/src/znet_cmd_thermostat_fan_state.c
	esp32/src/znet_cmd_thermostat_mode.c
	esp32/src/znet_cmd_thermostat_operating_state.c
	esp32/src/znet_cmd_thermostat_setback.c
	esp32/src/znet_cmd_thermostat_setpoint.c
	esp32/src/znet_cmd_time_parameters.c
	esp32/src/znet_cmd_time.c
	esp32/src/znet_cmd_schedule_entry_lock.c
	esp32/src/znet_cmd_version.c
	esp32/src/znet_cmd_firmware_update.c
	esp32/src/znet_cmd_wakeup.c
	esp32/src/znet_cmd_zwave_plus_info.c
	esp32/src/znet_cmd_indicator.c
	esp32/src/znet_cmd_anti_theft.c
	esp32/src/znet_cmd_simple_av_control.c
	esp32/src/znet_log.c
	esp32/src/znet_main.c
	esp32/src/znet_mem.c
	esp32/src/znet_store.c
	esp32/src/znet_timer.c
	esp32/src/znet_uart.c

	esp32/src/znet_command_class.cpp
	esp32/src/znet.cpp
	esp32/src/znet_error_message.cpp
	esp32/src/znet_handler.cpp
	esp32/src/znet_handler_interview.cpp
	esp32/src/znet_handler_interview_base.cpp
	esp32/src/znet_handler_exclude.cpp
	esp32/src/znet_handler_learn.cpp
	esp32/src/znet_handler_stop_learn.cpp
	esp32/src/znet_handler_monitoring_base.cpp
	esp32/src/znet_handler_monitoring.cpp
	esp32/src/znet_handler_values.cpp
	esp32/src/znet_handler_values_extended.cpp
	esp32/src/znet_handler_inclusion_controller.cpp
	esp32/src/znet_handler_inclusion_controller_replace.cpp
	esp32/src/znet_discover_handler.cpp
	esp32/src/znet_firmware_update_handler.cpp
	esp32/src/znet_handler_node_check.cpp
	esp32/src/znet_handler_backup_creation.cpp
	esp32/src/znet_handler_backup_restoring.cpp
	esp32/src/znet_impl.cpp
	esp32/src/znet_node.cpp
	esp32/src/iznet_owner.cpp
	esp32/src/znet_utils.cpp
	esp32/src/znet_notification.cpp
	esp32/src/znet_meter.cpp
	esp32/src/znet_multicast_mode.cpp
	esp32/src/znet_alarm_sensor.cpp
	esp32/src/znet_week_day.cpp
	esp32/src/znet_time_manager.cpp
	esp32/src/znet_jobs_holder.cpp
	esp32/src/znet_nodes_processor.cpp
	esp32/src/znet_schedule_helper.cpp
	esp32/src/znet_reachable_check_timeout_settings.cpp
	esp32/src/znet_schedule_polling_handler.cpp
	esp32/src/znet_time_synchronization_handler.cpp
	esp32/src/znet_default_date_time_provider.cpp
	esp32/src/znet_default_timer_provider.cpp
	esp32/src/znet_command_class_utils.cpp
	esp32/src/znet_requests_manager.cpp
	esp32/src/znet_command_class_values_setter.cpp
	esp32/src/znet_provisioning_list_controller.cpp
	esp32/src/znet_provisioning_list_query_executor.cpp
	esp32/src/znet_provisioning_list_provider_data_source.cpp
	esp32/src/node_provisioning_manager.cpp
	esp32/src/anti_theft_info.cpp
	esp32/src/znet_requests_cb_wrapper.cpp

	esp32/src/converters/barrier_operator_state_converter.cpp
	esp32/src/converters/barrier_operator_signal_converter.cpp
	esp32/src/converters/cc_converter.cpp
	esp32/src/converters/central_scene_attributes_converter.cpp
	esp32/src/converters/door_lock.cpp
	esp32/src/converters/generic_types_converter.cpp
	esp32/src/converters/multilevel_sensor_converter.cpp
	esp32/src/converters/security_modes_converter.cpp
	esp32/src/converters/signed_configuration_parameter_converter.cpp
	esp32/src/converters/specific_types_converter.cpp
	esp32/src/converters/value_change_command_converter.cpp
	esp32/src/converters/thermostat_setpoint_converter.cpp
	esp32/src/converters/thermostat_operating_state_converter.cpp
	esp32/src/converters/user_code.cpp
	esp32/src/converters/zwave_plus_icon_types_converter.cpp
	esp32/src/converters/manufacturer_names_converter.cpp
	esp32/src/converters/color_switch_converter.cpp
	esp32/src/converters/time_zones_converter.cpp
	esp32/src/converters/configuration_format_converter.cpp
	esp32/src/converters/thermostat_setback_converter.cpp
	esp32/src/converters/mode_converter.cpp
	esp32/src/converters/mode_status_converter.cpp
    esp32/src/converters/agi_profile_converter.cpp
    esp32/src/converters/agi_commands_converter.cpp
	esp32/src/converters/resources.cpp

	esp32/src/command_classes/cc_0x20_basic.cpp
	esp32/src/command_classes/cc_0x25_switch.cpp
	esp32/src/command_classes/cc_0x26_switch_multi_level.cpp
	esp32/src/command_classes/cc_0x2b_scene_activation.cpp
	esp32/src/command_classes/cc_0x2c_scene_actuator_conf.cpp
	esp32/src/command_classes/cc_0x4e_schedule_entry_lock.cpp
	esp32/src/command_classes/cc_0x30_binary_sensor.cpp
	esp32/src/command_classes/cc_0x31_sensor_multilevel.cpp
	esp32/src/command_classes/cc_0x32_meter.cpp
	esp32/src/command_classes/cc_0x33_color_switch.cpp
	esp32/src/command_classes/cc_0x40_thermostat_mode.cpp
	esp32/src/command_classes/cc_0x42_thermostat_operating_state.cpp
	esp32/src/command_classes/cc_0x43_thermostat_setpoint.cpp
	esp32/src/command_classes/cc_0x44_thermostat_fan_mode.cpp
	esp32/src/command_classes/cc_0x45_thermostat_fan_state.cpp
	esp32/src/command_classes/cc_0x47_thermostat_setback.cpp
	esp32/src/command_classes/cc_0x59_association_grp_info.cpp
	esp32/src/command_classes/cc_0x5b_central_scene.cpp
	esp32/src/command_classes/cc_0x60_multi_channel.cpp
	esp32/src/command_classes/cc_0x62_door_lock.cpp
	esp32/src/command_classes/cc_0x63_user_code.cpp
	esp32/src/command_classes/cc_0x6f_entry_control.cpp
	esp32/src/command_classes/cc_0x8a_time.cpp
	esp32/src/command_classes/cc_0x8b_time_parameters.cpp
	esp32/src/command_classes/cc_0x66_barrier_operator.cpp
	esp32/src/command_classes/cc_0x70_configuration.cpp
	esp32/src/command_classes/cc_0x71_notification.cpp
	esp32/src/command_classes/cc_0x72_manufacturer_specific.cpp
	esp32/src/command_classes/cc_0x79_sound_switch.cpp
	esp32/src/command_classes/cc_0x80_battery.cpp
	esp32/src/command_classes/cc_0x81_clock.cpp
	esp32/src/command_classes/cc_0x84_wakeup.cpp
	esp32/src/command_classes/cc_0x85_association.cpp
	esp32/src/command_classes/cc_0x86_version.cpp
	esp32/src/command_classes/cc_0x87_indicator.cpp
	esp32/src/command_classes/cc_0x8e_multi_channel_association.cpp
	esp32/src/command_classes/cc_0x9c_alarm_sensor.cpp
	esp32/src/command_classes/cc_0x9d_alarm_silence.cpp
	esp32/src/command_classes/cc_0x7a_firmware_update.cpp
	esp32/src/command_classes/cc_anti_theft_base.cpp
	esp32/src/command_classes/cc_0x5d_anti_theft.cpp
	esp32/src/command_classes/cc_0x7e_anti_theft_unlock.cpp
    )

list(APPEND SOURCE_FILES
#lib
	lib/cc/Application/Basic.c
	lib/cc/Application/BarrierOperator.c
	lib/cc/Application/BinarySwitch.c
	lib/cc/Application/Clock.c
	lib/cc/Application/CentralScene.c
	lib/cc/Application/ColorSwitch.c
	lib/cc/Application/Configuration.c
	lib/cc/Application/ColorSwitch.c
	lib/cc/Application/DoorLock.c
	lib/cc/Application/EntryControl.c
	lib/cc/Application/Hail.c
	lib/cc/Application/ManufacturerProprietary.c
	lib/cc/Application/Meter.c
	lib/cc/Application/MultilevelSwitch.c
	lib/cc/Application/Notification.c
	lib/cc/Application/Scene.c
	lib/cc/Application/ScheduleEntryLock.c
	lib/cc/Application/SensorBinary.c
	lib/cc/Application/SensorMultilevel.c
	lib/cc/Application/SensorAlarm.c
	lib/cc/Application/SilenceAlarm.c
	lib/cc/Application/SoundSwitch.c
	lib/cc/Application/ThermostatFanMode.c
	lib/cc/Application/ThermostatFanState.c
	lib/cc/Application/ThermostatMode.c
	lib/cc/Application/ThermostatOperatingState.c
	lib/cc/Application/ThermostatSetback.c
	lib/cc/Application/ThermostatSetpoint.c
	lib/cc/Application/UserCode.c
	lib/cc/Application/AntiTheft.c

	lib/cc/Management/Association.c
	lib/cc/Management/AssociationGroupInfo.c
	lib/cc/Management/Battery.c
	lib/cc/Management/DeviceResetLocaly.c
	lib/cc/Management/ManufacturerSpecific.c
	lib/cc/Management/MultiChannelAssociation.c
	lib/cc/Management/TimeParameters.c
	lib/cc/Management/Time.c
	lib/cc/Management/Version.c
	lib/cc/Management/Wakeup.c
	lib/cc/Management/ZWavePlusInfo.c
	lib/cc/Management/FirmwareUpdate.c
	lib/cc/Management/Indicator.c

	lib/cc/Network-Protocol/InclusionController.c
	lib/cc/Network-Protocol/NodeProvisioning.c
	lib/cc/Network-Protocol/NoOperation.c
	lib/cc/Network-Protocol/PowerLevel.c

	lib/cc/Transport-Encapsulation/Crc16Encap.c
	lib/cc/Transport-Encapsulation/MultiChannel.c
	lib/cc/Transport-Encapsulation/MultiCmd.c
	lib/cc/Transport-Encapsulation/Security2.c
	lib/cc/Transport-Encapsulation/Security.c
	lib/cc/Transport-Encapsulation/Supervision.c
	lib/cc/Transport-Encapsulation/TransportService.c

	lib/cc/Transport-Encapsulation/transport_service/transport2_fsm.c
	lib/cc/Transport-Encapsulation/transport_service/transport_service2.c
	lib/fc/SerialAPISoftReset.c
	lib/fc/AddNodeToNetwork.c
	lib/fc/ApplicationCommandHandler.c
	lib/fc/ApplicationCommandHandlerBridge.c
	lib/fc/ApplicationNodeUpdate.c
	lib/fc/AssignSUCReturnRoute.c
	lib/fc/DummyFunctionClass.c
	lib/fc/ExploreRequestInclusion.c
	lib/fc/GetBackgroundRSSI.c
	lib/fc/GetHomeId.c
	lib/fc/GetNodeProtocolInfo.c
	lib/fc/GetSerialAPICapabilities.c
	lib/fc/GetSUCNodeId.c
	lib/fc/IsFailedNode.c
	lib/fc/LibraryVersion.c
	lib/fc/PowerLevelGet.c
	lib/fc/PowerLevelSet.c
	lib/fc/RemoveFailedNode.c
	lib/fc/RemoveNodeFromNetwork.c
	lib/fc/ReplaceFailedNode.c
	lib/fc/RequestNetworkUpdate.c
	lib/fc/RequestNodeInformation.c
	lib/fc/RequestNodeNeighborUpdate.c
	lib/fc/SendData.c
	lib/fc/SendDataMulti.c
	lib/fc/SendNodeInformation.c
	lib/fc/SendSUCNodeId.c
	lib/fc/SendTestFrame.c
	lib/fc/SerialAPIApplicationNodeInfo.c
	lib/fc/SerialAPIGetInitData.c
	lib/fc/SetDefault.c
	lib/fc/SetLearnMode.c
	lib/fc/SetRoutingMAX.c
	lib/fc/SetSUCNodeID.c
	lib/fc/StoreHomeId.c
	lib/fc/TypeLibrary.c
	lib/fc/NVRGetValue.c
	lib/fc/SerialAPIStarted.c
	lib/fc/SetListenBeforeTalkThreshold.c
        lib/fc/NvmBackupRestore.c

	lib/byte_ops.c
	lib/logger.c
	lib/ZFunctionClass.c
	lib/ZNetActivities.c
	lib/znet_lib.c
	lib/znet_lib_cc.c
	lib/ZPlatform.c
	lib/ZTrackedTimes.c
	lib/znet_release_info.c
#libs2
	libs2/aes.c
	libs2/aes_cmac.c
	libs2/bigint.c
	libs2/ccm.c
	libs2/ctr_drbg.c
	libs2/curve25519.c
	libs2/fe25519.c
	libs2/kderiv.c
	libs2/nextnonce.c
	libs2/rijndael-alg-fst.c
	libs2/S2.c
	libs2/s2_inclusion.c
	libs2/s2_keystore.c
	libs2/S2_wrap.c
	libs2/security_layer.c
	libs2/Security_Scheme0.c
	libs2/ZW_PRNG.c
	libs2/ZW_SendDataAppl.c
#serial_port
	serial_port/serial_port.c
	serial_port/serial_port_esp32.c
	serial_port/serial_port_esp8266.c
	serial_port/serial_port_impl.c
	serial_port/serial_port_linux.c
	serial_port/serial_port_windows.c
#system_time
	system_time/system_time.c
)

set(PNAME znet)

if(NOT DEFINED ENV{BUILD_ESP32})
	project(${PNAME})
	find_package(Threads REQUIRED)
	set (CMAKE_CXX_STANDARD 11)
	option(BUILD_TESTS "Build tests" OFF)
	option(BUILD_TOOLS "Build tools" OFF)
	set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/esp32/cmake")
	get_directory_property(hasCmakeParent PARENT_DIRECTORY)

	add_library(${PNAME} ${HEADER_FILES} ${SOURCE_FILES})

	target_compile_definitions(${PNAME}
	PRIVATE BUILD_ZNET_LIBRARY
	PRIVATE $<$<BOOL:BUILD_TESTS>:BUILD_TESTS>
	)
	target_include_directories(${PNAME}
	PUBLIC esp32/include
	PUBLIC lib
	PRIVATE .
	PRIVATE datastructs
	PRIVATE esp32/src
	PRIVATE lib/cc/Transport-Encapsulation/transport_service
	PRIVATE lib/fc
	PRIVATE libs2
	PRIVATE multitasking
	PRIVATE serial_port
	PRIVATE system_time
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include
	)
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)
option(SKIP_ZNET_LIB_INFO_IF_EXISTS OFF)
message(STATUS "SKIP_ZNET_LIB_INFO_IF_EXISTS: ${SKIP_ZNET_LIB_INFO_IF_EXISTS}")
if(NOT CMAKE_BUILD_EARLY_EXPANSION)
	add_custom_target(lib_info
		COMMAND python3
		        ${CMAKE_CURRENT_SOURCE_DIR}/libinfo.py
		        --module_tag=ZNET
		        --exec_dir=${CMAKE_CURRENT_SOURCE_DIR}
		        --save_filename=${CMAKE_CURRENT_BINARY_DIR}/include/znet_lib_info.h
				--skip_if_exists=${SKIP_ZNET_LIB_INFO_IF_EXISTS}
		)
endif()

if(NOT DEFINED ENV{BUILD_ESP32})
	add_dependencies(${PNAME} lib_info)
	install(TARGETS ${PNAME} DESTINATION lib)
	install(FILES ${PUBLIC_HEADER_FILES} DESTINATION include/znet)

	if(THREADS_HAVE_PTHREAD_ARG)
	target_compile_options(${PNAME}
		PUBLIC "-pthread"
	)
	endif(THREADS_HAVE_PTHREAD_ARG)

	# Tools
	if(BUILD_TOOLS)
		set(TOOL_HEADER_FILES
		)

		set(TOOL_SOURCE_FILES
			esp32/tools/main.c
		)

		add_executable(cli_znet ${TOOL_HEADER_FILES} ${TOOL_SOURCE_FILES})
		target_link_libraries(cli_znet znet lib_info -lreadline -lhistory ${CMAKE_THREAD_LIBS_INIT})
		target_include_directories(cli_znet
			PUBLIC esp32/include
			PRIVATE esp32/tools
		)
		if(THREADS_HAVE_PTHREAD_ARG)
		target_compile_options(cli_znet
			PUBLIC "-pthread"
		)
		endif(THREADS_HAVE_PTHREAD_ARG)
	endif(BUILD_TOOLS)

	# Tests
	if(BUILD_TESTS)
		add_subdirectory(esp32/tests)
	endif(BUILD_TESTS)
endif()


if(DEFINED ENV{BUILD_ESP32})
	message(STATUS "ZNET_LOG_LEVEL: $ENV{ZNET_LOG_LEVEL}")
	idf_component_register(znet SRCS ${SOURCE_FILES}
        INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/include esp32/include . lib datastructs system_time esp32/src multitasking serial_port lib/fc libs2
        REQUIRES esp32 soc freertos)
    add_dependencies(${COMPONENT_LIB} lib_info)
    target_compile_options(${COMPONENT_LIB} PRIVATE -DBUILD_ZNET_LIBRARY -DZNET_LOG_LEVEL=$ENV{ZNET_LOG_LEVEL} -Wno-unused-const-variable -Wno-error=implicit-function-declaration)
    include(../config_flags_throw.cmake)
endif()
