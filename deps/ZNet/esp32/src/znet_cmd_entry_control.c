/**
 * @file znet_cmd_entry_control.c
 * @date 14 Oct 2018
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

/// INFO: Entry Control Command Class v1

/*********************************************************************************************************************/

void znet_cc_entry_control_notification( const ZFunction func,
                                         uint8_t node_id,
                                         int cc_data_len,
                                         const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 6 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_ENTRY_CONTROL );
    assert( cc_data[1] == ENTRY_CONTROL_NOTIFICATION );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( cc_data[5] > 32 )
        return;

    const size_t event_data_length = cc_data[5];
    const size_t buff_size = sizeof( znet_entry_control_notification_t ) + event_data_length * sizeof( uint8_t );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );
    znet_entry_control_notification_t* report = (znet_entry_control_notification_t*)&buff;

    // sequence number in range [0, 255]
    // first time initialization flag is 256
    static uint16_t sequence_number_previous = 256;

    if( sequence_number_previous == cc_data[2] ) {
        return;
    }

    sequence_number_previous = cc_data[2];

    report->sequence_number = cc_data[2];

    report->data_type = cc_data[3] & ( ~ENTRY_CONTROL_NOTIFICATION_PROPERTIES1_RESERVED_MASK );
    report->event_type =
        cc_data[4] & ENTRY_CONTROL_EVENT_SUPPORTED_REPORT_PROPERTIES2_EVENT_SUPPORTED_BIT_MASK_LENGTH_MASK;

    report->event_data_length = event_data_length;

    for( size_t i = 0; i < event_data_length; ++i ) {
        report->event_data[i] = cc_data[6 + i];
    }

    if( znet_cb->node_cmd_entry_control_notification )
        znet_cb->node_cmd_entry_control_notification( ZNET_ERR_OK, node_id, func->_endpoint, report, znet_cb->arg );
}

/*********************************************************************************************************************/

void znet_cc_entry_control_key_supported_report( const ZFunction func,
                                                 uint8_t node_id,
                                                 int cc_data_len,
                                                 const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 2 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_ENTRY_CONTROL );
    assert( cc_data[1] == ENTRY_CONTROL_KEY_SUPPORTED_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const size_t key_supported_bit_mask_length = cc_data[2];

    const size_t buff_size =
        sizeof( znet_entry_control_key_supported_report_t ) + key_supported_bit_mask_length * sizeof( uint8_t );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_entry_control_key_supported_report_t* report = (znet_entry_control_key_supported_report_t*)&buff;

    report->key_supported_bit_mask_length = cc_data[2];

    for( size_t i = 0; i < key_supported_bit_mask_length; ++i ) {
        report->key_supported_bit_mask[i] = cc_data[3 + i];
    }

    if( znet_cb->node_cmd_entry_control_key_supported_report )
        znet_cb->node_cmd_entry_control_key_supported_report(
            ZNET_ERR_OK, node_id, func->_endpoint, report, znet_cb->arg );
}

/*********************************************************************************************************************/

void znet_cc_entry_control_event_supported_report( const ZFunction func,
                                                   uint8_t node_id,
                                                   int cc_data_len,
                                                   const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 8 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_ENTRY_CONTROL );
    assert( cc_data[1] == ENTRY_CONTROL_EVENT_SUPPORTED_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const size_t data_type_supported_bit_mask_length =
        cc_data[2] & ENTRY_CONTROL_EVENT_SUPPORTED_REPORT_PROPERTIES1_DATA_TYPE_SUPPORTED_BIT_MASK_LENGTH_MASK;
    const size_t event_type_supported_bit_mask_length =
        cc_data[7] & ENTRY_CONTROL_EVENT_SUPPORTED_REPORT_PROPERTIES2_EVENT_SUPPORTED_BIT_MASK_LENGTH_MASK;

    const size_t buff_data_type_supported_size =
        sizeof( znet_data_type_supported_t ) + data_type_supported_bit_mask_length * sizeof( uint8_t );

    uint8_t buff_data_type_supported[buff_data_type_supported_size];
    memset( buff_data_type_supported, 0, buff_data_type_supported_size );

    const size_t buff_event_type_supported_size =
        sizeof( znet_event_type_supported_t ) + event_type_supported_bit_mask_length * sizeof( uint8_t );
    uint8_t buff_event_type_supported[buff_event_type_supported_size];
    memset( buff_event_type_supported, 0, buff_event_type_supported_size );

    znet_data_type_supported_t* data_type_supported_report = (znet_data_type_supported_t*)&buff_data_type_supported;
    znet_event_type_supported_t* event_type_supported_report = (znet_event_type_supported_t*)&buff_event_type_supported;

    znet_entry_control_event_supported_report_t report = {};

    report.data_type_supported = data_type_supported_report;
    report.event_type_supported = event_type_supported_report;

    report.data_type_supported->bit_mask_length = data_type_supported_bit_mask_length;
    report.event_type_supported->bit_mask_length = event_type_supported_bit_mask_length;

    for( size_t i = 0; i < data_type_supported_bit_mask_length; ++i ) {
        report.data_type_supported->bit_mask[i] = cc_data[3 + i];
    }

    for( size_t i = 0; i < event_type_supported_bit_mask_length; ++i ) {
        report.event_type_supported->bit_mask[i] = cc_data[8 + i];
    }

    report.key_cached.size_supported_max = cc_data[12];
    report.key_cached.size_supported_min = cc_data[13];
    report.key_cached.timeout_supported_max = cc_data[14];
    report.key_cached.timeout_supported_min = cc_data[15];

    if( znet_cb->node_cmd_entry_control_event_supported_report )
        znet_cb->node_cmd_entry_control_event_supported_report(
            ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

/*********************************************************************************************************************/

void znet_cc_entry_control_configuration_report( const ZFunction func,
                                                 uint8_t node_id,
                                                 int cc_data_len,
                                                 const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_ENTRY_CONTROL );
    assert( cc_data[1] == ENTRY_CONTROL_CONFIGURATION_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    znet_entry_control_configuration_report_t report = {};

    report.key_cache_size = cc_data[2];
    report.key_cache_timeout = cc_data[3];

    if( znet_cb->node_cmd_entry_control_configuration_report )
        znet_cb->node_cmd_entry_control_configuration_report(
            ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

/*********************************************************************************************************************/

znet_error_t znet_node_cmd_entry_control_configuration_set( znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id,
                                                            znet_entry_control_configuration_set_t* value ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( value->key_cache_size < 1 || value->key_cache_size > 32 ) {
        ZNET_LOGE( "Key Cache Size initialization failed! \n" );
        return ZNET_ERR_INVAL;
    }

    if( value->key_cache_timeout < 1 || value->key_cache_timeout > 10 ) {
        ZNET_LOGE( "Key Cache Timeout initialization failed! \n" );
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_entry_control_configuration_set(
        node_id, channel_id, value->key_cache_size, value->key_cache_timeout, NULL, NULL, encap );
}
