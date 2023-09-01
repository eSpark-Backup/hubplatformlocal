
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
// Send Scene Activation Set
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: sceneID
// Scene ID to set (from 1 to 255)
//
// @param: dimmingDuration
// Dimming Duration in seconds or minutes
// TODO: fill with encoding
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_scene_activation_set( uint8_t node_id,
                                                             uint8_t channel_id,
                                                             uint8_t sceneId,
                                                             uint8_t dimmingDuration,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SCENE_ACTIVATION, SCENE_ACTIVATION_SET, sceneId, dimmingDuration };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

// Send Scene Actuator Configuration Set
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: sceneId
// Scene ID to set (from 1 to 255)
//
// @param: dimmingDuration
// Dimming Duration in seconds or minutes
// TODO: fill with encoding
//
// @param: override
// Override to set (bool)
//
// @param: level
// Level to set (from 0 to 255)
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_scene_actuator_conf_set( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t sceneId,
                                                                uint8_t dimmingDuration,
                                                                bool override,
                                                                uint8_t level,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SCENE_ACTUATOR_CONF,
                       SCENE_ACTUATOR_CONF_SET,
                       sceneId,
                       dimmingDuration,
                       ( override ? 0x80 : 0x00 ),
                       level };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

// Send Scene Actuator Configuration Get
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: sceneId
// Scene ID to get(1...255) to request. If scene ID is equal to 0 then current active scene is requested.
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_scene_actuator_conf_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t sceneId,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SCENE_ACTUATOR_CONF, SCENE_ACTUATOR_CONF_GET, sceneId };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

// Send Scene Controller Configuration Set
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: groupID
// Group ID to set (from 1 to 255)
//
// @param: sceneID
// Scene ID to set (from 0 to 255)
//
// @param: dimmingDuration
// Dimming Duration in seconds or minutes
// TODO: fill with encoding
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_scene_controller_conf_set( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  uint8_t groupId,
                                                                  uint8_t sceneId,
                                                                  uint8_t dimmingDuration,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type ) {
    uint8_t data[] = {
        COMMAND_CLASS_SCENE_CONTROLLER_CONF, SCENE_CONTROLLER_CONF_SET, groupId, sceneId, dimmingDuration
    };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

// Send Scene Controller Configuration Get
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: groupId
// Group ID to get(1...255) to request. Group ID equal to 0 requests the currently active group and scene ID.
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_scene_controller_conf_get( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  uint8_t groupId,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SCENE_CONTROLLER_CONF, SCENE_CONTROLLER_CONF_GET, groupId };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
