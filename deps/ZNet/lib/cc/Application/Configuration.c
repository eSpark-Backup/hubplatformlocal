
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
// Send Configuration Get
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: parameter
// Parameter number (from 1 to 255)
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_get( uint8_t node_id,
                                                          uint8_t channel_id,
                                                          uint8_t parameter,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_CONFIGURATION, CONFIGURATION_GET, parameter };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send Configuration Set
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: parameter
// Parameter number (from 1 to 255)
//
// @param: value
// Value to be sent (negative and positive values are accepted, but will be
// stripped to size)
//
// @param: size
// @default: 0
// Size of the value (1, 2 or 4 bytes). Use 0 to guess from previously reported
// value if any 0 means use size previously obtained Get
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_set( uint8_t node_id,
                                                          uint8_t channel_id,
                                                          uint8_t parameter,
                                                          bool set_default,
                                                          uint32_t value,
                                                          uint8_t size,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_CONFIGURATION,
                       CONFIGURATION_SET,
                       parameter,
                       ( ( set_default ? CONFIGURATION_SET_LEVEL_DEFAULT_BIT_MASK_V2 : 0 ) |
                         ( size &= CONFIGURATION_SET_LEVEL_SIZE_MASK_V2 ) ),
                       1,
                       2,
                       3,
                       4 };
    if( size > 0 )
        int_to_bytes( value, size, &data[4] );

    if( ( 4 + size ) > sizeof( data ) ) {
        return ZNET_ERR_FAULT;
    }
    return znet_fc_send_data( node_id, channel_id, ( 4 + size ), data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_bulk_get( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint16_t parameter_offset,
                                                               uint8_t parameter_count,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_CONFIGURATION, CONFIGURATION_BULK_GET_V4, 0x01, 0x02, parameter_count };
    int16_to_bytes( parameter_offset, &data[2] );
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_bulk_set( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint16_t parameter_offset,
                                                               uint8_t parameter_count,
                                                               bool set_default,
                                                               bool handshake,
                                                               uint8_t size,
                                                               const uint8_t* parametr,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type ) {
    uint8_t header[] = { COMMAND_CLASS_CONFIGURATION, CONFIGURATION_BULK_SET_V4, 0, 0, parameter_count, 0 };
    int16_to_bytes( parameter_offset, &header[2] );
    header[5] = ( set_default ? 0x80 : 0x00 ) | ( handshake ? 0x40 : 0x00 ) | ( size & 0x07 );
    uint8_t parametr_size = ( size & 0x07 ) * parameter_count;
    uint8_t header_len = sizeof header;
    uint8_t data_len = header_len + parametr_size;
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    memcpy( &data[0], header, header_len );
    if( parametr_size > 0 )
        memcpy( &data[header_len], parametr, parametr_size );
    znet_error_t result = znet_fc_send_data( node_id, channel_id, data_len, data, Callback, callbackArg, encap_type );
    Heap_Free( data );

    return result;
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_name_get( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint16_t parameter_number,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_CONFIGURATION, CONFIGURATION_NAME_GET_V4, 0x01, 0x02 };
    int16_to_bytes( parameter_number, &data[2] );
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_info_get( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint16_t parameter_number,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_CONFIGURATION, CONFIGURATION_INFO_GET_V4, 0x01, 0x02 };
    int16_to_bytes( parameter_number, &data[2] );
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_properties_get( uint8_t node_id,
                                                                     uint8_t channel_id,
                                                                     uint16_t parameter_number,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg,
                                                                     encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_CONFIGURATION, CONFIGURATION_PROPERTIES_GET_V4, 0x01, 0x02 };
    int16_to_bytes( parameter_number, &data[2] );
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Configuration Default Reset Command
//
// @param: znet
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_default_reset( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_CONFIGURATION, CONFIGURATION_DEFAULT_RESET_V4 };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

/**
 * Retrieves parameter number from a serialized command considering its CC version.
 * @param command_data An array of bytes filled with properly formatted Configuration CC command data.
 * @param size The length of @param command_data
 * @return A valid parameter number found in @param command_data or UINT16_MAX in case of a failure.
 */
uint16_t znet_cc_confguration_get_param_number( const uint8_t* command_data, uint8_t size ) {
    uint16_t param_number = UINT16_MAX;
    switch( command_data[1] ) {
    case CONFIGURATION_PROPERTIES_GET_V4:
    case CONFIGURATION_INFO_GET_V4:
    case CONFIGURATION_NAME_GET_V4:
    case CONFIGURATION_BULK_SET_V4:
    case CONFIGURATION_BULK_GET_V4:
        if( size > 3 ) {
            param_number = bytes_to_int16( &command_data[2] );
        }
        break;
    case CONFIGURATION_SET:
    case CONFIGURATION_GET:
        if( size > 2 ) {
            param_number = command_data[2];
        }
        break;
    }
    return param_number;
}
