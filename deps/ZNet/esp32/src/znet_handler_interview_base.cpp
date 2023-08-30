
#include "znet_handler_interview_base.hpp"

#include <znet_impl.hpp>
#include <znet/znet.h>
#include <znet/znet_node.hpp>
#include <znet/iznet_owner.hpp>
#include <znet/cc_0x26_switch_multi_level.hpp>
#include <znet/cc_0x30_binary_sensor.hpp>
#include <znet/cc_0x31_sensor_multilevel.hpp>
#include <znet/cc_0x32_meter.hpp>
#include <znet/cc_0x71_notification.hpp>
#include <znet/cc_0x72_manufacturer_specific.hpp>
#include <znet/cc_0x33_color_switch.hpp>
#include <znet/cc_0x40_thermostat_mode.hpp>
#include <znet/cc_0x42_thermostat_operating_state.hpp>
#include <znet/cc_0x43_thermostat_setpoint.hpp>
#include <znet/cc_0x44_thermostat_fan_mode.hpp>
#include <znet/cc_0x59_association_grp_info.hpp>
#include <znet/cc_0x5b_central_scene.hpp>
#include <znet/cc_0x60_multi_channel.hpp>
#include <znet/cc_0x62_door_lock.hpp>
#include <znet/cc_0x66_barrier_operator.hpp>
#include <znet/cc_0x80_battery.hpp>
#include <znet/cc_0x84_wakeup.hpp>
#include <znet/cc_0x85_association.hpp>
#include <znet/cc_0x86_version.hpp>
#include <znet/cc_0x87_indicator.hpp>
#include <znet/cc_0x8e_multi_channel_association.hpp>
#include <znet/cc_0x9c_alarm_sensor.hpp>
#include <znet/cc_0x63_user_code.hpp>
#include <znet/cc_0x4e_schedule_entry_lock.hpp>
#include <znet/cc_0x79_sound_switch.hpp>
#include <znet/cc_0x70_configuration.hpp>
#include <znet/cc_0x7a_firmware_update.hpp>
#include <znet/znet_provisioning_list_query.hpp>
#include <znet/converters/security_modes_converter.hpp>

extern "C" {
#include <znet_log.h>
}

namespace znet {

namespace fields = api::filters::fields;

const int InterviewHandlerBase::DefaultAssociationGroupId = 1;
const int InterviewHandlerBase::DefaultMinWakeUpInterval = 0;
const int InterviewHandlerBase::SendWaitingTimeoutMs = 500;
const int InterviewHandlerBase::OneStepWaitingTimeoutMs = 12000;
const int InterviewHandlerBase::SecurityWaitingTimeoutMs = 480000;
const int InterviewHandlerBase::InclusionControllerTimeoutMs = 60000;
const int InterviewHandlerBase::PrimaryControllerTimeoutMs = 480000;
const int InterviewHandlerBase::InterviewTimeoutMs = 5 * 60 * 1000;

InterviewHandlerBase::InterviewHandlerBase( ZNetImpl& parent )
: ZNetHandler( parent )
, m_step( Step::Ready ) {}

void InterviewHandlerBase::initialize() {
    ZNetHandler::initialize();
    m_step = Step::Ready;
    m_nodeId = 0;
    updateInterviewTimeStamp();
}

bool InterviewHandlerBase::onLoop() {
    const auto waitingTime = getMaxWaitingTime( m_step );
    if( m_interviewTimeStamp + waitingTime > std::chrono::system_clock::now() ) {
        return false;
    }
    ZNET_LOGW( "timeout for " );
    switch( m_step ) {
    case Step::GetCommandClassVersion:
        ZNET_LOGW( "getting of version of command class. After refactoring via requests manager in "
                   "shouldn't be reproduced\n" );
        executeOneStep( Step::WakeUpCapabilities, m_nodeId );
        break;

    case Step::WakeUpCapabilities:
        ZNET_LOGW( "getting of wakeup capabilities\n" );
        executeOneStep( Step::MultichannelAssociation, m_nodeId );
        break;

    case Step::MultichannelAssociation:
        ZNET_LOGW( "multi channel association on step\n" );
        executeOneStep( Step::Association, m_nodeId );
        break;

    case Step::Association:
        ZNET_LOGW( "association on step\n" );
        executeOneStep( Step::GetManufactureSpecific, m_nodeId );
        break;

    case Step::AssociationGroupInfo:
        ZNET_LOGW( "association group info on step\n" );
        executeOneStep( Step::MultichannelEndpointSurvey, m_nodeId );
        break;

    case Step::MultichannelEndpointSurvey:
        ZNET_LOGW( "getting of a multichannel endpoint \n" );
        executeOneStep( Step::CommandClassesProcessing, m_nodeId );
        break;

    case Step::GetManufactureSpecific:
        ZNET_LOGW( "getting of a manufacture specific \n" );
        executeOneStep( Step::AssociationGroupInfo, m_nodeId );
        break;

    case Step::GetNodeInfo:
        ZNET_LOGW( "getting node info\n" );
        finalize( err::interview::message( ZNET_ERR_NIF_TIME, m_nodeId, utils::toUnderlyingType( m_step ) ) );
        break;

        //  TODO   add other steps
    case Step::SetupNodeSetting:
        ZNET_LOGW( "setup node setting\n" );
        executeOneStep( Step::Finished, m_nodeId );
        break;
    case Step::WaitInclusionController:
        ZNET_LOGW( "a waiting Inclusion Controller \n" );
        executeOneStep( Step::GetCommandClassVersion, m_nodeId );
        break;
    case Step::WaitSecurity:
        ZNET_LOGW( "a waiting security \n" );
        executeOneStep( Step::GetCommandClassVersion, m_nodeId );
        break;

    case Step::CommandClassesProcessing:
        ZNET_LOGW( "processing of of command class. After refactoring via requests manager in "
                   "shouldn't be reproduced\n" );
        executeOneStep( Step::SetupNodeSetting, m_nodeId );
        break;
    default:
        ZNET_LOGW( "unknown step" );
        break;
    }

    return true;
}

void InterviewHandlerBase::onNodeCmdSecuritySupported(
    const uint8_t nodeId,
    const znet_security_supported_report_t* nodeInfoSecuritySupported ) {
    if( nodeId != m_nodeId ) {
        ZNET_LOGI( "%s node_id=%u, expected node_id=%u\n", __func__, nodeId, m_nodeId );
        return;
    }

    if( m_step != Step::WaitSecurity && m_step != Step::WaitInclusionController ) {
        return;
    }

    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        finalize( err::interview::message( ZNET_ERR_NODE_NOT_FOUND, nodeId, utils::toUnderlyingType( m_step ) ) );
        return;
    }

