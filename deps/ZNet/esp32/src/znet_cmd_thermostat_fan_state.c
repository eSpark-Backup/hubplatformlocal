/// INFO: crt & system
#include <assert.h>
#include <string.h>

/// INFO: public
#include <znet/znet.h>

/// INFO: private
#include "znet_main.h"
#include "znet_log.h"
#include "znet_store.h"

/// INFO: internal
#include "heap.h"
#include <znet_lib.h>
#include <znet_lib_cc_application.h>

// node cmd Thermostat Fan State report ///////////////////////////////////////////////
/// INFO: Thermostat Fan State Command Class v1..v2
void znet_cc_thermostat_fan_state_report( const ZFunction func,
                                          uint8_t node_id,
                                          int cc_data_len,
                                          const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_THERMOSTAT_FAN_STATE );
    assert( cc_data[1] == THERMOSTAT_FAN_STATE_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( ( cc_data[2] & 0xF0 ) != 0 )
        return;

    if( znet_cb->node_cmd_thermostat_fan_state_result )
        znet_cb->node_cmd_thermostat_fan_state_result(
            ZNET_ERR_OK, node_id, func->_endpoint, cc_data[2], znet_cb->arg );
}

// node cmd Thermostat Fan State get //////////////////////////////////////////
/// INFO: Thermostat Fan State Command Class v1..v2
static void _znet_node_cmd_thermostat_fan_state_get_cb( ZFunction func,
                                                        void* arg,
                                                        ZFuncFailures_e reason,
                                                        const void* dataPtr,
                                                        size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_thermostat_fan_state_result )
        znet_cb->node_cmd_thermostat_fan_state_result( znet_get_error( reason ), nodeId, endpoint, 0, znet_cb->arg );
}

znet_error_t znet_node_cmd_thermostat_fan_state_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_thermostat_fan_state_get(
        node_id, channel_id, _znet_node_cmd_thermostat_fan_state_get_cb, NULL, encap );
}
