#pragma once
#include <znet/znet.h>
#include <znet/znet_error_message.hpp>
#include <znet/znet_node.hpp>
#include <znet/znet_request_status.hpp>
#include <znet/enums/user_code.hpp>
#include <znet/enums/door_lock.hpp>
#include <znet/enums/meter_type.hpp>
#include <znet/enums/firmware_update.hpp>
#include <znet/enums/thermostat_setback.hpp>
#include <znet/enums/color_component.hpp>
#include <znet/znet_provisioning_types.hpp>
#include <znet/optional.hpp>
#include <chrono>
#include <ctime>

namespace znet {

class ZNetImpl;

typedef std::chrono::time_point<std::chrono::system_clock> timeout_t;
using Seconds = std::chrono::seconds;
using SecondsOpt = utils::optional<Seconds>;
using Uint8Opt = utils::optional<uint8_t>;
using UserCodeNumberOpt = utils::optional<znet_supported_users_t>;

class ZNetHandler {
    ZNetHandler() = delete;

public:
    ZNetHandler( ZNetImpl& parent );
    virtual ~ZNetHandler() = default;
    virtual void initialize();
    virtual void finalize( ErrorOpt );

    // Incorrect:use finalize instead
    virtual void onDeactivated();

    virtual bool onLoop();

    virtual void onNodeAdd( const znet_nodeinfo_t* node_info );
    virtual void onNodeAddSecurityFailed();
    virtual void onNodeAddSecurityKeysRequested( znet_S2_keys_t );
    virtual void onNodeAddSecurityKeysGranted( znet_S2_keys_t );
    virtual void onNodeAddSecurityPubKeyRequested( const znet_S2_pub_key_t* );
    virtual void onNodeAddSecurityPubKeyGranted( const znet_S2_pub_key_t* );
    virtual void onNodeAddSecurityDone( SecurityMode::Class );
    virtual void onNodeLearn( const znet_node_id_t destNodeId );
    virtual void onNodeLearnBreak(::znet_error_t, ::znet_node_id_t );
    virtual void onNodeList( const uint8_t* nodesList );
    virtual void onNodeRemInvoked();
    virtual void onNodeRem( znet_error_t, const znet_nodeinfo_t* );
    virtual void onNodeRemBreak( znet_error_t, const znet_nodeinfo_t* );
    virtual void onNodeNoOperation( const znet_node_id_t nodeId, znet_error_t error );
    virtual void onForcedRequestResult();
    virtual void onNodeCmdSecuritySupported( const uint8_t nodeId, const znet_security_supported_report_t* value );

    virtual void onNodeCmdManufacturerSpecific( const uint8_t nodeId,
                                                const uint16_t manufacturerId,
                                                const uint16_t productType,
                                                const uint16_t productId );

    virtual void onNodeCmdCommandVersion( const uint8_t nodeId, const uint8_t classId, const uint8_t version );

    virtual void onNodeCmdVersion( znet_node_id_t nodeId, const znet_version_report_t* versionReport );

    virtual void onNodeCmdVersionCapabilities( znet_node_id_t nodeId, const znet_version_capabilities_report_t* value );

    virtual void onNodeCmdSoftwareVersion( znet_node_id_t nodeId, const znet_software_version_report_t* value );

    virtual void onNodeCmdZWavePlusInfo( const uint8_t nodeId, const znet_zwaveplus_info_report_t* value );

    virtual void onNodeCmdAlarmSensor( const uint8_t nodeId,
                                       const uint8_t channelId,
                                       const uint8_t sensorType,
                                       const uint8_t value,
                                       const uint16_t seconds );

    virtual void onNodeCmdAlarmSensorSupported( const uint8_t nodeId,
                                                const uint8_t channelId,
                                                const uint8_t* data,
                                                const uint8_t len );

    virtual void onNodeCmdBinarySensor( const uint8_t nodeId,
                                        const uint8_t channelId,
                                        const uint8_t value,
                                        const uint8_t sensorType );

    virtual void onNodeCmdBinarySensorSupported( const uint8_t nodeId,
                                                 const uint8_t channelId,
                                                 const uint8_t* data,
                                                 const uint8_t len );

