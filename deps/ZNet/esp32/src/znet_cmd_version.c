/**
 * @file znet_cmd_version.c
 * @date 10 Apr 2018
 * @brief TBD.
 */

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

// node cmd version report /////////////////////////////////////////////
/// INFO: Version Command Class v1..v3

static void znet_cc_version_report_v1( uint8_t node_id, const ZW_VERSION_REPORT_FRAME* data ) {
    assert( data->cmdClass == ZNET_COMMAND_CLASS_VERSION );
    assert( data->cmd == VERSION_REPORT );

    const size_t buff_size = sizeof( znet_version_report_t ) + 2;
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_version_report_t* report = (znet_version_report_t*)&buff;
    report->lib_type = data->zWaveLibraryType;
    report->proto.ver = data->zWaveProtocolVersion.version;
    report->proto.sub_ver = data->zWaveProtocolVersion.subVersion;
    report->firms_count = 1;
    report->firms[0].ver = data->applicationVersion.version;
    report->firms[0].sub_ver = data->applicationVersion.subVersion;

    if( znet_cb->node_cmd_version_result )
        znet_cb->node_cmd_version_result( ZNET_ERR_OK, node_id, report, znet_cb->arg );
}

static void znet_cc_version_report_v2( uint8_t node_id, const ZW_VERSION_REPORT_V2_FRAME* data ) {
    assert( data->cmdClass == ZNET_COMMAND_CLASS_VERSION );
    assert( data->cmd == VERSION_REPORT );

    const size_t buff_size = sizeof( znet_version_report_t ) + ( data->numberOfFirmwareTargets + 1 ) * 2;
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_version_report_t* report = (znet_version_report_t*)&buff;
    report->lib_type = data->zWaveLibraryType;
    report->proto.ver = data->zWaveProtocolVersion.version;
    report->proto.sub_ver = data->zWaveProtocolVersion.subVersion;
    report->hard_ver = data->hardwareVersion;
    report->firms_count = data->numberOfFirmwareTargets + 1;
    report->firms[0].ver = data->firmware0Version.version;
    report->firms[0].sub_ver = data->firmware0Version.subVersion;
    for( size_t i = 0; i < data->numberOfFirmwareTargets; ++i ) {
        report->firms[i + 1].ver = data->firmwareVersions[i].version;
        report->firms[i + 1].sub_ver = data->firmwareVersions[i].subVersion;
    }

    if( znet_cb->node_cmd_version_result )
        znet_cb->node_cmd_version_result( ZNET_ERR_OK, node_id, report, znet_cb->arg );
}

void znet_cc_version_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( cc_data_len == sizeof( ZW_VERSION_REPORT_FRAME ) ) {
        znet_cc_version_report_v1( node_id, (const ZW_VERSION_REPORT_FRAME*)cc_data );
    }
    else {
        znet_cc_version_report_v2( node_id, (const ZW_VERSION_REPORT_V2_FRAME*)cc_data );
    }
}

// node cmd version get ////////////////////////////////////////////////
/// INFO: Version Command Class v1..v3

