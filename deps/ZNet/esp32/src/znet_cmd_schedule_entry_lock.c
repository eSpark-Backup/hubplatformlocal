/// INFO: crt & system
#include <assert.h>

/// INFO: private
#include "znet_main.h"
#include "znet_log.h"
#include "znet_macros.h"

/// INFO: internal
#include <znet_lib_cc_application.h>

/*----------------------------------------------------------------------------*/

union schedule_id {
    void* data;
    struct {
        uint8_t slotId;
        uint8_t userId;
    };
};

_Static_assert( sizeof( union schedule_id ) == sizeof( void* ),
                "schedule_id size mismatch! Check the fields of union" );

/*----------------------------------------------------------------------------*/

static bool is_schedule_time_valid( uint8_t hour, uint8_t minute ) {
    return /*hour >= ZNET_CMD_SCHEDULE_ENTRY_LOCK_HOUR_MIN &&*/
        hour <= ZNET_CMD_SCHEDULE_ENTRY_LOCK_HOUR_MAX &&
        /*minute >= ZNET_CMD_SCHEDULE_ENTRY_LOCK_MINUTE_MIN &&*/
        minute <= ZNET_CMD_SCHEDULE_ENTRY_LOCK_MINUTE_MAX;
}

/*----------------------------------------------------------------------------*/

static bool is_schedule_date_valid( uint8_t year, uint8_t month, uint8_t day ) {
    return /*year >= ZNET_CMD_SCHEDULE_ENTRY_LOCK_YEAR_MIN && */
        year <= ZNET_CMD_SCHEDULE_ENTRY_LOCK_YEAR_MAX && month >= ZNET_CMD_SCHEDULE_ENTRY_LOCK_MONTH_MIN &&
        month <= ZNET_CMD_SCHEDULE_ENTRY_LOCK_MONTH_MAX && day >= ZNET_CMD_SCHEDULE_ENTRY_LOCK_DAY_MIN &&
        day <= ZNET_CMD_SCHEDULE_ENTRY_LOCK_DAY_MAX;
}

/*----------------------------------------------------------------------------*/

static bool is_time_offset_valid( const znet_cmd_schedule_entry_lock_time_offset_t* time_offset ) {
    return is_time_zone_valid( &time_offset->time_zone )
        /* don't check: abs( time_offset->minute_dst_offset ) <= ZNET_CMD_TIME_PARAMETERS_MINUTE_VALUE_MAX */;
}

/*----------------------------------------------------------------------------*/

bool is_week_day_schedule_valid( const znet_week_day_schedule_t* schedule ) {
    return schedule->day_of_week >= ZNET_CMD_SCHEDULE_ENTRY_LOCK_WEEK_DAY_MIN &&
           schedule->day_of_week <= ZNET_CMD_SCHEDULE_ENTRY_LOCK_WEEK_DAY_MAX &&
           is_schedule_time_valid( schedule->start_hour, schedule->start_min ) &&
           is_schedule_time_valid( schedule->stop_hour, schedule->stop_min );
}

/*----------------------------------------------------------------------------*/

bool is_year_day_schedule_valid( const znet_year_day_schedule_t* schedule ) {
    return is_schedule_date_valid( schedule->start_year, schedule->start_month, schedule->start_day ) &&
           is_schedule_time_valid( schedule->start_hour, schedule->start_minute ) &&
           is_schedule_date_valid( schedule->stop_year, schedule->stop_month, schedule->stop_day ) &&
           is_schedule_time_valid( schedule->stop_hour, schedule->stop_minute );
}

/*----------------------------------------------------------------------------*/

bool is_daily_repeating_valid( const znet_daily_repeating_schedule_t* schedule ) {
    return ( schedule->week_day_bitmask & ZNET_CMD_SCHEDULE_ENTRY_LOCK_ALL_WEEK_DAYS_MASK ) ==
               schedule->week_day_bitmask &&
           is_schedule_time_valid( schedule->start_hour, schedule->start_min ) &&
           is_schedule_time_valid( schedule->duration_hour, schedule->duration_min );
}

/*----------------------------------------------------------------------------*/

znet_error_t znet_node_cc_schedule_entry_lock_enable_all_set( znet_node_id_t node_id,
                                                              znet_node_channel_id_t channel_id,
                                                              bool is_enabled ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_schedule_entry_lock_enable_all_set( node_id, channel_id, is_enabled, NULL, NULL, encap );
}

/*----------------------------------------------------------------------------*/

znet_error_t znet_node_cc_schedule_entry_lock_enable_set( znet_node_id_t node_id,
                                                          znet_node_channel_id_t channel_id,
                                                          uint8_t user_id,
                                                          bool is_enabled ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_schedule_entry_lock_enable_set( node_id, channel_id, user_id, is_enabled, NULL, NULL, encap );
}

