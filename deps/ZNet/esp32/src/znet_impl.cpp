
#include "znet/znet.h"
#include "znet/iznet.hpp"
#include "znet_impl.hpp"
#include "znet/cc_0x43_thermostat_setpoint.hpp"
#include "znet/cc_0x26_switch_multi_level.hpp"
#include "znet/cc_0x84_wakeup.hpp"
#include "znet/cc_0x8e_multi_channel_association.hpp"
#include "znet/cc_0x85_association.hpp"
#include "znet/cc_0x59_association_grp_info.hpp"
#include "znet/cc_0x63_user_code.hpp"
#include "znet/znet_provisioning_list_controller_data_source.hpp"
#include "znet/znet_requests_cb_wrapper.hpp"
#include "znet/converters/thermostat_setback_converter.hpp"

#include "znet_schedule_helper.hpp"

#include "znet/znet_provisioning_list_query.hpp"

extern "C" {
#include "logger.h"
#include "znet_log.h"
#include "znet_lib_cc_codes.h"
}

#include <sstream>
#include <ios>
#include <thread>

#ifdef ESP_PLATFORM
#include <errno.h>
#include <rom/rtc.h>
#include <esp_clk.h>
#include <esp_timer.h>
#include <soc/soc.h>
#include <soc/rtc.h>
#include <soc/rtc_cntl_reg.h>
#include <soc/frc_timer_reg.h>

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC (clockid_t)4
#endif

#endif

namespace fields = ::znet::api::filters::fields;

namespace znet {

namespace {
struct CallbackDelegate {
    static RequestsManager& getRequestsManager( void* arg ) {
        assert( arg );
        return static_cast<ZNetImpl*>( arg )->getRequestsManager();
    }

    template <typename... Args>
    static void onRequestFailReported( void* arg, znet_node_id_t nodeId, Args&&... ) {
        assert( arg );
        auto& impl = *static_cast<ZNetImpl*>( arg );
        impl.getHandler( nodeId ).onNodeFailed( nodeId, ZNET_ERR_FAULT );
        impl.sendNop( nodeId );
    }
};

struct ChipCallbackDelegate {
    static RequestsManager& getRequestsManager( void* arg ) {
        assert( arg );
        return static_cast<ZNetImpl*>( arg )->getRequestsManager();
    }

    template <typename... Args>
    static void onRequestFailReported( void* arg, Args&&... ) {}
};

/* Brief Redirects callback to requests manager
 */
#define RequestsManagerCb( Request ) ZNetCbWrapper<Request, CallbackDelegate>::cb

#define RequestsManagerChipCb( Request ) ZNetCbWrapper<Request, ChipCallbackDelegate>::cb

/* Brief Reject callbacks with error == ZNET_ERR_REQUEST_SENT_OK and null arg (ZNetImpl)
 */
#define DefaultCb( ZNetFunc ) DefaultCbWrapper<decltype( &ZNetFunc ), &ZNetFunc>::cb

/* Brief
 * 1. Redirects callback to request manager
 * 2. Wrap base callback as DefaultCb and call it
 */
#define RequestCb( Request, ZNetFunc ) CombineZNetCb( RequestsManagerCb( Request ), DefaultCb( ZNetFunc ) )

#define RequestChipCb( Request, ZNetFunc ) CombineZNetCb( RequestsManagerChipCb( Request ), ZNetFunc )

std::string userCodesToString( const znet_user_code_report_t* userCodes, uint8_t count ) {
    assert( userCodes );

    std::stringstream userCodesStream;
    userCodesStream << "  User codes (count: " << unsigned( count ) << "):\n";
    for( size_t i = 0; i < count; ++i ) {
        userCodesStream << "  -------------№ " << i + 1
                        << "\n"
                           "    user id: "
                        << unsigned( userCodes[i].user_id ) << "\n"
                        << "    user status: " << unsigned( userCodes[i].user_id_status ) << "\n"
                        << "    user code: "
                        << std::string( reinterpret_cast<const char*>( userCodes[i].user_code ),
                                        userCodes[i].user_code_size ) +
                               "\n";
    }

    return userCodesStream.str();
}

constexpr const uint8_t ReachableStateCheckIntervalFactor = 3;

}  // namespace

ZNetImpl::ZNetImpl( IZNetOwner& owner,
                    ::znet::IProvisioningListProvider& classicInclusionSecurityProvider,
                    ::znet::IProvisioningListProvider& provisioningListProvider,
                    ::znet::IProvisioningListObserver& provisioningListObserver )
: m_znetOwner( owner )
, m_classicInclusionSecurityDataSource( classicInclusionSecurityProvider )
, m_provisioningListController( m_classicInclusionSecurityDataSource,
                                provisioningListProvider,
                                provisioningListObserver )
, m_provisioningListControllerDataSource( m_provisioningListController )
, m_nodeProvisioningManager( m_provisioningListController )
, m_emptyHandler( *this )
, m_interviewHandler( *this )
, m_excludeHandler( *this )
, m_learnHandler( *this )
, m_stopLearnHandler( *this )
, m_currentValuesHandler( *this )
, m_currentValuesExtendedHandler( *this )
, m_monitoringHandler( *this )
, m_inclusionHandler( *this )
, m_inclusionReplaceHandler( *this )
, m_timeSynchronizationHandler( *this )
, m_discoverHandler( *this )
, m_firmwareUpdateHandler( *this )
, m_nodeCheckHandler( *this )
, m_backupCreationHandler( *this )
, m_backupRestoringHandler( *this )
, m_timeManager( owner, owner, owner )
, m_lastNodeStatusCheckingTime( std::chrono::steady_clock::now() )
, m_requestsManager( owner )
, m_ccValuesSetter( owner, owner ) {
    m_znet_cb = {
        .arg = this,
        .alloc = znetAlloc,
        .clock = znetClock,
        .log = znetLog,
        .uart_write = znetUartWrite,
        .uart_read = znetUartRead,
        .uart_reset = nullptr,
        .store_save = znetStoreSave,
        .store_load = znetStoreLoad,
        .store_reset = znetStoreReset,

        .init_finish = znetInitFinish,
        .set_default_started = znetModuleResetStarted,
        .set_default = DefaultCb( znetModuleReset ),
        .set_default_raw_result = RequestsManagerChipCb( SetDefaultRawRequest ),
        .network_update = nullptr,
        .request_neighbor_result = DefaultCb( znetRequestNeighborCB ),
        .private_s2_dsk = nullptr,
        .node_add_result = DefaultCb( znetNodeAdd ),
        .node_add_security_failed = znetNodeAddSecurityFailed,
        .node_add_invoked = znetNodeAddInvoked,
        .node_add_break_result = DefaultCb( znetNodeAddBreak ),
        .node_rem_result = DefaultCb( znetNodeRem ),
        .node_rem_break_result = DefaultCb( znetNodeRemBreak ),
        .node_rem_invoked = znetNodeRemInvoked,
        .node_learn_invoked = znetNodeLearnInvoked,
        .node_replace_if_failed_result = nullptr,
        .node_learn_result = DefaultCb( znetNodeLearn ),
        .node_learn_break_result = DefaultCb( znetNodeLearnBreak ),
        .node_list_result = DefaultCb( znetNodeList ),
        .node_update_info_result = RequestCb( NodeInfoRequest, znetApplicationNodeUpdate ),
        .node_s2_select_keys = znetS2SelectKeys,
        .node_s2_request_pub_key = znetS2RequestPubKey,
        .node_s2_show_pub_key = znetS2ShowPubKey,
        .node_inif_received = znetNodeInifReceived,
        .node_nhid_received = znetNodeNhidReceived,
        .reset_zwave_chip = hardResetZwaveChip,
        .sis_updated = sisUpdated,
        .library_info = libraryInfoUpdated,
        .forced_request_result = DefaultCb( znetForcedRequest ),
        .check_node_supports_command_class = znetCheckNodeSupportsCommandClass,
        .node_cmd_version_result = RequestCb( VersionRequest, znetNodeCmdVersion ),
        .node_cmd_command_version_result = RequestCb( CommandClassVersionRequest, znetNodeCmdCommandVersion ),
        .node_cmd_version_capabilities_result = RequestCb( VersionCapabilitiesRequest, znetNodeCmdVersionCapabilities ),
        .node_cmd_software_version_result = RequestCb( SoftwareVersionRequest, znetNodeCmdSoftwareVersion ),
        .node_cmd_version_fill = znetNodeCmdVersionFill,
        .node_cmd_manufacturer_specific_result =
            RequestCb( ManufacturerSpecificRequest, znetNodeCmdManufacturerSpecific ),
        .node_cmd_manufacturer_specific_fill = znetNodeCmdManufacturerSpecificFill,
        .node_cmd_device_specific_result = DefaultCb( znetNodeCmdDeviceSpecific ),
        .node_cmd_zwaveplus_info_result = RequestCb( ZwavePlusInfoRequest, znetNodeCmdZWavePlusInfo ),
        .node_cmd_zwaveplus_info_fill = znetNodeCmdZWavePlusFill,
        .node_cmd_firmware_update_info_fill = znetNodeCmdFirmwareUpdateInfoFill,
        .node_cmd_basic_result = RequestCb( BasicValueRequest, znetNodeCmdBasic ),
        .node_cmd_basic_set_result = DefaultCb( znetNodeCmdBasicSet ),
        .node_cmd_battery_level_result = RequestCb( BatteryLevelRequest, znetNodeCmdBatteryLevel ),
        .node_cmd_binary_switch_result = RequestCb( BinarySwitchValueRequest, znetNodeCmdBinarySwitch ),
        .node_cmd_clock_result = RequestCb( ClockRequest, znetNodeCmdClock ),
        .node_cmd_time_parameters_result = RequestCb( TimeParametersRequest, znetNodeCmdTimeParameters ),
        .node_cmd_door_lock_operation_result = RequestCb( DoorLockOperationRequest, znetNodeCmdDoorLockOperation ),
        .node_cmd_door_lock_configuration_result =
            RequestCb( DoorLockConfigurationRequest, znetNodeCmdDoorLockConfiguration ),
        .node_cmd_door_lock_capabilities_result =
            RequestCb( DoorLockCapabilitiesRequest, znetNodeCmdDoorLockCapabilities ),
        .node_cmd_user_code_report = znetNodeCmdUserCodeReport,  // Not using wrappers here, handling inside
        .node_cmd_users_number_report = RequestCb( UserCodesNumberRequest, znetNodeCmdUsersNumberReport ),
        .node_cmd_entry_control_notification = nullptr,
        .node_cmd_entry_control_key_supported_report = nullptr,
        .node_cmd_entry_control_event_supported_report = nullptr,
        .node_cmd_entry_control_configuration_report = nullptr,
        .node_cmd_notification_result =
            []( znet_error_t err,
                znet_node_id_t node_id,
                znet_node_channel_id_t channel_id,
                uint8_t v1AlarmType,
                uint8_t notificationType,
                uint8_t eventType,
                const znet_notification_report_t* value,
                void* arg ) {
                bool requestedNotification = false;
                RequestsManagerCb( NotificationV1Request )(
                    err, node_id, channel_id, v1AlarmType, value, &requestedNotification, arg );
                RequestsManagerCb( NotificationV2Request )(
                    err, node_id, channel_id, notificationType, value, &requestedNotification, arg );
                DefaultCb( znetNodeCmdNotification )( err, node_id, channel_id, value, requestedNotification, arg );
            },
        .node_cmd_notification_supported_report =
            RequestCb( NotificationSupportedRequest, znetNodeCmdNotificationSupported ),
        .node_cmd_notification_event_supported_report =
            RequestCb( NotificationEventSupportedRequest, znetNodeCmdNotificationEventSupported ),
        .node_cmd_hail_result = DefaultCb( znetNodeCmdHail ),
        .node_cmd_meter_result =
            []( znet_error_t err,
                znet_node_id_t node_id,
                znet_node_channel_id_t channel_id,
                uint8_t scale,
                const znet_meter_report_t* value,
                void* arg ) {
                RequestsManagerCb( MeterUnknownScaleRequest )( err, node_id, channel_id, value, arg );
                RequestCb( MeterRequest, znetNodeCmdMeter )( err, node_id, channel_id, scale, value, arg );
            },
        .node_cmd_meter_set_done = DefaultCb( znetNodeCmdMeterSetDone ),
        .node_cmd_meter_supported_result = RequestCb( MeterSupportedTypesRequest, znetNodeCmdMeterSupported ),
        .node_cmd_multilevel_sensor_get_supported_sensor_result =
            RequestCb( MultiLevelSensorSupportedTypesRequest, znetNodeCmdSensorMultilevelSupportedSensor ),
        .node_cmd_multilevel_sensor_get_supported_scale_result =
            RequestCb( MultiLevelSensorSupportedScalesRequest, znetNodeCmdSensorMultilevelSupportedScale ),
        .node_cmd_multilevel_sensor_get_result =
            []( znet_error_t err,
                znet_node_id_t nodeId,
                znet_node_channel_id_t channelId,
                uint8_t type,
                uint8_t scale,
                const znet_multilevel_sensor_report_t* value,
                void* arg ) {
                RequestsManagerCb( MultiLevelSensorUnknownTypeRequest )( err, nodeId, channelId, value, arg );
                RequestCb( MultiLevelSensorRequest, znetNodeCmdSensorMultilevelGetReport )(
                    err, nodeId, channelId, type, value, arg );
            },
        .node_cmd_multilevel_switch_result = RequestCb( MultilevelSwitchValueRequest, znetNodeCmdMultilevelSwitch ),
        .node_cmd_multilevel_switch_set_done = DefaultCb( znetNodeCmdMultilevelSwitchSetDone ),
        .node_cmd_multilevel_switch_supported_result =
            RequestCb( SwitchMultilevelSupportedTypesRequest, znetNodeCmdMultilevelSwitchSupported ),
        .node_cmd_barrier_operator_result = RequestCb( BarrierOperatorRequest, znetNodeCmdBarrierOperator ),
        .node_cmd_barrier_operator_signal_result =
            RequestCb( BarrierOperatorSignalRequest, znetNodeCmdBarrierOperatorSignal ),
        .node_cmd_barrier_operator_set_done = DefaultCb( znetNodeCmdBarrierOperatorSetDone ),
        .node_cmd_barrier_operator_supported_signals_result =
            RequestCb( BarrierOperatorSupportedSignalsRequest, znetNodeCmdBarrierOperatorSupportedSignals ),
        .node_cmd_multichannel_endpoint_result =
            RequestCb( MultichannelEndpointRequest, znetNodeCmdMultichannelEndpoint ),
        .node_cmd_multichannel_capability_result =
            RequestCb( MultichannelEndpointCapabilityRequest, znetNodeCmdMultichannelCapability ),
        .node_cmd_multichannel_endpoint_find_result =
            RequestCb( MultichannelEndpointFindRequest, znetNodeCmdMultichannelEndpointFind ),
        .node_cmd_multichannel_aggregated_members_result =
            RequestCb( MultichannelAggregatedMemberRequest, znetNodeCmdMultichannelAggregatedMembers ),
        .node_cmd_configuration_result = RequestCb( ConfigurationRequest, znetNodeCmdConfiguration ),
        .node_cmd_configuration_request_status = znetNodeCmdConfigurationRequestStatus,
        .node_cmd_configuration_bulk_result = RequestCb( ConfigurationBulkRequest, znetNodeCmdConfigurationBulk ),
        .node_cmd_configuration_name_result = RequestCb( ConfigurationNameRequest, znetNodeCmdConfigurationName ),
        .node_cmd_configuration_info_result = RequestCb( ConfigurationInfoRequest, znetNodeCmdConfigurationInfo ),
        .node_cmd_configuration_properties_result =
            []( znet_error_t err,
                znet_node_id_t nodeId,
                znet_node_channel_id_t channelId,
                znet_cmd_configuration_id_t paramNumber,
                const znet_configuration_properties_report_t* value,
                void* arg ) {
                RequestsManagerCb( ConfigurationUnknownPropertiesRequest )( err, nodeId, channelId, value, arg );
                RequestCb( ConfigurationPropertiesRequest, znetNodeCmdConfigurationProperties )(
                    err, nodeId, channelId, paramNumber, value, arg );
            },
        .node_cmd_alarm_sensor_get_result = RequestCb( AlarmSensorRequest, znetNodeCmdAlarmSensorGetReport ),
        .node_cmd_alarm_sensor_get_supported_sensor_result =
            RequestCb( AlarmSensorSupportedTypesRequest, znetNodeCmdAlarmSensorSupportedSensor ),
        .node_cmd_binary_sensor_get_result =
            []( znet_error_t err,
                znet_node_id_t nodeId,
                znet_node_channel_id_t channelId,
                uint8_t sensorType,
                uint8_t value,
                void* arg ) {
                RequestsManagerCb( BinarySensorRequestV1 )( err, nodeId, channelId, value, arg );
                RequestCb( BinarySensorRequest, znetNodeCmdBinarySensorGetReport )(
                    err, nodeId, channelId, sensorType, value, arg );
            },
        .node_cmd_binary_sensor_get_supported_sensor_result =
            RequestCb( BinarySensorSupportedTypesRequest, znetNodeCmdBinarySensorSupportedSensor ),
        .node_cmd_color_switch_result = RequestCb( ColorSwitchValueRequest, znetNodeCmdColorSwitch ),
        .node_cmd_color_switch_supported_result =
            RequestCb( ColorSwitchSupportedTypesRequest, znetNodeCmdColorSwitchSupported ),
        .node_cmd_powerlevel_result = DefaultCb( znetNodeCmdPowerlevel ),
        .node_cmd_powerlevel_test_node_result = DefaultCb( znetNodeCmdPowerlevelTestNode ),
        .node_cmd_scene_actuator_configuration_result =
            RequestCb( SceneActuatorConfigurationRequest, znetNodeCmdSceneActuatorConfiguration ),
        .node_cmd_scene_controller_configuration_result = DefaultCb( znetNodeCmdSceneControllerConfiguration ),
        .node_cmd_central_scene_configuration_result =
            RequestCb( CentralSceneConfigurationRequest, znetNodeCmdCentralSceneConfiguration ),
        .node_cmd_central_scene_supported_result =
            RequestCb( CentralSceneSupportedRequest, znetNodeCmdCentralSceneSupported ),
        .node_cmd_central_scene_notification = DefaultCb( znetCentralSceneNotification ),
        .node_cmd_scene_activation_set_result = DefaultCb( znetSceneActivationSetResult ),
        .node_cmd_device_reset_localy_result = DefaultCb( znetNodeResetLocaly ),
        .node_cmd_inclusion_controller_initiate = DefaultCb( znetInclusionControllerInitiate ),
        .node_cmd_inclusion_controller_complete = DefaultCb( znetInclusionControllerComplete ),
        .node_cmd_node_provisioning_set = DefaultCb( znetNodeProvisioningSetReceived ),
        .node_cmd_node_provisioning_delete = DefaultCb( znetNodeProvisioningDeleteReceived ),
        .node_cmd_node_provisioning_get = DefaultCb( znetNodeProvisioningGetReceived ),
        .node_cmd_node_provisioning_list_iteration_get = DefaultCb( znetNodeProvisioningListIterationGetReceived ),
        .node_cmd_security_supported_result = RequestCb( SecuritySupportedRequest, znetNodeCmdSecuritySupported ),
        .node_cmd_association_result = RequestCb( AssociationRequest, znetAssociationResult ),
        .node_cmd_association_set_done = RequestCb( AssociationSet, znetAssociationSetResult ),
        .node_cmd_association_remove_done = DefaultCb( znetAssociationRemoveResult ),
        .node_cmd_association_supported_grouping_result =
            RequestCb( AssociationSupportedGroupingRequest, znetAssociationSupportedGroupingResult ),
        .node_cmd_association_specific_group_result = nullptr,
        .node_cmd_association_group_name_result =
            RequestCb( AssociationGrpNameRequest, znetAssociationGroupNameResult ),
        .node_cmd_association_group_info_result =
            RequestCb( AssociationGrpInfoRequest, znetAssociationGroupInfoResult ),
        .node_cmd_association_group_command_list_result =
            RequestCb( AssociationGrpCommandListRequest, znetAssociationGroupCommandListResult ),
        .node_cmd_multichannel_association_result =
            RequestCb( MultiChannelAssociationRequest, znetMultichannelAssociationResult ),
        .node_cmd_multichannel_association_set_done =
            RequestCb( MultiChannelAssociationSet, znetMultichannelAssociationSetResult ),
        .node_cmd_multichannel_association_remove_done = DefaultCb( znetMultichannelAssociationRemoveResult ),
        .node_cmd_multichannel_association_supported_grouping_result = RequestCb(
            MultiChannelAssociationSupportedGroupingRequest, znetMultichannelAssociationSupportedGroupingResult ),
        .node_cmd_simple_av_control_set_result = znetSimpleAvControlSetResult,
        .node_cmd_no_operation_result = znetNodeNoOperation,
        .node_cmd_wakeup_result = RequestCb( WakeupIntervalRequest, znetWakeupResult ),
        .node_cmd_wakeup_notification = DefaultCb( znetWakeupNotification ),
        .node_cmd_wakeup_capabilities_result = RequestCb( WakeupCapabilitiesRequest, znetWakeupCapabilitiesResult ),
        .node_cmd_thermostat_mode_result = RequestCb( ThermostatModeRequest, znetThermostatModeResult ),
        .node_cmd_thermostat_mode_supported_result =
            RequestCb( ThermostatSupportedModesRequest, znetThermostatModeSupportedResult ),
        .node_cmd_thermostat_operating_state_result =
            RequestCb( ThermostatOperatingStateRequest, znetThermostatOperatingStateResult ),
        .node_cmd_thermostat_operating_state_logging_supported_result =
            RequestCb( ThermostatSuppertedOperatingStatesRequest, znetThermostatOperatingStateLoggingSupportedResult ),
        .node_cmd_thermostat_operating_state_logging_result = DefaultCb( znetThermostatOperatingStateLoggingResult ),
        .node_cmd_thermostat_fan_mode_result = RequestCb( ThermostatFanModeRequest, znetThermostatFanModeResult ),
        .node_cmd_thermostat_fan_mode_supported_result =
            RequestCb( ThermostatSuppertedFanModesRequest, znetThermostatFanModeSupportedResult ),
        .node_cmd_thermostat_fan_state_result = RequestCb( ThermostatFanStateRequest, znetThermostatFanStateResult ),
        .node_cmd_thermostat_setback_result = RequestCb( ThermostatSetbackRequest, znetThermostatSetbackResult ),
        .node_cmd_thermostat_setpoint_result =
            []( int err,
                znet_node_id_t nodeId,
                znet_node_channel_id_t channelId,
                uint8_t type,
                const znet_thermostat_setpoint_report_t* value,
                void* arg ) {
                bool requested = false;
                RequestsManagerCb( ThermostatAmbiguousSetpointRequest )(
                    err, nodeId, channelId, value, &requested, arg );
                RequestsManagerCb( ThermostatSetpointRequest )( err, nodeId, channelId, type, value, &requested, arg );
                DefaultCb( znetThermostatSetpointResult )( err, nodeId, channelId, type, value, requested, arg );
            },
        .node_cmd_thermostat_setpoint_supported_result =
            RequestCb( ThermostatSetpointSupportedRequest, znetThermostatSetpointSupportedResult ),
        .node_cmd_thermostat_setpoint_capabilities_result =
            RequestCb( ThermostatSetpointCapabilitiesRequest, znetThermostatSetpointCapabilitiesResult ),
        .node_cmd_zwave_plus_info_get = znetZwavePlusInfoGet,
        .node_cmd_indicator_identify = znetIndicatorIdentify,
        .node_cmd_indicator_result =
            []( znet_error_t err,
                znet_node_id_t nodeId,
                znet_node_channel_id_t channelId,
                uint8_t indicatorId,
                const znet_indicator_report_t* value,
                void* arg ) {
                RequestsManagerCb( IndicatorRequestV1 )( err, nodeId, channelId, value, arg );
                RequestCb( IndicatorRequest, znetIndicatorReport )( err, nodeId, channelId, indicatorId, value, arg );
            },
        .node_cmd_indicator_supported_result =
            []( znet_error_t err,
                znet_node_id_t nodeId,
                znet_node_channel_id_t channelId,
                uint8_t indicatorId,
                const znet_indicator_supported_report_t* value,
                void* arg ) {
                RequestsManagerCb( IndicatorSupportedRequestWithUnspecifiedId )( err, nodeId, channelId, value, arg );
                RequestCb( IndicatorSupportedRequest, znetIndicatorSupportedReport )(
                    err, nodeId, channelId, indicatorId, value, arg );
            },
        .node_cmd_is_added = znetNodeIsAdded,
        .node_cmd_security_get = znetSecurityGet,
        .node_cmd_security_set = znetSecuritySet,
        .node_cmd_is_supported_cc = znetNodeIsSupportedCC,
        .node_cmd_is_wakeup = znetNodeIsWakeup,
        .node_cmd_time_requested = znetNodeTimeRequested,
        .node_cmd_date_requested = znetNodeDateRequested,
        .node_cmd_time_zone_requested = znetNodeTimeZoneRequested,
        .node_cmd_time_zone_set = znetNodeTimeZoneSeted,
        .node_cmd_time_zone_result = RequestCb( TimeZoneRequest, znetNodeTimeZoneReport ),
        .node_cmd_local_date_result = RequestCb( LocalDateRequest, znetNodeLocalDateReport ),
        .node_cmd_local_time_result = RequestCb( LocalTimeRequest, znetNodeLocalTimeReport ),
        .node_schedule_lock_entry_time_offset_result =
            RequestCb( ScheduleTimeZoneRequest, znetNodeScheduleLockEntryTimeOffsetReport ),
        .node_schedule_lock_entry_week_day_result =
            RequestCb( WeekDayScheduleRequest, znetNodeScheduleLockEntryWeekDayReport ),
        .node_schedule_lock_entry_year_day_result =
            RequestCb( YearDaySchduleRequest, znetNodeScheduleLockYearDayReport ),
        .node_schedule_lock_entry_supported_result =
            RequestCb( ScheduleEntryLockSupportedRequest, znetNodeScheduleLockEntrySupportedReport ),
        .node_schedule_lock_entry_daily_repeating_result =
            RequestCb( DailyRepeatingScheduleRequest, znetNodeScheduleLockEntryRepeatingReport ),
        .node_cmd_sound_switch_tones_number_result =
            RequestCb( SoundSwitchTonesNumberRequest, znetNodeSoundSwitchTonesNumberReport ),
        .node_cmd_sound_switch_tone_info_result =
            RequestCb( SoundSwitchToneInfoRequest, znetNodeSoundSwitchToneInfoReport ),
        .node_cmd_sound_switch_configuration_result =
            RequestCb( SoundSwitchConfigurationRequest, znetNodeSoundSwitchConfigurationReport ),
        .node_cmd_sound_switch_tone_play_result =
            RequestCb( SoundSwitchTonePlayRequest, znetNodeSoundSwitchTonePlayReport ),
        .node_cmd_user_code_capabilities_result =
            RequestCb( UserCodeCapabilitiesRequest, znetNodeUserCodeCapabilitiesReport ),
        .node_cmd_user_codes_keypad_mode_result =
            RequestCb( UserCodesKeypadModeRequest, znetNodeUserCodesKeypadModeReport ),
        .node_cmd_extended_user_code_result =
            znetNodeExtendedUserCodeReport,  // Not using wrappers here, handling inside
        .node_cmd_user_code_checksum_result = RequestCb( UserCodeChecksumRequest, znetNodeUserCodeChecksumReport ),
        .node_cmd_master_code_result = RequestCb( MasterCodeRequest, znetNodeMasterCodeReport ),
        .node_cmd_firmware_md_result = RequestCb( FirmwareMdRequest, znetNodeFirmwareMdReport ),
        .node_cmd_firmware_update_md_request_result = DefaultCb( znetNodeFirmwareUpdateMdRequestReport ),
        .node_cmd_firmware_update_md_requested = DefaultCb( znetNodeFirmwareUpdateMdRequested ),
        .node_cmd_firmware_update_md_result = RequestCb( FirmwareUpdateMdRequest, znetNodeFirmwareUpdateMdReport ),
        .node_cmd_firmware_update_md_status_result = DefaultCb( znetNodeFirmwareUpdateMdStatusReport ),
        .node_cmd_firmware_update_md_activation_status_result =
            DefaultCb( znetNodeFirmwareUpdateMdActivationStatusReport ),
        .node_cmd_firmware_update_md_preparation_result =
            RequestCb( FirmwareUpdateMdPrepareRequest, znetNodeFirmwareUpdateMdPreparationReport ),
        .backup_session_open_result = RequestChipCb( OpenBackupSessionRequest, znetBackupSessionOpenReport ),
        .backup_session_close_result = RequestChipCb( CloseBackupSessionRequest, znetBackupSessionCloseReport ),
        .backup_read_result = RequestChipCb( ReadBackupRequest, znetBackupReadReport ),
        .backup_write_result = RequestChipCb( WriteBackupRequest, znetBackupWriteReport ),
        .node_cmd_anti_theft_result = RequestCb( AntiTheftRequest, znetAntiTheftReport ),
        .node_cmd_anti_theft_unlock_state_result =
            RequestCb( AntiTheftUnlockStateRequest, znetAntiTheftUnlockStateReport ),
        .get_known_S0_key = znetGetKnownS0Key
        /* TODO: */
    };
    setState( IZNet::Mode::Initializing, ModeStatus::Invoked );
}

void ZNetImpl::resetZwaveModule( bool sendDeviceResetLocallyNotifications ) {
    setState( IZNet::Mode::Reseting, ModeStatus::Invoked );
    ZNET_LOGI( "%s, sendDeviceResetLocallyNotifications %s\n",
               __func__,
               utils::boolToYesNo( sendDeviceResetLocallyNotifications ) );
    activateMode( IZNet::Mode::Reseting );

    if( sendDeviceResetLocallyNotifications ) {
        znet_send_device_reset_locally_notifications( [] { znet_set_default(); } );
    }
    else {
        znet_set_default();
    }
}

void ZNetImpl::initZnet() {
    setState( IZNet::Mode::Initializing, ModeStatus::Invoked );
}

void ZNetImpl::startExclude( uint8_t nodeId ) {
    if( isBusy() ) {
        getOwner().onUpdateState( m_state.m_cmd, ModeStatus::Busy );
        return;
    }

    if( nodeId == 0 ) {
        activateMode( IZNet::Mode::ExcludeStarting );
        getHandler().startExclude();
    }
    else if( ZNode* node = onFindNode( nodeId ) ) {
        activateMode( IZNet::Mode::ExcludeStarting );
        getHandler().startExclude( *node );
    }
    else {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        getOwner().nodeRemove( nodeId );
        getOwner().onUpdateState( IZNet::Mode::ExcludeStarting, ModeStatus::Failed );
    }
}

void ZNetImpl::startInclude() {
    if( isBusy() ) {
        getOwner().onUpdateState( m_state.m_cmd, ModeStatus::Busy );
        return;
    }

    activateMode( IZNet::Mode::IncludeStarting );

    auto errCode = ::znet_node_add();
    if( ZNET_ERR_OK == errCode ) {
        setState( IZNet::Mode::IncludeStarting, ModeStatus::InProgress );
    }
    else {
        setState( IZNet::Mode::IncludeStarting, ModeStatus::Failed, err::message( errCode ) );
    }
}

void ZNetImpl::startLearn() {
    if( isBusy() ) {
        getOwner().onUpdateState( m_state.m_cmd, ModeStatus::Busy );
        return;
    }

    if( znet_get_self_node_id() != 1 && znet_get_self_node_id() == znet_get_sis_node_id() ) {
        // Do not start learn mode, when controller is not primary, but SIS (RT.1.5.1.2.1)
        setState( IZNet::Mode::LearnStarting, ModeStatus::Failed, err::message( ZNET_ERR_NOT_SIS ) );
        return;
    }

    activateMode( IZNet::Mode::LearnStarting );

    if( ZNET_ERR_OK != znet_node_learn() ) {
        setState( IZNet::Mode::LearnStarting, ModeStatus::Failed );
    }
}

void ZNetImpl::stopExclude() {
    if( m_state.m_cmd == IZNet::Mode::Idle && m_state.m_cmdStatus == ModeStatus::Empty ) {
        return;
    }

    if( m_state.m_cmd == IZNet::Mode::ExcludeStarting &&
        ( m_state.m_cmdStatus == ModeStatus::Invoked || m_state.m_cmdStatus == ModeStatus::InProgress ||
          m_state.m_cmdStatus == ModeStatus::Started ) ) {
        getHandler().stopExclude();
    }
    else {
        if( m_state.m_cmd == IZNet::Mode::ExcludeStarting || m_state.m_cmd == IZNet::Mode::ExcludeStopping ) {
            setState( IZNet::Mode::ExcludeStopping, ModeStatus::Failed );
            activateMode(::znet::IZNet::Mode::Monitoring );
        }
        else {
            getOwner().onUpdateState( IZNet::Mode::ExcludeStopping, ModeStatus::Failed );
        }
    }
}

void ZNetImpl::stopInclude() {
    if( m_state.m_cmd == IZNet::Mode::Idle && m_state.m_cmdStatus == ModeStatus::Empty ) {
        return;
    }

    if( m_state.m_cmd == IZNet::Mode::IncludeStarting &&
        ( m_state.m_cmdStatus == ModeStatus::Invoked || m_state.m_cmdStatus == ModeStatus::InProgress ||
          m_state.m_cmdStatus == ModeStatus::Started ) ) {
        getHandler().stopInclude();
    }
    else {
        if( m_state.m_cmd == IZNet::Mode::IncludeStarting || m_state.m_cmd == IZNet::Mode::IncludeStopping ) {
            setState( IZNet::Mode::IncludeStopping, ModeStatus::Failed );
            activateMode(::znet::IZNet::Mode::Monitoring );
        }
        else {
            getOwner().onUpdateState( IZNet::Mode::IncludeStopping, ModeStatus::Failed );
        }
    }
}

void ZNetImpl::stopLearn() {
    if( m_state.m_cmd == IZNet::Mode::LearnStarting &&
        ( m_state.m_cmdStatus == ModeStatus::Invoked || m_state.m_cmdStatus == ModeStatus::InProgress ||
          m_state.m_cmdStatus == ModeStatus::Started ) ) {
        activateMode( IZNet::Mode::LearnStopping );
        getHandler().stopLearn();
    }
    else {
        getOwner().onUpdateState( IZNet::Mode::LearnStopping, ModeStatus::Failed );
    }
}

void ZNetImpl::stopSecurityInclude() {
    if( m_state.m_cmd == IZNet::Mode::IncludeStarting &&
        ( m_state.m_cmdStatus == ModeStatus::Invoked || m_state.m_cmdStatus == ModeStatus::InProgress ||
          m_state.m_cmdStatus == ModeStatus::Started ) ) {
        znet_node_sec2_abort_join();
    }
    else {
        ZNET_LOGE( "try to stop security inclusion in non-inclusion state" );
    }
}

void ZNetImpl::sendNif() {
    znet_send_node_info();
}

bool ZNetImpl::requestNif( uint8_t nodeId ) {
    if( isBusy() ) {
        getOwner().onUpdateState( m_state.m_cmd, ModeStatus::Busy );
        return false;
    }

    return ZNET_ERR_OK == znet_node_request_info( nodeId, 0 );
}

bool ZNetImpl::sendNop( uint8_t nodeId ) {
    if( auto node = onFindNode( nodeId ) ) {
        if( node->isWakeup() )
            return true;
    }
    return znet_node_cmd_no_operation( nodeId ) == ZNET_ERR_OK;
}

void ZNetImpl::znetRequestNeighborCB( int err, znet_node_id_t node_id, void* arg ) {
    ZNET_LOGI( "%s node %d\n", __func__, node_id );
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );

