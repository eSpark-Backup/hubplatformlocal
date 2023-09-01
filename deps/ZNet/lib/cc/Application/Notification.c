
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "byte_ops.h"

#include "znet.h"

#include "ZPlatform.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

znet_error_t ICACHE_FLASH_ATTR znet_cc_notification_get( uint8_t node_id,
                                                         uint8_t channel_id,
                                                         uint8_t v1_alarm_type,
                                                         uint8_t type,
                                                         uint8_t event,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg,
                                                         encap_type_t encap_type,
                                                         uint8_t version ) {
    ZW_NOTIFICATION_GET_V8_FRAME data;
    data.cmdClass = COMMAND_CLASS_NOTIFICATION_V8;
    data.cmd = NOTIFICATION_GET_V8;
    data.v1AlarmType = v1_alarm_type;
    data.notificationType = type;
    data.mevent = event;
    uint8_t data_frame_size;
    switch( version ) {
    case ALARM_VERSION:
        data_frame_size = sizeof( ZW_ALARM_GET_FRAME );
        break;
    case ALARM_VERSION_V2:
        data_frame_size = sizeof( ZW_ALARM_GET_V2_FRAME );
        break;
    case NOTIFICATION_VERSION_V3:
    case NOTIFICATION_VERSION_V4:
    case NOTIFICATION_VERSION_V5:
    case NOTIFICATION_VERSION_V6:
    case NOTIFICATION_VERSION_V7:
    case NOTIFICATION_VERSION_V8:
        data_frame_size = sizeof( ZW_NOTIFICATION_GET_V8_FRAME );
        break;
    default:
        data_frame_size = sizeof( ZW_ALARM_GET_FRAME );
    }
    return znet_fc_send_data(
        node_id, channel_id, data_frame_size, (const uint8_t*)&data, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_notification_set( uint8_t node_id,
                                                         uint8_t channel_id,
                                                         uint8_t type,
                                                         uint8_t status,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg,
                                                         encap_type_t encap_type ) {
    ZW_NOTIFICATION_SET_V8_FRAME data;
    data.cmdClass = COMMAND_CLASS_NOTIFICATION_V8;
    data.cmd = NOTIFICATION_SET_V8;
    data.notificationType = type;
    data.notificationStatus = status;
    return znet_fc_send_data(
        node_id, channel_id, sizeof( data ), (const uint8_t*)&data, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_notification_supported_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type ) {
    ZW_NOTIFICATION_SUPPORTED_GET_V8_FRAME data;
    data.cmdClass = COMMAND_CLASS_NOTIFICATION_V8;
    data.cmd = NOTIFICATION_SUPPORTED_GET_V8;
    return znet_fc_send_data(
        node_id, channel_id, sizeof( data ), (const uint8_t*)&data, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_notification_event_supported_get( uint8_t node_id,
                                                                         uint8_t channel_id,
                                                                         uint8_t type,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type ) {
    ZW_EVENT_SUPPORTED_GET_V8_FRAME data;
    data.cmdClass = COMMAND_CLASS_NOTIFICATION_V8;
    data.cmd = EVENT_SUPPORTED_GET_V8;
    data.notificationType = type;
    return znet_fc_send_data(
        node_id, channel_id, sizeof( data ), (const uint8_t*)&data, Callback, callbackArg, encap_type );
}
