/**
 * @file znet_cmd_basic.c
 * @date 27 Mar 2018
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
#include <znet_lib_cc_application.h>
#include <znet_lib_fc.h>

// node cmd basic report ///////////////////////////////////////////////
/// INFO: Basic Command Class v1

static void znet_cc_basic_report_v1( const ZFunction func, uint8_t node_id, const uint8_t* cc_data ) {
    const ZW_BASIC_REPORT_FRAME* data = (const ZW_BASIC_REPORT_FRAME*)cc_data;

    assert( data );
    assert( data->cmdClass == ZNET_COMMAND_CLASS_BASIC );
    assert( data->cmd == BASIC_REPORT );

    if( data->value > ZNET_CMD_BASIC_VALUE_MAX && data->value != ZNET_CMD_BASIC_VALUE_UNKNOWN &&
        data->value != ZNET_CMD_BASIC_VALUE_MAX_ABSOLUTE )
        return;

    if( znet_cb->node_cmd_basic_result )
        znet_cb->node_cmd_basic_result(
            ZNET_ERR_OK, node_id, func->_endpoint, data->value, ZNET_CMD_BASIC_VALUE_UNKNOWN, 0, znet_cb->arg );
}

static void znet_cc_basic_report_v2( const ZFunction func, uint8_t node_id, const uint8_t* cc_data ) {
    const ZW_BASIC_REPORT_V2_FRAME* data = (const ZW_BASIC_REPORT_V2_FRAME*)cc_data;

    assert( data );
    assert( data->cmdClass == ZNET_COMMAND_CLASS_BASIC );
    assert( data->cmd == BASIC_REPORT );

    if( data->currentValue > ZNET_CMD_BASIC_VALUE_MAX && data->currentValue != ZNET_CMD_BASIC_VALUE_UNKNOWN &&
        data->currentValue != ZNET_CMD_BASIC_VALUE_MAX_ABSOLUTE )
        return;

    if( data->targetValue > ZNET_CMD_BASIC_VALUE_MAX && data->targetValue != ZNET_CMD_BASIC_VALUE_UNKNOWN &&
        data->targetValue != ZNET_CMD_BASIC_VALUE_MAX_ABSOLUTE )
        return;

    if( znet_cb->node_cmd_basic_result )
        znet_cb->node_cmd_basic_result( ZNET_ERR_OK,
                                        node_id,
                                        func->_endpoint,
                                        data->currentValue,
                                        data->targetValue,
                                        data->duration,
                                        znet_cb->arg );
}

void znet_cc_basic_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    switch( cc_data_len ) {
    case sizeof( ZW_BASIC_REPORT_FRAME ):
        znet_cc_basic_report_v1( func, node_id, cc_data );
        break;

    case sizeof( ZW_BASIC_REPORT_V2_FRAME ):
        znet_cc_basic_report_v2( func, node_id, cc_data );
        break;

    default:
        assert( 0 );
    }
}

// node cmd basic set receive ///////////////////////////////////////////////
//
void ICACHE_FLASH_ATTR znet_cc_basic_receive_set( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len == sizeof( ZW_BASIC_SET_FRAME ) );

    const ZW_BASIC_SET_FRAME* data = (const ZW_BASIC_SET_FRAME*)cc_data;

    assert( data->cmdClass == ZNET_COMMAND_CLASS_BASIC );
    assert( data->cmd == BASIC_SET );

    /// TODO: investigate, why?
    if( ( node_id == 1 ) )
        return;

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( data->value > ZNET_CMD_BASIC_VALUE_MAX && data->value != ZNET_CMD_BASIC_VALUE_UNKNOWN &&
        data->value != ZNET_CMD_BASIC_VALUE_MAX_ABSOLUTE )
        return;

    if( znet_cb->node_cmd_basic_set_result )
        znet_cb->node_cmd_basic_set_result(
            ZNET_ERR_OK, node_id, func->_endpoint, data->value, ZNET_CMD_BASIC_VALUE_UNKNOWN, 0, znet_cb->arg );
}

// node cmd basic get //////////////////////////////////////////////////
/// INFO: Basic Command Class v1

static void _znet_node_cmd_basic_get_cb( ZFunction func,
                                         void* arg,
                                         ZFuncFailures_e reason,
                                         const void* dataPtr,
                                         size_t dataSize ) {
    if( znet_cb->node_cmd_basic_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_basic_result( znet_get_error( reason ),
                                        nodeId,
                                        endpoint,
                                        ZNET_CMD_BASIC_VALUE_UNKNOWN,
                                        ZNET_CMD_BASIC_VALUE_UNKNOWN,
                                        0,
                                        znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_basic_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    /// INFO: mandatory command class, not need check
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_basic_get( node_id, channel_id, _znet_node_cmd_basic_get_cb, NULL, encap );
}

// node cmd basic set //////////////////////////////////////////////////
/// INFO: Basic Command Class v1

znet_error_t znet_node_cmd_basic_set( znet_node_id_t node_id,
                                      znet_node_channel_id_t channel_id,
                                      znet_cmd_basic_value_t value ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    /// TODO: investigate, need support report

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( value > ZNET_CMD_BASIC_VALUE_MAX && value != ZNET_CMD_BASIC_VALUE_MAX_ABSOLUTE ) {
        return ZNET_ERR_INVAL;
    }

    /// INFO: mandatory command class, not need check
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_basic_set( node_id, channel_id, value, NULL, NULL, encap );
}

// node cmd basic set multi //////////////////////////////////////////////////
/// INFO: Basic Command Class v1

znet_error_t znet_node_cmd_basic_set_multi( uint8_t nodeQuantity,
                                            const znet_node_id_t* listNodeId,
                                            znet_node_channel_id_t channel_id,
                                            znet_cmd_basic_value_t value,
                                            bool send_single_casts ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    for( uint8_t i = 0; i < nodeQuantity; ++i )
        if( listNodeId[i] < ZNET_NODE_ID_MIN || listNodeId[i] > ZNET_NODE_ID_MAX ) {
            return ZNET_ERR_INVAL;
        }

    if( value > ZNET_CMD_BASIC_VALUE_MAX && value != ZNET_CMD_BASIC_VALUE_MAX_ABSOLUTE ) {
        return ZNET_ERR_INVAL;
    }

    uint8_t data[] = { COMMAND_CLASS_BASIC, BASIC_SET, value };
    return znet_fc_send_data_multi( nodeQuantity, listNodeId, ( sizeof data ), data, send_single_casts, NULL, NULL );
}

////////////////////////////////////////////////////////////////////////