    node->addCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_BASIC );

    if( nodeInfoSecuritySupported != nullptr ) {
        for( size_t ccIndex = 0; ccIndex < nodeInfoSecuritySupported->commands_count; ++ccIndex ) {
            if( nodeInfoSecuritySupported->commands[ccIndex] == ZNET_COMMAND_CLASS_MARK )
                break;

            if( nodeInfoSecuritySupported->commands[ccIndex] > 0 )
                node->addCommandClass( ZNET_CHANNEL_ID_ROOT, nodeInfoSecuritySupported->commands[ccIndex] );
        }
    }
    else {
        node->setSecurity( SecurityMode::fromByte( ZNET_NODEINFO_SECURITY_NONE ) );
    }

    updateInitialWokenUpState( *node );

    if( m_step == Step::WaitSecurity ) {
        executeOneStep( Step::GetCommandClassVersion, nodeId );
    }
}

void InterviewHandlerBase::onWakeupNotification( uint8_t nodeId ) {  // need to investigate, PM
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        ZNET_LOGE( "%s could not find node info  Id = %d\n", __func__, nodeId );
        return;
    }
    else {
        ZNET_LOGD( "%s for node(%d)\n", __func__, nodeId );
    }

    if( node->getCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_WAKE_UP ) ) {
        node->setIsWokenUp( true );
    }
}

void InterviewHandlerBase::executeOneStep( Step step, const znet_node_id_t nodeId ) {
    if( step == m_step ) {
        ZNET_LOGE( "Cannot execute the same step %u\n", step );
        return;
    }

    ZNET_LOGD( "Execute interview step %u\n", step );

    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( node == nullptr ) {
        finalize( err::interview::message( ZNET_ERR_NODE_NOT_FOUND, nodeId, utils::toUnderlyingType( step ) ) );
        return;
    }

    m_step = step;
    updateInterviewTimeStamp();
    sendInclusionProgress( step );

    switch( m_step ) {
    case Step::WaitSecurity:
    case Step::WaitInclusionController:
    case Step::WaitPrimaryController:
        break;

    case Step::GetCommandClassVersion:
        getSupportedCommandClassVersions( nodeId );
        break;

    case Step::GetZWavePlusInfo:
        getZwavePlusInfo( nodeId );
        break;

    case Step::WakeUpCapabilities:
        getWakeUpCapabilities( nodeId );
        break;

    case Step::MultichannelAssociation:
        getMultichannelAssociations( nodeId );
        break;

    case Step::Association:
        getAssociations( nodeId );
        break;

    case Step::GetManufactureSpecific:
        getManufacturerSpecifics( nodeId );
        break;

    case Step::AssociationGroupInfo:
        getAssociationsGroupInfo( nodeId );
        break;

    case Step::MultichannelEndpointSurvey:
        buildMultiChannels( nodeId );
        break;

    case Step::CommandClassesProcessing:
        processCommandClasses( nodeId );
        break;

    case Step::SetupNodeSetting:
        setupNodeSetting( nodeId );
        break;
    case Step::Finished:
        finalize( {} );
        break;

    default:
        break;
    }
}

void InterviewHandlerBase::getSupportedCommandClassVersions( znet_node_id_t nodeId,
                                                             uint8_t classIndex,
                                                             bool shortRequest ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s cannot find node %u \n", __func__, nodeId );
        return;
    }

    if( auto versionCc = node->getCommandClass<ZCc0x86Version>( ZNET_CHANNEL_ID_ROOT ) ) {
        for( ; classIndex < node->getCommandClassCount( ZNET_CHANNEL_ID_ROOT ); ++classIndex ) {
            auto cc = node->getCommandClassByIndex( ZNET_CHANNEL_ID_ROOT, classIndex );

            if( cc && isAllowedCC( cc->getId() ) ) {
                const auto requestResult = versionCc->requestCommandClassVersion(
                    cc->getId(),
                    [=]( znet_command_class_version_t version ) {
                        cc->setVersion( version );
                        getSupportedCommandClassVersions( nodeId, classIndex + 1 );
                    },
                    [=]( RequestFailType failType ) {
                        ZNET_LOGE( "Fail on getting cc 0x%x version, fail %d\n", cc->getId(), failType );
                        getSupportedCommandClassVersions( nodeId, classIndex + 1, true );
                    },
                    getJobContext(),
                    shortRequest );

                if( requestResult ) {
                    return;
                }
            }
        }
    }

    getUnsupportedCommandClassVersions( nodeId, 0, shortRequest );
}

