/*
 * znet_cmd_binary_sensor.c
 *
 *  Created on: Dec 18, 2018
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

/*!****************************************************************************
 */
void znet_cc_sensor_binary_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data_len >= 3 );

    if( znet_cb->node_cmd_binary_sensor_get_result ) {
        uint8_t sensor_type = 0;
        if( cc_data_len == 4 ) {
            sensor_type = cc_data[3];
        }
        znet_cb->node_cmd_binary_sensor_get_result(
            ZNET_ERR_OK, node_id, func->_endpoint, sensor_type, cc_data[2], znet_cb->arg );
    }
}

union sensor_param {
    void* data;
    uint8_t sensor_type;
};

static void _znet_node_cmd_sensor_binary_get_cb( ZFunction func,
                                                 void* arg,
                                                 ZFuncFailures_e reason,
                                                 const void* dataPtr,
                                                 size_t dataSize ) {
    if( znet_cb->node_cmd_binary_sensor_get_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union sensor_param param;
        param.data = arg;
        znet_cb->node_cmd_binary_sensor_get_result(
            znet_get_error( reason ), nodeId, endpoint, param.sensor_type, 0, znet_cb->arg );
    }
}

/**
 * @brief Node cmd Binary Sensor Get
 * @param node_id       Destination Node Id
 * @param version       CC version
 * @return status operation
 */
znet_error_t znet_node_cmd_sensor_binary_get( znet_node_id_t node_id,
                                              znet_node_channel_id_t channelId,
                                              uint8_t sensor_type ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union sensor_param param;
    param.data = NULL;
    param.sensor_type = sensor_type;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_sensor_binary_get(
        node_id, channelId, sensor_type, _znet_node_cmd_sensor_binary_get_cb, param.data, encap, 2 );
}

znet_error_t znet_node_cmd_sensor_binary_get_v1( znet_node_id_t node_id, znet_node_channel_id_t channelId ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( node_id > ZNET_CMD_BINARY_SENSOR_GLASSBREAK && node_id != ZNET_CMD_BINARY_SENSOR_1STSUPPORTEDSENSOR ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_sensor_binary_get( node_id, channelId, 0, _znet_node_cmd_sensor_binary_get_cb, NULL, encap, 1 );
}

/*!****************************************************************************
 */
void znet_cc_sensor_binary_supported_report( const ZFunction func,
                                             uint8_t node_id,
                                             int cc_data_len,
                                             const uint8_t* cc_data ) {
    assert( cc_data_len >= 3 );

    if( znet_cb->node_cmd_binary_sensor_get_supported_sensor_result )
        znet_cb->node_cmd_binary_sensor_get_supported_sensor_result(
            ZNET_ERR_OK, node_id, func->_endpoint, cc_data + 2, cc_data_len - 2, znet_cb->arg );
}

static void _znet_node_cmd_sensor_binary_get_supported_sensor_cb( ZFunction func,
                                                                  void* arg,
                                                                  ZFuncFailures_e reason,
                                                                  const void* dataPtr,
                                                                  size_t dataSize ) {
    if( znet_cb->node_cmd_binary_sensor_get_supported_sensor_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );
        znet_cb->node_cmd_binary_sensor_get_supported_sensor_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, 0, znet_cb->arg );
    }
}

/**
 * @brief Node cmd Binary Sensor Get Supported Sensor
 * @param node_id       Destination Node Id
 * @param version       CC version
 * @return status operation
 */
znet_error_t znet_node_cmd_sensor_binary_supported_get( znet_node_id_t node_id,
                                                        znet_node_channel_id_t channelId,
                                                        uint8_t version ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_sensor_binary_supported_get(
        node_id, channelId, _znet_node_cmd_sensor_binary_get_supported_sensor_cb, NULL, encap, version );
}
