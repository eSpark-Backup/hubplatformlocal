#pragma once

#include "znet/znet.h"
#include "znet/znet_node.hpp"
#include "znet_handler.hpp"
#include "znet_handler_interview.hpp"
#include "znet_handler_exclude.hpp"
#include "znet_handler_learn.hpp"
#include "znet_handler_stop_learn.hpp"
#include "znet_handler_values.hpp"
#include "znet_handler_values_extended.hpp"
#include "znet_handler_monitoring.hpp"
#include "znet_handler_inclusion_controller.hpp"
#include "znet_handler_inclusion_controller_replace.hpp"
#include "znet_handler_backup_creation.hpp"
#include "znet_handler_backup_restoring.hpp"
#include "znet_time_synchronization_handler.hpp"
#include "znet_discover_handler.hpp"
#include "znet_firmware_update_handler.hpp"
#include "znet_handler_node_check.hpp"
#include "znet/znet_reachable_check_timeout_settings.hpp"

#include "znet_time_manager.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_command_class_values_setter.hpp"
#include "znet/znet_provisioning_list_controller.hpp"
#include "znet/znet_provisioning_list_controller_data_source.hpp"
#include "znet/znet_provisioning_list_provider_data_source.hpp"
#include "znet/node_provisioning_manager.hpp"

namespace znet {
namespace ut {
class ZNetImplAccessor;
}

namespace api {
class IProvisioningListQueryDataSource;
}  // namespace api

class IZNetOwner;

class ZNetImpl {
    friend class ut::ZNetImplAccessor;

    struct ZNetState {
        IZNet::Mode m_cmd = IZNet::Mode::Idle;
        ModeStatus m_cmdStatus = ModeStatus::Empty;
    };

private:
    static constexpr auto WaitingDelayGet = 10000;

public:
    ZNetImpl( IZNetOwner&,
              ::znet::IProvisioningListProvider&,
              ::znet::IProvisioningListProvider&,
              ::znet::IProvisioningListObserver& );
    bool onLoop();

public:  //  nma API
    void resetZwaveModule( bool sendDeviceResetLocallyNotifications = true );
    void initZnet();
    void startExclude( uint8_t nodeId );
    void stopExclude();
    void startInclude();
    void stopInclude();
    void startLearn();
    void stopLearn();
    void stopSecurityInclude();
    void sendNif();
    bool requestNif( uint8_t nodeId );
    bool sendNop( uint8_t nodeId );
    void reconstructNetwork();
    void getBackgroundRSSI();

    void setPublicKey( znet_S2_pub_key_t* pubKey );
    void s2SelectKeys( znet_S2_keys_t selected_keys );

    bool isResetting();

    ZNode* onFindNode( znet_node_id_t nodeId );

    void requestAllNodesValues( bool extended = true );

    void requestNodeValues( uint8_t nodeId,
                            std::chrono::milliseconds requestDelay,
                            bool extended = true,
                            bool force = false,
                            size_t unsetPinCodesCountToStop = 0 );
    void requestNodeValues( uint8_t nodeId, bool extended = true, bool force = false );

    void interviewNode( uint8_t nodeId );

    void synchronizeAllNodesTime();

    void synchronizeNodeTime( uint8_t nodeId );

    void updateReachableCheckTimeoutSettings( bool useFixedWakeUpInterval,
                                              uint32_t wakeUpInterval,
                                              uint32_t alwaysOnDevicesInetrval,
                                              uint8_t attemptsCount );

    void checkReachableStatus( const uint8_t nodeId );

    void checkNodeValues( const uint8_t nodeId,
                          std::chrono::milliseconds requestDelay,
                          std::vector<std::pair<uint8_t, uint8_t>> dataToCheck );

    bool createChipBackup( IOWrapper::DestinationUPtr destination );

    bool restoreChipBackup( IOWrapper::SourceUPtr source );

public:
    void activateMode( const IZNet::Mode mode );
    bool isPrimaryController() const;
    bool isIncludeExcludeAvailable() const;
    bool isBusy( IZNet::Mode except = IZNet::Mode::Idle ) const;

    void setState( IZNet::Mode cmd, ModeStatus cmdStatus = ModeStatus::Invoked, ErrorOpt err = ErrorOpt{} );

    bool checkState( IZNet::Mode cmd, ModeStatus cmdStatus = ModeStatus::Undefined );

    void checkReachableNodesState();
    void setReachableState( const znet_node_id_t nodeId, bool reachable, bool forceNotify = false );

    void updateRediscoverProgress( const znet_node_id_t nodeId,
                                   const bool isOk,
                                   ErrorOpt errorMessage,
                                   const uint8_t progress );

    void updateInclusionProgress( const uint8_t progress );

    void updateLearnProgress( uint8_t devicesAmount, uint8_t currentDeviceIndex );

    void updateReconstructNetworkProgress( const uint8_t devicesAmount, const uint8_t currentDeviceIndex );

    void updateReconstructNetworkError( const znet_node_id_t nodeId );

    bool addNodeToGroup( znet_node_id_t nodeId,
                         znet_node_channel_id_t channelId,
                         znet_grouping_id_t groupId,
                         znet_node_id_t targetNodeId,
                         znet_node_channel_id_t targetChannelId,
                         bool force = false );

