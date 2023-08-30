/**
 * @file znet_cmd_multilevel_switch.c
 * @date 2 Mar 2018
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

// node cmd multilevel switch report ///////////////////////////////////////
/// INFO: Multilevel Switch Command Class v1

static void znet_cc_switch_multilevel_report_v3( const ZFunction func, uint8_t node_id, const uint8_t* cc_data ) {
    const ZW_SWITCH_MULTILEVEL_REPORT_V3_FRAME* data = (const ZW_SWITCH_MULTILEVEL_REPORT_V3_FRAME*)cc_data;

    assert( data );
    assert( data->cmdClass == ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL );
    assert( data->cmd == SWITCH_MULTILEVEL_REPORT );

    if( data->value > ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX && data->value != ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN &&
        data->value != ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX_ABSOLUTE )
        return;

    /// TODO: check in storage node_id
    ZNET_LOGD( "MLS: node(%d) sent MLS report!\n", node_id );
    if( znet_cb->node_cmd_multilevel_switch_result )
        znet_cb->node_cmd_multilevel_switch_result( ZNET_ERR_OK,
                                                    node_id,
                                                    func->_endpoint,
                                                    data->value,
                                                    ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN,
                                                    0,
                                                    znet_cb->arg );
}

static void znet_cc_switch_multilevel_report_v4( const ZFunction func, uint8_t node_id, const uint8_t* cc_data ) {
    const ZW_SWITCH_MULTILEVEL_REPORT_V4_FRAME* data = (const ZW_SWITCH_MULTILEVEL_REPORT_V4_FRAME*)cc_data;

    assert( data );
    assert( data->cmdClass == ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL );
    assert( data->cmd == SWITCH_MULTILEVEL_REPORT );

    if( data->currentValue > ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX &&
        data->currentValue != ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN &&
        data->currentValue != ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX_ABSOLUTE )
        return;

    const uint8_t targetValue = data->targetValue > ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX &&
                                        data->targetValue != ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX_ABSOLUTE ?
                                    ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN :
                                    data->targetValue;

    /// TODO: check in storage node_id
    ZNET_LOGD( "MLS: node(%d) sent MLS report!\n", node_id );
    if( znet_cb->node_cmd_multilevel_switch_result )
        znet_cb->node_cmd_multilevel_switch_result(
            ZNET_ERR_OK, node_id, func->_endpoint, data->currentValue, targetValue, data->duration, znet_cb->arg );
}

void znet_cc_switch_multilevel_report( const ZFunction func,
                                       uint8_t node_id,
                                       int cc_data_len,
                                       const uint8_t* cc_data ) {
    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    switch( cc_data_len ) {
    case sizeof( ZW_SWITCH_MULTILEVEL_REPORT_V3_FRAME ):
        znet_cc_switch_multilevel_report_v3( func, node_id, cc_data );
        break;

    case sizeof( ZW_SWITCH_MULTILEVEL_REPORT_V4_FRAME ):
        znet_cc_switch_multilevel_report_v4( func, node_id, cc_data );
        break;

    default:
        assert( 0 );
    }
}

// node cmd multilevel switch get //////////////////////////////////////////
/// INFO: Multilevel Switch Command Class v1

static void _znet_node_cmd_multilevel_switch_get_cb( ZFunction func,
                                                     void* arg,
                                                     ZFuncFailures_e reason,
                                                     const void* dataPtr,
                                                     size_t dataSize ) {
    if( znet_cb->node_cmd_multilevel_switch_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_multilevel_switch_result( znet_get_error( reason ),
                                                    nodeId,
                                                    endpoint,
                                                    ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN,
                                                    ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN,
                                                    0,
                                                    znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_multilevel_switch_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }
    /// TODO: check wait report for node_id

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_switch_multilevel_get( node_id, channel_id, _znet_node_cmd_multilevel_switch_get_cb, NULL, encap );
}

// node cmd meter reset ////////////////////////////////////////////////
/// INFO: Multilevel Switch Command Class v1

static void _znet_node_cmd_multilevel_switch_set_cb( ZFunction func,
                                                     void* arg,
                                                     ZFuncFailures_e reason,
                                                     const void* dataPtr,
                                                     size_t dataSize ) {
    if( znet_cb->node_cmd_multilevel_switch_set_done ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_multilevel_switch_set_done( znet_get_error( reason ), nodeId, endpoint, znet_cb->arg );
    }
}

// node cmd multilevel switch set //////////////////////////////////////////
/// INFO: Multilevel Switch Command Class v1

znet_error_t znet_node_cmd_multilevel_switch_set( znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id,
                                                  znet_cmd_multilevel_switch_value_t value,
                                                  uint8_t duration_available,
                                                  uint8_t duration ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    /// TODO: investigate, need support report

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( value > ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX + 1 && value != ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX_ABSOLUTE ) {
        return ZNET_ERR_INVAL;
    }
    else if( value == ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX + 1 ) {
        value = ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_switch_multilevel_set( node_id,
                                          channel_id,
                                          value,
                                          duration_available,
                                          duration,
                                          _znet_node_cmd_multilevel_switch_set_cb,
                                          NULL,
                                          encap );
}

//  multilevel switch set multicast //////////////////////////////////////////
/// INFO: Multilevel Switch Command Class v1

znet_error_t znet_node_cmd_multilevel_switch_set_multi( uint8_t nodeQuantity,
                                                        const znet_node_id_t* listNodeId,
                                                        znet_node_channel_id_t channel_id,
                                                        znet_cmd_multilevel_switch_value_t value,
                                                        uint8_t duration_available,
                                                        uint8_t duration,
                                                        bool send_single_casts ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    for( uint8_t i = 0; i < nodeQuantity; ++i )
        if( listNodeId[i] < ZNET_NODE_ID_MIN || listNodeId[i] > ZNET_NODE_ID_MAX ) {
            return ZNET_ERR_INVAL;
        }

    if( value > ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX + 1 && value != ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX_ABSOLUTE ) {
        return ZNET_ERR_INVAL;
    }
    else if( value == ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX + 1 ) {
        value = ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX;
    }

    uint8_t data[] = { COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_SET, value, duration };

    return znet_fc_send_data_multi( nodeQuantity,
                                    listNodeId,
                                    ( ( sizeof data ) - ( duration_available ? 0 : 1 ) ),
                                    data,
                                    send_single_casts,
                                    NULL,
                                    NULL );
}

// node cmd multilevel switch start change /////////////////////////////
/// INFO: Multilevel Switch Command Class v1

znet_error_t znet_node_cmd_multilevel_switch_start_change( znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           znet_cmd_multilevel_switch_change_direction_t direction,
                                                           znet_cmd_multilevel_switch_value_t start_value,
                                                           uint8_t duration,
                                                           uint8_t version ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    /// TODO: investigate, need support report

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( start_value > ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX &&
        start_value != ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN /*&&
        start_value != ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX_ABSOLUTE [deprecated] */ ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    ZNET_LOGD( "MLS: sent start_level_change command to node(%d0)\n", node_id );
    return znet_cc_switch_multilevel_start_level_change( node_id,
                                                         channel_id,
                                                         (uint8_t)direction,
                                                         ( start_value == ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN ),
                                                         start_value,
                                                         version > 1,
                                                         duration,
                                                         version > 2,
                                                         0 /*TODO*/,
                                                         ZNET_CMD_MULTILEVEL_SWITCH_NO_CHANGE,
                                                         NULL,
                                                         NULL,
                                                         encap );
}