/*----------------------------------------------------------------------------*/

static void _znet_node_cc_schedule_entry_lock_time_offset_get( ZFunction func,
                                                               void* UNUSED( arg ),
                                                               ZFuncFailures_e reason,
                                                               const void* UNUSED( dataPtr ),
                                                               size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_schedule_lock_entry_time_offset_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_schedule_lock_entry_time_offset_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
    }
}

/*----------------------------------------------------------------------------*/

znet_error_t znet_node_cc_schedule_entry_lock_time_offset_get( uint8_t node_id, uint8_t channel_id ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_schedule_entry_lock_time_offset_get(
        node_id, channel_id, _znet_node_cc_schedule_entry_lock_time_offset_get, NULL, encap );
}

/*----------------------------------------------------------------------------*/

znet_error_t znet_node_cc_schedule_entry_lock_time_offset_set(
    uint8_t node_id,
    uint8_t channel_id,
    const znet_cmd_schedule_entry_lock_time_offset_t* time_offset ) {
    ZNET_CHECK_NODE( node_id )

    if( !is_time_offset_valid( time_offset ) ) {
        ZNET_LOGE( "znet_node_cc_schedule_entry_lock_time_offset_set: time offset is invalid\n" );
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_schedule_entry_lock_time_offset_set( node_id,
                                                        channel_id,
                                                        time_offset->time_zone.is_tzo_negative,
                                                        time_offset->time_zone.hour_tzo,
                                                        time_offset->time_zone.minute_tzo,
                                                        time_offset->minute_dst_offset,
                                                        NULL,
                                                        NULL,
                                                        encap );
}

/*----------------------------------------------------------------------------*/

void znet_cc_schedule_entry_lock_time_offset_report( const ZFunction func,
                                                     uint8_t node_id,
                                                     int cc_data_len,
                                                     const uint8_t* cc_data ) {
    assert( znet_cb );
    assert( cc_data );
    assert( cc_data_len == 5 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK );
    assert( cc_data[1] == SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_V2 );

    if( !znet_cb->node_schedule_lock_entry_time_offset_result )
        return;

    const ZW_SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_V2_FRAME* frame =
        (const ZW_SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_V2_FRAME*)cc_data;

    const znet_cmd_schedule_entry_lock_time_offset_t report = {
        .time_zone = { .is_tzo_negative = frame->isTzoNegative,
                       .hour_tzo = frame->hourTzo,
                       .minute_tzo = frame->minuteTzo },
        .minute_dst_offset = frame->isMinuteDstNegative ? -frame->minuteDst : frame->minuteDst
    };

    znet_cb->node_schedule_lock_entry_time_offset_result(
        ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

/*----------------------------------------------------------------------------*/

static void _znet_node_cc_schedule_entry_lock_week_day_get( ZFunction func,
                                                            void* arg,
                                                            ZFuncFailures_e reason,
                                                            const void* UNUSED( dataPtr ),
                                                            size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_schedule_lock_entry_week_day_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union schedule_id schedule_id;
        schedule_id.data = arg;

        znet_cb->node_schedule_lock_entry_week_day_result(
            znet_get_error( reason ), nodeId, endpoint, schedule_id.userId, schedule_id.slotId, NULL, znet_cb->arg );
    }
}

/*----------------------------------------------------------------------------*/

znet_error_t znet_node_cc_schedule_entry_lock_week_day_get( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint8_t user_id,
                                                            uint8_t slot_id ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    union schedule_id callback_arg;
    callback_arg.userId = user_id;
    callback_arg.slotId = slot_id;

    return znet_cc_schedule_entry_lock_week_day_get( node_id,
                                                     channel_id,
                                                     user_id,
                                                     slot_id,
                                                     _znet_node_cc_schedule_entry_lock_week_day_get,
                                                     callback_arg.data,
                                                     encap );
}

/*----------------------------------------------------------------------------*/

znet_error_t znet_node_cc_schedule_entry_lock_week_day_set( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint8_t action,
                                                            const znet_week_day_schedule_t* schedule ) {
    ZNET_CHECK_NODE( node_id )

    if( action && !is_week_day_schedule_valid( schedule ) ) {
        ZNET_LOGE( "znet_node_cc_schedule_entry_lock_week_day_set: week shedule is invalid\n" );
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_schedule_entry_lock_week_day_set( node_id,
                                                     channel_id,
                                                     action,
                                                     schedule->schedule_id.user_id,
                                                     schedule->schedule_id.slot_id,
                                                     schedule->day_of_week,
                                                     schedule->start_hour,
                                                     schedule->start_min,
                                                     schedule->stop_hour,
                                                     schedule->stop_min,
                                                     NULL,
                                                     NULL,
                                                     encap );
}

/*----------------------------------------------------------------------------*/

void znet_cc_schedule_entry_lock_week_day_report( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data ) {
    assert( znet_cb );
    assert( cc_data );
    assert( cc_data_len == 9 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK );
    assert( cc_data[1] == SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT );

    if( !znet_cb->node_schedule_lock_entry_week_day_result )
        return;

    const ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT_FRAME* frame =
        (const ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT_FRAME*)cc_data;

    const znet_week_day_schedule_t report = { .schedule_id.user_id = frame->userIdentifier,
                                              .schedule_id.slot_id = frame->scheduleSlotId,
                                              .day_of_week = frame->dayOfWeek,
                                              .start_hour = frame->startHour,
                                              .start_min = frame->startMinute,
                                              .stop_hour = frame->stopHour,
                                              .stop_min = frame->stopMinute };

    znet_cb->node_schedule_lock_entry_week_day_result(
        ZNET_ERR_OK, node_id, func->_endpoint, frame->userIdentifier, frame->scheduleSlotId, &report, znet_cb->arg );
}

/*----------------------------------------------------------------------------*/

static void _znet_node_cc_schedule_entry_lock_year_day_get( ZFunction func,
                                                            void* arg,
                                                            ZFuncFailures_e reason,
                                                            const void* UNUSED( dataPtr ),
                                                            size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_schedule_lock_entry_year_day_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union schedule_id schedule_id;
        schedule_id.data = arg;

        znet_cb->node_schedule_lock_entry_year_day_result(
            znet_get_error( reason ), nodeId, endpoint, schedule_id.userId, schedule_id.slotId, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cc_schedule_entry_lock_year_day_get( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint8_t user_id,
                                                            uint8_t slot_id ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    union schedule_id callback_arg;
    callback_arg.userId = user_id;
    callback_arg.slotId = slot_id;

    return znet_cc_schedule_entry_lock_year_day_get( node_id,
                                                     channel_id,
                                                     user_id,
                                                     slot_id,
                                                     _znet_node_cc_schedule_entry_lock_year_day_get,
                                                     callback_arg.data,
                                                     encap );
}

/*----------------------------------------------------------------------------*/

znet_error_t znet_node_cc_schedule_entry_lock_year_day_set( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint8_t action,
                                                            const znet_year_day_schedule_t* schedule ) {
    ZNET_CHECK_NODE( node_id )

    if( action && !is_year_day_schedule_valid( schedule ) ) {
        ZNET_LOGE( "znet_node_cc_schedule_entry_lock_year_day_set: year schedule is invalid\n" );
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_schedule_entry_lock_year_day_set( node_id,
                                                     channel_id,
                                                     action,
                                                     schedule->schedule_id.user_id,
                                                     schedule->schedule_id.slot_id,
                                                     schedule->start_year,
                                                     schedule->start_month,
                                                     schedule->start_day,
                                                     schedule->start_hour,
                                                     schedule->start_minute,
                                                     schedule->stop_year,
                                                     schedule->stop_month,
                                                     schedule->stop_day,
                                                     schedule->stop_hour,
                                                     schedule->stop_minute,
                                                     NULL,
                                                     NULL,
                                                     encap );
}

/*----------------------------------------------------------------------------*/

void znet_cc_schedule_entry_lock_year_day_report( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data ) {
    assert( znet_cb );
    assert( cc_data );
    assert( cc_data_len == 14 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK );
    assert( cc_data[1] == SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT );

    if( !znet_cb->node_schedule_lock_entry_year_day_result )
        return;

    const ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT_FRAME* frame =
        (const ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT_FRAME*)cc_data;

    const znet_year_day_schedule_t report = { .schedule_id.user_id = frame->userIdentifier,
                                              .schedule_id.slot_id = frame->scheduleSlotId,
                                              .start_year = frame->startYear,
                                              .start_month = frame->startMonth,
                                              .start_day = frame->startDay,
                                              .start_hour = frame->startHour,
                                              .start_minute = frame->startMinute,
                                              .stop_year = frame->stopYear,
                                              .stop_month = frame->stopMonth,
                                              .stop_day = frame->stopDay,
                                              .stop_hour = frame->stopHour,
                                              .stop_minute = frame->stopMinute };

    znet_cb->node_schedule_lock_entry_year_day_result(
        ZNET_ERR_OK, node_id, func->_endpoint, frame->userIdentifier, frame->scheduleSlotId, &report, znet_cb->arg );
}

/*----------------------------------------------------------------------------*/

static void _znet_node_cc_schedule_entry_type_supported_get( ZFunction func,
                                                             void* UNUSED( arg ),
                                                             ZFuncFailures_e reason,
                                                             const void* UNUSED( dataPtr ),
                                                             size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_schedule_lock_entry_supported_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_schedule_lock_entry_supported_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cc_schedule_entry_type_supported_get( uint8_t node_id, uint8_t channel_id ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_schedule_entry_type_supported_get(
        node_id, channel_id, _znet_node_cc_schedule_entry_type_supported_get, NULL, encap );
}

/*----------------------------------------------------------------------------*/

void znet_cc_schedule_entry_type_supported_report( const ZFunction func,
                                                   uint8_t node_id,
                                                   int cc_data_len,
                                                   const uint8_t* cc_data ) {
    assert( znet_cb );
    assert( cc_data );
    assert( cc_data_len >= 4 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK );
    assert( cc_data[1] == SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT );

    if( !znet_cb->node_schedule_lock_entry_supported_result )
        return;

    const ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT_FRAME* frame =
        (const ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT_FRAME*)cc_data;

    znet_cmd_schedule_entry_lock_supported_report_t report;
    report.week_day_slots = frame->numberOfSlotsWeekDay;
    report.year_day_slots = frame->numberOfSlotsYearDay;
    report.daily_repeating_slots = cc_data_len == 5 ? cc_data[4 /*numberOfSlotsDailyRepeating*/] : 0;

    znet_cb->node_schedule_lock_entry_supported_result( ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

/*----------------------------------------------------------------------------*/

static void _znet_node_cc_schedule_entry_lock_daily_repeating_get( ZFunction func,
                                                                   void* arg,
                                                                   ZFuncFailures_e reason,
                                                                   const void* UNUSED( dataPtr ),
                                                                   size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_schedule_lock_entry_daily_repeating_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union schedule_id schedule_id;
        schedule_id.data = arg;

        znet_cb->node_schedule_lock_entry_daily_repeating_result(
            znet_get_error( reason ), nodeId, endpoint, schedule_id.userId, schedule_id.slotId, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cc_schedule_entry_lock_daily_repeating_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   uint8_t user_id,
                                                                   uint8_t slot_id ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    union schedule_id callback_arg;
    callback_arg.userId = user_id;
    callback_arg.slotId = slot_id;

    return znet_cc_schedule_entry_lock_daily_repeating_get( node_id,
                                                            channel_id,
                                                            user_id,
                                                            slot_id,
                                                            _znet_node_cc_schedule_entry_lock_daily_repeating_get,
                                                            callback_arg.data,
                                                            encap );
}

/*----------------------------------------------------------------------------*/

znet_error_t znet_node_cc_schedule_entry_lock_daily_repeating_set( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   uint8_t action,
                                                                   const znet_daily_repeating_schedule_t* schedule ) {
    ZNET_CHECK_NODE( node_id )

    if( action && !is_daily_repeating_valid( schedule ) ) {
        ZNET_LOGE( "znet_node_cc_schedule_entry_lock_daily_repeating_set: daily repeating shedule is invalid\n" );
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_schedule_entry_lock_daily_repeating_set( node_id,
                                                            channel_id,
                                                            action,
                                                            schedule->schedule_id.user_id,
                                                            schedule->schedule_id.slot_id,
                                                            schedule->week_day_bitmask,
                                                            schedule->start_hour,
                                                            schedule->start_min,
                                                            schedule->duration_hour,
                                                            schedule->duration_min,
                                                            NULL,
                                                            NULL,
                                                            encap );
}

/*----------------------------------------------------------------------------*/

void znet_cc_schedule_entry_lock_daily_repeating_report( const ZFunction func,
                                                         uint8_t node_id,
                                                         int cc_data_len,
                                                         const uint8_t* cc_data ) {
    assert( znet_cb );
    assert( cc_data );
    assert( cc_data_len == 9 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK );
    assert( cc_data[1] == SCHEDULE_ENTRY_LOCK_DAILY_REPEATING_REPORT_V3 );

    if( !znet_cb->node_schedule_lock_entry_daily_repeating_result )
        return;

    const ZW_SCHEDULE_ENTRY_LOCK_DAILY_REPEATING_REPORT_V3_FRAME* frame =
        (const ZW_SCHEDULE_ENTRY_LOCK_DAILY_REPEATING_REPORT_V3_FRAME*)cc_data;

    const znet_daily_repeating_schedule_t report = { .schedule_id.user_id = frame->userIdentifier,
                                                     .schedule_id.slot_id = frame->scheduleSlotId,
                                                     .week_day_bitmask = frame->weekDayBitmask,
                                                     .start_hour = frame->startHour,
                                                     .start_min = frame->startMinute,
                                                     .duration_hour = frame->durationHour,
                                                     .duration_min = frame->durationMinute };

    znet_cb->node_schedule_lock_entry_daily_repeating_result(
        ZNET_ERR_OK, node_id, func->_endpoint, frame->userIdentifier, frame->scheduleSlotId, &report, znet_cb->arg );
}

/*----------------------------------------------------------------------------*/
