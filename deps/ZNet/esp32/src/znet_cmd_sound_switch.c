#include <assert.h>

#include <znet/znet.h>

#include "znet_main.h"
#include "znet_log.h"
#include "znet_store.h"

#include "heap.h"
#include <znet_lib.h>
#include <znet_lib_fc.h>
#include <znet_lib_cc_application.h>

static inline uint16_t concat_bytes( uint8_t msb, uint8_t lsb ) {
    return ( (uint16_t)msb << 8 ) | lsb;
}

// Tones Number

static void _znet_node_cmd_sound_switch_tones_number_cb( ZFunction func,
                                                         void* UNUSED( arg ),
                                                         ZFuncFailures_e reason,
                                                         const void* UNUSED( dataPtr ),
                                                         size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_sound_switch_tones_number_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_sound_switch_tones_number_result(
            znet_get_error( reason ), nodeId, endpoint, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_sound_switch_tones_number_get( uint8_t node_id, uint8_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_sound_switch_tones_number_get(
        node_id, channel_id, _znet_node_cmd_sound_switch_tones_number_cb, NULL, encap );
}

void znet_cc_sound_switch_tones_number_report( const ZFunction func,
                                               uint8_t node_id,
                                               int cc_data_len,
                                               const uint8_t* cc_data ) {
    const ZW_SOUND_SWITCH_TONES_NUMBER_REPORT_FRAME* data = (const ZW_SOUND_SWITCH_TONES_NUMBER_REPORT_FRAME*)cc_data;

    assert( znet_cb );
    assert( data );
    assert( data->cmdClass == ZNET_COMMAND_CLASS_SOUND_SWITCH );
    assert( data->cmd == SOUND_SWITCH_TONES_NUMBER_REPORT );

    if( znet_cb->node_cmd_sound_switch_tones_number_result ) {
        const znet_cmd_sound_switch_tones_number_t tonesNumber = data->tonesNumber;

        znet_cb->node_cmd_sound_switch_tones_number_result(
            ZNET_ERR_OK, node_id, func->_endpoint, tonesNumber, znet_cb->arg );
    }
}

// Tone Info

union ToneInfo {
    void* data;
    uint8_t tone_id;
};

static void _znet_node_cmd_sound_switch_tone_info_cb( ZFunction func,
                                                      void* arg,
                                                      ZFuncFailures_e reason,
                                                      const void* UNUSED( dataPtr ),
                                                      size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_sound_switch_tone_info_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union ToneInfo tone_info;
        tone_info.data = arg;

        znet_cb->node_cmd_sound_switch_tone_info_result(
            znet_get_error( reason ), nodeId, endpoint, tone_info.tone_id, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_sound_switch_tone_info_get( uint8_t node_id, uint8_t channel_id, uint8_t tone_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    union ToneInfo tone_info;
    tone_info.data = NULL;
    tone_info.tone_id = tone_id;

    return znet_cc_sound_switch_tone_info_get(
        node_id, channel_id, tone_id, _znet_node_cmd_sound_switch_tone_info_cb, tone_info.data, encap );
}

void znet_cc_sound_switch_tone_info_report( const ZFunction func,
                                            uint8_t node_id,
                                            int cc_data_len,
                                            const uint8_t* cc_data ) {
    const ZW_SOUND_SWITCH_TONE_INFO_REPORT_FRAME* data = (const ZW_SOUND_SWITCH_TONE_INFO_REPORT_FRAME*)cc_data;

    assert( znet_cb );
    assert( data );
    assert( data->cmdClass == ZNET_COMMAND_CLASS_SOUND_SWITCH );
    assert( data->cmd == SOUND_SWITCH_TONE_INFO_REPORT );

    if( znet_cb->node_cmd_sound_switch_tone_info_result ) {
        znet_cmd_sound_switch_tone_info_t toneInfo = { .tone_id = data->toneId,
                                                       .tone_duration =
                                                           concat_bytes( data->toneDuration1, data->toneDuration2 ),
                                                       .name_length = data->nameLength,
                                                       .name = data->name };

        znet_cb->node_cmd_sound_switch_tone_info_result(
            ZNET_ERR_OK, node_id, func->_endpoint, toneInfo.tone_id, &toneInfo, znet_cb->arg );
    }
}

// Sound Switch Configuration

znet_error_t znet_node_cmd_sound_switch_configuration_set( uint8_t node_id,
                                                           uint8_t channel_id,
                                                           uint8_t volume,
                                                           uint8_t default_tone_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_sound_switch_configuration_set( node_id, channel_id, volume, default_tone_id, NULL, NULL, encap );
}

static void _znet_node_cmd_sound_switch_configuration_cb( ZFunction func,
                                                          void* UNUSED( arg ),
                                                          ZFuncFailures_e reason,
                                                          const void* UNUSED( dataPtr ),
                                                          size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_sound_switch_configuration_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_sound_switch_configuration_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_sound_switch_configuration_get( uint8_t node_id, uint8_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_sound_switch_configuration_get(
        node_id, channel_id, _znet_node_cmd_sound_switch_configuration_cb, NULL, encap );
}

void znet_cc_sound_switch_configuration_report( const ZFunction func,
                                                uint8_t node_id,
                                                int cc_data_len,
                                                const uint8_t* cc_data ) {
    const ZW_SOUND_SWITCH_CONFIGURATION_REPORT_FRAME* data = (const ZW_SOUND_SWITCH_CONFIGURATION_REPORT_FRAME*)cc_data;

    assert( znet_cb );
    assert( data );
    assert( data->cmdClass == ZNET_COMMAND_CLASS_SOUND_SWITCH );
    assert( data->cmd == SOUND_SWITCH_CONFIGURATION_REPORT );

    if( znet_cb->node_cmd_sound_switch_configuration_result ) {
        const znet_cmd_sound_switch_configuration_t configuration = { .volume = data->volume,
                                                                      .default_tone_id = data->defaultToneId };

        znet_cb->node_cmd_sound_switch_configuration_result(
            ZNET_ERR_OK, node_id, func->_endpoint, &configuration, znet_cb->arg );
    }
}

// Tone Play

znet_error_t znet_node_cmd_sound_switch_tone_play_set( uint8_t node_id,
                                                       uint8_t channel_id,
                                                       uint8_t tone_id,
                                                       uint8_t volume ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_sound_switch_tone_play_set( node_id, channel_id, tone_id, volume, NULL, NULL, encap );
}

static void _znet_node_cmd_sound_switch_tone_play_get_cb( ZFunction func,
                                                          void* UNUSED( arg ),
                                                          ZFuncFailures_e reason,
                                                          const void* UNUSED( dataPtr ),
                                                          size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_sound_switch_tone_play_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_sound_switch_tone_play_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_sound_switch_tone_play_get( uint8_t node_id, uint8_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_sound_switch_tone_play_get(
        node_id, channel_id, _znet_node_cmd_sound_switch_tone_play_get_cb, NULL, encap );
}

static void znet_cc_sound_switch_tone_play_report_v1( const ZFunction func, uint8_t node_id, const uint8_t* cc_data ) {
    const ZW_SOUND_SWITCH_TONE_PLAY_REPORT_V1_FRAME* data = (const ZW_SOUND_SWITCH_TONE_PLAY_REPORT_V1_FRAME*)cc_data;

    assert( znet_cb );
    assert( data );
    assert( data->cmdClass == ZNET_COMMAND_CLASS_SOUND_SWITCH );
    assert( data->cmd == SOUND_SWITCH_TONE_PLAY_REPORT );

    if( znet_cb->node_cmd_sound_switch_tone_play_result ) {
        const znet_cmd_sound_switch_tone_play_t tonePlay = { .tone_id = data->toneId, .volume = 0 };

        znet_cb->node_cmd_sound_switch_tone_play_result(
            ZNET_ERR_OK, node_id, func->_endpoint, &tonePlay, znet_cb->arg );
    }
}

static void znet_cc_sound_switch_tone_play_report_v2( const ZFunction func, uint8_t node_id, const uint8_t* cc_data ) {
    const ZW_SOUND_SWITCH_TONE_PLAY_REPORT_V2_FRAME* data = (const ZW_SOUND_SWITCH_TONE_PLAY_REPORT_V2_FRAME*)cc_data;

    assert( znet_cb );
    assert( data );
    assert( data->cmdClass == ZNET_COMMAND_CLASS_SOUND_SWITCH );
    assert( data->cmd == SOUND_SWITCH_TONE_PLAY_REPORT );

    if( znet_cb->node_cmd_sound_switch_tone_play_result ) {
        const znet_cmd_sound_switch_tone_play_t tonePlay = { .tone_id = data->toneId, .volume = data->volume };

        znet_cb->node_cmd_sound_switch_tone_play_result(
            ZNET_ERR_OK, node_id, func->_endpoint, &tonePlay, znet_cb->arg );
    }
}

void znet_cc_sound_switch_tone_play_report( const ZFunction func,
                                            uint8_t node_id,
                                            int cc_data_len,
                                            const uint8_t* cc_data ) {
    switch( cc_data_len ) {
    case sizeof( ZW_SOUND_SWITCH_TONE_PLAY_REPORT_V1_FRAME ):
        znet_cc_sound_switch_tone_play_report_v1( func, node_id, cc_data );
        break;

    case sizeof( ZW_SOUND_SWITCH_TONE_PLAY_REPORT_V2_FRAME ):
        znet_cc_sound_switch_tone_play_report_v2( func, node_id, cc_data );
        break;

    default:
        assert( 0 );
    }
}
