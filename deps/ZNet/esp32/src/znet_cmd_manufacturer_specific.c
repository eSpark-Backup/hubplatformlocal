/**
 * @file znet_cmd_manufacturer_specific.c
 * @date 12 Apr 2018
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

// node cmd manufacturer specific report
// /////////////////////////////////////////////
/// INFO: Manufacturer Specific Command Class v1..v2

void znet_cc_manufacturer_specific_report( const ZFunction func,
                                           uint8_t node_id,
                                           int cc_data_len,
                                           const uint8_t* cc_data ) {
    assert( cc_data );

    assert( cc_data_len >= 2 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC );
    assert( cc_data[1] == MANUFACTURER_SPECIFIC_REPORT_V2 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX || cc_data_len < 8 ) {
        if( znet_cb->node_cmd_manufacturer_specific_result )
            znet_cb->node_cmd_manufacturer_specific_result(
                ZNET_ERR_INVAL, ZNET_NODE_ID_INVALID, 0, NULL, znet_cb->arg );
        return;
    }

    znet_manufacturer_specific_report_t report = {};
    report.manufacturer_id = ( (uint16_t)cc_data[2] << 8 ) + cc_data[3];
    report.product_type = ( (uint16_t)cc_data[4] << 8 ) + cc_data[5];
    report.product_id = ( (uint16_t)cc_data[6] << 8 ) + cc_data[7];

    if( znet_cb->node_cmd_manufacturer_specific_result )
        znet_cb->node_cmd_manufacturer_specific_result( ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

// node cmd manufacturer specific get
// ////////////////////////////////////////////////
/// INFO: Manufacturer Specific Command Class v1..v2

static void _znet_node_cmd_manufacturer_specific_get_cb( ZFunction func,
                                                         void* arg,
                                                         ZFuncFailures_e reason,
                                                         const void* dataPtr,
                                                         size_t dataSize ) {
    uint8_t node_id, channel_id;
    ZNet_CheckCallback( func, reason, &node_id, &channel_id );

    if( znet_cb->node_cmd_manufacturer_specific_result )
        znet_cb->node_cmd_manufacturer_specific_result(
            znet_get_error( reason ), node_id, channel_id, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_manufacturer_specific_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    /// TODO: investigte, for which the arg is used in
    /// znet_cc_manufacturer_specific_get

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_manufacturer_specific_get(
        node_id, channel_id, _znet_node_cmd_manufacturer_specific_get_cb, NULL, encap );
}

// node cmd command device specific report /////////////////////////////
/// INFO: Manufacturer Specific Command Class v1..v2

void znet_cc_device_specific_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC );
    assert( cc_data[1] == DEVICE_SPECIFIC_REPORT_V2 );
    assert( cc_data_len >= ( cc_data[3] & 0x1F ) + 4 );

    size_t dcount = cc_data[3] & 0x1F;

    const size_t buff_size = sizeof( znet_device_specific_report_t ) + dcount;
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_device_specific_report_t* report = (znet_device_specific_report_t*)&buff;
    report->type = cc_data[2] & 0x07;
    report->data_format = ( cc_data[3] & 0xE0 ) >> 5;
    report->data_count = dcount;
    memcpy( report->data, &cc_data[4], dcount );

    if( znet_cb->node_cmd_device_specific_result )
        znet_cb->node_cmd_device_specific_result( 0, node_id, report, znet_cb->arg );
}

// node cmd command device specific get ////////////////////////////////
/// INFO: Manufacturer Specific Command Class v1..v2

static void _znet_node_cmd_device_specific_get_cb( ZFunction func,
                                                   void* arg,
                                                   ZFuncFailures_e reason,
                                                   const void* dataPtr,
                                                   size_t dataSize ) {
    uint8_t nodeId;
    ZNet_CheckCallback( func, reason, &nodeId, NULL );

    if( znet_cb->node_cmd_device_specific_result )
        znet_cb->node_cmd_device_specific_result( znet_get_error( reason ), nodeId, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_device_specific_get( znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                znet_cmd_device_specific_type_t type ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    /// TODO: investigate, for which the arg is used in
    /// znet_cc_device_specific_get

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_device_specific_get( node_id, type, channel_id, _znet_node_cmd_device_specific_get_cb, NULL, encap );
}

void znet_cc_manufacturer_specific_fill( const ZFunction func, ZW_MANUFACTURER_SPECIFIC_REPORT_FRAME* data ) {
    assert( data );

    if( znet_cb && znet_cb->node_cmd_manufacturer_specific_fill ) {
        znet_manufacturer_specific_report_t report = {};
        znet_cb->node_cmd_manufacturer_specific_fill( &report, znet_cb->arg );

        data->manufacturerId1 = ( report.manufacturer_id >> 8 ) & 0xFF;  // MSB
        data->manufacturerId2 = report.manufacturer_id & 0xFF;           // LSB
        data->productTypeId1 = ( report.product_type >> 8 ) & 0xFF;      // MSB
        data->productTypeId2 = report.product_type & 0xFF;               // LSB
        data->productId1 = ( report.product_id >> 8 ) & 0xFF;            // MSB
        data->productId2 = report.product_id & 0xFF;                     // LSB
    }
}