void InterviewHandlerBase::getUnsupportedCommandClassVersions( znet_node_id_t nodeId,
                                                               uint8_t classIndex,
                                                               bool shortRequest ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s cannot find node %u \n", __func__, nodeId );
        return;
    }

    if( auto versionCc = node->getCommandClass<ZCc0x86Version>( ZNET_CHANNEL_ID_ROOT ) ) {
        for( ; classIndex < node->getUnsupportedCommandClassCount( ZNET_CHANNEL_ID_ROOT ); ++classIndex ) {
            auto ccId = node->getUnsupportedCommandClassId( ZNET_CHANNEL_ID_ROOT, classIndex );

            const auto requestResult = versionCc->requestCommandClassVersion(
                ccId,
                [=]( znet_command_class_version_t version ) {
                    // Todo: save the version if needed in the future
                    getUnsupportedCommandClassVersions( nodeId, classIndex + 1 );
                },
                [=]( RequestFailType failType ) {
                    ZNET_LOGE( "Fail on getting cc 0x%x version, fail %d\n", ccId, failType );
                    getUnsupportedCommandClassVersions( nodeId, classIndex + 1, true );
                },
                getJobContext(),
                shortRequest );

            if( requestResult ) {
                return;
            }
        }
    }

    executeOneStep( Step::GetZWavePlusInfo, nodeId );
}

void InterviewHandlerBase::processCommandClasses( znet_node_id_t nodeId,
                                                  znet_node_channel_id_t channelId,
                                                  uint8_t ccIndex ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s cannot find node %u \n", __func__, nodeId );
        return;
    }

    updateInterviewTimeStamp();

    ZNET_LOGD( "Interview: %s: ch = %d, ccIndex: = %d\n", __func__, channelId, ccIndex );

    for( ; channelId < node->getChannelsCount(); ++channelId ) {                 // iteration over channel
        for( ; ccIndex < node->getCommandClassCount( channelId ); ++ccIndex ) {  // iteration over classes
            auto cc = node->getCommandClassByIndex( channelId, ccIndex );
            if( !cc || !isAllowedCC( cc->getId() ) ) {
                continue;
            }

            auto doInterview = [=]() {
                if( channelId == ZNET_CHANNEL_ID_ROOT &&
                    ( cc->getId() == ZNET_COMMAND_CLASS_ASSOCIATION ||
                      cc->getId() == ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION ||
                      cc->getId() == ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO ) ) {
                    return false;
                }

                if( cc->hasInterviewDone() ) {
                    return false;
                }

                if( m_nodeSetting && m_nodeSetting->ignoreCcInterciew( cc->getId() ) ) {
                    ZNET_LOGD( "Ignore cc interview due to device settings, CC: %d\n", cc->getId() );
                    cc->setInterviewDone( true );
                    return false;
                }

                ZNET_LOGD( "Interview: CC = 0x%x : ch = %d\n", cc->getId(), cc->getParentChannel().getId() );

                return cc->interview(
                    [=]( RequestFailTypeOpt failOpt ) {
                        if( failOpt ) {
                            ZNET_LOGE( "Failed cc processing. CC = %d, ch = %d, fail = %d\n",
                                       cc->getId(),
                                       channelId,
                                       *failOpt );
                        }

                        if( failOpt && *failOpt == RequestFailType::ReportedFail ) {
                            finalize( err::message( ZNET_ERR_INTERVIEW, *failOpt ) );
                        }
                        else {
                            processCommandClasses( nodeId, channelId, ccIndex + 1 );
                        }
                    },
                    getJobContext() );
            };

            auto doVersionRequest = [=]() {
                ZNET_LOGD( "Version request: CC = 0x%x : ch = %d\n", cc->getId(), cc->getParentChannel().getId() );

                return node->getRequestsManager().doRequest(
                    CommandClassVersionRequest( nodeId, ZNET_CHANNEL_ID_ROOT, cc->getId() ),
                    getJobContext(),
                    [=]( znet_command_class_version_t version ) {
                        cc->setVersion( version );

                        if( !doInterview() ) {
                            processCommandClasses( nodeId, channelId, ccIndex + 1 );
                        }
                    },
                    [=]( RequestFailType failType ) {
                        ZNET_LOGE( "Fail to send version request during CC processing. CC = %d, ch = %d, fail %d\n",
                                   cc->getId(),
                                   channelId,
                                   failType );
                        if( !doInterview() ) {
                            processCommandClasses( nodeId, channelId, ccIndex + 1 );
                        }
                    } );
            };

            if( channelId != ZNET_CHANNEL_ID_ROOT ) {
                if( auto rootCC = node->getCommandClass( ZNET_CHANNEL_ID_ROOT, cc->getId() ) ) {
                    cc->setVersion( rootCC->getVersion() );
                }
                else if( doVersionRequest() ) {
                    return;
                }
            }

            if( doInterview() ) {
                return;
            }
        }
        ccIndex = 0;
    }

    processUnsupportedCommandClasses( nodeId );
}

