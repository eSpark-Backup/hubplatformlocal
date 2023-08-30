#include "zigbee/zigbee_node_manager.h"

#include "zigbee/resources/zigbee_resources.h"
#include "zigbee/common/zigbee_exceptions.h"

#include "json_utils/json_utils.h"
#include "log_utils/ezlo_log.h"
#include "database_client/idbclient.h"
#include "common_utils/cpp14.h"

namespace HubPlatform {
namespace Zigbee {

NodeManager::NodeManager( IDbClient& dbClient )
: m_dbClient( dbClient ) {
    const auto jsonNodesData = m_dbClient.findAll( Resources::DB::TableNames::Devices, {}, nullptr );
    for( auto& jsonNodeData : jsonNodesData ) {
        auto node = deserializeNode( jsonNodeData );
        assert( node );
        m_cachedNodes.emplace( node->getId(), std::move( node ) );
    }
}

ZigbeeNode* NodeManager::findNode( uint16_t nodeId ) {
    return NodeManager::findNodeImpl( *this, nodeId );
}

const ZigbeeNode* NodeManager::findNode( uint16_t nodeId ) const {
    return NodeManager::findNodeImpl( *this, nodeId );
}

template < typename Manager >
ZigbeeNode* NodeManager::findNodeImpl( Manager& nodeManager, uint16_t nodeId ) {
    {
        yamc::shared_lock< yamc::alternate::shared_mutex > sharedLocker( nodeManager.m_cacheMutex );
        auto cacheIt = nodeManager.m_cachedNodes.find( nodeId );
        if( cacheIt != nodeManager.m_cachedNodes.end() )
            return cacheIt->second.get();
    }

    const auto jsonNodeData = nodeManager.m_dbClient.findOne(
        Resources::DB::TableNames::Devices, JSON( Resources::DB::Fields::Id << std::to_string( nodeId ) ), nullptr );
    if( !jsonNodeData.empty() ) {
        auto node = deserializeNode( jsonNodeData );

        assert( node );

        std::unique_lock< yamc::alternate::shared_mutex > lock( nodeManager.m_cacheMutex );
        return nodeManager.m_cachedNodes.emplace( nodeId, std::move( node ) ).first->second.get();
    }

    LOGGER_TRACE( "fail to find a node with id: " + std::to_string( nodeId ) );
    return nullptr;
}

bool NodeManager::remove( uint16_t nodeId ) {
    const auto dev = JSON( Resources::DB::Fields::Id << std::to_string( nodeId ) );
    const auto joNode = m_dbClient.findOne( Resources::DB::TableNames::Devices, dev, nullptr );
    if( joNode.empty() )
        return false;

    m_dbClient.remove( Resources::DB::TableNames::Devices, dev );

    std::unique_lock< yamc::alternate::shared_mutex > lock( m_cacheMutex );
    m_cachedNodes.erase( nodeId );

    return true;
}

bool NodeManager::update( uint16_t nodeId ) {
    auto node = findNode( nodeId );
    if( !node )
        return false;

    m_dbClient.update( Resources::DB::TableNames::Devices,
                       JSON( Resources::DB::Fields::Id << std::to_string( nodeId ) ),
                       serializeNode( *node ),
                       false /*upsert*/,
                       true /*multi*/ );

    return true;
}

void NodeManager::reset() {
    m_dbClient.remove( Resources::DB::TableNames::Devices, JSON_EMPTY_OBJ );

    std::unique_lock< yamc::alternate::shared_mutex > lock( m_cacheMutex );
    m_cachedNodes.clear();
}

Json NodeManager::serializeNode( const ZigbeeNode& node ) {
    JsonObject jsonNodeData;
    jsonNodeData[ Resources::DB::Fields::Id ] = std::to_string( node.getId() );
    jsonNodeData[ Resources::DB::Fields::NodeData ] = node.toJson();
    return jsonNodeData;
}

std::unique_ptr< ZigbeeNode > NodeManager::deserializeNode( const Json& jsonNodeData ) {
    return std::make_unique< ZigbeeNode >( jsonNodeData );
}

}  // namespace Zigbee
}  // namespace HubPlatform
