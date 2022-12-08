#pragma once

#include <znet_handler.hpp>

namespace znet {

class MonitoringBaseHandler : public ZNetHandler {
public:
    using ZNetHandler::ZNetHandler;

    bool onLoop() override;

    void onForcedRequestResult() override;

    void onNodeRem(::znet_error_t, const ::znet_nodeinfo_t* ) override;

    void onNodeCmdBasic( const uint8_t nodeId,
                         const uint8_t channelId,
                         const uint8_t currentValue,
                         uint8_t targetValue ) override;

    void onNodeCmdBasicSet( const uint8_t nodeId,
                            const uint8_t channelId,
                            const uint8_t currentValue,
                            uint8_t targetValue ) override;

    void onNodeCmdBatteryLevel( const uint8_t nodeId, const uint8_t channelId, const uint8_t value ) override;

    void onNodeCmdBinarySwitch( const uint8_t nodeId,
                                const uint8_t channelId,
                                const uint8_t value,
                                const Uint8Opt targetValue,
                                const SecondsOpt duration ) override;

    void onNodeCmdColorSwitch( const uint8_t nodeId,
                               const uint8_t channelId,
                               const ColorComponentID componentId,
                               const uint8_t value,
                               const utils::optional<uint8_t> targetValue,
                               const SecondsOpt targetDuration ) override;

    void onNodeCmdDoorLockOperation( const uint8_t nodeId,
                                     const uint8_t channelId,
                                     const DoorLock::Mode mode,
                                     const uint8_t insideHandleModes,
                                     const uint8_t outsideHandleModes,
                                     const uint8_t condition,
                                     SecondsOpt lockTimeout,
                                     const DoorLock::ModeOpt tagretMode,
                                     const SecondsOpt tagretModeTimeOut ) override;

    void onNodeCmdDoorLockConfiguration(
        const uint8_t nodeId,
        const uint8_t channelId,
        const znet_door_lock_configuration_t& configuration,
        const znet_door_lock_extended_configuration_t* extendedConfiguration ) override;

    void onNodeCmdNotification( const uint8_t nodeId,
                                const uint8_t channelId,
                                const znet_notification_report_t& value,
                                bool isNotificationRequested ) override;

    void onNodeCmdMultilevelSwitch( const uint8_t nodeId,
                                    const uint8_t channelId,
                                    const uint8_t currentValue,
                                    const Uint8Opt targetValue,
                                    const SecondsOpt targetDuration ) override;

    void onNodeCmdBarrierOperator( const uint8_t nodeId, const uint8_t channelId, const uint8_t value ) override;

    void onNodeCmdBarrierOperatorSignal( const uint8_t nodeId,
                                         const uint8_t channelId,
                                         const uint8_t signalId,
                                         const uint8_t state ) override;

    void onNodeCmdAlarmSensor( const uint8_t nodeId,
                               const uint8_t channelId,
                               const uint8_t sensorType,
                               const uint8_t value,
                               const uint16_t seconds ) override;

    void onNodeCmdBinarySensor( const uint8_t nodeId,
                                const uint8_t channelId,
                                const uint8_t value,
                                const uint8_t sensorType ) override;

    void onNodeCmdSensorMultilevel( znet_node_id_t nodeId,
                                    znet_node_channel_id_t channelId,
                                    const znet_multilevel_sensor_report_t* value ) override;

    void onNodeCmdMeter( const znet_node_id_t nodeId,
                         const znet_node_channel_id_t channelId,
                         const meter::MeterType type,
                         const meter::Scale scale,
                         const int32_t value,
                         const uint8_t precision ) override;

    void onApplicationNodeUpdate( const znet_nodeinfo_t* nodeInfo ) override;

    void onWakeupNotification( uint8_t nodeId ) override;

    void onCentralSceneConfiguration( const uint8_t nodeId, const uint8_t channelId, const uint8_t value ) override;

    void onNodeCmdSceneActuatorConfiguration( const uint8_t nodeId,
                                              const uint8_t channelId,
                                              const uint8_t sceneId,
                                              const uint8_t level,
                                              const uint8_t dimmingDuration ) override;

    void onNodeCmdSceneControllerConfiguration( const uint8_t nodeId,
                                                const uint8_t channelId,
                                                const uint8_t groupId,
                                                const uint8_t sceneId,
                                                const uint8_t dimmingDuration ) override;

    void onCentralSceneNotification( const uint8_t nodeId,
                                     const uint8_t sequence_number,
                                     const uint8_t slow_refresh,
                                     const uint8_t key_attribute,
                                     const uint8_t scene_number ) override;

    void onSceneActivationSetResult( const uint8_t nodeId, const uint8_t scene_number ) override;

