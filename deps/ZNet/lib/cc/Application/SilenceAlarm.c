#include "byte_ops.h"
#include "znet.h"

#include "ZPlatform.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

znet_error_t ICACHE_FLASH_ATTR znet_cc_silence_alarm_set( uint8_t node_id,
                                                          uint8_t channel_id,
                                                          uint8_t mode,
                                                          uint16_t time,
                                                          uint8_t sensors_mask_size,
                                                          const uint8_t* sensors_mask,
                                                          ZFunction_Callback callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type ) {
    uint8_t data[6 + sensors_mask_size];
    data[0] = COMMAND_CLASS_SILENCE_ALARM;
    data[1] = SENSOR_ALARM_SET;
    data[2] = mode;
    memcpy( data + 3, &time, 2 );
    data[5] = sensors_mask_size;
    memcpy( data + 6, sensors_mask, sensors_mask_size );
    return znet_fc_send_data( node_id, channel_id, 6 + sensors_mask_size, data, callback, callbackArg, encap_type );
}
