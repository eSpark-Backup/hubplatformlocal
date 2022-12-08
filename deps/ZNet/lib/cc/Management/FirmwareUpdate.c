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
#include "znet_main.h"
#include "byte_ops.h"

//----------------------------------------------------------------------------

znet_error_t znet_cc_firmware_update_info_report( uint8_t node_id,
                                                  uint8_t channel_id,
                                                  // v1
                                                  uint16_t manufacturer_id,
                                                  uint16_t firmware_id,
                                                  uint16_t checksum,
                                                  // v3
                                                  bool firmware_upgradable,
                                                  uint16_t max_fragment_size,
                                                  uint8_t targets_count,
                                                  const uint16_t* targets,
                                                  // v5
                                                  uint8_t hardware_revision,
                                                  // callback
                                                  ZFunction_Callback callback,
                                                  void* callback_arg,
                                                  encap_type_t encap_type ) {
    const uint8_t offset_v5 =
        sizeof( ZW_FIRMWARE_MD_REPORT_FRAME ) + sizeof( FIRMWARE_MD_REPORT_SUBFRAME ) * targets_count;
    const uint8_t buffer_size = offset_v5 + 1;
    uint8_t buffer[buffer_size];
    ZW_FIRMWARE_MD_REPORT_FRAME* frame = (ZW_FIRMWARE_MD_REPORT_FRAME*)buffer;
    frame->cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD;
    frame->cmd = FIRMWARE_MD_REPORT;
    frame->manufacturerIdLsb = manufacturer_id & 0xFF;
    frame->manufacturerIdMsb = manufacturer_id >> 8;
    frame->firmware0IdLsb = firmware_id & 0xFF;
    frame->firmware0IdMsb = firmware_id >> 8;
    frame->firmware0ChecksumLsb = checksum & 0xFF;
    frame->firmware0ChecksumMsb = checksum >> 8;
    frame->firmwareUpgradable = firmware_upgradable ? 0xFF : 0;
    frame->maxFragmentSizeLsb = max_fragment_size & 0xFF;
    frame->maxFragmentSizeMsb = max_fragment_size >> 8;
    frame->numberOfFirmwareTargets = targets_count;
    for( uint8_t i = 0; i < targets_count; ++i ) {
        frame->firmware[i].firmwareIdLsb = targets[i] & 0xFF;
        frame->firmware[i].firmwareIdMsb = targets[i] >> 8;
    }

    buffer[offset_v5] = hardware_revision;

    return znet_fc_send_data( node_id, channel_id, buffer_size, buffer, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_firmware_md_get( uint8_t node_id,
                                      uint8_t channel_id,
                                      ZFunction_Callback callback,
                                      void* callback_arg,
                                      encap_type_t encap_type ) {
    ZW_FIRMWARE_MD_GET_FRAME frame = {};

    frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD;
    frame.cmd = FIRMWARE_MD_GET;

    return znet_fc_send_data(
        node_id, channel_id, sizeof frame, (const uint8_t*)&frame, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_firmware_update_md_request_get( uint8_t node_id,
                                                     uint8_t channel_id,
                                                     uint16_t manufacturer_id,
                                                     uint16_t firmware_id,
                                                     uint16_t firmware_checksum,
                                                     uint8_t firmware_target,
                                                     uint16_t fragment_size,
                                                     bool is_postponed_activation_allowed,
                                                     uint8_t hardware_revision,
                                                     uint8_t cc_version,
                                                     ZFunction_Callback callback,
                                                     void* callback_arg,
                                                     encap_type_t encap_type ) {
    ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_V5_FRAME frame = {};

    frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD;
    frame.cmd = FIRMWARE_UPDATE_MD_REQUEST_GET;
    frame.manufacturerIdLsb = manufacturer_id & 0xFF;
    frame.manufacturerIdMsb = manufacturer_id >> 8;
    frame.firmwareIdLsb = firmware_id & 0xFF;
    frame.firmwareIdMsb = firmware_id >> 8;
    frame.checksumLsb = firmware_checksum & 0xFF;
    frame.checksumMsb = firmware_checksum >> 8;
    frame.firmwareTarget = firmware_target;
    frame.fragmentSizeLsb = fragment_size & 0xFF;
    frame.fragmentSizeMsb = fragment_size >> 8;
    frame.postponedActivationAllowed = is_postponed_activation_allowed;
    frame.hardwareVersion = hardware_revision;

    const uint8_t dataSize = cc_version >= 5 ? sizeof( ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_V5_FRAME ) :
                                               cc_version >= 4 ?
                                               sizeof( ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_V4_FRAME ) :
                                               cc_version >= 3 ? sizeof( ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_V3_FRAME ) :
                                                                 sizeof( ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_V1_FRAME );
    return znet_fc_send_data(
        node_id, channel_id, dataSize, (const uint8_t*)&frame, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_firmware_update_md_request_report( uint8_t node_id,
                                                        uint8_t channel_id,
                                                        uint8_t status,
                                                        ZFunction_Callback callback,
                                                        void* callback_arg,
                                                        encap_type_t encap_type ) {
    ZW_FIRMWARE_UPDATE_MD_REQUEST_REPORT_FRAME frame = {};
    frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD;
    frame.cmd = FIRMWARE_UPDATE_MD_REQUEST_REPORT;
    frame.status = status;

    return znet_fc_send_data(
        node_id, channel_id, sizeof frame, (const uint8_t*)&frame, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_firmware_update_md_prepare_report( uint8_t node_id,
                                                        uint8_t channel_id,
                                                        uint8_t status,
                                                        uint16_t checksum,
                                                        ZFunction_Callback callback,
                                                        void* callback_arg,
                                                        encap_type_t encap_type ) {
    ZW_FIRMWARE_UPDATE_MD_PREPARE_REPORT_V5_FRAME frame = {};
    frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD;
    frame.cmd = FIRMWARE_UPDATE_MD_PREPARE_REPORT;
    frame.status = status;
    frame.firmwareChecksumLsb = checksum & 0xFF;
    frame.firmwareChecksumMsb = checksum >> 8;

    return znet_fc_send_data(
        node_id, channel_id, sizeof frame, (const uint8_t*)&frame, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_firmware_update_md_get( uint8_t node_id,
                                             uint8_t channel_id,
                                             uint8_t reports_count,
                                             uint16_t report_number,
                                             ZFunction_Callback callback,
                                             void* callback_arg,
                                             encap_type_t encap_type ) {
    ZW_FIRMWARE_UPDATE_MD_GET_FRAME frame = {};

    frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD;
    frame.cmd = FIRMWARE_UPDATE_MD_GET;
    frame.numberOfReports = reports_count;
    frame.reportNumberLsb = report_number & 0xFF;
    frame.reportNumberMsb = report_number >> 8;

    return znet_fc_send_data(
        node_id, channel_id, sizeof frame, (const uint8_t*)&frame, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_firmware_update_md_report( uint8_t node_id,
                                                uint8_t channel_id,
                                                uint16_t report_number,
                                                bool is_last_report,
                                                uint8_t data_size,
                                                const uint8_t* data,
                                                bool use_checksum,
                                                ZFunction_Callback callback,
                                                void* callback_arg,
                                                encap_type_t encap_type ) {
    const uint8_t frame_size_v1 = sizeof( ZW_FIRMWARE_UPDATE_MD_REPORT_V1_FRAME ) + data_size;
    const uint8_t buffer_size = frame_size_v1 + sizeof( ZW_FIRMWARE_UPDATE_MD_REPORT_V2_SUBFRAME );

    uint8_t buffer[buffer_size];

    ZW_FIRMWARE_UPDATE_MD_REPORT_V1_FRAME frame = {};

    frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD;
    frame.cmd = FIRMWARE_UPDATE_MD_REPORT;
    frame.isLastReport = is_last_report;
    frame.reportNumberLsb = report_number & 0xFF;
    frame.reportNumberMsb = report_number >> 8;

    memcpy( buffer, &frame, sizeof( frame ) );
    memcpy( buffer + sizeof( frame ), data, data_size );

    if( use_checksum ) {
        ZW_FIRMWARE_UPDATE_MD_REPORT_V2_SUBFRAME subframe_v2 = {};
        const uint16_t checksum = crc16( buffer, sizeof( frame ) + data_size );

        subframe_v2.checksumLsb = checksum & 0xFF;
        subframe_v2.checksumMsb = checksum >> 8;
        memcpy( buffer + frame_size_v1, &subframe_v2, sizeof( subframe_v2 ) );
    }

    const uint8_t frame_size = use_checksum ? buffer_size : frame_size_v1;

    return znet_fc_send_data( node_id, channel_id, frame_size, buffer, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_firmware_update_activation_set( uint8_t node_id,
                                                     uint8_t channel_id,
                                                     uint16_t manufacturer_id,
                                                     uint16_t firmware_id,
                                                     uint16_t firmware_checksum,
                                                     uint8_t firmware_target,
                                                     uint8_t hardware_version,
                                                     uint8_t cc_version,
                                                     ZFunction_Callback callback,
                                                     void* callback_arg,
                                                     encap_type_t encap_type ) {
    ZW_FIRMWARE_UPDATE_ACTIVATION_SET_V5_FRAME frame = {};

    frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD;
    frame.cmd = FIRMWARE_UPDATE_ACTIVATION_SET;
    frame.manufacturerIdLsb = manufacturer_id & 0xFF;
    frame.manufacturerIdMsb = manufacturer_id >> 8;
    frame.firmwareIdLsb = firmware_id & 0xFF;
    frame.firmwareIdMsb = firmware_id >> 8;
    frame.checksumLsb = firmware_checksum & 0xFF;
    frame.checksumMsb = firmware_checksum >> 8;
    frame.firmwareTarget = firmware_target;
    frame.hardwareVersion = hardware_version;

    const uint8_t frame_size = cc_version >= 5 ? sizeof( ZW_FIRMWARE_UPDATE_ACTIVATION_SET_V5_FRAME ) :
                                                 sizeof( ZW_FIRMWARE_UPDATE_ACTIVATION_SET_V4_FRAME );

    return znet_fc_send_data(
        node_id, channel_id, frame_size, (const uint8_t*)&frame, callback, callback_arg, encap_type );
}

znet_error_t znet_cc_firmware_update_md_prepare_get( uint8_t node_id,
                                                     uint8_t channel_id,
                                                     uint16_t manufacturer_id,
                                                     uint16_t firmware_id,
                                                     uint8_t firmware_target,
                                                     uint16_t fragment_size,
                                                     uint8_t hardware_version,
                                                     ZFunction_Callback callback,
                                                     void* callback_arg,
                                                     encap_type_t encap_type ) {
    ZW_FIRMWARE_UPDATE_MD_PREPARE_GET_V5_FRAME frame = {};

    frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD;
    frame.cmd = FIRMWARE_UPDATE_MD_PREPARE_GET;
    frame.manufacturerIdLsb = manufacturer_id & 0xFF;
    frame.manufacturerIdMsb = manufacturer_id >> 8;
    frame.firmwareIdLsb = firmware_id & 0xFF;
    frame.firmwareIdMsb = firmware_id >> 8;
    frame.firmwareTarget = firmware_target;
    frame.fragmentSizeLsb = fragment_size & 0xFF;
    frame.fragmentSizeMsb = fragment_size >> 8;
    frame.hardwareVersion = hardware_version;

    return znet_fc_send_data(
        node_id, channel_id, sizeof( frame ), (const uint8_t*)&frame, callback, callback_arg, encap_type );
}
