
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
// Thermostat Setback Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_setback_get( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_THERMOSTAT_SETBACK, THERMOSTAT_SETBACK_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Thermostat Setback Set Command
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
// Setback Type (2 bits)
//
// @param: state
// Setback State
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_setback_set( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint8_t type,
                                                               int8_t state,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type ) {
    const uint8_t data[] = { COMMAND_CLASS_THERMOSTAT_SETBACK, THERMOSTAT_SETBACK_SET, type & 0x03, (uint8_t)state };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}