// node cmd multilevel switch stop change /////////////////////////////
/// INFO: Multilevel Switch Command Class v1

znet_error_t znet_node_cmd_multilevel_switch_stop_change( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    /// TODO: investigate, need support report

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_switch_multilevel_stop_level_change( node_id, channel_id, NULL, NULL, encap );
}

// node cmd multilevel switch supported get ///////////////////////////
/// INFO: Central Multilevel Switch Command  Class v3
znet_error_t znet_node_cmd_multilevel_switch_supported_get( znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_switch_multilevel_supported_get( node_id, channel_id, NULL, NULL, encap );
}

// node cmd switch_multilevel supported report /////////////////////////////////////
/// INFO: Meter Command Class v3..v4

void znet_cc_switch_multilevel_supported_report( const ZFunction func,
                                                 uint8_t node_id,
                                                 int cc_data_len,
                                                 const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL );
    assert( cc_data[1] == SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    znet_multilevel_switch_supported_report_t report = {};
    report.primary_type = cc_data[2] & SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES1_PRIMARY_SWITCH_TYPE_MASK_V3;
    report.secondary_type = cc_data[3] & SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES2_SECONDARY_SWITCH_TYPE_MASK_V3;

    if( znet_cb->node_cmd_multilevel_switch_supported_result )
        znet_cb->node_cmd_multilevel_switch_supported_result(
            ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}
////////////////////////////////////////////////////////////////////////
