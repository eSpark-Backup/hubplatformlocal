#pragma once

#include <znet/znet_requests_manager_impl.hpp>
#include <znet/znet.h>
#include <znet/enums/user_code.hpp>

namespace znet {

template <typename F, F f>
struct ZnetFunctionWrapper;

template <typename... Args, int ( *f )( Args... )>
struct ZnetFunctionWrapper<int ( * )( Args... ), f> {
    bool operator()( Args... args ) const {
        return f( std::move( args )... ) == 0;
    }
};

template <typename WrappedType>
struct UnspecifiedParam {
    UnspecifiedParam( WrappedType value = {} )
    : m_value( value ) {}

    operator const WrappedType&() const {
        return m_value;
    }

    bool operator<( const WrappedType& other ) const {
        return false;
    }

    WrappedType m_value;
};

using UnspecifiedUint16 = UnspecifiedParam<uint16_t>;
using UnspecifiedUint8 = UnspecifiedParam<uint8_t>;
using UnspecifiedInt = UnspecifiedParam<int>;
using UnspecifiedBool = UnspecifiedParam<bool>;

template <typename, size_t Index = 0>
struct IsUnspecifiedParam : std::integral_constant<bool, false> {};

template <typename WrappedType, size_t Index>
struct IsUnspecifiedParam<UnspecifiedParam<WrappedType>, Index> : std::integral_constant<bool, true> {};

#define ZNET_FUNCTION_WRAPPER( F ) ZnetFunctionWrapper<decltype( &F ), &F>
#define REQUEST_PARAMS( ... ) RequestParams<__VA_ARGS__>
#define RESPOND_PARAMS( ... ) RespondParams<__VA_ARGS__>

/* @brief Defines request type via request params, respond params, C request doer.
 *        Actually request is defined as struct inherited from RequestInfo<> template.
 *        This way is prefferable than just "using" approach (because of debug info size).
 */
#define DEFINE_REQUEST( Request, RequestParamsList, Doer, RespondParamsList )                           \
    struct Request : RequestInfo<RequestParamsList, ZNET_FUNCTION_WRAPPER( Doer ), RespondParamsList> { \
        static const char* getName() {                                                                  \
            return #Request;                                                                            \
        }                                                                                               \
        using Base = RequestInfo;                                                                       \
        using Data = typename Base::Data;                                                               \
        using Base::Base;                                                                               \
    };

DEFINE_REQUEST( UserCodesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t ),
                znet_node_cmd_user_code_get,
                RESPOND_PARAMS( const znet_user_code_report_t&, UserCode::UserPollStatus* ) );

DEFINE_REQUEST( UserCodesNumberRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_users_number_get,
                RESPOND_PARAMS( uint16_t ) );

DEFINE_REQUEST( UserCodeCapabilitiesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_user_code_capabilities_get,
                RESPOND_PARAMS( const znet_user_code_capabilities_report_t& ) );

DEFINE_REQUEST( UserCodesKeypadModeRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_user_code_keypad_mode_get,
                RESPOND_PARAMS( znet_user_code_keypad_mode_t ) );

DEFINE_REQUEST( UserCodeChecksumRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_user_code_checksum_get,
                RESPOND_PARAMS( znet_user_code_checksum_t ) );

DEFINE_REQUEST(
    ExtendedCodesRequest,
    REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint16_t, UnspecifiedParam<bool> ),
    znet_node_cmd_extended_user_code_get,
    RESPOND_PARAMS( uint8_t, const znet_user_code_report_t*, znet_supported_users_t, UserCode::UserPollStatus* ) );

DEFINE_REQUEST( MasterCodeRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_master_code_get,
                RESPOND_PARAMS( uint8_t, const uint8_t* ) );

DEFINE_REQUEST( LocalTimeRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_local_time_get,
                RESPOND_PARAMS( const znet_local_time_t& ) );

DEFINE_REQUEST( LocalDateRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_local_date_get,
                RESPOND_PARAMS( const znet_local_date_t& ) );

DEFINE_REQUEST( TimeZoneRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_time_zone_get,
                RESPOND_PARAMS( const znet_time_zone_settings_t& ) );

DEFINE_REQUEST( TimeParametersRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_time_parameter_get,
                RESPOND_PARAMS( const znet_cmd_time_parameters_report_t& ) );

DEFINE_REQUEST( ClockRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_clock_get,
                RESPOND_PARAMS( const znet_clock_value_t& ) );

