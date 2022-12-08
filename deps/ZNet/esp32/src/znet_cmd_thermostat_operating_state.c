/**
 * @file znet_cmd_thermostat_operating_state.c
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

// node cmd thermostat operating state report ///////////////////////////////////
/// INFO: Thermostat Operating State Command Class v1..v2

void znet_cc_thermostat_operating_state_report( const ZFunction func,
                                                uint8_t node_id,
                                                int cc_data_len,
                                                const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_THERMOSTAT_OPERATING_STATE );
    assert( cc_data[1] == THERMOSTAT_OPERATING_STATE_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_thermostat_operating_state_result )
        znet_cb->node_cmd_thermostat_operating_state_result(
            ZNET_ERR_OK,
            node_id,
            func->_endpoint,
            cc_data[2] & THERMOSTAT_OPERATING_STATE_REPORT_LEVEL_OPERATING_STATE_MASK,
            znet_cb->arg );
}

// node cmd thermostat operating state get //////////////////////////////////////
/// INFO: Thermostat Operating State Command Class v1..v2

static void _znet_node_cmd_thermostat_operating_state_get_cb( ZFunction func,
                                                              void* arg,
                                                              ZFuncFailures_e reason,
                                                              const void* dataPtr,
                                                              size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_thermostat_operating_state_result )
        znet_cb->node_cmd_thermostat_operating_state_result(
            znet_get_error( reason ), nodeId, endpoint, 0, znet_cb->arg );
}

znet_error_t znet_node_cmd_thermostat_operating_state_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_thermostat_operating_state_get(
        node_id, channel_id, _znet_node_cmd_thermostat_operating_state_get_cb, NULL, encap );
}

// node cmd thermostat operating state logging supported report /////////////////
/// INFO: Thermostat Operating State Command Class v2

void znet_cc_thermostat_operating_state_logging_supported_report( const ZFunction func,
                                                                  uint8_t node_id,
                                                                  int cc_data_len,
                                                                  const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_THERMOSTAT_OPERATING_STATE );
    assert( cc_data[1] == THERMOSTAT_OPERATING_LOGGING_SUPPORTED_REPORT_V2 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_thermostat_operating_state_logging_supported_result ) {
        znet_thermostat_operating_state_logging_supported_report_t report = {};
        report.opscount = ( uint8_t )( cc_data_len - 2 );
        report.bit_mask = cc_data + 2;

        znet_cb->node_cmd_thermostat_operating_state_logging_supported_result(
            ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
    }
}

static void _znet_node_cmd_thermostat_operating_state_logging_supported_get_cb( ZFunction func,
                                                                                void* arg,
                                                                                ZFuncFailures_e reason,
                                                                                const void* dataPtr,
                                                                                size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_thermostat_operating_state_logging_supported_result )
        znet_cb->node_cmd_thermostat_operating_state_logging_supported_result(
            znet_get_error( reason ), nodeId, endpoint, 0, znet_cb->arg );
}

// node cmd thermostat operating state logging supported get ////////////////////
/// INFO: Thermostat Operating State Command Class v2..v5

znet_error_t znet_node_cmd_thermostat_operating_state_logging_supported_get( znet_node_id_t node_id,
                                                                             znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_thermostat_operating_state_logging_supported_get(
        node_id, channel_id, _znet_node_cmd_thermostat_operating_state_logging_supported_get_cb, NULL, encap );
}

// node cmd thermostat operating state logging report ///////////////////////////
/// INFO: Thermostat Operating State Command Class v2

void znet_cc_thermostat_operating_state_logging_report( const ZFunction func,
                                                        uint8_t node_id,
                                                        int cc_data_len,
                                                        const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_THERMOSTAT_OPERATING_STATE );
    assert( cc_data[1] == THERMOSTAT_OPERATING_STATE_LOGGING_REPORT_V2 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const size_t buff_size = sizeof( znet_thermostat_operating_state_logging_report_t ) + cc_data_len - 1;
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_thermostat_operating_state_logging_report_t* report = (znet_thermostat_operating_state_logging_report_t*)&buff;

    report->data_count = ( uint16_t )( cc_data_len - 3 );

    for( int i = 0; i < cc_data_len - 3; ++i ) {
        report->data[i] = cc_data[3 + i];
    }

    if( znet_cb->node_cmd_thermostat_operating_state_logging_result ) {
        znet_cb->node_cmd_thermostat_operating_state_logging_result(
            ZNET_ERR_OK, node_id, func->_endpoint, report, znet_cb->arg );

        if( cc_data[2] == 0 )  // no more reports
        {
            znet_cb->node_cmd_thermostat_operating_state_logging_result(
                0, node_id, func->_endpoint, NULL, znet_cb->arg );
        }
    }
}

// node cmd thermostat mode supported get ///////////////////////////////////////
/// INFO: Thermostat Operating State Command Class v2
static void _znet_node_cmd_thermostat_operating_state_logging_get_cb( ZFunction func,
                                                                      void* arg,
                                                                      ZFuncFailures_e reason,
                                                                      const void* dataPtr,
                                                                      size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_thermostat_operating_state_logging_result )
        znet_cb->node_cmd_thermostat_operating_state_logging_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_thermostat_operating_state_logging_get( znet_node_id_t node_id,
                                                                   znet_node_channel_id_t channel_id,
                                                                   uint8_t datalen,
                                                                   const uint8_t* bit_masks ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_thermostat_operating_state_logging_get( node_id,
                                                           channel_id,
                                                           datalen,
                                                           bit_masks,
                                                           _znet_node_cmd_thermostat_operating_state_logging_get_cb,
                                                           NULL,
                                                           encap );
}

////////////////////////////////////////////////////////////////////////
