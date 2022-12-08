#include <znet/znet_node.hpp>
#include <znet/znet_utils.hpp>

#include "fake_nodes_provider.h"

namespace znet {
namespace ut {

FakeNodesProvider::FakeNodesProvider( RequestsManager* requestsManager )
: m_requestsManager( requestsManager ) {}

void FakeNodesProvider::nodeCreate( const uint8_t nodeId ) {
    std::unique_ptr<ZNode> node( new ZNode( m_requestsManager, nullptr, &m_reachableCheckTimeoutSettings ) );
    node->init( nodeId );
    m_nodes.emplace( nodeId, std::move( node ) );
}

void FakeNodesProvider::nodeRemove( const uint8_t nodeId ) {
    m_nodes.erase( nodeId );
}

ZNode* FakeNodesProvider::onFindNode( znet_node_id_t nodeId ) {
    auto found = m_nodes.find( nodeId );

    if( found == m_nodes.end() ) {
        m_askedNullNodes.insert( nodeId );
    }

    return found != m_nodes.end() ? found->second.get() : nullptr;
}

FakeNodesProvider::NodeIdsSet FakeNodesProvider::getNodeIds() const {
    NodeIdsSet result;
    for( auto& idToNode : m_nodes ) {
        utils::setBit( result.mask, idToNode.first );
    }

    return result;
}

bool FakeNodesProvider::checkJobContexts( const std::unordered_set<znet_node_id_t>& expectedNodeIds ) {
    std::unordered_set<znet_node_id_t> actualIds;

    for( auto& idToNode : m_nodes ) {
        if( idToNode.second->getJobs().getJobContext() ) {
            actualIds.insert( idToNode.first );
        }
    }

    return actualIds == expectedNodeIds;
}

bool FakeNodesProvider::checkAskedNullNodes( const std::unordered_multiset<znet_node_id_t>& expectedAskedNullNodes ) {
    return m_askedNullNodes == expectedAskedNullNodes;
}

void FakeNodesProvider::setRequestsManager( RequestsManager* requestsManager ) {
    m_requestsManager = requestsManager;
}

std::vector<znet_command_class_t> FakeNodesProvider::getNodeCommandClassesIds( znet_node_id_t nodeId,
                                                                               znet_node_channel_id_t channelId ) {
    auto node = onFindNode( nodeId );
    std::vector<znet_command_class_t> result;

    if( !node ) {
        return {};
    }

    if( node->getChannelsCount() <= channelId ) {
        return {};
    }

    result.reserve( node->getCommandClassCount( channelId ) );

    for( uint8_t i = 0; i < node->getCommandClassCount( channelId ); ++i ) {
        result.push_back( node->getCommandClassByIndex( channelId, i )->getId() );
    }

    return result;
}

}  // namespace ut
}  // namespace znet