    if( ptr == nullptr ) {
        return;
    }

    if( err != ZNET_ERR_OK ) {
        ptr->updateReconstructNetworkError( node_id );
    }

    while( node_id < ZNET_NODE_ID_MAX ) {
        node_id++;
        if( auto node = ptr->onFindNode( node_id ) ) {
            znet_error_t request_status = znet_request_node_neighbor( node_id );
            if( ZNET_ERR_OK == request_status || ZNET_ERR_INPROGR == request_status ) {
                ptr->updateReconstructNetworkProgress( ptr->m_devicesAmount, ++ptr->m_currentDevice );
            }
            else {
                ptr->setState( IZNet::Mode::RequestNeighbor, ModeStatus::Failed );
            }
            // Proceed to the next node immediately if only the request was postponed. In all other cases exit.
            if( request_status != ZNET_ERR_INPROGR ) {
                return;
            }
        }
    }
    if( node_id >= ZNET_NODE_ID_MAX ) {
        ptr->setState( IZNet::Mode::RequestNeighbor, ModeStatus::Finished );
    }
}

void ZNetImpl::reconstructNetwork() {
    ZNET_LOGD( "%s \n", __func__ );

    if( isBusy() ) {
        getOwner().onUpdateState( m_state.m_cmd, ModeStatus::Busy );
        return;
    }

    m_devicesAmount = ( znet::utils::getBitsCount( getOwner().getNodeIds().mask, getOwner().getNodeIds().Size ) );
    m_currentDevice = 0;
    setState( IZNet::Mode::RequestNeighbor, ModeStatus::Started );

    znetRequestNeighborCB( ZNET_ERR_OK, 1, this );
}

void ZNetImpl::getBackgroundRSSI() {
    if( isBusy() ) {
        getOwner().onUpdateState( m_state.m_cmd, ModeStatus::Busy );
        return;
    }
    znet_get_background_RSSI();
}

void ZNetImpl::setPublicKey( znet_S2_pub_key_t* pubKey ) {
    if( ( pubKey->key[0] == 0 ) && ( pubKey->key[1] == 0 ) ) {
        ZNET_LOGI( "Entered S2 key = 0! \n" );
    }
    getHandler().onNodeAddSecurityPubKeyGranted( pubKey );
    znet_node_s2_response_pub_key( pubKey );
}

void ZNetImpl::s2SelectKeys( znet_S2_keys_t selected_keys ) {
    getHandler().onNodeAddSecurityKeysGranted( selected_keys );
    znet_node_s2_select_keys( selected_keys );
}

znet::ZNetHandler& ZNetImpl::getHandler( const znet_node_id_t nodeId ) {
    const auto canHandle = m_cbHandler.get().canHandleNode( nodeId );
    if( !canHandle ) {
        ZNET_LOGI( "Cant handle node %i, use monitoring handler instead\n", nodeId );
    }
    return canHandle ? m_cbHandler : m_monitoringHandler;
}

ZNode* ZNetImpl::onFindNode( znet_node_id_t nodeId ) {
    return getOwner().onFindNode( nodeId );
}

bool ZNetImpl::onLoop() {
    if( checkState( IZNet::Mode::Initializing, ModeStatus::Invoked ) ) {
        hardResetZwaveChip( this );
        setState( IZNet::Mode::Initializing, ModeStatus::Started );
        synchronizeAllNodesTime();
        setupDstTimer();
        initWakupNodesState();
    }
    else {
        znet_proc();

        // TODO: move to MonitoringBaseHandler::onLoop
        if( &m_cbHandler.get() == &m_monitoringHandler ) {
            // TODO: implement possibility parallel working on few handlers
            if( m_backupCreationHandler.isDirty() ) {
                activateMode( IZNet::Mode::BackupCreation );
            }
            else if( m_backupRestoringHandler.isDirty() ) {
                activateMode( IZNet::Mode::BackupRestoring );
            }
            else if( m_firmwareUpdateHandler.isDirty() ) {
                activateMode( IZNet::Mode::FirmwareUpdate );
            }
            else if( m_discoverHandler.isDirty() ) {
                activateMode( IZNet::Mode::Rediscover );
            }
            else if( getTimeManager().isDirty() ) {
                activateMode( IZNet::Mode::TimeSynchronization );
            }
            else if( m_currentValuesExtendedHandler.isDirty() ) {
                activateMode( IZNet::Mode::ValuesGettingExtended );
            }
            else if( m_currentValuesHandler.isDirty() ) {
                activateMode( IZNet::Mode::ValuesGetting );
            }
            else if( m_nodeCheckHandler.isDirty() ) {
                activateMode( IZNet::Mode::NodeChecking );
            }
        }

        m_cbHandler.get().onLoop();
    }

    return true;
}

