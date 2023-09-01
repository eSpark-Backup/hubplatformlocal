#pragma once

#include <znet/znet.h>
#include <znet/iassociations_configurator.h>
#include <znet/znet_firmware_update.hpp>
#include <znet/znet_io_wrapper.hpp>

#include <cstdint>
#include <chrono>

#include <memory>
#include <vector>

namespace znet {
namespace api {
class IProvisioningListController;
class IProvisioningListQueryDataSource;
}  // namespace api

class CommonInfo;
class RequestsManager;
class CommandClassValuesSetter;
class ReachableCheckTimeoutSettings;

class IZNet : public IAssociationsConfigurator {
public:
    enum class Mode {
        Initializing = 0,
        IncludeStarting,
        IncludeStopping,
        ExcludeStarting,
        ExcludeStopping,
        LearnStarting,
        LearnStopping,
        Monitoring,
        NifSending,
        NodeAdding,
        NodeRemoving,
        NodeLearning,
        NodeListGetting,
        ValuesGetting,
        ValuesGettingExtended,
        TimeSynchronization,
        Rediscover,
        FirmwareUpdate,
        Reseting,
        RequestNeighbor,
        InclusionControllerStarting,
        InclusionControllerStartingReplace,
        NodeChecking,
        BackupCreation,
        BackupRestoring,
        Idle
    };

public:
    virtual ~IZNet() = default;

    virtual void resetZwaveModule( bool immediately ) = 0;

    virtual void initZnet() = 0;

    virtual void startExclude( uint8_t nodeId ) = 0;

    virtual void stopExclude() = 0;

    virtual void startInclude() = 0;

    virtual void stopInclude() = 0;

    virtual void startLearn() = 0;

    virtual void stopLearn() = 0;

    virtual void reconstructNetwork() = 0;

    virtual void getBackgroundRSSI() = 0;

    virtual void stopSecurityInclude() = 0;

    virtual void sendNif() = 0;

    virtual bool requestNif( uint8_t nodeId ) = 0;

    virtual bool sendNop( uint8_t nodeId ) = 0;

    virtual void setPublicKey( znet_S2_pub_key_t* pubKey ) = 0;

    virtual void s2SelectKeys( znet_S2_keys_t selected_keys ) = 0;

    virtual bool isIncludeExcludeAvailable() const = 0;

    virtual bool isResetting() = 0;

    virtual bool isBusy() = 0;

    virtual bool onLoop() = 0;

    virtual void requestAllNodesValues( bool extended = true ) = 0;

    virtual void requestNodeValues( const uint8_t nodeId,
                                    std::chrono::milliseconds requestDelay,
                                    bool extended = true,
                                    bool force = false,
                                    size_t unsetPinCodesCountToStop = 0 ) = 0;

    virtual void interviewNode( const uint8_t nodeId ) = 0;

    virtual void synchronizeNodeTime( const uint8_t nodeId ) = 0;

    virtual void updateTimeSettings() = 0;

    virtual ::znet::api::IProvisioningListController& getProvisioningListController() = 0;
    virtual ::znet::api::IProvisioningListQueryDataSource& getProvisioningListControllerDataSource() = 0;

    virtual RequestsManager& getRequestsManager() = 0;

    virtual ReachableCheckTimeoutSettings& getReachableCheckTimeoutSettings() = 0;

    virtual CommandClassValuesSetter& getCommandClassValuesSetter() = 0;

    virtual std::unique_ptr<CommonInfo> getCommonInfo() = 0;

    virtual void updateNodeTargetFirmware( znet_node_id_t nodeId,
                                           uint8_t target,
                                           FirmwareUpdate::SourceUPtr source ) = 0;

    virtual void updateReachableCheckTimeoutSettings( bool useFixedWakeUpInterval,
                                                      uint32_t wakeUpInterval,
                                                      uint32_t alwaysOnDevicesInetrval,
                                                      uint8_t attemptsCount ) = 0;

    virtual void checkReachableStatus( const uint8_t nodeId ) = 0;

    virtual void checkNodeValues( const uint8_t nodeId,
                                  std::chrono::milliseconds requestDelay,
                                  std::vector<std::pair<uint8_t, uint8_t> > dataToCheck ) = 0;

    virtual bool createChipBackup( IOWrapper::DestinationUPtr destination ) = 0;

    virtual bool restoreChipBackup( IOWrapper::SourceUPtr source ) = 0;
};
}  // namespace znet
