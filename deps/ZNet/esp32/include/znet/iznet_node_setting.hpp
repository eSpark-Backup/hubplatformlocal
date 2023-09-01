#pragma once

#include <znet/znet.h>
#include <znet/optional.hpp>

#include <cstdint>
#include <functional>

namespace znet {

struct Configuration {
    uint8_t paramNumber;
    znet_cmd_configuration_value_t value;
    uint8_t size = 4;
};

struct Association {
    znet_node_channel_id_t channelId = ZNET_CHANNEL_ID_ROOT;
    znet_grouping_id_t groupId;
    znet_node_channel_id_t targetChannelId = ZNET_CHANNEL_ID_INVALID;
};

class INodeSetting {
public:
    virtual ~INodeSetting() = default;

    virtual void forEachConfiguration( std::function<void( const Configuration& )> callback ) const = 0;
    virtual void forEachAssociation( std::function<void( const Association& )> callback ) const = 0;
    virtual utils::optional<uint32_t> getWakeUpInterval() const = 0;
    virtual utils::optional<bool> shouldRequestNotifications() const = 0;
    virtual bool ignoreCcInterciew( uint8_t ccId ) const = 0;
    virtual utils::optional<bool> isExtendedUserCodesRequestRestricted() const = 0;
};

}  // namespace znet
