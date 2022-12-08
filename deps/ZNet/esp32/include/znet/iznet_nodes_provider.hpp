#pragma once

#include <znet/znet.h>

#include <vector>

namespace znet {

class ZNode;

class INodesProvider {
public:
    struct NodeIdsSet {
        constexpr static uint8_t Size = 256 / 8;
        uint8_t mask[Size] = {
            0,
        };
    };

    virtual ~INodesProvider() = default;

    virtual void nodeCreate( const uint8_t nodeId ) = 0;

    virtual void nodeRemove( const uint8_t nodeId ) = 0;

    virtual ZNode* onFindNode( znet_node_id_t nodeId ) = 0;

    virtual NodeIdsSet getNodeIds() const = 0;
};

}  // namespace znet