    virtual void onNodeCmdSensorMultilevelSupportedSensor( const uint8_t nodeId,
                                                           const uint8_t channelId,
                                                           const uint8_t* data,
                                                           uint8_t len );

    virtual void onNodeCmdSensorMultilevelSupportedScale( znet_node_id_t nodeId,
                                                          znet_node_channel_id_t channelId,
                                                          uint8_t sensor_type,
                                                          uint8_t scale_mask );

    virtual void onNodeCmdSensorMultilevel( znet_node_id_t nodeId,
                                            znet_node_channel_id_t channelId,
                                            const znet_multilevel_sensor_report_t* value );

    virtual void onNodeCmdMeterSupported( const znet_node_id_t nodeId,
                                          const znet_node_channel_id_t channelId,
                                          const bool resetSupported,
                                          const meter::MeterType type,
                                          const uint8_t scalesBitmaskSize,
                                          const uint8_t* scalesBitmask );

    virtual void onNodeCmdBasic( const uint8_t nodeId,
                                 const uint8_t channelId,
                                 const uint8_t currentValue,
                                 uint8_t targetValue );

    virtual void onNodeCmdBasicSet( const uint8_t nodeId,
                                    const uint8_t channelId,
                                    const uint8_t currentValue,
                                    uint8_t targetValue );

    virtual void onNodeCmdBatteryLevel( const uint8_t nodeId, const uint8_t channelId, const uint8_t value );

    virtual void onNodeCmdBinarySwitch( const uint8_t nodeId,
                                        const uint8_t channelId,
                                        const uint8_t value,
                                        const Uint8Opt targetValue,
                                        const SecondsOpt duration );

    virtual void onNodeCmdClock( const uint8_t nodeId, const uint8_t channelId, const znet_clock_value_t* value );

    virtual void onNodeCmdTimeParameters( const uint8_t nodeId,
                                          const uint8_t channelId,
                                          const znet_cmd_time_parameters_report_t* value );

    virtual void onNodeCmdColorSwitch( const uint8_t nodeId,
                                       const uint8_t channelId,
                                       const ColorComponentID componentId,
                                       const uint8_t value,
                                       const utils::optional<uint8_t> targetValue,
                                       const SecondsOpt targetDuration );

    virtual void onNodeCmdMultilevelSwitchSupported( znet_node_id_t nodeId,
                                                     znet_node_channel_id_t channelId,
                                                     uint8_t primary_type,
                                                     uint8_t secondary_type );

    virtual void onNodeCmdColorSwitchSupported( const uint8_t nodeId,
                                                const uint8_t channelId,
                                                const uint8_t masksCount,
                                                const uint8_t* mask );

    virtual void onNodeCmdDoorLockCapabilities( const uint8_t nodeId,
                                                const uint8_t channelId,
                                                const znet_door_lock_capabilities_t& capabilities );

    virtual void onNodeCmdDoorLockOperation( const uint8_t nodeId,
                                             const uint8_t channelId,
                                             const DoorLock::Mode mode,
                                             const uint8_t insideHandleModes,
                                             const uint8_t outsideHandleModes,
                                             const uint8_t condition,
                                             SecondsOpt lockTimeout,
                                             const DoorLock::ModeOpt tagretMode,
                                             const SecondsOpt tagretModeTimeOut );

    virtual void onNodeCmdDoorLockConfiguration( const uint8_t nodeId,
                                                 const uint8_t channelId,
                                                 const znet_door_lock_configuration_t& configuration,
                                                 const znet_door_lock_extended_configuration_t* extendedConfiguration );

    virtual void onNodeCmdNotification( const uint8_t node_id,
                                        const uint8_t channel_id,
                                        const znet_notification_report_t& value,
                                        bool isNotificationRequested );

    virtual void onNodeCmdNotificationSupported( const uint8_t node_id,
                                                 const uint8_t channel_id,
                                                 const znet_notification_supported_report_t* value );

    virtual void onNodeCmdNotificationEventSupported( const uint8_t node_id,
                                                      const uint8_t channel_id,
                                                      const uint8_t type,
                                                      const uint8_t* bitmask,
                                                      uint8_t bitmaskSize );

