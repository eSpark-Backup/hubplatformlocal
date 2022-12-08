/**
 * @file znet_cmd_thermostat_mode.c
 * @date 26 Sep 2018
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

// node cmd thermostat mode report ///////////////////////////////////////////////
/// INFO: Thermostat Mode Command Class v1..v5

void znet_cc_thermostat_mode_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_THERMOSTAT_MODE );
    assert( cc_data[1] == THERMOSTAT_MODE_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    size_t dcount = ( cc_data[2] >> 5 ) & 0x7;
    uint8_t mode = cc_data[2] & 0x1F;

    assert( cc_data_len >= 3 + dcount );

    const size_t buff_size = sizeof( znet_thermostat_mode_report_t ) + dcount;
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_thermostat_mode_report_t* report = (znet_thermostat_mode_report_t*)&buff;

    report->mode = mode;
    report->dcount = dcount;

    for( int i = 0; i < dcount; ++i ) {
        report->manuf_data[i] = cc_data[3 + i];
    }

    if( znet_cb->node_cmd_thermostat_mode_result )
        znet_cb->node_cmd_thermostat_mode_result( ZNET_ERR_OK, node_id, func->_endpoint, report, znet_cb->arg );
}

// node cmd thermostat mode get //////////////////////////////////////////////////
/// INFO: Thermostat Mode Command Class v1..v5

static void _znet_node_cmd_thermostat_mode_get_cb( ZFunction func,
                                                   void* arg,
                                                   ZFuncFailures_e reason,
                                                   const void* dataPtr,
                                                   size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_thermostat_mode_result )
        znet_cb->node_cmd_thermostat_mode_result( znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_thermostat_mode_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_thermostat_mode_get( node_id, channel_id, _znet_node_cmd_thermostat_mode_get_cb, NULL, encap );
}

// node cmd thermostat mode  set //////////////////////////////////////////
/// INFO: Thermostat Mode Command Class v1..v5

znet_error_t znet_node_cmd_thermostat_mode_set( znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                znet_cmd_thermostat_mode_value_t value,
                                                const uint8_t* data,
                                                uint8_t data_len ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    /// TODO: investigate, need support report

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_thermostat_mode_set(
        node_id, channel_id, value, data ? true : false, data ? data_len : 0, data, NULL, NULL, encap );
}

void znet_cc_thermostat_mode_supported_report( const ZFunction func,
                                               uint8_t node_id,
                                               int cc_data_len,
                                               const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_THERMOSTAT_MODE );
    assert( cc_data[1] == THERMOSTAT_MODE_SUPPORTED_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_thermostat_mode_supported_result ) {
        znet_thermostat_mode_supported_report_t report = {};
        report.mcount = ( uint8_t )( cc_data_len - 2 );
        report.bit_mask = cc_data + 2;

        znet_cb->node_cmd_thermostat_mode_supported_result(
            ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
    }
}

static void _znet_node_cmd_thermostat_mode_supported_get_cb( ZFunction func,
                                                             void* arg,
                                                             ZFuncFailures_e reason,
                                                             const void* dataPtr,
                                                             size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_thermostat_mode_supported_result )
        znet_cb->node_cmd_thermostat_mode_supported_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
}

// node cmd thermostat mode supported get ////////////////////////////////////////
/// INFO: Thermostat Mode Command Class v2..v5

znet_error_t znet_node_cmd_thermostat_mode_supported_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_thermostat_mode_supported_get(
        node_id, channel_id, _znet_node_cmd_thermostat_mode_supported_get_cb, NULL, encap );
}

////////////////////////////////////////////////////////////////////////
