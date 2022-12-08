/**
 * @file znet_cmd_notification.c
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

/// INFO: Notification Command Class v8

/*********************************************************************************************************************/

void znet_cc_notification_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    if( cc_data_len < 4 ) {
        ZNET_LOGE( "znet_cc_notification_report : cc_data_len < 4 %d \n" );
        supervision_status = SUPERVISION_REPORT_FAIL;
        return;
    }
    assert( cc_data[0] == ZNET_COMMAND_CLASS_NOTIFICATION );
    assert( cc_data[1] == NOTIFICATION_REPORT_V8 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        supervision_status = SUPERVISION_REPORT_FAIL;
        return;
    }

    znet_notification_report_t report = {};

    report.v1_alarm_type = cc_data[2];
    report.v1_alarm_level = cc_data[3];

    if( cc_data_len > 4 && cc_data_len < 9 ) {
        ZNET_LOGW( "znet_cc_notification_report, wrong data size %u\n", cc_data_len );
        supervision_status = SUPERVISION_REPORT_FAIL;
        return;
    }
    else if( cc_data_len >= 9 ) {
        report.event_parameters_length = cc_data[8] & NOTIFICATION_REPORT_PROPERTIES1_EVENT_PARAMETERS_LENGTH_MASK_V8;

        assert( cc_data_len >= 9 );
        report.notification_status = cc_data[5];
        report.notification_type = cc_data[6];
        report.notification_event = cc_data[7];
        report.sequence = ( cc_data[8] & NOTIFICATION_REPORT_PROPERTIES1_SEQUENCE_BIT_MASK_V8 ) >> 7;

        report.event_parameter = cc_data + 9;

        if( report.sequence )
            report.sequence_number = cc_data[9 + report.event_parameters_length];
    }

    supervision_status = SUPERVISION_REPORT_SUCCESS;

    if( znet_cb->node_cmd_notification_result )
        znet_cb->node_cmd_notification_result( ZNET_ERR_OK,
                                               node_id,
                                               func->_endpoint,
                                               report.v1_alarm_type,
                                               report.notification_type,
                                               report.notification_event,
                                               &report,
                                               znet_cb->arg );
}

void znet_cc_notification_supported_report( const ZFunction func,
                                            uint8_t node_id,
                                            int cc_data_len,
                                            const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_NOTIFICATION );
    assert( cc_data[1] == NOTIFICATION_SUPPORTED_REPORT_V8 );
    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;
    const uint8_t bit_mask_count = cc_data[2] & NOTIFICATION_SUPPORTED_REPORT_PROPERTIES1_NUMBER_OF_BIT_MASKS_MASK_V8;

    znet_notification_supported_report_t report = {};
    report.bit_mask_count = bit_mask_count;
    report.v1_alarm = ( cc_data[2] & NOTIFICATION_SUPPORTED_REPORT_PROPERTIES1_V1_ALARM_BIT_MASK_V8 );
    report.bit_mask = cc_data + 3;

    if( znet_cb->node_cmd_notification_supported_report )
        znet_cb->node_cmd_notification_supported_report( ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

void znet_cc_notification_event_supported_report( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_NOTIFICATION );
    assert( cc_data[1] == EVENT_SUPPORTED_REPORT_V8 );
    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_notification_supported_report ) {
        const uint8_t bit_mask_count = cc_data[3] & EVENT_SUPPORTED_REPORT_PROPERTIES1_NUMBER_OF_BIT_MASKS_MASK_V8;

        znet_cb->node_cmd_notification_event_supported_report(
            ZNET_ERR_OK, node_id, func->_endpoint, cc_data[2], cc_data + 4, bit_mask_count, znet_cb->arg );
    }
}

union notification_param {
    void* data;
    struct {
        uint8_t v1_alarm_type;
        uint8_t notification_type;
        uint8_t event;
    };
};

static void _znet_node_cmd_notification_get_cb( ZFunction func,
                                                void* arg,
                                                ZFuncFailures_e reason,
                                                const void* dataPtr,
                                                size_t dataSize ) {
    if( znet_cb->node_cmd_notification_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union notification_param param;
        param.data = arg;

        znet_cb->node_cmd_notification_result( znet_get_error( reason ),
                                               nodeId,
                                               endpoint,
                                               param.v1_alarm_type,
                                               param.notification_type,
                                               param.event,
                                               NULL,
                                               znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_notification_get( znet_node_id_t node_id,
                                             znet_node_channel_id_t channel_id, /* = ZNET_CHANNEL_ID_ROOT */
                                             uint8_t v1_alarm_type,
                                             uint8_t type,
                                             uint8_t event,
                                             uint8_t version ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union notification_param param;
    param.data = NULL;
    param.v1_alarm_type = v1_alarm_type;
    param.notification_type = type;
    param.event = event;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_notification_get( node_id,
                                     channel_id,
                                     v1_alarm_type,
                                     type,
                                     event,
                                     _znet_node_cmd_notification_get_cb,
                                     param.data,
                                     encap,
                                     version );
}

znet_error_t znet_node_cmd_notification_set( znet_node_id_t node_id,
                                             znet_node_channel_id_t channel_id, /* = ZNET_CHANNEL_ID_ROOT */
                                             uint8_t type,
                                             uint8_t status ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_notification_set( node_id, channel_id, type, status, NULL, NULL, encap );
}

static void _znet_node_cmd_notification_supported_get_cb( ZFunction func,
                                                          void* arg,
                                                          ZFuncFailures_e reason,
                                                          const void* dataPtr,
                                                          size_t dataSize ) {
    if( znet_cb->node_cmd_notification_supported_report ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_notification_supported_report(
            znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_notification_supported_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_notification_supported_get(
        node_id, channel_id, _znet_node_cmd_notification_supported_get_cb, NULL, encap );
}

union notification_event_param {
    void* data;
    uint8_t type;
};

static void _znet_node_cmd_notification_event_supported_get_cb( ZFunction func,
                                                                void* arg,
                                                                ZFuncFailures_e reason,
                                                                const void* dataPtr,
                                                                size_t dataSize ) {
    if( znet_cb->node_cmd_notification_event_supported_report ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union notification_event_param param;
        param.data = arg;

        znet_cb->node_cmd_notification_event_supported_report(
            znet_get_error( reason ), nodeId, endpoint, param.type, NULL, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_notification_event_supported_get( znet_node_id_t node_id,
                                                             znet_node_channel_id_t channel_id,
                                                             uint8_t type ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union notification_event_param param;
    param.data = NULL;
    param.type = type;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_notification_event_supported_get(
        node_id, channel_id, type, _znet_node_cmd_notification_event_supported_get_cb, param.data, encap );
}
