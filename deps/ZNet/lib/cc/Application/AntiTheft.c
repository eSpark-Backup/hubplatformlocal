#include "typedefs.h"

#include "byte_ops.h"

//#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"
//#include "znet_fc_private.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

znet_error_t znet_cc_anti_theft_set( uint8_t node_id,
                                     uint8_t channel_id,
                                     bool enabled,
                                     uint8_t magic_code_size,
                                     const uint8_t* magic_code,
                                     uint16_t manufacturer_id,
                                     uint8_t hint_size,
                                     const uint8_t* hint,
                                     uint16_t zwave_locking_id,
                                     ZFunction_Callback callback,
                                     void* callbackArg,
                                     encap_type_t encap_type ) {
    uint8_t buffer[sizeof( ZW_ANTITHEFT_SET_FRAME ) + magic_code_size + sizeof( ZW_ANTITHEFT_SET_SUB_FRAME ) +
                   hint_size + sizeof( ZW_ANTITHEFT_SET_SUB_FRAME_V3 )];
    memset( buffer, 0, sizeof( buffer ) );
    uint8_t size = 0;

    ZW_ANTITHEFT_SET_FRAME* frame = (ZW_ANTITHEFT_SET_FRAME*)buffer;
    frame->cmdClass = ZNET_COMMAND_CLASS_ANTITHEFT;
    frame->cmd = ANTITHEFT_SET;
    frame->enabled = enabled;
    frame->magicCodeLength = magic_code_size;
    memcpy( frame->magicCode, magic_code, magic_code_size );
    size += sizeof( ZW_ANTITHEFT_SET_FRAME ) + magic_code_size;

    ZW_ANTITHEFT_SET_SUB_FRAME* sub_frame = (ZW_ANTITHEFT_SET_SUB_FRAME*)( buffer + size );
    sub_frame->manufacturerIdLsb = manufacturer_id & 0xFF;
    sub_frame->manufacturerIdMsb = manufacturer_id >> 8;
    sub_frame->antiTheftHintLength = hint_size;
    memcpy( sub_frame->antiTheftHint, hint, hint_size );
    size += sizeof( ZW_ANTITHEFT_SET_SUB_FRAME ) + hint_size;

    ZW_ANTITHEFT_SET_SUB_FRAME_V3* sub_frame_v3 = (ZW_ANTITHEFT_SET_SUB_FRAME_V3*)( buffer + size );
    sub_frame_v3->zwaveAllianceLockingEntityIdLSB = zwave_locking_id & 0xFF;
    sub_frame_v3->zwaveAllianceLockingEntityIdMSB = zwave_locking_id >> 8;
    size += sizeof( ZW_ANTITHEFT_SET_SUB_FRAME_V3 );

    return znet_fc_send_data( node_id, channel_id, size, buffer, callback, callbackArg, encap_type );
}

znet_error_t znet_cc_anti_theft_get( uint8_t node_id,
                                     uint8_t channel_id,
                                     ZFunction_Callback callback,
                                     void* callbackArg,
                                     encap_type_t encap_type ) {
    ZW_ANTITHEFT_GET_FRAME frame;
    frame.cmdClass = ZNET_COMMAND_CLASS_ANTITHEFT;
    frame.cmd = ANTITHEFT_GET;

    return znet_fc_send_data( node_id,
                              channel_id,
                              sizeof( ZW_ANTITHEFT_GET_FRAME ),
                              (const uint8_t*)&frame,
                              callback,
                              callbackArg,
                              encap_type );
}

znet_error_t znet_cc_anti_theft_unlock_set( uint8_t node_id,
                                            uint8_t channel_id,
                                            uint8_t magic_code_size,
                                            const uint8_t* magic_code,
                                            ZFunction_Callback callback,
                                            void* callbackArg,
                                            encap_type_t encap_type ) {
    const uint8_t size = sizeof( ZW_ANTITHEFT_UNLOCK_SET_FRAME ) + magic_code_size;
    uint8_t buffer[size];
    memset( buffer, 0, sizeof( buffer ) );

    ZW_ANTITHEFT_UNLOCK_SET_FRAME* frame = (ZW_ANTITHEFT_UNLOCK_SET_FRAME*)buffer;
    frame->cmdClass = ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK;
    frame->cmd = ANTITHEFT_UNLOCK_SET;
    frame->magicCodeLength = magic_code_size;
    memcpy( frame->magicCode, magic_code, magic_code_size );

    return znet_fc_send_data( node_id, channel_id, size, buffer, callback, callbackArg, encap_type );
}

znet_error_t znet_cc_anti_theft_unlock_state_get( uint8_t node_id,
                                                  uint8_t channel_id,
                                                  ZFunction_Callback callback,
                                                  void* callbackArg,
                                                  encap_type_t encap_type ) {
    ZW_ANTITHEFT_UNLOCK_STATE_GET_FRAME frame;
    frame.cmdClass = ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK;
    frame.cmd = ANTITHEFT_UNLOCK_STATE_GET;

    return znet_fc_send_data( node_id,
                              channel_id,
                              sizeof( ZW_ANTITHEFT_UNLOCK_STATE_GET_FRAME ),
                              (const uint8_t*)&frame,
                              callback,
                              callbackArg,
                              encap_type );
}
