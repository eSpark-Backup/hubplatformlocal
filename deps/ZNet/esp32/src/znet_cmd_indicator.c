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

union indicator_info {
    void* data;
    uint8_t id;
};

// Indicator Report Command
//
void znet_cc_indicator_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_INDICATOR );
    assert( cc_data[1] == INDICATOR_REPORT );

    size_t buff_size = 0;
    if( cc_data_len == 3 ) {
        buff_size = sizeof( znet_indicator_report_t );
    }
    else {
        buff_size = sizeof( znet_indicator_report_t ) + ( cc_data[3] & 0x1F ) * sizeof( znet_indicator_property_t );
    }

    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_indicator_report_t* report = (znet_indicator_report_t*)&buff;

    report->indicator_0_value = cc_data[2];
    uint8_t indicator_id = 1;
    if( cc_data_len > 3 ) {
        report->indicator_objects_count = cc_data[3] & 0x1F;
        for( int i = 0; i < report->indicator_objects_count; ++i ) {
            uint8_t start_index = 4 + ( i * 3 );

            indicator_id = cc_data[start_index];

            report->properties[i].indicator_id = cc_data[start_index];
            report->properties[i].property_id = cc_data[++start_index];
            report->properties[i].value = cc_data[++start_index];
        }
    }

    if( znet_cb->node_cmd_indicator_result )
        znet_cb->node_cmd_indicator_result( ZNET_ERR_OK, node_id, func->_endpoint, indicator_id, report, znet_cb->arg );
}

// Indicator Supported Report Command
//
void znet_cc_indicator_supported_report( const ZFunction func,
                                         uint8_t node_id,
                                         int cc_data_len,
                                         const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 5 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_INDICATOR );
    assert( cc_data[1] == INDICATOR_SUPPORTED_REPORT_V2 );

    size_t buff_size = sizeof( znet_indicator_supported_report_t ) + ( cc_data[4] & 0x1F );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_indicator_supported_report_t* report = (znet_indicator_supported_report_t*)&buff;

    report->indicator_id = cc_data[2];
    report->next_indicator_id = cc_data[3];
    report->supported_mask_len = ( cc_data[4] & 0x1F );
    for( int i = 0; i < report->supported_mask_len; ++i ) {
        report->bit_mask[i] = cc_data[5 + i];
    }

    if( znet_cb->node_cmd_indicator_supported_result )
        znet_cb->node_cmd_indicator_supported_result(
            ZNET_ERR_OK, node_id, func->_endpoint, report->indicator_id, report, znet_cb->arg );
}

// Indicator Get
//
static void _znet_node_cmd_indicator_get_cb( ZFunction func,
                                             void* arg,
                                             ZFuncFailures_e reason,
                                             const void* dataPtr,
                                             size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    union indicator_info indicator;
    indicator.data = arg;

    if( znet_cb->node_cmd_indicator_result )
        znet_cb->node_cmd_indicator_result(
            znet_get_error( reason ), nodeId, endpoint, indicator.id, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_indicator_get( znet_node_id_t node_id,
                                          znet_node_channel_id_t channel_id,
                                          uint8_t indicator_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union indicator_info indicator;
    indicator.data = NULL;
    indicator.id = indicator_id;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_indicator_get(
        node_id, channel_id, indicator_id, 2, _znet_node_cmd_indicator_get_cb, indicator.data, encap );
}

znet_error_t znet_node_cmd_indicator_get_v1( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_indicator_get( node_id, channel_id, 0, 1, _znet_node_cmd_indicator_get_cb, NULL, encap );
}

// Indicator Get Supported
//
static void _znet_node_cmd_indicator_supported_get_cb( ZFunction func,
                                                       void* arg,
                                                       ZFuncFailures_e reason,
                                                       const void* dataPtr,
                                                       size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    union indicator_info indicator;
    indicator.data = arg;

    if( znet_cb->node_cmd_indicator_supported_result )
        znet_cb->node_cmd_indicator_supported_result(
            znet_get_error( reason ), nodeId, endpoint, indicator.id, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_indicator_supported_get( znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    uint8_t indicator_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union indicator_info indicator;
    indicator.data = NULL;
    indicator.id = indicator_id;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_indicator_supported_get(
        node_id, channel_id, indicator_id, _znet_node_cmd_indicator_supported_get_cb, indicator.data, encap );
}

// Indicator Identify (Indicator SET specific case)
//
znet_error_t znet_node_cmd_indicator_identify( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    uint8_t objects[] = { 0x50, 0x03, 0x08, 0x50, 0x04, 0x03, 0x50, 0x05, 0x06 };

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_indicator_set( node_id, channel_id, 0, 3, objects, NULL, NULL, encap );
}
