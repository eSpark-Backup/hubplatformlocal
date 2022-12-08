
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "byte_ops.h"

#include "znet.h"

#include "ZPlatform.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Send BarrierOperator Get
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_barrier_operator_get( uint8_t node_id,
                                                             uint8_t channel_id,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_BARRIER_OPERATOR, BARRIER_OPERATOR_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send BarrierOperator Sugnal Supported Get
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_barrier_operator_signal_supported_get( uint8_t node_id,
                                                                              uint8_t channel_id,
                                                                              ZFunction_Callback Callback,
                                                                              void* callbackArg,
                                                                              encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_BARRIER_OPERATOR, BARRIER_OPERATOR_SIGNAL_SUPPORTED_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send BarrierOperator Set
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: target
// Target Value
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_barrier_operator_set( uint8_t node_id,
                                                             uint8_t channel_id,
                                                             uint8_t target,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_BARRIER_OPERATOR, BARRIER_OPERATOR_SET, target };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send BarrierOperator Sugnal Get
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: type
// Subsystem Type
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_barrier_operator_signal_get( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    uint8_t type,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_BARRIER_OPERATOR, BARRIER_OPERATOR_SIGNAL_GET, type };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send BarrierOperator Sugnal Set
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: type
// Subsystem Type
//
// @param: state
// Subsystem State
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_barrier_operator_signal_set( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    uint8_t type,
                                                                    uint8_t state,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_BARRIER_OPERATOR, BARRIER_OPERATOR_SIGNAL_SET, type, state };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}
