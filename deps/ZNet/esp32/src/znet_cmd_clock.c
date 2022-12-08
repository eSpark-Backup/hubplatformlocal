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

static int _znet_node_cmd_clock_value_is_valid( znet_clock_hour_value_t hour,
                                                znet_clock_minute_value_t minute,
                                                znet_clock_weekday_value_t weekday ) {
    return ( ( hour > ZNET_CMD_CLOCK_HOUR_VALUE_MIN ) || ( hour == ZNET_CMD_CLOCK_HOUR_VALUE_MIN ) ) &&
           hour <= ZNET_CMD_CLOCK_HOUR_VALUE_MAX &&
           ( ( minute > ZNET_CMD_CLOCK_MINUTE_VALUE_MIN ) || ( minute == ZNET_CMD_CLOCK_MINUTE_VALUE_MIN ) ) &&
           minute <= ZNET_CMD_CLOCK_MINUTE_VALUE_MAX &&
           ( ( weekday >= ZNET_CMD_CLOCK_WEEKDAY_VALUE_MIN && weekday <= ZNET_CMD_CLOCK_WEEKDAY_VALUE_MAX ) ||
             ( weekday == ZNET_CMD_CLOCK_WEEKDAY_VALUE_UNUSED_UNKNOWN ) );
}

void znet_cc_clock_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_CLOCK );
    assert( cc_data[1] == CLOCK_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    /// TODO: check in storage node_id

    znet_clock_value_t report;
    report.hour = cc_data[2] & CLOCK_REPORT_LEVEL_HOUR_MASK;
    report.minute = cc_data[3];
    report.weekday = ( cc_data[2] & CLOCK_REPORT_LEVEL_WEEKDAY_MASK ) >> CLOCK_REPORT_LEVEL_WEEKDAY_SHIFT;

    if( !_znet_node_cmd_clock_value_is_valid( report.hour, report.minute, report.weekday ) ) {
        return;
    }

    if( znet_cb->node_cmd_clock_result )
        znet_cb->node_cmd_clock_result( ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

/**
 * @brief Set clock value
 * @param node_id
 * @param hour
 * @param minute
 * @param weekday
 * @return
 */
znet_error_t znet_node_cmd_clock_set( znet_node_id_t node_id,
                                      znet_node_channel_id_t channel_id,
                                      znet_clock_hour_value_t hour,
                                      znet_clock_minute_value_t minute,
                                      znet_clock_weekday_value_t weekday ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    /// TODO: investigate, need support report

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( !_znet_node_cmd_clock_value_is_valid( hour, minute, weekday ) ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_clock_set( node_id, channel_id, hour, minute, weekday, NULL, NULL, encap );
}

static void _znet_node_cmd_clock_get_cb( ZFunction func,
                                         void* arg,
                                         ZFuncFailures_e reason,
                                         const void* dataPtr,
                                         size_t dataSize ) {
    if( znet_cb->node_cmd_clock_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_clock_result( znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
    }
}

/**
 * @brief Get clock value
 * @param node_id
 * @param channel_id
 * @return
 */
znet_error_t znet_node_cmd_clock_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_clock_get( node_id, channel_id, _znet_node_cmd_clock_get_cb, NULL, encap );
}
