//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "znet/znet_errors.h"

#include "ZPlatform.h"
#include "ZFunctionClass.h"
#include "znet_lib_cc_codes.h"
#include "znet_lib_fc.h"

#include <assert.h>

znet_error_t znet_cc_local_time_respond( uint8_t node_id,
                                         uint8_t channel_id,
                                         uint8_t hour,
                                         uint8_t minute,
                                         uint8_t second,
                                         ZFunction_Callback callback,
                                         void* callback_arg,
                                         encap_type_t encap_type ) {
    const ZW_TIME_REPORT_FRAME frame = {
        .cmdClass = COMMAND_CLASS_TIME,
        .cmd = TIME_REPORT,
        .hourLocalTime = hour,
        .minuteLocalTime = minute,
        .secondLocalTime = second,
    };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_local_date_respond( uint8_t node_id,
                                         uint8_t channel_id,
                                         uint16_t year,
                                         uint8_t month,
                                         uint8_t day,
                                         ZFunction_Callback callback,
                                         void* callback_arg,
                                         encap_type_t encap_type ) {
    const ZW_DATE_REPORT_FRAME frame = { .cmdClass = COMMAND_CLASS_TIME,
                                         .cmd = DATE_REPORT,
                                         .year1 = year >> 8,
                                         .year2 = year & 0xFF,
                                         .month = month,
                                         .day = day };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_time_zone_set( uint8_t node_id,
                                    uint8_t channel_id,
                                    bool is_tzo_negative,
                                    uint8_t hour_tzo,
                                    uint8_t minute_tzo,
                                    int8_t minute_dst,
                                    uint8_t month_start_dst,
                                    uint8_t month_end_dst,
                                    uint8_t day_start_dst,
                                    uint8_t day_end_dst,
                                    uint8_t hour_start_dst,
                                    uint8_t hour_end_dst,
                                    ZFunction_Callback callback,
                                    void* callback_arg,
                                    encap_type_t encap_type ) {
    const ZW_TIME_OFFSET_SET_V2_FRAME frame = { .cmdClass = COMMAND_CLASS_TIME_V2,
                                                .cmd = TIME_OFFSET_SET_V2,
                                                .isTzoNegative = is_tzo_negative ? 1 : 0,
                                                .hoursTzo = hour_tzo,
                                                .minuteTzo = minute_tzo,
                                                .isMinuteDstNegative = minute_dst >= 0 ? 0 : 1,
                                                .minuteDst = abs( minute_dst ),
                                                .monthStartDst = month_start_dst,
                                                .dayStartDst = day_start_dst,
                                                .hourStartDst = hour_start_dst,
                                                .monthEndDst = month_end_dst,
                                                .dayEndDst = day_end_dst,
                                                .hourEndDst = hour_end_dst };
    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_time_zone_respond( uint8_t node_id,
                                        uint8_t channel_id,
                                        bool is_tzo_negative,
                                        uint8_t hour_tzo,
                                        uint8_t minute_tzo,
                                        int8_t minute_dst,
                                        uint8_t month_start_dst,
                                        uint8_t month_end_dst,
                                        uint8_t day_start_dst,
                                        uint8_t day_end_dst,
                                        uint8_t hour_start_dst,
                                        uint8_t hour_end_dst,
                                        ZFunction_Callback callback,
                                        void* callback_arg,
                                        encap_type_t encap_type ) {
    const ZW_TIME_OFFSET_REPORT_V2_FRAME frame = { .cmdClass = COMMAND_CLASS_TIME_V2,
                                                   .cmd = TIME_OFFSET_REPORT_V2,
                                                   .isTzoNegative = is_tzo_negative ? 1 : 0,
                                                   .hoursTzo = hour_tzo,
                                                   .minuteTzo = minute_tzo,
                                                   .isMinuteDstNegative = minute_dst >= 0 ? 0 : 1,
                                                   .minuteDst = abs( minute_dst ),
                                                   .monthStartDst = month_start_dst,
                                                   .dayStartDst = day_start_dst,
                                                   .hourStartDst = hour_start_dst,
                                                   .monthEndDst = month_end_dst,
                                                   .dayEndDst = day_end_dst,
                                                   .hourEndDst = hour_end_dst };
    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_local_date_get( uint8_t node_id,
                                     uint8_t channel_id,
                                     ZFunction_Callback callback,
                                     void* callback_arg,
                                     encap_type_t encap_type ) {
    const ZW_DATE_GET_FRAME frame = { .cmdClass = COMMAND_CLASS_TIME, .cmd = DATE_GET };
    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_local_time_get( uint8_t node_id,
                                     uint8_t channel_id,
                                     ZFunction_Callback callback,
                                     void* callback_arg,
                                     encap_type_t encap_type ) {
    const ZW_TIME_GET_FRAME frame = { .cmdClass = COMMAND_CLASS_TIME, .cmd = TIME_GET };
    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_time_zone_get( uint8_t node_id,
                                    uint8_t channel_id,
                                    ZFunction_Callback callback,
                                    void* callback_arg,
                                    encap_type_t encap_type ) {
    const ZW_TIME_OFFSET_GET_V2_FRAME frame = { .cmdClass = COMMAND_CLASS_TIME_V2, .cmd = TIME_OFFSET_GET_V2 };

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, callback, callback_arg, encap_type );
}
