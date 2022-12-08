//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "znet/znet_errors.h"

#include "ZPlatform.h"
#include "ZFunctionClass.h"
#include "znet_lib_cc_codes.h"
#include "znet_lib_fc.h"

#include <assert.h>

//////////////////////////////////////////////////////////////////////////////

// Tones Number

znet_error_t ICACHE_FLASH_ATTR znet_cc_sound_switch_tones_number_get( uint8_t node_id,
                                                                      uint8_t channel_id,
                                                                      ZFunction_Callback Callback,
                                                                      void* callbackArg,
                                                                      encap_type_t encap_type ) {
    const ZW_SOUND_SWITCH_TONES_NUMBER_GET_FRAME data = { .cmdClass = COMMAND_CLASS_SOUND_SWITCH,
                                                          .cmd = SOUND_SWITCH_TONES_NUMBER_GET };

    return znet_fc_send_data( node_id, channel_id, sizeof data, (uint8_t*)&data, Callback, callbackArg, encap_type );
}

// Tone Info

znet_error_t ICACHE_FLASH_ATTR znet_cc_sound_switch_tone_info_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   uint8_t tone_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type ) {
    const ZW_SOUND_SWITCH_TONE_INFO_GET_FRAME data = {
        .cmdClass = COMMAND_CLASS_SOUND_SWITCH, .cmd = SOUND_SWITCH_TONE_INFO_GET, .toneId = tone_id
    };

    return znet_fc_send_data( node_id, channel_id, sizeof data, (uint8_t*)&data, Callback, callbackArg, encap_type );
}

// Configuration

znet_error_t ICACHE_FLASH_ATTR znet_cc_sound_switch_configuration_set( uint8_t node_id,
                                                                       uint8_t channel_id,
                                                                       uint8_t volume,
                                                                       uint8_t default_tone_id,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type ) {
    const ZW_SOUND_SWITCH_CONFIGURATION_SET_FRAME data = { .cmdClass = COMMAND_CLASS_SOUND_SWITCH,
                                                           .cmd = SOUND_SWITCH_CONFIGURATION_SET,
                                                           .volume = volume,
                                                           .defaultToneId = default_tone_id };

    return znet_fc_send_data( node_id, channel_id, sizeof data, (uint8_t*)&data, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_sound_switch_configuration_get( uint8_t node_id,
                                                                       uint8_t channel_id,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type ) {
    const ZW_SOUND_SWITCH_CONFIGURATION_GET_FRAME data = { .cmdClass = COMMAND_CLASS_SOUND_SWITCH,
                                                           .cmd = SOUND_SWITCH_CONFIGURATION_GET };

    return znet_fc_send_data( node_id, channel_id, sizeof data, (uint8_t*)&data, Callback, callbackArg, encap_type );
}

// Tone Play

znet_error_t ICACHE_FLASH_ATTR znet_cc_sound_switch_tone_play_set( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   uint8_t tone_id,
                                                                   uint8_t volume,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type ) {
    const ZW_SOUND_SWITCH_TONE_PLAY_SET_V2_FRAME data = {
        .cmdClass = COMMAND_CLASS_SOUND_SWITCH, .cmd = SOUND_SWITCH_TONE_PLAY_SET, .toneId = tone_id, .volume = volume
    };

    return znet_fc_send_data( node_id, channel_id, sizeof data, (uint8_t*)&data, Callback, callbackArg, encap_type );
}

znet_error_t ICACHE_FLASH_ATTR znet_cc_sound_switch_tone_play_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type ) {
    const ZW_SOUND_SWITCH_TONE_PLAY_GET_FRAME data = { .cmdClass = COMMAND_CLASS_SOUND_SWITCH,
                                                       .cmd = SOUND_SWITCH_TONE_PLAY_GET };

    return znet_fc_send_data( node_id, channel_id, sizeof data, (uint8_t*)&data, Callback, callbackArg, encap_type );
}
