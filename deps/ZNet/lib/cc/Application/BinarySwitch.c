
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
// Send SwitchBinary Get
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_binary_get( uint8_t node_id,
                                                          uint8_t channel_id,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SWITCH_BINARY, SWITCH_BINARY_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send SwitchBinary Set
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: value
// Value
//
// @param: default_duration_available
//
// @param: default_duration
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_binary_set( uint8_t node_id,
                                                          uint8_t channel_id,
                                                          bool value,
                                                          uint8_t duration,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type ) {
    ZW_SWITCH_BINARY_SET_V2_FRAME data;
    data.cmdClass = COMMAND_CLASS_SWITCH_BINARY;
    data.cmd = SWITCH_BINARY_SET;
    data.targetValue = value ? 0xFF : 0;
    data.duration = duration;

    return znet_fc_send_data(
        node_id, channel_id, sizeof( data ), (const uint8_t*)&data, Callback, callbackArg, encap_type );
}
