#pragma once

#include <znet/iznet.hpp>
#include <znet/znet_notification.hpp>
#include <znet/znet_node.hpp>
#include <znet/znet_command.hpp>
#include <znet/znet_error_message.hpp>
#include <znet/znet_request_status.hpp>
#include <znet/znet_color_switch.hpp>
#include <znet/enums/multilevel_sensor.hpp>
#include <znet/enums/user_code.hpp>
#include <znet/enums/meter_type.hpp>
#include <znet/enums/thermostat_setback.hpp>
#include <znet/znet_door_condition.hpp>
#include <znet/znet_alarm_sensor.hpp>
#include <znet/iznet_date_time_provider.hpp>
#include <znet/iznet_nodes_provider.hpp>
#include <znet/iznet_timer_provider.hpp>
#include <znet/znet_firmware_update.hpp>
#include <znet/anti_theft_info.hpp>

#include <znet/optional.hpp>

#include <cstdint>

namespace znet {

class INodeSetting;
enum class ThermostatOperatingState;
using Seconds = std::chrono::seconds;
using SecondsOpt = utils::optional<Seconds>;
using Uint8Opt = utils::optional<uint8_t>;
using ManufacturerSpecificData = std::vector<uint8_t>;
using ManufacturerSpecificDataOpt = utils::optional<ManufacturerSpecificData>;

class IZNetOwner : public virtual INodesProvider, public virtual IDateTimeProvider, public virtual ITimerProvider {
public:
    virtual ~IZNetOwner() = default;

    virtual void znetLog( int lvl, const char* fmt, va_list list, void* arg ) = 0;

    virtual void* znetAlloc( void* ptr, size_t size, void* arg ) = 0;

    virtual uint32_t znetClock( void* arg ) = 0;

    virtual bool uartInit() = 0;

    virtual void uartClose() = 0;

    virtual int znetUartWrite( const void* data, size_t size, size_t* ret_size ) = 0;

    virtual int znetUartRead( void* data, size_t size, size_t* ret_size ) = 0;

    virtual int znetStoreSave( size_t offset, const void* data, size_t size ) = 0;

    virtual int znetStoreLoad( size_t offset, void* data, size_t size ) = 0;

    virtual int znetStoreReset() = 0;

    virtual void znetS2SelectKeys( znet_S2_keys_t keyList, void* arg ) = 0;

    virtual void znetS2RequestPubKey( znet_S2_keys_t keyList, const znet_S2_pub_key_t* key, void* arg ) = 0;

    virtual void znetS2ShowPubKey( znet_S2_keys_t keyList, const znet_S2_pub_key_t* key, void* arg ) = 0;

    virtual std::unique_ptr<INodeSetting> provideNodeSetting( uint16_t manufacturerId,
                                                              uint16_t productType,
                                                              uint16_t productId ) = 0;

    virtual void zwaveModuleResetPinRelease() = 0;
    virtual void zwaveModuleResetPinAssert() = 0;

    virtual bool getKnownS0Key( uint8_t* buf ) = 0;

public:
    virtual void interviewDone( const uint8_t nodeId ) = 0;

    virtual void onCheckReachable( uint8_t nodeId ) = 0;

    void onCheckReachable();

    virtual void onUpdateReachable( uint8_t nodeId, bool reachable ) = 0;

    virtual void onUpdateState( IZNet::Mode cmd, ModeStatus cmdStatus, ErrorOpt error = {} ) = 0;

    virtual void onNodeCheckStatusUpdated( uint8_t nodeId, ModeStatus status ) = 0;

    virtual void onDeviceIncludeProgressUpdate( uint8_t progress ) = 0;

    virtual void onDeviceRediscoverProgressUpdate( znet_node_id_t nodeId,
                                                   bool isOk,
                                                   ErrorOpt errorMessage,
                                                   uint8_t progress ) = 0;

    virtual void onLearnProgressUpdate( uint8_t devicesAmount, uint8_t currentDeviceIndex ) = 0;

    virtual void onReconstructNetworkUpdate( const uint8_t devicesAmount, const uint8_t currentDeviceIndex ) = 0;

    virtual void onReconstructNetworkError( const znet_node_id_t deviceId ) = 0;

    virtual void onUpdateValue( const uint8_t nodeId,
                                const uint8_t command,
                                const uint8_t channelId,
                                const uint8_t currentValue,
                                const utils::optional<uint8_t> targetValue = {},
                                const SecondsOpt targetDuration = {} ) = 0;

