#include "znet/znet.h"
#include "znet_main.h"
#include "znet_log.h"
#include "znet_lib_cc_application.h"

static void _znet_node_cmd_silence_alarm_set_cb( ZFunction func,
                                                 void* arg,
                                                 ZFuncFailures_e reason,
                                                 const void* dataPtr,
                                                 size_t dataSize ) {
    if( reason == FUNC_OK ) {
        ZNET_LOGI( "znet_node_cmd_alarm_silence_set succ res\n" );
    }
    else {
        ZNET_LOGE( "znet_node_cmd_alarm_silence_set fail res: %i\n", reason );
    }
}

znet_error_t znet_node_cmd_alarm_silence_set( znet_node_id_t node_id,
                                              znet_node_channel_id_t channelId,
                                              uint8_t mode,
                                              uint16_t delay,
                                              uint8_t sensors_mask_size,
                                              const uint8_t* sensors_mask ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_silence_alarm_set( node_id,
                                      channelId,
                                      mode,
                                      delay,
                                      sensors_mask_size,
                                      sensors_mask,
                                      _znet_node_cmd_silence_alarm_set_cb,
                                      NULL,
                                      encap );
}
