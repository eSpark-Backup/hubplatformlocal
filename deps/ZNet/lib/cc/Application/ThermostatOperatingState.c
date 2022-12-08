
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
// Thermostat Operating State Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_operating_state_get( uint8_t node_id,
                                                                       uint8_t channel_id,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_THERMOSTAT_OPERATING_STATE, THERMOSTAT_OPERATING_STATE_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Thermostat Operating State Logging Supported Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_operating_state_logging_supported_get( uint8_t node_id,
                                                                                         uint8_t channel_id,
                                                                                         ZFunction_Callback Callback,
                                                                                         void* callbackArg,
                                                                                         encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_THERMOSTAT_OPERATING_STATE, THERMOSTAT_OPERATING_STATE_LOGGING_SUPPORTED_GET_V2 };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Thermostat Operating State Logging Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_operating_state_logging_get( uint8_t node_id,
                                                                               uint8_t channel_id,
                                                                               uint8_t cmd_data_length,
                                                                               const uint8_t* cmd_data,
                                                                               ZFunction_Callback Callback,
                                                                               void* callbackArg,
                                                                               encap_type_t encap_type ) {
    uint8_t header[] = { COMMAND_CLASS_THERMOSTAT_OPERATING_STATE, THERMOSTAT_OPERATING_STATE_LOGGING_GET_V2 };
    uint8_t header_len = sizeof header;
    uint8_t data_len = header_len + cmd_data_length;
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    memcpy( &data[0], header, header_len );
    if( cmd_data_length > 0 )
        memcpy( &data[header_len], cmd_data, cmd_data_length );

    /// TODO: check channel support
    znet_error_t result = znet_fc_send_data( node_id, channel_id, data_len, data, Callback, callbackArg, encap_type );
    Heap_Free( data );

    return result;
}