DEFINE_REQUEST( ScheduleTimeZoneRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cc_schedule_entry_lock_time_offset_get,
                RESPOND_PARAMS( const znet_cmd_schedule_entry_lock_time_offset_t& ) );

DEFINE_REQUEST( ScheduleEntryLockSupportedRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cc_schedule_entry_type_supported_get,
                RESPOND_PARAMS( const znet_cmd_schedule_entry_lock_supported_report_t& ) );

DEFINE_REQUEST( WeekDayScheduleRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t, uint8_t ),
                znet_node_cc_schedule_entry_lock_week_day_get,
                RESPOND_PARAMS( const znet_week_day_schedule_t& ) );

DEFINE_REQUEST( YearDaySchduleRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t, uint8_t ),
                znet_node_cc_schedule_entry_lock_year_day_get,
                RESPOND_PARAMS( const znet_year_day_schedule_t& ) );

DEFINE_REQUEST( DailyRepeatingScheduleRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t, uint8_t ),
                znet_node_cc_schedule_entry_lock_daily_repeating_get,
                RESPOND_PARAMS( const znet_daily_repeating_schedule_t& ) );

DEFINE_REQUEST( BasicValueRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_basic_get,
                RESPOND_PARAMS( znet_cmd_basic_value_t, znet_cmd_basic_value_t, uint8_t ) );

DEFINE_REQUEST( BatteryLevelRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_battery_level_get,
                RESPOND_PARAMS( znet_cmd_battery_level_value_t ) );

DEFINE_REQUEST( BinarySwitchValueRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_binary_switch_get,
                RESPOND_PARAMS( znet_cmd_binary_switch_value_t, znet_cmd_binary_switch_value_t, uint8_t ) );

DEFINE_REQUEST( MultilevelSwitchValueRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_multilevel_switch_get,
                RESPOND_PARAMS( znet_cmd_multilevel_switch_value_t, znet_cmd_multilevel_switch_value_t, uint8_t ) );

DEFINE_REQUEST( ColorSwitchValueRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t ),
                znet_node_cmd_color_switch_get,
                RESPOND_PARAMS( const znet_color_switch_report_t& ) );

DEFINE_REQUEST( DoorLockOperationRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_door_lock_operation_get,
                RESPOND_PARAMS( const znet_door_lock_operation_report_t& ) );

DEFINE_REQUEST( DoorLockConfigurationRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_door_lock_configuration_get,
                RESPOND_PARAMS( const znet_door_lock_configuration_t&,
                                const znet_door_lock_extended_configuration_t* ) );

DEFINE_REQUEST( DoorLockCapabilitiesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_door_lock_capabilities_get,
                RESPOND_PARAMS( const znet_door_lock_capabilities_t& ) );

DEFINE_REQUEST( SoundSwitchTonesNumberRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_sound_switch_tones_number_get,
                RESPOND_PARAMS( znet_cmd_sound_switch_tones_number_t ) );

DEFINE_REQUEST( SoundSwitchToneInfoRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, znet_cmd_sound_switch_tone_id_t ),
                znet_node_cmd_sound_switch_tone_info_get,
                RESPOND_PARAMS( const znet_cmd_sound_switch_tone_info_t& ) );

DEFINE_REQUEST( SoundSwitchConfigurationRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_sound_switch_configuration_get,
                RESPOND_PARAMS( const znet_cmd_sound_switch_configuration_t& ) );

DEFINE_REQUEST( SoundSwitchTonePlayRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_sound_switch_tone_play_get,
                RESPOND_PARAMS( const znet_cmd_sound_switch_tone_play_t& ) );

DEFINE_REQUEST( ThermostatModeRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_thermostat_mode_get,
                RESPOND_PARAMS( const znet_thermostat_mode_report_t& ) );

DEFINE_REQUEST( ThermostatSetpointRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t ),
                znet_node_cmd_thermostat_setpoint_get,
                RESPOND_PARAMS( const znet_thermostat_setpoint_report_t&, bool& ) );

DEFINE_REQUEST( ThermostatAmbiguousSetpointRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, UnspecifiedUint8 ),
                znet_node_cmd_thermostat_setpoint_get,
                RESPOND_PARAMS( const znet_thermostat_setpoint_report_t&, bool& ) );

DEFINE_REQUEST( BarrierOperatorRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_barrier_operator_get,
                RESPOND_PARAMS( znet_cmd_barrier_operator_value_t ) );

