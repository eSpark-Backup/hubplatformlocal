/**
 * @file znet_cmd_binary_switch.c
 * @date 30 Mar 2018
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
#include <znet_lib_fc.h>
#include <znet_lib_cc_application.h>

// node cmd binary switch report ///////////////////////////////////////
/// INFO: Binary Switch Command Class v1

void znet_cc_switch_binary_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SWITCH_BINARY );
    assert( cc_data[1] == SWITCH_BINARY_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( cc_data_len <= 2 ) {
        ZNET_LOGW( "znet_cc_switch_binary_report: incorrect data length %u\n", cc_data_len );
    }

    const uint8_t value = cc_data_len > 2 ? cc_data[2] : 0;

    const uint8_t target_value = cc_data_len > 3 ? cc_data[3] : ZNET_CMD_BINARY_SWITCH_VALUE_UNKNOWN;
    const uint8_t duration = cc_data_len > 4 ? cc_data[4] : 0;

    /// TODO: check in storage node_id

    if( znet_cb->node_cmd_binary_switch_result )
        znet_cb->node_cmd_binary_switch_result(
            ZNET_ERR_OK, node_id, func->_endpoint, value, target_value, duration, znet_cb->arg );
}

// node cmd binary switch get //////////////////////////////////////////

static void _znet_node_cmd_binary_switch_get_cb( ZFunction func,
                                                 void* arg,
                                                 ZFuncFailures_e reason,
                                                 const void* dataPtr,
                                                 size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    znet_cb->node_cmd_binary_switch_result( znet_get_error( reason ),
                                            nodeId,
                                            endpoint,
                                            ZNET_CMD_BINARY_SWITCH_VALUE_UNKNOWN,
                                            ZNET_CMD_BINARY_SWITCH_VALUE_UNKNOWN,
                                            0,
                                            znet_cb->arg );
}

znet_error_t znet_node_cmd_binary_switch_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_switch_binary_get( node_id, channel_id, _znet_node_cmd_binary_switch_get_cb, NULL, encap );
}

// node cmd binary switch set //////////////////////////////////////////
/// INFO: Binary Switch Command Class v1

znet_error_t znet_node_cmd_binary_switch_set( znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              znet_cmd_binary_switch_value_t value ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    /// TODO: investigate, need support report

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( value > ZNET_CMD_BINARY_SWITCH_VALUE_MAX && value != ZNET_CMD_BINARY_SWITCH_VALUE_MAX_ABSOLUTE ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_switch_binary_set( node_id, channel_id, value, 0xFF, NULL, NULL, encap );
}

znet_error_t znet_node_cmd_binary_switch_set_multi( uint8_t nodeQuantity,
                                                    const znet_node_id_t* listNodeId,
                                                    znet_node_channel_id_t channel_id,
                                                    znet_cmd_binary_switch_value_t value,
                                                    bool send_single_casts ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    for( uint8_t i = 0; i < nodeQuantity; ++i )
        if( listNodeId[i] < ZNET_NODE_ID_MIN || listNodeId[i] > ZNET_NODE_ID_MAX ) {
            return ZNET_ERR_INVAL;
        }

    if( value > ZNET_CMD_BINARY_SWITCH_VALUE_MAX && value != ZNET_CMD_BINARY_SWITCH_VALUE_MAX_ABSOLUTE ) {
        return ZNET_ERR_INVAL;
    }

    uint8_t data[] = { COMMAND_CLASS_SWITCH_BINARY, SWITCH_BINARY_SET, value ? 0xFF : 0, 0xFF /*duration*/ };

    return znet_fc_send_data_multi( nodeQuantity, listNodeId, ( sizeof data ), data, send_single_casts, NULL, NULL );
}

////////////////////////////////////////////////////////////////////////