    virtual void onUpdateTokenValue( const uint8_t nodeId,
                                     const uint8_t command,
                                     const uint8_t channelId,
                                     const std::string& value ) = 0;

    virtual void onUpdateValueAlarmSensor( const uint8_t nodeId,
                                           const uint8_t channelId,
                                           const AlarmSensorType sensorType,
                                           const uint8_t value,
                                           const uint16_t seconds ) = 0;

    virtual void onUpdateValueBinarySensor( const uint8_t nodeId,
                                            const uint8_t command,
                                            const uint8_t channelId,
                                            const uint8_t sensorType,
                                            const uint8_t value ) = 0;

    virtual void onUpdateValueMultilevelSensor( const uint8_t nodeId,
                                                const uint8_t ccId,
                                                const uint8_t channelId,
                                                const MultilevelSensor::Type type,
                                                const uint8_t scale,
                                                const float value ) = 0;

    virtual void onUpdateValueDoorLockOperation( const uint8_t nodeId,
                                                 const uint8_t channelId,
                                                 const DoorLock::Mode mode,
                                                 const uint8_t insideHandlesMode,
                                                 const uint8_t outsideHandlesMode,
                                                 const DoorCondition condition,
                                                 const SecondsOpt lockTimeout,
                                                 const DoorLock::ModeOpt targetMode,
                                                 const SecondsOpt targetModeTimeout ) = 0;

    virtual void onUpdateValueDoorLockConfiguration(
        const uint8_t nodeId,
        const uint8_t channelId,
        const znet_door_lock_configuration_t& configuration,
        const znet_door_lock_extended_configuration_t* extendedConfiguration ) = 0;

    virtual void onUpdateThermostatModeValue( const uint8_t nodeId,
                                              const uint8_t channelId,
                                              const uint8_t mode,
                                              ManufacturerSpecificDataOpt manufacturerSpecificData ) = 0;

    virtual void onUpdateValueThermostatSetpoint( const uint8_t nodeId,
                                                  const uint8_t channelId,
                                                  const uint8_t type,
                                                  const uint8_t scale,
                                                  const float value,
                                                  const bool isSetpointRequested ) = 0;

    virtual void onUpdateValueThermostatOperatingState( const uint8_t nodeId,
                                                        const uint8_t channelId,
                                                        const uint8_t command,
                                                        ThermostatOperatingState state ) = 0;

    virtual void onUpdateThermostatSetbackValue( const uint8_t nodeId,
                                                 const uint8_t channelId,
                                                 const thermostat_setback::OverrideType overrideType,
                                                 const thermostat_setback::Mode mode,
                                                 const thermostat_setback::SetbackValueOpt valueOpt ) = 0;

    virtual void onUpdateFloatValue( const uint8_t nodeId,
                                     const uint8_t command,
                                     const uint8_t channelId,
                                     const meter::MeterType type,
                                     const meter::Scale scale,
                                     const int32_t value,
                                     const uint8_t precision ) = 0;

    virtual void onUpdateSceneValue( const uint8_t nodeId,
                                     const uint8_t command,
                                     const uint8_t channelId,
                                     const uint8_t sequenceNumber,
                                     const uint8_t sceneNumber,
                                     const std::string& keyAtribute ) = 0;

    virtual void onSceneActivated( const uint8_t nodeId,
                                   const uint8_t command,
                                   const uint8_t channelId,
                                   const uint8_t sceneNumber ) = 0;

    virtual void onSimpleAvControlSet( const uint8_t nodeId,
                                       const uint8_t channelId,
                                       const uint8_t sequenceNumber,
                                       const std::string& keyAtribute,
                                       std::vector<uint16_t>&& commands ) = 0;

    virtual void onUpdateRgbValue( const uint8_t nodeId,
                                   const uint8_t command,
                                   const uint8_t channelId,
                                   ColorComponentInfos&& components ) = 0;

    virtual void onConfigurationReport( const uint8_t nodeId,
                                        const uint8_t channelId,
                                        const uint16_t configParamNum,
                                        const uint8_t configParamSize,
                                        const uint32_t configParamValue ) = 0;

    virtual void onConfigurationRequestStatus( const uint8_t nodeId,
                                               const uint8_t configParamNum,
                                               RequestStatus::Enum status ) = 0;

    virtual void onNotification( const uint8_t nodeId,
                                 const uint8_t channelId,
                                 const uint8_t v1AlarmType,
                                 const uint8_t v1AlarmLevel,
                                 NotificationType notification,
                                 NotificationEvent event,
                                 const uint8_t* data,
                                 const uint8_t dataSize ) = 0;

