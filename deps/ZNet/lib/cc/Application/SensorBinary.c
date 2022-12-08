
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
// Send SensorBinary Get
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: sensorType
// Type of sensor to query information for
// @default: -1
// 0xFF to query information for the first available sensor type
// -1 to query information for all supported sensor types
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
// @param: version
// CC version
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_binary_get( uint8_t node_id,
                                                          uint8_t channel_id,
                                                          uint8_t sensorType,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type,
                                                          uint8_t version ) {
    const uint8_t size = version == SENSOR_BINARY_VERSION ? sizeof( ZW_SENSOR_BINARY_GET_FRAME ) :
                                                            sizeof( ZW_SENSOR_BINARY_GET_V2_FRAME );
    ZW_SENSOR_BINARY_GET_V2_FRAME data;
    data.cmdClass = COMMAND_CLASS_SENSOR_BINARY;
    data.cmd = SENSOR_BINARY_GET;
    data.sensorType = sensorType;

    return znet_fc_send_data( node_id, channel_id, size, (uint8_t*)&data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send SensorBinary Get
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
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
// @param: version
// CC version
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_binary_supported_get( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type,
                                                                    uint8_t version ) {
    if( version >= SENSOR_BINARY_VERSION_V2 ) {
        ZW_SENSOR_BINARY_SUPPORTED_GET_SENSOR_V2_FRAME data;
        data.cmdClass = COMMAND_CLASS_SENSOR_BINARY;
        data.cmd = SENSOR_BINARY_SUPPORTED_GET_SENSOR_V2;
        /// TODO: check channel support
        return znet_fc_send_data(
            node_id, channel_id, sizeof data, (uint8_t*)&data, Callback, callbackArg, encap_type );
    }
    else
        return ZNET_ERR_UNSUPPORT;
}
