
#include <znet_handler_learn.hpp>
#include <znet_impl.hpp>
#include <znet/znet.h>
#include <znet/znet_utils.hpp>
extern "C" {
#include <znet_log.h>
}

namespace znet {

const uint64_t LearnHandler::WaitingSecurityMs = 700000;
const uint64_t LearnHandler::WaitingFinishInterviewMs = 20000;
const uint64_t LearnHandler::WaitingAssociationMs = 10000;
const uint64_t LearnHandler::WaitingDeviceInfoMs = 15000;
const uint64_t LearnHandler::StopWaiting = 0;

LearnHandler::LearnHandler( ZNetImpl& parent )
: InterviewHandler( parent )
, m_nodeCounter( 0 ) {}

void LearnHandler::initialize() {
    InterviewHandler::initialize();
    m_learnStep = LearnStep::Ready;
    setTimeout( StopWaiting );
}

bool LearnHandler::onLoop() {
    if( m_timeOut > std::chrono::system_clock::from_time_t( 0 ) ) {
        auto currentTime = std::chrono::system_clock::now();
        if( currentTime > m_timeOut ) {
            setTimeout( StopWaiting );
            ZNET_LOGW( "timeout for " );
            switch( m_learnStep ) {
            case LearnStep::AssosiationSetFromSisWaiting:
                ZNET_LOGW( "setting of a association from SIS\n" );
                setTimeout( WaitingAssociationMs );
                znet_node_learn_nodes_request();
                m_learnStep = LearnStep::ListOfNodesSisWaiting;
                break;

            case LearnStep::ListOfNodesSisWaiting:
                ZNET_LOGW( "getting of a list of devices from SIS\n" );
                startDeviceInterview();
                break;

            case LearnStep::NodeInfoWaiting:
                ZNET_LOGW( "getting of a device's info from SIS\n" );
                ++m_nodeId;
                startDeviceInterview();
                break;

            default:
                ZNET_LOGW( "unknown step" );
                break;
            }
        }
    }

    return InterviewHandler::onLoop();
}

void LearnHandler::onNodeAdd( const znet_nodeinfo_t* nodeInfo ) {
    if( nodeInfo == nullptr ) {
        ZNET_LOGE( "wrong node info\n" );
        return;
    }

    if( m_nodeId != 0 && nodeInfo->node_id != m_nodeId ) {
        ZNET_LOGW( "%s node_id=%u, expected node_id=%u\n", __func__, nodeInfo->node_id, m_nodeId );
        return;
    }

    if( m_step != Step::Ready && m_step != Step::WaitSecurity && m_step != Step::WaitInclusionController ) {
        ZNET_LOGE( "%s: wrong step(%d)\n", __func__, m_step );
        return;
    }

    m_nodeId = nodeInfo->node_id;

    ZNode* node = m_parentMesh.onFindNode( m_nodeId );
    if( !node ) {
        m_parentMesh.setState( IZNet::Mode::IncludeStarting, ModeStatus::Started );

        IZNetOwner& owner = m_parentMesh.getOwner();
        owner.nodeCreate( m_nodeId /*, nodeInfo->generic, nodeInfo->basic, nodeInfo->specific*/ );
        node = m_parentMesh.onFindNode( m_nodeId );
        if( node == nullptr ) {
            ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, m_nodeId );
            return;
        }
        node->setNodeInfoExtTypes( nodeInfo->generic, nodeInfo->basic, nodeInfo->specific );
        node->setSecurityClassStarted( SecurityMode::fromByte( nodeInfo->security ) );
        node->setIsSmartStart( storeSmartStartData( *nodeInfo, StoreSmartStartDataMode::NodeId ) );
    }
    else if( node->getSecurityClassStarted() == SecurityMode::Class::None ) {
        m_nodeId = 0;
        finalize( err::message( ZNET_ERR_NODE_EXISTS, m_nodeId ) );
        return;
    }

    node->setNodeInfoExtTypes( nodeInfo->generic, nodeInfo->basic, nodeInfo->specific );
    node->setSecurity( SecurityMode::fromByte( nodeInfo->security ) );