    virtual void onNodeCmdMultilevelSwitch( const uint8_t nodeId,
                                            const uint8_t channelId,
                                            const uint8_t currentValue,
                                            const utils::optional<uint8_t> targetValue,
                                            SecondsOpt targetDuration );

    virtual void onNodeCmdMultilevelSwitchSetDone( const uint8_t nodeId, const uint8_t channelId );

    virtual void onNodeCmdBarrierOperator( const uint8_t nodeId, const uint8_t channelId, const uint8_t value );

    virtual void onNodeCmdBarrierOperatorSignal( const uint8_t nodeId,
                                                 const uint8_t channelId,
                                                 const uint8_t signalId,
                                                 const uint8_t state );

    virtual void onNodeCmdBarrierOperatorSupportedSignals( const uint8_t nodeId,
                                                           const uint8_t channelId,
                                                           const uint8_t* data,
                                                           uint8_t len );

    virtual void onNodeCmdBarrierOperatorSetDone( const uint8_t nodeId, const uint8_t channelId );

    virtual void onNodeCmdMeter( const znet_node_id_t nodeId,
                                 const znet_node_channel_id_t channelId,
                                 const meter::MeterType type,
                                 const meter::Scale scale,
                                 const int32_t value,
                                 const uint8_t precision );

    virtual void onNodeCmdMeterSetDone( const uint8_t nodeId, const uint8_t channelId );

    virtual void onNodeCmdMultichannelEndpoint( const uint8_t nodeId,
                                                const uint8_t agregated_channel_ids_count,
                                                const uint8_t channel_ids_count,
                                                const uint8_t identical,
                                                const uint8_t dynamic );

    virtual void onNodeCmdMultichannelCapability( const uint8_t nodeId,
                                                  const uint8_t channel_id,
                                                  const uint8_t dynamic,
                                                  const uint8_t generic,
                                                  const uint8_t specific,
                                                  const uint8_t commands_count,
                                                  const znet_command_class_t* commands );

    virtual void onApplicationNodeUpdate( const znet_nodeinfo_t* nodeInfo );

    virtual void onNodeCmdSceneActuatorConfiguration( const uint8_t nodeId,
                                                      const uint8_t channelId,
                                                      const uint8_t sceneId,
                                                      const uint8_t level,
                                                      const uint8_t dimmingDuration );

    virtual void onNodeCmdSceneControllerConfiguration( const uint8_t nodeId,
                                                        const uint8_t channelId,
                                                        const uint8_t groupId,
                                                        const uint8_t sceneId,
                                                        const uint8_t dimmingDuration );

    virtual void onCentralSceneConfiguration( const uint8_t nodeId, const uint8_t channelId, const uint8_t sr_value );

    virtual void onNodeCmdCentralSceneSupported( const uint8_t nodeId,
                                                 const uint8_t channelId,
                                                 const uint8_t scenes_num,
                                                 const uint8_t data_size,
                                                 const uint8_t identical,
                                                 const uint8_t slow_refresh,
                                                 const uint8_t* data );

    virtual void onCentralSceneNotification( const uint8_t nodeId,
                                             const uint8_t sequence_number,
                                             const uint8_t slow_refresh,
                                             const uint8_t key_attribute,
                                             const uint8_t scene_number );

    virtual void onSceneActivationSetResult( const uint8_t nodeId, const uint8_t scene_number );

    virtual void onWakeupNotification( const uint8_t nodeId );

    virtual void onNodeFailed( const uint8_t nodeId, const int error );

    virtual void onAssociationSupportedGroupingResult( const uint8_t nodeId,
                                                       const uint8_t channelId,
                                                       const uint8_t count );

    virtual void onAssociationResult( const uint8_t nodeId,
                                      const uint8_t channelId,
                                      const znet_association_report_t* value );

    virtual void onAssociationSetResult( const uint8_t nodeId, const uint8_t channelId );

    virtual void onAssociationRemoveResult( const uint8_t nodeId, const uint8_t channelId );

    virtual void onAssociationGroupNameResult( const uint8_t nodeId,
                                               const uint8_t channelId,
                                               const znet_association_group_name_report_t* value );

    virtual void onAssociationGroupInfoResult( const uint8_t nodeId,
                                               const uint8_t channelId,
                                               const znet_association_group_info_report_t* value );

