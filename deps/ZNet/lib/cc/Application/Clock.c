
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "byte_ops.h"

//#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"
//#include "znet_fc_private.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

//////////////////////////////////////////////////////////////////////////////

znet_error_t ICACHE_FLASH_ATTR znet_cc_clock_get( uint8_t node_id,
                                                  uint8_t channel_id,
                                                  ZFunction_Callback Callback,
                                                  void* callbackArg,
                                                  encap_type_t encap_type ) {
    ZW_CLOCK_GET_FRAME data;
    data.cmdClass = COMMAND_CLASS_CLOCK;
    data.cmd = CLOCK_GET;
    return znet_fc_send_data(
        node_id, channel_id, sizeof( data ), (const uint8_t*)&data, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_clock_set( uint8_t node_id,
                                                  uint8_t channel_id,
                                                  uint8_t hour,
                                                  uint8_t minute,
                                                  uint8_t weekday,
                                                  ZFunction_Callback Callback,
                                                  void* callbackArg,
                                                  encap_type_t encap_type ) {
    ZW_CLOCK_SET_FRAME data;
    data.cmdClass = COMMAND_CLASS_CLOCK;
    data.cmd = CLOCK_SET;
    data.level = ( ( weekday << CLOCK_REPORT_LEVEL_WEEKDAY_SHIFT ) & CLOCK_REPORT_LEVEL_WEEKDAY_MASK ) |
                 ( hour & CLOCK_REPORT_LEVEL_HOUR_MASK );
    data.minute = minute;
    return znet_fc_send_data(
        node_id, channel_id, sizeof( data ), (const uint8_t*)&data, Callback, callbackArg, encap_type );
}
