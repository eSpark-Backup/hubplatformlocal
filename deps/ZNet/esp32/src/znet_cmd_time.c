//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "byte_ops.h"

#include "znet.h"

#include "ZPlatform.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"
#include "znet_main.h"
#include "znet_macros.h"
#include "znet_log.h"

#include <assert.h>

#define CHECK_CC_DATA( znet_cb, cc_data, cc_data_len, FRAME_TYPE, expectedCmd )    \
    assert( znet_cb );                                                             \
    assert( cc_data );                                                             \
    assert( cc_data_len >= (int)sizeof( FRAME_TYPE ) );                            \
    assert( ( (const FRAME_TYPE*)cc_data )->cmdClass == ZNET_COMMAND_CLASS_TIME ); \
    assert( ( (const FRAME_TYPE*)cc_data )->cmd == expectedCmd );

// Local Date

void znet_cc_local_time_requested( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    CHECK_CC_DATA( znet_cb, cc_data, cc_data_len, ZW_TIME_GET_FRAME, TIME_GET )

    if( znet_cb->node_cmd_time_requested )
        znet_cb->node_cmd_time_requested( node_id, func->_endpoint, func->_security, znet_cb->arg );
}

void znet_cc_local_date_requested( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    CHECK_CC_DATA( znet_cb, cc_data, cc_data_len, ZW_DATE_GET_FRAME, DATE_GET )

    if( znet_cb->node_cmd_date_requested )
        znet_cb->node_cmd_date_requested( node_id, func->_endpoint, func->_security, znet_cb->arg );
}

void znet_cc_time_zone_requested( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    CHECK_CC_DATA( znet_cb, cc_data, cc_data_len, ZW_TIME_OFFSET_GET_V2_FRAME, TIME_OFFSET_GET_V2 )
    if( znet_cb->node_cmd_time_zone_requested )
        znet_cb->node_cmd_time_zone_requested( node_id, func->_endpoint, func->_security, znet_cb->arg );
}

