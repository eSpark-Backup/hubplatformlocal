#include "znet_handler_interview.hpp"

#include "znet_impl.hpp"
#include "znet/cc_0x84_wakeup.hpp"
#include "znet/cc_0x80_battery.hpp"
#include "znet/cc_0x85_association.hpp"
#include "znet/cc_0x70_configuration.hpp"
#include "znet/iznet_node_setting.hpp"
#include "znet/znet_provisioning_list_query.hpp"
#include "znet/converters/security_modes_converter.hpp"

extern "C" {
#include "znet_log.h"
}

namespace fields = ::znet::api::filters::fields;

namespace znet {

namespace {
class JobContextImpl : public JobContext {
public:
    bool isExpired() const override {
        return false;
    }
};
}  // namespace

void InterviewHandler::onNodeAdd( const znet_nodeinfo_t* nodeInfo ) {
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
        node->setCapability( nodeInfo->capability );
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

    uint8_t sis_id = znet_get_sis_node_id();
    uint8_t my_node_id = znet_get_self_node_id();
    if( sis_id != ZNET_NODE_ID_ANY && sis_id != my_node_id && sis_id != m_nodeId ) {
        // step 2 then Atom is IC

        if( znet_node_is_have_command( sis_id, ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_INCLUSION_CONTROLLER ) ) {
            inclusion_controller_info_t sis_info = {
                sis_id, znet_utils_get_security_encap_type_by_node_id( znet_get_self_node_id() )
            };
            znet_node_inclusion_initiate( sis_info, m_nodeId, ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION );
            executeOneStep( Step::WaitPrimaryController, m_nodeId );
        }
        else {
            if( ( nodeInfo->security & ZNET_NODEINFO_SECURITY_MASK ) == ZNET_NODEINFO_SECURITY_S0 ) {
                if( m_step != Step::WaitSecurity ) {
                    executeOneStep( Step::WaitSecurity, m_nodeId );
                    ::znet_node_start_S0( m_nodeId, ZNET_BASIC_CLASS_STATIC_CONTROLLER == nodeInfo->basic );
                }
            }
            else {
                node->setSecurity( SecurityMode::Class::None );
                executeOneStep( Step::GetCommandClassVersion, m_nodeId );
            }
        }
    }
    else {
        if( ( nodeInfo->security & ZNET_NODEINFO_SECURITY_MASK ) == ZNET_NODEINFO_SECURITY_NONE ) {
            updateInitialWokenUpState( *node );
            executeOneStep( Step::GetCommandClassVersion, m_nodeId );
        }
        else {
            executeOneStep( Step::WaitSecurity, m_nodeId );
        }
    }
}

void InterviewHandler::onNodeAddSecurityFailed() {
    if( ZNode* node = m_parentMesh.onFindNode( m_nodeId ) ) {
        if( node->isSmartStart() ) {
            m_parentMesh.resetDeviceSecurityData( m_nodeId, ProvisioningListEntryFields::Status::FAILED );
            m_parentMesh.getOwner().nodeRemove( m_nodeId );
            auto errorMessage = err::message( ZNET_ERR_SMART_SECURITY, m_nodeId );
            m_nodeId = 0;
            finalize( std::move( errorMessage ) );
        }
        else {
            node->setSecurity( SecurityMode::Class::None );
            executeOneStep( Step::GetCommandClassVersion, m_nodeId );
        }
    }
}

void InterviewHandler::onNodeAddSecurityKeysRequested( znet_S2_keys_t supported_keys ) {
    using fields_t = ::znet::ProvisioningListEntryFields;
    ::znet::api::ProvisioningListQuery{ m_parentMesh.getClassicInclusionSecurityDataSource() }
        .createIfNotExist()
        .setField( &fields_t::m_node, m_nodeId )
        .setField( &fields_t::m_supportedModes, supported_keys )
        .execute();
}

void InterviewHandler::onNodeAddSecurityPubKeyRequested( const znet_S2_pub_key_t* key ) {
    if( key->size == 14 ) {
        DSK dsk = { 0 };
        for( uint8_t i = 0; i < 7; ++i ) {
            dsk[i + 1] = ( (uint16_t)key->key[i * 2] << 8 ) | key->key[i * 2 + 1];
        }
        namespace fields = ::znet::api::filters::fields;
        ::znet::api::ProvisioningListQuery{ m_parentMesh.getClassicInclusionSecurityDataSource() }
            .filterBy( fields::nodeId( m_nodeId ) )
            .setDsk( dsk )
            .execute();
    }
}

void InterviewHandler::onNodeAddSecurityKeysGranted( znet_S2_keys_t selected_keys ) {
    using fields_t = ::znet::ProvisioningListEntryFields;
    ::znet::api::ProvisioningListQuery{ m_parentMesh.getClassicInclusionSecurityDataSource() }
        .filterBy( fields::nodeId( m_nodeId ) )
        .setField( &fields_t::m_securityModes, selected_keys )
        .execute();
}

void InterviewHandler::onNodeAddSecurityPubKeyGranted( const znet_S2_pub_key_t* pubKey ) {
    if( pubKey->size == 2 ) {
        std::array<uint8_t, 2> key{ 0 };
        std::memcpy( key.data(), pubKey->key, key.size() );
        ::znet::api::ProvisioningListQuery{ m_parentMesh.getClassicInclusionSecurityDataSource() }
            .filterBy( fields::nodeId( m_nodeId ) )
            .setDsk( key )
            .execute();
    }
}

void InterviewHandler::onNodeAddSecurityDone( SecurityMode::Class security ) {
    if( ZNode* node = m_parentMesh.onFindNode( m_nodeId ) ) {
        // We can't know which security level is highest available in case if we didn't perform bootstrapping
        if(::znet_get_self_node_id() != ::znet_get_sis_node_id() && security != SecurityMode::Class::S0 ) {
            node->setSecurityClassStarted( security );
        }
        node->setSecurity( security );
        auto& dataSource = node->isSmartStart() ? m_parentMesh.getProvisioningListControllerDataSource() :
                                                  m_parentMesh.getClassicInclusionSecurityDataSource();
        ::znet::api::ProvisioningListQuery{ dataSource }
            .filterBy( fields::nodeId( m_nodeId ) )
            .setField( &::znet::ProvisioningListEntryFields::m_nodeSecurityModes,
                       ::znet::security_modes_converter::typeToBitset( security ) )
            .execute();
    }
}

void InterviewHandler::startInclude( const DskDerivedHomeId& dskDerivedHomeId ) {
    auto errCode = ::znet_node_add_dsk_to_network( dskDerivedHomeId.data() );
    if( ZNET_ERR_OK == errCode ) {
        m_parentMesh.setState( IZNet::Mode::IncludeStarting, ModeStatus::InProgress );
    }
    else {
        m_parentMesh.setState( IZNet::Mode::IncludeStarting, ModeStatus::Failed, err::message( errCode ) );
        m_parentMesh.activateMode(::znet::IZNet::Mode::Monitoring );
    }
}

void InterviewHandler::stopInclude() {
    auto errCode = znet_node_add_break();
    if( ZNET_ERR_OK == errCode ) {
        m_parentMesh.setState( IZNet::Mode::IncludeStopping, ModeStatus::Invoked );
    }
    else {
        m_parentMesh.setState( IZNet::Mode::IncludeStopping, ModeStatus::Failed, err::message( errCode ) );
        m_parentMesh.activateMode( IZNet::Mode::Monitoring );
    }
}

void InterviewHandler::initialize() {
    updateJobContext();
    InterviewHandlerBase::initialize();
}

void InterviewHandler::finalize( ErrorOpt err ) {
    clearJobContext();

    const auto prevStep = m_step;
    m_step = Step::Ready;

    ZNode* node = m_parentMesh.onFindNode( m_nodeId );
    if( node != nullptr ) {
        ZNode::ClassPtr cc = node->getCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_BATTERY );
        if( cc ) {
            ZNode::ClassPtr ccTemp = node->getCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_WAKE_UP );
            if( !ccTemp ) {
                ZCc0x80Battery* cc_battery = static_cast<ZCc0x80Battery*>( cc.get() );
                cc_battery->setForcedRequest( true );
                node->setForcedRequests( true );
                znet_forced_request_timer_create( 0 );
            }
        }
    }
    else if( m_nodeId != 0 && m_nodeId != znet_get_sis_node_id() ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, m_nodeId );
    }

    if( prevStep == Step::Finished ) {
        m_parentMesh.getOwner().interviewDone( m_nodeId );
        m_parentMesh.setState( IZNet::Mode::IncludeStarting, ModeStatus::Finished );
        m_parentMesh.synchronizeNodeTime( m_nodeId );
    }
    else {
        m_parentMesh.setState( IZNet::Mode::IncludeStarting,
                               prevStep == Step::Timeout ? ModeStatus::Timeout : ModeStatus::Failed,
                               std::move( err ) );
        if( m_nodeId != 0 ) {
            m_parentMesh.resetDeviceSecurityData( m_nodeId, ProvisioningListEntryFields::Status::FAILED );
        }
    }

    m_nodeId = 0;

    m_parentMesh.activateMode( IZNet::Mode::Monitoring );
}