    bool removeNodeFromGroup( znet_node_id_t nodeId,
                              znet_node_channel_id_t channelId,
                              znet_grouping_id_t groupId,
                              znet_node_id_t targetNodeId,
                              znet_node_channel_id_t targetChannelId );

    bool setupCommandClassAssociation( znet_node_id_t nodeId, znet_command_class_t cmdClass );

    znet::ZNetHandler& getHandler( const znet_node_id_t nodeId = 0 );

    IZNetOwner& getOwner();

    ::znet::api::IProvisioningListController& getProvisioningListController();
    ::znet::api::IProvisioningListQueryDataSource& getProvisioningListControllerDataSource();
    ::znet::api::IProvisioningListQueryDataSource& getClassicInclusionSecurityDataSource();
    void resetDeviceSecurityData( uint8_t nodeId, ::znet::ProvisioningListEntryFields::Status status );

    TimeManager& getTimeManager();

    NodeProvisioningManager& getNodeProvisioningManager();

    RequestsManager& getRequestsManager();

    ReachableCheckTimeoutSettings& getReachableCheckTimeoutSettings();

    CommandClassValuesSetter& getCommandClassValuesSetter();

    std::unique_ptr<CommonInfo> getCommonInfo();

    void updateNodeTargetFirmware( znet_node_id_t nodeId, uint8_t target, FirmwareUpdate::SourceUPtr source );

    const znet_callbacks_t& getCallbacks() const;

private:
    ZNetImpl() = delete;

    void setupDstTimer();

    void initWakupNodesState();

    /// INFO: ZNet callbacks
    static void znetLog( int lvl, const char* fmt, va_list list, void* arg );
    static void* znetAlloc( void* ptr, size_t size, void* arg );
    static uint64_t znetClock( void* arg );
    static int znetUartWrite( const void* data, size_t size, size_t* ret_size, void* arg );
    static int znetUartRead( void* data, size_t size, size_t* ret_size, void* arg );
    static int znetStoreSave( size_t offset, const void* data, size_t size, void* arg );
    static int znetStoreLoad( size_t offset, void* data, size_t size, void* arg );
    static int znetStoreReset( void* arg );

    static void znetNodeAdd( int err, const znet_nodeinfo_t* node_info, void* arg );
    static void znetNodeAddSecurityFailed( znet_node_id_t, void* arg );
    static void znetNodeAddBreak( int err, const znet_nodeinfo_t* node_info, void* arg );
    static void znetNodeAddInvoked( void* arg );
    static void znetNodeRem( int err, const znet_nodeinfo_t* node_info, void* arg );
    static void znetNodeRemBreak( int err, const znet_nodeinfo_t* node_info, void* arg );
    static void znetNodeRemInvoked( void* arg );
    static void znetNodeLearn( znet_error_t err, znet_node_id_t dest_node_id, void* arg );
    static void znetNodeLearnBreak( int err, znet_node_id_t destNodeId, void* arg );
    static void znetNodeLearnInvoked( void* arg );
    static void znetNodeList( int err, const uint8_t* node_info, void* arg );

    static void znetS2SelectKeys( znet_S2_keys_t keyList, znet_node_id_t node_id, void* arg );
    static void znetS2RequestPubKey( znet_node_id_t node_id,
                                     znet_S2_keys_t keyList,
                                     const znet_S2_pub_key_t* key,
                                     void* arg );
    static void znetS2ShowPubKey( znet_S2_keys_t keyList, const znet_S2_pub_key_t* key, void* arg );

    static void znetNodeInifReceived( uint8_t nodeId, uint8_t* nhid, bool foreign, void* arg );
    static void znetNodeNhidReceived( uint8_t* nhid, void* arg );

    static void hardResetZwaveChip( void* arg );
    static void sisUpdated( int err, znet_node_id_t sis_id, void* arg );
    static void libraryInfoUpdated( int err, znet_library_info_t* info, uint8_t lib_type, void* arg );

    static void znetForcedRequest( int err, void* arg );
    static void znetRequestNeighborCB( int err, znet_node_id_t node_id, void* arg );

    static bool znetCheckNodeSupportsCommandClass( znet_node_id_t nodeId,
                                                   znet_node_channel_id_t endpoint,
                                                   znet_command_class_t commandClassId,
                                                   void* arg );

    static void znetNodeCmdVersion( int err, znet_node_id_t node_id, const znet_version_report_t* value, void* arg );
    static void znetNodeCmdCommandVersion( znet_error_t err,
                                           znet_node_id_t nodeId,
                                           znet_node_channel_id_t channel_id,
                                           znet_command_class_t class_id,
                                           znet_command_class_version_t version,
                                           void* arg );
    static void znetNodeCmdVersionCapabilities( int err,
                                                znet_node_id_t node_id,
                                                const znet_version_capabilities_report_t* value,
                                                void* arg );
    static void znetNodeCmdSoftwareVersion( int err,
                                            znet_node_id_t node_id,
                                            const znet_software_version_report_t* value,
                                            void* arg );

