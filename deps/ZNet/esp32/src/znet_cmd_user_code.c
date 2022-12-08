/**
 * @file znet_cmd_user_code.c
 * @date 11 Jun 2019
 * @brief TBD.
 */

/// INFO: crt & system
#include <assert.h>
#include <string.h>

/// INFO: public
#include "znet/znet.h"

/// INFO: private
#include "znet_main.h"
#include "znet_log.h"
#include "znet_store.h"

/// INFO: internal
#include "heap.h"
#include "znet_lib.h"
#include "znet_lib_cc_application.h"
#include "znet_macros.h"
#include "znet_lib_cc_codes.h"
#include "byte_ops.h"

/// INFO: User Code Command Class v1

static const size_t cc_offset = 0;
static const size_t command_offset = 1;

/*********************************************************************************************************************/

void znet_cc_user_code_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    const size_t user_id_offset = 2;
    const size_t user_id_status_offset = 3;
    const size_t user_code_offset = 4;

    assert( cc_data );
    assert( cc_data[cc_offset] == ZNET_COMMAND_CLASS_USER_CODE );
    assert( cc_data[command_offset] == USER_CODE_REPORT );

    const size_t user_code_size = cc_data_len - ( user_code_offset * sizeof( uint8_t ) );

    znet_user_code_report_t user_code_report;
    user_code_report.user_id = cc_data[user_id_offset];
    user_code_report.user_id_status = cc_data[user_id_status_offset];
    user_code_report.user_code_size = user_code_size;
    user_code_report.user_code = cc_data + user_code_offset;

    if( znet_cb->node_cmd_user_code_report ) {
        znet_cb->node_cmd_user_code_report(
            ZNET_ERR_OK, node_id, func->_endpoint, cc_data[user_id_offset], &user_code_report, znet_cb->arg );
    }
}

/*********************************************************************************************************************/

void znet_cc_users_number_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    const size_t supported_users_offset = 2;
    const size_t extended_supported_users_msb_offset = 3;
    const size_t extended_supported_users_lsb_offset = 4;

    assert( cc_data );
    assert( cc_data[cc_offset] == ZNET_COMMAND_CLASS_USER_CODE );
    assert( cc_data[command_offset] == USERS_NUMBER_REPORT );

    znet_supported_users_t supported_users = cc_data[supported_users_offset];
    if( supported_users == 0xFF && cc_data_len >= 5 ) {
        supported_users = cc_data[extended_supported_users_lsb_offset] |
                          ( znet_supported_users_t )( cc_data[extended_supported_users_msb_offset] << 8 );
    }

    if( znet_cb->node_cmd_users_number_report ) {
        znet_cb->node_cmd_users_number_report( ZNET_ERR_OK, node_id, func->_endpoint, supported_users, znet_cb->arg );
    }
}

/*********************************************************************************************************************/

