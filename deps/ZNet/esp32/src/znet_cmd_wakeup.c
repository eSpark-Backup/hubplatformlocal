/**
 * @file znet_cmd_binary_switch.c
 * @date 2 May 2018
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
#include <znet_lib_cc_management.h>

/// INFO:  WakeUp_Report Command Class v1
void znet_cc_wakeup_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= ZNET_CMD_WAKEUP_REPORT_CHECK_LEN );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_WAKE_UP );
    assert( cc_data[1] == WAKE_UP_INTERVAL_REPORT_V2 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_wakeup_result ) {
        znet_cmd_wakeup_value_t value;
        value = (uint32_t)cc_data[2];
        value = ( value << 8 ) | ( (uint32_t)cc_data[3] );
        value = ( value << 8 ) | ( (uint32_t)cc_data[4] );

        znet_cb->node_cmd_wakeup_result( ZNET_ERR_OK, node_id, func->_endpoint, value, cc_data[5], znet_cb->arg );
    }
}

/// INFO: Wakeup_Interval_Get Command Class v1
static void _znet_node_cmd_wakeup_interval_get_cb( ZFunction func,
                                                   void* arg,
                                                   ZFuncFailures_e reason,
                                                   const void* dataPtr,
                                                   size_t dataSize ) {
    uint8_t nodeId, channelId;
    ZNet_CheckCallback( func, reason, &nodeId, &channelId );

    if( znet_cb->node_cmd_wakeup_result )
        znet_cb->node_cmd_wakeup_result( znet_get_error( reason ), nodeId, channelId, 0, 0, znet_cb->arg );
}

znet_error_t znet_node_cmd_wakeup_interval_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    void* callbackArg = NULL;
    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_wakeup_get( node_id, channel_id, _znet_node_cmd_wakeup_interval_get_cb, callbackArg, encap );
}

/// INFO: Wakeup_Notification Command Class v1
void znet_cc_wakeup_notification( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= ZNET_CMD_WAKEUP_NOTIFICATION_CHECK_LEN );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_WAKE_UP );

    if( znet_cb->node_cmd_wakeup_notification )
        znet_cb->node_cmd_wakeup_notification( 0, node_id, znet_cb->arg );
}

/// INFO: Wakeup_Interval_Set Command Class v1
znet_error_t znet_node_cmd_wakeup_interval_set( znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                znet_cmd_wakeup_value_t wakeup_interval ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    znet_node_id_t dest_node_id = znet_get_sis_node_id();

    return znet_cc_wakeup_set(
        node_id,
        channel_id,
        wakeup_interval,
        dest_node_id,  // 1,  // TODO if current hub is master.  notification_node_id  can be not 1
        NULL,
        NULL,
        encap );
}

znet_node_id_t wakeup_sleep_node_id = 0;
static void _znet_node_cmd_wakeup_sleep_cb( ZFunction func,
                                            void* arg,
                                            ZFuncFailures_e reason,
                                            const void* dataPtr,
                                            size_t dataSize ) {
    ZNet_WakingNode( wakeup_sleep_node_id, 0 );
}

/// INFO: Wakeup_Interval_Sleep Command Class v1
znet_error_t znet_node_cmd_wakeup_sleep( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    wakeup_sleep_node_id = node_id;
    return znet_cc_wakeup_sleep( node_id, channel_id, _znet_node_cmd_wakeup_sleep_cb, NULL, encap );
}

/// INFO: Wakeup Command Class v2
void znet_cc_wakeup_capabilities_report( const ZFunction func,
                                         uint8_t node_id,
                                         int cc_data_len,
                                         const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= ZNET_CMD_WAKEUP_CAPABILITIES_REPORT_CHECK_LEN );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_WAKE_UP );
    assert( cc_data[1] == WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    znet_wakeup_capabilities_report_t report = {};

    uint32_t value;
    value = (uint32_t)cc_data[2];
    value = ( value << 8 ) | ( (uint32_t)cc_data[3] );
    value = ( value << 8 ) | ( (uint32_t)cc_data[4] );

    report.min_wakeup_interval = value;

    value = (uint32_t)cc_data[5];
    value = ( value << 8 ) | ( (uint32_t)cc_data[6] );
    value = ( value << 8 ) | ( (uint32_t)cc_data[7] );

    report.max_wakeup_interval = value;

    value = (uint32_t)cc_data[8];
    value = ( value << 8 ) | ( (uint32_t)cc_data[9] );
    value = ( value << 8 ) | ( (uint32_t)cc_data[10] );
    report.default_wakeup_interval = value;

    value = (uint32_t)cc_data[11];
    value = ( value << 8 ) | ( (uint32_t)cc_data[12] );
    value = ( value << 8 ) | ( (uint32_t)cc_data[13] );
    report.step_wakeup_interval = value;

    if( znet_cb->node_cmd_wakeup_capabilities_result )
        znet_cb->node_cmd_wakeup_capabilities_result( ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

static void _znet_node_cmd_wakeup_capabilities_get_cb( ZFunction func,
                                                       void* arg,
                                                       ZFuncFailures_e reason,
                                                       const void* dataPtr,
                                                       size_t dataSize ) {
    if( znet_cb->node_cmd_wakeup_capabilities_result ) {
        uint8_t node_id, channel_id;
        ZNet_CheckCallback( func, reason, &node_id, &channel_id );

        znet_cb->node_cmd_wakeup_capabilities_result(
            znet_get_error( reason ), node_id, channel_id, NULL, znet_cb->arg );
    }
}

/// INFO: Wakeup Command Class v2
znet_error_t znet_node_cmd_wakeup_capabilities_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_wakeup_capabilities_get(
        node_id, channel_id, _znet_node_cmd_wakeup_capabilities_get_cb, NULL, encap );
}
