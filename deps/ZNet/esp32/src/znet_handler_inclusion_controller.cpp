
#include <znet_handler_inclusion_controller.hpp>
#include <znet_impl.hpp>
#include <znet/znet.h>

extern "C" {
#include <znet_log.h>
}

namespace znet {

const uint64_t InclusionHandler::WaitingInitiateMs = 10000;
const uint64_t InclusionHandler::WaitingDeviceInfoMs = 15000;
const uint64_t InclusionHandler::WaitingSecurityMs = 480000;
const uint64_t InclusionHandler::WaitingNodeUpdateMs = 1000;
const uint64_t InclusionHandler::WaitingSecuritySupportedMs = 20000;
const uint64_t InclusionHandler::StopWaiting = 0;
const uint64_t InclusionHandler::WaitingInterviewFinishedMs = 60000;

InclusionHandler::InclusionHandler( ZNetImpl& parent )
: InterviewHandler( parent )
, m_nodeBasic( 0 ) {}

void InclusionHandler::initialize() {
    InterviewHandler::initialize();
    m_inclusionStep = InclusionStep::Ready;
}

bool InclusionHandler::onLoop() {
    if( m_timeOut > std::chrono::system_clock::from_time_t( 0 ) ) {
        auto currentTime = std::chrono::system_clock::now();
        if( currentTime > m_timeOut ) {
            setTimeout( StopWaiting );
            switch( m_inclusionStep ) {
            case InclusionStep::WaitingInitiate:
                ZNET_LOGI( "Initiate Command was not received, starting the unsecured interview\n" );
                setTimeout( WaitingDeviceInfoMs );
                m_inclusionStep = InclusionStep::WaitingNodeInfoToStartUnsecuredInterview;
                if(::znet_node_request_info( m_nodeId, 0 ) != ZNET_ERR_OK ) {
                    m_parentMesh.setState( IZNet::Mode::InclusionControllerStarting,
                                           ModeStatus::Failed,
                                           err::message( ZNET_ERR_NIF_REQ, m_nodeId ) );
                    m_parentMesh.activateMode( IZNet::Mode::Monitoring );
                    return false;
                }
                break;
            case InclusionStep::NodeInfoWaiting:
            case InclusionStep::WaitingNodeInfoToStartS0Bootstrapping:
            case InclusionStep::WaitingS0InclutionComplete:
            case InclusionStep::WaitingSecurity:
            case InclusionStep::WaitingSecuritySupported:
            case InclusionStep::WaitingNodeUpdate:
                ZNET_LOGW( "timeout for getting of a device's info from an inclusion controller\n" );
                znet_node_inclusion_complete( znet_get_ic_info(),
                                              ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION,
                                              ZNET_INCLUSION_CONTROLLER_STEP_FAILED );
            case InclusionStep::WaitingNodeInfoToStartUnsecuredInterview:
                ZNET_LOGW( "Timeout for getting NIF of a device added by an inclusion controller\n" );
                m_parentMesh.setState( IZNet::Mode::InclusionControllerStarting,
                                       ModeStatus::Failed,
                                       err::message( ZNET_ERR_NIF_TIME, m_nodeId ) );
                m_parentMesh.activateMode( IZNet::Mode::Monitoring );
                return false;

            default:
                break;
            }
        }
    }

    return InterviewHandler::onLoop();
}

bool InclusionHandler::canHandleNode( znet_node_id_t nodeId ) const {
    return true;
}

void InclusionHandler::onNodeAdd( const znet_nodeinfo_t* node_info ) {
    if( InclusionStep::Ready == m_inclusionStep ) {
        ZNode* node = m_parentMesh.getOwner().onFindNode( node_info->node_id );
        if( !node ) {
            m_parentMesh.getOwner().nodeCreate( node_info->node_id );
            node = m_parentMesh.getOwner().onFindNode( node_info->node_id );
            assert( node );
            node->setNodeInfoExtTypes( node_info->generic, node_info->basic, node_info->specific );
        }
        m_nodeId = node_info->node_id;
        setTimeout( WaitingInitiateMs );
        m_inclusionStep = InclusionStep::WaitingInitiate;
    }
}

void InclusionHandler::onApplicationNodeUpdate( const znet_nodeinfo_t* nodeInfo ) {
    switch( m_inclusionStep ) {
    case InclusionStep::WaitingNodeInfoToStartUnsecuredInterview:
    case InclusionStep::NodeInfoWaiting: {
        setTimeout( StopWaiting );

        if( nodeInfo == nullptr ) {
            m_parentMesh.setState( IZNet::Mode::InclusionControllerStarting,
                                   ModeStatus::Failed,
                                   err::message( ZNET_ERR_NIF_INVAL, m_nodeId ) );
            m_parentMesh.activateMode( IZNet::Mode::Monitoring );
            return;
        }

        m_nodeId = nodeInfo->node_id;

        ZNode* node = m_parentMesh.onFindNode( m_nodeId );
        m_parentMesh.setState( IZNet::Mode::IncludeStarting, ModeStatus::Started );
        if( !node ) {
            ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, m_nodeId );
            return;
        }

        node->setNodeInfoExtTypes( nodeInfo->generic, nodeInfo->basic, nodeInfo->specific );
        node->setSecurity( SecurityMode::fromByte( nodeInfo->security ) );
        node->setSecurityClassStarted( SecurityMode::fromByte( nodeInfo->security ) );

        node->addCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_BASIC );
        node->addCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_CONFIGURATION );