    static void znetNodeCmdManufacturerSpecific( int err,
                                                 znet_node_id_t node_id,
                                                 znet_node_channel_id_t channel_id,
                                                 const znet_manufacturer_specific_report_t* value,
                                                 void* arg );
    static void znetNodeCmdDeviceSpecific( int err,
                                           znet_node_id_t node_id,
                                           const znet_device_specific_report_t* value,
                                           void* arg );
    static void znetNodeCmdZWavePlusInfo( int err,
                                          znet_node_id_t node_id,
                                          const znet_zwaveplus_info_report_t* value,
                                          void* arg );
    static void znetNodeCmdManufacturerSpecificFill( znet_manufacturer_specific_report_t* data, void* arg );
    static void znetNodeCmdVersionFill( znet_version_report_t* data, void* arg );
    static void znetNodeCmdZWavePlusFill( znet_zwaveplus_info_report_t* data, void* arg );
    static void znetNodeCmdFirmwareUpdateInfoFill( znet_firmware_md_report_v1_t* dataV1,
                                                   znet_firmware_md_report_v3_t* dataV3,
                                                   znet_firmware_md_report_v5_t* dataV5,
                                                   void* arg );
    static void znetNodeCmdBasic( int err,
                                  znet_node_id_t node_id,
                                  znet_node_channel_id_t channel_id,
                                  znet_cmd_basic_value_t currentValue,
                                  znet_cmd_basic_value_t targetValue,
                                  uint8_t duration,
                                  void* arg );
    static void znetNodeCmdBasicSet( int err,
                                     znet_node_id_t node_id,
                                     znet_node_channel_id_t channel_id,
                                     znet_cmd_basic_value_t currentValue,
                                     znet_cmd_basic_value_t targetValue,
                                     uint8_t duration,
                                     void* arg );
    static void znetNodeCmdBatteryLevel( int err,
                                         znet_node_id_t node_id,
                                         znet_node_channel_id_t channel_id,
                                         znet_cmd_battery_level_value_t value,
                                         void* arg );
    static void znetNodeCmdBinarySwitch( znet_error_t err,
                                         znet_node_id_t nodeId,
                                         znet_node_channel_id_t channelId,
                                         znet_cmd_binary_switch_value_t value,
                                         znet_cmd_binary_switch_value_t targetValue,
                                         uint8_t duration,
                                         void* arg );

    static void znetNodeCmdClock( int err,
                                  znet_node_id_t node_id,
                                  znet_node_channel_id_t channel_id,
                                  const znet_clock_value_t* value,
                                  void* arg );

    static void znetNodeCmdTimeParameters( int err,
                                           znet_node_id_t node_id,
                                           znet_node_channel_id_t channel_id,
                                           const znet_cmd_time_parameters_report_t* value,
                                           void* arg );

    static void znetNodeCmdColorSwitch( znet_error_t err,
                                        znet_node_id_t node_id,
                                        znet_node_channel_id_t channel_id,
                                        uint8_t color_component,
                                        const znet_color_switch_report_t* report,
                                        void* arg );

    static void znetNodeCmdColorSwitchSupported( znet_error_t err,
                                                 znet_node_id_t node_id,
                                                 znet_node_channel_id_t channel_id,
                                                 const znet_color_switch_supported_report_t* report,
                                                 void* arg );

    static void znetNodeCmdDoorLockOperation( znet_error_t err,
                                              znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              const znet_door_lock_operation_report_t* value,
                                              void* arg );

    static void znetNodeCmdDoorLockConfiguration( znet_error_t err,
                                                  znet_node_id_t nodeId,
                                                  znet_node_channel_id_t channelId,
                                                  const znet_door_lock_configuration_t* configuration,
                                                  const znet_door_lock_extended_configuration_t* extendedConfiguration,
                                                  void* arg );

    static void znetNodeCmdDoorLockCapabilities( znet_error_t err,
                                                 znet_node_id_t node_id,
                                                 znet_node_channel_id_t channel_id,
                                                 const znet_door_lock_capabilities_t* capabilities,
                                                 void* arg );

    static void znetNodeCmdNotification( znet_error_t err,
                                         znet_node_id_t node_id,
                                         znet_node_channel_id_t channel_id,
                                         const znet_notification_report_t* value,
                                         bool isNotificationRequested,
                                         void* arg );

    static void znetNodeCmdNotificationSupported( int err,
                                                  znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id,
                                                  const znet_notification_supported_report_t* value,
                                                  void* arg );