void znet_cc_user_code_capabilities_report( const ZFunction func,
                                            uint8_t node_id,
                                            int cc_data_len,
                                            const uint8_t* cc_data ) {
    assert( cc_data );
    const ZW_USER_CODE_CAPABILITIES_REPORT_FRAME* frame = (const ZW_USER_CODE_CAPABILITIES_REPORT_FRAME*)cc_data;

    assert( frame->cmdClass == ZNET_COMMAND_CLASS_USER_CODE );
    assert( frame->cmd == USER_CODE_CAPABILITIES_REPORT );

    if( znet_cb->node_cmd_user_code_capabilities_result ) {
        const ZW_USER_ID_STATUS_CAPABILITY_SUB_FRAME* userIdStatusSubFrame =
            (const ZW_USER_ID_STATUS_CAPABILITY_SUB_FRAME*)frame->subFrames;

        const size_t keypadModesOffset =
            sizeof( ZW_USER_ID_STATUS_CAPABILITY_SUB_FRAME ) + userIdStatusSubFrame->userIdStatusBitmaskLength;
        const ZW_KEYPAD_MODES_CAPABILITY_SUB_FRAME* keypadModesSubFrame =
            (const ZW_KEYPAD_MODES_CAPABILITY_SUB_FRAME*)( frame->subFrames + keypadModesOffset );

        const size_t keysOffset = keypadModesOffset + sizeof( ZW_KEYPAD_MODES_CAPABILITY_SUB_FRAME ) +
                                  keypadModesSubFrame->keypadModeBitmaskLength;
        const ZW_KEYS_CAPABILITY_SUB_FRAME* keysSubFrame =
            (const ZW_KEYS_CAPABILITY_SUB_FRAME*)( frame->subFrames + keysOffset );

        znet_user_code_capabilities_report_t report = {};

        report.user_id_status_capability.MC_support = userIdStatusSubFrame->McSupport;
        report.user_id_status_capability.MCD_support = userIdStatusSubFrame->McdSupport;
        report.user_id_status_capability.bitmask_length = userIdStatusSubFrame->userIdStatusBitmaskLength;
        report.user_id_status_capability.bitmask = userIdStatusSubFrame->bitmask;

        report.keypad_modes_capability.UCC_support = keypadModesSubFrame->UccSupport;
        report.keypad_modes_capability.MUCR_support = keypadModesSubFrame->MucrSupport;
        report.keypad_modes_capability.MUCS_support = keypadModesSubFrame->MucsSupport;
        report.keypad_modes_capability.bitmask_length = keypadModesSubFrame->keypadModeBitmaskLength;
        report.keypad_modes_capability.bitmask = keypadModesSubFrame->bitmask;

        report.keys_capability.bitmask_length = keysSubFrame->keysBitmasksLength;
        report.keys_capability.bitmask = keysSubFrame->bitmask;

        znet_cb->node_cmd_user_code_capabilities_result( ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
    }
}

/*********************************************************************************************************************/

void znet_cc_user_code_keypad_mode_report( const ZFunction func,
                                           uint8_t node_id,
                                           int cc_data_len,
                                           const uint8_t* cc_data ) {
    assert( cc_data );
    const ZW_KEYPAD_MODE_REPORT_FRAME* frame = (const ZW_KEYPAD_MODE_REPORT_FRAME*)cc_data;

    assert( frame->cmdClass == ZNET_COMMAND_CLASS_USER_CODE );
    assert( frame->cmd == USER_CODE_KEYPAD_MODE_REPORT );

    if( znet_cb->node_cmd_user_codes_keypad_mode_result ) {
        znet_cb->node_cmd_user_codes_keypad_mode_result(
            ZNET_ERR_OK, node_id, func->_endpoint, frame->keypadMode, znet_cb->arg );
    }
}

/*********************************************************************************************************************/

static int user_code_report_comparator( const void* a, const void* b ) {
    return (int)( (const znet_user_code_report_t*)a )->user_id - (int)( (const znet_user_code_report_t*)b )->user_id;
}

void znet_cc_extended_user_code_report( const ZFunction func,
                                        uint8_t node_id,
                                        int cc_data_len,
                                        const uint8_t* cc_data ) {
    assert( cc_data );
    const ZW_EXTENDED_USER_CODES_REPORT_FRAME* frame = (const ZW_EXTENDED_USER_CODES_REPORT_FRAME*)cc_data;

    assert( frame->cmdClass == ZNET_COMMAND_CLASS_USER_CODE );
    assert( frame->cmd == EXTENDED_USER_CODE_REPORT );
    assert( frame->userCodesCount );

    if( znet_cb->node_cmd_extended_user_code_result && frame->userCodesCount ) {
        znet_user_code_report_t reports[frame->userCodesCount];

        const uint8_t* userCodes = frame->userCodes;

        for( uint8_t i = 0; i < frame->userCodesCount; ++i ) {
            const ZW_EXTENDED_USER_CODE_REPORT_SUB_FRAME* sub_frame =
                (const ZW_EXTENDED_USER_CODE_REPORT_SUB_FRAME*)userCodes;

            reports[i].user_code_size = sub_frame->userCodeLength;
            reports[i].user_code = sub_frame->userCode;
            reports[i].user_id_status = sub_frame->userIdStatus;
            reports[i].user_id = sub_frame->userIdLSB | ( uint16_t )( sub_frame->userIdMSB << 8 );

            userCodes += sizeof( ZW_EXTENDED_USER_CODE_REPORT_SUB_FRAME ) + sub_frame->userCodeLength;
        }

        qsort( reports, frame->userCodesCount, sizeof( znet_user_code_report_t ), user_code_report_comparator );

        const ZW_EXTENDED_USER_CODE_NEXT_ID_SUB_FRAME* nextUserIdSubFrame =
            (const ZW_EXTENDED_USER_CODE_NEXT_ID_SUB_FRAME*)userCodes;

        znet_cb->node_cmd_extended_user_code_result(
            ZNET_ERR_OK,
            node_id,
            func->_endpoint,
            reports[0].user_id,
            frame->userCodesCount,
            reports,
            ( znet_supported_users_t )( nextUserIdSubFrame->nextUserIdLSB |
                                        ( nextUserIdSubFrame->nextUserIdMSB << 8 ) ),
            znet_cb->arg );
    }
}

/*********************************************************************************************************************/

void znet_cc_master_code_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );

    const ZW_MASTER_CODE_REPORT_FRAME* frame = (const ZW_MASTER_CODE_REPORT_FRAME*)cc_data;

    assert( frame->cmdClass == ZNET_COMMAND_CLASS_USER_CODE );
    assert( frame->cmd == MASTER_CODE_REPORT );

    if( znet_cb->node_cmd_master_code_result ) {
        znet_cb->node_cmd_master_code_result(
            ZNET_ERR_OK, node_id, func->_endpoint, frame->codeLength, frame->masterCode, znet_cb->arg );
    }
}

