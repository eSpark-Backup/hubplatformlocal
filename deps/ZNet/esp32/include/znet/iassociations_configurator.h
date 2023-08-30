#pragma once

#include <znet/znet.h>

namespace znet {

class IAssociationsConfigurator {
public:
    virtual ~IAssociationsConfigurator() = default;

    virtual bool removeNodeFromGroup( znet_node_id_t nodeId,
                                      znet_node_channel_id_t channelId,
                                      znet_grouping_id_t groupId,
                                      znet_node_id_t targetNodeId,
                                      znet_node_channel_id_t targetChannelId ) = 0;

    virtual bool addNodeToGroup( znet_node_id_t nodeId,
                                 znet_node_channel_id_t channelId,
                                 znet_grouping_id_t groupId,
                                 znet_node_id_t targetNodeId,
                                 znet_node_channel_id_t targetChannelId,
                                 bool force = false ) = 0;

    virtual bool setupCommandClassAssociation( znet_node_id_t nodeId, znet_command_class_t cmdClass ) = 0;
};

}  // namespace znet