void InterviewHandlerBase::processUnsupportedCommandClasses( znet_node_id_t nodeId,
                                                             znet_node_channel_id_t channelId,
                                                             uint8_t ccIndex ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );

    updateInterviewTimeStamp();

    ZNET_LOGD( "Interview: %s: ch = %d, ccIndex: = %d\n", __func__, channelId, ccIndex );

    auto ccExistsInRootChannel = [node]( uint8_t ccId ) {
        for( uint8_t i = 0; i != node->getUnsupportedCommandClassCount( ZNET_CHANNEL_ID_ROOT ); ++i ) {
            if( node->getUnsupportedCommandClassId( ZNET_CHANNEL_ID_ROOT, i ) == ccId )
                return true;
        }

        return false;
    };

    for( ; channelId < node->getChannelsCount(); ++channelId ) {
        if( channelId == ZNET_CHANNEL_ID_ROOT ) {
            ccIndex = 0;
            continue;
        }

        for( ; ccIndex < node->getUnsupportedCommandClassCount( channelId ); ++ccIndex ) {
            const uint8_t ccId = node->getUnsupportedCommandClassId( channelId, ccIndex );

            if( !ccExistsInRootChannel( ccId ) ) {
                ZNET_LOGD( "Version request: CC = 0x%x : ch = %d\n", ccId, channelId );

                const bool requestSent = node->getRequestsManager().doRequest(
                    CommandClassVersionRequest( nodeId, ZNET_CHANNEL_ID_ROOT, ccId ),
                    getJobContext(),
                    [=]( znet_command_class_version_t version ) {
                        processUnsupportedCommandClasses( nodeId, channelId, ccIndex + 1 );
                    },
                    [=]( RequestFailType failType ) {
                        ZNET_LOGE( "Fail to send version request during CC processing. CC = %d, ch = %d, fail %d\n",
                                   ccId,
                                   channelId,
                                   failType );
                        processUnsupportedCommandClasses( nodeId, channelId, ccIndex + 1 );
                    } );

                if( requestSent )
                    return;
            }
        }

        ccIndex = 0;
    }

    ZNET_LOGD( "Interview of Command Classes End \n" );

    executeOneStep( Step::SetupNodeSetting, nodeId );
}

void InterviewHandlerBase::getManufacturerSpecifics( znet_node_id_t nodeId ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s cannot find node %u \n", __func__, nodeId );
        return;
    }

    auto cc = node->getCommandClass<ZCc0x72ManufacturerSpecific>( ZNET_CHANNEL_ID_ROOT );

    if( cc ) {
        const auto result = cc->requestManufacturerSpecific(
            [=]( const znet_manufacturer_specific_report_t& value ) {
                ZNET_LOGD( "%s\n", __func__ );
                ZNET_LOGD( "  node_id: %d\n", nodeId );
                ZNET_LOGD( "  manufacturer_id: %d\n", value.manufacturer_id );
                ZNET_LOGD( "  product_type: %d\n", value.product_type );
                ZNET_LOGD( "  product_id: %d\n", value.product_id );

                cc->setInterviewDone( true );
                node->setManufacturerSpecific( value.manufacturer_id, value.product_id, value.product_type );

                m_nodeSetting = m_parentMesh.getOwner().provideNodeSetting(
                    node->getManufacturerId(), node->getProductType(), node->getProductId() );

                executeOneStep( Step::AssociationGroupInfo, nodeId );
            },
            [=]( RequestFailType failType ) {
                ZNET_LOGE( "%s fail = %d\n", __func__, failType );
                executeOneStep( Step::AssociationGroupInfo, nodeId );
            },
            getJobContext() );

        if( result ) {
            return;
        }
    }

    executeOneStep( Step::AssociationGroupInfo, nodeId );
}

void InterviewHandlerBase::getMultichannelAssociations( znet_node_id_t nodeId ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s cannot find node %u \n", __func__, nodeId );
        return;
    }
    if( auto cc = node->getCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION ) ) {
        if( cc->interview(
                [=]( RequestFailTypeOpt failTypeOpt ) {
                    if( failTypeOpt ) {
                        ZNET_LOGE( "%s fail = %d\n", __func__, *failTypeOpt );
                    }
                    executeOneStep( failTypeOpt ? Step::Association : Step::GetManufactureSpecific, nodeId );
                },
                getJobContext() ) ) {
            return;
        }
    }

    executeOneStep( Step::Association, nodeId );
}

void InterviewHandlerBase::getAssociations( znet_node_id_t nodeId ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s cannot find node %u \n", __func__, nodeId );
        return;
    }
    if( auto cc = node->getCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_ASSOCIATION ) ) {
        if( cc->interview(
                [=]( RequestFailTypeOpt failTypeOpt ) {
                    if( failTypeOpt ) {
                        ZNET_LOGE( "%s fail = %d\n", __func__, *failTypeOpt );
                    }
                    executeOneStep( Step::GetManufactureSpecific, nodeId );
                },
                getJobContext() ) ) {
            return;
        }
    }

    executeOneStep( Step::GetManufactureSpecific, nodeId );
}

void InterviewHandlerBase::getAssociationsGroupInfo( znet_node_id_t nodeId ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s cannot find node %u \n", __func__, nodeId );
        return;
    }
    if( auto cc = node->getCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO ) ) {
        if( m_nodeSetting && m_nodeSetting->ignoreCcInterciew( ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO ) ) {
            ZNET_LOGD( "Ignore AssociationGroupInfoCC interview due to device settings\n" );
            cc->setInterviewDone( true );
            executeOneStep( Step::MultichannelEndpointSurvey, nodeId );
            return;
        }

        if( cc->interview(
                [=]( RequestFailTypeOpt failTypeOpt ) {
                    if( failTypeOpt ) {
                        ZNET_LOGE( "%s fail = %d\n", __func__, *failTypeOpt );
                    }
                    executeOneStep( Step::MultichannelEndpointSurvey, nodeId );
                },
                getJobContext() ) ) {
            return;
        }
    }
    executeOneStep( Step::MultichannelEndpointSurvey, nodeId );
}