static void _znet_node_cmd_version_get_cb( ZFunction func,
                                           void* arg,
                                           ZFuncFailures_e reason,
                                           const void* dataPtr,
                                           size_t dataSize ) {
    uint8_t nodeId;
    ZNet_CheckCallback( func, reason, &nodeId, NULL );

    if( znet_cb->node_cmd_version_result )
        znet_cb->node_cmd_version_result( znet_get_error( reason ), nodeId, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_version_get( znet_node_id_t node_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    /// TODO: investigte, for which the arg is used in znet_cc_version_get

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_version_get( node_id, ZNET_CHANNEL_ID_ROOT, _znet_node_cmd_version_get_cb, NULL, encap );
}

// node cmd command version report /////////////////////////////////////
/// INFO: Version Command Class v1..v3

void znet_cc_version_command_class_report( const ZFunction func,
                                           uint8_t node_id,
                                           int cc_data_len,
                                           const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_VERSION );
    assert( cc_data[1] == VERSION_COMMAND_CLASS_REPORT_V2 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const uint8_t version = cc_data_len >= 4 ? cc_data[3] : 1;  // hack for specific devices that don't report version

    if( znet_cb->node_cmd_command_version_result )
        znet_cb->node_cmd_command_version_result(
            ZNET_ERR_OK, node_id, func->_endpoint, cc_data[2], version, znet_cb->arg );
}

// node cmd command version get ////////////////////////////////////////
/// INFO: Version Command Class v1..v3

union command_version_param {
    void* data;
    uint8_t class_id;
};

static void _znet_node_cmd_command_version_get_cb( ZFunction func,
                                                   void* arg,
                                                   ZFuncFailures_e reason,
                                                   const void* dataPtr,
                                                   size_t dataSize ) {
    uint8_t node_id, channel_id;
    ZNet_CheckCallback( func, reason, &node_id, &channel_id );

    union command_version_param param;
    param.data = arg;

    if( znet_cb->node_cmd_command_version_result )
        znet_cb->node_cmd_command_version_result(
            znet_get_error( reason ), node_id, channel_id, param.class_id, 0, znet_cb->arg );
}

znet_error_t znet_node_cmd_command_version_get( znet_node_id_t node_id,
                                                znet_node_channel_id_t channel_id,
                                                znet_command_class_t command ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    /// TODO: investigte, for which the arg is used in
    /// znet_cc_version_command_class_get

    union command_version_param param;
    param.data = NULL;
    param.class_id = command;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_version_command_class_get(
        node_id, channel_id, command, _znet_node_cmd_command_version_get_cb, param.data, encap );
}

// node cmd capabilities version get ////////////////////////////////////////
/// INFO: Version Command Class v3

static void _znet_node_cmd_capabilities_get_cb( ZFunction func,
                                                void* arg,
                                                ZFuncFailures_e reason,
                                                const void* dataPtr,
                                                size_t dataSize ) {
    if( znet_cb->node_cmd_version_capabilities_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_version_capabilities_result( znet_get_error( reason ), nodeId, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_capabilities_version_get( znet_node_id_t node_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_version_capabilities_get(
        node_id, ZNET_CHANNEL_ID_ROOT, _znet_node_cmd_capabilities_get_cb, NULL, encap );
}

// node cmd capabilities report /////////////////////////////////////
/// INFO: Version Command Class v3

void znet_cc_version_capabilities_report( const ZFunction func,
                                          uint8_t node_id,
                                          int cc_data_len,
                                          const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len == sizeof( ZW_VERSION_CAPABILITIES_REPORT_V3_FRAME ) );

    const ZW_VERSION_CAPABILITIES_REPORT_V3_FRAME* data = (const ZW_VERSION_CAPABILITIES_REPORT_V3_FRAME*)cc_data;
    assert( data->cmdClass == ZNET_COMMAND_CLASS_VERSION );
    assert( data->cmd == VERSION_CAPABILITIES_REPORT_V3 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_version_capabilities_result ) {
        znet_version_capabilities_report_t report = { .version_supported = data->version_supported,
                                                      .command_class_supported = data->command_class_supported,
                                                      .software_supported = data->software_supported };

        znet_cb->node_cmd_version_capabilities_result( ZNET_ERR_OK, node_id, &report, znet_cb->arg );
    }
}

// node cmd software version get ////////////////////////////////////////
/// INFO: Version Command Class v3

static void _znet_node_cmd_software_get_cb( ZFunction func,
                                            void* arg,
                                            ZFuncFailures_e reason,
                                            const void* dataPtr,
                                            size_t dataSize ) {
    if( znet_cb->node_cmd_version_capabilities_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_version_capabilities_result( znet_get_error( reason ), nodeId, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_software_version_get( znet_node_id_t node_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_version_software_get( node_id, ZNET_CHANNEL_ID_ROOT, _znet_node_cmd_software_get_cb, NULL, encap );
}

// node cmd software report /////////////////////////////////////
/// INFO: Version Command Class v3

static uint16_t combineBytes( uint8_t msb, uint8_t lsb ) {
    return ( (uint16_t)msb << 8 ) | lsb;
}

void znet_cc_version_software_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len == sizeof( ZW_VERSION_ZWAVE_SOFTWARE_REPORT_V3_FRAME ) );

    const ZW_VERSION_ZWAVE_SOFTWARE_REPORT_V3_FRAME* data = (const ZW_VERSION_ZWAVE_SOFTWARE_REPORT_V3_FRAME*)cc_data;
    assert( data->cmdClass == ZNET_COMMAND_CLASS_VERSION );
    assert( data->cmd == VERSION_ZWAVE_SOFTWARE_REPORT_V3 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_software_version_result ) {
        znet_software_version_report_t report = {
            .sdk_version = { data->sdkVersion1, data->sdkVersion2, data->sdkVersion3 },
            .application_framework_api_version = { data->applicationFrameworkApiVersion1,
                                                   data->applicationFrameworkApiVersion2,
                                                   data->applicationFrameworkApiVersion3 },
            .host_interface_version = { data->hostInterfaceVersion1,
                                        data->hostInterfaceVersion2,
                                        data->hostInterfaceVersion3 },
            .protocol_version = { data->zWaveProtocolVersion1,
                                  data->zWaveProtocolVersion2,
                                  data->zWaveProtocolVersion3 },
            .application_version = { data->applicationFrameworkApiVersion1,
                                     data->applicationFrameworkApiVersion2,
                                     data->applicationFrameworkApiVersion3 },
            .application_framework_api_build_number =
                combineBytes( data->applicationBuildNumber1, data->applicationBuildNumber2 ),
            .host_interface_build_number =
                combineBytes( data->hostInterfaceBuildNumber1, data->hostInterfaceBuildNumber2 ),
            .protocol_build_number = combineBytes( data->zWaveProtocolBuildNumber1, data->zWaveProtocolBuildNumber2 ),
            .application_build_number = combineBytes( data->applicationBuildNumber1, data->applicationBuildNumber2 )
        };

        znet_cb->node_cmd_software_version_result( ZNET_ERR_OK, node_id, &report, znet_cb->arg );
    }
}
void znet_cc_version_fill( const ZFunction func, ZW_VERSION_REPORT_V2_FRAME* frame ) {
    assert( frame );

    if( znet_cb && znet_cb->node_cmd_version_fill ) {
        const size_t buff_size =
            sizeof( znet_version_report_t ) + ( FW_MAX_COUNT + 1 ) * sizeof( znet_version_minor_major_t );
        uint8_t buff[buff_size];
        memset( buff, 0, buff_size );

        znet_version_report_t* report = (znet_version_report_t*)&buff;

        znet_cb->node_cmd_version_fill( report, znet_cb->arg );

        frame->hardwareVersion = report->hard_ver;  // HW_1

        if( report->firms_count > 1 ) {
            frame->numberOfFirmwareTargets =
                report->firms_count - 1;  // The Firmware 0 Version fields are not included.
            for( uint8_t i = 0; i < report->firms_count - 1; ++i ) {
                frame->firmwareVersions[i].version = report->firms[i + 1].ver;         // FW_1
                frame->firmwareVersions[i].subVersion = report->firms[i + 1].sub_ver;  // FW_2
            }
        }
    }
}
