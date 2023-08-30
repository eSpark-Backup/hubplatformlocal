
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

//----------------------------------------------------------------------------
const uint8_t version_command_class[][2] = { { COMMAND_CLASS_ZWAVEPLUS_INFO, ZWAVEPLUS_INFO_VERSION_V2 },
                                             { COMMAND_CLASS_CRC_16_ENCAP, CRC_16_ENCAP_VERSION },
                                             { COMMAND_CLASS_DEVICE_RESET_LOCALLY, DEVICE_RESET_LOCALLY_VERSION },
                                             { COMMAND_CLASS_INCLUSION_CONTROLLER, INCLUSION_CONTROLLER_VERSION },
                                             { COMMAND_CLASS_MANUFACTURER_SPECIFIC, MANUFACTURER_SPECIFIC_VERSION },
                                             { COMMAND_CLASS_POWERLEVEL, 0x01 },
                                             { COMMAND_CLASS_SECURITY, SECURITY_VERSION },
                                             { COMMAND_CLASS_SECURITY_2, SECURITY_2_VERSION },
                                             { COMMAND_CLASS_SUPERVISION, SUPERVISION_VERSION },
                                             { COMMAND_CLASS_TRANSPORT_SERVICE, TRANSPORT_SERVICE_VERSION_V2 },
                                             { COMMAND_CLASS_VERSION, VERSION_VERSION_V2 },
                                             { COMMAND_CLASS_ASSOCIATION, ASSOCIATION_VERSION_V3 },
                                             { COMMAND_CLASS_ASSOCIATION_GRP_INFO_V3, ASSOCIATION_GRP_INFO_VERSION_V3 },
                                             { COMMAND_CLASS_INDICATOR, 0x03 },
                                             { COMMAND_CLASS_NODE_PROVISIONING, 0x01 },
                                             { COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3,
                                               MULTI_CHANNEL_ASSOCIATION_VERSION_V4 },
                                             { COMMAND_CLASS_TIME, TIME_VERSION_V2 },
                                             { COMMAND_CLASS_MULTI_CMD, MULTI_CMD_VERSION },
                                             { COMMAND_CLASS_FIRMWARE_UPDATE_MD, 0x5 } };
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_version_get( uint8_t node_id,
                                                    uint8_t channel_id,
                                                    ZFunction_Callback Callback,
                                                    void* callbackArg,
                                                    encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_VERSION, VERSION_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_version_command_class_get( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  uint8_t cc_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_VERSION, VERSION_COMMAND_CLASS_GET, cc_id };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_version_capabilities_get( uint8_t node_id,
                                                                 uint8_t channel_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_VERSION, VERSION_CAPABILITIES_GET_V3 };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_version_software_get( uint8_t node_id,
                                                             uint8_t channel_id,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_VERSION, VERSION_ZWAVE_SOFTWARE_GET_V3 };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Version Request CB
//
void ICACHE_FLASH_ATTR znet_cc_command_class_version_request( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data ) {
    uint8_t version = 0;
    for( uint8_t i = 0; i < sizeof( version_command_class ) / 2; ++i ) {
        if( version_command_class[i][0] == cc_data[2] ) {
            version = version_command_class[i][1];
            break;
        }
    }
    ZW_VERSION_COMMAND_CLASS_REPORT_FRAME data;
    data.cmdClass = COMMAND_CLASS_VERSION;
    data.cmd = VERSION_COMMAND_CLASS_REPORT;
    data.requestedCommandClass = cc_data[2];
    data.commandClassVersion = version;
    znet_fc_send_data( node_id, func->_endpoint, sizeof data, (const uint8_t*)&data, NULL, NULL, func->_security );
}

void ICACHE_FLASH_ATTR znet_cc_version_request( const ZFunction func,
                                                uint8_t node_id,
                                                int UNUSED( cc_data_len ),
                                                const uint8_t* cc_data ) {
    const uint8_t buffer_size =
        sizeof( ZW_VERSION_REPORT_V2_FRAME ) + sizeof( VG_VERSION_REPORT_VG ) * ( FW_MAX_COUNT + 1 );
    uint8_t data[buffer_size];
    memset( data, 0, buffer_size );
    ZW_VERSION_REPORT_V2_FRAME* frame = (ZW_VERSION_REPORT_V2_FRAME*)data;
    frame->cmdClass = COMMAND_CLASS_VERSION;
    frame->cmd = VERSION_REPORT;
    frame->zWaveLibraryType = Z_WAVE_LIBRARY_TYPE;
    frame->zWaveProtocolVersion.version = Z_WAVE_PROTOCOL_VERSION;
    frame->zWaveProtocolVersion.subVersion = Z_WAVE_PROTOCOL_SUB_VERSION;
    frame->firmware0Version.version = Z_WAVE_PROTOCOL_VERSION;
    frame->firmware0Version.subVersion = Z_WAVE_PROTOCOL_SUB_VERSION;

    znet_cc_version_fill( func, frame );  //  modify by firmware

    const uint8_t length =
        sizeof( ZW_VERSION_REPORT_V2_FRAME ) + sizeof( VG_VERSION_REPORT_VG ) * frame->numberOfFirmwareTargets;

    znet_fc_send_data( node_id, func->_endpoint, length, data, NULL, NULL, func->_security );
}