    static void znetNodeCmdNotificationEventSupported( int err,
                                                       znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       uint8_t type,
                                                       const uint8_t* bitmask,
                                                       uint8_t bitmaskSize,
                                                       void* arg );
    static void znetNodeCmdHail( int err, znet_node_id_t node_id, void* arg );
    static void znetNodeCmdMultilevelSwitch( znet_error_t err,
                                             znet_node_id_t node_id,
                                             znet_node_channel_id_t channel_id,
                                             znet_cmd_multilevel_switch_value_t currentValue,
                                             znet_cmd_multilevel_switch_value_t targetValue,
                                             uint8_t duration,
                                             void* arg );
    static void znetNodeCmdMultilevelSwitchSetDone( int err,
                                                    znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    void* arg );
    static void znetNodeCmdMultilevelSwitchSupported( int err,
                                                      znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      const znet_multilevel_switch_supported_report_t* value,
                                                      void* arg );
    static void znetNodeCmdBarrierOperator( int err,
                                            znet_node_id_t nodeId,
                                            znet_node_channel_id_t channelId,
                                            znet_cmd_barrier_operator_value_t value,
                                            void* arg );
    static void znetNodeCmdBarrierOperatorSignal( int err,
                                                  znet_node_id_t nodeId,
                                                  znet_node_channel_id_t channelId,
                                                  znet_cmd_barrier_operator_signal_t signalId,
                                                  znet_cmd_barrier_operator_signal_state_t state,
                                                  void* arg );
    static void znetNodeCmdBarrierOperatorSupportedSignals( int err,
                                                            znet_node_id_t nodeId,
                                                            znet_node_channel_id_t channelId,
                                                            const uint8_t* data,
                                                            uint8_t len,
                                                            void* arg );
    static void znetNodeCmdBarrierOperatorSetDone( int err,
                                                   znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   void* arg );
    static void znetNodeCmdAlarmSensorGetReport( znet_error_t err,
                                                 znet_node_id_t node_id,
                                                 znet_node_channel_id_t channel_id,
                                                 uint8_t sensor_type,
                                                 uint8_t value,
                                                 uint16_t seconds,
                                                 void* arg );
    static void znetNodeCmdAlarmSensorSupportedSensor( znet_error_t err,
                                                       znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       const uint8_t* data,
                                                       uint8_t len,
                                                       void* arg );
    static void znetNodeCmdBinarySensorGetReport( znet_error_t err,
                                                  znet_node_id_t node_d,
                                                  znet_node_channel_id_t channel_id,
                                                  uint8_t sensorType,
                                                  uint8_t value,
                                                  void* arg );
    static void znetNodeCmdBinarySensorSupportedSensor( znet_error_t err,
                                                        znet_node_id_t node_id,
                                                        znet_node_channel_id_t channel_id,
                                                        const uint8_t* data,
                                                        uint8_t len,
                                                        void* arg );
    static void znetNodeCmdSensorMultilevelSupportedSensor( znet_error_t err,
                                                            znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id,
                                                            const uint8_t* data,
                                                            uint8_t len,
                                                            void* arg );
    static void znetNodeCmdSensorMultilevelSupportedScale( znet_error_t err,
                                                           znet_node_id_t nodeId,
                                                           znet_node_channel_id_t channelId,
                                                           uint8_t sensor_type,
                                                           uint8_t scale_mask,
                                                           void* arg );
    static void znetNodeCmdSensorMultilevelGetReport( znet_error_t err,
                                                      znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      uint8_t type,
                                                      const znet_multilevel_sensor_report_t* value,
                                                      void* arg );
    static void znetNodeCmdMeter( znet_error_t err,
                                  znet_node_id_t node_id,
                                  znet_node_channel_id_t channel_id,
                                  uint8_t scale,
                                  const znet_meter_report_t* value,
                                  void* arg );
    static void znetNodeCmdMeterSetDone( int err,
                                         znet_node_id_t node_id,
                                         znet_node_channel_id_t channel_id,
                                         void* arg );
    static void znetNodeCmdMeterSupported( int err,
                                           znet_node_id_t node_id,
                                           znet_node_channel_id_t channel_id,
                                           const znet_meter_supported_report_t* value,
                                           void* arg );
    static void znetNodeCmdMultichannelEndpoint( int err,
                                                 znet_node_id_t node_id,
                                                 const znet_multichannel_endpoint_report_t* value,
                                                 void* arg );
    static void znetNodeCmdMultichannelCapability( int err,
                                                   znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   const znet_multichannel_capability_report_t* value,
                                                   void* arg );
    static void znetNodeCmdMultichannelEndpointFind( int err,
                                                     znet_node_id_t node_id,
                                                     const znet_multichannel_endpoint_find_report_t* value,
                                                     void* arg );
    static void znetNodeCmdMultichannelAggregatedMembers( int err,
                                                          znet_node_id_t node_id,
                                                          znet_node_channel_id_t channel_id,
                                                          const znet_multichannel_aggregated_members_report_t* value,
                                                          void* arg );
    static void znetNodeCmdPowerlevel( int err,
                                       znet_node_id_t node_id,
                                       const znet_powerlevel_report_t* value,
                                       void* arg );
    static void znetNodeCmdPowerlevelTestNode( int err,
                                               znet_node_id_t node_id,
                                               const znet_powerlevel_test_node_report_t* value,
                                               void* arg );
    static void znetNodeCmdConfiguration( znet_error_t err,
                                          znet_node_id_t node_id,
                                          znet_node_channel_id_t channel_id,
                                          uint8_t parameter_number,
                                          const znet_configuration_report_t* value,
                                          void* arg );
    static void znetNodeCmdConfigurationRequestStatus( znet_error_t err,
                                                       znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       uint8_t parameter_number,
                                                       void* arg );
    static void znetNodeCmdConfigurationBulk( int err,
                                              znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              znet_cmd_configuration_id_t param_number,
                                              uint8_t config_count,
                                              const znet_configuration_bulk_report_t* value,
                                              void* arg );
    static void znetNodeCmdConfigurationName( int err,
                                              znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              znet_cmd_configuration_id_t param_number,
                                              const znet_configuration_name_report_t* value,
                                              void* arg );
    static void znetNodeCmdConfigurationInfo( int err,
                                              znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              znet_cmd_configuration_id_t param_number,
                                              const znet_configuration_info_report_t* value,
                                              void* arg );
    static void znetNodeCmdConfigurationProperties( int err,
                                                    znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    znet_cmd_configuration_id_t param_number,
                                                    const znet_configuration_properties_report_t* value,
                                                    void* arg );

