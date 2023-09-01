
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
// Thermostat Setpoint Get Command
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
// Setpoint Type (4 bits)
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_setpoint_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t type,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_THERMOSTAT_SETPOINT, THERMOSTAT_SETPOINT_GET, type & 0x0F };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Thermostat Setpoint Set Command
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
// Setpoint Type (4 bits)
//
// @param: size
// Size (3 bits)
//
// @param: scale
// Scale (2 bits)
//
// @param: precision
// Precision (3 bits)
//
// @param: value
// Value
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_setpoint_set( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t type,
                                                                uint8_t size,
                                                                uint8_t scale,
                                                                uint8_t precision,
                                                                const uint8_t* value,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type ) {
    uint8_t header[] = { COMMAND_CLASS_THERMOSTAT_SETPOINT,
                         THERMOSTAT_SETPOINT_SET,
                         type & 0x0F,
                         ( ( precision & 0x07 ) << 5 ) | ( ( scale & 0x03 ) << 3 ) | ( size & 0x07 ) };
    uint8_t header_len = sizeof header;
    uint8_t data_len = header_len + ( size & 0x07 );
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    memcpy( &data[0], header, header_len );
    if( size > 0 )
        memcpy( &data[header_len], value, size & 0x07 );

    /// TODO: check channel support
    znet_error_t result = znet_fc_send_data( node_id, channel_id, data_len, data, Callback, callbackArg, encap_type );
    Heap_Free( data );

    return result;
}

//----------------------------------------------------------------------------
// Thermostat Setpoint Supported Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_setpoint_supported_get( uint8_t node_id,
                                                                          uint8_t channel_id,
                                                                          ZFunction_Callback Callback,
                                                                          void* callbackArg,
                                                                          encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_THERMOSTAT_SETPOINT, THERMOSTAT_SETPOINT_SUPPORTED_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Thermostat Setpoint Capabilities Get Command
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
// Setpoint Type (4 bits)
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_setpoint_capabilities_get( uint8_t node_id,
                                                                             uint8_t channel_id,
                                                                             uint8_t type,
                                                                             ZFunction_Callback Callback,
                                                                             void* callbackArg,
                                                                             encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_THERMOSTAT_SETPOINT, THERMOSTAT_SETPOINT_CAPABILITIES_GET_V3, type & 0x0F };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}