void ZNetImpl::requestAllNodesValues( bool extended ) {
    if( extended ) {
        m_currentValuesExtendedHandler.addAllNodesToProcess();
        m_currentValuesHandler.removeNodesFromProcess( m_currentValuesExtendedHandler.getProcessingNodeIds() );
    }
    else {
        m_currentValuesHandler.addAllNodesToProcess();
    }
}

void ZNetImpl::requestNodeValues( uint8_t nodeId,
                                  std::chrono::milliseconds requestDelay,
                                  bool extended,
                                  bool force,
                                  size_t unsetPinCodesCountToStop ) {
    ZNET_LOGI( "%s %i, requestDelay %llu, unsetPinCodesCountToStop %i\n",
               __func__,
               nodeId,
               requestDelay.count(),
               unsetPinCodesCountToStop );

    ZNode* node = onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }
    node->setRequestDelay( requestDelay );

    for( uint8_t i = 0; i < node->getChannelsCount(); ++i ) {
        if( auto userCodeCommandClass = node->getCommandClass<znet::ZCc0x63UserCode>( i ) ) {
            userCodeCommandClass->setUnsetPinCodesCountToStop( unsetPinCodesCountToStop );
        }
    }

    requestNodeValues( nodeId, extended, force );
}

void ZNetImpl::requestNodeValues( uint8_t nodeId, bool extended, bool force ) {
    ZNET_LOGI( "%s node %d\n", __func__, nodeId );
    if( extended ) {
        m_currentValuesExtendedHandler.addNodeToProcess( nodeId, force );
        m_currentValuesHandler.removeNodesFromProcess( m_currentValuesExtendedHandler.getProcessingNodeIds() );
    }
    else if( !m_currentValuesExtendedHandler.isNodeAddedToProcess( nodeId ) ) {
        m_currentValuesHandler.addNodeToProcess( nodeId, force );
    }

    if( force &&
        ( m_state.m_cmd == IZNet::Mode::ValuesGettingExtended || m_state.m_cmd == IZNet::Mode::ValuesGetting ) ) {
        activateMode( IZNet::Mode::Monitoring );
        if( extended ) {
            activateMode( IZNet::Mode::ValuesGettingExtended );
        }
        else {
            activateMode( IZNet::Mode::ValuesGetting );
        }
    }
}

void ZNetImpl::interviewNode( uint8_t nodeId ) {
    m_discoverHandler.addNodeToProcess( nodeId );
}

void ZNetImpl::activateMode( const IZNet::Mode mode ) {
    assert( &m_cbHandler.get() != &m_backupRestoringHandler );

    auto activateHandler = [this]( ZNetHandler& handler, const char* name ) {
        if( &m_cbHandler.get() != &handler ) {
            ZNET_LOGI( "activateMode %s\n", name );

            m_cbHandler.get().onDeactivated();  // use finalize or remove
            m_cbHandler = handler;
            handler.initialize();
        }
    };

    switch( mode ) {
    case IZNet::Mode::IncludeStarting:
        activateHandler( m_interviewHandler, "Include" );
        break;

    case IZNet::Mode::ExcludeStarting:
        activateHandler( m_excludeHandler, "Exclude" );
        break;

    case IZNet::Mode::LearnStarting:
        activateHandler( m_learnHandler, "Learn" );
        break;

    case IZNet::Mode::LearnStopping:
        activateHandler( m_stopLearnHandler, "Stop learn" );
        break;

    case IZNet::Mode::ValuesGetting:
        activateHandler( m_currentValuesHandler, "Current values" );
        break;

    case IZNet::Mode::NodeChecking:
        activateHandler( m_nodeCheckHandler, "Node checking" );
        break;

    case IZNet::Mode::BackupCreation:
        activateHandler( m_backupCreationHandler, "Backup creation" );
        break;

    case IZNet::Mode::BackupRestoring:
        activateHandler( m_backupRestoringHandler, "Backup restoring" );
        break;

    case IZNet::Mode::ValuesGettingExtended:
        activateHandler( m_currentValuesExtendedHandler, "Current values extended" );
        break;

    case IZNet::Mode::Monitoring:
        activateHandler( m_monitoringHandler, "Monitoring" );
        break;

    case IZNet::Mode::TimeSynchronization:
        activateHandler( m_timeSynchronizationHandler, "Time synchronization" );
        break;

    case IZNet::Mode::InclusionControllerStarting:
        activateHandler( m_inclusionHandler, "Inclusion controller" );
        break;

    case IZNet::Mode::InclusionControllerStartingReplace:
        activateHandler( m_inclusionReplaceHandler, "Inclusion controller replace" );
        break;

    case IZNet::Mode::Rediscover:
        activateHandler( m_discoverHandler, "Rediscover" );
        break;

    case IZNet::Mode::FirmwareUpdate:
        activateHandler( m_firmwareUpdateHandler, "Firmware Update" );
        break;

    case IZNet::Mode::Reseting:
        activateHandler( m_emptyHandler, "Reset" );
        break;

    default:
        ZNET_LOGI( "Cannot activate mode %i\n", mode );
        break;
    }
}

bool ZNetImpl::isResetting() {
    return checkState( IZNet::Mode::Reseting, ModeStatus::Invoked );
}

void ZNetImpl::znetInitFinish( int err, void* arg ) {
    ZNET_LOGI( "%s\n", __func__ );
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( ptr->checkState( IZNet::Mode::Initializing, ModeStatus::Started ) ) {
            if( err ) {
                ptr->setState( IZNet::Mode::Initializing, ModeStatus::Failed );
            }
            else {
                ptr->setState( IZNet::Mode::Initializing, ModeStatus::Finished );
                ptr->activateMode( znet::IZNet::Mode::Monitoring );
            }
        }
    }
}

void ZNetImpl::znetModuleResetStarted( void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ZNET_LOGI( "module reset has been started\n" );

        ptr->setState( IZNet::Mode::Reseting, ModeStatus::Started );
    }
}

void ZNetImpl::znetModuleReset( int err, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ZNET_LOGE( "%s fail\n", __func__ );

            ModeStatus status = err == ZNET_ERR_TIME ? ModeStatus::Timeout : ModeStatus::Failed;
            ptr->setState( IZNet::Mode::Reseting, status );
        }
        else {
            ZNET_LOGI( "%s success\n", __func__ );

            ptr->setState( IZNet::Mode::Reseting, ModeStatus::Finished );
        }

        ZNetImpl* impl = static_cast<ZNetImpl*>( arg );
        if( impl != nullptr ) {
            impl->activateMode( IZNet::Mode::Monitoring );
        }
    }
}

void ZNetImpl::znetLog( int lvl, const char* fmt, va_list list, void* arg ) {
    ZNetImpl* impl = static_cast<ZNetImpl*>( arg );
    if( impl != nullptr ) {
        return impl->getOwner().znetLog( lvl, fmt, list, arg );
    }
}

void* ZNetImpl::znetAlloc( void* inptr, size_t size, void* arg ) {
    ZNetImpl* impl = static_cast<ZNetImpl*>( arg );
    if( impl != nullptr ) {
        return impl->getOwner().znetAlloc( inptr, size, nullptr );
    }
    return nullptr;
}

uint64_t ZNetImpl::znetClock( void* arg ) {
    ZNetImpl* impl = static_cast<ZNetImpl*>( arg );
    if( impl != nullptr ) {
        return impl->getOwner().znetClock( nullptr );
    }
    return 0;
}

void ZNetImpl::znetNodeAdd( int err, const znet_nodeinfo_t* node_info, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err || !node_info || node_info->node_id == 0 ) {
            ZNET_LOGE( "%s fail\n", __func__ );
            ptr->getHandler().onNodeFailed( node_info ? node_info->node_id : 0, err );
        }
        else {
            ZNET_LOGI( "%s success node: %hhu\n", __func__, node_info->node_id );
            ptr->getHandler().onNodeAdd( node_info );
        }
    }
}

void ZNetImpl::znetNodeAddSecurityFailed( znet_node_id_t nodeId, void* arg ) {
    auto znetImpl = static_cast<ZNetImpl*>( arg );
    if( !znetImpl ) {
        return;
    }
    ZNET_LOGD( "%s, nodeId=%d\n", __func__, nodeId );
    znetImpl->getHandler( nodeId ).onNodeAddSecurityFailed();
}

void ZNetImpl::znetNodeAddBreak( int err, const znet_nodeinfo_t* node_info, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            switch( err ) {
            case ZNET_ERR_TIME:
                ptr->setState( IZNet::Mode::IncludeStopping, ModeStatus::Timeout );

                break;
            default:
                ptr->setState( IZNet::Mode::IncludeStopping, ModeStatus::Failed );
                break;
            }
        }
        else {
            ptr->setState( IZNet::Mode::IncludeStopping, ModeStatus::Finished );
        }
        ptr->activateMode( IZNet::Mode::Monitoring );
    }
}

void ZNetImpl::znetNodeAddInvoked( void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->setState( IZNet::Mode::IncludeStarting, ModeStatus::Invoked );
    }
}

void ZNetImpl::znetNodeRem( int err, const znet_nodeinfo_t* node_info, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->getHandler().onNodeRem( err, node_info );
    }
}

void ZNetImpl::znetNodeRemBreak( int err, const znet_nodeinfo_t* node_info, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->getHandler().onNodeRemBreak( err, node_info );
    }
}

void ZNetImpl::znetNodeRemInvoked( void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->getHandler().onNodeRemInvoked();
    }
}

void ZNetImpl::znetNodeLearnInvoked( void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->setState( IZNet::Mode::LearnStarting, ModeStatus::Invoked );
    }
}

void ZNetImpl::znetNodeLearn( int err, znet_node_id_t destNodeId, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ZNET_LOGE( "%s fail error: %hhu\n", __func__, err );
            switch( err ) {
            case ZNET_ERR_TIME:
                ptr->setState( IZNet::Mode::LearnStarting, ModeStatus::Timeout );
                break;

            default:
                ptr->setState( IZNet::Mode::LearnStarting, ModeStatus::Failed );
                break;
            }
            ptr->getHandler().onNodeFailed( destNodeId, err );
        }
        else {
            if( destNodeId ) {
                ZNET_LOGI( "%s success (node id: %hhu)\n", __func__, destNodeId );
            }
            else {
                ZNET_LOGI( "%s success\n", __func__ );
            }
            ptr->getHandler().onNodeLearn( destNodeId );
        }
    }
}

void ZNetImpl::znetNodeLearnBreak( int err, znet_node_id_t destNodeId, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->getHandler().onNodeLearnBreak( err, destNodeId );
    }
}

void ZNetImpl::znetNodeResetLocaly( int err, const znet_node_id_t nodeId, void* arg ) {
    if( err ) {
        ZNET_LOGE( "%s fail\n", __func__ );
    }
    else {
        ZNET_LOGI( "%s success node: %hhu)\n", __func__, nodeId );

        ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
        if( ptr != nullptr ) {
            ptr->setState( IZNet::Mode::ExcludeStarting, ModeStatus::Started );

            ZNode* node = ptr->getOwner().onFindNode( nodeId );
            if( node == nullptr ) {
                ptr->setState( IZNet::Mode::ExcludeStarting, ModeStatus::Failed );
                return;
            }
            ptr->getOwner().nodeRemove( nodeId );

            ptr->setState( IZNet::Mode::ExcludeStarting, ModeStatus::Finished );
        }
    }
}

void ZNetImpl::znetInclusionControllerInitiate( znet_error_t err,
                                                const znet_node_id_t nodeId,
                                                const znet_node_id_t childNodeId,
                                                const znet_inclution_controller_status_t stepId,
                                                void* arg ) {
    if( err ) {
        ZNET_LOGE( "%s fail\n", __func__ );
    }
    else {
        ZNET_LOGI( "%s (node id: %hhu)\n", __func__, nodeId );

        ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
        if( ptr != nullptr ) {
            ptr->getHandler( nodeId ).onNodeInclusionControllerInitiate( nodeId, childNodeId, stepId );
        }
    }
}

void ZNetImpl::znetInclusionControllerComplete( znet_error_t err,
                                                const znet_node_id_t nodeId,
                                                const znet_inclution_controller_step_id_t stepId,
                                                const znet_inclution_controller_status_t status,
                                                void* arg ) {
    if( err ) {
        ZNET_LOGE( "%s fail\n", __func__ );
    }
    else {
        ZNET_LOGI( "%s (node id: %hhu)\n", __func__, nodeId );

        ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
        if( ptr != nullptr ) {
            ptr->getHandler( nodeId ).onNodeInclusionControllerComplete( nodeId, stepId, status );
        }
    }
}

void ZNetImpl::znetNodeProvisioningSetReceived( znet_error_t err,
                                                const znet_node_id_t nodeId,
                                                znet_node_channel_id_t channelId,
                                                const znet_node_provisioning_set_t* value,
                                                void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->getHandler( nodeId ).onNodeProvisioningSet( nodeId, channelId, value );
    }
}

void ZNetImpl::znetNodeProvisioningDeleteReceived( znet_error_t err,
                                                   const znet_node_id_t nodeId,
                                                   znet_node_channel_id_t channelId,
                                                   const znet_node_provisioning_delete_t* value,
                                                   void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->getHandler( nodeId ).onNodeProvisioningDelete( nodeId, channelId, value );
    }
}

void ZNetImpl::znetNodeProvisioningGetReceived( znet_error_t err,
                                                const znet_node_id_t nodeId,
                                                znet_node_channel_id_t channelId,
                                                uint8_t sequenceNumber,
                                                const uint8_t dskKey[16],
                                                void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->getHandler( nodeId ).onNodeProvisioningGet( nodeId, channelId, sequenceNumber, dskKey );
    }
}

void ZNetImpl::znetNodeProvisioningListIterationGetReceived( znet_error_t err,
                                                             const znet_node_id_t nodeId,
                                                             znet_node_channel_id_t channelId,
                                                             uint8_t sequenceNumber,
                                                             uint8_t remainingCounter,
                                                             void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->getHandler( nodeId ).onNodeProvisioningListIterationGet(
            nodeId, channelId, sequenceNumber, remainingCounter );
    }
}

void ZNetImpl::znetNodeCmdSecuritySupported( int err,
                                             znet_node_id_t nodeId,
                                             znet_node_channel_id_t channel_id,
                                             const znet_security_supported_report_t* value,
                                             void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ptr->getHandler( nodeId ).onNodeCmdSecuritySupported( nodeId, value );
    }
}

void ZNetImpl::znetNodeList( int err, const uint8_t* nodesList, void* arg ) {
    if( err ) {
        ZNET_LOGE( "%s fail\n", __func__ );
    }
    else {
        if( nodesList == nullptr ) {
            ZNET_LOGE( "empty nodesList\n" );
            return;
        }
        ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
        if( ptr != nullptr ) {
            ptr->getHandler().onNodeList( nodesList );
        }
    }
}

void ZNetImpl::znetNodeInifReceived( uint8_t nodeId, uint8_t* nhid, bool foreign, void* arg ) {
    if( !foreign ) {
        ZNET_LOGD( "%s NodeID: % 02x.\n ", __func__, nodeId );
        // todo: start interview here?
        return;
    }
    auto znetImpl = reinterpret_cast<ZNetImpl*>( arg );
    if( !znetImpl ) {
        return;
    }
    using fields_t = ::znet::ProvisioningListEntryFields;
    ::znet::api::IProvisioningListController::OperationStatus status;
    ::znet::api::ProvisioningListQuery{ znetImpl->getProvisioningListControllerDataSource() }
        .filterBy( fields::nhid(::znet::utils::toStdArray<4>( nhid ) ) )
        .setField( &fields_t::m_status, fields_t::Status::OTHER_NETWORK )
        .execute( status );
    switch( status ) {
    case ::znet::api::IProvisioningListController::OperationStatus::SUCCESS: {
        ZNET_LOGD( "%s SUCCESS NHID: ", __func__ );
        print_hex_buf( nhid, 4 );
        break;
    }
    case ::znet::api::IProvisioningListController::OperationStatus::ENTRY_NOT_FOUND:
        ZNET_LOGD( "%s ENTRY_NOT_FOUND NHID: ", __func__ );
        print_hex_buf( nhid, 4 );
        break;
    default:
        ZNET_LOGE( "%s Cannot handle : OperationStatus=%d.", __func__, status );
    }
}

void ZNetImpl::znetNodeNhidReceived( uint8_t* nhid, void* arg ) {
    auto znetImpl = reinterpret_cast<ZNetImpl*>( arg );
    if( !znetImpl ) {
        return;
    }
    DskDerivedHomeId fullHomeId;
    ::znet::api::IProvisioningListController::OperationStatus status;
    ::znet::api::ProvisioningListQuery{ znetImpl->getProvisioningListControllerDataSource() }
        .filterBy( fields::nhid(::znet::utils::toStdArray<4>( nhid ) ) &&
                   fields::addingMode(::znet::ProvisioningListEntryFields::AddingMode::SMART ) &&
                   ( !fields::status(::znet::ProvisioningListEntryFields::Status::ADDED ) ) )
        .getDskDerivedHomeId( fullHomeId )
        .execute( status );
    switch( status ) {
    case ::znet::api::IProvisioningListController::OperationStatus::SUCCESS:
        ZNET_LOGD( "Initiating Smart Start inclusion. DSK derived HomeID: " );
        print_hex_buf( fullHomeId.data(), fullHomeId.size() );
        znetImpl->activateMode(::znet::IZNet::Mode::IncludeStarting );
        znetImpl->getHandler().startInclude( fullHomeId );
        break;
    case ::znet::api::IProvisioningListController::OperationStatus::ENTRY_NOT_FOUND:
        ZNET_LOGD( "Ignoring Smart Start inclusion request. NHID: " );
        print_hex_buf( nhid, 4 );
        break;
    default:
        ZNET_LOGE( "%s Cannot handle : OperationStatus=%d.", __func__, status );
    }
}

void ZNetImpl::znetS2SelectKeys( znet_S2_keys_t keyList, znet_node_id_t node_id, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );

    if( ptr != nullptr ) {
        ZNode* node = ptr->onFindNode( node_id );
        if( node ) {
            node->setSecurityClassStarted( SecurityMode::getHighest( keyList ) );
        }

        if( !ptr->tryAutoRespondS2SecurityModes( node_id, keyList ) ) {
            ptr->getHandler().onNodeAddSecurityKeysRequested( keyList );
            ptr->getOwner().znetS2SelectKeys( keyList, arg );
        }
    }
}

void ZNetImpl::znetS2RequestPubKey( znet_node_id_t node_id,
                                    znet_S2_keys_t keyList,
                                    const znet_S2_pub_key_t* key,
                                    void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( !ptr->tryAutoRespondS2PublicKey( node_id ) ) {
            ptr->getHandler().onNodeAddSecurityPubKeyRequested( key );
            ptr->getOwner().znetS2RequestPubKey( keyList, key, arg );
        }
    }
}

bool ZNetImpl::tryAutoRespondS2SecurityModes( znet_node_id_t nodeId, znet_S2_keys_t supportedModes ) {
    uint8_t userDefinedModes;
    using fields_t = ::znet::ProvisioningListEntryFields;
    auto entryFound = ::znet::api::ProvisioningListQuery{ getProvisioningListControllerDataSource() }
                          .filterBy( fields::nodeId( nodeId ) )
                          .getField( &fields_t::m_securityModes, userDefinedModes, supportedModes )
                          .setField( &fields_t::m_supportedModes, supportedModes )
                          .execute();
    if( entryFound ) {
        // Validate selected security modes: keep only supported ones.
        ::znet_node_s2_select_keys( userDefinedModes & supportedModes );
    }
    return entryFound;
}

