
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
// Color Switch Supported Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_color_switch_supported_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SWITCH_COLOR, SWITCH_COLOR_SUPPORTED_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Color Switch Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: cl_component_id
// Destination Color Component ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_color_switch_get( uint8_t node_id,
                                                         uint8_t channel_id,
                                                         uint8_t cl_component_id,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg,
                                                         encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SWITCH_COLOR, SWITCH_COLOR_GET, cl_component_id };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Color Switch Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: cl_component_count
// Destination Color Component Count
//
// @param: components
// Destination Color Component ID + Value
//
// @param: duration
// Destination Duration
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: version
// CC version
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_color_switch_set( uint8_t node_id,
                                                         uint8_t channel_id,
                                                         uint8_t cl_component_count,
                                                         const uint8_t* components,
                                                         uint8_t duration,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg,
                                                         encap_type_t encap_type,
                                                         uint8_t version ) {
    uint8_t header[] = { COMMAND_CLASS_SWITCH_COLOR, SWITCH_COLOR_SET, cl_component_count };
    uint8_t header_len = sizeof header;
    uint8_t data_len = header_len + cl_component_count * 2;
    if( version != SWITCH_COLOR_VERSION )
        ++data_len;
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );  //+1 for
    if( data == NULL )
        return FALSE;

    memcpy( &data[0], header, sizeof header );
    if( cl_component_count > 0 )
        memcpy( &data[header_len], components, cl_component_count * 2 );
    if( version != SWITCH_COLOR_VERSION ) {
        data[data_len - 1] = duration;
    }

    bool result = znet_fc_send_data( node_id, channel_id, data_len, data, Callback, callbackArg, encap_type );
    Heap_Free( data );

    return result;
}

//----------------------------------------------------------------------------
// Color Switch Start Level Change Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: up_down
// Destination Up Down (1 bits)
//
// @param: ignore_start_state
// Destination Ignore Start State (1 bits)
//
// @param: cl_component_id
// Destination Color Component ID
//
// @param: start_level
// Destination Start Level
//
// @param: duration
// Destination Duration
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: version
// CC version
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_color_switch_start_level_change( uint8_t node_id,
                                                                        uint8_t channel_id,
                                                                        bool down_direction,
                                                                        bool ignore_start_state,
                                                                        uint8_t cl_component_id,
                                                                        uint8_t start_level,
                                                                        uint8_t duration,
                                                                        ZFunction_Callback Callback,
                                                                        void* callbackArg,
                                                                        encap_type_t encap_type,
                                                                        uint8_t version ) {
    uint8_t data[] = { COMMAND_CLASS_SWITCH_COLOR,
                       SWITCH_COLOR_START_LEVEL_CHANGE,
                       ( down_direction ? 0x40 : 0x00 ) | ( ignore_start_state ? 0x20 : 0x00 ),
                       cl_component_id,
                       start_level,
                       duration };
    uint8_t size = sizeof( data );
    if( ( version == SWITCH_COLOR_VERSION ) || ( version == SWITCH_COLOR_VERSION_V2 ) )
        --size;
    return znet_fc_send_data( node_id, channel_id, size, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Color Switch Stop Level Change Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: cl_component_id
// Destination Color Component ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_color_switch_stop_level_change( uint8_t node_id,
                                                                       uint8_t channel_id,
                                                                       uint8_t cl_component_id,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SWITCH_COLOR, SWITCH_COLOR_STOP_LEVEL_CHANGE, cl_component_id };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}