void InterviewHandlerBase::setupNodeSettingImpl( znet_node_id_t nodeId,
                                                 bool setupAssociations,
                                                 bool setupConfigurations ) {
    ZNET_LOGD( "%s: nodeId %d\n", __func__, nodeId );

    auto node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s cannot find node %u \n", __func__, nodeId );
        return;
    }

    const auto shouldRequestNotificationsOpt =
        m_nodeSetting ? m_nodeSetting->shouldRequestNotifications() : utils::optional<bool>();
    if( shouldRequestNotificationsOpt.has_value() ) {
        for( uint8_t i = 0; i < node->getChannelsCount(); ++i ) {
            auto notificationCommandClass = node->getCommandClass<znet::ZCc0x71Notification>( i );
            if( notificationCommandClass ) {
                notificationCommandClass->setShouldRequestValue( *shouldRequestNotificationsOpt );
            }
        }
    }

    const auto extendedUserCodesRequestRestrictedOpt =
        m_nodeSetting ? m_nodeSetting->isExtendedUserCodesRequestRestricted() : utils::optional<bool>();

    if( extendedUserCodesRequestRestrictedOpt ) {
        for( uint8_t i = 0; i < node->getChannelsCount(); ++i ) {
            if( auto userCodeCommandClass = node->getCommandClass<znet::ZCc0x63UserCode>( i ) ) {
                userCodeCommandClass->setExtendedUserCodesRequestRestricted( *extendedUserCodesRequestRestrictedOpt );
            }
        }
    }

    const auto specificInterval = m_nodeSetting ? m_nodeSetting->getWakeUpInterval() : utils::optional<uint32_t>();
    if( specificInterval ) {
        node->setDeviceSpecificWakeUpInterval( *specificInterval );
    }

    if( is_self_node_sis_or_primary() ) {
        node->updateWakeUpInterval();
    }

    if( setupConfigurations || setupAssociations ) {
        if( !m_nodeSetting ) {
            ZNET_LOGD( "%s: there is no setting for manufactureId: %d, productType: %d, productId: %d\n",
                       __func__,
                       node->getManufacturerId(),
                       node->getProductType(),
                       node->getProductId() );
        }
        else {
            if( setupConfigurations ) {
                m_nodeSetting->forEachConfiguration( [&]( const Configuration& config ) {
                    // TODO: implement settting of device specific configurations and associations step by step via
                    // callbacks

                    ZNET_LOGI( "%s, paramNumber: %d, size: %d, value: %d\n",
                               __func__,
                               config.paramNumber,
                               config.size,
                               config.value );

                    auto configurationCommandClass =
                        node->getCommandClass<znet::ZCc0x70Configuration>( ZNET_CHANNEL_ID_ROOT );
                    if( configurationCommandClass ) {
                        configurationCommandClass->setConfigurationValue(
                            config.paramNumber, config.size, config.value );
                    }
                    else {
                        ZNET_LOGW(
                            "%s: there is no configuration command class, nodeId %d\n", __func__, node->getId() );
                    }
                } );
            }

            if( setupAssociations ) {
                m_nodeSetting->forEachAssociation( [&]( const Association& association ) {
                    // TODO: implement settting of device specific configurations and associations step by step via
                    // callbacks

                    ZNET_LOGI( "%s, channelId: %d, groupId: %d, targetChannelId: %d\n",
                               __func__,
                               association.channelId,
                               association.groupId,
                               association.targetChannelId );

                    bool force = association.channelId == 0 && association.groupId == 1;

                    m_parentMesh.addNodeToGroup( nodeId,
                                                 association.channelId,
                                                 association.groupId,
                                                 znet_get_sis_node_id(),
                                                 association.targetChannelId,
                                                 force );
                } );
            }
        }
    }

    executeOneStep( Step::Finished, nodeId );
}

void InterviewHandlerBase::updateInitialWokenUpState( ZNode& node ) {
    if( node.getCommandClass<ZCc0x84WakeUp>( ZNET_CHANNEL_ID_ROOT ) ) {
        node.setIsWokenUp( true /*is_self_node_sis_or_primary()*/ );
    }
}

void InterviewHandlerBase::getZwavePlusInfo( znet_node_id_t nodeId ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s cannot find node %u \n", __func__, nodeId );
        return;
    }

    if( auto cc = node->getCommandClass( ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO ) ) {
        const auto result = node->getRequestsManager().doRequest(
            ZwavePlusInfoRequest( nodeId ),
            getJobContext(),
            [=]( const znet_zwaveplus_info_report_t& value ) {
                if( node->getBasicType() != ZNET_BASIC_CLASS_STATIC_CONTROLLER ) {
                    znet_assign_suc_return_route( nodeId );
                }

                node->setZwavePlusIconType( value.installer_icon );
                ZNET_LOGD( "set iconType(%d) to %d on ZWAVEPLUS_INFO\n", nodeId, value.installer_icon );

                node->setRoleType( value.role_type );
                ZNET_LOGD( "set roleType(%d) on ZWAVEPLUS_INFO\n", nodeId );

                executeOneStep( Step::WakeUpCapabilities, nodeId );
            },
            [=]( RequestFailType failType ) {
                ZNET_LOGE( "%s fail = %d\n", __func__, failType );
                executeOneStep( Step::WakeUpCapabilities, nodeId );
            } );

        if( result ) {
            return;
        }
    }

    executeOneStep( Step::WakeUpCapabilities, nodeId );
}

void InterviewHandlerBase::getWakeUpCapabilities( znet_node_id_t nodeId ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s cannot find node %u \n", __func__, nodeId );
        return;
    }
    auto cc = node->getCommandClass<ZCc0x84WakeUp>( ZNET_CHANNEL_ID_ROOT );

    auto onResult = [=]( RequestFailTypeOpt failTypeOpt ) {
        if( failTypeOpt ) {
            ZNET_LOGE( "%s fail = %d\n", __func__, *failTypeOpt );
        }
        executeOneStep( Step::MultichannelAssociation, nodeId );
    };

    if( cc && cc->interview( onResult, getJobContext() ) ) {
        return;
    }

    executeOneStep( Step::MultichannelAssociation, nodeId );
}