bool ZNetImpl::tryAutoRespondS2PublicKey( znet_node_id_t nodeId ) {
    std::array<uint8_t, 2> key;
    auto success = ::znet::api::ProvisioningListQuery{ getProvisioningListControllerDataSource() }
                       .filterBy( fields::nodeId( nodeId ) )
                       .getPubKey( key )
                       .execute();
    if( success ) {
        const size_t buffSize = sizeof( znet_S2_pub_key_t ) + 2 * sizeof( ( (znet_S2_pub_key_t*)nullptr )->key[0] );
        uint8_t buff[buffSize] = {};
        auto pubKey = reinterpret_cast<znet_S2_pub_key_t*>( &buff );
        pubKey->size = 2;
        std::memcpy( pubKey->key, key.data(), key.size() );
        ::znet_node_s2_response_pub_key( pubKey );
    }
    return success;
}

void ZNetImpl::hardResetZwaveChip( void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ZNET_LOGI( "Zwave hard reset is requested\n" );
        ::znet_deinit();
        ptr->getOwner().uartClose();
        ptr->getOwner().zwaveModuleResetPinAssert();
        if( !ptr->getOwner().uartInit() ) {
            ZNET_LOGE( "uartInit failed!\n" );
            return;
        }
        if(::znet_init( &ptr->m_znet_cb ) != 0 ) {
            ZNET_LOGE( "znet_init failed!\n" );
            return;
        }
        ptr->getOwner().zwaveModuleResetPinRelease();
        // Waiting for 500ms according to specs.
        for( size_t i = 0; i < 5; ++i ) {
            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
            znet_proc();
        }
        ZNET_LOGI( "Zwave reset is completed\n" );
    }
}

void ZNetImpl::sisUpdated( int err, znet_node_id_t nodeId, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->getOwner().onSisUpdated( ptr->isIncludeExcludeAvailable() );
    }
}

void ZNetImpl::libraryInfoUpdated( int err, znet_library_info_t* info, uint8_t lib_type, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        void* buf = info->name;
        ptr->m_libraryType = lib_type;
        ptr->m_libraryVersion.assign( static_cast<char*>( buf ), static_cast<size_t>( info->size ) );
    }
}

void ZNetImpl::znetS2ShowPubKey( znet_S2_keys_t keyList, const znet_S2_pub_key_t* key, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->getOwner().znetS2ShowPubKey( keyList, key, arg );
    }
}

void ZNetImpl::znetForcedRequest( int err, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err != ZNET_ERR_OK ) {
            ZNET_LOGE( "forced request report fail\n" );
        }
        else {
            ptr->getHandler().onForcedRequestResult();
        }
    }
}

bool ZNetImpl::znetCheckNodeSupportsCommandClass( znet_node_id_t nodeId,
                                                  znet_node_channel_id_t endpoint,
                                                  znet_command_class_t commandClassId,
                                                  void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr == nullptr )
        return false;

    auto node = ptr->getOwner().onFindNode( nodeId );
    if( node == nullptr )
        return false;

    if( node->getCommandClass( endpoint, commandClassId ) != nullptr )
        return true;

    for( uint8_t i = 0; i != node->getUnsupportedCommandClassCount( endpoint ); ++i ) {
        if( node->getUnsupportedCommandClassId( endpoint, i ) == commandClassId )
            return true;
    }

    return false;
}

void ZNetImpl::znetNodeCmdVersion( int err, znet_node_id_t nodeId, const znet_version_report_t* value, void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu)\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onNodeCmdVersion( nodeId, value );
    }
}

void ZNetImpl::znetNodeCmdCommandVersion( znet_error_t err,
                                          znet_node_id_t nodeId,
                                          znet_node_channel_id_t channel_id,
                                          znet_command_class_t class_id,
                                          znet_command_class_version_t version,
                                          void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        //            esp_log_buffer_hexdump_internal( __func__, value, sizeof( znet_command_version_report_t ),
        //            ESP_LOG_INFO );

        //  move to a finish of an interview

        ZNET_LOGI( "%s node: %u, CC: 0x%02X, version: %u\n", __func__, nodeId, class_id, version );
        ptr->getHandler( nodeId ).onNodeCmdCommandVersion( nodeId, class_id, version );
    }
}

void ZNetImpl::znetNodeCmdVersionCapabilities( int err,
                                               znet_node_id_t nodeId,
                                               const znet_version_capabilities_report_t* value,
                                               void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu)\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onNodeCmdVersionCapabilities( nodeId, value );
    }
}

void ZNetImpl::znetNodeCmdSoftwareVersion( int err,
                                           znet_node_id_t nodeId,
                                           const znet_software_version_report_t* value,
                                           void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu)\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onNodeCmdSoftwareVersion( nodeId, value );
    }
}

void ZNetImpl::znetNodeCmdManufacturerSpecific( int err,
                                                znet_node_id_t nodeId,
                                                znet_node_channel_id_t channel_id,
                                                const znet_manufacturer_specific_report_t* value,
                                                void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu)\n", __func__, nodeId );
        //            esp_log_buffer_hexdump_internal(
        //                __func__, value, sizeof( znet_manufacturer_specific_report_t ), ESP_LOG_INFO );

        //  move to a finish of an interview
        if( value != nullptr ) {
            ptr->getHandler( nodeId ).onNodeCmdManufacturerSpecific(
                nodeId, value->manufacturer_id, value->product_type, value->product_id );
        }
    }
}

void ZNetImpl::znetNodeCmdDeviceSpecific( int err,
                                          znet_node_id_t nodeId,
                                          const znet_device_specific_report_t* value,
                                          void* arg ) {
    if( err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu)\n", __func__, nodeId );
    }
}

void ZNetImpl::znetNodeCmdZWavePlusInfo( int err,
                                         znet_node_id_t nodeId,
                                         const znet_zwaveplus_info_report_t* value,
                                         void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu)\n", __func__, nodeId );
        //            esp_log_buffer_hexdump_internal( __func__, value, sizeof( znet_zwaveplus_info_report_t ),
        //            ESP_LOG_INFO );

        if( value != nullptr ) {
            ptr->getHandler( nodeId ).onNodeCmdZWavePlusInfo( nodeId, value );
        }
    }
}

void ZNetImpl::znetNodeCmdManufacturerSpecificFill( znet_manufacturer_specific_report_t* data, void* arg ) {
    if( auto ptr = static_cast<ZNetImpl*>( arg ) ) {
        assert( data );
        ptr->getOwner().fillManufacturerSpecific( *data );
    }
}

void ZNetImpl::znetNodeCmdVersionFill( znet_version_report_t* data, void* arg ) {
    if( auto ptr = static_cast<ZNetImpl*>( arg ) ) {
        assert( data );
        ptr->getOwner().fillVersion( *data );
    }
}

void ZNetImpl::znetNodeCmdZWavePlusFill( znet_zwaveplus_info_report_t* data, void* arg ) {
    if( auto ptr = static_cast<ZNetImpl*>( arg ) ) {
        assert( data );
        ptr->getOwner().fillZWavePlusInfo( *data );
    }
}

void ZNetImpl::znetNodeCmdFirmwareUpdateInfoFill( znet_firmware_md_report_v1_t* dataV1,
                                                  znet_firmware_md_report_v3_t* dataV3,
                                                  znet_firmware_md_report_v5_t* dataV5,
                                                  void* arg ) {
    if( auto ptr = static_cast<ZNetImpl*>( arg ) ) {
        assert( dataV1 );
        assert( dataV3 );
        assert( dataV5 );
        ptr->getOwner().fillFirmwareUpdateInfo( *dataV1, *dataV3, *dataV5 );
    }
}

void ZNetImpl::znetNodeCmdBasic( int err,
                                 znet_node_id_t nodeId,
                                 znet_node_channel_id_t channel_id,
                                 znet_cmd_basic_value_t currentValue,
                                 znet_cmd_basic_value_t targetValue,
                                 uint8_t duration,
                                 void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu, channel id: %hhu) %hhu -> %hhu\n",
                   __func__,
                   nodeId,
                   channel_id,
                   currentValue,
                   targetValue );
        ptr->getHandler( nodeId ).onNodeCmdBasic( nodeId, channel_id, currentValue, targetValue );
    }
}

void ZNetImpl::znetNodeCmdBasicSet( int err,
                                    znet_node_id_t nodeId,
                                    znet_node_channel_id_t channel_id,
                                    znet_cmd_basic_value_t currentValue,
                                    znet_cmd_basic_value_t targetValue,
                                    uint8_t duration,
                                    void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ZNET_LOGE( "%s fail\n", __func__ );
            ptr->getHandler( nodeId ).onNodeFailed( nodeId, err );
        }
        else {
            ZNET_LOGI( "%s (node id: %hhu, channel id: %hhu) %hhu -> %hhu\n",
                       __func__,
                       nodeId,
                       channel_id,
                       currentValue,
                       targetValue );
            ptr->getHandler( nodeId ).onNodeCmdBasicSet( nodeId, channel_id, currentValue, targetValue );
        }
    }
}

void ZNetImpl::znetNodeCmdBatteryLevel( int err,
                                        znet_node_id_t nodeId,
                                        znet_node_channel_id_t channel_id,
                                        znet_cmd_battery_level_value_t value,
                                        void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu) value %hhu\n", __func__, nodeId, value );
        ptr->getHandler( nodeId ).onNodeCmdBatteryLevel( nodeId, channel_id, value );
    }
}

void ZNetImpl::znetNodeCmdBinarySwitch( znet_error_t err,
                                        znet_node_id_t nodeId,
                                        znet_node_channel_id_t channelId,
                                        znet_cmd_binary_switch_value_t value,
                                        znet_cmd_binary_switch_value_t targetValue,
                                        uint8_t duration,
                                        void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu channel id: %hhu) value: %hhu, target value: %hhu, duration: %hhu\n",
                   __func__,
                   nodeId,
                   channelId,
                   value,
                   targetValue,
                   duration );

        if( value == ZNET_CMD_BINARY_SWITCH_VALUE_UNKNOWN ) {
            // TODO: if it's possible on devices, move this check to plugin
            ZNET_LOGW( "Unknown switch value recevied\n" );
            return;
        }

        const auto isTargetValueValid = targetValue != ZNET_CMD_BINARY_SWITCH_VALUE_UNKNOWN;
        ptr->getHandler( nodeId ).onNodeCmdBinarySwitch(
            nodeId,
            channelId,
            value,
            isTargetValueValid ? Uint8Opt( targetValue ) : Uint8Opt(),
            isTargetValueValid ? utils::actuatorReportTimeToSeconds( duration ) : SecondsOpt() );
    }
}

void ZNetImpl::znetNodeCmdClock( int err,
                                 znet_node_id_t node_id,
                                 znet_node_channel_id_t channel_id,
                                 const znet_clock_value_t* value,
                                 void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu) HH:%hhu MM:%hhu WD:%hhu\n",
                   __func__,
                   node_id,
                   value->hour,
                   value->minute,
                   value->weekday );
        ptr->getHandler( node_id ).onNodeCmdClock( node_id, channel_id, value );
    }
}

void ZNetImpl::znetNodeCmdTimeParameters( int err,
                                          znet_node_id_t node_id,
                                          znet_node_channel_id_t channel_id,
                                          const znet_cmd_time_parameters_report_t* value,
                                          void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu) year: %u, month: %hhu, day: %hhu, hour: %hhu, "
                   "minute: %hhu, second: %hhu\n",
                   __func__,
                   node_id,
                   value->date_utc.year,
                   value->date_utc.month,
                   value->date_utc.day,
                   value->time_utc.hour,
                   value->time_utc.minute,
                   value->time_utc.second );

        ptr->getHandler( node_id ).onNodeCmdTimeParameters( node_id, channel_id, value );
    }
}

void ZNetImpl::znetNodeCmdColorSwitch( znet_error_t err,
                                       znet_node_id_t nodeId,
                                       znet_node_channel_id_t channelId,
                                       uint8_t colorComponentId,
                                       const znet_color_switch_report_t* report,
                                       void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        assert( report );

        ZNET_LOGI( "%s succ node: %hhu, "
                   "channel: %hhu, "
                   "component: %hhu, "
                   "value: %hhu, "
                   "has target: %s, "
                   "target value: %hhu, "
                   "target duration: %hhu\n",
                   __func__,
                   nodeId,
                   channelId,
                   colorComponentId,
                   report->value,
                   utils::boolToYesNo( report->has_target_value ),
                   report->target_value,
                   report->target_duration );

        ptr->getHandler( nodeId ).onNodeCmdColorSwitch(
            nodeId,
            channelId,
            static_cast<ColorComponentID>( colorComponentId ),
            report->value,
            report->has_target_value ? report->target_value : utils::optional<uint8_t>(),
            report->has_target_value ? utils::actuatorReportTimeToSeconds( report->target_duration ) : SecondsOpt() );
    }
}

void ZNetImpl::znetNodeCmdColorSwitchSupported( znet_error_t err,
                                                znet_node_id_t nodeId,
                                                znet_node_channel_id_t channelId,
                                                const znet_color_switch_supported_report_t* report,
                                                void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s succ node: %hhu, channel: %hhu)\n", __func__, nodeId, channelId );
        ptr->getHandler( nodeId ).onNodeCmdColorSwitchSupported(
            nodeId, channelId, report->component_mask_count, report->component_mask );
    }
}

void ZNetImpl::znetNodeCmdDoorLockOperation( znet_error_t err,
                                             znet_node_id_t node_id,
                                             znet_node_channel_id_t channel_id,
                                             const znet_door_lock_operation_report_t* value,
                                             void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s \n"
                   "  node id: %hhu\n"
                   "  channel_id: %hhu\n"
                   "  mode: %hhu\n"
                   "  enabled_inside_handle_ids: %s\n"
                   "  enabled_outside_handle_ids: %s\n"
                   "  is_lock_timeout_activated: %s\n"
                   "  condition: %hhu\n"
                   "  lock_timeout: %u\n"
                   "  tagret_mode: %hhu\n"
                   "  tagret_mode_timeout: %hhu\n",
                   __func__,
                   node_id,
                   channel_id,
                   value->mode,
                   utils::bitmaskToString( &value->enabled_inside_handle_ids, 1 ).c_str(),
                   utils::bitmaskToString( &value->enabled_outside_handle_ids, 1 ).c_str(),
                   utils::boolToYesNo( value->is_lock_timeout_activated ),
                   value->condition,
                   value->lock_timeout,
                   value->tagret_mode,
                   value->tagret_mode_timeout );

        ptr->getHandler( node_id ).onNodeCmdDoorLockOperation(
            node_id,
            channel_id,
            static_cast<DoorLock::Mode>( value->mode ),
            value->enabled_inside_handle_ids,
            value->enabled_outside_handle_ids,
            value->condition,
            value->is_lock_timeout_activated ? SecondsOpt( Seconds( value->lock_timeout ) ) : SecondsOpt{},
            value->has_target_mode ? DoorLock::ModeOpt( static_cast<DoorLock::Mode>( value->tagret_mode ) ) :
                                     DoorLock::ModeOpt{},
            value->has_target_mode ? SecondsOpt( Seconds( value->tagret_mode_timeout ) ) : SecondsOpt{} );
    }
}

void ZNetImpl::znetNodeCmdDoorLockConfiguration( znet_error_t err,
                                                 znet_node_id_t nodeId,
                                                 znet_node_channel_id_t channelId,
                                                 const znet_door_lock_configuration_t* configuration,
                                                 const znet_door_lock_extended_configuration_t* extendedConfiguration,
                                                 void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        assert( configuration );

        ZNET_LOGI( "%s \n "
                   "  node id: %hhu\n"
                   "  channel_id: %hhu\n"
                   "  inside door handle modes: %s\n"
                   "  outside door handle modes: %s\n"
                   "  are_timed_operations_enabled: %s\n"
                   "  lock_timeout: %u\n",
                   __func__,
                   nodeId,
                   channelId,
                   utils::bitmaskToString( &configuration->enabled_inside_handle_ids, 1 ).c_str(),
                   utils::bitmaskToString( &configuration->enabled_outside_handle_ids, 1 ).c_str(),
                   utils::boolToYesNo( configuration->are_timed_operations_enabled ),
                   configuration->lock_timeout );

        if( extendedConfiguration ) {
            ZNET_LOGI( "%s ext config (v4): \n"
                       "  auto_relock_timeout: %u\n"
                       "  hold_and_release_timeout: %u\n"
                       "  twist_assist_enabled: %s\n"
                       "  block_to_block_enabled: %s\n",
                       __func__,
                       extendedConfiguration->auto_relock_timeout,
                       extendedConfiguration->hold_and_release_timeout,
                       utils::boolToYesNo( extendedConfiguration->twist_assist_enabled ),
                       utils::boolToYesNo( extendedConfiguration->block_to_block_enabled ) );
        }

        ptr->getHandler( nodeId ).onNodeCmdDoorLockConfiguration(
            nodeId, channelId, *configuration, extendedConfiguration );
    }
}

void ZNetImpl::znetNodeCmdDoorLockCapabilities( znet_error_t err,
                                                znet_node_id_t nodeId,
                                                znet_node_channel_id_t channelId,
                                                const znet_door_lock_capabilities_t* capabilities,
                                                void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s \n"
                   "  node id: %hhu\n"
                   "  channel_id: %hhu\n"
                   "  inside door handle modes: %s\n"
                   "  outside door handle modes: %s\n"
                   "  is_door_supported: %s\n"
                   "  is_bolt_supported: %s\n"
                   "  is_latch_supported: %s\n"
                   "  is_block_to_block_supported: %s\n"
                   "  is_twist_assist_supported: %s\n"
                   "  is_hold_and_release_supported: %s\n"
                   "  are_timed_operations_supported: %s\n"
                   "  is_auto_relock_supported: %s\n",
                   __func__,
                   nodeId,
                   channelId,
                   utils::bitmaskToString( &capabilities->supported_inside_handle_ids, 1 ).c_str(),
                   utils::bitmaskToString( &capabilities->supported_outside_handle_ids, 1 ).c_str(),
                   utils::boolToYesNo( capabilities->is_door_supported ),
                   utils::boolToYesNo( capabilities->is_bolt_supported ),
                   utils::boolToYesNo( capabilities->is_latch_supported ),
                   utils::boolToYesNo( capabilities->is_block_to_block_supported ),
                   utils::boolToYesNo( capabilities->is_twist_assist_supported ),
                   utils::boolToYesNo( capabilities->is_hold_and_release_supported ),
                   utils::boolToYesNo( capabilities->are_timed_operations_supported ),
                   utils::boolToYesNo( capabilities->is_auto_relock_supported ) );

        ptr->getHandler( nodeId ).onNodeCmdDoorLockCapabilities( nodeId, channelId, *capabilities );
    }
}

void ZNetImpl::znetNodeCmdNotification( znet_error_t err,
                                        znet_node_id_t node_id,
                                        znet_node_channel_id_t channel_id,
                                        const znet_notification_report_t* value,
                                        bool isNotificationRequested,
                                        void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        if( value ) {
            ZNET_LOGI( "%s Node ID: %hhu, v1 type: %hhu, type: %hhu, event: %hhu\n",
                       __func__,
                       node_id,
                       value->v1_alarm_type,
                       value->notification_type,
                       value->notification_event );
            ptr->getHandler( node_id ).onNodeCmdNotification( node_id, channel_id, *value, isNotificationRequested );
        }
    }
}

void ZNetImpl::znetNodeCmdNotificationSupported( int err,
                                                 znet_node_id_t node_id,
                                                 znet_node_channel_id_t channel_id,
                                                 const znet_notification_supported_report_t* value,
                                                 void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        if( value ) {
            ZNET_LOGI( "notification supported report received. Node ID: %hhu\n", node_id );
            ptr->getHandler( node_id ).onNodeCmdNotificationSupported( node_id, channel_id, value );
        }
    }
}

