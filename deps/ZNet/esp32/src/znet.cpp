#include <znet/znet.hpp>
#include <znet/znet_provisioning_list_controller.hpp>

#include <znet_impl.hpp>
extern "C" {
#include "znet_log.h"
}

namespace znet {

ZNet::ZNet(::znet::IProvisioningListProvider& classicInclusionSecurityProvider,
           ::znet::IProvisioningListProvider& provisioningListProvider,
           ::znet::IProvisioningListObserver& provisioningListObserver ) {
    m_impl.reset(
        new ZNetImpl( *this, classicInclusionSecurityProvider, provisioningListProvider, provisioningListObserver ) );
}

ZNet::~ZNet() = default;

void ZNet::resetZwaveModule( bool immediately ) {
    if( immediately ) {
        ZNET_LOGD( "Reset zwave module\n" );
        m_impl->resetZwaveModule();
    }
    else {
        m_needReset = true;
    }
}

void ZNet::initZnet() {
    m_impl->initZnet();
}

void ZNet::startExclude( uint8_t nodeId ) {
    m_impl->startExclude( nodeId );
}

void ZNet::stopExclude() {
    m_impl->stopExclude();
}

void ZNet::startInclude() {
    m_impl->startInclude();
}

void ZNet::stopInclude() {
    m_impl->stopInclude();
}

void ZNet::startLearn() {
    m_impl->startLearn();
}

void ZNet::stopLearn() {
    m_impl->stopLearn();
}

void ZNet::stopSecurityInclude() {
    m_impl->stopSecurityInclude();
}

void ZNet::sendNif() {
    m_impl->sendNif();
}

bool ZNet::requestNif( uint8_t nodeId ) {
    return m_impl->requestNif( nodeId );
}

bool ZNet::sendNop( uint8_t nodeId ) {
    return m_impl->sendNop( nodeId );
}

void ZNet::setPublicKey( znet_S2_pub_key_t* pubKey ) {
    m_impl->setPublicKey( pubKey );
}

void ZNet::s2SelectKeys( znet_S2_keys_t selected_keys ) {
    m_impl->s2SelectKeys( selected_keys );
}

void ZNet::reconstructNetwork() {
    m_impl->reconstructNetwork();
}

void ZNet::getBackgroundRSSI() {
    m_impl->getBackgroundRSSI();
}

bool ZNet::isResetting() {
    return m_impl->isResetting();
}

bool ZNet::isBusy() {
    return m_impl->isBusy();
}

bool ZNet::onLoop() {
    bool result = m_impl->onLoop();
    if( !( m_impl->checkState( Mode::Initializing, ModeStatus::Invoked ) ||
           m_impl->checkState( Mode::Initializing, ModeStatus::Started ) ||
           m_impl->checkState( Mode::Initializing, ModeStatus::InProgress ) ) &&
        m_needReset ) {
        m_impl->resetZwaveModule();
        m_needReset = false;
    }
    return result;
}

bool ZNet::addNodeToGroup( znet_node_id_t nodeId,
                           znet_node_channel_id_t channelId,
                           znet_grouping_id_t groupId,
                           znet_node_id_t targetNodeId,
                           znet_node_channel_id_t targetChannelId,
                           bool force ) {
    return m_impl->addNodeToGroup( nodeId, channelId, groupId, targetNodeId, targetChannelId, force );
}

bool ZNet::removeNodeFromGroup( znet_node_id_t nodeId,
                                znet_node_channel_id_t channelId,
                                znet_grouping_id_t groupId,
                                znet_node_id_t targetNodeId,
                                znet_node_channel_id_t targetChannelId ) {
    return m_impl->removeNodeFromGroup( nodeId, channelId, groupId, targetNodeId, targetChannelId );
}

bool ZNet::setupCommandClassAssociation( znet_node_id_t nodeId, znet_command_class_t cmdClass ) {
    return m_impl->setupCommandClassAssociation( nodeId, cmdClass );
}

bool ZNet::isIncludeExcludeAvailable() const {
    return m_impl->isIncludeExcludeAvailable();
}

void ZNet::requestAllNodesValues( bool extended ) {
    m_impl->requestAllNodesValues( extended );
}

void ZNet::requestNodeValues( const uint8_t nodeId,
                              std::chrono::milliseconds requestDelay,
                              bool extended,
                              bool force,
                              size_t unsetPinCodesCountToStop ) {
    m_impl->requestNodeValues( nodeId, requestDelay, extended, force, unsetPinCodesCountToStop );
}

void ZNet::interviewNode( const uint8_t nodeId ) {
    m_impl->interviewNode( nodeId );
}

void ZNet::synchronizeNodeTime( const uint8_t nodeId ) {
    m_impl->synchronizeNodeTime( nodeId );
}

void ZNet::updateTimeSettings() {
    m_impl->synchronizeAllNodesTime();
}

::znet::api::IProvisioningListController& ZNet::getProvisioningListController() {
    return m_impl->getProvisioningListController();
}

::znet::api::IProvisioningListQueryDataSource& ZNet::getProvisioningListControllerDataSource() {
    return m_impl->getProvisioningListControllerDataSource();
}

RequestsManager& ZNet::getRequestsManager() {
    return m_impl->getRequestsManager();
}

ReachableCheckTimeoutSettings& ZNet::getReachableCheckTimeoutSettings() {
    return m_impl->getReachableCheckTimeoutSettings();
}

CommandClassValuesSetter& ZNet::getCommandClassValuesSetter() {
    return m_impl->getCommandClassValuesSetter();
}

std::unique_ptr<CommonInfo> ZNet::getCommonInfo() {
    return m_impl->getCommonInfo();
}

void ZNet::updateNodeTargetFirmware( znet_node_id_t nodeId, uint8_t target, FirmwareUpdate::SourceUPtr source ) {
    m_impl->updateNodeTargetFirmware( nodeId, target, std::move( source ) );
}

void ZNet::updateReachableCheckTimeoutSettings( bool useFixedWakeUpInterval,
                                                uint32_t wakeUpInterval,
                                                uint32_t alwaysOnDevicesInetrval,
                                                uint8_t attemptsCount ) {
    m_impl->updateReachableCheckTimeoutSettings(
        useFixedWakeUpInterval, wakeUpInterval, alwaysOnDevicesInetrval, attemptsCount );
}

void ZNet::checkReachableStatus( const uint8_t nodeId ) {
    m_impl->checkReachableStatus( nodeId );
}

void ZNet::checkNodeValues( const uint8_t nodeId,
                            std::chrono::milliseconds requestDelay,
                            std::vector<std::pair<uint8_t, uint8_t> > dataToCheck ) {
    m_impl->checkNodeValues( nodeId, requestDelay, dataToCheck );
}

bool ZNet::createChipBackup( IOWrapper::DestinationUPtr destination ) {
    return m_impl->createChipBackup( std::move( destination ) );
}

bool ZNet::restoreChipBackup( IOWrapper::SourceUPtr source ) {
    return m_impl->restoreChipBackup( std::move( source ) );
}

}  // namespace znet