    virtual void onAssociationGroupCommandListResult( const uint8_t nodeId,
                                                      const uint8_t channelId,
                                                      const znet_association_group_command_list_report_t* value );

    virtual void onMultichannelAssociationSupportedGroupingResult( const uint8_t nodeId,
                                                                   const uint8_t channelId,
                                                                   const uint8_t count );

    virtual void onMultichannelAssociationResult( const uint8_t nodeId,
                                                  const uint8_t channelId,
                                                  const znet_multichannel_association_report_t* value );

    virtual void onMultichannelAssociationSetResult( const uint8_t nodeId, const uint8_t channelId );

    virtual void onMultichannelAssociationRemoveResult( const uint8_t nodeId, const uint8_t channelId );

    virtual void onSimpleAvControlSetResult( const uint8_t nodeId,
                                             const uint8_t channelId,
                                             const znet_simple_av_set_report_t* value );

    virtual void onConfigurationReport( const uint8_t nodeId,
                                        const uint8_t channelId,
                                        const uint16_t configParamNum,
                                        const uint8_t configParamSize,
                                        const uint32_t configParamValue );

    virtual void onConfigurationRequestStatus( const uint8_t nodeId,
                                               const uint8_t channelId,
                                               const uint8_t configParamNum,
                                               RequestStatus::Enum status );

    virtual void onThermostatModeResult( uint8_t nodeId,
                                         uint8_t channelId,
                                         const znet_thermostat_mode_report_t& value );

    virtual void onThermostatModeSupportedResult( const uint8_t nodeId,
                                                  const uint8_t channelId,
                                                  const uint8_t mcount,
                                                  const uint8_t* bit_mask );

    virtual void onThermostatFanModeResult( const uint8_t nodeId, const uint8_t value );

    virtual void onThermostatFanModeSupportedResult( const uint8_t nodeId,
                                                     const uint8_t channelId,
                                                     const uint8_t fmcount,
                                                     const uint8_t* bit_mask );

    virtual void onThermostatFanStateResult( const uint8_t nodeId, const uint8_t value );

    virtual void onThermostatOperatingStateResult( const uint8_t nodeId, const uint8_t value );

    virtual void onThermostatOperatingStateLoggingSupportedResult( const uint8_t nodeId,
                                                                   const uint8_t channelId,
                                                                   const uint8_t opscount,
                                                                   const uint8_t* bit_mask );

    virtual void onThermostatOperatingStateLoggingResult( const uint8_t nodeId,
                                                          const uint8_t rep_to_follows,
                                                          const uint16_t data_count,
                                                          const uint8_t* data );

    virtual void onThermostatSetbackResult( const znet_node_id_t nodeId,
                                            const uint8_t channelId,
                                            const thermostat_setback::OverrideType type,
                                            const thermostat_setback::Mode mode,
                                            const thermostat_setback::SetbackValueOpt valueOpt );

    virtual void onThermostatSetpointResult( const uint8_t nodeId,
                                             const uint8_t channelId,
                                             const uint8_t type,
                                             const uint8_t scale,
                                             const uint8_t size,
                                             const uint8_t precision,
                                             const int32_t sp_value,
                                             const bool isSetpointRequested );

    virtual void onThermostatSetpointSupportedResult( const uint8_t nodeId,
                                                      const uint8_t channelId,
                                                      const uint8_t stcount,
                                                      const uint8_t* bit_mask );

    virtual void onThermostatSetpointCapabilitiesResult( const uint8_t nodeId,
                                                         const uint8_t channelId,
                                                         const uint8_t type,
                                                         const uint8_t minParams,
                                                         const int32_t minValue,
                                                         const uint8_t maxParams,
                                                         const int32_t maxValue );

    virtual void onWakeUpResult( const znet_node_id_t nodeId,
                                 const uint32_t interval,
                                 const znet_node_id_t targetNodeId );

    virtual void onWakeupCapabilitiesResult( const uint8_t nodeId,
                                             const uint32_t min_wakeup_interval,
                                             const uint32_t max_wakeup_interval,
                                             const uint32_t default_wakeup_interval,
                                             const uint32_t step_wakeup_interval );

