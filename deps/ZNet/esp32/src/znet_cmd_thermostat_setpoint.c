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
#include <znet_lib_cc_application.h>

union setpoint_info {
    void* data;
    uint8_t type;
};

// node cmd Thermostat Setpoint report //////////////////////////////////////////
/// INFO: Thermostat Setpoint Command Class v1..v3
void znet_cc_thermostat_setpoint_report( const ZFunction func,
                                         uint8_t node_id,
                                         int cc_data_len,
                                         const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT );
    assert( cc_data[1] == THERMOSTAT_SETPOINT_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( ( cc_data[2] & THERMOSTAT_SETPOINT_REPORT_LEVEL_RESERVED_MASK_V3 ) != 0 )
        return;

    uint8_t size = cc_data[3] & 0x07;
    if( size > 4 || size == 0 || size == 3 ) {
        return;
    }

    znet_thermostat_setpoint_report_t report = { .type = cc_data[2] & 0x0F,
                                                 .scale = ( cc_data[3] & 0x18 ) >> 3,
                                                 .size = size,
                                                 .precision = ( cc_data[3] & 0xE0 ) >> 5 };

    switch( report.size ) {
    case 1: {
        report.sp_value = ( uint32_t )( cc_data[4] );
        break;
    }
    case 2: {
        uint32_t res = ( (uint32_t)cc_data[4] << 8 | (uint32_t)cc_data[5] );
        report.sp_value = ( res < (uint32_t)INT32_MIN ) ? (int32_t)res : INT32_MIN + ( int32_t )( res & INT32_MAX );
        break;
    }
    case 4: {
        uint32_t res = ( (uint32_t)cc_data[4] << 24 | (uint32_t)cc_data[5] << 16 | (uint32_t)cc_data[6] << 8 |
                         (uint32_t)cc_data[7] );

        report.sp_value = ( res < (uint32_t)INT32_MIN ) ? (int32_t)res : INT32_MIN + ( int32_t )( res & INT32_MAX );
        break;
    }
    }

    if( znet_cb->node_cmd_thermostat_setpoint_result )
        znet_cb->node_cmd_thermostat_setpoint_result(
            ZNET_ERR_OK, node_id, func->_endpoint, report.type, &report, znet_cb->arg );
}

