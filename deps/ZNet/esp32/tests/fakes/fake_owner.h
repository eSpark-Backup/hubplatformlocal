#pragma once

#include "fake_nodes_provider.h"
#include "fake_timer_provider.h"

#include <znet/iznet_owner.hpp>
#include <znet/znet_default_date_time_provider.hpp>
#include <znet/iznet_node_setting.hpp>

#include <gmock/gmock.h>

namespace znet {
namespace ut {

class FakeOwner : public IZNetOwner,
                  public FakeNodesProvider,
                  public FakeTimerProvider,
                  public DefaultDateTimeProvider {
public:
    void znetLog( int lvl, const char* fmt, va_list list, void* arg ) override {}

    void* znetAlloc( void* ptr, size_t size, void* arg ) override {
        return nullptr;
    }

    uint32_t znetClock( void* arg ) override {
        return 0;
    }

    bool uartInit() override {
        return false;
    }

    void uartClose() override {}

    int znetUartWrite( const void* data, size_t size, size_t* ret_size ) override {
        return 0;
    }

    int znetUartRead( void* data, size_t size, size_t* ret_size ) override {
        return 0;
    }

    int znetStoreLoad( size_t offset, void* data, size_t size ) override {
        return 0;
    }

    MOCK_METHOD( int, znetStoreSave, ( size_t offset, const void* data, size_t size ), ( override ) );

    MOCK_METHOD( int, znetStoreReset, (), ( override ) );

    void znetS2SelectKeys( znet_S2_keys_t keyList, void* arg ) override {}

    void znetS2RequestPubKey( znet_S2_keys_t keyList, const znet_S2_pub_key_t* key, void* arg ) override {}

    void znetS2ShowPubKey( znet_S2_keys_t keyList, const znet_S2_pub_key_t* key, void* arg ) override {}

    std::unique_ptr<INodeSetting> provideNodeSetting( uint16_t manufacturerId,
                                                      uint16_t productType,
                                                      uint16_t productId ) override {
        return nullptr;
    }

    MOCK_METHOD( void, zwaveModuleResetPinAssert, (), ( override ) );

    MOCK_METHOD( void, zwaveModuleResetPinRelease, (), ( override ) );

    bool getKnownS0Key( uint8_t* buf ) override {
        return false;
    }

    void interviewDone( const uint8_t nodeId ) override {}

    void onCheckReachable( uint8_t nodeId ) override {}

    // TODO: make another overriden functions as mock
    MOCK_METHOD( void, onUpdateReachable, ( uint8_t nodeId, bool reachable ), ( override ) );

    MOCK_METHOD( void, onUpdateState, ( IZNet::Mode cmd, ModeStatus cmdStatus, ErrorOpt errorMessage ), ( override ) );

    void onNodeCheckStatusUpdated( uint8_t nodeId, znet::ModeStatus status ) override {}

    void onDeviceIncludeProgressUpdate( uint8_t progress ) override {}

    void onDeviceRediscoverProgressUpdate( znet_node_id_t nodeId,
                                           bool isOk,
                                           ErrorOpt errorMessage,
                                           uint8_t progress ) override {}

    void onLearnProgressUpdate( uint8_t devicesAmount, uint8_t currentDeviceIndex ) override {}

    MOCK_METHOD( void,
                 onUpdateValue,
                 ( const uint8_t nodeId,
                   const uint8_t command,
                   const uint8_t channelId,
                   const uint8_t currentValue,
                   const Uint8Opt targetValue,
                   const SecondsOpt targetDuration ),
                 ( override ) );

    void onUpdateTokenValue( const uint8_t nodeId,
                             const uint8_t command,
                             const uint8_t channelId,
                             const std::string& value ) override {}

    void onUpdateValueAlarmSensor( const uint8_t nodeId,
                                   const uint8_t channelId,
                                   const AlarmSensorType sensorType,
                                   const uint8_t value,
                                   const uint16_t seconds ) override {}

    void onUpdateValueBinarySensor( const uint8_t nodeId,
                                    const uint8_t command,
                                    const uint8_t channelId,
                                    const uint8_t sensorType,
                                    const uint8_t value ) override {}

    void onUpdateValueMultilevelSensor( const uint8_t nodeId,
                                        const uint8_t ccId,
                                        const uint8_t channelId,
                                        const MultilevelSensor::Type type,
                                        const uint8_t scale,
                                        const float value ) override {}

    void onUpdateValueDoorLockOperation( const uint8_t nodeId,
                                         const uint8_t channelId,
                                         const DoorLock::Mode mode,
                                         const uint8_t insideHandlesMode,
                                         const uint8_t outsideHandlesMode,
                                         const DoorCondition condition,
                                         const SecondsOpt lockTimeout,
                                         const DoorLock::ModeOpt targetMode,
                                         const SecondsOpt targetModeTimeout ) override {}

    void onUpdateValueDoorLockConfiguration(
        const uint8_t nodeId,
        const uint8_t channelId,
        const znet_door_lock_configuration_t& configuration,
        const znet_door_lock_extended_configuration_t* extendedConfiguration ) override {}

    void onUpdateThermostatModeValue( const uint8_t nodeId,
                                      const uint8_t channelId,
                                      const uint8_t mode,
                                      ManufacturerSpecificDataOpt manufacturerSpecificData ) override {}

    void onUpdateValueThermostatSetpoint( const uint8_t nodeId,
                                          const uint8_t channelId,
                                          const uint8_t type,
                                          const uint8_t scale,
                                          const float value,
                                          const bool isSetpointRequested ) override {}

    void onUpdateValueThermostatOperatingState( const uint8_t nodeId,
                                                const uint8_t channelId,
                                                const uint8_t command,
                                                ThermostatOperatingState state ) override {}

    MOCK_METHOD( void,
                 onUpdateThermostatSetbackValue,
                 ( const uint8_t nodeId,
                   const uint8_t channelId,
                   const thermostat_setback::OverrideType overrideType,
                   const thermostat_setback::Mode mode,
                   const thermostat_setback::SetbackValueOpt valueOpt ),
                 ( override ) );

    MOCK_METHOD( void,
                 onUpdateFloatValue,
                 ( const uint8_t nodeId,
                   const uint8_t command,
                   const uint8_t channelId,
                   const meter::MeterType type,
                   const meter::Scale scale,
                   const int32_t value,
                   const uint8_t precision ),
                 ( override ) );

    void onUpdateSceneValue( const uint8_t nodeId,
                             const uint8_t command,
                             const uint8_t channelId,
                             const uint8_t sequenceNumber,
                             const uint8_t sceneNumber,
                             const std::string& keyAtribute ) override {}

    void onSceneActivated( const uint8_t nodeId,
                           const uint8_t command,
                           const uint8_t channelId,
                           const uint8_t sceneNumber ) override {}

    void onSimpleAvControlSet( const uint8_t nodeId,
                               const uint8_t channelId,
                               const uint8_t sequenceNumber,
                               const std::string& keyAtribute,
                               std::vector<uint16_t>&& commands ) override {}

    void onUpdateRgbValue( const uint8_t nodeId,
                           const uint8_t command,
                           const uint8_t channelId,
                           ColorComponentInfos&& components ) override {}

    void onConfigurationReport( const uint8_t nodeId,
                                const uint8_t channelId,
                                const uint16_t configParamNum,
                                const uint8_t configParamSize,
                                const uint32_t configParamValue ) override {}

    void onConfigurationRequestStatus( const uint8_t nodeId,
                                       const uint8_t configParamNum,
                                       RequestStatus::Enum status ) override {}

    void onNotification( const uint8_t nodeId,
                         const uint8_t channelId,
                         const uint8_t v1AlarmType,
                         const uint8_t v1AlarmLevel,
                         NotificationType notification,
                         NotificationEvent event,
                         const uint8_t* data,
                         const uint8_t dataSize ) override {}

    void onUnknownNotification( const uint8_t nodeId,
                                const uint8_t channelId,
                                const uint8_t v1AlarmType,
                                const uint8_t v1AlarmLevel,
                                NotificationType notification,
                                NotificationEvent event,
                                const uint8_t* data,
                                const uint8_t dataSize ) override {}

    void onAssociationSupportedGroupingResult( const uint8_t nodeId, const uint8_t count ) override {}

    void onAssociationResult( const uint8_t nodeId,
                              const uint8_t channelId,
                              const uint8_t groupId,
                              const std::vector<uint8_t>& nodes ) override {}

    void onMultichannelAssociationResult( const uint8_t nodeId,
                                          const uint8_t channelId,
                                          const uint8_t groupId,
                                          const std::vector<std::pair<uint8_t, uint8_t> >& nodes ) override {}

    MOCK_METHOD( void,
                 onUserCodeUpdated,
                 ( const znet_node_id_t nodeId,
                   const znet_node_channel_id_t channelId,
                   znet::UserCode::UserCodeRanges&& userCodeRanges,
                   const znet_supported_users_t usersCount,
                   znet::UserCode::UserPollStatus userPollingStatus ),
                 ( override ) );

    void onMasterCodeUpdated( const znet_node_id_t nodeId,
                              const znet_node_channel_id_t channelId,
                              const std::string& masterCode ) override {}

    void onUserCodesKeypadModeUpdated( const znet_node_id_t nodeId,
                                       const znet_node_channel_id_t channelId,
                                       UserCode::KeypadMode keypadMode ) override {}

    void onUpdateScheduleEntryLockWeekDay( znet_node_id_t nodeId,
                                           znet_node_channel_id_t channelId,
                                           const znet_week_day_schedule_t& value ) override {}

    void onUpdateScheduleEntryLockYearDay( znet_node_id_t nodeId,
                                           znet_node_channel_id_t channelId,
                                           const znet_year_day_schedule_t& value ) override {}

    void onUpdateScheduleEntryLockDailyRepeating( znet_node_id_t nodeId,
                                                  znet_node_channel_id_t channelId,
                                                  const znet_daily_repeating_schedule_t& value ) override {}

    void onUpdateSoundSwitchConfiguration( znet_node_id_t nodeId,
                                           znet_node_channel_id_t channelId,
                                           const znet_cmd_sound_switch_configuration_t& value ) override {}

    void onUpdateSoundSwitchTonePlay( znet_node_id_t nodeId,
                                      znet_node_channel_id_t channelId,
                                      const znet_cmd_sound_switch_tone_play_t& value ) override {}

    void onNodeTargetFirmwareUpdateProgress( const znet_node_id_t nodeId,
                                             const uint8_t target,
                                             FirmwareUpdate::UpdatingResult status,
                                             uint8_t progress ) override {}

    void onUpdateBarrierSignalValue( znet_node_id_t nodeId,
                                     znet_node_channel_id_t channelId,
                                     uint8_t signalId,
                                     uint8_t value ) override {}

    void onHail( const znet_node_id_t nodeId ) override {}

    void onNif( const znet_node_id_t nodeId ) override {}

    void onSisUpdated( bool enabled ) override {}

    void onReconstructNetworkUpdate( const uint8_t devicesAmount, const uint8_t currentDeviceIndex ) override {}

    void onReconstructNetworkError( const znet_node_id_t deviceId ) override {}

    void onIndicatorIdentify() override {}

    MOCK_METHOD( void, fillManufacturerSpecific, (znet_manufacturer_specific_report_t&), ( override ) );

    MOCK_METHOD( void, fillVersion, (znet_version_report_t&), ( override ) );

    MOCK_METHOD( void, fillZWavePlusInfo, (znet_zwaveplus_info_report_t&), ( override ) );

    MOCK_METHOD( void,
                 fillFirmwareUpdateInfo,
                 (znet_firmware_md_report_v1_t&, znet_firmware_md_report_v3_t&, znet_firmware_md_report_v5_t&),
                 ( override ) );

    MOCK_METHOD( void, fillFirmwareLocale, (std::string&), ( override ) );

    MOCK_METHOD( void, onWakeupIntervalUpdated, (znet_node_id_t, Seconds, bool), ( override ) );

    MOCK_METHOD( void,
                 onAntiTheftStateUpdated,
                 (znet_node_id_t, znet_node_channel_id_t, znet_command_class_t, const AntiTheftInfoOpt&),
                 ( override ) );
};

}  // namespace ut
}  // namespace znet
