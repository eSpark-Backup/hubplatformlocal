
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
// Initiate Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: node_number
// Node ID display as: NODE_NUMBER
//
// @param: step_id
// Step ID
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_inclusion_initiate( uint8_t node_id,
                                                           uint8_t node_number,
                                                           uint8_t step_id,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg,
                                                           encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_INCLUSION_CONTROLLER, INITIATE, node_number, step_id };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Complete Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: step_id
// Step ID
//
// @param: status
// status
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_inclusion_complete( uint8_t node_id,
                                                           uint8_t step_id,
                                                           uint8_t status,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg,
                                                           encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_INCLUSION_CONTROLLER, COMPLETE, step_id, status };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, encap_type );
}

bool is_inclusion_controller_initiate_command( const uint8_t* data, uint8_t len ) {
    return len >= 2 && data[0] == COMMAND_CLASS_INCLUSION_CONTROLLER && data[1] == INITIATE;
}