DEFINE_REQUEST( ConfigurationRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t ),
                znet_node_cmd_configuration_get,
                RESPOND_PARAMS( const znet_configuration_report_t& ) );

DEFINE_REQUEST( ConfigurationBulkRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, znet_cmd_configuration_id_t, uint8_t ),
                znet_node_cmd_configuration_bulk_get,
                RESPOND_PARAMS( const znet_configuration_bulk_report_t& ) );

DEFINE_REQUEST( ConfigurationPropertiesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint16_t ),
                znet_node_cmd_configuration_properties_get,
                RESPOND_PARAMS( const znet_configuration_properties_report_t& ) );

DEFINE_REQUEST( ConfigurationUnknownPropertiesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, UnspecifiedUint16 ),
                znet_node_cmd_configuration_properties_get,
                RESPOND_PARAMS( const znet_configuration_properties_report_t& ) );

DEFINE_REQUEST( ConfigurationNameRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint16_t, UnspecifiedBool ),
                znet_node_cmd_configuration_name_get,
                RESPOND_PARAMS( const znet_configuration_name_report_t& ) );

DEFINE_REQUEST( ConfigurationInfoRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint16_t, UnspecifiedBool ),
                znet_node_cmd_configuration_info_get,
                RESPOND_PARAMS( const znet_configuration_info_report_t& ) );

DEFINE_REQUEST( FirmwareMdRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_firmware_md_get,
                RESPOND_PARAMS( const znet_firmware_md_report_v1_t&,
                                const znet_firmware_md_report_v3_t*,
                                const znet_firmware_md_report_v5_t* ) );

DEFINE_REQUEST( VersionRequest,
                REQUEST_PARAMS( znet_node_id_t ),
                znet_node_cmd_version_get,
                RESPOND_PARAMS( const znet_version_report_t& ) );

DEFINE_REQUEST( VersionCapabilitiesRequest,
                REQUEST_PARAMS( znet_node_id_t ),
                znet_node_cmd_capabilities_version_get,
                RESPOND_PARAMS( const znet_version_capabilities_report_t& ) );

DEFINE_REQUEST( SoftwareVersionRequest,
                REQUEST_PARAMS( znet_node_id_t ),
                znet_node_cmd_software_version_get,
                RESPOND_PARAMS( const znet_software_version_report_t& ) );

DEFINE_REQUEST( FirmwareUpdateMdPrepareRequest,
                REQUEST_PARAMS( znet_node_id_t,
                                znet_node_channel_id_t,
                                UnspecifiedUint16,
                                UnspecifiedUint16,
                                UnspecifiedUint8,
                                UnspecifiedUint16,
                                UnspecifiedUint8 ),
                znet_node_cmd_firmware_update_md_prepare_get,
                RESPOND_PARAMS( uint8_t, uint16_t ) );

DEFINE_REQUEST( FirmwareUpdateMdRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, UnspecifiedUint8, uint16_t ),
                znet_node_cmd_firmware_update_md_get,
                RESPOND_PARAMS( bool, uint8_t, const uint8_t* ) );

DEFINE_REQUEST( SwitchMultilevelSupportedTypesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_multilevel_switch_supported_get,
                RESPOND_PARAMS( const znet_multilevel_switch_supported_report_t& ) );

DEFINE_REQUEST( BinarySensorSupportedTypesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, UnspecifiedUint8 ),
                znet_node_cmd_sensor_binary_supported_get,
                RESPOND_PARAMS( const uint8_t*, uint8_t ) );

DEFINE_REQUEST( ColorSwitchSupportedTypesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_color_switch_supported_get,
                RESPOND_PARAMS( const znet_color_switch_supported_report_t& ) );

DEFINE_REQUEST( ThermostatSupportedModesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_thermostat_mode_supported_get,
                RESPOND_PARAMS( const znet_thermostat_mode_supported_report_t& ) );

DEFINE_REQUEST( ThermostatSuppertedOperatingStatesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_thermostat_operating_state_logging_supported_get,
                RESPOND_PARAMS( const znet_thermostat_operating_state_logging_supported_report_t& ) );

DEFINE_REQUEST( ThermostatSuppertedFanModesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_thermostat_fan_mode_supported_get,
                RESPOND_PARAMS( const znet_thermostat_fan_mode_supported_report_t& ) );

