
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
// Send SwitchMultilevel Get
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_multilevel_get( uint8_t node_id,
                                                              uint8_t channel_id,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg,
                                                              encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send SwitchMultilevel Set
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: level
// Level to be set
//
// @param: duration
// @default: 0xff
// Duration of change:
//  0 instantly
//  0x01-0x7f in seconds
//  0x80-0xfe in minutes mapped to 1-127 (value 0x80=128 is 1 minute)
//  0xff use device factory default
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_multilevel_set( uint8_t node_id,
                                                              uint8_t channel_id,
                                                              uint8_t level,
                                                              bool duration_available,
                                                              uint8_t duration,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg,
                                                              encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_SET, level, duration };
    return znet_fc_send_data( node_id,
                              channel_id,
                              ( ( sizeof data ) - ( duration_available ? 0 : 1 ) ),
                              data,
                              Callback,
                              callbackArg,
                              encap_type );
}

//----------------------------------------------------------------------------
// Send SwitchMultilevel StartLevelChange
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: dir
// Direction of change: 0 to incrase, 1 to decrase
//
// @param: ignoreStartLevel
// @default: TRUE
// If set to True, device will ignore start level value and will use it's curent
// value
//
// @param: startLevel
// @default: 50
// Start level to change from
//
// @param: duration_available
//
// @param: duration
// @default: 0xff
// Duration of change:
//  0 instantly
//  0x01-0x7f in seconds
//  0x80-0xfe in minutes mapped to 1-127 (value 0x80=128 is 1 minute)
//  0xff use device factory default
//
// @param: step_available
//
// @param: step
// @default: 0xff
// Step to be used in level change in percentage
// 0-99 mapped to 1-100%
// 0xff uses device factory default
//
// @param: incdec
// @default: 0
// Increment/decrement type for step:
//  0 Increment
//  1 Decrement
//  2 Reserved
//  3 No Inc/Dec
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_multilevel_start_level_change( uint8_t node_id,
                                                                             uint8_t channel_id,
                                                                             uint8_t dir,
                                                                             bool ignore_start_level,
                                                                             uint8_t startLevel,
                                                                             bool duration_available,
                                                                             uint8_t duration,
                                                                             bool secondary_switch_step_available,
                                                                             uint8_t secondary_switch_step,
                                                                             uint8_t secondary_switch_dir,
                                                                             ZFunction_Callback Callback,
                                                                             void* callbackArg,
                                                                             encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SWITCH_MULTILEVEL,
                       SWITCH_MULTILEVEL_START_LEVEL_CHANGE,
                       1,
                       startLevel,
                       duration,
                       secondary_switch_step };
    data[2] = ( ( dir & ( secondary_switch_step_available ? 3 : 1 ) ) << 6 ) | ( ( ignore_start_level ? 1 : 0 ) << 5 ) |
              ( ( secondary_switch_step_available ? ( secondary_switch_dir & 3 ) : 0 ) << 3 );
    return znet_fc_send_data(
        node_id,
        channel_id,
        ( ( sizeof data ) - ( duration_available ? ( secondary_switch_step_available ? 0 : 1 ) : 2 ) ),
        data,
        Callback,
        callbackArg,
        encap_type );
}

//----------------------------------------------------------------------------
// Send SwitchMultilevel StopLevelChange
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_multilevel_stop_level_change( uint8_t node_id,
                                                                            uint8_t channel_id,
                                                                            ZFunction_Callback Callback,
                                                                            void* callbackArg,
                                                                            encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_multilevel_supported_get( uint8_t node_id,
                                                                        uint8_t channel_id,
                                                                        ZFunction_Callback Callback,
                                                                        void* callbackArg,
                                                                        encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_SUPPORTED_GET_V4 };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}
