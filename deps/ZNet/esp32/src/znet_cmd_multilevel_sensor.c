/*
 * znet_cmd_multilevel_sensor.c
 *
 *  Created on: Sep 17, 2018
 *      Author: del
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

static const uint8_t maxSensorType = 69;

/*!****************************************************************************
 */
void znet_cc_sensor_multilevel_supported_sensor_report( const ZFunction func,
                                                        uint8_t node_id,
                                                        int cc_data_len,
                                                        const uint8_t* cc_data ) {
    assert( cc_data_len >= 3 );

    if( znet_cb->node_cmd_multilevel_sensor_get_supported_sensor_result )
        znet_cb->node_cmd_multilevel_sensor_get_supported_sensor_result(
            ZNET_ERR_OK, node_id, func->_endpoint, cc_data + 2, cc_data_len - 2, znet_cb->arg );
}

static void _znet_node_cmd_multilevel_sensor_get_supported_sensor_cb( ZFunction func,
                                                                      void* arg,
                                                                      ZFuncFailures_e reason,
                                                                      const void* dataPtr,
                                                                      size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_multilevel_sensor_get_supported_sensor_result )
        znet_cb->node_cmd_multilevel_sensor_get_supported_sensor_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, 0, znet_cb->arg );
}

/**
 * @brief Node cmd Multilevel Sensor Get Supported Sensor
 * @param node_id       Destination Node Id
 * @param channel_id    Z-Wave channel
 * @return status operation
 */
znet_error_t znet_node_cmd_multilevel_sensor_get_supported_sensor( znet_node_id_t node_id,
                                                                   znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_sensor_multilevel_supported_sensor_get(
        node_id, channel_id, _znet_node_cmd_multilevel_sensor_get_supported_sensor_cb, NULL, encap );
}

/*!****************************************************************************
 */
void znet_cc_sensor_multilevel_supported_scale_report( const ZFunction func,
                                                       uint8_t node_id,
                                                       int cc_data_len,
                                                       const uint8_t* cc_data ) {
    assert( cc_data_len >= 3 );

    if( znet_cb->node_cmd_multilevel_sensor_get_supported_scale_result )
        znet_cb->node_cmd_multilevel_sensor_get_supported_scale_result(
            ZNET_ERR_OK, node_id, func->_endpoint, cc_data[2], cc_data[3], znet_cb->arg );
}

static void _znet_node_cmd_multilevel_sensor_get_supported_scale_cb( ZFunction func,
                                                                     void* arg,
                                                                     ZFuncFailures_e reason,
                                                                     const void* dataPtr,
                                                                     size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_multilevel_sensor_get_supported_sensor_result )
        znet_cb->node_cmd_multilevel_sensor_get_supported_sensor_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, 0, znet_cb->arg );
}

/**
 *
 * @brief Node cmd Multilevel Sensor Get Supported Scale
 * @param node_id       Destination Node Id
 * @param sensor_type   Sensor type number [1 - maxSensorType]
 * @return status operation
 */
znet_error_t znet_node_cmd_multilevel_sensor_get_supported_scale( znet_node_id_t node_id,
                                                                  znet_node_channel_id_t channel_id,
                                                                  uint8_t sensor_type ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( sensor_type < 1 || sensor_type > maxSensorType ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_sensor_multilevel_supported_scale_get(
        node_id, channel_id, sensor_type, _znet_node_cmd_multilevel_sensor_get_supported_scale_cb, NULL, encap );
}

/*!****************************************************************************
 */
void znet_cc_sensor_multilevel_report( const ZFunction func,
                                       uint8_t node_id,
                                       int cc_data_len,
                                       const uint8_t* cc_data ) {
    assert( cc_data_len >= 5 );

    if( znet_cb->node_cmd_multilevel_sensor_get_result ) {
        znet_multilevel_sensor_report_t value;
        value.type = cc_data[2];
        value.precision = ( cc_data[3] & 0xE0 ) >> 5;
        value.scale = ( cc_data[3] & 0x18 ) >> 3;
        value.size = cc_data[3] & 0x07;

        znet_error_t err = ZNET_ERR_OK;

        switch( value.size ) {
        case 1: {
            if( cc_data_len < 5 ) {
                err = ZNET_ERR_INVAL;
                break;
            }
            value.value = (int8_t)cc_data[4];
            break;
        }
        case 2: {
            assert( cc_data_len == 6 );
            if( cc_data_len < 6 ) {
                err = ZNET_ERR_INVAL;
                break;
            }
            value.value = ( int16_t )( cc_data[4] << 8 | cc_data[5] );
            break;
        }
        case 4: {
            assert( cc_data_len == 8 );
            if( cc_data_len < 8 ) {
                err = ZNET_ERR_INVAL;
                break;
            }
            value.value = ( int32_t )( cc_data[4] << 24 | cc_data[5] << 16 | cc_data[6] << 8 | cc_data[7] );
            break;
        }
        }

        znet_cb->node_cmd_multilevel_sensor_get_result(
            err, node_id, func->_endpoint, value.type, value.scale, &value, znet_cb->arg );
    }
}

union multilevel_sensor_get_param {
    void* data;
    struct {
        uint8_t scale;
        uint8_t type;
    };
};

static void _znet_node_cmd_multilevel_sensor_get_cb( ZFunction func,
                                                     void* arg,
                                                     ZFuncFailures_e reason,
                                                     const void* dataPtr,
                                                     size_t dataSize ) {
    if( znet_cb->node_cmd_multilevel_sensor_get_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union multilevel_sensor_get_param param;
        param.data = arg;

        znet_cb->node_cmd_multilevel_sensor_get_result(
            znet_get_error( reason ), nodeId, endpoint, param.type, param.scale, NULL, znet_cb->arg );
    }
}

/**
 * @brief Node cmd Multilevel Sensor Get
 * @param node_id       Destination Node Id
 * @param channel_id    Z-Wave channel
 * @param sensor_type   Sensor type number [1 - maxSensorType]
 * @param scale         Scale number [0-3]
 * @param version       version Multilevel Sensor Command Class [1-11]
 * @return status operation
 */
znet_error_t znet_node_cmd_multilevel_sensor_get( znet_node_id_t node_id,
                                                  znet_node_channel_id_t channel_id,
                                                  uint8_t sensor_type,
                                                  uint8_t scale,
                                                  uint8_t version ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( sensor_type > maxSensorType ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    union multilevel_sensor_get_param param;
    param.data = NULL;
    param.type = sensor_type;
    param.scale = scale;

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_sensor_multilevel_get(
        node_id, channel_id, sensor_type, scale, _znet_node_cmd_multilevel_sensor_get_cb, param.data, encap, version );
}

znet_error_t znet_node_cmd_multilevel_sensor_unknown_type_get( znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id ) {
    return znet_node_cmd_multilevel_sensor_get( node_id, channel_id, 0, 0, 1 );
}