void InterviewHandler::setupNodeSetting( znet_node_id_t nodeId ) {
    setupNodeSettingImpl( nodeId, true, true );
}

bool InterviewHandler::canHandleNode( znet_node_id_t nodeId ) const {
    return m_nodeId == nodeId || nodeId == 0 || nodeId == znet_get_sis_node_id() || nodeId == 0xFF;
}

void InterviewHandler::updateJobContext() {
    m_interviewJobcontext = std::make_shared<JobContextImpl>();
}

void InterviewHandler::clearJobContext() {
    m_interviewJobcontext.reset();
}

WeakJobContext InterviewHandler::getJobContext() const {
    return { m_interviewJobcontext };
}

void InterviewHandler::onNodeCmdSecuritySupported( const uint8_t nodeId,
                                                   const znet_security_supported_report_t* value ) {
    InterviewHandlerBase::onNodeCmdSecuritySupported( nodeId, value );

    if( m_step == Step::WaitInclusionController ) {
        znet_node_inclusion_complete(
            { znet_get_sis_node_id(), znet_utils_get_security_encap_type_by_node_id( znet_get_self_node_id() ) },
            ZNET_INCLUSION_CONTROLLER_S0_INCLUSION,
            ZNET_INCLUSION_CONTROLLER_STEP_OK );
    }
}

