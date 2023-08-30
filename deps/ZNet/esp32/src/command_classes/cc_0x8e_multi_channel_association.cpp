#include <znet/znet_node.hpp>
#include <znet/cc_0x8e_multi_channel_association.hpp>
#include <znet/znet_requests_manager.hpp>

#include <cassert>

extern "C" {
#include <znet_log.h>
}

namespace znet {

bool ZCc0x8eMultiChannelAssociation::interview( InterviewCallback callback, WeakJobContext context ) {
    auto callbackPtr = std::make_shared<InterviewCallback>( std::move( callback ) );

    return getParentNode().getRequestsManager().doRequest(
        MultiChannelAssociationSupportedGroupingRequest( getNodeId(), getChannelId() ),
        context,
        [=]( uint8_t groupsCount ) {
            m_associationsCount = groupsCount;
            m_groupsMaxNodesCount.reserve( m_associationsCount );
            requestNextGroupInterview( context, callbackPtr, 1 );
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
}

void ZCc0x8eMultiChannelAssociation::onGroupInfo( const znet_multichannel_association_report_t* value ) {
    m_groupInfos[value->grouping_id] =
        std::vector<znet_endpoint_t>( value->end_pints, value->end_pints + value->end_pints_count );
}

uint8_t ZCc0x8eMultiChannelAssociation::getAssociationGroupsCount() const {
    return m_associationsCount;
}

const std::vector<znet_endpoint_t>* ZCc0x8eMultiChannelAssociation::getGroupInfo( uint8_t groupId ) const {
    if( groupId <= m_associationsCount ) {
        auto info = m_groupInfos.find( groupId );
        if( info != m_groupInfos.end() ) {
            return &info->second;
        }
    }

    return nullptr;
}

uint8_t ZCc0x8eMultiChannelAssociation::getGroupMaxNodesCount( uint8_t groupId ) const {
    if( m_groupsMaxNodesCount.size() >= groupId ) {
        return m_groupsMaxNodesCount.at( groupId - 1 );
    }
    else {
        return 255;
    }
}

ZCc0x8eMultiChannelAssociation::NodeAddResult ZCc0x8eMultiChannelAssociation::addNodeToGroup( uint8_t groupId,
                                                                                              uint8_t nodeId,
                                                                                              uint8_t targetChannelId,
                                                                                              bool force,
                                                                                              OnFailCallback onFail,
                                                                                              WeakJobContext context ) {
    if( groupId > m_associationsCount ) {
        return NodeAddResult::Failed;
    }

    std::vector<znet_endpoint_t>* groupEndpoints = nullptr;

    if( m_groupInfos.find( groupId ) != m_groupInfos.end() ) {
        groupEndpoints = &m_groupInfos[groupId];
        for( auto groupEndpoint : *groupEndpoints ) {
            if( groupEndpoint.node_id == nodeId && groupEndpoint.channel_id == targetChannelId ) {
                ZNET_LOGE( "cc_0x%x: addNodeToGroup: node %d already in group %d\n", getId(), nodeId, groupId );
                return NodeAddResult::AlreadyExists;
            }
        }

        if( groupEndpoints->size() >= getGroupMaxNodesCount( groupId ) ) {
            if( force ) {
                removeNodeFromGroup( groupId, groupEndpoints->front().node_id, groupEndpoints->front().channel_id );
            }
            else {
                ZNET_LOGE( "cc_0x%x: addNodeToGroup: group %d is completely filled \n", getId(), groupId );
                return NodeAddResult::Failed;
            }
        }

        groupEndpoints->push_back( { nodeId, targetChannelId } );
    }
    else {
        if( getGroupMaxNodesCount( groupId ) != 0 ) {
            m_groupInfos[groupId] = { { nodeId, targetChannelId } };
            groupEndpoints = &m_groupInfos[groupId];
        }
        else {
            ZNET_LOGE( "cc_0x%x: addNodeToGroup: group %d must be empty \n", getId(), groupId );
            return NodeAddResult::Failed;
        }
    }

    return getParentNode().getRequestsManager().doRequest(
               MultiChannelAssociationSet( getNodeId(), getChannelId(), groupId, &groupEndpoints->back(), 1 ),
               context,
               [=]() {
                   getParentNode().getRequestsManager().doRequest(
                       MultiChannelAssociationRequest( getNodeId(), getChannelId(), groupId ),
                       context,
                       [=]( const znet_multichannel_association_report_t* report ) { onGroupInfo( report ); },
                       onFail,
                       DefaultPostponedRequestDelay );
               },
               onFail ) ?
               NodeAddResult::Success :
               NodeAddResult::Failed;
}

bool ZCc0x8eMultiChannelAssociation::removeNodeFromGroup( uint8_t groupId, uint8_t nodeId, uint8_t targetChannelId ) {
    if( groupId > m_associationsCount ) {
        return false;
    }

    bool result = false;

    if( m_groupInfos.find( groupId ) != m_groupInfos.end() ) {
        auto& groupEndpoints = m_groupInfos[groupId];
        int nodeIndex = -1;
        for( int i = 0; i < groupEndpoints.size(); ++i ) {
            auto& groupEndpoint = groupEndpoints.at( i );
            if( groupEndpoint.node_id == nodeId && groupEndpoint.channel_id == targetChannelId ) {
                znet_endpoint_t value{ nodeId, targetChannelId };
                znet_node_cmd_multichannel_association_remove(
                    getParentNode().getId(), getParentChannel().getId(), groupId, &value, 1 );

                nodeIndex = i;
                result = true;
                break;
            }
        }
        if( nodeIndex >= 0 ) {
            groupEndpoints.erase( groupEndpoints.begin() + nodeIndex );
        }
        else {
            ZNET_LOGE( "cc_0x%x: removeNodeFromGroup: node %d is not in group %d.\n", getId(), nodeId, groupId );
        }
    }

    return result;
}

bool ZCc0x8eMultiChannelAssociation::requestValue( const RequestValue& request ) {
    auto context = request.jobContext;

    auto callbackPtr =
        request.resultCallback ? std::make_shared<RequestValue::Callback>( request.resultCallback ) : nullptr;

    if( request.toRequestValueAssociation() != nullptr &&
        request.toRequestValueAssociation()->groupNum <= m_associationsCount ) {
        return getParentNode().getRequestsManager().doRequest(
            MultiChannelAssociationRequest(
                getNodeId(), getChannelId(), request.toRequestValueAssociation()->groupNum ),
            context,
            [=]( const znet_multichannel_association_report_t* report ) { onGroupInfo( report ); },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
    }
    else if( m_associationsCount != ms_undefinedGroupsCount ) {
        return requestNextGroup( context, callbackPtr, 1 );
    }
    else {
        return false;
    }
}

bool ZCc0x8eMultiChannelAssociation::storeData( IFile& file ) const {
    bool result = file.write( &m_associationsCount, sizeof( m_associationsCount ) );
    result &= file.write( &m_currentProcessingGroup, sizeof( m_currentProcessingGroup ) );

    int groups_count = m_groupInfos.size();
    result &= file.write( &groups_count, sizeof( groups_count ) );

    for( auto const& groupInfo : m_groupInfos ) {
        result &= file.write( &groupInfo.first, sizeof( groupInfo.first ) );

        int nodesCount = groupInfo.second.size();
        result &= file.write( &nodesCount, sizeof( nodesCount ) );

        for( auto nodeInfo : groupInfo.second ) {
            result &= file.write( &nodeInfo.node_id, sizeof( nodeInfo.node_id ) );
            result &= file.write( &nodeInfo.channel_id, sizeof( nodeInfo.channel_id ) );
        }
    }

    groups_count = m_groupsMaxNodesCount.size();
    result &= file.write( &groups_count, sizeof( groups_count ) );

    for( auto const& maxNodesCount : m_groupsMaxNodesCount ) {
        result &= file.write( &maxNodesCount, sizeof( maxNodesCount ) );
    }

    return result;
}

bool ZCc0x8eMultiChannelAssociation::restoreData( IFile& file, int version ) {
    bool result = file.read( &m_associationsCount, sizeof( m_associationsCount ) );
    result &= file.read( &m_currentProcessingGroup, sizeof( m_currentProcessingGroup ) );

    int groups_count;
    result &= file.read( &groups_count, sizeof( groups_count ) );
    for( int i = 0; i < groups_count; ++i ) {
        uint8_t groupNum;
        result &= file.read( &groupNum, sizeof( groupNum ) );

        int nodesCount;
        result &= file.read( &nodesCount, sizeof( nodesCount ) );
        std::vector<znet_endpoint_t> nodes;
        for( int j = 0; j < nodesCount; ++j ) {
            znet_endpoint_t nodeInfo;
            result &= file.read( &nodeInfo.node_id, sizeof( nodeInfo.node_id ) );
            result &= file.read( &nodeInfo.channel_id, sizeof( nodeInfo.channel_id ) );
            nodes.push_back( nodeInfo );
        }

        m_groupInfos[groupNum] = std::move( nodes );
    }

    if( version >= majorFormatVersion_v23 ) {
        result &= file.read( &groups_count, sizeof( groups_count ) );
        m_groupsMaxNodesCount.reserve( groups_count );
        for( int i = 0; i < groups_count; ++i ) {
            uint8_t maxNodesCount;
            result &= file.read( &maxNodesCount, sizeof( maxNodesCount ) );
            m_groupsMaxNodesCount.push_back( maxNodesCount );
        }
    }
    else {
        m_groupsMaxNodesCount = std::vector<uint8_t>( m_associationsCount, 255 );
    }

    return result;
}

bool ZCc0x8eMultiChannelAssociation::requestNextGroupInterview( WeakJobContext jobContext,
                                                                RequestValue::CallbackPtr cb,
                                                                uint8_t groupId ) {
    return getParentNode().getRequestsManager().doRequest(
        MultiChannelAssociationRequest( getNodeId(), getChannelId(), groupId ),
        jobContext,
        [=]( const znet_multichannel_association_report_t* report ) {
            m_groupsMaxNodesCount.push_back( report->max_end_pints_supported );

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

bool ZCc0x8eMultiChannelAssociation::requestNextGroup( WeakJobContext jobContext,
                                                       RequestValue::CallbackPtr cb,
                                                       uint8_t groupId ) {
    if( groupId > m_associationsCount ) {
        onRequestsFinished( cb, {} );
        return true;
    }

    return getParentNode().getRequestsManager().doRequest(
        MultiChannelAssociationRequest( getNodeId(), getChannelId(), groupId ),
        jobContext,
        [=]( const znet_multichannel_association_report_t* report ) {
            onGroupInfo( report );

            if( !requestNextGroup( jobContext, cb, groupId + 1 ) ) {
                onRequestsFinished( cb, RequestFailType::SendingError );
            }
        },
        std::bind( onRequestsFinished, cb, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

bool ZCc0x8eMultiChannelAssociation::setSisToLifelineGroup( WeakJobContext jobContext, RequestValue::CallbackPtr cb ) {
    static znet_endpoint_t selfNodeId;
    selfNodeId.node_id = znet_get_self_node_id();
    selfNodeId.channel_id = getVersion() >= 3 ? ZNET_CHANNEL_ID_ROOT : ZNET_CHANNEL_ID_INVALID;

    return getParentNode().getRequestsManager().doRequest(
        MultiChannelAssociationSet( getNodeId(), getChannelId(), 1, &selfNodeId, 1 ),
        jobContext,
        [=]() {
            getParentNode().getRequestsManager().doRequest(
                MultiChannelAssociationRequest( getNodeId(), getChannelId(), 1 ),
                jobContext,
                [=]( const znet_multichannel_association_report_t* report ) {
                    onGroupInfo( report );
                    completeInterviewProcess( cb );
                },
                std::bind( onRequestsFinished, cb, std::placeholders::_1 ),
                DefaultPostponedRequestDelay );
        },
        std::bind( onRequestsFinished, cb, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

void ZCc0x8eMultiChannelAssociation::completeInterviewProcess( RequestValue::CallbackPtr cb ) {
    znet::ZNode::ClassPtr cc =
        getParentChannel().getParentNode().getCommandClass( getChannelId(), ZNET_COMMAND_CLASS_ASSOCIATION );
    if( cc ) {
        cc->setInterviewDone( true );
    }

    setInterviewDone( true );
    onRequestsFinished( cb, {} );
}

}  // namespace znet