    virtual void onNodeCmdUserCodeReport( const znet_node_id_t nodeId,
                                          const znet_node_channel_id_t channelId,
                                          const znet_user_code_report_t* userCodeReport,
                                          uint8_t count,
                                          UserCodeNumberOpt nextUserCode,
                                          UserCode::UserPollStatus pollStatus );

    virtual void onNodeCmdUsersNumberReport( const znet_node_id_t nodeId,
                                             const znet_node_channel_id_t channelId,
                                             const znet_supported_users_t supportedUsers );

    virtual void onNodeCmdHail( const znet_node_id_t nodeId );

    virtual void startInclude( const DskDerivedHomeId& dskDerivedHomeId );
    virtual void stopInclude();
    virtual void stopLearn();
    virtual void startExclude();
    virtual void startExclude( const ::znet::ZNode& );
    virtual void stopExclude();

    virtual void onZwavePlusInfoGet();

    virtual void onIndicatorIdentify();

    virtual void onTimeRequested( const uint8_t nodeId, const uint8_t channelId, const uint8_t encap );

    virtual void onDateRequested( const uint8_t nodeId, const uint8_t channelId, const uint8_t encap );

    virtual void onTimeZoneRequested( const uint8_t nodeId, const uint8_t channelId, const uint8_t encap );

    virtual void onNodeTimeZoneSet( znet_node_id_t nodeId,
                                    znet_node_channel_id_t channelId,
                                    const znet_time_zone_settings_t& timeZoneSettings );

    virtual void onNodeTimeZoneReport( znet_node_id_t nodeId,
                                       znet_node_channel_id_t channelId,
                                       const znet_time_zone_settings_t& timeZoneSettings );

    virtual void onNodeLocalTimeReport( znet_node_id_t nodeId,
                                        znet_node_channel_id_t channelId,
                                        const znet_local_time_t& localTime );

    virtual void onNodeLocalDateReport( znet_node_id_t nodeId,
                                        znet_node_channel_id_t channelId,
                                        const znet_local_date_t& localDate );

    virtual void onNodeScheduleLockEntryTimeOffsetReport( znet_node_id_t node_id,
                                                          znet_node_channel_id_t channel_id,
                                                          const znet_cmd_schedule_entry_lock_time_offset_t& value );

    virtual void onNodeScheduleEntryLockWeekDayReport( znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       const znet_week_day_schedule_t& value );

    virtual void onNodeScheduleEntryLockYearDayReport( znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       const znet_year_day_schedule_t& value );

    virtual void onNodeScheduleLockEntrySupportedReport( znet_node_id_t node_id,
                                                         znet_node_channel_id_t channel_id,
                                                         const znet_cmd_schedule_entry_lock_supported_report_t& value );

    virtual void onNodeScheduleEntryLockDailyRepeatingReport( znet_node_id_t node_id,
                                                              znet_node_channel_id_t channel_id,
                                                              const znet_daily_repeating_schedule_t& value );

    virtual void onNodeSoundSwitchTonesNumberReport( znet_node_id_t nodeId,
                                                     znet_node_channel_id_t channelId,
                                                     znet_cmd_sound_switch_tones_number_t value );

    virtual void onNodeSoundSwitchToneInfoReport( znet_node_id_t nodeId,
                                                  znet_node_channel_id_t channelId,
                                                  const znet_cmd_sound_switch_tone_info_t& value );

    virtual void onNodeSoundSwitchConfigurationReport( znet_node_id_t node_id,
                                                       znet_node_channel_id_t channelId,
                                                       const znet_cmd_sound_switch_configuration_t& value );

    virtual void onNodeSoundSwitchTonePlayReport( znet_node_id_t nodeId,
                                                  znet_node_channel_id_t channelId,
                                                  const znet_cmd_sound_switch_tone_play_t& value );

    virtual void onNodeUserCodeCapabilitiesReport( znet_node_id_t nodeId,
                                                   znet_node_channel_id_t channelId,
                                                   const znet_user_code_capabilities_report_t& userCodeCapabilities );

    virtual void onNodeUserCodesKeypadModeReport( znet_node_id_t nodeId,
                                                  znet_node_channel_id_t channelId,
                                                  znet_user_code_keypad_mode_t keypadMode );