DEFINE_REQUEST( AlarmSensorSupportedTypesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_sensor_alarm_supported_get,
                RESPOND_PARAMS( const uint8_t*, uint8_t ) );

DEFINE_REQUEST( MeterSupportedTypesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_meter_supported_get,
                RESPOND_PARAMS( const znet_meter_supported_report_t& ) );

DEFINE_REQUEST( MeterRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t, UnspecifiedUint8 ),
                znet_node_cmd_meter_get,
                RESPOND_PARAMS( const znet_meter_report_t& ) );

DEFINE_REQUEST( MeterUnknownScaleRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_meter_unknown_scale_get,
                RESPOND_PARAMS( const znet_meter_report_t& ) );

DEFINE_REQUEST( MultiLevelSensorSupportedTypesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_multilevel_sensor_get_supported_sensor,
                RESPOND_PARAMS( const uint8_t*, uint8_t ) );

DEFINE_REQUEST( MultiLevelSensorSupportedScalesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t ),
                znet_node_cmd_multilevel_sensor_get_supported_scale,
                RESPOND_PARAMS( uint8_t ) );

DEFINE_REQUEST( MultiLevelSensorRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t, UnspecifiedUint8, UnspecifiedUint8 ),
                znet_node_cmd_multilevel_sensor_get,
                RESPOND_PARAMS( const znet_multilevel_sensor_report_t& ) );

DEFINE_REQUEST( MultiLevelSensorUnknownTypeRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_multilevel_sensor_unknown_type_get,
                RESPOND_PARAMS( const znet_multilevel_sensor_report_t& ) );

DEFINE_REQUEST( BarrierOperatorSupportedSignalsRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_barrier_operator_signal_supported_get,
                RESPOND_PARAMS( const uint8_t*, uint8_t ) );

DEFINE_REQUEST( BarrierOperatorSignalRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t ),
                znet_node_cmd_barrier_operator_signal_get,
                RESPOND_PARAMS( uint8_t ) );

DEFINE_REQUEST( CentralSceneConfigurationRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_central_scene_configuration_get,
                RESPOND_PARAMS( znet_cmd_central_scene_value_t ) );

DEFINE_REQUEST( CentralSceneSupportedRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_central_scene_supported_get,
                RESPOND_PARAMS( const znet_central_scene_supported_report_t& ) );

DEFINE_REQUEST( NotificationSupportedRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_notification_supported_get,
                RESPOND_PARAMS( const znet_notification_supported_report_t& ) );

DEFINE_REQUEST( NotificationEventSupportedRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t ),
                znet_node_cmd_notification_event_supported_get,
                RESPOND_PARAMS( const uint8_t*, uint8_t ) );

DEFINE_REQUEST( WakeupCapabilitiesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_wakeup_capabilities_get,
                RESPOND_PARAMS( const znet_wakeup_capabilities_report_t& ) );

DEFINE_REQUEST( CommandClassVersionRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, znet_command_class_t ),
                znet_node_cmd_command_version_get,
                RESPOND_PARAMS( znet_command_class_version_t ) );

DEFINE_REQUEST( ManufacturerSpecificRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_manufacturer_specific_get,
                RESPOND_PARAMS( const znet_manufacturer_specific_report_t& ) );

DEFINE_REQUEST( ZwavePlusInfoRequest,
                REQUEST_PARAMS( znet_node_id_t ),
                znet_node_cmd_zwaveplus_info_get,
                RESPOND_PARAMS( const znet_zwaveplus_info_report_t& ) );

DEFINE_REQUEST( SecuritySupportedRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_security_supported_get,
                RESPOND_PARAMS( const znet_security_supported_report_t* ) );

DEFINE_REQUEST( MultichannelEndpointRequest,
                REQUEST_PARAMS( znet_node_id_t ),
                znet_node_cmd_multichannel_endpoint_get,
                RESPOND_PARAMS( const znet_multichannel_endpoint_report_t& ) );

DEFINE_REQUEST( MultichannelEndpointFindRequest,
                REQUEST_PARAMS( znet_node_id_t, UnspecifiedUint8, UnspecifiedUint8, UnspecifiedBool ),
                znet_node_cmd_multichannel_endpoint_find,
                RESPOND_PARAMS( const znet_multichannel_endpoint_find_report_t& ) );

DEFINE_REQUEST( MultichannelEndpointCapabilityRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_multichannel_capability_get,
                RESPOND_PARAMS( const znet_multichannel_capability_report_t& ) );

