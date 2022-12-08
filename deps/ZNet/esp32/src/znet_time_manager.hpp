#pragma once

#include <znet_nodes_processor.hpp>

namespace znet {

class IDateTimeProvider;
class ZNode;

class TimeManager : public NodesProcessor {
public:
    explicit TimeManager( IDateTimeProvider& dateTimeProvider,
                          INodesProvider& nodesProvider,
                          ITimerProvider& timerProvider );

    void onTimeRequested( znet_node_id_t nodeId, znet_node_channel_id_t channelId, uint8_t encap );
    void onDateRequested( znet_node_id_t nodeId, znet_node_channel_id_t channelId, uint8_t encap );
    void onTimeZoneRequested( znet_node_id_t nodeId, znet_node_channel_id_t channelId, uint8_t encap );
    void onTimeZoneSet( znet_node_id_t nodeId,
                        znet_node_channel_id_t channelId,
                        const znet_time_zone_settings_t& timeZoneSettings );

protected:
    bool processNode( ZNode& currentNode ) override;

private:
    bool requestClock( znet_node_channel_id_t prevChannelId = ZNET_CHANNEL_ID_INVALID );

    bool requestTimeParameters();

    bool requestScheduleTimeZone();

    bool requestTimeZone();

    bool requestDateTime();

    void onFail( const char* requestName, RequestFailType failType );

private:
    static constexpr znet_node_id_t InvalidNodeId = 0xFF;

    IDateTimeProvider& m_dateTimeProvider;

    std::unique_ptr<znet_time_zone_settings_t> m_setedTimeZone;

    uint8_t m_atteptsCount = 0;
    static constexpr uint8_t ms_maxAttemptsCount = 2;
};

}  // namespace znet
