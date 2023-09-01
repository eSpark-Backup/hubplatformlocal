
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
// Thermostat Mode Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_mode_get( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_THERMOSTAT_MODE, THERMOSTAT_MODE_GET };

    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Thermostat Mode Set Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: mode
// Mode (5 bits)
//
// @param: next_data_available
//
// @param: data_size
// No of Manufacturer Data fields (3 bits)
//
// @param: man_data
// Manufacturer Data
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_mode_set( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint8_t mode,
                                                            bool next_data_available,
                                                            uint8_t data_size,
                                                            const uint8_t* man_data,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type ) {
    uint8_t header[] = { COMMAND_CLASS_THERMOSTAT_MODE,
                         THERMOSTAT_MODE_SET,
                         ( next_data_available ? ( ( data_size & 0x07 ) << 5 ) : 0 ) | ( mode & 0x1F ) };
    uint8_t header_len = sizeof header;
    uint8_t data_len = header_len + ( next_data_available ? ( data_size & 0x07 ) : 0 );
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    memcpy( &data[0], header, header_len );
    if( next_data_available && ( data_size > 0 ) )
        memcpy( &data[header_len], man_data, data_size & 0x07 );

    /// TODO: check channel support
    znet_error_t result = znet_fc_send_data( node_id, channel_id, data_len, data, Callback, callbackArg, encap_type );
    Heap_Free( data );

    return result;
}

//----------------------------------------------------------------------------
// Thermostat Mode Supported Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_mode_supported_get( uint8_t node_id,
                                                                      uint8_t channel_id,
                                                                      ZFunction_Callback Callback,
                                                                      void* callbackArg,
                                                                      encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_THERMOSTAT_MODE, THERMOSTAT_MODE_SUPPORTED_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}
