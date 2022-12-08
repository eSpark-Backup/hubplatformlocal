
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
// Entry Control Key Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
bool ICACHE_FLASH_ATTR znet_cc_entry_control_key_supported_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_ENTRY_CONTROL, ENTRY_CONTROL_KEY_SUPPORTED_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Entry Control Event Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
bool ICACHE_FLASH_ATTR znet_cc_entry_control_event_supported_get( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_ENTRY_CONTROL, ENTRY_CONTROL_EVENT_SUPPORTED_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Entry Control Configuration Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: key_cache_size
// Key Cache Size
//
// @param: key_cache_timeout
// Key Cache Timeout
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
bool ICACHE_FLASH_ATTR znet_cc_entry_control_configuration_set( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t key_cache_size,
                                                                uint8_t key_cache_timeout,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type ) {
    uint8_t data[] = {
        COMMAND_CLASS_ENTRY_CONTROL, ENTRY_CONTROL_CONFIGURATION_SET, key_cache_size, key_cache_timeout
    };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Entry Control Configuration Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
bool ICACHE_FLASH_ATTR znet_cc_entry_control_configuration_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_ENTRY_CONTROL, ENTRY_CONTROL_CONFIGURATION_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}