    node->addCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_BASIC );
    node->addCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_CONFIGURATION );

    for( size_t ccIndex = 0; ccIndex < nodeInfo->commands_count; ++ccIndex ) {
        if( nodeInfo->commands[ccIndex] == ZNET_COMMAND_CLASS_MARK )
            break;

        node->addCommandClass( ZNET_CHANNEL_ID_ROOT, nodeInfo->commands[ccIndex] );
    }

    if( nodeInfo->security != ZNET_NODEINFO_SECURITY_NONE ) {
        executeOneStep( Step::WaitSecurity, m_nodeId );
        znet_node_cmd_security_supported_get( nodeInfo->node_id, ZNET_CHANNEL_ID_ROOT );
    }
    else {
        executeOneStep( Step::GetCommandClassVersion, m_nodeId );
    }
}

void LearnHandler::onNodeLearn( const znet_node_id_t destNodeId ) {
    if( m_learnStep != LearnStep::Ready ) {
        if( m_learnStep == LearnStep::AssosiationSetFromSisWaiting ) {
            setTimeout( WaitingFinishInterviewMs );
            return;
        }
        ZNET_LOGE( "wrong learn mode step(%d)\n", m_learnStep );
        return;
    }

    if( destNodeId == 0 ) {
        m_parentMesh.setState( IZNet::Mode::LearnStarting, ModeStatus::Finished );
        m_nodeId = 0;
        finalize( {} );
        m_parentMesh.resetZwaveModule( false );
        return;
    }

    m_nodeCounter = 0;
    m_devicesToLearnAmount = 0;
    m_destNodeId = destNodeId;
    m_parentMesh.setState( IZNet::Mode::LearnStarting, ModeStatus::Started );

    if( destNodeId == znet_get_self_node_id() ) {
        ZNET_LOGI( "%s replication\n", __func__ );

        setTimeout( WaitingAssociationMs );
        znet_node_learn_nodes_request();
        m_learnStep = LearnStep::ListOfNodesSisWaiting;
        return;
    }

    // TODO START WAITING of Association set from SIS  (  learn_ok() -> znet_node_request_info( m_nodeId );  ->
    // onApplicationNodeUpdate )

    m_learnStep = LearnStep::AssosiationSetFromSisWaiting;
    setTimeout( WaitingSecurityMs );
}

void LearnHandler::onNodeList( const uint8_t* nodesList ) {
    setTimeout( StopWaiting );

    if( nodesList == nullptr ) {
        ZNET_LOGE( "wrong nodesList\n" );
        return;
    }
    std::memcpy( m_nodesList, nodesList, ZNET_NODE_ID_MAX / 8 );
    m_nodeId = ZNET_NODE_ID_MIN;
    m_devicesToLearnAmount = znet::utils::getBitsCount( m_nodesList, ZNET_NODE_ID_MAX / 8 );
    startDeviceInterview();
}

void LearnHandler::onApplicationNodeUpdate( const znet_nodeinfo_t* nodeInfo ) {
    setTimeout( StopWaiting );

    m_learnStep = LearnStep::Ready;
    if( nodeInfo == nullptr ) {
        ZNET_LOGE( "wrong node info\n" );
        m_parentMesh.setState( IZNet::Mode::LearnStarting, ModeStatus::Finished );
        return;
    }

    onNodeAdd( nodeInfo );
}

void LearnHandler::onNodeCmdSecuritySupported( const uint8_t nodeId,
                                               const znet_security_supported_report_t* nodeInfoSecuritySupported ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    node->setSecurityClassStarted( node->getSecurity() );
    InterviewHandler::onNodeCmdSecuritySupported( nodeId, nodeInfoSecuritySupported );
}

