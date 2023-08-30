/**
 * @file znet_cmd_thermostat_setback.c
 * @date 5 Nov 2018
 * @brief TBD.
 */

/// INFO: crt & system
#include <assert.h>

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

// node cmd thermostat setback report ///////////////////////////////////////////////
/// INFO: Thermostat Setback Command Class v1

void znet_cc_thermostat_setback_report( const ZFunction func,
                                        uint8_t node_id,
                                        int cc_data_len,
                                        const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_THERMOSTAT_SETBACK );
    assert( cc_data[1] == THERMOSTAT_SETBACK_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const uint8_t type = cc_data[2] & 0x03;
    const int8_t state = (int8_t)cc_data[3];

    if( znet_cb->node_cmd_thermostat_setback_result )
        znet_cb->node_cmd_thermostat_setback_result( ZNET_ERR_OK, node_id, func->_endpoint, type, state, znet_cb->arg );
}

// node cmd thermostat setback get //////////////////////////////////////////////////
/// INFO: Thermostat Setback Command Class v1

static void _znet_node_cmd_thermostat_setback_get_cb( ZFunction func,
                                                      void* arg,
                                                      ZFuncFailures_e reason,
                                                      const void* dataPtr,
                                                      size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_thermostat_setback_result )
        znet_cb->node_cmd_thermostat_setback_result( znet_get_error( reason ), nodeId, endpoint, 0, 0, znet_cb->arg );
}

znet_error_t znet_node_cmd_thermostat_setback_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id
    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_thermostat_setback_get( node_id, channel_id, _znet_node_cmd_thermostat_setback_get_cb, NULL, encap );
}

// node cmd thermostat setback  set //////////////////////////////////////////
/// INFO: Thermostat Setback Command Class v1

znet_error_t znet_node_cmd_thermostat_setback_set( znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   uint8_t type,
                                                   int8_t state ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( type & 0xFC ) {
        return ZNET_ERR_INVAL;
    }
    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_thermostat_setback_set( node_id, channel_id, type, state, NULL, NULL, encap );
}

////////////////////////////////////////////////////////////////////////
