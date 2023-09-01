/**
 * @file znet_cmd_basic.c
 * @date 27 Sep 2018
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

// node cmd color switch supported report ///////////////////////////////////////////////
/// INFO: Color Switch Command Class v1

void znet_cc_color_switch_supported_report( const ZFunction func,
                                            uint8_t node_id,
                                            int cc_data_len,
                                            const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SWITCH_COLOR );
    assert( cc_data[1] == SWITCH_COLOR_SUPPORTED_REPORT );

    if( znet_cb->node_cmd_color_switch_supported_result ) {
        znet_color_switch_supported_report_t report = {};
        report.component_mask_count = ( uint8_t )( cc_data_len - 2 );
        report.component_mask = cc_data + 2;

        znet_cb->node_cmd_color_switch_supported_result( ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
    }
}

// node cmd color switch supported get
/// INFO: Color Switch Command Class v1

static void _znet_node_cmd_color_switch_supported_get_cb( ZFunction func,
                                                          void* arg,
                                                          ZFuncFailures_e reason,
                                                          const void* dataPtr,
                                                          size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_color_switch_supported_result ) {
        znet_cb->node_cmd_color_switch_supported_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_color_switch_supported_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_color_switch_supported_get(
        node_id, channel_id, _znet_node_cmd_color_switch_supported_get_cb, NULL, encap );
}

// node cmd color switch report ///////////////////////////////////////////////
/// INFO: Color Switch Command Class v1

void znet_cc_color_switch_report( const ZFunction func,
                                  znet_node_id_t node_id,
                                  int cc_data_len,
                                  const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SWITCH_COLOR );
    assert( cc_data[1] == SWITCH_COLOR_REPORT );

    if( znet_cb->node_cmd_color_switch_result ) {
        znet_color_switch_report_t report = {};

        report.value = cc_data[3];
        report.component_id = cc_data[2];
        report.has_target_value = cc_data_len >= 6;

        if( report.has_target_value ) {
            report.target_value = cc_data[4];
            report.target_duration = cc_data[5];
        }
        else {
            report.target_duration = 0xFE;
        }

        znet_cb->node_cmd_color_switch_result(
            ZNET_ERR_OK, node_id, func->_endpoint, cc_data[2], &report, znet_cb->arg );
    }
}

// node cmd color switch get ///////////////////////////////////////////////
/// INFO: Color Switch Command Class v1

union color_switch_param {
    void* data;
    uint8_t component_id;
};

static void _znet_node_cmd_color_switch_get_cb( ZFunction func,
                                                void* arg,
                                                ZFuncFailures_e reason,
                                                const void* dataPtr,
                                                size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_color_switch_result ) {
        union color_switch_param param;
        param.data = arg;
        znet_cb->node_cmd_color_switch_result(
            znet_get_error( reason ), nodeId, endpoint, param.component_id, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_color_switch_get( znet_node_id_t node_id,
                                             znet_node_channel_id_t channel_id,
                                             uint8_t component_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union color_switch_param param;
    param.data = NULL;
    param.component_id = component_id;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_color_switch_get(
        node_id, channel_id, component_id, _znet_node_cmd_color_switch_get_cb, param.data, encap );
}

// node cmd color switch set
/// INFO: Color Switch Command Class v1

znet_error_t znet_node_cmd_color_switch_set( znet_node_id_t node_id,
                                             znet_node_channel_id_t channel_id,
                                             uint8_t cl_components_count,
                                             const znet_cl_component_t* components,
                                             uint8_t version ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_color_switch_set(
        node_id, channel_id, cl_components_count, (const uint8_t*)components, 0, NULL, NULL, encap, version );
}

// node cmd color switch set multi
/// INFO: Color Switch Command Class v1

znet_error_t znet_node_cmd_color_switch_set_multi( uint8_t nodeQuantity,
                                                   const znet_node_id_t* listNodeId,
                                                   znet_node_channel_id_t channel_id,
                                                   uint8_t cl_components_count,
                                                   const znet_cl_component_t* components,
                                                   bool send_single_casts ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    for( uint8_t i = 0; i < nodeQuantity; ++i )
        if( listNodeId[i] < ZNET_NODE_ID_MIN || listNodeId[i] > ZNET_NODE_ID_MAX ) {
            return ZNET_ERR_INVAL;
        }

    uint8_t header[] = { COMMAND_CLASS_SWITCH_COLOR, SWITCH_COLOR_SET, cl_components_count };
    uint8_t header_len = sizeof header;
    uint8_t data_len = header_len + cl_components_count * 2;
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );  //+1 for
    if( data == NULL )
        return FALSE;

    memcpy( &data[0], header, sizeof header );
    if( cl_components_count > 0 )
        memcpy( &data[header_len], components, cl_components_count * 2 );

    const znet_error_t result =
        znet_fc_send_data_multi( nodeQuantity, listNodeId, data_len, data, send_single_casts, NULL, NULL );
    Heap_Free( data );

    return result;
}

// node cmd color switch start level change
/// INFO: Color Switch Command Class v1

znet_error_t znet_node_cmd_color_switch_start_level_change( znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id,
                                                            bool down_direction,
                                                            bool ignore_start_state,
                                                            uint8_t cl_component_id,
                                                            uint8_t start_level,
                                                            uint8_t duration,
                                                            uint8_t version ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_color_switch_start_level_change( node_id,
                                                    channel_id,
                                                    down_direction,
                                                    ignore_start_state,
                                                    cl_component_id,
                                                    start_level,
                                                    duration,
                                                    NULL,
                                                    NULL,
                                                    encap,
                                                    version );
}

// node cmd color switch stop level change
/// INFO: Color Switch Command Class v1

znet_error_t znet_node_cmd_color_switch_stop_level_change( znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           uint8_t cl_component_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_color_switch_stop_level_change( node_id, channel_id, cl_component_id, NULL, NULL, encap );
}
