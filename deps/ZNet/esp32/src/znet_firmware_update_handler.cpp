#include "znet_firmware_update_handler.hpp"
#include "znet_impl.hpp"

#include "znet/znet_node.hpp"
#include "znet/cc_0x7a_firmware_update.hpp"

extern "C" {
#include "znet_log.h"
}

#include <cassert>

namespace znet {

FirmwareUpdateHandler::FirmwareUpdateHandler( ZNetImpl& parent )
: ZNetHandler( parent ) {}

void FirmwareUpdateHandler::addTarget( znet_node_id_t nodeId, uint8_t target, FirmwareUpdate::SourceUPtr source ) {
    assert( source );

    const auto it = std::find_if( m_targets.begin(), m_targets.end(), [nodeId, target]( const TargetInfo& targetInfo ) {
        return targetInfo.nodeId == nodeId && targetInfo.target == target;
    } );

    if( it != m_targets.end() ) {
        m_targets.erase( it );
    }
    m_targets.push_back( TargetInfo{ nodeId, target, std::move( source ) } );
}

bool FirmwareUpdateHandler::isDirty() const {
    return !m_targets.empty();
}

bool FirmwareUpdateHandler::isInProgress() const {
    return m_currentNode != 0;
}

void FirmwareUpdateHandler::initialize() {
    assert( !isInProgress() );
    m_parentMesh.setState( IZNet::Mode::FirmwareUpdate, ModeStatus::Started );

    doStep();
}

void FirmwareUpdateHandler::finalize( ErrorOpt ) {}

void FirmwareUpdateHandler::onDeactivated() {
    assert( "Don't deactivate FirmwareUpdateHandler" );
}

bool FirmwareUpdateHandler::onLoop() {
    if( !isInProgress() ) {
        m_parentMesh.setState( IZNet::Mode::FirmwareUpdate, ModeStatus::Finished );
        m_parentMesh.activateMode( IZNet::Mode::Monitoring );
    }

    return true;
}

void FirmwareUpdateHandler::onNodeCmdFirmwareUpdateMdRequestReport(
    znet_node_id_t nodeId,
    znet_node_channel_id_t channelId,
    FirmwareUpdate::UpdateMdRequestReportStatus status ) {
    // TODO: try to implement following logic via request manager

    if( auto cc = getFirmwareUpdateCc( nodeId, channelId, __func__ ) ) {
        cc->onUpdateRequestStatusReceived( status );
    }
}

void FirmwareUpdateHandler::onNodeCmdFirmwareUpdateMdRequested( znet_node_id_t nodeId,
                                                                znet_node_channel_id_t channelId,
                                                                uint8_t requestedReportsCount,
                                                                uint16_t reportNumber ) {
    // TODO: try to implement following logic via request manager

    if( auto cc = getFirmwareUpdateCc( nodeId, channelId, __func__ ) ) {
        cc->onDataRequested( requestedReportsCount, reportNumber );
    }
}

void FirmwareUpdateHandler::onNodeCmdFirmwareUpdateMdStatusReport( znet_node_id_t nodeId,
                                                                   znet_node_channel_id_t channelId,
                                                                   FirmwareUpdate::DataSendingStatus status,
                                                                   uint16_t waitingTime ) {
    // TODO: try to implement following logic via request manager

    if( auto cc = getFirmwareUpdateCc( nodeId, channelId, __func__ ) ) {
        cc->onDataSendingStatusReceived( status, waitingTime );
    }
}

void FirmwareUpdateHandler::onNodeFirmwareUpdateMdActivationStatusReport( znet_node_id_t nodeId,
                                                                          znet_node_channel_id_t channelId,
                                                                          uint8_t target,
                                                                          const znet_firmware_info_t& firmware,
                                                                          FirmwareUpdate::ActivationStatus status,
                                                                          Uint8Opt /*hardwareId*/ ) {
    // TODO: try to implement following logic via request manager

    if( auto cc = getFirmwareUpdateCc( nodeId, channelId, __func__ ) ) {
        cc->onActivationStatusReceived( target, firmware, status );
    }
}

std::shared_ptr<ZCc0x7AFirmwareUpdate> FirmwareUpdateHandler::getFirmwareUpdateCc( znet_node_id_t nodeId,
                                                                                   znet_node_channel_id_t channelId,
                                                                                   const char* method ) {
    auto node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s : could not find node info Id = %d\n", method, nodeId );
        return {};
    }

    auto cc = node->getCommandClass<ZCc0x7AFirmwareUpdate>( channelId );

    if( !cc ) {
        ZNET_LOGE( "%s : could not find firmware update CC. Node Id = %d\n", method, nodeId );
    }

    return cc;
}

bool FirmwareUpdateHandler::canHandleNode( znet_node_id_t nodeId ) const {
    return nodeId && m_currentNode == nodeId;
}

void FirmwareUpdateHandler::doStep() {
    while( true ) {
        if( m_targets.empty() ) {
            m_currentNode = 0;
            break;
        }

        auto targetInfo = std::move( m_targets.front() );
        const auto nodeId = targetInfo.nodeId;
        const auto target = targetInfo.target;
        m_currentNode = nodeId;

        m_targets.pop_front();

        auto node = m_parentMesh.getOwner().onFindNode( targetInfo.nodeId );
        if( !node ) {
            m_parentMesh.getOwner().onNodeTargetFirmwareUpdateProgress(
                nodeId, target, FirmwareUpdate::UpdatingResult::Error, 0 );
            continue;
        }

        auto cc = node->getCommandClass<ZCc0x7AFirmwareUpdate>( 0 );
        if( !cc ) {
            m_parentMesh.getOwner().onNodeTargetFirmwareUpdateProgress(
                nodeId, target, FirmwareUpdate::UpdatingResult::Error, 0 );
            continue;
        }

        const auto updateStarted = cc->update( targetInfo.target,
                                               std::move( targetInfo.source ),
                                               [=]( FirmwareUpdate::UpdatingResult status, uint8_t progress ) {
                                                   m_parentMesh.getOwner().onNodeTargetFirmwareUpdateProgress(
                                                       nodeId, target, status, progress );
                                                   if( status != FirmwareUpdate::UpdatingResult::InProgress ) {
                                                       doStep();
                                                   }
                                               } );

        if( updateStarted ) {
            break;
        }
    }
}

}  // namespace znet
