/**
 * @file znet_cmd_meter.c
 * @date 5 Apr 2018
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
#include <znet_lib_cc_application.h>

static int32_t read_meter_value( const uint8_t* data, uint8_t value_size ) {
    int32_t result = 0;
    for( uint8_t i = 0; i < value_size; ++i ) {
        result <<= 8;
        result |= data[i];
    }

    return result;
}

// node cmd meter report ///////////////////////////////////////////////
/// INFO: Meter Command Class v1..v5

void znet_cc_meter_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );

    if( cc_data_len < 4 ) {
        ZNET_LOGW( "Wrong meter report data size %i\n", cc_data_len );
        return;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_meter_result ) {
        const uint8_t vsize = cc_data[3] & 0x07;  // value size

        znet_meter_report_t report = {};
        report.type = cc_data[2] & 0x1F;
        report.scale = ( ( cc_data[2] & 0x80 ) >> 5 ) | ( ( cc_data[3] & 0x18 ) >> 3 );
        report.precision = ( cc_data[3] & 0xE0 ) >> 5;
        report.rate_type = ( cc_data[2] & 0x60 ) >> 5;

        uint8_t offset = 4;

        if( offset + vsize <= cc_data_len ) {
            report.value = read_meter_value( cc_data + offset, vsize );
            offset += vsize;

            if( offset + 2 <= cc_data_len ) {
                report.dtime = ( uint16_t )( cc_data[offset + 1] | ( cc_data[offset] << 8 ) );
                offset += 2;

                if( report.dtime ) {
                    if( offset + vsize <= cc_data_len ) {
                        report.prev_value = read_meter_value( cc_data + vsize + 6, vsize );
                        offset += vsize;
                    }
                    else {
                        offset = (uint8_t)cc_data_len;
                        ZNET_LOGW( "Wrong meter report data size %i, cannot find prev_value\n", cc_data_len );
                    }
                }

                if( report.scale == 0x07 ) {  // scale2
                    if( offset + 1 <= cc_data_len ) {
                        report.scale = cc_data[offset] + 0x08;
                        ++offset;
                    }
                    else {
                        offset = (uint8_t)cc_data_len;
                        ZNET_LOGW( "Wrong meter report data size %i, cannot find scale 2\n", cc_data_len );
                    }
                }
            }
            else {
                offset = (uint8_t)cc_data_len;
            }
        }
        else {
            offset = (uint8_t)cc_data_len;
            ZNET_LOGW( "Wrong meter report data size %i, cannot find value\n", cc_data_len );
        }

        znet_cb->node_cmd_meter_result( ZNET_ERR_OK, node_id, func->_endpoint, report.scale, &report, znet_cb->arg );
    }
}

// node cmd meter get //////////////////////////////////////////////////
/// INFO: Meter Command Class v1..v6

union meter_param {
    void* data;
    uint8_t scale;
};

static void _znet_node_cmd_meter_get_cb( ZFunction func,
                                         void* arg,
                                         ZFuncFailures_e reason,
                                         const void* dataPtr,
                                         size_t dataSize ) {
    if( znet_cb->node_cmd_meter_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union meter_param param;
        param.data = arg;

        znet_cb->node_cmd_meter_result( znet_get_error( reason ), nodeId, endpoint, param.scale, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_meter_get( znet_node_id_t node_id,
                                      znet_node_channel_id_t channel_id,
                                      uint8_t scale,
                                      znet_command_class_version_t version ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( scale == 0x07 ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    union meter_param param;
    param.data = NULL;
    param.scale = scale;

    const uint8_t scale1 = scale > 0x07 ? 0x07 : scale;
    const uint8_t scale2 = scale > 0x07 ? scale - 0x08 : 0;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_meter_get(
        node_id, channel_id, scale1, scale2, 0, _znet_node_cmd_meter_get_cb, param.data, encap, version );
}

znet_error_t znet_node_cmd_meter_unknown_scale_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    return znet_node_cmd_meter_get( node_id, channel_id, 0, 1 );
}

// node cmd meter supported report /////////////////////////////////////
/// INFO: Meter Command Class v2..v5

void znet_cc_meter_supported_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_METER );
    assert( cc_data[1] == METER_SUPPORTED_REPORT_V5 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    znet_meter_supported_report_t report = {};
    report.type = cc_data[2] & 0x1F;
    report.reset = ( cc_data[2] & 0x80 ) == 0x80;
    report.rate_import = ( cc_data[2] & 0x20 ) == 0x20;
    report.rate_export = ( cc_data[2] & 0x40 ) == 0x40;
    report.scales_bitmask_size = 1;

    if( ( cc_data[3] & 0x80 ) == 0x80 ) {
        assert( cc_data_len >= 5 );
        assert( cc_data_len >= 5 + cc_data[4] );
        report.scales_bitmask_size += cc_data[4];
    }

    uint8_t scales_bitmask[report.scales_bitmask_size];
    scales_bitmask[0] = cc_data[3] & 0x7F;

    if( report.scales_bitmask_size > 1 ) {
        memcpy( scales_bitmask + 1, cc_data + 5, report.scales_bitmask_size - 1 );
    }

    report.scales_bitmask = scales_bitmask;

    if( znet_cb->node_cmd_meter_supported_result )
        znet_cb->node_cmd_meter_supported_result( ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

// node cmd meter supported get ////////////////////////////////////////
/// INFO: Meter Command Class v2..v5

static void _znet_node_cmd_meter_supported_get_cb( ZFunction func,
                                                   void* arg,
                                                   ZFuncFailures_e reason,
                                                   const void* dataPtr,
                                                   size_t dataSize ) {
    if( znet_cb->node_cmd_meter_supported_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_meter_supported_result( znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_meter_supported_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_meter_supported_get( node_id, channel_id, _znet_node_cmd_meter_supported_get_cb, NULL, encap );
}

// node cmd meter reset ////////////////////////////////////////////////
/// INFO: Meter Command Class v2..v6

static void _znet_node_cmd_meter_set_cb( ZFunction func,
                                         void* arg,
                                         ZFuncFailures_e reason,
                                         const void* dataPtr,
                                         size_t dataSize ) {
    if( znet_cb->node_cmd_meter_set_done ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_meter_set_done( znet_get_error( reason ), nodeId, endpoint, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_meter_reset( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_meter_reset( node_id, channel_id, _znet_node_cmd_meter_set_cb, NULL, encap );
}

znet_error_t znet_node_cmd_meter_reset_v6( znet_node_id_t node_id,
                                           znet_node_channel_id_t channel_id,
                                           uint8_t meter_type,
                                           uint8_t data_size,
                                           const uint8_t* data ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( data_size != 1 && data_size != 2 && data_size != 4 ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_meter_reset_v6(
        node_id, channel_id, meter_type, data_size, data, _znet_node_cmd_meter_set_cb, NULL, encap );
}

////////////////////////////////////////////////////////////////////////