    virtual void onUnknownNotification( const uint8_t nodeId,
                                        const uint8_t channelId,
                                        const uint8_t v1AlarmType,
                                        const uint8_t v1AlarmLevel,
                                        NotificationType notification,
                                        NotificationEvent event,
                                        const uint8_t* data,
                                        const uint8_t dataSize ) = 0;

    virtual void onAssociationSupportedGroupingResult( const uint8_t nodeId, const uint8_t count ) = 0;

    virtual void onAssociationResult( const uint8_t nodeId,
                                      const uint8_t channelId,
                                      const uint8_t groupId,
                                      const std::vector<uint8_t>& nodes ) = 0;

    virtual void onMultichannelAssociationResult( const uint8_t nodeId,
                                                  const uint8_t channelId,
                                                  const uint8_t groupId,
                                                  const std::vector<std::pair<uint8_t, uint8_t> >& nodes ) = 0;

    virtual void onUserCodeUpdated( const znet_node_id_t nodeId,
                                    const znet_node_channel_id_t channelId,
                                    UserCode::UserCodeRanges&& userCodeRanges,
                                    const znet_supported_users_t usersCount,
                                    UserCode::UserPollStatus userPollingStatus ) = 0;

    virtual void onMasterCodeUpdated( const znet_node_id_t nodeId,
                                      const znet_node_channel_id_t channelId,
                                      const std::string& masterCode ) = 0;

    virtual void onUserCodesKeypadModeUpdated( const znet_node_id_t nodeId,
                                               const znet_node_channel_id_t channelId,
                                               UserCode::KeypadMode keypadMode ) = 0;

    virtual void onUpdateScheduleEntryLockWeekDay( znet_node_id_t nodeId,
                                                   znet_node_channel_id_t channelId,
                                                   const znet_week_day_schedule_t& value ) = 0;

    virtual void onUpdateScheduleEntryLockYearDay( znet_node_id_t nodeId,
                                                   znet_node_channel_id_t channelId,
                                                   const znet_year_day_schedule_t& value ) = 0;

    virtual void onUpdateScheduleEntryLockDailyRepeating( znet_node_id_t nodeId,
                                                          znet_node_channel_id_t channelId,
                                                          const znet_daily_repeating_schedule_t& value ) = 0;

    virtual void onUpdateSoundSwitchConfiguration( znet_node_id_t nodeId,
                                                   znet_node_channel_id_t channelId,
                                                   const znet_cmd_sound_switch_configuration_t& value ) = 0;

    virtual void onUpdateSoundSwitchTonePlay( znet_node_id_t nodeId,
                                              znet_node_channel_id_t channelId,
                                              const znet_cmd_sound_switch_tone_play_t& value ) = 0;

    virtual void onNodeTargetFirmwareUpdateProgress( const znet_node_id_t nodeId,
                                                     const uint8_t target,
                                                     FirmwareUpdate::UpdatingResult status,
                                                     uint8_t progress ) = 0;

    virtual void onUpdateBarrierSignalValue( znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             uint8_t signalId,
                                             uint8_t value ) = 0;

    virtual void onHail( const znet_node_id_t nodeId ) = 0;

    virtual void onIndicatorIdentify() = 0;

    virtual void onNif( const znet_node_id_t nodeId ) = 0;

    virtual void onSisUpdated( bool enabled ) = 0;

    virtual void fillManufacturerSpecific( znet_manufacturer_specific_report_t& data ) = 0;

    virtual void fillVersion( znet_version_report_t& data ) = 0;

    virtual void fillZWavePlusInfo( znet_zwaveplus_info_report_t& data ) = 0;

    virtual void fillFirmwareUpdateInfo( znet_firmware_md_report_v1_t& dataV1,
                                         znet_firmware_md_report_v3_t& dataV3,
                                         znet_firmware_md_report_v5_t& dataV5 ) = 0;

    virtual void fillFirmwareLocale( std::string& locale ) = 0;

    virtual void onWakeupIntervalUpdated( const znet_node_id_t nodeId, Seconds interval, bool isChanged ) = 0;

    virtual void onAntiTheftStateUpdated( znet_node_id_t nodeId,
                                          znet_node_channel_id_t channelId,
                                          znet_command_class_t commandClassId,
                                          const AntiTheftInfoOpt& antiTheftInfo ) = 0;
};

}  // namespace znet
