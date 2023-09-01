#pragma once

#include "fake_requests_manager.h"
#include "fake_nodes_provider.h"
#include "fake_timer_provider.h"
#include "znet_node_accessor.h"

#include <znet/znet_command_class.hpp>

#include <gmock/gmock.h>

namespace znet {

namespace ut {

class NodeRequestsManagingFixture : public ZNodeAccessor {
public:
    NodeRequestsManagingFixture();

    FakeTimerProvider& getTimerProvider();

    FakeRequestsManager& getRequestsManager();

    FakeNodesProvider& getNodesProvider();

    static RequestValue createRequestValue(::testing::MockFunction<void( RequestFailTypeOpt )>& callback,
                                           WeakJobContext context = {} );

    template <typename CommandClass>
    std::shared_ptr<CommandClass> ensureNodeAndCc( znet_node_id_t nodeId,
                                                   znet_node_id_t channelId,
                                                   znet_command_class_version_t version ) {
        auto node = getNodesProvider().onFindNode( nodeId );
        if( !node ) {
            getNodesProvider().nodeCreate( nodeId );
            node = getNodesProvider().onFindNode( nodeId );
        }

        if( channelId >= node->getChannelsCount() ) {
            node->createAdditionalChannels( channelId - node->getChannelsCount() + 1 );
        }

        using ID = typename CommandClass::ID;
        node->addCommandClass( channelId, ID::value, version );
        return node->getCommandClass<CommandClass>( channelId );
    }

private:
    FakeTimerProvider m_timerProvider;
    FakeRequestsManager m_requestsManager;
    FakeNodesProvider m_nodesProvider;
};

}  // namespace ut
}  // namespace znet
