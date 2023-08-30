#include <znet/znet_node.hpp>
#include <znet/cc_0x85_association.hpp>
#include <znet/znet_requests_manager.hpp>
#include <znet/znet_utils.hpp>

#include <cassert>

extern "C" {
#include <znet_log.h>
}

namespace znet {

bool ZCc0x85Association::interview( InterviewCallback callback, WeakJobContext context ) {
    ZNode::ClassPtr ccTempMultichannelAssociations =
        getParentNode().getCommandClass( getParentChannel().getId(), ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION );
    if( getChannelId() != ZNET_CHANNEL_ID_ROOT && ccTempMultichannelAssociations ) {
        return ZClass::interview( callback, context );
    }

    auto callbackPtr = std::make_shared<InterviewCallback>( std::move( callback ) );

    return getParentNode().getRequestsManager().doRequest(
        AssociationSupportedGroupingRequest( getNodeId(), getChannelId() ),
        context,
        [=]( uint8_t groupsCount ) {
            m_associationsCount = groupsCount;
            m_groupsMaxNodesCount.reserve( m_associationsCount );
            requestNextGroupInterview( context, callbackPtr, 1 );
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
}

void ZCc0x85Association::onGroupInfo( const znet_association_report_t* value ) {
    m_groupInfos[value->grouping_id] = std::vector<uint8_t>( value->node_ids, value->node_ids + value->node_id_count );
}

uint8_t ZCc0x85Association::getAssociationGroupsCount() const {
    return m_associationsCount;
}

const std::vector<uint8_t>* ZCc0x85Association::getGroupNodes( uint8_t groupId ) const {
    if( groupId <= m_associationsCount ) {
        auto info = m_groupInfos.find( groupId );
        if( info != m_groupInfos.end() ) {
            return &info->second;
        }
    }

    return nullptr;
}

uint8_t ZCc0x85Association::getGroupMaxNodesCount( uint8_t groupId ) const {
    if( m_groupsMaxNodesCount.size() >= groupId ) {
        return m_groupsMaxNodesCount.at( groupId - 1 );
    }
    else {
        return 255;
    }
}

bool ZCc0x85Association::addNodeToGroup( uint8_t groupId,
                                         uint8_t nodeId,
                                         OnFailCallback onFail,
                                         WeakJobContext context ) {
    // Temporary commented till MultiChannelAssociation interview has been implemented
    //    if( groupId > m_associationsCount ) {
    //        return false;
    //    }

    std::vector<uint8_t>* groupNodes = nullptr;

    if( m_groupInfos.find( groupId ) != m_groupInfos.end() ) {
        groupNodes = &m_groupInfos[groupId];
        for( auto groupNodeId : *groupNodes ) {
            if( groupNodeId == nodeId ) {
                ZNET_LOGI( "cc_0x%x: addNodeToGroup: node %d already in group %d.", getId(), nodeId, groupId );
                return true;
            }
        }
        groupNodes->push_back( nodeId );
    }
    else {
        m_groupInfos[groupId] = { nodeId };
        groupNodes = &m_groupInfos[groupId];
    }

    return getParentNode().getRequestsManager().doRequest(
        AssociationSet( getNodeId(), getChannelId(), groupId, &nodeId, 1 ),
        context,
        [=]() {
            getParentNode().getRequestsManager().doRequest(
                AssociationRequest( getNodeId(), getChannelId(), groupId ),
                context,
                [=]( const znet_association_report_t* report ) { onGroupInfo( report ); },
                onFail,
                DefaultPostponedRequestDelay );
        },
        onFail );
}

bool ZCc0x85Association::removeNodeFromGroup( uint8_t groupId, uint8_t nodeId ) {
    if( groupId > m_associationsCount ) {
        return false;
    }

    bool result = false;

    if( m_groupInfos.find( groupId ) != m_groupInfos.end() ) {
        auto& groupEndpoints = m_groupInfos[groupId];
        int nodeIndex = -1;
        for( int i = 0; i < groupEndpoints.size(); ++i ) {
            auto& groupNodeId = groupEndpoints.at( i );
            if( groupNodeId == nodeId ) {
                znet_node_cmd_association_remove(
                    getParentNode().getId(), getParentChannel().getId(), groupId, &nodeId, 1 );

                nodeIndex = i;
                result = true;
                break;
            }
        }
        if( nodeIndex > 0 ) {
            groupEndpoints.erase( groupEndpoints.begin() + nodeIndex );
        }
        else {
            ZNET_LOGE( "cc_0x%x: removeNodeFromGroup: node %d is not in group %d\n", getId(), nodeId, groupId );
        }
    }

    return result;
}

bool ZCc0x85Association::requestValue( const RequestValue& request ) {
    auto context = request.jobContext;

    auto callbackPtr =
        request.resultCallback ? std::make_shared<RequestValue::Callback>( request.resultCallback ) : nullptr;

    if( request.toRequestValueAssociation() != nullptr &&
        request.toRequestValueAssociation()->groupNum <= m_associationsCount ) {
        return getParentNode().getRequestsManager().doRequest(
            AssociationRequest( getNodeId(), getChannelId(), request.toRequestValueAssociation()->groupNum ),
            context,
            [=]( const znet_association_report_t* report ) { onGroupInfo( report ); },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
    }
    else if( m_associationsCount != ms_undefinedGroupsCount ) {
        return requestNextGroup( context, callbackPtr, 1 );
    }
    else {
        return false;
    }
}

bool ZCc0x85Association::storeData( IFile& file ) const {
    bool result = file.write( &m_associationsCount, sizeof( m_associationsCount ) );
    result &= file.write( &m_currentProcessingGroup, sizeof( m_currentProcessingGroup ) );

    int groups_count = m_groupInfos.size();
    result &= file.write( &groups_count, sizeof( groups_count ) );

    for( auto const& groupInfo : m_groupInfos ) {
        result &= file.write( &groupInfo.first, sizeof( groupInfo.first ) );

        int nodesCount = groupInfo.second.size();
        result &= file.write( &nodesCount, sizeof( nodesCount ) );

        for( auto nodeId : groupInfo.second ) {
            result &= file.write( &nodeId, sizeof( nodeId ) );
        }
    }

    result &= utils::writeVectorToFile( file, m_groupsMaxNodesCount );

    return result;
}

bool ZCc0x85Association::restoreData( IFile& file, int version ) {
    bool result = file.read( &m_associationsCount, sizeof( m_associationsCount ) );
    result &= file.read( &m_currentProcessingGroup, sizeof( m_currentProcessingGroup ) );

    int groups_count;
    result &= file.read( &groups_count, sizeof( groups_count ) );
    for( int i = 0; i < groups_count; ++i ) {
        uint8_t groupNum;
        result &= file.read( &groupNum, sizeof( groupNum ) );

        int nodesCount;
        result &= file.read( &nodesCount, sizeof( nodesCount ) );
        std::vector<uint8_t> nodes;
        for( int j = 0; j < nodesCount; ++j ) {
            uint8_t nodeId;
            result &= file.read( &nodeId, sizeof( nodeId ) );
            nodes.push_back( nodeId );
        }

        m_groupInfos[groupNum] = std::move( nodes );
    }

    if( version >= majorFormatVersion_v24 ) {
        result &= utils::readVectorFromFile( file, m_groupsMaxNodesCount );
    }
    else {
        m_groupsMaxNodesCount = std::vector<uint8_t>( m_associationsCount, 255 );
    }

    return result;
}

bool ZCc0x85Association::requestNextGroupInterview( WeakJobContext jobContext,
                                                    RequestValue::CallbackPtr cb,
                                                    uint8_t groupId ) {
    return getParentNode().getRequestsManager().doRequest(
        AssociationRequest( getNodeId(), getChannelId(), groupId ),
        jobContext,
        [=]( const znet_association_report_t* report ) {
            m_groupsMaxNodesCount.push_back( report->max_nodes_supported );

            if( groupId < 255 && groupId + 1 <= m_associationsCount ) {
                requestNextGroupInterview( jobContext, cb, groupId + 1 );
            }
            else if( getParentChannel().getId() == ZNET_CHANNEL_ID_ROOT && is_self_node_sis_or_primary() ) {
                setSisToLifelineGroup( jobContext, cb );
            }
            else {
                completeInterviewProcess( cb );
            }
        },
        std::bind( onRequestsFinished, cb, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

bool ZCc0x85Association::requestNextGroup( WeakJobContext jobContext, RequestValue::CallbackPtr cb, uint8_t groupId ) {
    if( groupId > m_associationsCount ) {
        onRequestsFinished( cb, {} );
        return true;
    }

    return getParentNode().getRequestsManager().doRequest( AssociationRequest( getNodeId(), getChannelId(), groupId ),
                                                           jobContext,
                                                           [=]( const znet_association_report_t* report ) {
                                                               onGroupInfo( report );

                                                               if( !requestNextGroup( jobContext, cb, groupId + 1 ) ) {
                                                                   onRequestsFinished( cb,
                                                                                       RequestFailType::SendingError );
                                                               }
                                                           },
                                                           std::bind( onRequestsFinished, cb, std::placeholders::_1 ),
                                                           DefaultPostponedRequestDelay );
}

bool ZCc0x85Association::setSisToLifelineGroup( WeakJobContext jobContext, RequestValue::CallbackPtr cb ) {
    static znet_node_id_t selfNodeId;
    selfNodeId = znet_get_self_node_id();

    return getParentNode().getRequestsManager().doRequest(
        AssociationSet( getNodeId(), getChannelId(), 1, &selfNodeId, 1 ),
        jobContext,
        [=]() {
            getParentNode().getRequestsManager().doRequest( AssociationRequest( getNodeId(), getChannelId(), 1 ),
                                                            jobContext,
                                                            [=]( const znet_association_report_t* report ) {
                                                                onGroupInfo( report );
                                                                completeInterviewProcess( cb );
                                                            },
                                                            std::bind( onRequestsFinished, cb, std::placeholders::_1 ),
                                                            DefaultPostponedRequestDelay );
        },
        std::bind( onRequestsFinished, cb, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

void ZCc0x85Association::completeInterviewProcess( RequestValue::CallbackPtr cb ) {
    setInterviewDone( true );
    onRequestsFinished( cb, {} );
}

}  // namespace znet
