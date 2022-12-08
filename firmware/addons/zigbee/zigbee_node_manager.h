#pragma once
#include "model/zigbee_node.h"

#include "base_utils/non_copyable.h"
#include "json_utils/json_fwd.h"

#include "tiny/include/yamc/yamc_shared_lock.hpp"
#include "tiny/include/yamc/alternate_shared_mutex.hpp"

#include <map>
#include <memory>

class IDbClient;

namespace HubPlatform {
namespace Zigbee {

class NodeManager final : private common::NonCopyable {
public:
    explicit NodeManager( IDbClient& dbClient );
    ~NodeManager() = default;

    ZigbeeNode* findNode( uint16_t nodeId );
    const ZigbeeNode* findNode( uint16_t nodeId ) const;

    bool remove( uint16_t nodeId );
    bool update( uint16_t nodeId );

    static Json serializeNode( const ZigbeeNode& node );
    static std::unique_ptr< ZigbeeNode > deserializeNode( const Json& jsonNode );

    void reset();

    template < typename Handler >
    void forEachNode( Handler&& handler ) const {
        yamc::shared_lock< yamc::alternate::shared_mutex > sharedLocker( m_cacheMutex );
        for( const auto& idToNode : m_cachedNodes ) {
            handler( const_cast< const ZigbeeNode& >( ( *idToNode.second ) ) );
        }
    }

private:
    template < typename Manager >
    static ZigbeeNode* findNodeImpl( Manager& nodeManager, uint16_t nodeId );

private:
    IDbClient& m_dbClient;

    mutable yamc::alternate::shared_mutex m_cacheMutex;
    mutable std::map< uint16_t, std::unique_ptr< ZigbeeNode > > m_cachedNodes;
};

}  // namespace Zigbee
}  // namespace HubPlatform