void ZNetImpl::znetNodeCmdNotificationEventSupported( int err,
                                                      znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      uint8_t type,
                                                      const uint8_t* bitmask,
                                                      uint8_t size,
                                                      void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node_id: %hhu\n", __func__, node_id );
        ptr->getHandler( node_id ).onNodeCmdNotificationEventSupported( node_id, channel_id, type, bitmask, size );
    }
}

void ZNetImpl::znetNodeCmdMultilevelSwitch( znet_error_t err,
                                            znet_node_id_t nodeId,
                                            znet_node_channel_id_t channelId,
                                            znet_cmd_multilevel_switch_value_t currentValue,
                                            znet_cmd_multilevel_switch_value_t targetValue,
                                            uint8_t duration,
                                            void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu, channel id: %hhu) %hhu -> %hhu\n",
                   __func__,
                   nodeId,
                   channelId,
                   currentValue,
                   targetValue,
                   duration );
        const auto isTargetValueValid = targetValue != ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN;
        ptr->getHandler( nodeId ).onNodeCmdMultilevelSwitch(
            nodeId,
            channelId,
            currentValue,
            isTargetValueValid ? Uint8Opt( targetValue ) : Uint8Opt(),
            isTargetValueValid ? utils::actuatorReportTimeToSeconds( duration ) : SecondsOpt() );
    }
}

void ZNetImpl::znetNodeCmdMultilevelSwitchSetDone( int err,
                                                   znet_node_id_t nodeId,
                                                   znet_node_channel_id_t channelId,
                                                   void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ptr->getHandler( nodeId ).onNodeFailed( nodeId, err );
        }
        else {
            ZNET_LOGI( "%s (node id: %hhu, channel id: %hhu)\n", __func__, nodeId, channelId );
            ptr->getHandler( nodeId ).onNodeCmdMultilevelSwitchSetDone( nodeId, channelId );
        }
    }
}

void ZNetImpl::znetNodeCmdMultilevelSwitchSupported( int err,
                                                     znet_node_id_t node_id,
                                                     znet_node_channel_id_t channel_id,
                                                     const znet_multilevel_switch_supported_report_t* value,
                                                     void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu, channel id: %hhu)\n", __func__, node_id, channel_id );
        ptr->getHandler( node_id ).onNodeCmdMultilevelSwitchSupported(
            node_id, channel_id, value->primary_type, value->secondary_type );
    }
}

void ZNetImpl::znetNodeCmdBarrierOperator( int err,
                                           znet_node_id_t nodeId,
                                           znet_node_channel_id_t channelId,
                                           znet_cmd_barrier_operator_value_t value,
                                           void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu, channel id: %hhu) %hhu\n", __func__, nodeId, channelId, value );
        ptr->getHandler( nodeId ).onNodeCmdBarrierOperator( nodeId, channelId, value );
    }
}

void ZNetImpl::znetNodeCmdBarrierOperatorSignal( int err,
                                                 znet_node_id_t nodeId,
                                                 znet_node_channel_id_t channelId,
                                                 znet_cmd_barrier_operator_signal_t signalId,
                                                 znet_cmd_barrier_operator_signal_state_t state,
                                                 void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu, channel id: %hhu) signal id: %hhu, state: %hhu\n",
                   __func__,
                   nodeId,
                   channelId,
                   signalId,
                   state );
        ptr->getHandler( nodeId ).onNodeCmdBarrierOperatorSignal( nodeId, channelId, signalId, state );
    }
}

void ZNetImpl::znetNodeCmdBarrierOperatorSupportedSignals( int err,
                                                           znet_node_id_t nodeId,
                                                           znet_node_channel_id_t channelId,
                                                           const uint8_t* data,
                                                           uint8_t len,
                                                           void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu, channel id: %hhu)\n", __func__, nodeId, channelId );
        ptr->getHandler( nodeId ).onNodeCmdBarrierOperatorSupportedSignals( nodeId, channelId, data, len );
    }
}

void ZNetImpl::znetNodeCmdBarrierOperatorSetDone( int err,
                                                  znet_node_id_t nodeId,
                                                  znet_node_channel_id_t channelId,
                                                  void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ptr->getHandler( nodeId ).onNodeFailed( nodeId, err );
        }
        else {
            ZNET_LOGI( "%s (node id: %hhu, channel id: %hhu)\n", __func__, nodeId, channelId );
            ptr->getHandler( nodeId ).onNodeCmdBarrierOperatorSetDone( nodeId, channelId );
        }
    }
}

void ZNetImpl::znetNodeCmdAlarmSensorGetReport( znet_error_t err,
                                                znet_node_id_t nodeId,
                                                znet_node_channel_id_t channel_id,
                                                uint8_t sensorType,
                                                uint8_t value,
                                                uint16_t seconds,
                                                void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onNodeCmdAlarmSensor( nodeId, channel_id, sensorType, value, seconds );
    }
}

void ZNetImpl::znetNodeCmdAlarmSensorSupportedSensor( znet_error_t err,
                                                      znet_node_id_t nodeId,
                                                      znet_node_channel_id_t channel_id,
                                                      const uint8_t* data,
                                                      uint8_t len,
                                                      void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onNodeCmdAlarmSensorSupported( nodeId, channel_id, data, len );
    }
}

void ZNetImpl::znetNodeCmdBinarySensorGetReport( znet_error_t err,
                                                 znet_node_id_t nodeId,
                                                 znet_node_channel_id_t channel_id,
                                                 uint8_t sensorType,
                                                 uint8_t value,
                                                 void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onNodeCmdBinarySensor( nodeId, channel_id, value, sensorType );
    }
}

void ZNetImpl::znetNodeCmdBinarySensorSupportedSensor( znet_error_t err,
                                                       znet_node_id_t nodeId,
                                                       znet_node_channel_id_t channel_id,
                                                       const uint8_t* data,
                                                       uint8_t len,
                                                       void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onNodeCmdBinarySensorSupported( nodeId, channel_id, data, len );
    }
}

void ZNetImpl::znetNodeCmdSensorMultilevelSupportedSensor( znet_error_t err,
                                                           znet_node_id_t nodeId,
                                                           znet_node_channel_id_t channelId,
                                                           const uint8_t* data,
                                                           uint8_t len,
                                                           void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u, channelId: %u, bitmask: %s\n",
                   __func__,
                   nodeId,
                   channelId,
                   utils::bitmaskToString( data, len ).c_str() );

        ptr->getHandler( nodeId ).onNodeCmdSensorMultilevelSupportedSensor( nodeId, channelId, data, len );
    }
}

void ZNetImpl::znetNodeCmdSensorMultilevelSupportedScale( znet_error_t err,
                                                          znet_node_id_t nodeId,
                                                          znet_node_channel_id_t channelId,
                                                          uint8_t sensor_type,
                                                          uint8_t scale_mask,
                                                          void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u, channelId: %u\n", __func__, nodeId, channelId );
        ptr->getHandler( nodeId ).onNodeCmdSensorMultilevelSupportedScale( nodeId, channelId, sensor_type, scale_mask );
    }
}

void ZNetImpl::znetNodeCmdSensorMultilevelGetReport( znet_error_t err,
                                                     znet_node_id_t nodeId,
                                                     znet_node_channel_id_t channelId,
                                                     uint8_t type,
                                                     const znet_multilevel_sensor_report_t* value,
                                                     void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u, channelId: %u\n", __func__, nodeId, channelId );
        ptr->getHandler( nodeId ).onNodeCmdSensorMultilevel( nodeId, channelId, value );
    }
}

void ZNetImpl::znetNodeCmdMeter( int err,
                                 znet_node_id_t nodeId,
                                 znet_node_channel_id_t channelId,
                                 uint8_t scale,
                                 const znet_meter_report_t* value,
                                 void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        if( value ) {
            ZNET_LOGI( "%s (node id: %hhu, channel id: %hhu, type: %u, scale: %u, value: %i)\n",
                       __func__,
                       nodeId,
                       channelId,
                       value->type,
                       value->scale,
                       value->value );

            ptr->getHandler( nodeId ).onNodeCmdMeter( nodeId,
                                                      channelId,
                                                      static_cast<meter::MeterType>( value->type ),
                                                      value->scale,
                                                      value->value,
                                                      value->precision );
        }
    }
}

void ZNetImpl::znetNodeCmdMeterSetDone( int err, znet_node_id_t nodeId, znet_node_channel_id_t channelId, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ptr->getHandler( nodeId ).onNodeFailed( nodeId, err );
        }
        else {
            ZNET_LOGI( "%s node: %u, channelId: %u\n", __func__, nodeId, channelId );
            ptr->getHandler( nodeId ).onNodeCmdMeterSetDone( nodeId, channelId );
        }
    }
}

void ZNetImpl::znetNodeCmdMeterSupported( int err,
                                          znet_node_id_t nodeId,
                                          znet_node_channel_id_t channelId,
                                          const znet_meter_supported_report_t* value,
                                          void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u, channelId: %u\n", __func__, nodeId, channelId );
        if( value != nullptr ) {
            ptr->getHandler( nodeId ).onNodeCmdMeterSupported( nodeId,
                                                               channelId,
                                                               value->reset,
                                                               static_cast<meter::MeterType>( value->type ),
                                                               value->scales_bitmask_size,
                                                               value->scales_bitmask );
        }
    }
}

void ZNetImpl::znetNodeCmdMultichannelEndpoint( int err,
                                                znet_node_id_t nodeId,
                                                const znet_multichannel_endpoint_report_t* value,
                                                void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );

        if( value != nullptr ) {
            ptr->getHandler( nodeId ).onNodeCmdMultichannelEndpoint( nodeId,
                                                                     value->agregated_channel_ids_count,
                                                                     value->channel_ids_count,
                                                                     value->identical,
                                                                     value->dynamic );
        }
    }
}

void ZNetImpl::znetNodeCmdMultichannelCapability( int err,
                                                  znet_node_id_t nodeId,
                                                  znet_node_channel_id_t channel_id,
                                                  const znet_multichannel_capability_report_t* value,
                                                  void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        if( value != nullptr ) {
            ZNET_LOGI( "%s node: %u, channelId: %u\n", __func__, nodeId, value->channel_id );
            ptr->getHandler( nodeId ).onNodeCmdMultichannelCapability( nodeId,
                                                                       value->channel_id,
                                                                       value->dynamic,
                                                                       value->generic,
                                                                       value->specific,
                                                                       value->commands_count,
                                                                       value->commands );
        }
    }

    /*    if( err ) {
            ZNET_LOGE( "multichannel capability report for node fail\n" );
        }
        else {
            ZNET_LOGI( "multichannel capability report (node id: %hhu)\n", nodeId );
            //        esp_log_buffer_hexdump_internal(
            //            __func__, value, sizeof( znet_multichannel_capability_report_t ), ESP_LOG_INFO );
        }*/
}

void ZNetImpl::znetNodeCmdMultichannelEndpointFind( int err,
                                                    znet_node_id_t nodeId,
                                                    const znet_multichannel_endpoint_find_report_t* value,
                                                    void* arg ) {
    if( err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u end\n", __func__, nodeId );
    }
}

void ZNetImpl::znetNodeCmdMultichannelAggregatedMembers( int err,
                                                         znet_node_id_t nodeId,
                                                         znet_node_channel_id_t channel_id,
                                                         const znet_multichannel_aggregated_members_report_t* value,
                                                         void* arg ) {
    if( err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u end\n", __func__, nodeId );
    }
}

void ZNetImpl::znetNodeCmdConfiguration( znet_error_t err,
                                         znet_node_id_t nodeId,
                                         znet_node_channel_id_t channel_id,
                                         uint8_t parameter_number,
                                         const znet_configuration_report_t* value,
                                         void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        if( value == nullptr ) {
            return;
        }

        ZNET_LOGI( "%s (node id: %hhu, channel id: %hhu, param: %d, value: %d)\n",
                   __func__,
                   nodeId,
                   channel_id,
                   parameter_number,
                   value->value );

        ptr->getHandler( nodeId ).onConfigurationReport(
            nodeId, channel_id, parameter_number, value->data_count, value->value );
    }
}

void ZNetImpl::znetNodeCmdConfigurationRequestStatus( znet_error_t err,
                                                      znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      uint8_t parameter_number,
                                                      void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );

    if( ptr != nullptr ) {
        const auto status = RequestStatus::fromStatus( err );
        ptr->getHandler( node_id ).onConfigurationRequestStatus( node_id, channel_id, parameter_number, status );
    }
}

void ZNetImpl::znetNodeCmdConfigurationBulk( int err,
                                             znet_node_id_t nodeId,
                                             znet_node_channel_id_t channel_id,
                                             znet_cmd_configuration_id_t param_number,
                                             uint8_t config_count,
                                             const znet_configuration_bulk_report_t* value,
                                             void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        if( value == nullptr ) {
            return;
        }

        ZNET_LOGI( "%s (node id: %hhu, channel id: %hhu, param: %d, value: %d)\n",
                   __func__,
                   nodeId,
                   channel_id,
                   param_number,
                   value->values[0] );

        // TODO
        ptr->getHandler( nodeId ).onConfigurationReport(
            nodeId, channel_id, param_number, value->param_size, value->values[0] );
    }
}

void ZNetImpl::znetNodeCmdConfigurationName( int err,
                                             znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             znet_cmd_configuration_id_t param_number,
                                             const znet_configuration_name_report_t* value,
                                             void* arg ) {
    if( err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u, channelId: %u\n", __func__, nodeId, channelId );
    }
}

void ZNetImpl::znetNodeCmdConfigurationInfo( int err,
                                             znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             znet_cmd_configuration_id_t param_number,
                                             const znet_configuration_info_report_t* value,
                                             void* arg ) {
    if( err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u, channelId: %u\n", __func__, nodeId, channelId );
    }
}

void ZNetImpl::znetNodeCmdConfigurationProperties( int err,
                                                   znet_node_id_t nodeId,
                                                   znet_node_channel_id_t channelId,
                                                   znet_cmd_configuration_id_t param_number,
                                                   const znet_configuration_properties_report_t* value,
                                                   void* arg ) {
    if( err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u, channelId: %u\n", __func__, nodeId, channelId );
    }
}

void ZNetImpl::znetNodeCmdPowerlevel( int err,
                                      znet_node_id_t nodeId,
                                      const znet_powerlevel_report_t* value,
                                      void* arg ) {
    if( err ) {
        ZNET_LOGE( "%s fail node: %hhu\n", __func__, nodeId );
    }
    else {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        // esp_log_buffer_hexdump_internal( __func__, value, sizeof( znet_powerlevel_report_t ), ESP_LOG_INFO );
    }
}

void ZNetImpl::znetNodeCmdPowerlevelTestNode( int err,
                                              znet_node_id_t nodeId,
                                              const znet_powerlevel_test_node_report_t* value,
                                              void* arg ) {
    if( err ) {
        ZNET_LOGE( "%s fail node: %hhu\n", __func__, nodeId );
    }
    else {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
    }
}

void ZNetImpl::znetNodeCmdSceneActuatorConfiguration( int err,
                                                      znet_node_id_t nodeId,
                                                      znet_node_channel_id_t channel_id,
                                                      znet_cmd_scene_id_t sceneId,
                                                      const znet_scene_actuator_configuration_report_t* value,
                                                      void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        //            esp_log_buffer_hexdump_internal(
        //                __func__, value, sizeof( scene_actuator_conf_report_t ) + value->scount,
        //                ESP_LOG_INFO );

        if( value != nullptr ) {
            ptr->getHandler( nodeId ).onNodeCmdSceneActuatorConfiguration(
                nodeId, channel_id, value->scene_id, value->level, value->dimming_duration );
        }
    }
}

void ZNetImpl::znetNodeCmdSceneControllerConfiguration( int err,
                                                        znet_node_id_t nodeId,
                                                        znet_node_channel_id_t channel_id,
                                                        const znet_scene_controller_configuration_report_t* value,
                                                        void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ZNET_LOGE( "%s fail node: %hhu\n", __func__, nodeId );
            ptr->getHandler( nodeId ).onNodeFailed( nodeId, err );
        }
        else {
            ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
            //            esp_log_buffer_hexdump_internal(
            //                __func__, value, sizeof( znet_scene_controller_conf_report_t ), ESP_LOG_INFO );

            if( value != nullptr ) {
                ptr->getHandler( nodeId ).onNodeCmdSceneControllerConfiguration(
                    nodeId, channel_id, value->group_id, value->scene_id, value->dimming_duration );
            }
        }
    }
}

void ZNetImpl::znetNodeCmdCentralSceneConfiguration( int err,
                                                     znet_node_id_t nodeId,
                                                     znet_node_channel_id_t channel_id,
                                                     znet_cmd_central_scene_value_t value,
                                                     void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ZNET_LOGE( "%s fail node: %hhu\n", __func__, nodeId );
        }
        else {
            ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
            //            esp_log_buffer_hexdump_internal(
            //                __func__, value, sizeof( znet_scene_controller_configuration_report_t ), ESP_LOG_INFO );

            ptr->getHandler( nodeId ).onCentralSceneConfiguration( nodeId, channel_id, value );
        }
    }
}

void ZNetImpl::znetNodeCmdCentralSceneSupported( int err,
                                                 znet_node_id_t nodeId,
                                                 znet_node_channel_id_t channel_id,
                                                 const znet_central_scene_supported_report_t* value,
                                                 void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s:\n"
                   "  node id: %hhu\n"
                   "  channel id: %hhu\n"
                   "  scenes number: %hhu\n"
                   "  data size: %hhu\n"
                   "  identical: %hhu\n"
                   "  slow_refresh: %hhu\n",
                   __func__,
                   nodeId,
                   channel_id,
                   value->scenes_num,
                   value->data_size,
                   value->identical,
                   value->slow_refresh );
        ptr->getHandler( nodeId ).onNodeCmdCentralSceneSupported( nodeId,
                                                                  channel_id,
                                                                  value->scenes_num,
                                                                  value->data_size,
                                                                  value->identical,
                                                                  value->slow_refresh,
                                                                  value->data );
    }
}

void ZNetImpl::znetCentralSceneNotification( int err,
                                             znet_node_id_t nodeId,
                                             const znet_central_scene_notification_t* value,
                                             void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ZNET_LOGE( "%s fail node: %hhu\n", __func__, nodeId );
        }
        else {
            ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
            if( value != nullptr ) {
                ptr->getHandler( nodeId ).onCentralSceneNotification(
                    nodeId, value->sequence_number, value->slow_refresh, value->key_attribute, value->scene_number );
            }
        }
    }
}

void ZNetImpl::znetSceneActivationSetResult( znet_error_t err,
                                             znet_node_id_t nodeId,
                                             const znet_scene_activation_t sceneNumber,
                                             void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ZNET_LOGE( "%s fail node: %hhu\n", __func__, nodeId );
        }
        else {
            ZNET_LOGI( "%s (node id: %hhu, scene number: %hhu)\n", __func__, nodeId, sceneNumber );
            ptr->getHandler( nodeId ).onSceneActivationSetResult( nodeId, sceneNumber );
        }
    }
}

void ZNetImpl::znetNodeNoOperation( znet_error_t err, const znet_node_id_t nodeId, void* arg ) {
    ZNET_LOGD( "%u node reachable %s, err = %u\n",
               nodeId,
               err == ZNET_ERR_REQUEST_SENT_OK ? "true" : err == ZNET_ERR_UNREACH ? "false" : "unknown",
               err );

    if( auto ptr = static_cast<ZNetImpl*>( arg ) ) {
        ptr->getHandler().onNodeNoOperation( nodeId, err );
    }
}

void ZNetImpl::znetApplicationNodeUpdate( znet_error_t err,
                                          znet_node_id_t node_id,
                                          const znet_nodeinfo_t* node_info,
                                          void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            if( node_info == nullptr ) {
                ptr->getHandler( 0 ).onNodeFailed( 0, err );
                return;
            }

            ptr->getHandler( node_info->node_id ).onNodeFailed( node_info->node_id, err );
        }
        else {
            if( node_info == nullptr ) {
                ZNET_LOGE( "%s fail. Data is corrupted\n", __func__ );
                return;
            }

            ZNET_LOGI( "%s node: %u\n", __func__, node_id );
            ptr->getHandler( node_info->node_id ).onApplicationNodeUpdate( node_info );
        }
    }
}

