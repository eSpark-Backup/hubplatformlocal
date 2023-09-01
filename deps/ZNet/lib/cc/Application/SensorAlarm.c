
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
// Sensor Alarm Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: type
// Sensor Type
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_alarm_get( uint8_t node_id,
                                                         uint8_t channel_id,
                                                         uint8_t type,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg,
                                                         encap_type_t encap_type ) {
    ZW_SENSOR_ALARM_GET_FRAME data;
    data.cmdClass = COMMAND_CLASS_SENSOR_ALARM;
    data.cmd = SENSOR_ALARM_GET;
    data.sensorType = type;
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, (uint8_t*)&data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Sensor Alarm Supported Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_alarm_supported_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type ) {
    ZW_SENSOR_ALARM_SUPPORTED_GET_FRAME data;
    data.cmdClass = COMMAND_CLASS_SENSOR_ALARM;
    data.cmd = SENSOR_ALARM_SUPPORTED_GET;
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, (uint8_t*)&data, Callback, callbackArg, encap_type );
}
