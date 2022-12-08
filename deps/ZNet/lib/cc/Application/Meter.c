
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

//----------------------------------------------------------------------------
// Send Meter Get
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: scale
// @default: -1
// Desired scale
// -1 for all scales
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_get( uint8_t node_id,
                                                  uint8_t channel_id,
                                                  uint8_t scale,
                                                  uint8_t scale2,
                                                  uint8_t rate_type,
                                                  ZFunction_Callback Callback,
                                                  void* callbackArg,
                                                  encap_type_t encap_type,
                                                  uint8_t version ) {
    uint8_t size = 0;
    ZW_METER_GET_V4_FRAME data;
    data.cmdClass = COMMAND_CLASS_METER;
    data.cmd = METER_GET;
    data.properties1 = 0;
    data.scale2 = 0;
    switch( version ) {
    case METER_VERSION:
        size = sizeof( ZW_METER_GET_FRAME );
        break;
    case METER_VERSION_V2:
        data.properties1 |= ( scale & 0x03 ) << 3;
        size = sizeof( ZW_METER_GET_V2_FRAME );
        break;
    case METER_VERSION_V3:
        data.properties1 |= ( scale & 0x07 ) << 3;
        size = sizeof( ZW_METER_GET_V3_FRAME );
        break;
    case METER_VERSION_V4:
        data.properties1 = ( ( scale & 0x07 ) << 3 ) | ( ( rate_type & 0x03 ) << 6 );
        data.scale2 = scale2;
        size = sizeof( ZW_METER_GET_V4_FRAME );
        if( scale != 7 )
            size -= 1;
        break;
    default:
        return ZNET_ERR_INVAL;
    }
    return znet_fc_send_data( node_id, channel_id, size, (uint8_t*)&data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send Meter Reset
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_reset( uint8_t node_id,
                                                    uint8_t channel_id,
                                                    ZFunction_Callback Callback,
                                                    void* callbackArg,
                                                    encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_METER, METER_RESET_V5 };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_reset_v6( uint8_t node_id,
                                                       uint8_t channel_id,
                                                       uint8_t type,
                                                       uint8_t data_size,
                                                       const uint8_t* data,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg,
                                                       encap_type_t encap_type ) {
    uint8_t buffer[sizeof( ZW_METER_RESET_V6_FRAME ) + data_size];
    ZW_METER_RESET_V6_FRAME* frame = (ZW_METER_RESET_V6_FRAME*)buffer;
    frame->cmdClass = COMMAND_CLASS_METER;
    frame->cmd = METER_RESET_V6;
    frame->meterType = type;
    frame->size = data_size;
    memcpy( frame->value, data, data_size );

    return znet_fc_send_data(
        node_id, channel_id, sizeof( ZW_METER_RESET_V6_FRAME ) + data_size, buffer, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send Meter SupportedGet
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_supported_get( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_METER, METER_SUPPORTED_GET_V5 };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}
