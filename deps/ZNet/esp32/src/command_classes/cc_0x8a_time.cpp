#include "znet/cc_0x8a_time.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"

#include "znet/znet_request_value.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x8ATime::interview( InterviewCallback callback, WeakJobContext context ) {
    if( !getParentNode().getAssociationsConfigurator().setupCommandClassAssociation( getNodeId(),
                                                                                     ZNET_COMMAND_CLASS_TIME ) ) {
        ZNET_LOGW( "Setup time cc associations failed for node %d\n", getNodeId() );
    }

    return ZClass::interview( std::move( callback ), context );
}

bool ZCc0x8ATime::requestDateAndTime( std::function<void( const znet_date_time_t& )> onSuccess,
                                      std::function<void( RequestFailType )> onFail,
                                      WeakJobContext context ) const {
    return getParentNode().getRequestsManager().doRequest( LocalDateRequest( getNodeId(), getChannelId() ),
                                                           context,
                                                           [=]( const znet_local_date_t& date ) {
                                                               getParentNode().getRequestsManager().doRequest(
                                                                   LocalTimeRequest( getNodeId(), getChannelId() ),
                                                                   context,
                                                                   [=]( const znet_local_time_t& time ) {
                                                                       onSuccess( znet_date_time_t{ date, time } );
                                                                   },
                                                                   std::move( onFail ),
                                                                   DefaultPostponedRequestDelay );
                                                           },
                                                           onFail );
}

bool ZCc0x8ATime::requestTimeZone( std::function<void( const znet_time_zone_settings_t& )> onSuccess,
                                   std::function<void( RequestFailType )> onFail,
                                   WeakJobContext context ) const {
    return getParentNode().getRequestsManager().doRequest(
        TimeZoneRequest( getNodeId(), getChannelId() ), context, std::move( onSuccess ), std::move( onFail ) );
}

void ZCc0x8ATime::setTimeZone( const znet_time_zone_settings_t& settings ) {
    znet_node_cmd_time_zone_set( getNodeId(), getChannelId(), &settings );
}

void ZCc0x8ATime::sendTimeRespond( znet_node_id_t nodeId,
                                   znet_node_channel_id_t channelId,
                                   const znet_local_time_t& time,
                                   const uint8_t encap ) {
    znet_node_cmd_local_time_respond( nodeId, channelId, &time, encap );
}

void ZCc0x8ATime::sendDateRespond( znet_node_id_t nodeId,
                                   znet_node_channel_id_t channelId,
                                   const znet_local_date_t& date,
                                   const uint8_t encap ) {
    znet_node_cmd_local_date_respond( nodeId, channelId, &date, encap );
}

void ZCc0x8ATime::sendTimeZoneRespond( znet_node_id_t nodeId,
                                       znet_node_channel_id_t channelId,
                                       const znet_time_zone_settings_t& settings,
                                       const uint8_t encap ) {
    znet_node_cmd_time_zone_respond( nodeId, channelId, &settings, encap );
}

bool ZCc0x8ATime::isTimeZoneSupported() const {
    return getVersion() > 1;
}

}  // namespace znet
