/// INFO: crt & system
#include <assert.h>

/// INFO: private
#include "znet_main.h"
#include "znet_log.h"

/// INFO: internal
#include <znet_lib_cc_management.h>

/*----------------------------------------------------------------------------*/

static int _znet_node_cmd_time_parameters_value_is_valid( znet_cmd_time_parameters_year_t UNUSED( year ),
                                                          znet_cmd_time_parameters_month_t month,
                                                          znet_cmd_time_parameters_day_t day,
                                                          znet_cmd_time_parameters_hour_t hour_utc,
                                                          znet_cmd_time_parameters_minute_t minute_utc,
                                                          znet_cmd_time_parameters_second_t second_utc ) {
    return month >= ZNET_CMD_TIME_PARAMETERS_MONTH_VALUE_MIN && month <= ZNET_CMD_TIME_PARAMETERS_MONTH_VALUE_MAX &&
           day >= ZNET_CMD_TIME_PARAMETERS_DAY_VALUE_MIN && day <= ZNET_CMD_TIME_PARAMETERS_DAY_VALUE_MAX &&
           hour_utc >= ZNET_CMD_TIME_PARAMETERS_HOUR_VALUE_MIN && hour_utc <= ZNET_CMD_TIME_PARAMETERS_HOUR_VALUE_MAX &&
           minute_utc >= ZNET_CMD_TIME_PARAMETERS_MINUTE_VALUE_MIN &&
           minute_utc <= ZNET_CMD_TIME_PARAMETERS_MINUTE_VALUE_MAX &&
           second_utc >= ZNET_CMD_TIME_PARAMETERS_SECOND_VALUE_MIN &&
           second_utc <= ZNET_CMD_TIME_PARAMETERS_SECOND_VALUE_MAX;
}

/*----------------------------------------------------------------------------*/

static void _znet_node_cmd_time_parameters_get_cb( ZFunction func,
                                                   void* UNUSED( arg ),
                                                   ZFuncFailures_e reason,
                                                   const void* UNUSED( dataPtr ),
                                                   size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_time_parameters_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_time_parameters_result( znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
    }
}

/*----------------------------------------------------------------------------*/

znet_error_t znet_node_cmd_time_parameter_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_time_parameters_get( node_id, channel_id, _znet_node_cmd_time_parameters_get_cb, NULL, encap );
}

/*----------------------------------------------------------------------------*/

void znet_cc_time_parameters_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len == 9 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_TIME_PARAMETERS );
    assert( cc_data[1] == TIME_PARAMETERS_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    znet_cmd_time_parameters_report_t report = {};

    report.date_utc.year = ( znet_cmd_time_parameters_year_t )( cc_data[3] | ( cc_data[2] << 8 ) );
    report.date_utc.month = cc_data[4];
    report.date_utc.day = cc_data[5];
    report.time_utc.hour = cc_data[6];
    report.time_utc.minute = cc_data[7];
    report.time_utc.second = cc_data[8];

    if( znet_cb->node_cmd_time_parameters_result )
        znet_cb->node_cmd_time_parameters_result( ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

/*----------------------------------------------------------------------------*/

znet_error_t znet_node_cmd_time_parameters_set( znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                const znet_date_t* date_utc,
                                                const znet_time_t* time_utc ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( !_znet_node_cmd_time_parameters_value_is_valid(
            date_utc->year, date_utc->month, date_utc->day, time_utc->hour, time_utc->minute, time_utc->second ) ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_time_parameters_set( node_id,
                                        channel_id,
                                        date_utc->year,
                                        date_utc->month,
                                        date_utc->day,
                                        time_utc->hour,
                                        time_utc->minute,
                                        time_utc->second,
                                        NULL,
                                        NULL,
                                        encap );
}

/*----------------------------------------------------------------------------*/