DEFINE_REQUEST( MultichannelAggregatedMemberRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_multichannel_aggregated_members_get,
                RESPOND_PARAMS( const znet_multichannel_aggregated_members_report_t& ) );

DEFINE_REQUEST( ThermostatSetpointSupportedRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_thermostat_setpoint_supported_get,
                RESPOND_PARAMS( const znet_thermostat_setpoint_supported_report_t& ) );

DEFINE_REQUEST( ThermostatSetpointCapabilitiesRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t ),
                znet_node_cmd_thermostat_setpoint_capabilities_get,
                RESPOND_PARAMS( const znet_thermostat_setpoint_capabilities_report_t& ) );

DEFINE_REQUEST( IndicatorRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t ),
                znet_node_cmd_indicator_get,
                RESPOND_PARAMS( const znet_indicator_report_t& ) );

DEFINE_REQUEST( IndicatorRequestV1,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_indicator_get_v1,
                RESPOND_PARAMS( const znet_indicator_report_t& ) );

DEFINE_REQUEST( IndicatorSupportedRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t ),
                znet_node_cmd_indicator_supported_get,
                RESPOND_PARAMS( const znet_indicator_supported_report_t& ) );

DEFINE_REQUEST( IndicatorSupportedRequestWithUnspecifiedId,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, UnspecifiedUint8 ),
                znet_node_cmd_indicator_supported_get,
                RESPOND_PARAMS( const znet_indicator_supported_report_t& ) );

DEFINE_REQUEST( SceneActuatorConfigurationRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, znet_cmd_scene_id_t ),
                znet_node_cmd_scene_actuator_configuration_get,
                RESPOND_PARAMS( const znet_scene_actuator_configuration_report_t& ) );

DEFINE_REQUEST( ThermostatOperatingStateRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_thermostat_operating_state_get,
                RESPOND_PARAMS( uint8_t ) );

DEFINE_REQUEST( ThermostatFanModeRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_thermostat_fan_mode_get,
                RESPOND_PARAMS( uint8_t ) );

DEFINE_REQUEST( WakeupIntervalRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_wakeup_interval_get,
                RESPOND_PARAMS( uint32_t, znet_node_id_t ) );

DEFINE_REQUEST( ThermostatFanStateRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_thermostat_fan_state_get,
                RESPOND_PARAMS( uint8_t ) );

DEFINE_REQUEST( ThermostatSetbackRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_thermostat_setback_get,
                RESPOND_PARAMS( uint8_t, int8_t ) );

DEFINE_REQUEST( AlarmSensorRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t ),
                znet_node_cmd_sensor_alarm_get,
                RESPOND_PARAMS( uint8_t, uint16_t ) );

DEFINE_REQUEST( BinarySensorRequestV1,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_sensor_binary_get_v1,
                RESPOND_PARAMS( uint8_t ) );

DEFINE_REQUEST( BinarySensorRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, uint8_t ),
                znet_node_cmd_sensor_binary_get,
                RESPOND_PARAMS( uint8_t ) );

DEFINE_REQUEST( NotificationV1Request,
                REQUEST_PARAMS( znet_node_id_t,
                                znet_node_channel_id_t,
                                uint8_t,
                                UnspecifiedUint8,
                                UnspecifiedUint8,
                                UnspecifiedUint8 ),
                znet_node_cmd_notification_get,
                RESPOND_PARAMS( const znet_notification_report_t&, bool& ) );

DEFINE_REQUEST( NotificationV2Request,
                REQUEST_PARAMS( znet_node_id_t,
                                znet_node_channel_id_t,
                                UnspecifiedUint8,
                                uint8_t,
                                UnspecifiedUint8,
                                UnspecifiedUint8 ),
                znet_node_cmd_notification_get,
                RESPOND_PARAMS( const znet_notification_report_t&, bool& ) );

DEFINE_REQUEST( NodeInfoRequest,
                REQUEST_PARAMS( znet_node_id_t, UnspecifiedUint8 ),
                znet_node_request_info,
                RESPOND_PARAMS( const znet_nodeinfo_t& ) );

DEFINE_REQUEST( AssociationSupportedGroupingRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_association_supported_grouping_get,
                RESPOND_PARAMS( uint8_t ) );