        for( size_t ccIndex = 0; ccIndex < nodeInfo->commands_count; ++ccIndex ) {
            if( nodeInfo->commands[ccIndex] != ZNET_COMMAND_CLASS_MARK )
                break;

            node->addCommandClass( ZNET_CHANNEL_ID_ROOT, nodeInfo->commands[ccIndex] );
        }

        if( m_inclusionStep == InclusionStep::WaitingNodeInfoToStartUnsecuredInterview ||
            ( nodeInfo->security & ZNET_NODEINFO_SECURITY_MASK ) == ZNET_NODEINFO_SECURITY_NONE ) {
            // step 11 then Atom is SIS
            setTimeout( WaitingInterviewFinishedMs );
            m_inclusionStep = InclusionStep::WaitingInterviewFinished;
            node->setSecurity( SecurityMode::Class::None );
            executeOneStep( Step::GetCommandClassVersion, m_nodeId );
        }
        else if( ( nodeInfo->security & ZNET_NODEINFO_SECURITY_MASK ) == ZNET_NODEINFO_SECURITY_S0 ) {
            m_inclusionStep = InclusionStep::WaitingS0InclutionComplete;
            setTimeout( WaitingInterviewFinishedMs );
            znet_node_inclusion_initiate(
                znet_get_ic_info(), nodeInfo->node_id, ZNET_INCLUSION_CONTROLLER_S0_INCLUSION );
        }
        else if( ( nodeInfo->security & ZNET_NODEINFO_SECURITY_MASK ) == ZNET_NODEINFO_SECURITY_S2_UNAUTH ||
                 ( nodeInfo->security & ZNET_NODEINFO_SECURITY_MASK ) == ZNET_NODEINFO_SECURITY_S2_AUTH ||
                 ( nodeInfo->security & ZNET_NODEINFO_SECURITY_MASK ) == ZNET_NODEINFO_SECURITY_S2_ACCESS ) {
            m_inclusionStep = InclusionStep::WaitingSecurity;
            setTimeout( WaitingSecurityMs );
            // step 5 then Atom is SIS
            znet_node_start_S2( m_nodeId );
        }
        break;
    }
    case InclusionStep::WaitingSecurity:
    case InclusionStep::WaitingNodeUpdate:
        setTimeout( WaitingSecuritySupportedMs );
        m_inclusionStep = InclusionStep::WaitingSecuritySupported;
        executeOneStep( Step::WaitSecurity, m_nodeId );
        znet_node_cmd_security_supported_get( nodeInfo->node_id, ZNET_CHANNEL_ID_ROOT );
        break;
    case InclusionStep::WaitingNodeInfoToStartS0Bootstrapping:
        setTimeout( WaitingSecurityMs );
        m_inclusionStep = InclusionStep::WaitingSecurity;
        ::znet_node_start_S0( nodeInfo->node_id, ZNET_BASIC_CLASS_STATIC_CONTROLLER == nodeInfo->basic );
        break;
    default:
        break;
    }
}