void InterviewHandlerBase::buildMultiChannels( znet_node_id_t nodeId ) {
    ZNode* node = m_parentMesh.onFindNode( nodeId );
    if( !node ) {
        ZNET_LOGE( "%s cannot find node %u \n", __func__, nodeId );
        return;
    }

    auto cc = node->getCommandClass<ZCc0x60MultiChannel>( ZNET_CHANNEL_ID_ROOT );

    if( cc && cc->buildChannels(
                  [=]( RequestFailTypeOpt failTypeOpt ) {
                      if( failTypeOpt ) {
                          ZNET_LOGE( "%s fail = %d\n", __func__, *failTypeOpt );
                      }
                      executeOneStep( Step::CommandClassesProcessing, nodeId );
                  },
                  getJobContext() ) ) {
        return;
    }

    executeOneStep( Step::CommandClassesProcessing, nodeId );
}

utils::optional<uint8_t> InterviewHandlerBase::getProgressByStep( const Step interviewStep ) {
    switch( interviewStep ) {
    case Step::Started:
    case Step::GetNodeInfo:
        return 0;
    case Step::GetCommandClassVersion:
        return 5;
    case Step::WakeUpCapabilities:
        return 15;
    case Step::Association:
        return 20;
    case Step::MultichannelAssociation:
        return 30;
    case Step::AssociationGroupInfo:
        return 40;
    case Step::MultichannelEndpointSurvey:
        return 45;
    case Step::CommandClassesProcessing:
        return 50;
    case Step::SetupNodeSetting:
        return 80;
    case Step::Finished:
        return 95;
    default:
        return {};
    }
}

std::chrono::milliseconds InterviewHandlerBase::getMaxWaitingTime( Step step ) {
    switch( step ) {
    case Step::WaitSecurity:
        return std::chrono::milliseconds( SecurityWaitingTimeoutMs );
    case Step::WaitInclusionController:
        return std::chrono::milliseconds( InclusionControllerTimeoutMs );
    case Step::WaitPrimaryController:
        return std::chrono::milliseconds( PrimaryControllerTimeoutMs );
    default:
        // Other command classes shouldn't hang
        return std::chrono::milliseconds( InterviewTimeoutMs );
    }
}

void InterviewHandlerBase::sendInclusionProgress( const Step interviewStep ) {
    if( auto progress = getProgressByStep( interviewStep ) ) {
        m_parentMesh.updateInclusionProgress( *progress );
    }
}

void InterviewHandlerBase::onNodeFailed( uint8_t nodeId, int error ) {
    ZNET_LOGE( "Interview for node %d failed\n", nodeId );
    auto prevStep = m_step;
    if( error == ZNET_ERR_TIME ) {
        m_step = Step::Timeout;
    }

    finalize( err::interview::message( error, nodeId, utils::toUnderlyingType( prevStep ) ) );
}

