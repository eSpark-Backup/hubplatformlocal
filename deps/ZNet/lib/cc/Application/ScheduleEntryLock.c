
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "znet/znet_errors.h"

#include "ZPlatform.h"
#include "ZFunctionClass.h"
#include "znet_lib_cc_codes.h"
#include "znet_lib_fc.h"

//////////////////////////////////////////////////////////////////////////////

znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_enable_all_set( uint8_t node_id,
                                                                           uint8_t channel_id,
                                                                           bool is_enabled,
                                                                           ZFunction_Callback Callback,
                                                                           void* callbackArg,
                                                                           encap_type_t encap_type ) {
    const ZW_SCHEDULE_ENTRY_LOCK_ENABLE_ALL_SET_FRAME frame = { .cmdClass = COMMAND_CLASS_SCHEDULE_ENTRY_LOCK,
                                                                .cmd = SCHEDULE_ENTRY_LOCK_ENABLE_ALL_SET,
                                                                .enabled = is_enabled ? 0x01 : 0x00 };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_enable_set( uint8_t node_id,
                                                                       uint8_t channel_id,
                                                                       uint8_t user_id,
                                                                       bool is_enabled,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type ) {
    const ZW_SCHEDULE_ENTRY_LOCK_ENABLE_SET_FRAME frame = { .cmdClass = COMMAND_CLASS_SCHEDULE_ENTRY_LOCK,
                                                            .cmd = SCHEDULE_ENTRY_LOCK_ENABLE_SET,
                                                            .userIdentifier = user_id,
                                                            .enabled = is_enabled ? 0x01 : 0x00 };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_time_offset_get( uint8_t node_id,
                                                                            uint8_t channel_id,
                                                                            ZFunction_Callback Callback,
                                                                            void* callbackArg,
                                                                            encap_type_t encap_type ) {
    const ZW_SCHEDULE_ENTRY_LOCK_TIME_OFFSET_GET_V2_FRAME frame = { .cmdClass = COMMAND_CLASS_SCHEDULE_ENTRY_LOCK_V2,
                                                                    .cmd = SCHEDULE_ENTRY_LOCK_TIME_OFFSET_GET_V2 };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_time_offset_set( uint8_t node_id,
                                                                            uint8_t channel_id,
                                                                            bool is_tzo_negative,
                                                                            uint8_t hour_tzo,
                                                                            uint8_t min_tzo,
                                                                            int8_t min_offset_dst,
                                                                            ZFunction_Callback Callback,
                                                                            void* callbackArg,
                                                                            encap_type_t encap_type ) {
    const ZW_SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_V2_FRAME frame = {
        .cmdClass = COMMAND_CLASS_SCHEDULE_ENTRY_LOCK_V2,
        .cmd = SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_V2,
        .isTzoNegative = is_tzo_negative ? 1 : 0,
        .hoursTzo = hour_tzo,
        .minuteTzo = min_tzo,
        .isMinuteDstNegative = min_offset_dst >= 0 ? 0 : 1,
        .minuteDst = abs( min_offset_dst ),
    };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_week_day_get( uint8_t node_id,
                                                                         uint8_t channel_id,
                                                                         uint8_t user_id,
                                                                         uint8_t slot_id,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type ) {
    const ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_GET_FRAME frame = { .cmdClass = COMMAND_CLASS_SCHEDULE_ENTRY_LOCK,
                                                              .cmd = SCHEDULE_ENTRY_LOCK_WEEK_DAY_GET,
                                                              .userIdentifier = user_id,
                                                              .scheduleSlotId = slot_id };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_week_day_set( uint8_t node_id,
                                                                         uint8_t channel_id,
                                                                         uint8_t action,
                                                                         uint8_t user_id,
                                                                         uint8_t slot_id,
                                                                         uint8_t day_of_week,
                                                                         uint8_t start_hour,
                                                                         uint8_t start_min,
                                                                         uint8_t stop_hour,
                                                                         uint8_t stop_min,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type ) {
    const ZW_SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET_FRAME frame = { .cmdClass = COMMAND_CLASS_SCHEDULE_ENTRY_LOCK,
                                                              .cmd = SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET,
                                                              .setAction = action,
                                                              .userIdentifier = user_id,
                                                              .scheduleSlotId = slot_id,
                                                              .dayOfWeek = day_of_week,
                                                              .startHour = start_hour,
                                                              .startMinute = start_min,
                                                              .stopHour = stop_hour,
                                                              .stopMinute = stop_min };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_year_day_get( uint8_t node_id,
                                                                         uint8_t channel_id,
                                                                         uint8_t user_id,
                                                                         uint8_t slot_id,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type ) {
    const ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_GET_FRAME frame = { .cmdClass = COMMAND_CLASS_SCHEDULE_ENTRY_LOCK,
                                                              .cmd = SCHEDULE_ENTRY_LOCK_YEAR_DAY_GET,
                                                              .userIdentifier = user_id,
                                                              .scheduleSlotId = slot_id };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_year_day_set( uint8_t node_id,
                                                                         uint8_t channel_id,
                                                                         uint8_t action,
                                                                         uint8_t user_id,
                                                                         uint8_t slot_id,
                                                                         uint8_t start_year,
                                                                         uint8_t start_month,
                                                                         uint8_t start_day,
                                                                         uint8_t start_hour,
                                                                         uint8_t start_min,
                                                                         uint8_t stop_year,
                                                                         uint8_t stop_month,
                                                                         uint8_t stop_day,
                                                                         uint8_t stop_hour,
                                                                         uint8_t stop_min,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type ) {
    const ZW_SCHEDULE_ENTRY_LOCK_YEAR_DAY_SET_FRAME frame = { .cmdClass = COMMAND_CLASS_SCHEDULE_ENTRY_LOCK,
                                                              .cmd = SCHEDULE_ENTRY_LOCK_YEAR_DAY_SET,
                                                              .setAction = action,
                                                              .userIdentifier = user_id,
                                                              .scheduleSlotId = slot_id,
                                                              .startYear = start_year,
                                                              .startMonth = start_month,
                                                              .startDay = start_day,
                                                              .startHour = start_hour,
                                                              .startMinute = start_min,
                                                              .stopYear = stop_year,
                                                              .stopMonth = stop_month,
                                                              .stopDay = stop_day,
                                                              .stopHour = stop_hour,
                                                              .stopMinute = stop_min };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_type_supported_get( uint8_t node_id,
                                                                          uint8_t channel_id,
                                                                          ZFunction_Callback Callback,
                                                                          void* callbackArg,
                                                                          encap_type_t encap_type ) {
    const ZW_SCHEDULE_ENTRY_TYPE_SUPPORTED_GET_FRAME frame = { .cmdClass = COMMAND_CLASS_SCHEDULE_ENTRY_LOCK,
                                                               .cmd = SCHEDULE_ENTRY_TYPE_SUPPORTED_GET };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_daily_repeating_get( uint8_t node_id,
                                                                                uint8_t channel_id,
                                                                                uint8_t user_id,
                                                                                uint8_t slot_id,
                                                                                ZFunction_Callback Callback,
                                                                                void* callbackArg,
                                                                                encap_type_t encap_type ) {
    const ZW_SCHEDULE_ENTRY_LOCK_DAILY_REPEATING_GET_V3_FRAME frame = {
        .cmdClass = COMMAND_CLASS_SCHEDULE_ENTRY_LOCK_V3,
        .cmd = SCHEDULE_ENTRY_LOCK_DAILY_REPEATING_GET_V3,
        .userIdentifier = user_id,
        .scheduleSlotId = slot_id
    };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_daily_repeating_set( uint8_t node_id,
                                                                                uint8_t channel_id,
                                                                                uint8_t action,
                                                                                uint8_t user_id,
                                                                                uint8_t slot_id,
                                                                                uint8_t week_day,
                                                                                uint8_t start_hour,
                                                                                uint8_t start_min,
                                                                                uint8_t duration_hour,
                                                                                uint8_t duration_min,
                                                                                ZFunction_Callback Callback,
                                                                                void* callbackArg,
                                                                                encap_type_t encap_type ) {
    const ZW_SCHEDULE_ENTRY_LOCK_DAILY_REPEATING_SET_V3_FRAME frame = {
        .cmdClass = COMMAND_CLASS_SCHEDULE_ENTRY_LOCK_V3,
        .cmd = SCHEDULE_ENTRY_LOCK_DAILY_REPEATING_SET_V3,
        .setAction = action,
        .userIdentifier = user_id,
        .scheduleSlotId = slot_id,
        .weekDayBitmask = week_day,
        .startHour = start_hour,
        .startMinute = start_min,
        .durationHour = duration_hour,
        .durationMinute = duration_min
    };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, Callback, callbackArg, encap_type );
}
