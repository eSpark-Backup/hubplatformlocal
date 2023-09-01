#include "znet/cc_0x47_thermostat_setback.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/converters/thermostat_setback_converter.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x47ThermostatSetback::setSetback( thermostat_setback::OverrideType type,
                                           thermostat_setback::Mode mode,
                                           thermostat_setback::SetbackValueOpt value ) {
    return ZNET_ERR_OK == znet_node_cmd_thermostat_setback_set( getNodeId(),
                                                                getChannelId(),
                                                                static_cast<uint8_t>( type ),
                                                                thermostat_setback::toRawValue( mode, value ) );
}

bool ZCc0x47ThermostatSetback::requestValue( const RequestValue& request ) {
    const auto result = requestValueImpl( *this, ThermostatSetbackRequest( getNodeId(), getChannelId() ), request );
    if( !result ) {
        ZNET_LOGE( "cc_0x45: fan setback request failed, node %u\n", getNodeId() );
    }

    return result;
}

}  // namespace znet
