#include "znet_nodes_processor.hpp"
#include "znet/znet_utils.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_command_class_values_setter.hpp"

extern "C" {
#include "znet_log.h"
}

#include <cassert>

namespace znet {

namespace {

// TODO: implement s2 multicast and change criterica
bool canSendChipSinglecast( ZNode& node ) {
    return node.getSecurity() == SecurityMode::Class::None;
}

}  // namespace

class CommandClassValuesSetter::MulticastNodesProcessor : public NodesProcessor {
public:
    MulticastNodesProcessor( CommandClassValuesSetter& owner,
                             znet_command_class_t classId,
                             znet_node_channel_id_t channelId,
                             CommandClassValue value,
                             bool sendSequentSinglecasts )
    : NodesProcessor( owner.m_nodesProvider,
                      owner.m_timerProvider,
                      DefaultTimeout,
                      JobPriorityOpt( JobPriority(), false ) )
    , m_owner( owner )
    , m_classId( classId )
    , m_channelId( channelId )
    , m_value( std::move( value ) )
    , m_sendSequentSinglecasts( sendSequentSinglecasts ) {}

    bool processNode( ZNode& currentNode ) override {
        const auto nodeId = currentNode.getId();
        auto cc = currentNode.getCommandClass( m_channelId, m_classId );
        if( !cc ) {
            ZNET_LOGW( "CommandClassValuesSetter: cannot start request, cc %u not found, node %u, ch %u\n",
                       m_classId,
                       nodeId,
                       m_channelId );
            return false;
        }

        // TODO setValueImpl was removed because the set value is sent along with the multicast by the chip itself.
        // It is necessary to implement the ability to send a set value after multicast in two ways.

        RequestValue request;

        request.resultCallback = [this, nodeId]( RequestFailTypeOpt failTypeOpt ) {
            if( failTypeOpt ) {
                ZNET_LOGW( "CommandClassValuesSetter: request value error, node %u, error %u\n", nodeId, *failTypeOpt );
            }
            onNodeProcessed( failTypeOpt ? err::message( ZNET_ERR_MVALUES_GETTING, *failTypeOpt ) : ErrorOpt{} );
        };
        request.jobContext = getJobContext();

        if( m_sendSequentSinglecasts || !canSendChipSinglecast( currentNode ) ) {
            m_owner.setValueImpl( *cc, m_value );
        }

        const auto result = cc->requestValue( request );

        if( !result ) {
            ZNET_LOGW( "CommandClassValuesSetter: cannot start values request for node %u\n", nodeId );
        }

        return result;
    }

private:
    CommandClassValuesSetter& m_owner;
    znet_command_class_t m_classId;
    znet_node_channel_id_t m_channelId;
    CommandClassValue m_value;
    bool m_sendSequentSinglecasts;
};

CommandClassValuesSetter::CommandClassValuesSetter( INodesProvider& nodesProvider, ITimerProvider& timerProvider )
: m_nodesProvider( nodesProvider )
, m_timerProvider( timerProvider ) {}

CommandClassValuesSetter::~CommandClassValuesSetter() = default;

void CommandClassValuesSetter::setValueMulticast( uint8_t nodesCount,
                                                  const znet_node_id_t* nodeIds,
                                                  znet_node_channel_id_t channelId,
                                                  znet_command_class_t classId,
                                                  CommandClassValue value,
                                                  MulticastMode multicastMode ) {
    NodeIdsSet nodeIdsSet;

    bool sendChipMulticasts = false;

    for( uint8_t index = 0; index < nodesCount; ++index ) {
        const auto nodeId = nodeIds[index];
        auto node = m_nodesProvider.onFindNode( nodeId );

        if( !node ) {
            ZNET_LOGE( "cannot find node %u for multicast\n", nodeId );
            continue;
        }

        auto cc = node->getCommandClass( channelId, classId );

        if( !cc ) {
            ZNET_LOGE( "cannot find cc %u in node %u, channel %u for multicast\n", classId, nodeId, channelId );
            continue;
        }

        sendChipMulticasts = sendChipMulticasts || canSendChipSinglecast( *node );

        utils::setBit( nodeIdsSet.mask, nodeId );
    }

    const auto firstNodeOpt = utils::findFirstBit( nodeIdsSet.mask, NodesProcessor::NodeIdsSet::Size );
    if( !firstNodeOpt ) {
        ZNET_LOGE( "no nodes for multicast, cc %u\n", classId );
        return;
    }
    auto node = m_nodesProvider.onFindNode( *firstNodeOpt );

    if( !node ) {
        ZNET_LOGE( "cannot find node %u for multicast\n", *firstNodeOpt );
        return;
    }

    auto cc = node->getCommandClass( channelId, classId );

    sendChipMulticasts = sendChipMulticasts && ( multicastMode == MulticastMode::ChipSinglecasts ||
                                                 multicastMode == MulticastMode::NoValuesGetting );

    setValueMulticastImpl( *cc, nodesCount, nodeIds, value, sendChipMulticasts );

    removeProcessingNodes( nodeIdsSet, classId, channelId );

    if( multicastMode == MulticastMode::NoValuesGetting ) {
        utils::forEachBit( nodeIdsSet.mask, nodeIdsSet.Size, [&]( znet_node_id_t nodeId ) {
            auto node = m_nodesProvider.onFindNode( nodeId );
            assert( node );
            if( !canSendChipSinglecast( *node ) ) {
                setValueImpl( *node->getCommandClass( channelId, classId ), value );
            }
        } );
    }
    else {
        NodesProcessorUPtr nodesProcessor(
            new MulticastNodesProcessor( *this, classId, channelId, std::move( value ), !sendChipMulticasts ) );
        nodesProcessor->addNodesToProcess( nodeIdsSet );
        nodesProcessor->startNodesProcessing();

        m_nodesProcessors.emplace( std::make_pair( classId, channelId ), std::move( nodesProcessor ) );
    }
}

void CommandClassValuesSetter::setValue( znet_node_id_t nodeId,
                                         znet_node_channel_id_t channelId,
                                         znet_command_class_t classId,
                                         CommandClassValue value ) {
    auto node = m_nodesProvider.onFindNode( nodeId );

    if( !node ) {
        ZNET_LOGE( "no node for setValue, node %u\n", nodeId );
        return;
    }

    auto cc = node->getCommandClass( channelId, classId );

    if( !cc ) {
        ZNET_LOGE( "no cc for setValue, node id %u, cc %u\n", nodeId, classId );
        return;
    }

    NodeIdsSet nodeIds;
    utils::setBit( nodeIds.mask, nodeId );

    removeProcessingNodes( nodeIds, classId, channelId );

    setValueImpl( *cc, value );
}

void CommandClassValuesSetter::removeProcessingNodes( const NodeIdsSet& nodeIds,
                                                      znet_command_class_t classId,
                                                      znet_node_channel_id_t channelId ) {
    const auto range = m_nodesProcessors.equal_range( std::make_pair( classId, channelId ) );
    for( auto it = range.first; it != range.second; ) {
        it->second->removeNodesFromProcess( nodeIds );

        if( it->second->isInProgress() ) {
            ++it;
        }
        else {
            it = m_nodesProcessors.erase( it );
        }
    }
}

void CommandClassValuesSetter::setValueImpl( ZClass& cc, const CommandClassValue& value ) {
    visitors::visit( visitors::makeOverloads( [&cc]( uint8_t value ) { cc.setValue( value ); },
                                              [&cc]( const std::string& token ) { cc.setToken( token ); },
                                              [&cc]( ColorComponents colors ) { cc.setRgb( colors ); } ),
                     value );
}

void CommandClassValuesSetter::setValueMulticastImpl( ZClass& cc,
                                                      uint8_t nodesCount,
                                                      const znet_node_id_t* nodeIds,
                                                      const CommandClassValue& value,
                                                      bool sendSingleCasts ) {
    visitors::visit(
        visitors::makeOverloads(
            [&]( uint8_t value ) { cc.setValueMulticast( nodesCount, nodeIds, value, sendSingleCasts ); },
            [&]( const std::string& token ) { cc.setTokenMulticast( nodesCount, nodeIds, token, sendSingleCasts ); },
            [&]( ColorComponents colors ) { cc.setRgbMulticast( nodesCount, nodeIds, colors, sendSingleCasts ); } ),
        value );
}

}  // namespace znet
