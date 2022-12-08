#pragma once

#include <znet/iznet_owner.hpp>
#include <znet/iznet.hpp>

namespace znet {
class IProvisioningListProvider;
class IProvisioningListObserver;

struct CommonInfo {
    uint8_t m_type;
    std::string m_module;
    std::string m_module_firmware_version;
    std::string m_locale;
    std::string m_home_id;
    uint8_t m_node_id;
    //        std::string m_port;  // for Linux
    struct Pole {
        uint32_t m_powered;
        uint32_t m_battery;
    } m_pole;

    struct Roles {
        uint8_t m_sis : 1;
        uint8_t m_primary : 1;
        uint8_t m_secondary : 1;
        uint8_t m_static : 1;
        uint8_t m_inclusion : 1;
    } m_roles;
};

class ZNetImpl;

class ZNet : public IZNet, public IZNetOwner {
public:
    ZNet(::znet::IProvisioningListProvider&, ::znet::IProvisioningListProvider&, ::znet::IProvisioningListObserver& );
    virtual ~ZNet() override;

    void resetZwaveModule( bool immediately ) override;
    void initZnet() override;
    void startExclude( uint8_t nodeId ) override;
    void stopExclude() override;
    void startInclude() override;
    void stopInclude() override;
    void startLearn() override;
    void stopLearn() override;
    void stopSecurityInclude() override;
    void sendNif() override;
    bool requestNif( uint8_t nodeId ) override;
    bool sendNop( uint8_t nodeId ) override;
    void reconstructNetwork() override;
    void getBackgroundRSSI() override;

    void setPublicKey( znet_S2_pub_key_t* pubKey ) override;
    void s2SelectKeys( znet_S2_keys_t selected_keys ) override;

    bool isIncludeExcludeAvailable() const override;

    bool isResetting() override;
    bool isBusy() override;
    bool onLoop() override;

    bool addNodeToGroup( znet_node_id_t nodeId,
                         znet_node_channel_id_t channelId,
                         znet_grouping_id_t groupId,
                         znet_node_id_t targetNodeId,
                         znet_node_channel_id_t targetChannelId,
                         bool force = false ) override;

    bool removeNodeFromGroup( znet_node_id_t nodeId,
                              znet_node_channel_id_t channelId,
                              znet_grouping_id_t groupId,
                              znet_node_id_t targetNodeId,
                              znet_node_channel_id_t targetChannelId ) override;

    bool setupCommandClassAssociation( znet_node_id_t nodeId, znet_command_class_t cmdClass ) override;

    void requestAllNodesValues( bool extended = true ) override;

    void requestNodeValues( const uint8_t nodeId,
                            std::chrono::milliseconds requestDelay,
                            bool extended = true,
                            bool force = false,
                            size_t unsetPinCodesCountToStop = 0 ) override;

    void interviewNode( const uint8_t nodeId ) override;

    void synchronizeNodeTime( const uint8_t nodeId ) override;

    void updateTimeSettings() override;

    ::znet::api::IProvisioningListController& getProvisioningListController() override;
    ::znet::api::IProvisioningListQueryDataSource& getProvisioningListControllerDataSource() override;

    RequestsManager& getRequestsManager() override;

    ReachableCheckTimeoutSettings& getReachableCheckTimeoutSettings() override;

    CommandClassValuesSetter& getCommandClassValuesSetter() override;

    std::unique_ptr<CommonInfo> getCommonInfo() override;

    void updateNodeTargetFirmware( znet_node_id_t nodeId, uint8_t target, FirmwareUpdate::SourceUPtr source ) override;

    void updateReachableCheckTimeoutSettings( bool useFixedWakeUpInterval,
                                              uint32_t wakeUpInterval,
                                              uint32_t alwaysOnDevicesInetrval,
                                              uint8_t attemptsCount ) override;

    void checkReachableStatus( const uint8_t nodeId ) override;

    void checkNodeValues( const uint8_t nodeId,
                          std::chrono::milliseconds requestDelay,
                          std::vector<std::pair<uint8_t, uint8_t> > dataToCheck ) override;

    bool createChipBackup( IOWrapper::DestinationUPtr destination ) override;

    bool restoreChipBackup( IOWrapper::SourceUPtr source ) override;

private:
    bool m_needReset = false;
    std::unique_ptr<ZNetImpl> m_impl;
};

}  // namespace znet