void InclusionHandler::onNodeInclusionControllerInitiate( znet_node_id_t nodeId,
                                                          const znet_node_id_t childNodeId,
                                                          const znet_inclution_controller_step_id_t stepId ) {
    if(::znet_get_sis_node_id() != ::znet_get_self_node_id() ) {
        ZNET_LOGW( "Initiate Command is received, but we are not a SIS in the current network\n" );
        return;
    }
    if( !m_parentMesh.getOwner().onFindNode( childNodeId ) ) {
        m_parentMesh.getOwner().nodeCreate( childNodeId );
    }
    m_nodeId = childNodeId;
    switch( stepId ) {
    case ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION_REPLACE:
        m_parentMesh.activateMode( IZNet::Mode::InclusionControllerStartingReplace );
    case ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION:
        setTimeout( WaitingDeviceInfoMs );
        m_inclusionStep = InclusionStep::NodeInfoWaiting;
        if( ZNET_ERR_OK != ::znet_node_request_info( childNodeId, 0 ) ) {
            m_parentMesh.setState( IZNet::Mode::InclusionControllerStarting,
                                   ModeStatus::Failed,
                                   err::message( ZNET_ERR_NIF_REQ, childNodeId ) );
            m_parentMesh.activateMode( IZNet::Mode::Monitoring );
        }
    }
}

void InclusionHandler::onNodeInclusionControllerComplete( znet_node_id_t nodeId,
                                                          const znet_inclution_controller_step_id_t stepId,
                                                          const znet_inclution_controller_status_t status ) {
    if( m_inclusionStep == InclusionStep::WaitingS0InclutionComplete ) {
        if( stepId == ZNET_INCLUSION_CONTROLLER_S0_INCLUSION ) {
            if( ZNET_INCLUSION_CONTROLLER_STEP_NOT_SUPPORTED == status ) {
                ZNET_LOGI( "The inclusion controller does not support S0_INCLUSION step, thus we gonna perform S0 "
                           "bootstrapping on our side\n" );
                setTimeout( WaitingDeviceInfoMs );
                m_inclusionStep = InclusionStep::WaitingNodeInfoToStartS0Bootstrapping;
                if( ZNET_ERR_OK != ::znet_node_request_info( m_nodeId, 0 ) ) {
                    m_parentMesh.setState( IZNet::Mode::InclusionControllerStarting,
                                           ModeStatus::Failed,
                                           err::message( ZNET_ERR_NIF_REQ, m_nodeId ) );
                    m_parentMesh.activateMode( IZNet::Mode::Monitoring );
                }
            }
            else {
                setTimeout( WaitingNodeUpdateMs );
                m_inclusionStep = InclusionStep::WaitingNodeUpdate;
                znet_node_request_info( m_nodeId, ZNET_NODEINFO_SECURITY_S0 );
            }
        }
    }
}

void InclusionHandler::onNodeCmdSecuritySupported( const uint8_t nodeId,
                                                   const znet_security_supported_report_t* nodeInfoSecuritySupported ) {
    if( m_inclusionStep == InclusionStep::WaitingSecuritySupported ) {
        InterviewHandler::onNodeCmdSecuritySupported( nodeId, nodeInfoSecuritySupported );

        setTimeout( WaitingInterviewFinishedMs );
        m_inclusionStep = InclusionStep::WaitingInterviewFinished;
    }
}

void InclusionHandler::executeOneStep( const Step interviewStep, const uint8_t nodeId ) {
    if( Step::Finished == interviewStep && ::znet_get_ic_info().id != ZNET_NODE_ID_ANY ) {
        znet_node_inclusion_complete(
            znet_get_ic_info(), ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION, ZNET_INCLUSION_CONTROLLER_STEP_OK );
    }
    InterviewHandlerBase::executeOneStep( interviewStep, nodeId );
}

}  // namespace znet
