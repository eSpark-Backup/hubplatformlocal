/**
 * @file znet_cmd_binary_switch.c
 * @date 2 May 2018
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

// node cmd barrier operator report ///////////////////////////////////////
/// INFO: Barrier Operator Command Class v1

void znet_cc_barrier_operator_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_BARRIER_OPERATOR );
    assert( cc_data[1] == BARRIER_OPERATOR_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    uint8_t value = cc_data[2];

    if( value >= ZNET_CMD_BARRIER_OPERATOR_VALUE_STOP_POSITION_L &&
        value <= ZNET_CMD_BARRIER_OPERATOR_VALUE_STOP_POSITION_H ) {
        value = BARRIER_OPERATOR_REPORT_STOPPED;
    }

    switch( value ) {
    case BARRIER_OPERATOR_REPORT_CLOSED:
    case BARRIER_OPERATOR_REPORT_CLOSING:
    case BARRIER_OPERATOR_REPORT_STOPPED:
    case BARRIER_OPERATOR_REPORT_OPENING:
    case BARRIER_OPERATOR_REPORT_OPEN:
        break;

    default:
        return;
    }

    ZNET_LOGD( "Barrier: node(%d) sent barrier report!\n", node_id );
    if( znet_cb->node_cmd_barrier_operator_result )
        znet_cb->node_cmd_barrier_operator_result( ZNET_ERR_OK, node_id, func->_endpoint, value, znet_cb->arg );
}

// node cmd barrier operator signal report ///////////////////////////////////////
/// INFO: Barrier Operator Command Class v1

void znet_cc_barrier_operator_signal_report( const ZFunction func,
                                             uint8_t node_id,
                                             int cc_data_len,
                                             const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_BARRIER_OPERATOR );
    assert( cc_data[1] == BARRIER_OPERATOR_SIGNAL_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    uint8_t signal_type = cc_data[2];
    uint8_t signal_state = cc_data[3];

    ZNET_LOGD( "Barrier: node(%d) sent barrier signal report!\n", node_id );
    if( znet_cb->node_cmd_barrier_operator_signal_result )
        znet_cb->node_cmd_barrier_operator_signal_result(
            ZNET_ERR_OK, node_id, func->_endpoint, signal_type, signal_state, znet_cb->arg );
}

// node cmd barrier operator supported signals report ///////////////////////////////////////
/// INFO: Barrier Operator Command Class v1

void znet_cc_barrier_operator_supported_signals_report( const ZFunction func,
                                                        uint8_t node_id,
                                                        int cc_data_len,
                                                        const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_BARRIER_OPERATOR );
    assert( cc_data[1] == BARRIER_OPERATOR_SIGNAL_SUPPORTED_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    ZNET_LOGD( "Barrier: node(%d) sent barrier supported signals report!\n", node_id );
    if( znet_cb->node_cmd_barrier_operator_supported_signals_result )
        znet_cb->node_cmd_barrier_operator_supported_signals_result(
            ZNET_ERR_OK, node_id, func->_endpoint, cc_data + 2, cc_data_len - 2, znet_cb->arg );
}

// node cmd barrier operator get ///////////////////////////////////////
/// INFO: Barrier Operator Command Class v1

static void _znet_node_cmd_barrier_operator_get_cb( ZFunction func,
                                                    void* arg,
                                                    ZFuncFailures_e reason,
                                                    const void* dataPtr,
                                                    size_t dataSize ) {
    if( znet_cb->node_cmd_barrier_operator_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_barrier_operator_result(
            znet_get_error( reason ), nodeId, endpoint, ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_barrier_operator_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_barrier_operator_get( node_id, channel_id, _znet_node_cmd_barrier_operator_get_cb, NULL, encap );
}

// node cmd barrier operator set ///////////////////////////////////////
/// INFO: Barrier Operator Command Class v1

static void _znet_node_cmd_barrier_operator_set_cb( ZFunction func,
                                                    void* arg,
                                                    ZFuncFailures_e reason,
                                                    const void* dataPtr,
                                                    size_t dataSize ) {
    if( znet_cb->node_cmd_barrier_operator_set_done ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_barrier_operator_set_done( znet_get_error( reason ), nodeId, endpoint, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_barrier_operator_set( znet_node_id_t node_id,
                                                 znet_node_channel_id_t channel_id,
                                                 znet_cmd_barrier_operator_value_t value ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( value != BARRIER_OPERATOR_SET_CLOSE && value != BARRIER_OPERATOR_SET_OPEN ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_barrier_operator_set(
        node_id, channel_id, value, _znet_node_cmd_barrier_operator_set_cb, NULL, encap );
}

// node cmd barrier operator set multi ///////////////////////////////////////
/// INFO: Barrier Operator Command Class v1

znet_error_t znet_node_cmd_barrier_operator_set_multi( uint8_t nodeQuantity,
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

    if( value != BARRIER_OPERATOR_SET_CLOSE && value != BARRIER_OPERATOR_SET_OPEN ) {
        return ZNET_ERR_INVAL;
    }

    uint8_t data[] = { COMMAND_CLASS_BARRIER_OPERATOR, BARRIER_OPERATOR_SET, value };
    return znet_fc_send_data_multi( nodeQuantity, listNodeId, ( sizeof data ), data, send_single_casts, NULL, NULL );
}

// node cmd barrier operator signal supported get ///////////////////////////////////////
/// INFO: Barrier Operator Command Class v1

static void _znet_node_cmd_barrier_operator_signal_supported_get_cb( ZFunction func,
                                                                     void* arg,
                                                                     ZFuncFailures_e reason,
                                                                     const void* dataPtr,
                                                                     size_t dataSize ) {
    if( znet_cb->node_cmd_barrier_operator_supported_signals_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_barrier_operator_supported_signals_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_barrier_operator_signal_supported_get( znet_node_id_t node_id,
                                                                  znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_barrier_operator_signal_supported_get(
        node_id, channel_id, _znet_node_cmd_barrier_operator_signal_supported_get_cb, NULL, encap );
}

// node cmd barrier operator signal get ///////////////////////////////////////
/// INFO: Barrier Operator Command Class v1

union barrier_operator_signal_param {
    void* data;
    uint8_t signal_id;
};

static void _znet_node_cmd_barrier_operator_signal_get_cb( ZFunction func,
                                                           void* arg,
                                                           ZFuncFailures_e reason,
                                                           const void* dataPtr,
                                                           size_t dataSize ) {
    if( znet_cb->node_cmd_barrier_operator_signal_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union barrier_operator_signal_param param;
        param.data = arg;

        znet_cb->node_cmd_barrier_operator_signal_result(
            znet_get_error( reason ), nodeId, endpoint, param.signal_id, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_barrier_operator_signal_get( znet_node_id_t node_id,
                                                        znet_node_channel_id_t channel_id,
                                                        znet_cmd_barrier_operator_signal_t signal_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union barrier_operator_signal_param param;
    param.data = NULL;
    param.signal_id = signal_id;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_barrier_operator_signal_get(
        node_id, channel_id, signal_id, _znet_node_cmd_barrier_operator_signal_get_cb, param.data, encap );
}

// node cmd barrier operator signal get ///////////////////////////////////////
/// INFO: Barrier Operator Command Class v1

znet_error_t znet_node_cmd_barrier_operator_signal_set( znet_node_id_t node_id,
                                                        znet_node_channel_id_t channel_id,
                                                        znet_cmd_barrier_operator_signal_t signal_id,
                                                        znet_cmd_barrier_operator_signal_state_t signal_state ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( signal_id != ZNET_CMD_BARRIER_OPERATOR_SIGNAL_SIREN && signal_id != ZNET_CMD_BARRIER_OPERATOR_SIGNAL_LIGHT ) {
        return ZNET_ERR_INVAL;
    }

    if( signal_state != BARRIER_OPERATOR_SIGNAL_SET_OFF && signal_state != BARRIER_OPERATOR_SIGNAL_SET_ON ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_barrier_operator_signal_set( node_id, channel_id, signal_id, signal_state, NULL, NULL, encap );
}