    static void znetNodeCmdSceneActuatorConfiguration( int err,
                                                       znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       znet_cmd_scene_id_t sceneId,
                                                       const znet_scene_actuator_configuration_report_t* value,
                                                       void* arg );

    static void znetNodeCmdSceneControllerConfiguration( int err,
                                                         znet_node_id_t node_id,
                                                         znet_node_channel_id_t channel_id,
                                                         const znet_scene_controller_configuration_report_t* value,
                                                         void* arg );

    static void znetNodeCmdCentralSceneConfiguration( int err,
                                                      znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      znet_cmd_central_scene_value_t value,
                                                      void* arg );

    static void znetNodeCmdCentralSceneSupported( int err,
                                                  znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id,
                                                  const znet_central_scene_supported_report_t* value,
                                                  void* arg );

    static void znetCentralSceneNotification( int err,
                                              znet_node_id_t nodeId,
                                              const znet_central_scene_notification_t* value,
                                              void* arg );

    static void znetSceneActivationSetResult( znet_error_t err,
                                              znet_node_id_t nodeId,
                                              const znet_scene_activation_t sceneNumber,
                                              void* arg );

    static void znetApplicationNodeUpdate( znet_error_t err,
                                           znet_node_id_t nodeId,
                                           const znet_nodeinfo_t* node_info,
                                           void* arg );

    static void znetNodeResetLocaly( int err, const znet_node_id_t nodeId, void* arg );

    static void znetInclusionControllerInitiate( znet_error_t err,
                                                 const znet_node_id_t node_id,
                                                 const znet_node_id_t child_node_id,
                                                 const znet_inclution_controller_step_id_t step_id,
                                                 void* arg );

    static void znetInclusionControllerComplete( znet_error_t err,
                                                 const znet_node_id_t node_id,
                                                 const znet_inclution_controller_step_id_t step_id,
                                                 const znet_inclution_controller_status_t status,
                                                 void* arg );

    static void znetNodeProvisioningSetReceived( znet_error_t err,
                                                 const znet_node_id_t nodeId,
                                                 znet_node_channel_id_t channelId,
                                                 const znet_node_provisioning_set_t* value,
                                                 void* arg );

    static void znetNodeProvisioningDeleteReceived( znet_error_t err,
                                                    const znet_node_id_t nodeId,
                                                    znet_node_channel_id_t channelId,
                                                    const znet_node_provisioning_delete_t* value,
                                                    void* arg );

    static void znetNodeProvisioningGetReceived( znet_error_t err,
                                                 const znet_node_id_t nodeId,
                                                 znet_node_channel_id_t channelId,
                                                 uint8_t sequenceNumber,
                                                 const uint8_t dskKey[16],
                                                 void* arg );

    static void znetNodeProvisioningListIterationGetReceived( znet_error_t err,
                                                              const znet_node_id_t nodeId,
                                                              znet_node_channel_id_t channelId,
                                                              uint8_t sequenceNumber,
                                                              uint8_t remainingCounter,
                                                              void* arg );

    static void znetNodeCmdSecuritySupported( int err,
                                              znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              const znet_security_supported_report_t* value,
                                              void* arg );

    static void znetModuleResetStarted( void* arg );

    static void znetModuleReset( int err, void* arg );

    static void znetInitFinish( int err, void* arg );

    static void znetNodeNoOperation( znet_error_t err, const znet_node_id_t nodeId, void* arg );

    static void znetWakeupResult( znet_error_t err,
                                  znet_node_id_t nodeId,
                                  znet_node_channel_id_t channelId,
                                  znet_cmd_wakeup_value_t value,
                                  znet_node_id_t target_node_id,
                                  void* arg );

    static void znetWakeupNotification( znet_error_t err, znet_node_id_t nodeId, void* arg );

    static void znetWakeupCapabilitiesResult( znet_error_t err,
                                              znet_node_id_t nodeId,
                                              znet_node_channel_id_t channelId,
                                              const znet_wakeup_capabilities_report_t* value,
                                              void* arg );

    static void znetAssociationGroupNameResult( int err,
                                                znet_node_id_t nodeId,
                                                znet_node_channel_id_t channelId,
                                                znet_grouping_id_t grouping_id,
                                                const znet_association_group_name_report_t* value,
                                                void* arg );

    static void znetAssociationGroupInfoResult( int err,
                                                znet_node_id_t nodeId,
                                                znet_node_channel_id_t channelId,
                                                znet_grouping_id_t grouping_id,
                                                const znet_association_group_info_report_t* value,
                                                void* arg );

    static void znetAssociationSupportedGroupingResult( int err,
                                                        znet_node_id_t nodeId,
                                                        znet_node_channel_id_t channelId,
                                                        const uint8_t value,
                                                        void* arg );

    static void znetAssociationGroupCommandListResult( int err,
                                                       znet_node_id_t nodeId,
                                                       znet_node_channel_id_t channelId,
                                                       znet_grouping_id_t grouping_id,
                                                       const znet_association_group_command_list_report_t* value,
                                                       void* arg );

    static void znetAssociationResult( int err,
                                       znet_node_id_t nodeId,
                                       znet_node_channel_id_t channelId,
                                       znet_grouping_id_t grouping_id,
                                       const znet_association_report_t* value,
                                       void* arg );