// node cmd Thermostat Setpoint get /////////////////////////////////////////////
/// INFO: Thermostat Setpoint Command Class v1..v3
static void _znet_node_cmd_thermostat_setpoint_get_cb( ZFunction func,
                                                       void* arg,
                                                       ZFuncFailures_e reason,
                                                       const void* dataPtr,
                                                       size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    union setpoint_info setpoint;
    setpoint.data = arg;

    if( znet_cb->node_cmd_thermostat_setpoint_result )
        znet_cb->node_cmd_thermostat_setpoint_result(
            znet_get_error( reason ), nodeId, endpoint, setpoint.type, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_thermostat_setpoint_get( znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    znet_cmd_thermostat_setpoint_value_t type ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union setpoint_info info;
    info.type = type;

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_thermostat_setpoint_get(
        node_id, channel_id, type, _znet_node_cmd_thermostat_setpoint_get_cb, info.data, encap );
}

// node cmd Thermostat Setpoint set /////////////////////////////////////////////
/// INFO: Thermostat Setpoint Command Class v1..v3
znet_error_t znet_node_cmd_thermostat_setpoint_set( znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    znet_cmd_thermostat_setpoint_value_t type,
                                                    znet_cmd_thermostat_setpoint_value_t size,
                                                    znet_cmd_thermostat_setpoint_value_t scale,
                                                    znet_cmd_thermostat_setpoint_value_t precision,
                                                    int32_t value ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( size > 4 || size == 0 || size == 3 ) {
        return ZNET_ERR_INVAL;
    }

    uint8_t buff[size];
    memset( buff, 0, size );

    for( int i = 0; i < size; ++i ) {
        buff[size - 1 - i] = ( uint8_t )( ( value >> ( 8 * i ) ) & 0xFF );
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_thermostat_setpoint_set(
        node_id, channel_id, type, size, scale, precision, buff, NULL, NULL, encap );
}

// node cmd Thermostat Setpoint report //////////////////////////////////////////
/// INFO: Thermostat Setpoint Command Class v1..v3
void znet_cc_thermostat_setpoint_supported_report( const ZFunction func,
                                                   uint8_t node_id,
                                                   int cc_data_len,
                                                   const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT );
    assert( cc_data[1] == THERMOSTAT_SETPOINT_SUPPORTED_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    znet_thermostat_setpoint_supported_report_t report = {};
    report.stcount = cc_data_len - 2;
    report.bit_mask = cc_data + 2;

    if( znet_cb->node_cmd_thermostat_setpoint_supported_result )
        znet_cb->node_cmd_thermostat_setpoint_supported_result(
            ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

// node cmd Thermostat Setpoint Supported get //////////////////////////////////////////////////
/// INFO: Thermostat Setpoint Command Class v1..v3

static void _znet_node_cmd_thermostat_setpoint_supported_get_cb( ZFunction func,
                                                                 void* arg,
                                                                 ZFuncFailures_e reason,
                                                                 const void* dataPtr,
                                                                 size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_thermostat_setpoint_supported_result )
        znet_cb->node_cmd_thermostat_setpoint_supported_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_thermostat_setpoint_supported_get( znet_node_id_t node_id,
                                                              znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_thermostat_setpoint_supported_get(
        node_id, channel_id, _znet_node_cmd_thermostat_setpoint_supported_get_cb, NULL, encap );
}

// node cmd Thermostat Setpoint Capability report ///////////////////////////////
/// INFO: Thermostat Setpoint Command Class v3
void znet_cc_thermostat_setpoint_capabilities_report( const ZFunction func,
                                                      uint8_t node_id,
                                                      int cc_data_len,
                                                      const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT );
    assert( cc_data[1] == THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_V3 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return;
    }

    znet_thermostat_setpoint_capabilities_report_t report = {};
    report.type = cc_data[2] & 0xF;
    report.min_params = cc_data[3];
    uint8_t size = cc_data[3] & THERMOSTAT_SETPOINT_REPORT_LEVEL2_SIZE_MASK_V3;
    uint32_t value;
    int i = 0;
    value = (uint32_t)cc_data[4];

    for( uint8_t index = 1; index < size; ++index ) {
        value = ( value << 8 ) | ( (uint32_t)cc_data[++i + 4] );
    }

    report.min_value = value;
    report.max_params = cc_data[++i + 4];
    value = (uint32_t)cc_data[++i + 4];

    size = report.max_params & THERMOSTAT_SETPOINT_REPORT_LEVEL2_SIZE_MASK_V3;
    for( uint8_t index = 1; index < size; ++index ) {
        value = ( value << 8 ) | ( (uint32_t)cc_data[++i + 4] );
    }
    report.max_value = value;

    if( znet_cb->node_cmd_thermostat_setpoint_capabilities_result )
        znet_cb->node_cmd_thermostat_setpoint_capabilities_result(
            0, node_id, func->_endpoint, report.type, &report, znet_cb->arg );
}

// node cmd Thermostat Setpoint Capabilities get ////////////////////////////////
/// INFO: Thermostat Setpoint Command Class v3

static void _znet_node_cmd_thermostat_setpoint_capabilities_get_cb( ZFunction func,
                                                                    void* arg,
                                                                    ZFuncFailures_e reason,
                                                                    const void* dataPtr,
                                                                    size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    union setpoint_info setpoint;
    setpoint.data = arg;

    if( znet_cb->node_cmd_thermostat_setpoint_capabilities_result )
        znet_cb->node_cmd_thermostat_setpoint_capabilities_result(
            znet_get_error( reason ), nodeId, endpoint, setpoint.type, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_thermostat_setpoint_capabilities_get( znet_node_id_t node_id,
                                                                 znet_node_channel_id_t channel_id,
                                                                 znet_cmd_thermostat_setpoint_value_t type ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union setpoint_info setpoint;
    setpoint.data = NULL;
    setpoint.type = type;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_thermostat_setpoint_capabilities_get(
        node_id, channel_id, type, _znet_node_cmd_thermostat_setpoint_capabilities_get_cb, setpoint.data, encap );
}