void InterviewHandler::onNodeInclusionControllerInitiate( znet_node_id_t nodeId,
                                                          const znet_node_id_t childNodeId,
                                                          const znet_inclution_controller_step_id_t stepId ) {
    if( stepId == ZNET_INCLUSION_CONTROLLER_S0_INCLUSION ) {
        // step 8 then Atom is IC and device is S0
        executeOneStep( Step::WaitPrimaryController, nodeId );
        if( znet_if_support_security_level( ZNET_NODEINFO_SECURITY_S0 ) ) {
            uint8_t my_security = znet_utils_get_security_encap_type_by_node_id( znet_get_self_node_id() );
            uint8_t message_security = znet_get_ic_info().security;
            ZNET_LOGI( "%s security: my(0x%X), message(0x%X)\n", __func__, my_security, message_security );
            if( my_security == message_security ) {
                znet_node_start_S0( childNodeId, false );
            }
            else {
                ZNode* node = m_parentMesh.onFindNode( m_nodeId );
                if( node == nullptr ) {
                    ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, m_nodeId );
                    return;
                }
                else {
                    node->setSecurity( SecurityMode::Class::None );
                }
            }
        }
        else {
            znet_node_inclusion_complete( znet_get_ic_info(),
                                          ZNET_INCLUSION_CONTROLLER_S0_INCLUSION,
                                          ZNET_INCLUSION_CONTROLLER_STEP_NOT_SUPPORTED );
        }
    }
}

void InterviewHandler::onNodeInclusionControllerComplete( znet_node_id_t nodeId,
                                                          const znet_inclution_controller_step_id_t stepId,
                                                          const znet_inclution_controller_status_t status ) {
    if( m_step == Step::WaitInclusionController ) {
        ZNode* node = m_parentMesh.onFindNode( m_nodeId );
        if( node == nullptr ) {
            znet_nodeinfo_t node_info;
            node_info.node_id = m_nodeId;

            znet_node_request_info( m_nodeId, znet_get_my_highest_security() );
            node_info.security = znet_get_my_highest_security();
            onNodeAdd( &node_info );
            executeOneStep( Step::WaitSecurity, m_nodeId );
        }
        else {
            executeOneStep( Step::GetCommandClassVersion, m_nodeId );
        }
    }
    if( m_step == Step::WaitPrimaryController ) {
        if( stepId == ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION ||
            stepId == ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION_REPLACE ) {
            ZNode* node = m_parentMesh.onFindNode( m_nodeId );
            if( node == nullptr ) {
                ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, m_nodeId );
                return;
            }
            if( node->getSecurity() == SecurityMode::Class::None ) {
                executeOneStep( Step::GetCommandClassVersion, m_nodeId );
            }
            else {
                znet_node_request_info( m_nodeId, znet_get_my_highest_security() );
                executeOneStep( Step::WaitSecurity, m_nodeId );
            }
        }
        else {
            ZNET_LOGE( "%s: wrong stepId(%d), status = %d\n", __func__, stepId, status );
        }
    }
}

void InterviewHandler::onApplicationNodeUpdate( const znet_nodeinfo_t* nodeInfo ) {
    if( Step::WaitSecurity == m_step && ZNET_NODEINFO_SECURITY_NONE == nodeInfo->security ) {
        executeOneStep( Step::GetCommandClassVersion, m_nodeId );
    }
}

}  // namespace znet