bool InterviewHandlerBase::isAllowedCC( uint8_t classId ) const {
    switch( classId ) {
        // TODO  expand list of classes by the roadmap of Atom32

    case ZNET_COMMAND_CLASS_BASIC:                       //    0x20
    case ZNET_COMMAND_CLASS_SWITCH_BINARY:               //    0x25
    case ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL:           //    0x26
                                                         //        ZNET_COMMAND_CLASS_SWITCH_ALL   0x27
                                                         //        ZNET_COMMAND_CLASS_SWITCH_TOGGLE_BINARY 0x28
                                                         //        ZNET_COMMAND_CLASS_SWITCH_TOGGLE_MULTILEVEL 0x29
                                                         //        ZNET_COMMAND_CLASS_CHIMNEY_FAN  0x2A
    case ZNET_COMMAND_CLASS_SCENE_ACTIVATION:            // 0x2B
    case ZNET_COMMAND_CLASS_SCENE_ACTUATOR_CONF:         //  0x2C
                                                         //        ZNET_COMMAND_CLASS_SCENE_CONTROLLER_CONF    0x2D
                                                         //        ZNET_COMMAND_CLASS_SECURITY_PANEL_ZONE  0x2E
                                                         //        ZNET_COMMAND_CLASS_SECURITY_PANEL_ZONE_SENSOR   0x2F
    case ZNET_COMMAND_CLASS_SENSOR_BINARY:               //    0x30
    case ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL:           //    0x31
    case ZNET_COMMAND_CLASS_METER:                       //    0x32
    case ZNET_COMMAND_CLASS_SWITCH_COLOR:                // 0x33
                                                         //        ZNET_COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION 0x34
                                                         //        ZNET_COMMAND_CLASS_METER_PULSE  0x35
                                                         //        ZNET_COMMAND_CLASS_BASIC_TARIFF_INFO    0x36
                                                         //        ZNET_COMMAND_CLASS_HRV_STATUS   0x37
                                                         //        ZNET_COMMAND_CLASS_THERMOSTAT_HEATING   0x38
                                                         //        ZNET_COMMAND_CLASS_HRV_CONTROL  0x39
                                                         //        ZNET_COMMAND_CLASS_DCP_CONFIG   0x3A
                                                         //        ZNET_COMMAND_CLASS_DCP_MONITOR  0x3B
                                                         //        ZNET_COMMAND_CLASS_METER_TBL_CONFIG 0x3C
                                                         //        ZNET_COMMAND_CLASS_METER_TBL_MONITOR    0x3D
                                                         //        ZNET_COMMAND_CLASS_METER_TBL_PUSH   0x3E
                                                         //        ZNET_COMMAND_CLASS_PREPAYMENT   0x3F
    case ZNET_COMMAND_CLASS_THERMOSTAT_MODE:             //  0x40
                                                         //        ZNET_COMMAND_CLASS_PREPAYMENT_ENCAPSULATION 0x41
    case ZNET_COMMAND_CLASS_THERMOSTAT_OPERATING_STATE:  //   0x42
    case ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT:         //  0x43
    case ZNET_COMMAND_CLASS_THERMOSTAT_FAN_MODE:         //  0x44
    case ZNET_COMMAND_CLASS_THERMOSTAT_FAN_STATE:        // 0x45
                                                         //        ZNET_COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE 0x46
    case ZNET_COMMAND_CLASS_THERMOSTAT_SETBACK:          //   0x47
                                                         //        ZNET_COMMAND_CLASS_RATE_TBL_CONFIG  0x48
                                                         //        ZNET_COMMAND_CLASS_RATE_TBL_MONITOR 0x49
                                                         //        ZNET_COMMAND_CLASS_TARIFF_CONFIG    0x4A
                                                         //        ZNET_COMMAND_CLASS_TARIFF_TBL_MONITOR   0x4B
                                                         //        ZNET_COMMAND_CLASS_DOOR_LOCK_LOGGING    0x4C
                                                         //        ZNET_COMMAND_CLASS_NETWORK_MANAGEMENT_BASIC 0x4D
    case ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK:         //  0x4E
                                                         //        ZNET_COMMAND_CLASS_ZIP_6LOWPAN  0x4F
                                                         //        ZNET_COMMAND_CLASS_BASIC_WINDOW_COVERING    0x50
                                                         //        ZNET_COMMAND_CLASS_MTP_WINDOW_COVERING  0x51
                                                         //        ZNET_COMMAND_CLASS_NETWORK_MANAGEMENT_PROXY 0x52
                                                         //        ZNET_COMMAND_CLASS_SCHEDULE 0x53
                                                         //        ZNET_COMMAND_CLASS_NETWORK_MANAGEMENT_PRIMARY   0x54
                                                         //        ZNET_COMMAND_CLASS_TRANSPORT_SERVICE    0x55
                                                         //        ZNET_COMMAND_CLASS_CRC_16_ENCAP 0x56
                                                         //        ZNET_COMMAND_CLASS_APPLICATION_CAPABILITY   0x57
                                                         //        ZNET_COMMAND_CLASS_ZIP_ND   0x58
    case ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO:        // 0x59
                                                         //        ZNET_COMMAND_CLASS_DEVICE_RESET_LOCALLY 0x5A
    case ZNET_COMMAND_CLASS_CENTRAL_SCENE:               //    0x5B
                                                         //        ZNET_COMMAND_CLASS_IP_ASSOCIATION   0x5C
    case ZNET_COMMAND_CLASS_ANTITHEFT:
    case ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK:
    case ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO:    // 0x5E
                                               //        ZNET_COMMAND_CLASS_ZIP_GATEWAY  0x5F
    case ZNET_COMMAND_CLASS_MULTI_CHANNEL:     //    0x60
                                               //        ZNET_COMMAND_CLASS_MULTI_INSTANCE   0x60
                                               //        ZNET_COMMAND_CLASS_ZIP_PORTAL   0x61
    case ZNET_COMMAND_CLASS_DOOR_LOCK:         //    0x62
    case ZNET_COMMAND_CLASS_USER_CODE:         //    0x63
                                               //        ZNET_COMMAND_CLASS_APPLIANCE    0x64
                                               //        ZNET_COMMAND_CLASS_HUMIDITY_CONTROL_SETPOINT    0x64
                                               //        ZNET_COMMAND_CLASS_DMX  0x65
    case ZNET_COMMAND_CLASS_BARRIER_OPERATOR:  // 0x66
                                               //        ZNET_COMMAND_CLASS_NETWORK_MANAGEMENT_INSTALLATION_MAINTENANCE
                                               //        0x67 ZNET_COMMAND_CLASS_ZIP_NAMING   0x68
                                               //        ZNET_COMMAND_CLASS_MAILBOX  0x69
                                               //        ZNET_COMMAND_CLASS_WINDOW_COVERING  0x6A
                                               //        ZNET_COMMAND_CLASS_IRRIGATION   0x6B
                                               //        ZNET_COMMAND_CLASS_SUPERVISION  0x6C
                                               //        ZNET_COMMAND_CLASS_HUMIDITY_CONTROL_MODE    0x6D
                                               //        ZNET_COMMAND_CLASS_HUMIDITY_CONTROL_OPERATING_STATE 0x6E
    case ZNET_COMMAND_CLASS_ENTRY_CONTROL:     //    0x6F
    case ZNET_COMMAND_CLASS_CONFIGURATION:     //    0x70
    case ZNET_COMMAND_CLASS_NOTIFICATION:      // 0x71
    case ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC:      //    0x72
                                                        //        ZNET_COMMAND_CLASS_POWERLEVEL 0x73
                                                        //        ZNET_COMMAND_CLASS_INCLUSION_CONTROLLER 0x74
                                                        //        ZNET_COMMAND_CLASS_PROTECTION   0x75
                                                        //        ZNET_COMMAND_CLASS_LOCK 0x76
                                                        //        ZNET_COMMAND_CLASS_NODE_NAMING  0x77
                                                        //        ZNET_COMMAND_CLASS_NODE_PROVISIONING    0x78
    case ZNET_COMMAND_CLASS_SOUND_SWITCH:               // 0x79
    case ZNET_COMMAND_CLASS_FIRMWARE_UPDATE_MD:         //   0x7A
                                                        //        ZNET_COMMAND_CLASS_GROUPING_NAME    0x7B
                                                        //        ZNET_COMMAND_CLASS_REMOTE_ASSOCIATION_ACTIVATE  0x7C
                                                        //        ZNET_COMMAND_CLASS_REMOTE_ASSOCIATION   0x7D
    case ZNET_COMMAND_CLASS_BATTERY:                    //  0x80
    case ZNET_COMMAND_CLASS_CLOCK:                      //    0x81
                                                        //        ZNET_COMMAND_CLASS_HAIL 0x82
    case ZNET_COMMAND_CLASS_WAKE_UP:                    //  0x84
    case ZNET_COMMAND_CLASS_ASSOCIATION:                //  0x85
    case ZNET_COMMAND_CLASS_VERSION:                    //  0x86
    case ZNET_COMMAND_CLASS_INDICATOR:                  //  0x87
                                                        //        ZNET_COMMAND_CLASS_PROPRIETARY  0x88
                                                        //        ZNET_COMMAND_CLASS_LANGUAGE 0x89
    case ZNET_COMMAND_CLASS_TIME:                       // 0x8A
    case ZNET_COMMAND_CLASS_TIME_PARAMETERS:            //  0x8B
                                                        //        ZNET_COMMAND_CLASS_GEOGRAPHIC_LOCATION  0x8C
    case ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION:  //    0x8E
                                                        //        ZNET_COMMAND_CLASS_MULTI_INSTANCE_ASSOCIATION   0x8E
                                                        //        ZNET_COMMAND_CLASS_MULTI_CMD    0x8F
                                                        //        ZNET_COMMAND_CLASS_ENERGY_PRODUCTION    0x90
                                                        //        ZNET_COMMAND_CLASS_MANUFACTURER_PROPRIETARY 0x91
                                                        //        ZNET_COMMAND_CLASS_SCREEN_MD    0x92
                                                        //        ZNET_COMMAND_CLASS_SCREEN_ATTRIBUTES    0x93
                                                        //        ZNET_COMMAND_CLASS_SIMPLE_AV_CONTROL    0x94
                                                        //        ZNET_COMMAND_CLASS_AV_CONTENT_DIRECTORY_MD  0x95
                                                        //        ZNET_COMMAND_CLASS_AV_RENDERER_STATUS   0x96
                                                        //        ZNET_COMMAND_CLASS_AV_CONTENT_SEARCH_MD 0x97
    case ZNET_COMMAND_CLASS_SECURITY:                   // 0x98
                                                        //        ZNET_COMMAND_CLASS_AV_TAGGING_MD    0x99
                                                        //        ZNET_COMMAND_CLASS_IP_CONFIGURATION 0x9A
        //        ZNET_COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION    0x9B
    case ZNET_COMMAND_CLASS_SENSOR_ALARM:   // 0x9C
    case ZNET_COMMAND_CLASS_SILENCE_ALARM:  //    0x9D
                                            //        ZNET_COMMAND_CLASS_SENSOR_CONFIGURATION 0x9E
    case ZNET_COMMAND_CLASS_SECURITY_2:     // 0x9F
                                            //        ZNET_COMMAND_CLASS_MARK 0xEF
                                            //        ZNET_COMMAND_CLASS_NON_INTEROPERABLE    0xF0

        return true;

    default:
        return false;
    }
}

