#include "znet_discover_handler.hpp"
#include "znet_impl.hpp"
#include "znet/znet.h"
#include "znet/iznet_node_setting.hpp"
#include "znet/cc_0x85_association.hpp"
#include "znet/cc_0x84_wakeup.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

DiscoverHandler::DiscoverHandler( ZNetImpl& parent )
: InterviewHandlerBase( parent )
, NodesProcessor( parent.getOwner(), parent.getOwner(), std::chrono::minutes( 2 ) ) {}

void DiscoverHandler::initialize() {
    InterviewHandlerBase::initialize();

    m_parentMesh.setState( IZNet::Mode::Rediscover, ModeStatus::Started );

    startNodesProcessing();
}

void DiscoverHandler::finalize( ErrorOpt err ) {
    onNodeProcessed( std::move( err ) );
}

void DiscoverHandler::onDeactivated() {
    stopNodesProcessing();
}

bool DiscoverHandler::onLoop() {
    InterviewHandlerBase::onLoop();

    if( !isInProgress() ) {
        m_parentMesh.setState( IZNet::Mode::Rediscover, ModeStatus::Finished );
        m_parentMesh.activateMode( IZNet::Mode::Monitoring );
    }
    return true;
}

void DiscoverHandler::setupNodeSetting( znet_node_id_t nodeId ) {
    if( auto node = getCurrentNode() ) {
        setupNodeSettingImpl(
            nodeId, true /*additional associations*/, node->needExtendedRediscover() /*additional configurations*/ );
    }
}

bool DiscoverHandler::canHandleNode( znet_node_id_t nodeId ) const {
    return m_nodeId && m_nodeId == nodeId;
}

void DiscoverHandler::sendInclusionProgress( const Step interviewStep ) {
    if( auto progress = getProgressByStep( interviewStep ) ) {
        m_parentMesh.updateRediscoverProgress( getCurrentNodeId(), m_nodeId != 0, {}, *progress );
    }
}

bool DiscoverHandler::processNode( ZNode& currentNode ) {
    if( !requestNodeInfo( currentNode ) ) {
        return false;
    }

    currentNode.onRediscoverStarted();
    m_nodeId = currentNode.getId();

    sendInclusionProgress( Step::Started );
    m_step = Step::GetNodeInfo;

    return true;
}

void DiscoverHandler::onNodeProcessed( ErrorOpt errorMessage ) {
    if( auto node = getCurrentNode() ) {
        node->onRediscoverFinished();
        // TODO: call sendInclusionProgress
        m_parentMesh.updateRediscoverProgress( node->getId(), m_step == Step::Finished, errorMessage, 100 );
    }

    m_step = Step::Ready;
    m_nodeId = 0;

    NodesProcessor::onNodeProcessed( std::move( errorMessage ) );
}

WeakJobContext DiscoverHandler::getJobContext() const {
    return NodesProcessor::getJobContext();
}

bool DiscoverHandler::requestNodeInfo( ZNode& currentNode ) {
    ZNET_LOGI( "DiscoverHandler::requestNodeInfo\n" );
    return m_parentMesh.getRequestsManager().doRequest(
        NodeInfoRequest( getCurrentNodeId(), SecurityMode::toByte( currentNode.getSecurity() ) ),
        getJobContext(),
        [this]( const znet_nodeinfo_t& nodeInfo ) {
            assert( m_step == Step::GetNodeInfo );
            m_nodeId = nodeInfo.node_id;

            auto node = m_parentMesh.onFindNode( m_nodeId );
            if( !node ) {
                ZNET_LOGE( "node not found\n" );
                finalize( err::interview::message(
                    ZNET_ERR_NODE_NOT_FOUND, m_nodeId, utils::toUnderlyingType( Step::GetNodeInfo ) ) );
                return;
            }

            node->setNodeInfoExtTypes( nodeInfo.generic, nodeInfo.basic, nodeInfo.specific );
            node->setIsSmartStart( storeSmartStartData( nodeInfo, StoreSmartStartDataMode::All ) );
            node->setNodeInfoExtTypes( nodeInfo.generic, nodeInfo.basic, nodeInfo.specific );
            node->setSecurity( SecurityMode::fromByte( nodeInfo.security ) );

            node->addCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_BASIC );
            node->addCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_CONFIGURATION );

            for( size_t ccIndex = 0; ccIndex < nodeInfo.commands_count; ++ccIndex ) {
                if( nodeInfo.commands[ccIndex] == ZNET_COMMAND_CLASS_MARK )
                    break;

                node->addCommandClass( ZNET_CHANNEL_ID_ROOT, nodeInfo.commands[ccIndex] );
            }

            updateInitialWokenUpState( *node );

            if( ( nodeInfo.security & ZNET_NODEINFO_SECURITY_MASK ) == ZNET_NODEINFO_SECURITY_NONE ) {
                executeOneStep( Step::GetCommandClassVersion, m_nodeId );
            }
            else {
                executeOneStep( Step::WaitSecurity, m_nodeId );
            }
        },
        [this]( RequestFailType failType ) { onNodeProcessed( err::message( ZNET_ERR_DISCOVER_REQ, failType ) ); } );
}

}  // namespace znet
