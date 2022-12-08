#include "znet/cc_0x45_thermostat_fan_state.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x45ThermostatFanState::requestValue( const RequestValue& request ) {
    const auto result = requestValueImpl( *this, ThermostatFanStateRequest( getNodeId(), getChannelId() ), request );
    if( !result ) {
        ZNET_LOGE( "cc_0x45: fan state request failed, node %u\n", getNodeId() );
    }
    return result;
}

}  // namespace znet