    virtual void onNodeExtendedUserCodeReport( znet_node_id_t nodeId,
                                               znet_node_channel_id_t channelId,
                                               uint8_t user_codes_count,
                                               const znet_user_code_report_t* userCodes );

    virtual void onNodeUserCodeChecksumReport( znet_node_id_t nodeId,
                                               znet_node_channel_id_t channelId,
                                               znet_user_code_checksum_t checksum );

    virtual void onNodeMasterCodeReport( znet_node_id_t nodeId,
                                         znet_node_channel_id_t channelId,
                                         uint8_t codeLength,
                                         const uint8_t* masterCode );

    virtual void onNodeInclusionControllerInitiate( znet_node_id_t nodeId,
                                                    const znet_node_id_t childNodeId,
                                                    const znet_inclution_controller_step_id_t stepId );

    virtual void onNodeInclusionControllerComplete( znet_node_id_t nodeId,
                                                    const znet_inclution_controller_step_id_t stepId,
                                                    const znet_inclution_controller_status_t status );

    virtual void onNodeFirmwareMdReport( znet_node_id_t nodeId,
                                         znet_node_channel_id_t channelId,
                                         const znet_firmware_md_report_v1_t& reportV1,
                                         const znet_firmware_md_report_v3_t* reportV3Opt,
                                         const znet_firmware_md_report_v5_t* reportV5Opt );

    virtual void onNodeCmdFirmwareUpdateMdRequestReport( znet_node_id_t nodeId,
                                                         znet_node_channel_id_t channelId,
                                                         FirmwareUpdate::UpdateMdRequestReportStatus status );

    virtual void onNodeCmdFirmwareUpdateMdRequested( znet_node_id_t nodeId,
                                                     znet_node_channel_id_t channelId,
                                                     uint8_t requestedReportsCount,
                                                     uint16_t reportNumber );

    virtual void onNodeCmdFirmwareUpdateMdStatusReport( znet_node_id_t nodeId,
                                                        znet_node_channel_id_t channelId,
                                                        FirmwareUpdate::DataSendingStatus status,
                                                        uint16_t waitingTime );

    virtual void onNodeFirmwareUpdateMdActivationStatusReport( znet_node_id_t nodeId,
                                                               znet_node_channel_id_t channelId,
                                                               uint8_t target,
                                                               const znet_firmware_info_t& firmware,
                                                               FirmwareUpdate::ActivationStatus status,
                                                               Uint8Opt hardwareId );

    virtual void onNodeCmdAntiTheftReport( znet_node_id_t nodeId,
                                           znet_node_channel_id_t channelId,
                                           const znet_cmd_anti_theft_report_t& report );

    virtual void onNodeCmdAntiTheftUnlockStateReport( znet_node_id_t nodeId,
                                                      znet_node_channel_id_t channelId,
                                                      const znet_cmd_anti_theft_unlock_state_report_t& report );

    virtual void onNodeProvisioningSet( znet_node_id_t nodeId,
                                        znet_node_channel_id_t channelId,
                                        const znet_node_provisioning_set_t* value );

    virtual void onNodeProvisioningDelete( znet_node_id_t nodeId,
                                           znet_node_channel_id_t channelId,
                                           const znet_node_provisioning_delete_t* value );

    virtual void onNodeProvisioningGet( znet_node_id_t nodeId,
                                        znet_node_channel_id_t channelId,
                                        uint8_t sequenceNumber,
                                        const uint8_t dskKey[16] );

    virtual void onNodeProvisioningListIterationGet( znet_node_id_t nodeId,
                                                     znet_node_channel_id_t channelId,
                                                     uint8_t sequenceNumber,
                                                     uint8_t remainingCounter );

    virtual bool canHandleNode( znet_node_id_t nodeId ) const;

    SecurityMode::Class onSecurityGet( znet_node_id_t nodeId );

    void setTimeout( const uint64_t timeout_ms );

protected:
    virtual void updateReachableState( ZNode& node, bool forceNotify = false );

    ZNetImpl& m_parentMesh;

    timeout_t m_timeOut;
};

}  // namespace znet
