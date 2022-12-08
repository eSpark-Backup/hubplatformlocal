/// INFO: crt & system
#include <assert.h>
#include <string.h>

/// INFO: public
#include <znet/znet.h>

/// INFO: private
#include "znet_main.h"
#include "znet_log.h"
#include "znet_store.h"

/// INFO: internal
#include "heap.h"
#include <znet_lib.h>
#include <znet_lib_cc_management.h>
#include "znet_macros.h"

#include "lib/byte_ops.h"

static void _znet_node_cmd_firmware_md_get_cb( ZFunction func,
                                               void* arg,
                                               ZFuncFailures_e reason,
                                               const void* dataPtr,
                                               size_t dataSize ) {
    if( znet_cb->node_cmd_firmware_md_result ) {
        uint8_t nodeId, channelId;
        ZNet_CheckCallback( func, reason, &nodeId, &channelId );

        znet_cb->node_cmd_firmware_md_result(
            znet_get_error( reason ), nodeId, channelId, NULL, NULL, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_firmware_md_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_firmware_md_get( node_id, channel_id, _znet_node_cmd_firmware_md_get_cb, NULL, encap );
}

static void _znet_node_cmd_firmware_update_md_request_get_cb( ZFunction func,
                                                              void* arg,
                                                              ZFuncFailures_e reason,
                                                              const void* dataPtr,
                                                              size_t dataSize ) {
    if( znet_cb->node_cmd_firmware_update_md_request_result ) {
        uint8_t nodeId, channelId;
        ZNet_CheckCallback( func, reason, &nodeId, &channelId );

        znet_cb->node_cmd_firmware_update_md_request_result(
            znet_get_error( reason ), nodeId, channelId, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_firmware_update_md_request_get(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_firmware_update_md_request_get_params_v1* params_v1,
    const znet_firmware_update_md_request_get_params_v3* params_v3_opt,
    const znet_firmware_update_md_request_get_params_v4* params_v4_opt,
    const znet_firmware_update_md_request_get_params_v5* params_v5_opt ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_firmware_update_md_request_get( node_id,
                                                   channel_id,
                                                   params_v1->manufacturer_id,
                                                   params_v1->firmware_id,
                                                   params_v1->checksum,
                                                   params_v3_opt ? params_v3_opt->firmware_target : 0,
                                                   params_v3_opt ? params_v3_opt->fragment_size : 0,
                                                   params_v4_opt && params_v4_opt->is_postponed_activation_allowed,
                                                   params_v5_opt ? params_v5_opt->hardware_revision : 0,
                                                   params_v5_opt ? 5 : params_v4_opt ? 4 : params_v3_opt ? 3 : 1,
                                                   _znet_node_cmd_firmware_update_md_request_get_cb,
                                                   NULL,
                                                   encap );
}

union firmware_update_md_get_param {
    struct {
        uint8_t reports_count;
        uint16_t report_number;
    };
    void* data;
};

static void _znet_node_cmd_firmware_update_md_get_cb( ZFunction func,
                                                      void* arg,
                                                      ZFuncFailures_e reason,
                                                      const void* dataPtr,
                                                      size_t dataSize ) {
    if( znet_cb->node_cmd_firmware_update_md_result ) {
        uint8_t nodeId, channelId;
        ZNet_CheckCallback( func, reason, &nodeId, &channelId );

        union firmware_update_md_get_param param;
        param.data = arg;

        znet_cb->node_cmd_firmware_update_md_result(
            znet_get_error( reason ), nodeId, channelId, param.report_number, false, 0, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_firmware_update_md_get( znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   uint8_t reports_count,
                                                   uint16_t report_number ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    union firmware_update_md_get_param param;
    param.reports_count = reports_count;
    param.report_number = report_number;

    return znet_cc_firmware_update_md_get( node_id,
                                           channel_id,
                                           reports_count,
                                           report_number,
                                           _znet_node_cmd_firmware_update_md_get_cb,
                                           param.data,
                                           encap );
}

znet_error_t znet_node_cmd_firmware_update_md_report( znet_node_id_t node_id,
                                                      znet_node_channel_id_t channel_id,
                                                      uint16_t report_number,
                                                      bool is_last_report,
                                                      uint8_t data_size,
                                                      const uint8_t* data,
                                                      bool use_checksum ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_firmware_update_md_report(
        node_id, channel_id, report_number, is_last_report, data_size, data, use_checksum, NULL, NULL, encap );
}

union firmware_update_activation_set_params {
    struct {
        uint8_t firmware_target;
    };

    void* data;
};

static void _znet_node_cmd_firmware_update_activation_set_cb( ZFunction func,
                                                              void* arg,
                                                              ZFuncFailures_e reason,
                                                              const void* dataPtr,
                                                              size_t dataSize ) {
    if( znet_cb->node_cmd_firmware_update_md_activation_status_result ) {
        uint8_t nodeId, channelId;
        ZNet_CheckCallback( func, reason, &nodeId, &channelId );

        union firmware_update_activation_set_params params;
        params.data = arg;

        znet_cb->node_cmd_firmware_update_md_activation_status_result(
            znet_get_error( reason ), nodeId, channelId, params.firmware_target, NULL, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_firmware_update_activation_set(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_firmware_update_activation_set_params_v4* params_v4,
    const znet_firmware_update_activation_set_params_v5* params_v5_opt ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    union firmware_update_activation_set_params params;
    params.firmware_target = params_v4->firmware_target;

    return znet_cc_firmware_update_activation_set( node_id,
                                                   channel_id,
                                                   params_v4->firmware.manufacturer_id,
                                                   params_v4->firmware.firmware_id,
                                                   params_v4->firmware.checksum,
                                                   params_v4->firmware_target,
                                                   params_v5_opt ? params_v5_opt->hardware_version : 0,
                                                   params_v5_opt ? 5 : 4,
                                                   _znet_node_cmd_firmware_update_activation_set_cb,
                                                   params.data,
                                                   encap );
}

static void _znet_node_cmd_firmware_update_md_prepare_get_cb( ZFunction func,
                                                              void* arg,
                                                              ZFuncFailures_e reason,
                                                              const void* dataPtr,
                                                              size_t dataSize ) {
    if( znet_cb->node_cmd_firmware_update_md_preparation_result ) {
        uint8_t nodeId, channelId;
        ZNet_CheckCallback( func, reason, &nodeId, &channelId );

        znet_cb->node_cmd_firmware_update_md_preparation_result(
            znet_get_error( reason ), nodeId, channelId, 0, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_firmware_update_md_prepare_get( znet_node_id_t node_id,
                                                           znet_node_channel_id_t channel_id,
                                                           uint16_t manufacturer_id,
                                                           uint16_t firmware_id,
                                                           uint8_t firmware_target,
                                                           uint16_t fragment_size,
                                                           uint8_t hardware_version ) {
    ZNET_CHECK_NODE( node_id )

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_firmware_update_md_prepare_get( node_id,
                                                   channel_id,
                                                   manufacturer_id,
                                                   firmware_id,
                                                   firmware_target,
                                                   fragment_size,
                                                   hardware_version,
                                                   _znet_node_cmd_firmware_update_md_prepare_get_cb,
                                                   NULL,
                                                   encap );
}

void znet_cc_firmware_md_requested( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    uint16_t additional_firmware[FW_MAX_COUNT] = {
        0,
    };

    znet_firmware_md_report_v1_t report_v1 = {};
    report_v1.checksum = CRC16_DEFAULT_INIT_VALUE;

    znet_firmware_md_report_v3_t report_v3 = {};
    report_v3.additional_firmware = additional_firmware;

    znet_firmware_md_report_v5_t report_v5 = {};

    if( znet_cb->node_cmd_firmware_update_info_fill ) {
        znet_cb->node_cmd_firmware_update_info_fill( &report_v1, &report_v3, &report_v5, znet_cb->arg );
    }

    znet_cc_firmware_update_info_report( node_id,
                                         func->_endpoint,
                                         report_v1.manufacturer_id,
                                         report_v1.firmware_id,
                                         report_v1.checksum,
                                         report_v3.firmware_upgradable,
                                         report_v3.max_fragment_size,
                                         report_v3.additional_firmware_count,
                                         report_v3.additional_firmware,
                                         report_v5.hardware_revision,
                                         NULL,
                                         NULL,
                                         func->_security );
}

void znet_cc_firmware_md_received( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( sizeof( ZW_FIRMWARE_MD_REPORT_V1_FRAME ) <= (size_t)cc_data_len );

    if( znet_cb->node_cmd_firmware_md_result ) {
        const ZW_FIRMWARE_MD_REPORT_V1_FRAME* frame_v1 = (const ZW_FIRMWARE_MD_REPORT_V1_FRAME*)cc_data;

        znet_firmware_md_report_v1_t report_v1 = {};
        znet_firmware_md_report_v3_t report_v3 = {};
        znet_firmware_md_report_v5_t report_v5 = {};

        report_v1.checksum = frame_v1->checksumLsb | ( uint16_t )( frame_v1->checksumMsb << 8 );
        report_v1.firmware_id = frame_v1->firmwareIdLsb | ( uint16_t )( frame_v1->firmwareIdMsb << 8 );
        report_v1.manufacturer_id = frame_v1->manufacturerIdLsb | ( uint16_t )( frame_v1->manufacturerIdMsb << 8 );

        uint8_t offset = sizeof( ZW_FIRMWARE_MD_REPORT_V1_FRAME );

        const bool has_v3 = (size_t)cc_data_len >= sizeof( ZW_FIRMWARE_MD_REPORT_V3_SUBFRAME ) + offset;

        if( has_v3 ) {
            const ZW_FIRMWARE_MD_REPORT_V3_SUBFRAME* subframe =
                (const ZW_FIRMWARE_MD_REPORT_V3_SUBFRAME*)( cc_data + offset );
            report_v3.firmware_upgradable = subframe->firmwareUpgradable == 0xFF;
            report_v3.additional_firmware_count = subframe->numberOfFirmwareTargets;
            report_v3.max_fragment_size =
                subframe->maxFragmentSizeLsb | ( uint16_t )( subframe->maxFragmentSizeMsb << 8 );

            offset += sizeof( ZW_FIRMWARE_MD_REPORT_V3_SUBFRAME );
        }

        uint16_t targets[report_v3.additional_firmware_count];
        if( report_v3.additional_firmware_count ) {
            const ZW_FIRMWARE_MD_REPORT_TARGET_SUBFRAME* target_subframes =
                (const ZW_FIRMWARE_MD_REPORT_TARGET_SUBFRAME*)( cc_data + offset );
            for( size_t i = 0; i < report_v3.additional_firmware_count; ++i ) {
                targets[i] = target_subframes[i].firmwareIdLsb | ( uint16_t )( target_subframes[i].firmwareIdMsb << 8 );
            }

            report_v3.additional_firmware = targets;

            offset += report_v3.additional_firmware_count * sizeof( ZW_FIRMWARE_MD_REPORT_TARGET_SUBFRAME );
        }

        const bool has_v5 = has_v3 && (size_t)cc_data_len >= sizeof( ZW_FIRMWARE_MD_REPORT_V5_SUBFRAME ) + offset;

        if( has_v5 ) {
            const ZW_FIRMWARE_MD_REPORT_V5_SUBFRAME* subframe =
                (const ZW_FIRMWARE_MD_REPORT_V5_SUBFRAME*)( cc_data + offset );
            report_v5.hardware_revision = subframe->hardwareVersion;
        }

        znet_cb->node_cmd_firmware_md_result( ZNET_ERR_OK,
                                              node_id,
                                              func->_endpoint,
                                              &report_v1,
                                              has_v3 ? &report_v3 : NULL,
                                              has_v5 ? &report_v5 : NULL,
                                              znet_cb->arg );
    }
}

void znet_cc_firmware_update_md_request_report_received( const ZFunction func,
                                                         uint8_t node_id,
                                                         int cc_data_len,
                                                         const uint8_t* cc_data ) {
    assert( cc_data );
    assert( sizeof( ZW_FIRMWARE_UPDATE_MD_REQUEST_REPORT_FRAME ) <= (size_t)cc_data_len );

    if( znet_cb->node_cmd_firmware_update_md_request_result ) {
        const ZW_FIRMWARE_UPDATE_MD_REQUEST_REPORT_FRAME* frame =
            (const ZW_FIRMWARE_UPDATE_MD_REQUEST_REPORT_FRAME*)cc_data;

        znet_cb->node_cmd_firmware_update_md_request_result(
            ZNET_ERR_OK, node_id, func->_endpoint, frame->status, znet_cb->arg );
    }
}

void znet_cc_firmware_update_md_request_received( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data ) {
    znet_cc_firmware_update_md_request_report(
        node_id, func->_endpoint, FIRMWARE_UPDATE_MD_REQUEST_REPORT_NOT_UPGRADABLE_V5, NULL, NULL, func->_security );
}

void znet_cc_firmware_update_md_requested( const ZFunction func,
                                           uint8_t node_id,
                                           int cc_data_len,
                                           const uint8_t* cc_data ) {
    assert( cc_data );
    assert( sizeof( ZW_FIRMWARE_UPDATE_MD_GET_FRAME ) <= (size_t)cc_data_len );

    if( znet_cb->node_cmd_firmware_update_md_requested ) {
        const ZW_FIRMWARE_UPDATE_MD_GET_FRAME* frame = (const ZW_FIRMWARE_UPDATE_MD_GET_FRAME*)cc_data;

        znet_cb->node_cmd_firmware_update_md_requested(
            ZNET_ERR_OK,
            node_id,
            func->_endpoint,
            frame->numberOfReports,
            frame->reportNumberLsb | ( uint16_t )( frame->reportNumberMsb << 8 ),
            znet_cb->arg );
    }
}

void znet_cc_firmware_update_md_received( const ZFunction func,
                                          uint8_t node_id,
                                          int cc_data_len,
                                          const uint8_t* cc_data ) {
    assert( cc_data );
    assert( sizeof( ZW_FIRMWARE_UPDATE_MD_REPORT_V1_FRAME ) <= (size_t)cc_data_len );

    if( znet_cb->node_cmd_firmware_update_md_result ) {
        const ZW_FIRMWARE_UPDATE_MD_REPORT_V1_FRAME* frame_v1 = (const ZW_FIRMWARE_UPDATE_MD_REPORT_V1_FRAME*)cc_data;

        znet_cb->node_cmd_firmware_update_md_result(
            ZNET_ERR_OK,
            node_id,
            func->_endpoint,
            frame_v1->reportNumberLsb | ( uint16_t )( frame_v1->reportNumberMsb << 8 ),
            frame_v1->isLastReport != 0,
            ( uint8_t )( (size_t)cc_data_len - sizeof( ZW_FIRMWARE_UPDATE_MD_REPORT_V1_FRAME ) ),
            frame_v1->data,
            znet_cb->arg );
    }
}

void znet_cc_firmware_update_md_status_received( const ZFunction func,
                                                 uint8_t node_id,
                                                 int cc_data_len,
                                                 const uint8_t* cc_data ) {
    assert( cc_data );
    assert( sizeof( ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_V1_FRAME ) <= (size_t)cc_data_len );

    if( znet_cb->node_cmd_firmware_update_md_status_result ) {
        const ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_V1_FRAME* frame_v1 =
            (const ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_V1_FRAME*)cc_data;

        uint16_t waiting_time = 0;
        if( (size_t)cc_data_len >= sizeof( ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_V1_FRAME ) +
                                       sizeof( ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_V3_SUBFRAME ) ) {
            const ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_V3_SUBFRAME* subframe =
                (const ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_V3_SUBFRAME*)( cc_data +
                                                                          sizeof(
                                                                              ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_V1_FRAME ) );
            waiting_time = subframe->waittimeLsb | ( uint16_t )( subframe->waittimeMsb << 8 );
        }

        znet_cb->node_cmd_firmware_update_md_status_result(
            ZNET_ERR_OK, node_id, func->_endpoint, frame_v1->status, waiting_time, znet_cb->arg );
    }
}

void znet_cc_firmware_update_md_activation_status_received( const ZFunction func,
                                                            uint8_t node_id,
                                                            int cc_data_len,
                                                            const uint8_t* cc_data ) {
    assert( cc_data );
    assert( sizeof( ZW_FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_V4_FRAME ) <= (size_t)cc_data_len );

    if( znet_cb->node_cmd_firmware_update_md_activation_status_result ) {
        const ZW_FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_V4_FRAME* frame_v4 =
            (const ZW_FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_V4_FRAME*)cc_data;

        znet_firmware_update_md_activation_status_report_v4_t report_v4 = {};
        znet_firmware_update_md_activation_status_report_v5_t report_v5 = {};

        report_v4.firmware.checksum = frame_v4->checksumLsb | ( uint16_t )( frame_v4->checksumMsb << 8 );
        report_v4.firmware.firmware_id = frame_v4->firmwareIdLsb | ( uint16_t )( frame_v4->firmwareIdMsb << 8 );
        report_v4.firmware.manufacturer_id =
            frame_v4->manufacturerIdLsb | ( uint16_t )( frame_v4->manufacturerIdMsb << 8 );
        report_v4.status = frame_v4->firmwareUpdateStatus;

        const bool has_v5 = (size_t)cc_data_len >= sizeof( ZW_FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_V5_SUBFRAME ) +
                                                       sizeof( ZW_FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_V4_FRAME );

        if( has_v5 ) {
            const ZW_FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_V5_SUBFRAME* subframe_v5 =
                (const ZW_FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_V5_SUBFRAME*)( cc_data +
                                                                                  sizeof(
                                                                                      ZW_FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_V4_FRAME ) );

            report_v5.hardware_revision = subframe_v5->hardwareVersion;
        }

        znet_cb->node_cmd_firmware_update_md_activation_status_result( ZNET_ERR_OK,
                                                                       node_id,
                                                                       func->_endpoint,
                                                                       frame_v4->firmwareTarget,
                                                                       &report_v4,
                                                                       has_v5 ? &report_v5 : NULL,
                                                                       znet_cb->arg );
    }
}

void znet_cc_firmware_update_md_prepare_report_received( const ZFunction func,
                                                         uint8_t node_id,
                                                         int cc_data_len,
                                                         const uint8_t* cc_data ) {
    assert( cc_data );
    assert( sizeof( ZW_FIRMWARE_UPDATE_MD_PREPARE_REPORT_V5_FRAME ) <= (size_t)cc_data_len );

    if( znet_cb->node_cmd_firmware_update_md_preparation_result ) {
        const ZW_FIRMWARE_UPDATE_MD_PREPARE_REPORT_V5_FRAME* frame =
            (const ZW_FIRMWARE_UPDATE_MD_PREPARE_REPORT_V5_FRAME*)cc_data;
        znet_cb->node_cmd_firmware_update_md_preparation_result(
            ZNET_ERR_OK,
            node_id,
            func->_endpoint,
            frame->status,
            frame->firmwareChecksumLsb | ( uint16_t )( frame->firmwareChecksumMsb << 8 ),
            znet_cb->arg );
    }
}

void znet_cc_firmware_update_md_prepare_requested( const ZFunction func,
                                                   uint8_t node_id,
                                                   int cc_data_len,
                                                   const uint8_t* cc_data ) {
    znet_cc_firmware_update_md_prepare_report( node_id,
                                               func->_endpoint,
                                               FIRMWARE_UPDATE_MD_PREPARE_REPORT_NOT_DOWNLOADABLE_V5,
                                               CRC16_DEFAULT_INIT_VALUE,
                                               NULL,
                                               NULL,
                                               func->_security );
}
