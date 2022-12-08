#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x8ATime final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_TIME>;
    using ZClass::ZClass;

public:
    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool requestDateAndTime( std::function<void( const znet_date_time_t& )> onSuccess,
                             std::function<void( RequestFailType )> onFail,
                             WeakJobContext context ) const;

    bool requestTimeZone( std::function<void( const znet_time_zone_settings_t& )> onSuccess,
                          std::function<void( RequestFailType )> onFail,
                          WeakJobContext context ) const;

    void setTimeZone( const znet_time_zone_settings_t& settings );

    static void sendTimeRespond( znet_node_id_t nodeId,
                                 znet_node_channel_id_t channelId,
                                 const znet_local_time_t& time,
                                 const uint8_t encap );

    static void sendDateRespond( znet_node_id_t nodeId,
                                 znet_node_channel_id_t channelId,
                                 const znet_local_date_t& date,
                                 const uint8_t encap );

    static void sendTimeZoneRespond( znet_node_id_t nodeId,
                                     znet_node_channel_id_t channelId,
                                     const znet_time_zone_settings_t& settings,
                                     const uint8_t encap );

    bool isTimeZoneSupported() const;

    ZNET_DEFINE_ACCEPT()
};

}  // namespace znet