bool LearnHandler::startDeviceInterview() {
    m_step = Step::Ready;
    IZNetOwner& owner = m_parentMesh.getOwner();
    while( m_nodeId <= ZNET_NODE_ID_MAX ) {
        if( m_nodeId == znet_get_self_node_id() ) {
            m_parentMesh.updateLearnProgress( m_devicesToLearnAmount, ++m_nodeCounter );
            m_nodeId++;
            continue;
        }
        ZNode* node = m_parentMesh.onFindNode( m_nodeId );
        bool nodeIsOnNetwork = znet::utils::checkBit( m_nodesList, m_nodeId - 1 );
        if( ( node == nullptr ) && nodeIsOnNetwork ) {
            owner.nodeCreate( m_nodeId );
            node = m_parentMesh.onFindNode( m_nodeId );
            if( !node ) {
                ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, m_nodeId );
                m_nodeId++;
                continue;
            }
            node->setSecurityClassStarted( SecurityMode::Class::Unknown );
            if( ZNET_ERR_OK != znet_node_request_info( m_nodeId, 0 ) ) {
                ZNET_LOGE( "%s could not request node info  Id = %d\n", __func__, m_nodeId );
                m_nodeId++;
                continue;
            }
            ZNET_LOGI( "%s new node %u\n", __func__, m_nodeId );

            m_learnStep = LearnStep::NodeInfoWaiting;
            setTimeout( WaitingDeviceInfoMs );

            m_parentMesh.updateLearnProgress( m_devicesToLearnAmount, ++m_nodeCounter );
            updateJobContext();
            updateInterviewTimeStamp();

            return true;
        }
        if( ( node != nullptr ) && nodeIsOnNetwork ) {
            ZNET_LOGI( "%s node exist %u\n", __func__, m_nodeId );
            m_parentMesh.updateLearnProgress( m_devicesToLearnAmount, ++m_nodeCounter );
        }
        if( ( node != nullptr ) && !nodeIsOnNetwork ) {
            ZNET_LOGI( "%s remove node %u\n", __func__, m_nodeId );

            owner.nodeRemove( m_nodeId );
        }
        m_nodeId++;
    }
    ZNET_LOGI( "Learn node successfully finished\n" );
    m_parentMesh.setState( IZNet::Mode::LearnStarting, ModeStatus::Finished );

    clearJobContext();
    m_parentMesh.activateMode( IZNet::Mode::Monitoring );
    m_nodeId = 0;

    return false;
}

void LearnHandler::onNodeFailed( uint8_t nodeId, int error ) {
    m_parentMesh.setState( IZNet::Mode::LearnStarting, ModeStatus::Failed, err::message( error ) );
    setTimeout( StopWaiting );
    m_parentMesh.activateMode( IZNet::Mode::Monitoring );
    clearJobContext();
}

void LearnHandler::executeOneStep( Step step, const znet_node_id_t nodeId ) {
    setTimeout( StopWaiting );

    switch( step ) {
    case Step::Association:
    case Step::AssociationGroupInfo:
    case Step::MultichannelAssociation:
        if( znet_get_sis_node_id() == nodeId ) {
            if( m_step == Step::GetManufactureSpecific ) {
                ZNET_LOGD( "next CommandClassesProcessing\n" );
                step = Step::CommandClassesProcessing;
            }
            else {
                ZNET_LOGD( "skip Association\n" );
                step = Step::GetManufactureSpecific;
            }
        }
        break;

    default:
        break;
    }

    InterviewHandler::executeOneStep( step, nodeId );
}

void LearnHandler::finalize( ErrorOpt err ) {
    if( err ) {
        ZNET_LOGE( "%s\n", utils::stringify( *err ).c_str() );
    }
    if( m_nodeId > 0 ) {
        m_parentMesh.getOwner().interviewDone( m_nodeId );

        if( m_nodeId <= ZNET_NODE_ID_MAX ) {
            m_nodeId++;
            if( startDeviceInterview() )
                return;
        }
    }

    clearJobContext();
    m_parentMesh.activateMode( IZNet::Mode::Monitoring );
    m_nodeId = 0;
}

void LearnHandler::onAssociationSetResult( const uint8_t nodeId, const uint8_t channelId ) {
    if( m_step == Step::Association ) {
        InterviewHandler::onAssociationSetResult( nodeId, channelId );
    }
    else {
        ZNET_LOGI( "%s LEARN (node id: %hhu)\n", __func__, nodeId );
        m_nodeId = nodeId;
        znet_node_learn_nodes_request();
    }
}

void LearnHandler::onMultichannelAssociationSetResult( const uint8_t nodeId, const uint8_t channelId ) {
    if( m_step == Step::MultichannelAssociation ) {
        InterviewHandler::onMultichannelAssociationSetResult( nodeId, channelId );
    }
    else {
        ZNET_LOGI( "%s LEARN (node id: %hhu)\n", __func__, nodeId );
        m_nodeId = nodeId;
        znet_node_learn_nodes_request();
    }
}

void LearnHandler::onZwavePlusInfoGet() {
    setTimeout( WaitingAssociationMs );
}

bool LearnHandler::canHandleNode( znet_node_id_t nodeId ) const {
    return true;
}

}  // namespace znet
