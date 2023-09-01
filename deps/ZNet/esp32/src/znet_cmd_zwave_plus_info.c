/**
 * @file znet_cmd_zwave_plus_info.c
 * @date 16 Apr 2018
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

// node cmd zwave plus info report /////////////////////////////////////
/// INFO: Z-Wave Plus Info Command Class v2

void znet_cc_zwave_plus_info_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 9 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO );
    assert( cc_data[1] == ZWAVEPLUS_INFO_REPORT_V2 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    znet_zwaveplus_info_report_t report = {};
    report.zwave_plus_ver = cc_data[2];
    report.role_type = cc_data[3];
    report.node_type = cc_data[4];
    report.installer_icon = ( (uint16_t)cc_data[5] << 8 ) + cc_data[6];
    report.user_icon = ( (uint16_t)cc_data[7] << 8 ) + cc_data[8];

    if( znet_cb->node_cmd_zwaveplus_info_result )
        znet_cb->node_cmd_zwaveplus_info_result( 0, node_id, &report, znet_cb->arg );
}

// node cmd zwave plus info get ////////////////////////////////////////
/// INFO: Z-Wave Plus Info Command Class v2

static void _znet_node_cmd_zwaveplus_info_get_cb( ZFunction func,
                                                  void* arg,
                                                  ZFuncFailures_e reason,
                                                  const void* dataPtr,
                                                  size_t dataSize ) {
    uint8_t nodeId;
    ZNet_CheckCallback( func, reason, &nodeId, NULL );

    if( znet_cb->node_cmd_zwaveplus_info_result )
        znet_cb->node_cmd_zwaveplus_info_result( znet_get_error( reason ), nodeId, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_zwaveplus_info_get( znet_node_id_t node_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    /// TODO: investigte, for which the arg is used in
    /// znet_cc_zwave_plus_info_get

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_zwave_plus_info_get(
        node_id, ZNET_CHANNEL_ID_ROOT, _znet_node_cmd_zwaveplus_info_get_cb, NULL, encap );
}

void znet_cc_zwave_plus_info_fill( const ZFunction func, ZW_ZWAVEPLUS_INFO_REPORT_V2_FRAME* data ) {
    assert( data );

    if( znet_cb->node_cmd_zwaveplus_info_fill ) {
        znet_zwaveplus_info_report_t report = {};
        znet_cb->node_cmd_zwaveplus_info_fill( &report, znet_cb->arg );

        data->installerIconType1 = ( report.installer_icon >> 8 ) & 0xFF;  // MSB
        data->installerIconType2 = report.installer_icon & 0xFF;           // LSB
        data->userIconType1 = ( report.user_icon >> 8 ) & 0xFF;            // MSB
        data->userIconType2 = report.user_icon & 0xFF;                     // LSB
    }
}
