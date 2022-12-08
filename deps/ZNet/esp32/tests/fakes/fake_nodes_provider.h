#pragma once

#include <znet/iznet_nodes_provider.hpp>
#include <znet/znet_reachable_check_timeout_settings.hpp>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace znet {

class RequestsManager;

namespace ut {

class FakeNodesProvider : public virtual INodesProvider {
public:
    FakeNodesProvider( RequestsManager* requestsManager = nullptr );

    void nodeCreate( const uint8_t nodeId ) override;

    void nodeRemove( const uint8_t nodeId ) override;

    ZNode* onFindNode( znet_node_id_t nodeId ) override;

    NodeIdsSet getNodeIds() const override;

    bool checkJobContexts( const std::unordered_set<znet_node_id_t>& expectedNodeIds );
    bool checkAskedNullNodes( const std::unordered_multiset<znet_node_id_t>& expectedAskedNullNodes );

    void setRequestsManager( RequestsManager* requestsManager );

    std::vector<znet_command_class_t> getNodeCommandClassesIds( znet_node_id_t nodeId,
                                                                znet_node_channel_id_t channelId );

private:
    std::unordered_multiset<znet_node_id_t> m_askedNullNodes;
    std::unordered_map<znet_node_id_t, std::unique_ptr<ZNode> > m_nodes;
    RequestsManager* m_requestsManager;
    ReachableCheckTimeoutSettings m_reachableCheckTimeoutSettings;
};

}  // namespace ut
}  // namespace znet