/*********************************************************************************************************************/

void znet_cc_user_code_checksum_report( const ZFunction func,
                                        uint8_t node_id,
                                        int cc_data_len,
                                        const uint8_t* cc_data ) {
    assert( cc_data );

    const ZW_USER_CODE_CHECKSUM_REPORT_FRAME* frame = (const ZW_USER_CODE_CHECKSUM_REPORT_FRAME*)cc_data;

    assert( frame->cmdClass == ZNET_COMMAND_CLASS_USER_CODE );
    assert( frame->cmd == USER_CODE_CHECKSUM_REPORT );

    if( znet_cb->node_cmd_user_code_checksum_result ) {
        znet_cb->node_cmd_user_code_checksum_result( ZNET_ERR_OK,
                                                     node_id,
                                                     func->_endpoint,
                                                     frame->checkSumLSB | ( uint16_t )( frame->checkSumMSB << 8 ),
                                                     znet_cb->arg );
    }
}

/*********************************************************************************************************************/

union UserCodeParam {
    uint8_t requested_user_id;
    uint16_t extended_requested_user_id;
    void* data;
};

static void _znet_node_cmd_user_code_cb( ZFunction func,
                                         void* arg,
                                         ZFuncFailures_e reason,
                                         const void* UNUSED( dataPtr ),
                                         size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_user_code_report ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union UserCodeParam param;
        param.data = arg;

        znet_cb->node_cmd_user_code_report(
            znet_get_error( reason ), nodeId, endpoint, param.requested_user_id, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_user_code_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id, uint8_t user_id ) {
    ZNET_LOGI( "USER_CODE_GET!\n" );

    ZNET_CHECK_NODE( node_id )

    union UserCodeParam param;
    param.data = NULL;
    param.requested_user_id = user_id;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_user_code_get( node_id, channel_id, user_id, _znet_node_cmd_user_code_cb, param.data, encap );
}

/*********************************************************************************************************************/

znet_error_t znet_node_cmd_user_code_set( znet_node_id_t node_id,
                                          znet_node_channel_id_t channel_id,
                                          uint8_t user_id,
                                          uint8_t user_id_status,
                                          uint8_t user_code_size,
                                          const uint8_t* user_code ) {
    ZNET_LOGI( "USER_CODE_SET!\n" );

    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_user_code_set(
        node_id, channel_id, user_id, user_id_status, user_code_size, user_code, NULL, NULL, encap );
}

/*********************************************************************************************************************/

static void _znet_node_cmd_users_number_cb( ZFunction func,
                                            void* UNUSED( arg ),
                                            ZFuncFailures_e reason,
                                            const void* UNUSED( dataPtr ),
                                            size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_users_number_report ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );
        znet_cb->node_cmd_users_number_report( znet_get_error( reason ), nodeId, endpoint, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_users_number_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    ZNET_LOGI( "USERS_NUMBER_GET!\n" );

    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_users_number_get( node_id, channel_id, _znet_node_cmd_users_number_cb, NULL, encap );
}

/*********************************************************************************************************************/

static void _znet_node_cmd_user_code_capabilities_cb( ZFunction func,
                                                      void* UNUSED( arg ),
                                                      ZFuncFailures_e reason,
                                                      const void* UNUSED( dataPtr ),
                                                      size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_user_code_capabilities_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );
        znet_cb->node_cmd_user_code_capabilities_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_user_code_capabilities_get( uint8_t node_id, uint8_t channel_id ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_user_code_capabilities_get(
        node_id, channel_id, _znet_node_cmd_user_code_capabilities_cb, NULL, encap );
}

/*********************************************************************************************************************/

znet_error_t znet_node_cmd_user_code_keypad_mode_set( uint8_t node_id, uint8_t channel_id, uint8_t keypad_mode ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_user_code_keypad_mode_set( node_id, channel_id, keypad_mode, NULL, NULL, encap );
}

/*********************************************************************************************************************/

static void _znet_node_cmd_user_code_keypad_mode_cb( ZFunction func,
                                                     void* UNUSED( arg ),
                                                     ZFuncFailures_e reason,
                                                     const void* UNUSED( dataPtr ),
                                                     size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_user_codes_keypad_mode_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );
        znet_cb->node_cmd_user_codes_keypad_mode_result( znet_get_error( reason ), nodeId, endpoint, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_user_code_keypad_mode_get( uint8_t node_id, uint8_t channel_id ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_user_code_keypad_mode_get(
        node_id, channel_id, _znet_node_cmd_user_code_keypad_mode_cb, NULL, encap );
}

/*********************************************************************************************************************/

static void _znet_node_cmd_extended_user_code_cb( ZFunction func,
                                                  void* arg,
                                                  ZFuncFailures_e reason,
                                                  const void* UNUSED( dataPtr ),
                                                  size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_extended_user_code_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union UserCodeParam param;
        param.data = arg;

        znet_cb->node_cmd_extended_user_code_result(
            znet_get_error( reason ), nodeId, endpoint, param.extended_requested_user_id, 0, NULL, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_extended_user_code_get( uint8_t node_id,
                                                   uint8_t channel_id,
                                                   uint16_t user_id,
                                                   bool should_report_more ) {
    ZNET_CHECK_NODE( node_id )

    union UserCodeParam param;
    param.data = NULL;
    param.extended_requested_user_id = user_id;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_extended_user_code_get(
        node_id, channel_id, user_id, should_report_more, _znet_node_cmd_extended_user_code_cb, param.data, encap );
}

/*********************************************************************************************************************/

znet_error_t znet_node_cmd_extended_user_code_set( uint8_t node_id,
                                                   uint8_t channel_id,
                                                   uint8_t user_codes_count,
                                                   const znet_user_code_report_t* user_codes ) {
    ZNET_CHECK_NODE( node_id )

    if( user_codes_count > 10 ) {
        ZNET_LOGW( "It's not recomended to send more than 10 user codes" );
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    uint16_t data_size = 0;
    for( size_t i = 0; i < user_codes_count; ++i ) {
        data_size += user_codes[i].user_code_size + sizeof( ZW_EXTENDED_USER_CODE_SET_SUB_FRAME );

        if( data_size > 0xFF ) {
            ZNET_LOGE( "znet_node_cmd_extended_user_code_set: trying to send too large frame. Please decrease number "
                       "of user codes." );
            return ZNET_ERR_INVAL;
        }
    }

    uint8_t data[data_size];
    uint8_t* data_ptr = data;

    for( size_t i = 0; i < user_codes_count; ++i ) {
        ZW_EXTENDED_USER_CODE_SET_SUB_FRAME* frame = (ZW_EXTENDED_USER_CODE_SET_SUB_FRAME*)data_ptr;

        frame->userIdLSB = user_codes[i].user_id & 0xFF;
        frame->userIdMSB = user_codes[i].user_id >> 8;
        frame->userIdStatus = user_codes[i].user_id_status;
        frame->userCodeLength = user_codes[i].user_code_size;
        memcpy( frame->userCode, user_codes[i].user_code, user_codes[i].user_code_size );

        data_ptr += user_codes[i].user_code_size + sizeof( ZW_EXTENDED_USER_CODE_SET_SUB_FRAME );
    }

    return znet_cc_extended_user_code_set(
        node_id, channel_id, user_codes_count, (uint8_t)data_size, data, NULL, NULL, encap );
}

/*********************************************************************************************************************/

znet_error_t znet_node_cmd_master_code_set( uint8_t node_id,
                                            uint8_t channel_id,
                                            uint8_t master_code_length,
                                            const uint8_t* master_code ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_master_code_set( node_id, channel_id, master_code_length, master_code, NULL, NULL, encap );
}

/*********************************************************************************************************************/

static void _znet_node_cmd_master_code_cb( ZFunction func,
                                           void* UNUSED( arg ),
                                           ZFuncFailures_e reason,
                                           const void* UNUSED( dataPtr ),
                                           size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_master_code_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_master_code_result( znet_get_error( reason ), nodeId, endpoint, 0, NULL, znet_cb->arg );
    }
}

/*********************************************************************************************************************/

static void _znet_node_cmd_checksum_cb( ZFunction func,
                                        void* UNUSED( arg ),
                                        ZFuncFailures_e reason,
                                        const void* UNUSED( dataPtr ),
                                        size_t UNUSED( dataSize ) ) {
    if( znet_cb->node_cmd_user_code_checksum_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_user_code_checksum_result( znet_get_error( reason ), nodeId, endpoint, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_master_code_get( uint8_t node_id, uint8_t channel_id ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_master_code_get( node_id, channel_id, _znet_node_cmd_master_code_cb, NULL, encap );
}

znet_error_t znet_node_cmd_user_code_checksum_get( uint8_t node_id, uint8_t channel_id ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_user_code_checksum_get( node_id, channel_id, _znet_node_cmd_checksum_cb, NULL, encap );
}

static int compare_user_codes_by_id( const void* a, const void* b ) {
    return ( (const znet_user_code_report_t*)a )->user_id - ( (const znet_user_code_report_t*)b )->user_id;
}

znet_user_code_checksum_t calculate_user_codes_checksum( const znet_user_code_report_t* values,
                                                         znet_supported_users_t count ) {
    znet_user_code_report_t* sorted_values = malloc( sizeof( znet_user_code_report_t ) * count );

    memcpy( sorted_values, values, count * sizeof( znet_user_code_report_t ) );
    qsort( sorted_values, count, sizeof( znet_user_code_report_t ), compare_user_codes_by_id );

    znet_user_code_checksum_t result = CRC16_DEFAULT_INIT_VALUE;

    znet_supported_users_t prev_id = 0;
    for( size_t i = 0; i < count; ++i ) {
        if( sorted_values[i].user_id == 0 ) {
            ZNET_LOGW( "Zero user id!!!" );
        }
        else if( prev_id == sorted_values[i].user_id ) {
            ZNET_LOGW( "Duplicated user ids %u!!!", prev_id );
        }

        const uint8_t id_and_status[3] = {
            sorted_values[i].user_id >> 8, sorted_values[i].user_id & 0xFF, sorted_values[i].user_id_status
        };

        result = crc16_with_init_value( id_and_status, 3, result );
        result = crc16_with_init_value( sorted_values[i].user_code, sorted_values[i].user_code_size, result );

        prev_id = sorted_values[i].user_id;
    }

    free( sorted_values );

    return result;
}
