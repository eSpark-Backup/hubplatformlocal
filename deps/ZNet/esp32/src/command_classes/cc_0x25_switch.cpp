#include "znet/cc_0x25_switch.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x25Switch::setValue( const uint8_t value ) {
    ZNET_LOGI( "set Switch: value(%d), node %d\n", value, getNodeId() );

    const znet_error_t result =
        znet_node_cmd_binary_switch_set( getParentNode().getId(), getParentChannel().getId(), value );
    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "cc_0x%x: set: error: %i\n\n", getId(), result );
        return false;
    }
    return true;
}

bool ZCc0x25Switch::setValueMulticast( const uint8_t nodeQuantity,
                                       const znet_node_id_t* listNodeId,
                                       const uint8_t value,
                                       const bool sendSingleCasts ) {
    ZNET_LOGI( "set Switch Multicast: value(%d)\n", value );

    znet_error_t result = znet_node_cmd_binary_switch_set_multi(
        nodeQuantity, listNodeId, getParentChannel().getId(), value, sendSingleCasts );
    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "error: %i\n\n", result );
        return false;
    }
    return true;
}

bool ZCc0x25Switch::requestValue( const RequestValue& request ) {
    const bool result = requestValueImpl( *this, BinarySwitchValueRequest( getNodeId(), getChannelId() ), request );

    if( !result ) {
        ZNET_LOGE( "binary switch get error, node %u\n", getNodeId() );
    }
    return result;
}

}  // namespace znet
