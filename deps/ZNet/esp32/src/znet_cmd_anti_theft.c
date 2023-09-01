#include <znet/znet.h>

#include "znet_main.h"
#include "znet_log.h"
#include "lib/znet_lib_cc_application.h"

void znet_cc_anti_theft_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    if( znet_cb->node_cmd_anti_theft_result ) {
        if( cc_data_len < sizeof( ZW_ANTITHEFT_REPORT_FRAME ) ) {
            znet_cb->node_cmd_anti_theft_result( ZNET_ERR_INV_RES, node_id, func->_endpoint, NULL, znet_cb->arg );
            return;
        }

        znet_cmd_anti_theft_report_t report = {};

        const ZW_ANTITHEFT_REPORT_FRAME* frame = (const ZW_ANTITHEFT_REPORT_FRAME*)cc_data;

        if( frame->antiTheftProtectionStatus < 0x01 || frame->antiTheftProtectionStatus > 0x03 ) {
            ZNET_LOGW( "Anti-theft protection status = %u, how to handle it?\n", frame->antiTheftProtectionStatus );
        }

        report.is_locked = frame->antiTheftProtectionStatus == 0x02 || frame->antiTheftProtectionStatus == 0x03;
        report.is_restricted = frame->antiTheftProtectionStatus == 0x03;
        report.hint_length = frame->antiTheftHintLength;
        report.hint = frame->antiTheftHint;
        report.manufacturer_id =
            ( uint16_t )( ( frame->manufacturerIdLSB & 0xFF ) | ( frame->manufacturerIdMSB << 8 ) );

        const uint8_t sub_frame_offset = sizeof( ZW_ANTITHEFT_REPORT_FRAME ) + frame->antiTheftHintLength;
        if( cc_data_len >= sub_frame_offset + sizeof( ZW_ANTITHEFT_REPORT_SUB_FRAME_V3 ) ) {
            const ZW_ANTITHEFT_REPORT_SUB_FRAME_V3* sub_frame =
                (const ZW_ANTITHEFT_REPORT_SUB_FRAME_V3*)( cc_data + sub_frame_offset );
            report.zwave_locking_id = ( uint16_t )( ( sub_frame->zwaveAllianceLockingEntityIdLSB & 0xFF ) |
                                                    ( sub_frame->zwaveAllianceLockingEntityIdMSB << 8 ) );
        }

        znet_cb->node_cmd_anti_theft_result( ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
    }
}

void znet_cc_anti_theft_unlock_state_report( const ZFunction func,
                                             uint8_t node_id,
                                             int cc_data_len,
                                             const uint8_t* cc_data ) {
    if( znet_cb->node_cmd_anti_theft_unlock_state_result ) {
        if( cc_data_len < sizeof( ZW_ANTITHEFT_UNLOCK_STATE_REPORT_FRAME ) ) {
            znet_cb->node_cmd_anti_theft_unlock_state_result(
                ZNET_ERR_INV_RES, node_id, func->_endpoint, NULL, znet_cb->arg );
            return;
        }

        znet_cmd_anti_theft_unlock_state_report_t report = {};

        const ZW_ANTITHEFT_UNLOCK_STATE_REPORT_FRAME* frame = (const ZW_ANTITHEFT_UNLOCK_STATE_REPORT_FRAME*)cc_data;

        report.is_locked = frame->locked;
        report.is_restricted = frame->restricted;
        report.hint_length = frame->antiTheftHintLength;
        report.hint = frame->antiTheftHint;

        uint8_t sub_frame_offset = sizeof( ZW_ANTITHEFT_UNLOCK_STATE_REPORT_FRAME ) + frame->antiTheftHintLength;

        if( cc_data_len < sub_frame_offset + sizeof( ZW_ANTITHEFT_UNLOCK_STATE_REPORT_SUB_FRAME ) ) {
            znet_cb->node_cmd_anti_theft_unlock_state_result(
                ZNET_ERR_INV_RES, node_id, func->_endpoint, NULL, znet_cb->arg );
            return;
        }

        const ZW_ANTITHEFT_UNLOCK_STATE_REPORT_SUB_FRAME* sub_frame =
            (const ZW_ANTITHEFT_UNLOCK_STATE_REPORT_SUB_FRAME*)( cc_data + sub_frame_offset );

        report.manufacturer_id =
            ( uint16_t )( ( sub_frame->manufacturerIdLSB & 0xFF ) | ( sub_frame->manufacturerIdMSB << 8 ) );

        sub_frame_offset += sizeof( ZW_ANTITHEFT_UNLOCK_STATE_REPORT_SUB_FRAME );

        if( cc_data_len >= sub_frame_offset + sizeof( ZW_ANTITHEFT_REPORT_SUB_FRAME_V3 ) ) {
            const ZW_ANTITHEFT_REPORT_SUB_FRAME_V3* sub_frame =
                (const ZW_ANTITHEFT_REPORT_SUB_FRAME_V3*)( cc_data + sub_frame_offset );
            report.zwave_locking_id = ( uint16_t )( ( sub_frame->zwaveAllianceLockingEntityIdLSB & 0xFF ) |
                                                    ( sub_frame->zwaveAllianceLockingEntityIdMSB << 8 ) );
        }

        znet_cb->node_cmd_anti_theft_unlock_state_result(
            ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_anti_theft_set( znet_node_id_t node_id,
                                           znet_node_channel_id_t channel_id,
                                           bool enabled,
                                           uint8_t magic_code_size,
                                           const uint8_t* magic_code,
                                           uint16_t manufacturer_id,
                                           uint8_t hint_size,
                                           const uint8_t* hint,
                                           uint16_t zwave_locking_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_anti_theft_set( node_id,
                                   channel_id,
                                   enabled,
                                   magic_code_size,
                                   magic_code,
                                   enabled ? manufacturer_id : 0,
                                   hint_size,
                                   hint,
                                   enabled ? zwave_locking_id : 0,
                                   NULL,
                                   NULL,
                                   encap );
}

static void _znet_node_cmd_anti_theft_get_cb( ZFunction func,
                                              void* arg,
                                              ZFuncFailures_e reason,
                                              const void* dataPtr,
                                              size_t dataSize ) {
    if( znet_cb->node_cmd_anti_theft_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_anti_theft_result( znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_anti_theft_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_anti_theft_get( node_id, channel_id, _znet_node_cmd_anti_theft_get_cb, NULL, encap );
}

static void _znet_node_cmd_anti_theft_unlock_state_get_cb( ZFunction func,
                                                           void* arg,
                                                           ZFuncFailures_e reason,
                                                           const void* dataPtr,
                                                           size_t dataSize ) {
    if( znet_cb->node_cmd_anti_theft_unlock_state_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_anti_theft_unlock_state_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_anti_theft_unlock_state_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_anti_theft_unlock_state_get(
        node_id, channel_id, _znet_node_cmd_anti_theft_unlock_state_get_cb, NULL, encap );
}

znet_error_t znet_node_cmd_anti_theft_unlock_set( znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id,
                                                  uint8_t magic_code_size,
                                                  const uint8_t* magic_code ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_anti_theft_unlock_set( node_id, channel_id, magic_code_size, magic_code, NULL, NULL, encap );
}