void ZNetImpl::znetWakeupResult( znet_error_t err,
                                 znet_node_id_t nodeId,
                                 znet_node_channel_id_t channelId,
                                 znet_cmd_wakeup_value_t value,
                                 znet_node_id_t target_node_id,
                                 void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node id: %hhu, value: %u, target_node_id: %hhu\n", __func__, nodeId, value, target_node_id );
        ptr->getHandler( nodeId ).onWakeUpResult( nodeId, value, target_node_id );
    }
}

void ZNetImpl::znetWakeupNotification( znet_error_t err, znet_node_id_t nodeId, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ZNET_LOGE( "%s fail node: %hhu\n", __func__, nodeId );
        }
        else {
            ZNET_LOGI( "%s node: %u\n", __func__, nodeId );

            ptr->getHandler( nodeId ).onWakeupNotification( nodeId );
        }
    }
}

void ZNetImpl::znetWakeupCapabilitiesResult( znet_error_t err,
                                             znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             const znet_wakeup_capabilities_report_t* value,
                                             void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s\n"
                   " node id: %hhu"
                   ", minumum wakeup interval %u"
                   ", maximum wakeup interval %u"
                   ", default wakeup interval %u"
                   ", step wakeup interval %u\n",
                   __func__,
                   nodeId,
                   value->min_wakeup_interval,
                   value->max_wakeup_interval,
                   value->default_wakeup_interval,
                   value->step_wakeup_interval );

        ptr->getHandler( nodeId ).onWakeupCapabilitiesResult( nodeId,
                                                              value->min_wakeup_interval,
                                                              value->max_wakeup_interval,
                                                              value->default_wakeup_interval,
                                                              value->step_wakeup_interval );
    }
}

void ZNetImpl::znetAssociationGroupNameResult( int err,
                                               znet_node_id_t nodeId,
                                               znet_node_channel_id_t channelId,
                                               znet_grouping_id_t grouping_id,
                                               const znet_association_group_name_report_t* value,
                                               void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu, group id: %d)\n", __func__, nodeId, value->grouping_id );

        ptr->getHandler( nodeId ).onAssociationGroupNameResult( nodeId, ZNET_CHANNEL_ID_ROOT, value );
    }
}

void ZNetImpl::znetAssociationGroupInfoResult( int err,
                                               znet_node_id_t nodeId,
                                               znet_node_channel_id_t channelId,
                                               znet_grouping_id_t grouping_id,
                                               const znet_association_group_info_report_t* value,
                                               void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );

        ptr->getHandler( nodeId ).onAssociationGroupInfoResult( nodeId, ZNET_CHANNEL_ID_ROOT, value );
    }
}

void ZNetImpl::znetAssociationSupportedGroupingResult( int err,
                                                       znet_node_id_t nodeId,
                                                       znet_node_channel_id_t channelId,
                                                       const uint8_t value,
                                                       void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );

        ptr->getHandler( nodeId ).onAssociationSupportedGroupingResult( nodeId, ZNET_CHANNEL_ID_ROOT, value );
    }
}

void ZNetImpl::znetAssociationGroupCommandListResult( int err,
                                                      znet_node_id_t nodeId,
                                                      znet_node_channel_id_t channelId,
                                                      znet_grouping_id_t grouping_id,
                                                      const znet_association_group_command_list_report_t* value,
                                                      void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu, group id: %d)\n", __func__, nodeId, value->grouping_id );

        ptr->getHandler( nodeId ).onAssociationGroupCommandListResult( nodeId, ZNET_CHANNEL_ID_ROOT, value );
    }
}

void ZNetImpl::znetAssociationResult( int err,
                                      znet_node_id_t nodeId,
                                      znet_node_channel_id_t channelId,
                                      znet_grouping_id_t grouping_id,
                                      const znet_association_report_t* value,
                                      void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu, group id: %d)\n", __func__, nodeId, value->grouping_id );

        ptr->getHandler( nodeId ).onAssociationResult( nodeId, channelId, value );
    }
}

void ZNetImpl::znetAssociationSetResult( znet_error_t err,
                                         znet_node_id_t nodeId,
                                         znet_node_channel_id_t channelId,
                                         znet_grouping_id_t grouping_id,
                                         void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onAssociationSetResult( nodeId, channelId );
    }
}

void ZNetImpl::znetAssociationRemoveResult( znet_error_t err,
                                            znet_node_id_t nodeId,
                                            znet_node_channel_id_t channelId,
                                            void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ZNET_LOGE( "%s fail node: %hhu\n", __func__, nodeId );
        }
        else {
            ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
            ptr->getHandler( nodeId ).onAssociationRemoveResult( nodeId, channelId );
        }
    }
}

void ZNetImpl::znetMultichannelAssociationResult( int err,
                                                  znet_node_id_t nodeId,
                                                  znet_node_channel_id_t channelId,
                                                  znet_grouping_id_t groupingId,
                                                  const znet_multichannel_association_report_t* value,
                                                  void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI(
            "%s (node id: %hhu, channelId: %hhu, group id: %d)\n", __func__, nodeId, channelId, value->grouping_id );

        ptr->getHandler( nodeId ).onMultichannelAssociationResult( nodeId, channelId, value );
    }
}

void ZNetImpl::znetMultichannelAssociationSetResult( znet_error_t err,
                                                     znet_node_id_t nodeId,
                                                     znet_node_channel_id_t channelId,
                                                     znet_grouping_id_t groupingId,
                                                     void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onMultichannelAssociationSetResult( nodeId, channelId );
    }
}

void ZNetImpl::znetMultichannelAssociationRemoveResult( znet_error_t err,
                                                        znet_node_id_t nodeId,
                                                        znet_node_channel_id_t channelId,
                                                        void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ZNET_LOGE( "%s fail node: %hhu\n", __func__, nodeId );
        }
        else {
            ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
            ptr->getHandler( nodeId ).onMultichannelAssociationRemoveResult( nodeId, channelId );
        }
    }
}

void ZNetImpl::znetMultichannelAssociationSupportedGroupingResult( int err,
                                                                   znet_node_id_t nodeId,
                                                                   znet_node_channel_id_t channelId,
                                                                   const uint8_t value,
                                                                   void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );

        ptr->getHandler( nodeId ).onMultichannelAssociationSupportedGroupingResult( nodeId, channelId, value );
    }
}

void ZNetImpl::znetSimpleAvControlSetResult( znet_error_t err,
                                             znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             const znet_simple_av_set_report_t* value,
                                             void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );

        ptr->getHandler( nodeId ).onSimpleAvControlSetResult( nodeId, channelId, value );
    }
}

void ZNetImpl::znetThermostatModeResult( int err,
                                         znet_node_id_t nodeId,
                                         znet_node_channel_id_t channelId,
                                         const znet_thermostat_mode_report_t* value,
                                         void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        if( value ) {
            ptr->getHandler( nodeId ).onThermostatModeResult( nodeId, channelId, *value );
        }
    }
}

void ZNetImpl::znetThermostatModeSupportedResult( int err,
                                                  znet_node_id_t nodeId,
                                                  znet_node_channel_id_t channelId,
                                                  const znet_thermostat_mode_supported_report_t* value,
                                                  void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onThermostatModeSupportedResult( nodeId, channelId, value->mcount, value->bit_mask );
    }
}

void ZNetImpl::znetThermostatOperatingStateResult( int err,
                                                   znet_node_id_t nodeId,
                                                   znet_node_channel_id_t channelId,
                                                   const uint8_t value,
                                                   void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onThermostatOperatingStateResult( nodeId, value );
    }
}

void ZNetImpl::znetThermostatOperatingStateLoggingSupportedResult(
    int err,
    znet_node_id_t nodeId,
    znet_node_channel_id_t channelId,
    const znet_thermostat_operating_state_logging_supported_report_t* value,
    void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onThermostatOperatingStateLoggingSupportedResult(
            nodeId, channelId, value->opscount, value->bit_mask );
    }
}

void ZNetImpl::znetThermostatOperatingStateLoggingResult( int err,
                                                          znet_node_id_t nodeId,
                                                          znet_node_channel_id_t channelId,
                                                          znet_thermostat_operating_state_logging_report_t* value,
                                                          void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        // TODO: uncoment code below after appopriate handler implementation
        // ptr->getHandler(nodeId).onThermostatOperatingStateLoggingResult( nodeId, channelId, value->opscount,
        // value->bit_mask );
    }
}

void ZNetImpl::znetThermostatFanModeResult( int err,
                                            znet_node_id_t nodeId,
                                            znet_node_channel_id_t channelId,
                                            const uint8_t value,
                                            void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onThermostatFanModeResult( nodeId, value );
    }
}

void ZNetImpl::znetThermostatFanModeSupportedResult( int err,
                                                     znet_node_id_t nodeId,
                                                     znet_node_channel_id_t channelId,
                                                     const znet_thermostat_fan_mode_supported_report_t* value,
                                                     void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onThermostatFanModeSupportedResult(
            nodeId, channelId, value->fmcount, value->bit_mask );
    }
}

void ZNetImpl::znetThermostatFanStateResult( int err,
                                             znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             const uint8_t value,
                                             void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onThermostatFanStateResult( nodeId, value );
    }
}

void ZNetImpl::znetThermostatSetpointResult( int err,
                                             znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             uint8_t type,
                                             const znet_thermostat_setpoint_report_t* value,
                                             bool requested,
                                             void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ptr->getHandler( nodeId ).onThermostatSetpointResult(
            nodeId, channelId, value->type, value->scale, value->size, value->precision, value->sp_value, requested );
    }
}

void ZNetImpl::znetThermostatSetpointSupportedResult( int err,
                                                      znet_node_id_t nodeId,
                                                      znet_node_channel_id_t channelId,
                                                      const znet_thermostat_setpoint_supported_report_t* value,
                                                      void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onThermostatSetpointSupportedResult(
            nodeId, channelId, value->stcount, value->bit_mask );
    }
}

void ZNetImpl::znetThermostatSetpointCapabilitiesResult( int err,
                                                         znet_node_id_t nodeId,
                                                         znet_node_channel_id_t channelId,
                                                         uint8_t type,
                                                         const znet_thermostat_setpoint_capabilities_report_t* value,
                                                         void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s node: %u\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onThermostatSetpointCapabilitiesResult(
            nodeId, channelId, value->type, value->min_params, value->min_value, value->max_params, value->max_value );
    }
}

void ZNetImpl::znetThermostatSetbackResult( int err,
                                            znet_node_id_t nodeId,
                                            znet_node_channel_id_t channelId,
                                            const uint8_t type,
                                            const int8_t state,
                                            void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s (node id: %hhu), type(%d), state(%d)\n", __func__, nodeId, type, state );
        const auto modeAndValue = thermostat_setback::fromRawValue( state );
        ptr->getHandler( nodeId ).onThermostatSetbackResult( nodeId,
                                                             channelId,
                                                             static_cast<thermostat_setback::OverrideType>( type ),
                                                             modeAndValue.first,
                                                             modeAndValue.second );
    }
}

IZNetOwner& ZNetImpl::getOwner() {
    return m_znetOwner;
}

::znet::api::IProvisioningListController& ZNetImpl::getProvisioningListController() {
    return m_provisioningListController;
}

::znet::api::IProvisioningListQueryDataSource& ZNetImpl::getProvisioningListControllerDataSource() {
    return m_provisioningListControllerDataSource;
}

::znet::api::IProvisioningListQueryDataSource& ZNetImpl::getClassicInclusionSecurityDataSource() {
    return m_classicInclusionSecurityDataSource;
}

void ZNetImpl::resetDeviceSecurityData( uint8_t nodeId, ::znet::ProvisioningListEntryFields::Status status ) {
    using fields_t = ::znet::ProvisioningListEntryFields;
    ::znet::api::ProvisioningListQuery{ getProvisioningListControllerDataSource() }
        .filterBy( fields::nodeId( nodeId ) )
        .setField( &fields_t::m_status, status )
        .setField( &fields_t::m_nodeSecurityModes, utils::optional<uint8_t>{} )
        .setField( &fields_t::m_node, ZNET_NODE_ID_ANY )
        .execute();
    ::znet::api::ProvisioningListQuery{ getClassicInclusionSecurityDataSource() }
        .filterBy( fields::nodeId( nodeId ) )
        .remove()
        .execute();
}

TimeManager& ZNetImpl::getTimeManager() {
    return m_timeManager;
}

NodeProvisioningManager& ZNetImpl::getNodeProvisioningManager() {
    return m_nodeProvisioningManager;
}

RequestsManager& ZNetImpl::getRequestsManager() {
    return m_requestsManager;
}

ReachableCheckTimeoutSettings& ZNetImpl::getReachableCheckTimeoutSettings() {
    return m_reachableTimeoutSettings;
}

CommandClassValuesSetter& ZNetImpl::getCommandClassValuesSetter() {
    return m_ccValuesSetter;
}

void ZNetImpl::checkReachableNodesState() {
    using namespace std::chrono;

    const auto now = steady_clock::now();
    auto reachableCheckTimeoutSeconds =
        std::min( { m_reachableTimeoutSettings.getWakeUpInterval(),
                    m_reachableTimeoutSettings.getAlwaysOnDevicesInterval(),
                    static_cast<uint32_t>( duration_cast<seconds>( AllNodesReachableCheckTimeout ).count() ) } );

    const auto elapsedTime = now - m_lastNodeStatusCheckingTime;
    if( duration_cast<seconds>( elapsedTime ) < seconds{ reachableCheckTimeoutSeconds } )
        return;
    if( duration_cast<TimeDurationUnitHours>( elapsedTime ) > CheckTimeoutHoursPerDay ) {
        m_lastNodeStatusCheckingTime = now;  // this is needed because NTP update system time
        return;
    }

    const auto nodes = getOwner().getNodeIds();
    bool nopSent = false;

    utils::forEachBit( nodes.mask, nodes.Size, [&]( znet_node_id_t nodeId ) {
        if( auto node = onFindNode( nodeId ) ) {
            if( node->isWakeup( true ) ) {
                const auto wakeUpInterval = node->getWakeUpInterval();
                if( is_self_node_sis_or_primary() && wakeUpInterval && *wakeUpInterval != 0 ) {
                    const seconds checkingInterval( *wakeUpInterval * m_reachableTimeoutSettings.getAttemptsCount() );
                    const auto inactiveInterval = duration_cast<seconds>( now - node->getLastUpdateTime() );
                    if( inactiveInterval > checkingInterval ) {
                        ZNET_LOGI( "Set unreachable for wakeup node %u, inactive interval %u, checking interval %u\n",
                                   nodeId,
                                   inactiveInterval.count(),
                                   checkingInterval.count() );
                        setReachableState( nodeId, false );
                    }
                }
                // else don't set unreachable state
            }
            else if( !nopSent && now - node->getLastUpdateTime() >
                                     std::chrono::seconds{ m_reachableTimeoutSettings.getAlwaysOnDevicesInterval() } ) {
                nopSent = sendNop( nodeId );
                // next nodes will be checked next time
            }
        }
    } );

    m_lastNodeStatusCheckingTime = now;
}

void ZNetImpl::setReachableState( const znet_node_id_t nodeId, bool reachable, bool forceNotify ) {
    if( auto node = onFindNode( nodeId ) ) {
        const auto prevReachableState = node->isReachable();
        node->setReachable( reachable ? ReachableState::Reachable : ReachableState::Unreachable );

        if( prevReachableState != node->isReachable() || forceNotify ) {
            m_znetOwner.onUpdateReachable( nodeId, reachable );
        }

        if( prevReachableState == ReachableState::Unreachable && reachable ) {
            requestNodeValues( nodeId, false );
        }
    }
}

void ZNetImpl::updateRediscoverProgress( const znet_node_id_t nodeId,
                                         const bool isOk,
                                         ErrorOpt errorMessage,
                                         const uint8_t progress ) {
    getOwner().onDeviceRediscoverProgressUpdate( nodeId, isOk, std::move( errorMessage ), progress );
}

void ZNetImpl::updateInclusionProgress( const uint8_t progress ) {
    getOwner().onDeviceIncludeProgressUpdate( progress );
}

void ZNetImpl::updateLearnProgress( uint8_t devicesAmount, uint8_t currentDeviceIndex ) {
    getOwner().onLearnProgressUpdate( devicesAmount, currentDeviceIndex );
}

void ZNetImpl::updateReconstructNetworkProgress( const uint8_t devicesAmount, const uint8_t currentDeviceIndex ) {
    getOwner().onReconstructNetworkUpdate( devicesAmount, currentDeviceIndex );
}

void ZNetImpl::updateReconstructNetworkError( const znet_node_id_t deviceId ) {
    getOwner().onReconstructNetworkError( deviceId );
}

int ZNetImpl::znetUartWrite( const void* data, size_t size, size_t* ret_size, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        return ptr->getOwner().znetUartWrite( data, size, ret_size );
    }
    return -1;
}

int ZNetImpl::znetUartRead( void* data, size_t size, size_t* ret_size, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        return ptr->getOwner().znetUartRead( data, size, ret_size );
    }
    return -1;
}

int ZNetImpl::znetStoreSave( size_t offset, const void* data, size_t size, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        return ptr->getOwner().znetStoreSave( offset, data, size );
    }
    return -1;
}

int ZNetImpl::znetStoreLoad( size_t offset, void* data, size_t size, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        return ptr->getOwner().znetStoreLoad( offset, data, size );
    }
    return -1;
}

int ZNetImpl::znetStoreReset( void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        return ptr->getOwner().znetStoreReset();
    }
    return -1;
}

bool ZNetImpl::isPrimaryController() const {
    return znet_get_self_node_id() == 1;  // TODO  gt primary ID from chip
}

bool ZNetImpl::isIncludeExcludeAvailable() const {
    znet_node_id_t SIS = znet_get_sis_node_id();
    bool isSISSetuped = SIS != ZNET_NODE_ID_ANY && SIS != ZNET_NODE_ID_INVALID;
    return isSISSetuped ? true : isPrimaryController();
}

void ZNetImpl::znetZwavePlusInfoGet( void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->getHandler().onZwavePlusInfoGet();
    }
}

void ZNetImpl::znetIndicatorIdentify( void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->getHandler().onIndicatorIdentify();
    }
}

void ZNetImpl::znetIndicatorReport( znet_error_t err,
                                    znet_node_id_t nodeId,
                                    znet_node_channel_id_t channelId,
                                    uint8_t indicatorId,
                                    const znet_indicator_report_t* value,
                                    void* arg ) {
    // Nothing to do
}

void ZNetImpl::znetIndicatorSupportedReport( znet_error_t err,
                                             znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             uint8_t indicatorId,
                                             const znet_indicator_supported_report_t* value,
                                             void* arg ) {
    // Nothing to do
}

znet_error_t ZNetImpl::znetNodeIsAdded( znet_node_id_t node_id, void* arg ) {
    if( auto ptr = static_cast<ZNetImpl*>( arg ) ) {
        if( auto node = ptr->onFindNode( node_id ) ) {
            // command classes are added on node adding but node is clear on rediscover starting
            // TODO: specify unambiguous criteria
            if( node->isRediscoverStarted() || node->getCommandClassCount( ZNET_CHANNEL_ID_ROOT ) > 0 ) {
                return ZNET_ERR_OK;
            }
        }
    }
    return ZNET_ERR_FAULT;
}

uint8_t ZNetImpl::znetSecurityGet( znet_node_id_t node_id, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ZNode* node = ptr->onFindNode( node_id );
        if( node != nullptr )
            return SecurityMode::toByte( node->getSecurity() );
    }
    return ZNET_NODEINFO_SECURITY_NONE;
}

int ZNetImpl::znetSecuritySet( znet_node_id_t node_id, uint8_t security, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ptr->getHandler().onNodeAddSecurityDone( SecurityMode::fromByte( security ) );
        return 0;
    }
    return -1;
}