    static void znetAssociationSetResult( znet_error_t err,
                                          znet_node_id_t nodeId,
                                          znet_node_channel_id_t channelId,
                                          znet_grouping_id_t grouping_id,
                                          void* arg );

    static void znetAssociationRemoveResult( znet_error_t err,
                                             znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             void* arg );

    static void znetMultichannelAssociationResult( int err,
                                                   znet_node_id_t nodeId,
                                                   znet_node_channel_id_t channelId,
                                                   znet_grouping_id_t groupingId,
                                                   const znet_multichannel_association_report_t* value,
                                                   void* arg );

    static void znetMultichannelAssociationSetResult( znet_error_t err,
                                                      znet_node_id_t nodeId,
                                                      znet_node_channel_id_t channelId,
                                                      znet_grouping_id_t groupingId,
                                                      void* arg );

    static void znetMultichannelAssociationRemoveResult( znet_error_t err,
                                                         znet_node_id_t nodeId,
                                                         znet_node_channel_id_t channelId,
                                                         void* arg );

    static void znetMultichannelAssociationSupportedGroupingResult( int err,
                                                                    znet_node_id_t nodeId,
                                                                    znet_node_channel_id_t channelId,
                                                                    const uint8_t value,
                                                                    void* arg );

    static void znetSimpleAvControlSetResult( znet_error_t err,
                                              znet_node_id_t nodeId,
                                              znet_node_channel_id_t channelId,
                                              const znet_simple_av_set_report_t* value,
                                              void* arg );

    static void znetThermostatModeResult( int err,
                                          znet_node_id_t nodeId,
                                          znet_node_channel_id_t channelId,
                                          const znet_thermostat_mode_report_t* value,
                                          void* arg );

    static void znetThermostatModeSupportedResult( int err,
                                                   znet_node_id_t nodeId,
                                                   znet_node_channel_id_t channelId,
                                                   const znet_thermostat_mode_supported_report_t* value,
                                                   void* arg );

    static void znetThermostatFanModeResult( int err,
                                             znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             const uint8_t value,
                                             void* arg );

    static void znetThermostatFanModeSupportedResult( int err,
                                                      znet_node_id_t nodeId,
                                                      znet_node_channel_id_t channelId,
                                                      const znet_thermostat_fan_mode_supported_report_t* value,
                                                      void* arg );
    static void znetThermostatFanStateResult( int err,
                                              znet_node_id_t nodeId,
                                              znet_node_channel_id_t channelId,
                                              const uint8_t value,
                                              void* arg );

    static void znetThermostatOperatingStateResult( int err,
                                                    znet_node_id_t nodeId,
                                                    znet_node_channel_id_t channelId,
                                                    const uint8_t value,
                                                    void* arg );

    static void znetThermostatOperatingStateLoggingSupportedResult(
        int err,
        znet_node_id_t nodeId,
        znet_node_channel_id_t channelId,
        const znet_thermostat_operating_state_logging_supported_report_t* value,
        void* arg );

    static void znetThermostatOperatingStateLoggingResult( int err,
                                                           znet_node_id_t nodeId,
                                                           znet_node_channel_id_t channelId,
                                                           znet_thermostat_operating_state_logging_report_t* value,
                                                           void* arg );

    static void znetThermostatSetbackResult( int err,
                                             znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             const uint8_t type,
                                             const int8_t state,
                                             void* arg );

    static void znetThermostatSetpointResult( int err,
                                              znet_node_id_t nodeId,
                                              znet_node_channel_id_t channelId,
                                              uint8_t type,
                                              const znet_thermostat_setpoint_report_t* value,
                                              bool requested,
                                              void* arg );

    static void znetThermostatSetpointSupportedResult( int err,
                                                       znet_node_id_t nodeId,
                                                       znet_node_channel_id_t channelId,
                                                       const znet_thermostat_setpoint_supported_report_t* value,
                                                       void* arg );

    static void znetThermostatSetpointCapabilitiesResult( int err,
                                                          znet_node_id_t nodeId,
                                                          znet_node_channel_id_t channelId,
                                                          uint8_t type,
                                                          const znet_thermostat_setpoint_capabilities_report_t* value,
                                                          void* arg );

    static void znetNodeCmdUserCodeReport( znet_error_t err,
                                           znet_node_id_t node_id,
                                           uint8_t user_id,
                                           znet_node_channel_id_t channel_id,
                                           const znet_user_code_report_t* value,
                                           void* arg );

    static void znetNodeCmdUsersNumberReport( znet_error_t err,
                                              znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              znet_supported_users_t supported_users,
                                              void* arg );

    static void znetZwavePlusInfoGet( void* arg );

    static void znetIndicatorIdentify( void* arg );
    static void znetIndicatorReport( znet_error_t err,
                                     znet_node_id_t nodeId,
                                     znet_node_channel_id_t channelId,
                                     uint8_t indicatorId,
                                     const znet_indicator_report_t* value,
                                     void* arg );
    static void znetIndicatorSupportedReport( znet_error_t err,
                                              znet_node_id_t nodeId,
                                              znet_node_channel_id_t channelId,
                                              uint8_t indicatorId,
                                              const znet_indicator_supported_report_t* value,
                                              void* arg );

