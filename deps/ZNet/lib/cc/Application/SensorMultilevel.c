
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
// Send SensorMultilevel Get
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: sensor_type
// @default: -1
// Type of sensor to be requested.
// -1 means all sensor types supported by the device
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_multilevel_get( uint8_t node_id,
                                                              uint8_t channel_id,
                                                              uint8_t sensor_type,
                                                              uint8_t scale,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg,
                                                              encap_type_t encap_type,
                                                              uint8_t version ) {
    uint8_t size = 0;
    ZW_SENSOR_MULTILEVEL_GET_V11_FRAME data;
    memset( &data, 0, sizeof( data ) );
    data.cmdClass = COMMAND_CLASS_SENSOR_MULTILEVEL;
    data.cmd = SENSOR_MULTILEVEL_GET;
    data.sensorType = sensor_type;
    data.properties1 = ( ( scale & 3 ) << 3 );

    switch( version ) {
    case SENSOR_MULTILEVEL_VERSION:
    case SENSOR_MULTILEVEL_VERSION_V2:
    case SENSOR_MULTILEVEL_VERSION_V3:
    case SENSOR_MULTILEVEL_VERSION_V4:
        size = sizeof( ZW_SENSOR_MULTILEVEL_GET_FRAME );
        break;
    case SENSOR_MULTILEVEL_VERSION_V5:
    case SENSOR_MULTILEVEL_VERSION_V6:
    case SENSOR_MULTILEVEL_VERSION_V7:
    case SENSOR_MULTILEVEL_VERSION_V8:
    case SENSOR_MULTILEVEL_VERSION_V9:
    case SENSOR_MULTILEVEL_VERSION_V10:
    case SENSOR_MULTILEVEL_VERSION_V11:
        size = sizeof( ZW_SENSOR_MULTILEVEL_GET_V11_FRAME );
        break;
    }
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, size, (uint8_t*)&data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_multilevel_supported_sensor_get( uint8_t node_id,
                                                                               uint8_t channel_id,
                                                                               ZFunction_Callback Callback,
                                                                               void* callbackArg,
                                                                               encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SENSOR_MULTILEVEL, SENSOR_MULTILEVEL_SUPPORTED_GET_SENSOR_V11 };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_multilevel_supported_scale_get( uint8_t node_id,
                                                                              uint8_t channel_id,
                                                                              uint8_t sensor_type,
                                                                              ZFunction_Callback Callback,
                                                                              void* callbackArg,
                                                                              encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_SENSOR_MULTILEVEL, SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_V11, sensor_type };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}
