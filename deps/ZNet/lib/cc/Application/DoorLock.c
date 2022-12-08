
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
// Door Lock Configuration Get Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_door_lock_configuration_get( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    ZFunction_Callback callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_DOOR_LOCK, DOOR_LOCK_CONFIGURATION_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Door Lock Capabilities Get Command
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_door_lock_capabilities_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   ZFunction_Callback callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_DOOR_LOCK, DOOR_LOCK_CAPABILITIES_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Door Lock Configuration Set Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: type
// Operation Type
//
// @param: inside_state
// Inside Door Handles State (4 bits)
//
// @param: outside_state
// Outside Door Handles State (4 bits)
//
// @param: lock_min
// Lock Timeout Minutes
//
// @param: lock_sec
// Lock Timeout Seconds
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

znet_error_t ICACHE_FLASH_ATTR znet_cc_door_lock_configuration_set( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    uint8_t type,
                                                                    uint8_t inside_state,
                                                                    uint8_t outside_state,
                                                                    uint8_t lock_min,
                                                                    uint8_t lock_sec,
                                                                    uint16_t auto_relock_timeout,
                                                                    uint16_t hold_and_release_timeout,
                                                                    bool block_to_block_enabled,
                                                                    bool twist_assist_enabled,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type ) {
    ZW_DOOR_LOCK_CONFIGURATION_SET_V4_FRAME frame = {};
    frame.cmdClass = COMMAND_CLASS_DOOR_LOCK;
    frame.cmd = DOOR_LOCK_CONFIGURATION_SET;
    frame.operationType = type;
    frame.insideDoorHandleModes = inside_state;
    frame.outsideDoorHandleModes = outside_state;
    frame.lockTimeoutMinutes = lock_min;
    frame.lockTimeoutSeconds = lock_sec;

    frame.autoRelockTimeLsb = auto_relock_timeout & 0xFF;
    frame.autoRelockTimeMsb = auto_relock_timeout >> 8;
    frame.holdAndReleaseTimeLsb = hold_and_release_timeout & 0xFF;
    frame.holdAndReleaseTimeMsb = hold_and_release_timeout >> 8;
    frame.twistAsisitEnabled = twist_assist_enabled ? 1 : 0;
    frame.blockToBlockEnabled = block_to_block_enabled ? 1 : 0;

    return znet_fc_send_data( node_id,
                              channel_id,
                              sizeof( ZW_DOOR_LOCK_CONFIGURATION_SET_V4_FRAME ),
                              (const uint8_t*)&frame,
                              Callback,
                              callbackArg,
                              encap_type );
}

//----------------------------------------------------------------------------
// Door Lock Operation Get Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_door_lock_operation_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_DOOR_LOCK, DOOR_LOCK_OPERATION_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Door Lock Operation Set Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: mode
// Door Lock Mode
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_door_lock_operation_set( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t mode,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_DOOR_LOCK, DOOR_LOCK_OPERATION_SET, mode };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}