    static znet_error_t znetNodeIsAdded( znet_node_id_t node_id, void* arg );
    static uint8_t znetSecurityGet( znet_node_id_t node_id, void* arg );
    static int znetSecuritySet( znet_node_id_t node_id, uint8_t security, void* arg );
    static znet_error_t znetNodeIsSupportedCC( znet_node_id_t node_id, znet_command_class_t command, void* arg );
    static int znetNodeIsWakeup( znet_node_id_t node_id, void* arg );

    static void znetNodeTimeRequested( znet_node_id_t node_id,
                                       znet_node_channel_id_t channel_id,
                                       uint8_t encap,
                                       void* arg );
    static void znetNodeDateRequested( znet_node_id_t node_id,
                                       znet_node_channel_id_t channel_id,
                                       uint8_t encap,
                                       void* arg );
    static void znetNodeTimeZoneRequested( znet_node_id_t node_id,
                                           znet_node_channel_id_t channel_id,
                                           uint8_t encap,
                                           void* arg );

    static void znetNodeTimeZoneReport( znet_error_t err,
                                        znet_node_id_t node_id,
                                        znet_node_channel_id_t channel_id,
                                        const znet_time_zone_settings_t* time_zone_settings,
                                        void* arg );

    static void znetNodeTimeZoneSeted( znet_node_id_t node_id,
                                       znet_node_channel_id_t channel_id,
                                       const znet_time_zone_settings_t* time_zone_settings,
                                       void* arg );

    static void znetNodeLocalTimeReport( znet_error_t err,
                                         znet_node_id_t node_id,
                                         znet_node_channel_id_t channel_id,
                                         const znet_local_time_t* local_time,
                                         void* arg );

    static void znetNodeLocalDateReport( znet_error_t err,
                                         znet_node_id_t node_id,
                                         znet_node_channel_id_t channel_id,
                                         const znet_local_date_t* local_date,
                                         void* arg );

    static void znetNodeScheduleLockEntryTimeOffsetReport( znet_error_t err,
                                                           znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           const znet_cmd_schedule_entry_lock_time_offset_t* value,
                                                           void* arg );

    static void znetNodeScheduleLockEntryWeekDayReport( znet_error_t err,
                                                        znet_node_id_t node_id,
                                                        znet_node_channel_id_t channel_id,
                                                        uint8_t user_id,
                                                        uint8_t slot_id,
                                                        const znet_week_day_schedule_t* value,
                                                        void* arg );

    static void znetNodeScheduleLockYearDayReport( znet_error_t err,
                                                   znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   uint8_t user_id,
                                                   uint8_t slot_id,
                                                   const znet_year_day_schedule_t* value,
                                                   void* arg );

    static void znetNodeScheduleLockEntrySupportedReport( znet_error_t err,
                                                          znet_node_id_t node_id,
                                                          znet_node_channel_id_t channel_id,
                                                          const znet_cmd_schedule_entry_lock_supported_report_t* value,
                                                          void* arg );

    static void znetNodeScheduleLockEntryRepeatingReport( znet_error_t err,
                                                          znet_node_id_t node_id,
                                                          znet_node_channel_id_t channel_id,
                                                          uint8_t user_id,
                                                          uint8_t slot_id,
                                                          const znet_daily_repeating_schedule_t* value,
                                                          void* arg );

    static void znetNodeSoundSwitchTonesNumberReport( znet_error_t err,
                                                      znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      znet_cmd_sound_switch_tones_number_t value,
                                                      void* arg );

    static void znetNodeSoundSwitchToneInfoReport( znet_error_t err,
                                                   znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   znet_cmd_sound_switch_tone_id_t toneId,
                                                   const znet_cmd_sound_switch_tone_info_t* value,
                                                   void* arg );

    static void znetNodeSoundSwitchConfigurationReport( znet_error_t err,
                                                        znet_node_id_t node_id,
                                                        znet_node_channel_id_t channel_id,
                                                        const znet_cmd_sound_switch_configuration_t* value,
                                                        void* arg );

    static void znetNodeSoundSwitchTonePlayReport( znet_error_t err,
                                                   znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   const znet_cmd_sound_switch_tone_play_t* value,
                                                   void* arg );

    static void znetNodeUserCodeCapabilitiesReport( znet_error_t err,
                                                    znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    const znet_user_code_capabilities_report_t* user_code_capabilities,
                                                    void* arg );

    static void znetNodeUserCodesKeypadModeReport( znet_error_t err,
                                                   znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   znet_user_code_keypad_mode_t keypad_mode,
                                                   void* arg );

    static void znetNodeExtendedUserCodeReport( znet_error_t err,
                                                znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                znet_supported_users_t requested_user_id,
                                                uint8_t user_codes_count,
                                                const znet_user_code_report_t* user_codes,
                                                znet_supported_users_t nextUserCode,
                                                void* arg );

    static void znetNodeUserCodeChecksumReport( znet_error_t err,
                                                znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                znet_user_code_checksum_t checksum,
                                                void* arg );

    static void znetNodeMasterCodeReport( znet_error_t err,
                                          znet_node_id_t node_id,
                                          znet_node_channel_id_t channel_id,
                                          uint8_t code_length,
                                          const uint8_t* code,
                                          void* arg );

