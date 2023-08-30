#pragma once

#include <znet/znet_node.hpp>

namespace znet {
namespace ut {

class ZNodeAccessor {
public:
    ZChannel& getNodeChannel( ZNode& node, znet_node_channel_id_t channelId );

    void eraseCommandClass( ZChannel::ClassPtr cc );
};

}  // namespace ut
}  // namespace znet
