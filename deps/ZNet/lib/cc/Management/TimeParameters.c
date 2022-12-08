
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "znet/znet_errors.h"

#include "ZPlatform.h"
#include "ZFunctionClass.h"
#include "znet_lib_cc_codes.h"
#include "znet_lib_fc.h"

//////////////////////////////////////////////////////////////////////////////

znet_error_t znet_cc_time_parameters_get( uint8_t node_id,
                                          uint8_t channel_id,
                                          ZFunction_Callback Callback,
                                          void* callbackArg,
                                          encap_type_t encap_type ) {
    ZW_TIME_PARAMETERS_GET_FRAME data;
    data.cmdClass = COMMAND_CLASS_TIME_PARAMETERS;
    data.cmd = TIME_PARAMETERS_GET;

    return znet_fc_send_data(
        node_id, channel_id, sizeof( data ), (const uint8_t*)&data, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_time_parameters_set( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint16_t year,
                                                            uint8_t month,
                                                            uint8_t day,
                                                            uint8_t hour_utc,
                                                            uint8_t minute_utc,
                                                            uint8_t second_utc,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type ) {
    ZW_TIME_PARAMETERS_SET_FRAME data;
    data.cmdClass = COMMAND_CLASS_TIME_PARAMETERS;
    data.cmd = TIME_PARAMETERS_SET;
    data.year1 = ( uint8_t )( ( year >> 8 ) & 0xFF );
    data.year2 = ( uint8_t )( year & 0xFF );
    data.month = month;
    data.day = day;
    data.hourUtc = hour_utc;
    data.minuteUtc = minute_utc;
    data.secondUtc = second_utc;

    return znet_fc_send_data(
        node_id, channel_id, sizeof( data ), (const uint8_t*)&data, Callback, callbackArg, encap_type );
}