    void onSimpleAvControlSetResult( const uint8_t nodeId,
                                     const uint8_t channelId,
                                     const znet_simple_av_set_report_t* value ) override;

    void onConfigurationReport( const uint8_t nodeId,
                                const uint8_t channelId,
                                const uint16_t configParamNum,
                                const uint8_t configParamSize,
                                const uint32_t configParamValue ) override;

    void onConfigurationRequestStatus( const uint8_t nodeId,
                                       const uint8_t channelId,
                                       const uint8_t configParamNum,
                                       RequestStatus::Enum status ) override;

    void onAssociationResult( const uint8_t nodeId,
                              const uint8_t channelId,
                              const znet_association_report_t* value ) override;

    void onMultichannelAssociationResult( const uint8_t nodeId,
                                          const uint8_t channelId,
                                          const znet_multichannel_association_report_t* value ) override;

    void onAssociationSupportedGroupingResult( const uint8_t nodeId,
                                               const uint8_t channelId,
                                               const uint8_t count ) override;

    void onThermostatModeResult( uint8_t nodeId,
                                 uint8_t channelId,
                                 const znet_thermostat_mode_report_t& value ) override;

    void onThermostatFanModeResult( const uint8_t nodeId, const uint8_t value ) override;

    void onThermostatFanStateResult( const uint8_t nodeId, const uint8_t value ) override;

    void onThermostatOperatingStateResult( const uint8_t nodeId, const uint8_t value ) override;

    void onThermostatOperatingStateLoggingResult( const uint8_t nodeId,
                                                  const uint8_t rep_to_follows,
                                                  const uint16_t data_count,
                                                  const uint8_t* data ) override;

    void onThermostatSetpointResult( const uint8_t nodeId,
                                     const uint8_t channelId,
                                     const uint8_t type,
                                     const uint8_t scale,
                                     const uint8_t size,
                                     const uint8_t precision,
                                     const int32_t sp_value,
                                     const bool isSetpointRequested ) override;

    void onThermostatSetbackResult( const znet_node_id_t nodeId,
                                    const uint8_t channelId,
                                    const thermostat_setback::OverrideType type,
                                    const thermostat_setback::Mode mode,
                                    const thermostat_setback::SetbackValueOpt valueOpt ) override;

    void onWakeUpResult( const znet_node_id_t nodeId,
                         const uint32_t interval,
                         const znet_node_id_t targetNodeId ) override;

    void onNodeCmdUserCodeReport( const znet_node_id_t nodeId,
                                  const znet_node_channel_id_t channelId,
                                  const znet_user_code_report_t* user_code_reports,
                                  uint8_t count,
                                  UserCodeNumberOpt nextUserCode,
                                  UserCode::UserPollStatus pollStatus ) override;

    void onNodeScheduleEntryLockWeekDayReport( znet_node_id_t nodeId,
                                               znet_node_channel_id_t channelId,
                                               const znet_week_day_schedule_t& value ) override;

    void onNodeScheduleEntryLockYearDayReport( znet_node_id_t nodeId,
                                               znet_node_channel_id_t channelId,
                                               const znet_year_day_schedule_t& value ) override;

    void onNodeScheduleEntryLockDailyRepeatingReport( znet_node_id_t nodeId,
                                                      znet_node_channel_id_t channelId,
                                                      const znet_daily_repeating_schedule_t& value ) override;

    void onNodeSoundSwitchConfigurationReport( znet_node_id_t nodeId,
                                               znet_node_channel_id_t channelId,
                                               const znet_cmd_sound_switch_configuration_t& value ) override;

    void onNodeSoundSwitchTonePlayReport( znet_node_id_t nodeId,
                                          znet_node_channel_id_t channelId,
                                          const znet_cmd_sound_switch_tone_play_t& value ) override;

    void onNodeMasterCodeReport( znet_node_id_t nodeId,
                                 znet_node_channel_id_t channelId,
                                 uint8_t codeLength,
                                 const uint8_t* masterCode ) override;

    void onNodeUserCodesKeypadModeReport( znet_node_id_t nodeId,
                                          znet_node_channel_id_t channelId,
                                          znet_user_code_keypad_mode_t keypadMode ) override;

    void onNodeCmdHail( const znet_node_id_t nodeId ) override;

    void onIndicatorIdentify() override;

    void onNodeCmdAntiTheftReport( znet_node_id_t nodeId,
                                   znet_node_channel_id_t channelId,
                                   const znet_cmd_anti_theft_report_t& report ) override;

    void onNodeCmdAntiTheftUnlockStateReport( znet_node_id_t nodeId,
                                              znet_node_channel_id_t channelId,
                                              const znet_cmd_anti_theft_unlock_state_report_t& report ) override;
};

}  // namespace znet
