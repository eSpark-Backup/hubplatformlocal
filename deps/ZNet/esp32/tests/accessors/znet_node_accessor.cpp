#include "znet_node_accessor.h"

#include <znet/znet_node.hpp>

#include <algorithm>

namespace znet {
namespace ut {

ZChannel& ZNodeAccessor::getNodeChannel( ZNode& node, znet_node_channel_id_t channelId ) {
    if( channelId >= node.m_nodeChannels.size() ) {
        throw std::logic_error( "Wrong channel id" );
    }

    return node.m_nodeChannels[channelId];
}

void ZNodeAccessor::eraseCommandClass( ZChannel::ClassPtr cc ) {
    auto& commandClasses = cc->getParentChannel().m_channelClasses;
    commandClasses.erase( std::find( commandClasses.begin(), commandClasses.end(), cc ) );
}

}  // namespace ut
}  // namespace znet