    static void znetNodeFirmwareMdReport( znet_error_t err,
                                          znet_node_id_t node_id,
                                          znet_node_channel_id_t channel_id,
                                          const znet_firmware_md_report_v1_t* report_v1,
                                          const znet_firmware_md_report_v3_t* report_v3,
                                          const znet_firmware_md_report_v5_t* report_v5,
                                          void* arg );

    static void znetNodeFirmwareUpdateMdRequestReport( znet_error_t err,
                                                       znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       uint8_t status,
                                                       void* arg );

    static void znetNodeFirmwareUpdateMdRequested( znet_error_t err,
                                                   znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   uint8_t requested_reports_count,
                                                   uint16_t report_number,
                                                   void* arg );

    static void znetNodeFirmwareUpdateMdReport( znet_error_t err,
                                                znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                uint16_t report_number,
                                                bool is_last_report,
                                                uint8_t data_size,
                                                const uint8_t* data,
                                                void* arg );

    static void znetNodeFirmwareUpdateMdStatusReport( znet_error_t err,
                                                      znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      uint8_t status,
                                                      uint16_t waiting_time,
                                                      void* arg );

    static void znetNodeFirmwareUpdateMdActivationStatusReport(
        znet_error_t err,
        znet_node_id_t node_id,
        znet_node_channel_id_t channel_id,
        uint8_t target,
        const znet_firmware_update_md_activation_status_report_v4_t* report_v4,
        const znet_firmware_update_md_activation_status_report_v5_t* report_v5_opt,
        void* arg );

    static void znetNodeFirmwareUpdateMdPreparationReport( znet_error_t err,
                                                           znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           uint8_t status,
                                                           uint16_t firmware_checksum,
                                                           void* arg );

    static void znetBackupSessionOpenReport( znet_error_t err, znet_backup_status_t status, uint16_t size, void* arg );

    static void znetBackupSessionCloseReport( znet_error_t err, znet_backup_status_t status, void* arg );

    static void znetBackupReadReport( znet_error_t err,
                                      uint16_t requestedOffset,
                                      znet_backup_status_t status,
                                      uint16_t reportedOffset,
                                      uint8_t dataLength,
                                      const uint8_t* data,
                                      void* arg );

    static void znetBackupWriteReport( znet_error_t err,
                                       uint16_t requestedOffset,
                                       znet_backup_status_t status,
                                       uint16_t reportedOffset,
                                       uint8_t reportedLength,
                                       void* arg );

    static void znetAntiTheftReport( znet_error_t err,
                                     znet_node_id_t node_id,
                                     znet_node_channel_id_t channel_id,
                                     const znet_cmd_anti_theft_report_t* report,
                                     void* arg );

    static void znetAntiTheftUnlockStateReport( znet_error_t err,
                                                znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                const znet_cmd_anti_theft_unlock_state_report_t* report,
                                                void* arg );

    static bool znetGetKnownS0Key( uint8_t* buf, void* arg );

private:
    bool tryAutoRespondS2SecurityModes( znet_node_id_t, znet_S2_keys_t );
    bool tryAutoRespondS2PublicKey( znet_node_id_t );

private:
    IZNetOwner& m_znetOwner;
    ::znet::api::ProvisioningListProviderDataSource m_classicInclusionSecurityDataSource;
    ::znet::api::ProvisioningListController m_provisioningListController;
    ::znet::api::ProvisioningListControllerDataSource m_provisioningListControllerDataSource;
    NodeProvisioningManager m_nodeProvisioningManager;
    ZNetState m_state;

    ZNetHandler m_emptyHandler;
    InterviewHandler m_interviewHandler;
    ExcludeHandler m_excludeHandler;
    LearnHandler m_learnHandler;
    StopLearnHandler m_stopLearnHandler;
    CurrentValuesHandler m_currentValuesHandler;
    CurrentValuesExtendedHandler m_currentValuesExtendedHandler;
    MonitoringHandler m_monitoringHandler;
    InclusionHandler m_inclusionHandler;
    InclusionReplaceHandler m_inclusionReplaceHandler;
    TimeSynchronizationHandler m_timeSynchronizationHandler;
    DiscoverHandler m_discoverHandler;
    FirmwareUpdateHandler m_firmwareUpdateHandler;
    NodeCheckHandler m_nodeCheckHandler;
    BackupCreationHandler m_backupCreationHandler;
    BackupRestoringHandler m_backupRestoringHandler;

    ReachableCheckTimeoutSettings m_reachableTimeoutSettings;

    std::reference_wrapper<ZNetHandler> m_cbHandler{ m_emptyHandler };

    TimeManager m_timeManager;
    ITimerProvider::TimerUPtr m_dstTimer;

    using TimeDurationUnitMinutes = std::chrono::minutes;
    const TimeDurationUnitMinutes AllNodesReachableCheckTimeout{ 15 };
    using TimeDurationUnitHours = std::chrono::hours;
    const TimeDurationUnitHours EachNodeReachableCheckTimeout{ 3 };
    const TimeDurationUnitHours CheckTimeoutHoursPerDay{ 24 };

    std::chrono::steady_clock::time_point m_lastNodeStatusCheckingTime;

    znet_callbacks_t m_znet_cb = {};

    RequestsManager m_requestsManager;
    CommandClassValuesSetter m_ccValuesSetter;

    uint8_t m_libraryType;
    std::string m_libraryVersion;

public:
    uint8_t m_devicesAmount;
    uint8_t m_currentDevice;
};

}  // namespace znet