znet_error_t ZNetImpl::znetNodeIsSupportedCC( znet_node_id_t node_id, znet_command_class_t command, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ZNode* node = ptr->onFindNode( node_id );
        if( node != nullptr ) {
            ZNode::ClassPtr ccTemp = node->getCommandClass( ZNET_CHANNEL_ID_ROOT, command );
            if( ccTemp ) {
                return ZNET_ERR_OK;
            }
        }
    }
    return ZNET_ERR_FAULT;
}

int ZNetImpl::znetNodeIsWakeup( znet_node_id_t node_id, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ZNode* node = ptr->onFindNode( node_id );
        if( node != nullptr ) {
            return node->isWakeup();
        }
    }
    return 0;
}

bool ZNetImpl::isBusy( IZNet::Mode except ) const {
    if( ( m_state.m_cmd == IZNet::Mode::IncludeStarting || m_state.m_cmd == IZNet::Mode::ExcludeStarting ||
          m_state.m_cmd == IZNet::Mode::LearnStarting || m_state.m_cmd == IZNet::Mode::RequestNeighbor ||
          m_state.m_cmd == IZNet::Mode::Rediscover || m_state.m_cmd == IZNet::Mode::FirmwareUpdate ||
          m_state.m_cmd == IZNet::Mode::BackupCreation || m_state.m_cmd == IZNet::Mode::BackupRestoring ) &&
        ( m_state.m_cmdStatus == ModeStatus::Invoked || m_state.m_cmdStatus == ModeStatus::InProgress ||
          m_state.m_cmdStatus == ModeStatus::Started ) ) {
        return true;
    }

    if( except != IZNet::Mode::Idle && m_state.m_cmd != IZNet::Mode::Idle ) {
        return except == IZNet::Mode::Monitoring && m_state.m_cmd != IZNet::Mode::Monitoring;
    }
    return false;
}

void ZNetImpl::setState( IZNet::Mode cmd, ModeStatus cmdStatus, ErrorOpt err ) {
    m_state.m_cmd = cmd;
    m_state.m_cmdStatus = cmdStatus;

    if( err ) {
        ZNET_LOGE( "%s\n", utils::stringify( *err ).c_str() );
    }
    if( cmd == IZNet::Mode::Initializing && cmdStatus != ModeStatus::Finished ) {
        return;
    }
    getOwner().onUpdateState( cmd, cmdStatus, std::move( err ) );

    if( cmd == IZNet::Mode::BackupRestoring ) {
        return;
    }

    switch( cmdStatus ) {
    case znet::ModeStatus::Finished:
    case znet::ModeStatus::Failed:
    case znet::ModeStatus::Timeout:
    case znet::ModeStatus::Undefined:
        m_state.m_cmd = IZNet::Mode::Idle;
        m_state.m_cmdStatus = ModeStatus::Empty;
        break;

    default:
        return;
    }
}

bool ZNetImpl::checkState( IZNet::Mode cmd, ModeStatus cmdStatus ) {
    return m_state.m_cmd == cmd && ( cmdStatus == ModeStatus::Undefined || m_state.m_cmdStatus == cmdStatus );
}

void ZNetImpl::znetNodeCmdUsersNumberReport( znet_error_t err,
                                             znet_node_id_t node_id,
                                             znet_node_channel_id_t channel_id,
                                             znet_supported_users_t supported_users,
                                             void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ptr->getHandler( node_id ).onNodeCmdUsersNumberReport( node_id, channel_id, supported_users );
    }
}

void ZNetImpl::znetNodeCmdHail( znet_error_t err, znet_node_id_t node_id, void* arg ) {
    ZNetImpl* pZNetImpl = static_cast<ZNetImpl*>( arg );
    if( pZNetImpl != nullptr ) {
        ZNET_LOGI( "%s node: %hhu\n", __func__, node_id );
        pZNetImpl->getHandler( node_id ).onNodeCmdHail( node_id );
    }
}

void ZNetImpl::znetNodeTimeRequested( znet_node_id_t node_id,
                                      znet_node_channel_id_t channel_id,
                                      uint8_t encap,
                                      void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ZNET_LOGI( "%s node: %hhu\n", __func__, node_id );
        ptr->getHandler( node_id ).onTimeRequested( node_id, channel_id, encap );
    }
}

void ZNetImpl::znetNodeDateRequested( znet_node_id_t node_id,
                                      znet_node_channel_id_t channel_id,
                                      uint8_t encap,
                                      void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ZNET_LOGI( "%s node: %hhu\n", __func__, node_id );
        ptr->getHandler( node_id ).onDateRequested( node_id, channel_id, encap );
    }
}

void ZNetImpl::znetNodeTimeZoneRequested( znet_node_id_t node_id,
                                          znet_node_channel_id_t channel_id,
                                          uint8_t encap,
                                          void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ZNET_LOGI( "%s node: %hhu\n", __func__, node_id );
        ptr->getHandler( node_id ).onTimeZoneRequested( node_id, channel_id, encap );
    }
}

void ZNetImpl::znetNodeTimeZoneReport( znet_error_t err,
                                       znet_node_id_t nodeId,
                                       znet_node_channel_id_t channelId,
                                       const znet_time_zone_settings_t* timeZoneSettings,
                                       void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        assert( timeZoneSettings );
        ZNET_LOGI( "%s, node %d, tz %d:%d %d:%d:%d %d:%d:%d\n",
                   __func__,
                   nodeId,

                   timeZoneSettings->time_zone.hour_tzo * ( timeZoneSettings->time_zone.is_tzo_negative ? -1 : 1 ),
                   timeZoneSettings->time_zone.minute_tzo,

                   timeZoneSettings->start_dst.month,
                   timeZoneSettings->start_dst.hour,
                   timeZoneSettings->start_dst.day,

                   timeZoneSettings->end_dst.month,
                   timeZoneSettings->end_dst.day,
                   timeZoneSettings->end_dst.hour );

        ptr->getHandler( nodeId ).onNodeTimeZoneReport( nodeId, nodeId, *timeZoneSettings );
    }
}

void ZNetImpl::znetNodeTimeZoneSeted( znet_node_id_t nodeId,
                                      znet_node_channel_id_t channelId,
                                      const znet_time_zone_settings_t* timeZoneSettings,
                                      void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        ZNET_LOGI( "%s node: %hhu\n", __func__, nodeId );
        ptr->getHandler( nodeId ).onNodeTimeZoneSet( nodeId, channelId, *timeZoneSettings );
    }
}

void ZNetImpl::znetNodeLocalTimeReport( znet_error_t err,
                                        znet_node_id_t nodeId,
                                        znet_node_channel_id_t channelId,
                                        const znet_local_time_t* localTime,
                                        void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        assert( localTime );
        ZNET_LOGI( "%s. Node ID: %hhu, time %d:%d:%d \n",
                   __func__,
                   nodeId,
                   localTime->hour,
                   localTime->minute,
                   localTime->second );

        ptr->getHandler( nodeId ).onNodeLocalTimeReport( nodeId, channelId, *localTime );
    }
}

void ZNetImpl::znetNodeLocalDateReport( znet_error_t err,
                                        znet_node_id_t nodeId,
                                        znet_node_channel_id_t channelId,
                                        const znet_local_date_t* localDate,
                                        void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        assert( localDate );
        ZNET_LOGI( "%s. Node ID: %hhu, date %d:%d:%d \n",
                   __func__,
                   nodeId,
                   localDate->year,
                   localDate->month,
                   localDate->day );

        ptr->getHandler( nodeId ).onNodeLocalDateReport( nodeId, channelId, *localDate );
    }
}

void ZNetImpl::znetNodeScheduleLockEntryTimeOffsetReport( znet_error_t err,
                                                          znet_node_id_t node_id,
                                                          znet_node_channel_id_t channel_id,
                                                          const znet_cmd_schedule_entry_lock_time_offset_t* value,
                                                          void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s, node %d, tzo %d:%d, dst %dmin\n",
                   __func__,
                   node_id,
                   value->time_zone.hour_tzo * ( value->time_zone.is_tzo_negative ? -1 : 1 ),
                   value->time_zone.minute_tzo,
                   value->minute_dst_offset );

        ptr->getHandler( node_id ).onNodeScheduleLockEntryTimeOffsetReport( node_id, channel_id, *value );
    }
}

void ZNetImpl::znetNodeScheduleLockEntryWeekDayReport( znet_error_t err,
                                                       znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       uint8_t user_id,
                                                       uint8_t slot_id,
                                                       const znet_week_day_schedule_t* value,
                                                       void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s, node %d, user_id = %d, slot_id = %d, day_of_week = %d, "
                   "start_hour = %d, start_minute = %d, stop_hour = %d, stop_minute = %d \n",
                   __func__,
                   node_id,
                   value->schedule_id.user_id,
                   value->schedule_id.slot_id,
                   value->day_of_week,
                   value->start_hour,
                   value->start_min,
                   value->stop_hour,
                   value->stop_min );

        ptr->getHandler( node_id ).onNodeScheduleEntryLockWeekDayReport( node_id, channel_id, *value );
    }
}

void ZNetImpl::znetNodeScheduleLockYearDayReport( znet_error_t err,
                                                  znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id,
                                                  uint8_t user_id,
                                                  uint8_t slot_id,
                                                  const znet_year_day_schedule_t* value,
                                                  void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        auto node = ptr->onFindNode( node_id );
        if( !node ) {
            ZNET_LOGE( "%s fail node: %hhu\n", __func__, node_id );
            return;
        }

        const auto schedule = ScheduleHelper::adjustSchedule( *value, -( node->getTimeOffset() ) );

        ZNET_LOGI( "%s, node %d, user_id = %d, slot_id = %d, start_year = %d, "
                   "start_month = %d, start_day = %d, start_hour = %d, start_minute = %d, stop_year = %d, stop_month = "
                   "%d, stop_day = %d, stop_hour = %d, stop_minute = %d \n",
                   __func__,
                   node_id,
                   schedule.schedule_id.user_id,
                   schedule.schedule_id.slot_id,
                   schedule.start_year,
                   schedule.start_month,
                   schedule.start_day,
                   schedule.start_hour,
                   schedule.start_minute,
                   schedule.stop_year,
                   schedule.stop_month,
                   schedule.stop_day,
                   schedule.stop_hour,
                   schedule.stop_minute );

        ptr->getHandler( node_id ).onNodeScheduleEntryLockYearDayReport( node_id, channel_id, schedule );
    }
}

void ZNetImpl::znetNodeScheduleLockEntrySupportedReport( znet_error_t err,
                                                         znet_node_id_t node_id,
                                                         znet_node_channel_id_t channel_id,
                                                         const znet_cmd_schedule_entry_lock_supported_report_t* value,
                                                         void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s, node %d, week_day_slots = %d, year_day_slots = %d, daily_repeating_slots = %d\n",
                   __func__,
                   node_id,
                   value->week_day_slots,
                   value->year_day_slots,
                   value->daily_repeating_slots );

        ptr->getHandler( node_id ).onNodeScheduleLockEntrySupportedReport( node_id, channel_id, *value );
    }
}

void ZNetImpl::znetNodeScheduleLockEntryRepeatingReport( znet_error_t err,
                                                         znet_node_id_t node_id,
                                                         znet_node_channel_id_t channel_id,
                                                         uint8_t user_id,
                                                         uint8_t slot_id,
                                                         const znet_daily_repeating_schedule_t* value,
                                                         void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        auto node = ptr->onFindNode( node_id );
        if( !node ) {
            ZNET_LOGE( "%s fail to find a node with node_id: %d\n", __func__, node_id );
            return;
        }

        const auto schedule = ScheduleHelper::adjustSchedule( *value, -( node->getTimeOffset() ) );

        ZNET_LOGD( "%s Origin\n node %d, user_id = %d, slot_id = %d, week_day_bitmask "
                   "= %d, start_hour = %d, start_min = %d, duration_hour = %d, duration_min = %d\n",
                   __func__,
                   node_id,
                   value->schedule_id.user_id,
                   value->schedule_id.slot_id,
                   value->week_day_bitmask,
                   value->start_hour,
                   value->start_min,
                   value->duration_hour,
                   value->duration_min );

        ZNET_LOGI( "%s\n node %d, user_id = %d, slot_id = %d, week_day_bitmask "
                   "= %d, start_hour = %d, start_min = %d, duration_hour = %d, duration_min = %d\n",
                   __func__,
                   node_id,
                   schedule.schedule_id.user_id,
                   schedule.schedule_id.slot_id,
                   schedule.week_day_bitmask,
                   schedule.start_hour,
                   schedule.start_min,
                   schedule.duration_hour,
                   schedule.duration_min );

        ptr->getHandler( node_id ).onNodeScheduleEntryLockDailyRepeatingReport( node_id, channel_id, schedule );
    }
}

void ZNetImpl::znetNodeSoundSwitchTonesNumberReport( znet_error_t err,
                                                     znet_node_id_t node_id,
                                                     znet_node_channel_id_t channel_id,
                                                     znet_cmd_sound_switch_tones_number_t value,
                                                     void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s\n "
                   "  node id: %hhu\n "
                   "  channel_id: %hhu\n "
                   "  tones number: %hhu\n",
                   __func__,
                   node_id,
                   channel_id,
                   value );

        ptr->getHandler( node_id ).onNodeSoundSwitchTonesNumberReport( node_id, channel_id, value );
    }
}

void ZNetImpl::znetNodeSoundSwitchToneInfoReport( znet_error_t err,
                                                  znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id,
                                                  znet_cmd_sound_switch_tone_id_t toneId,
                                                  const znet_cmd_sound_switch_tone_info_t* value,
                                                  void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        std::string name( (const char*)value->name, value->name_length );
        ZNET_LOGI( "%s\n "
                   "  node id: %hhu\n "
                   "  channel_id: %hhu\n "
                   "  tone id: %hhu\n "
                   "  tone duration: %hhu\n "
                   "  tone name: %s\n",
                   __func__,
                   node_id,
                   channel_id,
                   value->tone_id,
                   value->tone_duration,
                   name.c_str() );

        ptr->getHandler( node_id ).onNodeSoundSwitchToneInfoReport( node_id, channel_id, *value );
    }
}

void ZNetImpl::znetNodeSoundSwitchConfigurationReport( znet_error_t err,
                                                       znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       const znet_cmd_sound_switch_configuration_t* value,
                                                       void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s\n"
                   "  node id: %hhu\n"
                   "  channel id: %hhu\n"
                   "  volume: %hhu\n"
                   "  default tone id: %hhu\n",
                   __func__,
                   node_id,
                   channel_id,
                   value->volume,
                   value->default_tone_id );

        ptr->getHandler( node_id ).onNodeSoundSwitchConfigurationReport( node_id, channel_id, *value );
    }
}

void ZNetImpl::znetNodeSoundSwitchTonePlayReport( znet_error_t err,
                                                  znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id,
                                                  const znet_cmd_sound_switch_tone_play_t* value,
                                                  void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s\n"
                   "  node id: %hhu\n"
                   "  channel id: %hhu\n"
                   "  tone id: %hhu\n"
                   "  volume: %hhu\n",
                   __func__,
                   node_id,
                   channel_id,
                   value->tone_id,
                   value->volume );

        ptr->getHandler( node_id ).onNodeSoundSwitchTonePlayReport( node_id, channel_id, *value );
    }
}

void ZNetImpl::znetNodeUserCodeCapabilitiesReport( znet_error_t err,
                                                   znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   const znet_user_code_capabilities_report_t* user_code_capabilities,
                                                   void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s\n"
                   "  node id: %hhu\n"
                   "  channel id: %hhu\n"
                   "  User status capabilities\n"
                   "    MC Support: %hhu\n"
                   "    MCD Support: %hhu\n"
                   "    Bitmask: %s\n"
                   "  Keypad modes capabilities:\n"
                   "    UCC Support: %hhu\n"
                   "    MUCR Support: %hhu\n"
                   "    MUCR Support: %hhu\n"
                   "    Bitmask: %s\n"
                   "  Key capabilities:\n"
                   "    Bitmask: %s\n",
                   __func__,
                   node_id,
                   channel_id,
                   user_code_capabilities->user_id_status_capability.MC_support,
                   user_code_capabilities->user_id_status_capability.MCD_support,
                   utils::bitmaskToString( user_code_capabilities->user_id_status_capability.bitmask,
                                           user_code_capabilities->user_id_status_capability.bitmask_length )
                       .c_str(),
                   user_code_capabilities->keypad_modes_capability.UCC_support,
                   user_code_capabilities->keypad_modes_capability.MUCR_support,
                   user_code_capabilities->keypad_modes_capability.MUCS_support,
                   utils::bitmaskToString( user_code_capabilities->keypad_modes_capability.bitmask,
                                           user_code_capabilities->keypad_modes_capability.bitmask_length )
                       .c_str(),
                   utils::bitmaskToString( user_code_capabilities->keys_capability.bitmask,
                                           user_code_capabilities->keys_capability.bitmask_length )
                       .c_str() );

        ptr->getHandler( node_id ).onNodeUserCodeCapabilitiesReport( node_id, channel_id, *user_code_capabilities );
    }
}

void ZNetImpl::znetNodeUserCodesKeypadModeReport( znet_error_t err,
                                                  znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id,
                                                  znet_user_code_keypad_mode_t keypad_mode,
                                                  void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s\n"
                   "  node id: %hhu\n"
                   "  channel id: %hhu\n"
                   "  keypad mode: %hhu\n",
                   __func__,
                   node_id,
                   channel_id,
                   keypad_mode );

        ptr->getHandler( node_id ).onNodeUserCodesKeypadModeReport( node_id, channel_id, keypad_mode );
    }
}

void ZNetImpl::znetNodeCmdUserCodeReport( znet_error_t err,
                                          znet_node_id_t nodeId,
                                          znet_node_channel_id_t channelId,
                                          uint8_t requestedUserId,
                                          const znet_user_code_report_t* value,
                                          void* arg ) {
    UserCode::UserPollStatus pollingStatus = UserCode::UserPollStatus::Idle;
    RequestsManagerCb( UserCodesRequest )( err, nodeId, channelId, requestedUserId, value, &pollingStatus, arg );

    if( err == ZNET_ERR_REQUEST_SENT_OK ) {
        return;
    }

    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s\n"
                   "  node id: %hhu\n"
                   "  channel id: %hhu\n"
                   "  requested user id: %u\n"
                   "%s",
                   __func__,
                   nodeId,
                   channelId,
                   requestedUserId,
                   userCodesToString( value, 1 ).c_str() );

        ptr->getHandler( nodeId ).onNodeCmdUserCodeReport( nodeId, channelId, value, 1, {}, pollingStatus );
    }
}

void ZNetImpl::znetNodeExtendedUserCodeReport( znet_error_t err,
                                               znet_node_id_t node_id,
                                               znet_node_channel_id_t channel_id,
                                               znet_supported_users_t requested_user_id,
                                               uint8_t user_codes_count,
                                               const znet_user_code_report_t* user_codes,
                                               znet_supported_users_t nextUserCode,
                                               void* arg ) {
    UserCode::UserPollStatus pollingStatus = UserCode::UserPollStatus::Idle;

    RequestsManagerCb( ExtendedCodesRequest )(
        err, node_id, channel_id, requested_user_id, user_codes_count, user_codes, nextUserCode, &pollingStatus, arg );

    if( err == ZNET_ERR_REQUEST_SENT_OK ) {
        return;
    }

    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s\n"
                   "  node id: %hhu\n"
                   "  channel id: %hhu\n"
                   "  requested user id: %u\n"
                   "%s",
                   __func__,
                   node_id,
                   channel_id,
                   requested_user_id,
                   userCodesToString( user_codes, user_codes_count ).c_str() );

        ptr->getHandler( node_id ).onNodeCmdUserCodeReport(
            node_id, channel_id, user_codes, user_codes_count, nextUserCode, pollingStatus );
    }
}

