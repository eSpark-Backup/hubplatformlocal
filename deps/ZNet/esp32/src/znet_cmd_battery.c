/**
 * @file znet_cmd_battery.c
 * @date 20 Aug 2018
 * @brief TBD.
 */

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
#include <znet_lib_cc_management.h>

// node cmd battery_level report ///////////////////////////////////////////////
/// INFO: Battery Command Class v1

void znet_cc_battery_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_BATTERY );
    assert( cc_data[1] == BATTERY_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( cc_data[2] > ZNET_CMD_BATTERY_LEVEL_MAX && cc_data[2] != ZNET_CMD_BATTERY_LEVEL_LOW_WARN )
        return;

    /// TODO: check in storage node_id

    if( znet_cb->node_cmd_battery_level_result )
        znet_cb->node_cmd_battery_level_result( ZNET_ERR_OK, node_id, func->_endpoint, cc_data[2], znet_cb->arg );
}

// node cmd battery_level get //////////////////////////////////////////////////
/// INFO: Battery Command Class v1
static void _znet_node_cmd_battery_level_get_cb( ZFunction func,
                                                 void* arg,
                                                 ZFuncFailures_e reason,
                                                 const void* dataPtr,
                                                 size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_battery_level_result )
        znet_cb->node_cmd_battery_level_result(
            znet_get_error( reason ), nodeId, endpoint, ZNET_CMD_BATTERY_LEVEL_UNKNOWN, znet_cb->arg );
}

znet_error_t znet_node_cmd_battery_level_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_battery_get( node_id, channel_id, _znet_node_cmd_battery_level_get_cb, NULL, encap );
}

////////////////////////////////////////////////////////////////////////