void znet_cc_local_date_received( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    CHECK_CC_DATA( znet_cb, cc_data, cc_data_len, ZW_DATE_REPORT_FRAME, DATE_REPORT )

    if( znet_cb->node_cmd_local_date_result ) {
        const ZW_DATE_REPORT_FRAME* frame = (const ZW_DATE_REPORT_FRAME*)cc_data;

        const znet_local_date_t local_date = {
            .year = frame->year2 | ( uint16_t )( frame->year1 << 8 ), .month = frame->month, .day = frame->day
        };
        znet_cb->node_cmd_local_date_result( ZNET_ERR_OK, node_id, func->_endpoint, &local_date, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_local_date_respond( znet_node_id_t node_id,
                                               znet_node_channel_id_t channel_id,
                                               const znet_local_date_t* local_date,
                                               const uint8_t encap ) {
    ZNET_CHECK_NODE( node_id )

    if( !is_date_valid( local_date ) ) {
        ZNET_LOGE( "znet_node_cmd_local_date_respond: date is invalid\n" );
        return ZNET_ERR_INVAL;
    }

    return znet_cc_local_date_respond(
        node_id, channel_id, local_date->year, local_date->month, local_date->day, NULL, NULL, encap );
}

static void _znet_node_cmd_local_date_cb( ZFunction func,
                                          void* UNUSED( arg ),
                                          ZFuncFailures_e reason,
                                          const void* UNUSED( dataPtr ),
                                          size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_local_date_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_local_date_result( znet_get_error( reason ), nodeId, endpoint, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_local_date_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_local_date_get( node_id, channel_id, _znet_node_cmd_local_date_cb, NULL, encap );
}

// Local Time

void znet_cc_local_time_received( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    CHECK_CC_DATA( znet_cb, cc_data, cc_data_len, ZW_TIME_REPORT_FRAME, TIME_REPORT )

    if( znet_cb->node_cmd_local_time_result ) {
        const ZW_TIME_REPORT_FRAME* frame = (const ZW_TIME_REPORT_FRAME*)cc_data;

        const znet_local_time_t local_time = {
            .hour = frame->hourLocalTime, .minute = frame->minuteLocalTime, .second = frame->secondLocalTime
        };
        znet_cb->node_cmd_local_time_result( ZNET_ERR_OK, node_id, func->_endpoint, &local_time, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_local_time_respond( znet_node_id_t node_id,
                                               znet_node_channel_id_t channel_id,
                                               const znet_local_time_t* local_time,
                                               const uint8_t encap ) {
    ZNET_CHECK_NODE( node_id )

    if( !is_time_valid( local_time ) ) {
        ZNET_LOGE( "znet_node_cmd_local_time_respond: time is invalid\n" );
        return ZNET_ERR_INVAL;
    }

    return znet_cc_local_time_respond(
        node_id, channel_id, local_time->hour, local_time->minute, local_time->second, NULL, NULL, encap );
}

static void _znet_node_cmd_local_time_cb( ZFunction func,
                                          void* UNUSED( arg ),
                                          ZFuncFailures_e reason,
                                          const void* UNUSED( dataPtr ),
                                          size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_local_time_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_local_time_result( znet_get_error( reason ), nodeId, endpoint, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_local_time_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_local_time_get( node_id, channel_id, _znet_node_cmd_local_time_cb, NULL, encap );
}

// Time Zone

static bool are_tzo_settings_valid( const znet_time_zone_settings_t* settings ) {
    return is_time_zone_valid( &settings->time_zone ) &&
           /* settings->start_dst.hour >= ZNET_CMD_TIME_PARAMETERS_HOUR_VALUE_MIN && */
           settings->start_dst.hour <= ZNET_CMD_TIME_PARAMETERS_HOUR_VALUE_MAX &&
           /* settings->end_dst.hour >= ZNET_CMD_TIME_PARAMETERS_HOUR_VALUE_MIN && */
           settings->end_dst.hour <= ZNET_CMD_TIME_PARAMETERS_HOUR_VALUE_MAX &&
           /* don't check, zero is possibe: settings->start_dst.day */
           settings->start_dst.day <= ZNET_CMD_TIME_PARAMETERS_DAY_VALUE_MAX &&
           /* don't check, zero is possibe: settings->end_dst.day */
           settings->end_dst.day <= ZNET_CMD_TIME_PARAMETERS_DAY_VALUE_MAX &&
           /* don't check, zero is possibe: settings->start_dst.month  */
           settings->start_dst.month <= ZNET_CMD_TIME_PARAMETERS_MONTH_VALUE_MAX &&
           /* don't check, zero is possibe: settings->end_dst.month */
           settings->end_dst.month <= ZNET_CMD_TIME_PARAMETERS_MONTH_VALUE_MAX;
}

void znet_cc_time_zone_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    CHECK_CC_DATA( znet_cb, cc_data, cc_data_len, ZW_TIME_OFFSET_REPORT_V2_FRAME, TIME_OFFSET_REPORT_V2 )

    if( znet_cb->node_cmd_time_zone_result ) {
        const ZW_TIME_OFFSET_REPORT_V2_FRAME* frame = (const ZW_TIME_OFFSET_REPORT_V2_FRAME*)cc_data;

        const znet_time_zone_settings_t settings = {
            .time_zone = { .is_tzo_negative = frame->isTzoNegative,
                           .hour_tzo = frame->hoursTzo,
                           .minute_tzo = frame->minuteTzo },
            .minute_dst_offset = frame->isMinuteDstNegative ? -frame->minuteDst : frame->minuteDst,
            .start_dst = { .month = frame->monthStartDst, .day = frame->dayStartDst, .hour = frame->hourStartDst },
            .end_dst = { .month = frame->monthEndDst, .day = frame->dayEndDst, .hour = frame->hourEndDst }
        };

        znet_cb->node_cmd_time_zone_result( ZNET_ERR_OK, node_id, func->_endpoint, &settings, znet_cb->arg );
    }
}

void znet_cc_time_zone_received( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    CHECK_CC_DATA( znet_cb, cc_data, cc_data_len, ZW_TIME_OFFSET_REPORT_V2_FRAME, TIME_OFFSET_SET_V2 )

    if( znet_cb->node_cmd_time_zone_set ) {
        const ZW_TIME_OFFSET_SET_V2_FRAME* frame = (const ZW_TIME_OFFSET_SET_V2_FRAME*)cc_data;

        bool isCorrectData = frame->hoursTzo >= 0 && frame->hoursTzo <= 14 && frame->minuteTzo >= 0 &&
                             frame->minuteTzo <= 59 && frame->monthStartDst >= 1 && frame->monthStartDst <= 12 &&
                             frame->dayStartDst >= 1 && frame->dayStartDst <= 31 && frame->hourStartDst >= 0 &&
                             frame->hourStartDst <= 23 && frame->monthEndDst >= 1 && frame->monthEndDst <= 12 &&
                             frame->dayEndDst >= 1 && frame->dayEndDst <= 31 && frame->hourEndDst >= 0 &&
                             frame->hourEndDst <= 23;

        if( !isCorrectData ) {
            return;
        }

        const znet_time_zone_settings_t settings = {
            .time_zone = { .is_tzo_negative = frame->isTzoNegative,
                           .hour_tzo = frame->hoursTzo,
                           .minute_tzo = frame->minuteTzo },
            .minute_dst_offset = frame->isMinuteDstNegative ? -frame->minuteDst : frame->minuteDst,
            .start_dst = { .month = frame->monthStartDst, .day = frame->dayStartDst, .hour = frame->hourStartDst },
            .end_dst = { .month = frame->monthEndDst, .day = frame->dayEndDst, .hour = frame->hourEndDst }
        };

        znet_cb->node_cmd_time_zone_set( node_id, func->_endpoint, &settings, znet_cb->arg );
    }
}

static void _znet_node_cmd_time_zone_cb( ZFunction func,
                                         void* UNUSED( arg ),
                                         ZFuncFailures_e reason,
                                         const void* UNUSED( dataPtr ),
                                         size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_time_zone_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_time_zone_result( znet_get_error( reason ), nodeId, endpoint, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_time_zone_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_time_zone_get( node_id, channel_id, _znet_node_cmd_time_zone_cb, NULL, encap );
}

znet_error_t znet_node_cmd_time_zone_set( znet_node_id_t node_id,
                                          znet_node_channel_id_t channel_id,
                                          const znet_time_zone_settings_t* settings ) {
    ZNET_CHECK_NODE( node_id )

    if( !are_tzo_settings_valid( settings ) ) {
        ZNET_LOGE( "znet_node_cmd_time_zone_set: TZO settings are invalid\n" );
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_time_zone_set( node_id,
                                  channel_id,
                                  settings->time_zone.is_tzo_negative,
                                  settings->time_zone.hour_tzo,
                                  settings->time_zone.minute_tzo,
                                  settings->minute_dst_offset,
                                  settings->start_dst.month,
                                  settings->end_dst.month,
                                  settings->start_dst.day,
                                  settings->end_dst.day,
                                  settings->start_dst.hour,
                                  settings->end_dst.hour,
                                  NULL,
                                  NULL,
                                  encap );
}

znet_error_t znet_node_cmd_time_zone_respond( znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              const znet_time_zone_settings_t* settings,
                                              const uint8_t encap ) {
    ZNET_CHECK_NODE( node_id )

    if( !are_tzo_settings_valid( settings ) ) {
        ZNET_LOGE( "znet_node_cmd_time_zone_respond: TZO settings are invalid\n" );
        return ZNET_ERR_INVAL;
    }

    return znet_cc_time_zone_respond( node_id,
                                      channel_id,
                                      settings->time_zone.is_tzo_negative,
                                      settings->time_zone.hour_tzo,
                                      settings->time_zone.minute_tzo,
                                      settings->minute_dst_offset,
                                      settings->start_dst.month == 0 ? 1 : settings->start_dst.month,
                                      settings->end_dst.month == 0 ? 1 : settings->end_dst.month,
                                      settings->start_dst.day == 0 ? 1 : settings->start_dst.day,
                                      settings->end_dst.day == 0 ? 1 : settings->end_dst.day,
                                      settings->start_dst.hour,
                                      settings->end_dst.hour,
                                      NULL,
                                      NULL,
                                      encap );
}