void ZNetImpl::znetNodeUserCodeChecksumReport( znet_error_t err,
                                               znet_node_id_t node_id,
                                               znet_node_channel_id_t channel_id,
                                               znet_user_code_checksum_t checksum,
                                               void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s\n"
                   "  node id: %hhu\n"
                   "  channel id: %hhu\n"
                   "  checksum: %u\n",
                   __func__,
                   node_id,
                   channel_id,
                   checksum );

        ptr->getHandler( node_id ).onNodeUserCodeChecksumReport( node_id, channel_id, checksum );
    }
}

void ZNetImpl::znetNodeMasterCodeReport( znet_error_t err,
                                         znet_node_id_t node_id,
                                         znet_node_channel_id_t channel_id,
                                         uint8_t code_length,
                                         const uint8_t* code,
                                         void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s\n"
                   "  node id: %hhu\n"
                   "  channel id: %hhu\n"
                   "  master code: %s\n",
                   __func__,
                   node_id,
                   channel_id,
                   std::string( reinterpret_cast<const char*>( code ), code_length ).c_str() );

        ptr->getHandler( node_id ).onNodeMasterCodeReport( node_id, channel_id, code_length, code );
    }
}

void ZNetImpl::znetNodeFirmwareMdReport( znet_error_t err,
                                         znet_node_id_t node_id,
                                         znet_node_channel_id_t channel_id,
                                         const znet_firmware_md_report_v1_t* report_v1,
                                         const znet_firmware_md_report_v3_t* report_v3_opt,
                                         const znet_firmware_md_report_v5_t* report_v5_opt,
                                         void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s\n"
                   "  node id: %hhu\n"
                   "  channel id: %hhu\n",
                   __func__,
                   node_id,
                   channel_id );

        ptr->getHandler( node_id ).onNodeFirmwareMdReport(
            node_id, channel_id, *report_v1, report_v3_opt, report_v5_opt );
    }
}

void ZNetImpl::znetNodeFirmwareUpdateMdRequestReport( znet_error_t err,
                                                      znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      uint8_t status,
                                                      void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ZNET_LOGE( "%s fail node: %hhu\n", __func__, node_id );
            ptr->getHandler( node_id ).onNodeFailed( node_id, err );
        }
        else {
            ZNET_LOGI( "%s\n"
                       "  node id: %hhu\n"
                       "  channel id: %hhu\n",
                       __func__,
                       node_id,
                       channel_id );

            ptr->getHandler( node_id ).onNodeCmdFirmwareUpdateMdRequestReport(
                node_id, channel_id, static_cast<FirmwareUpdate::UpdateMdRequestReportStatus>( status ) );
        }
    }
}

void ZNetImpl::znetNodeFirmwareUpdateMdRequested( znet_error_t err,
                                                  znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id,
                                                  uint8_t requested_reports_count,
                                                  uint16_t report_number,
                                                  void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s\n"
                   "  node id: %hhu\n"
                   "  channel id: %hhu\n"
                   "  requested_reports_count: %u\n"
                   "  report_number: %u\n",
                   __func__,
                   node_id,
                   channel_id,
                   requested_reports_count,
                   report_number );

        ptr->getHandler( node_id ).onNodeCmdFirmwareUpdateMdRequested(
            node_id, channel_id, requested_reports_count, report_number );
    }
}

void ZNetImpl::znetNodeFirmwareUpdateMdReport( znet_error_t err,
                                               znet_node_id_t node_id,
                                               znet_node_channel_id_t channel_id,
                                               uint16_t report_number,
                                               bool is_last_report,
                                               uint8_t data_size,
                                               const uint8_t* data,
                                               void* arg ) {
    auto ptr = static_cast<ZNetImpl*>( arg );
    if( ptr && err == ZNET_ERR_OK ) {
        ZNET_LOGI( "%s\n"
                   "  node id: %hhu\n"
                   "  channel id: %hhu\n",
                   __func__,
                   node_id,
                   channel_id );
    }
}

void ZNetImpl::znetNodeFirmwareUpdateMdStatusReport( znet_error_t err,
                                                     znet_node_id_t node_id,
                                                     znet_node_channel_id_t channel_id,
                                                     uint8_t status,
                                                     uint16_t waiting_time,
                                                     void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ZNET_LOGE( "%s fail node: %hhu\n", __func__, node_id );
            ptr->getHandler( node_id ).onNodeFailed( node_id, err );
        }
        else {
            ZNET_LOGI( "%s\n"
                       "  node id: %hhu\n"
                       "  channel id: %hhu\n"
                       "  status: %hhu\n"
                       "  waiting_time: %u\n",
                       __func__,
                       node_id,
                       channel_id,
                       status,
                       waiting_time );

            ptr->getHandler( node_id ).onNodeCmdFirmwareUpdateMdStatusReport(
                node_id, channel_id, static_cast<FirmwareUpdate::DataSendingStatus>( status ), waiting_time );
        }
    }
}

void ZNetImpl::znetNodeFirmwareUpdateMdActivationStatusReport(
    znet_error_t err,
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    uint8_t target,
    const znet_firmware_update_md_activation_status_report_v4_t* report_v4,
    const znet_firmware_update_md_activation_status_report_v5_t* report_v5_opt,
    void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        if( err ) {
            ZNET_LOGE( "%s fail node: %hhu\n", __func__, node_id );
            ptr->getHandler( node_id ).onNodeFailed( node_id, err );
        }
        else {
            ZNET_LOGI( "%s\n"
                       "  node id: %hhu\n"
                       "  channel id: %hhu\n"
                       "  status: %hhu\n",
                       __func__,
                       node_id,
                       channel_id,
                       report_v4->status );

            ptr->getHandler( node_id ).onNodeFirmwareUpdateMdActivationStatusReport(
                node_id,
                channel_id,
                target,
                report_v4->firmware,
                static_cast<FirmwareUpdate::ActivationStatus>( report_v4->status ),
                report_v5_opt ? utils::optional<uint8_t>( report_v5_opt->hardware_revision ) :
                                utils::optional<uint8_t>() );
        }
    }
}

void ZNetImpl::znetNodeFirmwareUpdateMdPreparationReport( znet_error_t err,
                                                          znet_node_id_t node_id,
                                                          znet_node_channel_id_t channel_id,
                                                          uint8_t status,
                                                          uint16_t firmware_checksum,
                                                          void* arg ) {}

void ZNetImpl::znetBackupSessionOpenReport( znet_error_t err, znet_backup_status_t status, uint16_t size, void* arg ) {
    if( err == ZNET_ERR_OK ) {
        ZNET_LOGI( "Backup session open result. status %u, size %u\n", status, size );
    }
}

void ZNetImpl::znetBackupSessionCloseReport( znet_error_t err, znet_backup_status_t status, void* arg ) {
    if( err == ZNET_ERR_OK ) {
        ZNET_LOGI( "Backup session close result. status %u\n", status );
    }
}

void ZNetImpl::znetBackupReadReport( znet_error_t err,
                                     uint16_t requestedOffset,
                                     znet_backup_status_t status,
                                     uint16_t reportedOffset,
                                     uint8_t dataLength,
                                     const uint8_t* data,
                                     void* arg ) {
    if( err == ZNET_ERR_OK ) {
        ZNET_LOGD(
            "Backup read data report. requested offset = %u, status = %u, reported offset = %u, data length = %u\n",
            requestedOffset,
            status,
            reportedOffset,
            dataLength );
    }
}

void ZNetImpl::znetBackupWriteReport( znet_error_t err,
                                      uint16_t requestedOffset,
                                      znet_backup_status_t status,
                                      uint16_t reportedOffset,
                                      uint8_t reportedLength,
                                      void* arg ) {
    if( err == ZNET_ERR_OK ) {
        ZNET_LOGD(
            "Backup write data report. requested offset = %u, status = %u, reported offset = %u, data length = %u\n",
            requestedOffset,
            status,
            reportedOffset,
            reportedLength );
    }
}

void ZNetImpl::znetAntiTheftReport( znet_error_t err,
                                    znet_node_id_t nodeId,
                                    znet_node_channel_id_t channelId,
                                    const znet_cmd_anti_theft_report_t* report,
                                    void* arg ) {
    if( auto ptr = static_cast<ZNetImpl*>( arg ) ) {
        if( err == ZNET_ERR_OK ) {
            assert( report );

            ZNET_LOGI( "%s. node_id=%u, channel_id=%u, is_locked=%s, is_restricted=%s, manufacturer_id=%u, "
                       "zwave_locking_id=%u\n",
                       __func__,
                       nodeId,
                       channelId,
                       utils::boolToYesNo( report->is_locked ),
                       utils::boolToYesNo( report->is_restricted ),
                       report->manufacturer_id,
                       report->zwave_locking_id );

            ptr->getHandler( nodeId ).onNodeCmdAntiTheftReport( nodeId, channelId, *report );
        }
    }
}

void ZNetImpl::znetAntiTheftUnlockStateReport( znet_error_t err,
                                               znet_node_id_t nodeId,
                                               znet_node_channel_id_t channelId,
                                               const znet_cmd_anti_theft_unlock_state_report_t* report,
                                               void* arg ) {
    if( auto ptr = static_cast<ZNetImpl*>( arg ) ) {
        if( err == ZNET_ERR_OK ) {
            assert( report );

            ZNET_LOGI( "%s. node_id=%u, channel_id=%u, is_locked=%s, is_restricted=%s, manufacturer_id=%u, "
                       "zwave_locking_id=%u\n",
                       __func__,
                       nodeId,
                       channelId,
                       utils::boolToYesNo( report->is_locked ),
                       utils::boolToYesNo( report->is_restricted ),
                       report->manufacturer_id,
                       report->zwave_locking_id );

            ptr->getHandler( nodeId ).onNodeCmdAntiTheftUnlockStateReport( nodeId, channelId, *report );
        }
    }
}

bool ZNetImpl::znetGetKnownS0Key( uint8_t* buf, void* arg ) {
    ZNetImpl* ptr = static_cast<ZNetImpl*>( arg );
    if( ptr != nullptr ) {
        return ptr->getOwner().getKnownS0Key( buf );
    }

    return false;
}

void ZNetImpl::setupDstTimer() {
    ZNET_LOGD( "setupDstTimer\n" );

    const auto utcDateTime = m_znetOwner.getUtcDateTime();
    const auto localDateTime = m_znetOwner.getLocalDateTime();
    const auto tzSettings = m_znetOwner.getTimeZoneSettings();

    auto dstTimePointToDateTime = [&localDateTime]( const znet_dst_time_point_t& dstTimePoint ) {
        return znet_date_time_t{ .date = znet_date_t{ .year = localDateTime.date.year,
                                                      .month = dstTimePoint.month,
                                                      .day = dstTimePoint.day },
                                 .time = znet_time_t{ .hour = dstTimePoint.hour, .minute = 0, .second = 0 } };
    };

    const auto startDst = dstTimePointToDateTime( tzSettings.start_dst );
    const auto endDst = dstTimePointToDateTime( tzSettings.end_dst );

    if( !is_date_valid( &startDst.date ) || !is_date_valid( &endDst.date ) ) {
        ZNET_LOGI( "Don't setup dst timer (no dst time)\n" );
        return;
    }

    if( !m_dstTimer ) {
        m_dstTimer = m_znetOwner.createTimer( [this]() {
            synchronizeAllNodesTime();
            setupDstTimer();
        } );
    }

    auto tryNextDstTimePoint = [this, &utcDateTime]( const znet_date_time_t& dateTime ) {
        const auto diff = utils::getTimeDifference( dateTime, utcDateTime );
        if( diff.count() < 0 )
            return false;

        m_dstTimer->runOnce( diff + std::chrono::seconds( 30 ) );

        return true;
    };

    auto getNextYearDstTime = [&utcDateTime]() {
        // TODO: Calculate next year dst time.
        const znet_cmd_time_parameters_year_t nextYear = utcDateTime.date.year + 1;
        return znet_date_time_t{ .date = znet_date_t{ .year = nextYear, .month = 1, .day = 1 },
                                 .time = znet_time_t{ .hour = 1, .minute = 0, .second = 0 } };
    };

    const auto tzOffset =
        ( tzSettings.time_zone.is_tzo_negative ? -1 : 1 ) * ( std::chrono::minutes( tzSettings.time_zone.minute_tzo ) +
                                                              std::chrono::hours( tzSettings.time_zone.hour_tzo ) );

    const auto isTimerRun = tryNextDstTimePoint( utils::addTimeDifference( startDst, -tzOffset ) ) ||
                            tryNextDstTimePoint( utils::addTimeDifference( endDst, -tzOffset ) ) ||
                            tryNextDstTimePoint( getNextYearDstTime() );

    assert( isTimerRun );
}

void ZNetImpl::initWakupNodesState() {
    ZNET_LOGD( "initWakupNodesState\n" );

    const auto nodeIds = getOwner().getNodeIds();

    utils::forEachBit( nodeIds.mask, nodeIds.Size, [this]( znet_node_id_t nodeId ) {
        auto node = onFindNode( nodeId );
        if( node && node->isWakeup() && node->isReachable() == ReachableState::Unknown ) {
            node->setIsWokenUp( false );
            setReachableState( nodeId, true );
        }
    } );
}

void ZNetImpl::synchronizeAllNodesTime() {
    m_timeManager.addAllNodesToProcess();
}

void ZNetImpl::synchronizeNodeTime( uint8_t nodeId ) {
    ZNET_LOGI( "%s node: %hhu\n", __func__, nodeId );
    m_timeManager.addNodeToProcess( nodeId );
}

void ZNetImpl::updateReachableCheckTimeoutSettings( bool useFixedWakeUpInterval,
                                                    uint32_t wakeUpInterval,
                                                    uint32_t alwaysOnDevicesInetrval,
                                                    uint8_t attemptsCount ) {
    ZNET_LOGI( "%s, "
               "use_fixed_wake_up_interval: %s, wake_up_interval: %d, "
               "always_on_devices_inetrval: %d, attempts_count: %d \n",
               __func__,
               useFixedWakeUpInterval ? "true" : "false",
               wakeUpInterval,
               alwaysOnDevicesInetrval,
               attemptsCount );
    m_reachableTimeoutSettings.update( useFixedWakeUpInterval, wakeUpInterval, alwaysOnDevicesInetrval, attemptsCount );
}

void ZNetImpl::checkReachableStatus( const uint8_t nodeId ) {
    ZNET_LOGI( "%s node: %hhu\n", __func__, nodeId );
    m_nodeCheckHandler.addNodeToProcess( nodeId );
}

void ZNetImpl::checkNodeValues( const uint8_t nodeId,
                                std::chrono::milliseconds requestDelay,
                                std::vector<std::pair<uint8_t, uint8_t>> dataToCheck ) {
    ZNET_LOGI( "%s node: %hhu\n", __func__, nodeId );

    ZNode* node = onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }
    node->setRequestDelay( requestDelay );
    m_nodeCheckHandler.addDataToCheck( nodeId, dataToCheck );
    m_nodeCheckHandler.addNodeToProcess( nodeId );
}

bool ZNetImpl::createChipBackup( IOWrapper::DestinationUPtr destination ) {
    assert( destination );

    return m_backupCreationHandler.setDestination( std::move( destination ) );
}

bool ZNetImpl::restoreChipBackup( IOWrapper::SourceUPtr source ) {
    assert( source );

    return m_backupRestoringHandler.setSource( std::move( source ) );
}

bool ZNetImpl::addNodeToGroup( znet_node_id_t nodeId,
                               znet_node_channel_id_t channelId,
                               znet_grouping_id_t groupId,
                               znet_node_id_t targetNodeId,
                               znet_node_channel_id_t targetChannelId,
                               bool force ) {
    auto node = onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s fail node: %hhu\n", __func__, nodeId );
        return false;
    }

    auto ccMulti = node->getCommandClass<znet::ZCc0x8eMultiChannelAssociation>( channelId );
    if( !ccMulti ) {
        ccMulti = node->getCommandClass<znet::ZCc0x8eMultiChannelAssociation>( ZNET_CHANNEL_ID_ROOT );
    }

    if( ccMulti ) {
        auto result = ccMulti->addNodeToGroup( groupId, targetNodeId, targetChannelId, force );
        if( result != ZCc0x8eMultiChannelAssociation::NodeAddResult::Failed )
            return true;
    }

    auto cc = node->getCommandClass<znet::ZCc0x85Association>( channelId );
    if( cc ) {
        return cc->addNodeToGroup( groupId, targetNodeId );
    }

    ZNET_LOGE( "%s fail node: %hhu, channel: %hhu)\n", __func__, nodeId, channelId );
    return false;
}

bool ZNetImpl::removeNodeFromGroup( znet_node_id_t nodeId,
                                    znet_node_channel_id_t channelId,
                                    znet_grouping_id_t groupId,
                                    znet_node_id_t targetNodeId,
                                    znet_node_channel_id_t targetChannelId ) {
    auto node = onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s fail node: %hhu\n", __func__, nodeId );
        return false;
    }

    auto ccMulti = node->getCommandClass<znet::ZCc0x8eMultiChannelAssociation>( channelId );
    if( ccMulti ) {
        return ccMulti->removeNodeFromGroup( groupId, targetNodeId, targetChannelId );
    }

    auto cc = node->getCommandClass<znet::ZCc0x85Association>( channelId );
    if( cc ) {
        return cc->removeNodeFromGroup( groupId, targetNodeId );
    }

    ZNET_LOGE( "%s fail node: %hhu, channel: %hhu)\n", __func__, nodeId, channelId );
    return false;
}

bool ZNetImpl::setupCommandClassAssociation( znet_node_id_t nodeId, znet_command_class_t cmdClass ) {
    auto node = onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s fail node: %hhu\n", __func__, nodeId );
        return false;
    }

    auto ccGroup = node->getCommandClass<znet::ZCc0x59AssociationGrpInfo>( ZNET_CHANNEL_ID_ROOT );

    if( !ccGroup ) {
        ZNET_LOGE( "%s fail to find association group info cc for node: %d\n", __func__, nodeId );
        return false;
    }

    const auto group = ccGroup->findAssociationGroup( cmdClass );

    if( !group.second ) {
        ZNET_LOGE( "%s fail to find group for cc %d in node %d\n", __func__, cmdClass, nodeId );
        return false;
    }

    return group.first == 1 ||
           addNodeToGroup( nodeId, ZNET_CHANNEL_ID_ROOT, group.first, znet_get_self_node_id(), ZNET_CHANNEL_ID_ROOT );
}

std::unique_ptr<CommonInfo> ZNetImpl::getCommonInfo() {
    std::unique_ptr<CommonInfo> info;
    info.reset( new CommonInfo );

    info->m_type = m_libraryType;
    info->m_module = "";  // TODO from library type
    info->m_module_firmware_version = m_libraryVersion;
    getOwner().fillFirmwareLocale( info->m_locale );

    char buffer[9] = { 0 };
    sprintf( buffer, "%X", znet_get_home_id() );
    info->m_home_id = std::string{ buffer };

    info->m_node_id = znet_get_self_node_id();

    info->m_pole.m_powered = std::chrono::seconds( EachNodeReachableCheckTimeout ).count();  // from hours to seconds
    info->m_pole.m_battery = ZCc0x84WakeUp::DefaultWakeUpInterval;                           // seconds

    znet_node_id_t SIS = znet_get_sis_node_id();

    info->m_roles.m_sis = znet_get_self_node_id() == SIS;
    info->m_roles.m_primary = isPrimaryController();
    info->m_roles.m_secondary = !isPrimaryController() && SIS == ZNET_NODE_ID_ANY;
    info->m_roles.m_static = false;
    info->m_roles.m_inclusion = !isPrimaryController() && SIS != ZNET_NODE_ID_ANY;

    return info;
}

void ZNetImpl::updateNodeTargetFirmware( znet_node_id_t nodeId, uint8_t target, FirmwareUpdate::SourceUPtr source ) {
    m_firmwareUpdateHandler.addTarget( nodeId, target, std::move( source ) );
}

const znet_callbacks_t& ZNetImpl::getCallbacks() const {
    return m_znet_cb;
}

}  // namespace znet
