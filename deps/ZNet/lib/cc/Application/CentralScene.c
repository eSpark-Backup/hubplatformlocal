
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
// Send Central Scene Supported Get
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
// @param: TODO: describe params
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_central_scene_supported_get( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type ) {
    ZW_CENTRAL_SCENE_V3_SUPPORTED_GET_FRAME data = { .cmdClass = COMMAND_CLASS_CENTRAL_SCENE,
                                                     .cmd = CENTRAL_SCENE_SUPPORTED_GET };

    /// TODO: check channel support
    return znet_fc_send_data(
        node_id, channel_id, sizeof data, (const uint8_t*)&data, Callback, callbackArg, encap_type );
}

// Send Central Scene Configuration Get
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
// @param: slow_refresh
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_central_scene_configuration_get( uint8_t node_id,
                                                                        uint8_t channel_id,
                                                                        ZFunction_Callback Callback,
                                                                        void* callbackArg,
                                                                        encap_type_t encap_type ) {
    ZW_CENTRAL_SCENE_V3_CONFIGURATION_GET_FRAME data = { .cmdClass = COMMAND_CLASS_CENTRAL_SCENE,
                                                         .cmd = CENTRAL_SCENE_CONFIGURATION_GET };

    /// TODO: check channel support
    return znet_fc_send_data(
        node_id, channel_id, sizeof data, (const uint8_t*)&data, Callback, callbackArg, encap_type );
}

// Send Central Scene Configuration Set
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
// @param: slow_refresh
// Configure the use of the Slow Refresh capability (bool)
//
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_central_scene_configuration_set( uint8_t node_id,
                                                                        uint8_t channel_id,
                                                                        uint8_t slow_refresh,
                                                                        ZFunction_Callback Callback,
                                                                        void* callbackArg,
                                                                        encap_type_t encap_type ) {
    ZW_CENTRAL_SCENE_V3_CONFIGURATION_SET_FRAME data = {
        .cmdClass = COMMAND_CLASS_CENTRAL_SCENE, .cmd = CENTRAL_SCENE_CONFIGURATION_SET, .slowRefresh = slow_refresh
    };
    /// TODO: check channel support
    return znet_fc_send_data(
        node_id, channel_id, sizeof data, (const uint8_t*)&data, Callback, callbackArg, encap_type );
}
//----------------------------------------------------------------------------
