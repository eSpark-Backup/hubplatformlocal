#pragma once

#include <znet/znet.h>
#include <znet/iznet_nodes_provider.hpp>
#include <znet/iznet_timer_provider.hpp>
#include <znet/znet_command_class_value.hpp>
#include <znet/znet_multicast_mode.hpp>

#include <memory>
#include <map>

namespace znet {

class NodesProcessor;
class ZClass;
using NodesProcessorUPtr = std::unique_ptr<NodesProcessor>;

class CommandClassValuesSetter {
public:
    class MulticastNodesProcessor;
    using ClassAndChannelIds = std::pair<znet_command_class_t, znet_node_channel_id_t>;
    using NodeIdsSet = INodesProvider::NodeIdsSet;

    CommandClassValuesSetter( INodesProvider& nodesProvider, ITimerProvider& timerProvider );

    ~CommandClassValuesSetter();

    void setValueMulticast( uint8_t nodesCount,
                            const znet_node_id_t* nodeIds,
                            znet_node_channel_id_t channelId,
                            znet_command_class_t classId,
                            CommandClassValue value,
                            MulticastMode multicastMode = MulticastMode::Default );

    void setValue( znet_node_id_t nodeId,
                   znet_node_channel_id_t channelId,
                   znet_command_class_t classId,
                   CommandClassValue value );

private:
    void removeProcessingNodes( const NodeIdsSet& nodeIds,
                                znet_command_class_t classId,
                                znet_node_channel_id_t channelId );

    void setValueImpl( ZClass& cc, const CommandClassValue& value );

    void setValueMulticastImpl( ZClass& cc,
                                uint8_t nodesCount,
                                const znet_node_id_t* nodeIds,
                                const CommandClassValue& value,
                                bool sendSingleCasts );

private:
    INodesProvider& m_nodesProvider;
    ITimerProvider& m_timerProvider;
    std::multimap<ClassAndChannelIds, NodesProcessorUPtr> m_nodesProcessors;
};

}  // namespace znet
