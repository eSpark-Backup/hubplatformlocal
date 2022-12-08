#include "znet_handler_node_check.hpp"

#include "znet/cc_0x63_user_code.hpp"

#include "znet_impl.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

namespace {

/* @brief
 * ( 3s (per each retry) * 5 (number of retries) ) + 10s ( some additional interval )
 * In perfect, interval is not needed becasuse CurrentValuesHandler has been refactored via requests manager.
 * Interval is still used because of potential bugs.
 */
const std::chrono::seconds NodeCheckHandlerTimeout(
    35 );  //  ( 3s (per each retry) * 5 (number of retries) ) + 5 ( some additional interval )
}  // namespace

NodeCheckHandler::NodeCheckHandler( ZNetImpl& parent )
: MonitoringBaseHandler( parent )
, NodesProcessor( parent.getOwner(), parent.getOwner(), NodeCheckHandlerTimeout ) {}

void NodeCheckHandler::initialize() {
    ZNetHandler::initialize();

    m_parentMesh.setState( IZNet::Mode::NodeChecking, ModeStatus::Started );

    // init FRT here on RESET! TODO: refactor it!!!!
    utils::forEachBit( getProcessingNodeIds().mask, NodeIdsSet::Size, [this]( uint8_t nodeId ) {
        ZNode* node = m_parentMesh.onFindNode( nodeId );
        if( node != nullptr ) {
            if( node->hasForcedRequests() ) {
                znet_forced_request_timer_create( 0 );
            }
        }
    } );

    startNodesProcessing();
}

void NodeCheckHandler::onDeactivated() {
    stopNodesProcessing();
}

bool NodeCheckHandler::onLoop() {
    if( !isInProgress() ) {
        m_parentMesh.activateMode( IZNet::Mode::Monitoring );
        m_parentMesh.setState( IZNet::Mode::NodeChecking, ModeStatus::Finished );
    }
    return true;
}

bool NodeCheckHandler::canHandleNode( znet_node_id_t nodeId ) const {
    return getCurrentNodeId() == nodeId || nodeId == 0 || nodeId == 0xFF;
}

void NodeCheckHandler::addDataToCheck( uint8_t nodeId, std::vector<std::pair<uint8_t, uint8_t> > data ) {
    m_valuesToCheck.emplace( nodeId, data );
}

bool NodeCheckHandler::processNode( ZNode& currentNode ) {
    m_parentMesh.getOwner().onNodeCheckStatusUpdated( getCurrentNodeId(), ModeStatus::Started );

    if( currentNode.isWakeup( true ) ) {
        finishNodeProcessing( ModeStatus::Failed );
        return false;
    }

    if( m_valuesToCheck.find( currentNode.getId() ) == m_valuesToCheck.end() ) {
        return znet_node_cmd_no_operation( currentNode.getId() ) == ZNET_ERR_OK;
    }
    else {
        return processCommandClasses();
    }
}

void NodeCheckHandler::onReportWaitingExpired() {
    finishNodeProcessing( ModeStatus::Timeout );
}

void NodeCheckHandler::onNodeNoOperation( const znet_node_id_t nodeId, znet_error_t error ) {
    ZNetHandler::onNodeNoOperation( nodeId, error );
    finishNodeProcessing( ModeStatus::Finished );
}

bool NodeCheckHandler::processCommandClasses( size_t index ) {
    auto node = getCurrentNode();
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find current node info  index = %d\n", __func__, index );
        return false;
    }

    const auto nodeId = node->getId();

    const auto& nodeDataToCheck = m_valuesToCheck[nodeId];
    if( index >= nodeDataToCheck.size() ) {
        finishNodeProcessing( ModeStatus::Finished );
        return false;
    }

    auto channelId = nodeDataToCheck.at( index ).first;
    auto ccId = nodeDataToCheck.at( index ).second;

    auto cc = node->getCommandClass( channelId, ccId );
    if( !cc ) {
        return processCommandClasses( index + 1 );
    }

    RequestValue request;
    request.jobContext = getJobContext();
    request.resultCallback = [=]( RequestFailTypeOpt failOpt ) {
        if( nodeId != getCurrentNodeId() ) {
            ZNET_LOGE( "Something went wrong, report received for unexpected node. Actual node id: %u, expected %u\n",
                       getCurrentNodeId(),
                       nodeId );
            return;
        }

        if( failOpt ) {
            ZNET_LOGE( "NodeCheckHandler, Failed cc processing. CC = %d, ch = %d, fail = %d\n",
                       cc->getId(),
                       channelId,
                       *failOpt );
        }

        if( failOpt && *failOpt == RequestFailType::ReportedFail ) {
            ZNET_LOGE( "Finish values geting due to reported fail\n" );
            finishNodeProcessing( ModeStatus::Failed );
        }
        else {
            if( !processCommandClasses( index + 1 ) ) {
                onNodeProcessed( {} );
            }
        }
    };

    if( cc->getId() == ZNET_COMMAND_CLASS_USER_CODE ) {
        // Request all pin codes during hub.device.check
        node->getCommandClass<znet::ZCc0x63UserCode>( channelId )->setUnsetPinCodesCountToStop( 0 );
    }

    if( cc->canRequest() && cc->requestValue( request ) ) {
        updateTimePoint();  // TODO: do we need it? Remove if possible. See CurrentValuesHandlerTimeout
        return true;
    }
    else {
        return processCommandClasses( index + 1 );
    }
}

void NodeCheckHandler::updateReachableState( ZNode& node, bool forceNotify ) {
    MonitoringBaseHandler::updateReachableState( node );
    updateTimePoint();  // TODO: do we need it? Remove if possible. See CurrentValuesHandlerTimeout
}

void NodeCheckHandler::finishNodeProcessing( ModeStatus status ) {
    if( m_valuesToCheck.find( getCurrentNodeId() ) != m_valuesToCheck.end() ) {
        m_valuesToCheck.erase( getCurrentNodeId() );
    }

    m_parentMesh.setState( IZNet::Mode::NodeChecking, status );
    m_parentMesh.getOwner().onNodeCheckStatusUpdated( getCurrentNodeId(), status );
    onNodeProcessed( {} );
}

}  // namespace znet