bool InterviewHandlerBase::isActuatorCC( uint8_t classId ) const {
    switch( classId ) {
    case ZNET_COMMAND_CLASS_BARRIER_OPERATOR:     // 0x66
    case ZNET_COMMAND_CLASS_BASIC:                // 0x20
    case ZNET_COMMAND_CLASS_SWITCH_BINARY:        // 0x25
    case ZNET_COMMAND_CLASS_SWITCH_COLOR:         // 0x33
    case ZNET_COMMAND_CLASS_DOOR_LOCK:            // 0x62
    case ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL:    // 0x26
    case ZNET_COMMAND_CLASS_SOUND_SWITCH:         // 0x79
    case ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT:  // 0x43
        return true;

    default:
        return false;
    }
}

InterviewHandlerBase::Step InterviewHandlerBase::getCurrentStep() const {
    return m_step;
}

bool InterviewHandlerBase::storeSmartStartData( const znet_nodeinfo_t& nodeInfo, StoreSmartStartDataMode mode ) {
    using fields_t = ::znet::ProvisioningListEntryFields;
    if( nodeInfo.smart_start_data.data != nullptr ) {
        std::array<uint8_t, 4> nhid;
        std::memcpy( nhid.data(), nodeInfo.smart_start_data.nhid, nhid.size() );
        ::znet::api::ProvisioningListQuery query{ m_parentMesh.getProvisioningListControllerDataSource() };
        query.filterBy( fields::nhid( nhid ) );
        query.setField( &fields_t::m_status, fields_t::Status::ADDED );
        if( StoreSmartStartDataMode::All == mode || StoreSmartStartDataMode::NodeId == mode ) {
            query.setField( &fields_t::m_node, nodeInfo.node_id );
        }
        if( StoreSmartStartDataMode::All == mode || StoreSmartStartDataMode::SecurityModes == mode ) {
            query.setField(
                &fields_t::m_nodeSecurityModes,
                ::znet::security_modes_converter::typeToBitset( SecurityMode::fromByte( nodeInfo.security ) ) );
        }
        return query.execute();
    }
    return false;
}

void InterviewHandlerBase::updateInterviewTimeStamp() {
    m_interviewTimeStamp = std::chrono::system_clock::now();
}

}  // namespace znet