DEFINE_REQUEST( AssociationRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, znet_grouping_id_t ),
                znet_node_cmd_association_get,
                RESPOND_PARAMS( const znet_association_report_t* ) );

DEFINE_REQUEST( AssociationSet,
                REQUEST_PARAMS( znet_node_id_t,
                                znet_node_channel_id_t,
                                znet_grouping_id_t,
                                UnspecifiedParam<const znet_node_id_t*>,
                                UnspecifiedParam<size_t> ),
                znet_node_cmd_association_set,
                RESPOND_PARAMS() );

DEFINE_REQUEST( AssociationGrpNameRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, znet_grouping_id_t ),
                znet_node_cmd_association_group_name_get,
                RESPOND_PARAMS( const znet_association_group_name_report_t* ) );

DEFINE_REQUEST(
    AssociationGrpInfoRequest,
    REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, znet_grouping_id_t, UnspecifiedInt, UnspecifiedInt ),
    znet_node_cmd_association_group_info_get,
    RESPOND_PARAMS( const znet_association_group_info_report_t* ) );

DEFINE_REQUEST( AssociationGrpCommandListRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, znet_grouping_id_t, UnspecifiedInt ),
                znet_node_cmd_association_group_command_list_get,
                RESPOND_PARAMS( const znet_association_group_command_list_report_t* ) );

DEFINE_REQUEST( MultiChannelAssociationSupportedGroupingRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_multichannel_association_supported_grouping_get,
                RESPOND_PARAMS( uint8_t ) );

DEFINE_REQUEST( MultiChannelAssociationRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t, znet_grouping_id_t ),
                znet_node_cmd_multichannel_association_get,
                RESPOND_PARAMS( const znet_multichannel_association_report_t* ) );

DEFINE_REQUEST( MultiChannelAssociationSet,
                REQUEST_PARAMS( znet_node_id_t,
                                znet_node_channel_id_t,
                                znet_grouping_id_t,
                                UnspecifiedParam<const znet_endpoint_t*>,
                                UnspecifiedParam<uint8_t> ),
                znet_node_cmd_multichannel_association_set,
                RESPOND_PARAMS() );

DEFINE_REQUEST( OpenBackupSessionRequest,
                REQUEST_PARAMS(),
                znet_open_backup_session,
                RESPOND_PARAMS( znet_backup_status_t, uint16_t ) );

DEFINE_REQUEST( CloseBackupSessionRequest,
                REQUEST_PARAMS(),
                znet_close_backup_session,
                RESPOND_PARAMS( znet_backup_status_t ) );

DEFINE_REQUEST( WriteBackupRequest,
                REQUEST_PARAMS( uint16_t, UnspecifiedUint8, UnspecifiedParam<const uint8_t*> ),
                znet_write_backup_data,
                RESPOND_PARAMS( znet_backup_status_t, uint16_t, uint8_t ) );

DEFINE_REQUEST( ReadBackupRequest,
                REQUEST_PARAMS( uint16_t, UnspecifiedUint8 ),
                znet_read_backup_data,
                RESPOND_PARAMS( znet_backup_status_t, uint16_t, uint8_t, const uint8_t* ) );

DEFINE_REQUEST( SetDefaultRawRequest, REQUEST_PARAMS(), znet_set_default_raw, RESPOND_PARAMS() );

DEFINE_REQUEST( AntiTheftRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_anti_theft_get,
                RESPOND_PARAMS( const znet_cmd_anti_theft_report_t& ) );

DEFINE_REQUEST( AntiTheftUnlockStateRequest,
                REQUEST_PARAMS( znet_node_id_t, znet_node_channel_id_t ),
                znet_node_cmd_anti_theft_unlock_state_get,
                RESPOND_PARAMS( const znet_cmd_anti_theft_report_t& ) );

using BaseRequestsManager = RequestsManagerImpl<UserCodesRequest,
                                                UserCodesNumberRequest,
                                                UserCodeCapabilitiesRequest,
                                                UserCodeChecksumRequest,
                                                ExtendedCodesRequest,
                                                UserCodesKeypadModeRequest,
                                                MasterCodeRequest,
                                                LocalTimeRequest,
                                                LocalDateRequest,
                                                TimeZoneRequest,
                                                TimeParametersRequest,
                                                ClockRequest,
                                                ScheduleTimeZoneRequest,
                                                ScheduleEntryLockSupportedRequest,
                                                WeekDayScheduleRequest,
                                                YearDaySchduleRequest,
                                                DailyRepeatingScheduleRequest,
                                                BasicValueRequest,
                                                BatteryLevelRequest,
                                                DoorLockOperationRequest,
                                                DoorLockConfigurationRequest,
                                                DoorLockCapabilitiesRequest,
                                                BinarySwitchValueRequest,
                                                ColorSwitchValueRequest,
                                                MultilevelSwitchValueRequest,
                                                SoundSwitchTonesNumberRequest,
                                                SoundSwitchToneInfoRequest,
                                                SoundSwitchConfigurationRequest,
                                                SoundSwitchTonePlayRequest,
                                                ThermostatModeRequest,
                                                ThermostatSetpointRequest,
                                                ThermostatAmbiguousSetpointRequest,
                                                ThermostatSetpointSupportedRequest,
                                                ThermostatSetpointCapabilitiesRequest,
                                                ConfigurationRequest,
                                                ConfigurationBulkRequest,
                                                ConfigurationPropertiesRequest,
                                                ConfigurationUnknownPropertiesRequest,
                                                ConfigurationNameRequest,
                                                ConfigurationInfoRequest,
                                                BarrierOperatorRequest,
                                                FirmwareMdRequest,
                                                VersionRequest,
                                                VersionCapabilitiesRequest,
                                                FirmwareUpdateMdPrepareRequest,
                                                FirmwareUpdateMdRequest,
                                                SoftwareVersionRequest,
                                                SwitchMultilevelSupportedTypesRequest,
                                                BinarySensorSupportedTypesRequest,
                                                ColorSwitchSupportedTypesRequest,
                                                ThermostatSupportedModesRequest,
                                                ThermostatSuppertedOperatingStatesRequest,
                                                ThermostatSuppertedFanModesRequest,
                                                AlarmSensorSupportedTypesRequest,
                                                MeterSupportedTypesRequest,
                                                MeterRequest,
                                                MeterUnknownScaleRequest,
                                                MultiLevelSensorSupportedTypesRequest,
                                                MultiLevelSensorSupportedScalesRequest,
                                                MultiLevelSensorRequest,
                                                MultiLevelSensorUnknownTypeRequest,
                                                BarrierOperatorSupportedSignalsRequest,
                                                BarrierOperatorSignalRequest,
                                                CentralSceneConfigurationRequest,
                                                CentralSceneSupportedRequest,
                                                NotificationSupportedRequest,
                                                NotificationEventSupportedRequest,
                                                WakeupCapabilitiesRequest,
                                                CommandClassVersionRequest,
                                                ManufacturerSpecificRequest,
                                                ZwavePlusInfoRequest,
                                                SecuritySupportedRequest,
                                                MultichannelEndpointRequest,
                                                MultichannelEndpointFindRequest,
                                                MultichannelEndpointCapabilityRequest,
                                                MultichannelAggregatedMemberRequest,
                                                IndicatorRequestV1,
                                                IndicatorRequest,
                                                IndicatorSupportedRequest,
                                                IndicatorSupportedRequestWithUnspecifiedId,
                                                SceneActuatorConfigurationRequest,
                                                ThermostatOperatingStateRequest,
                                                ThermostatFanModeRequest,
                                                WakeupIntervalRequest,
                                                ThermostatFanStateRequest,
                                                ThermostatSetbackRequest,
                                                AlarmSensorRequest,
                                                BinarySensorRequest,
                                                BinarySensorRequestV1,
                                                NotificationV1Request,
                                                NotificationV2Request,
                                                NodeInfoRequest,
                                                AssociationSupportedGroupingRequest,
                                                AssociationRequest,
                                                AssociationSet,
                                                AssociationGrpNameRequest,
                                                AssociationGrpInfoRequest,
                                                AssociationGrpCommandListRequest,
                                                MultiChannelAssociationSupportedGroupingRequest,
                                                MultiChannelAssociationRequest,
                                                MultiChannelAssociationSet,
                                                AntiTheftRequest,
                                                AntiTheftUnlockStateRequest,

                                                // chip requests
                                                OpenBackupSessionRequest,
                                                CloseBackupSessionRequest,
                                                WriteBackupRequest,
                                                ReadBackupRequest,
                                                SetDefaultRawRequest>;

// use inheritance instead of alias to provide convinient forward declaration
class RequestsManager : public BaseRequestsManager {
public:
    using This::This;
};

}  // namespace znet
