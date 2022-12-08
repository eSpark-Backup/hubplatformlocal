/**
 * @file znet_cmd_cental_scene.c
 * @date 25 Sep 2018
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

// node cmd_central_scene_configuration_report//////////////////////////////////
/// INFO: Central Scene Command  Class v3

void znet_cc_central_scene_configuration_report( const ZFunction func,
                                                 uint8_t node_id,
                                                 int cc_data_len,
                                                 const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len == sizeof( ZW_CENTRAL_SCENE_V3_CONFIGURATION_REPORT_FRAME ) );

    const ZW_CENTRAL_SCENE_V3_CONFIGURATION_REPORT_FRAME* data =
        (const ZW_CENTRAL_SCENE_V3_CONFIGURATION_REPORT_FRAME*)cc_data;

    assert( data->cmdClass == ZNET_COMMAND_CLASS_CENTRAL_SCENE );
    assert( data->cmd == CENTRAL_SCENE_CONFIGURATION_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_central_scene_configuration_result ) {
        znet_cb->node_cmd_central_scene_configuration_result(
            ZNET_ERR_OK, node_id, func->_endpoint, data->slowRefresh, znet_cb->arg );
    }
}

// node cmd central scene configuration get ////////////////////////////////////
/// INFO: Central Scene Command  Class v3
static void _znet_node_cmd_central_scene_configuration_get_cb( ZFunction func,
                                                               void* arg,
                                                               ZFuncFailures_e reason,
                                                               const void* dataPtr,
                                                               size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_central_scene_configuration_result )
        znet_cb->node_cmd_central_scene_configuration_result(
            znet_get_error( reason ), nodeId, endpoint, 0, znet_cb->arg );
}

znet_error_t znet_node_cmd_central_scene_configuration_get( znet_node_id_t node_id, znet_node_channel_id_t channelId ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_central_scene_configuration_get(
        node_id, channelId, _znet_node_cmd_central_scene_configuration_get_cb, NULL, encap );
}

// node cmd central scene configuration set /////////////////////////////////
/// INFO: Central Scene Command  Class v3
znet_error_t znet_node_cmd_central_scene_configuration_set( znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id,
                                                            uint8_t slow_refresh ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_central_scene_configuration_set( node_id, channel_id, slow_refresh, NULL, NULL, encap );
}

// node cmd_central_scene_configuration_report//////////////////////////////////
/// INFO: Central Scene Command  Class v1
void znet_cc_central_scene_supported_report_v1( const ZFunction func, uint8_t node_id, const uint8_t* cc_data ) {
    const ZW_CENTRAL_SCENE_V1_SUPPORTED_REPORT_FRAME* data = (const ZW_CENTRAL_SCENE_V1_SUPPORTED_REPORT_FRAME*)cc_data;

    assert( data );
    assert( data->cmdClass == ZNET_COMMAND_CLASS_CENTRAL_SCENE );
    assert( data->cmd == CENTRAL_SCENE_SUPPORTED_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const uint8_t supportedKeyAttributes =
        ZW_CENTRAL_SCENE_KEY_PRESSED_1_TIME | ZW_CENTRAL_SCENE_KEY_RELEASED | ZW_CENTRAL_SCENE_KEY_HELD_DOWN;

    znet_central_scene_supported_report_t report = { .scenes_num = data->supportedScenes,
                                                     .data_size = 1,
                                                     .identical = 1,
                                                     .slow_refresh = 0,
                                                     .data = &supportedKeyAttributes };

    if( znet_cb->node_cmd_central_scene_supported_result )
        znet_cb->node_cmd_central_scene_supported_result(
            ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

// node cmd_central_scene_configuration_report//////////////////////////////////
/// INFO: Central Scene Command  Class v3
void znet_cc_central_scene_supported_report_v3( const ZFunction func, uint8_t node_id, const uint8_t* cc_data ) {
    const ZW_CENTRAL_SCENE_V3_SUPPORTED_REPORT_FRAME* data = (const ZW_CENTRAL_SCENE_V3_SUPPORTED_REPORT_FRAME*)cc_data;

    assert( data );
    assert( data->cmdClass == ZNET_COMMAND_CLASS_CENTRAL_SCENE );
    assert( data->cmd == CENTRAL_SCENE_SUPPORTED_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    znet_central_scene_supported_report_t report = { .scenes_num = data->supportedScenes,
                                                     .data_size = data->bitMaskBytesNumber,
                                                     .identical = data->identical,
                                                     .slow_refresh = data->slowRefreshSupport,
                                                     .data = data->supportedKeyAttributes };

    if( znet_cb->node_cmd_central_scene_supported_result )
        znet_cb->node_cmd_central_scene_supported_result(
            ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

// node cmd_central_scene_configuration_report//////////////////////////////////
/// INFO: Central Scene Command Class
void znet_cc_central_scene_supported_report( const ZFunction func,
                                             uint8_t node_id,
                                             int cc_data_len,
                                             const uint8_t* cc_data ) {
    if( cc_data_len == sizeof( ZW_CENTRAL_SCENE_V1_SUPPORTED_REPORT_FRAME ) )
        znet_cc_central_scene_supported_report_v1( func, node_id, cc_data );
    else if( cc_data_len >= sizeof( ZW_CENTRAL_SCENE_V3_SUPPORTED_REPORT_FRAME ) )
        znet_cc_central_scene_supported_report_v3( func, node_id, cc_data );
    else
        assert( 0 );
}

// node cmd central scene supported get /////////////////////////////////////////
/// INFO: Central Scene Command  Class v3
znet_error_t znet_node_cmd_central_scene_supported_get( znet_node_id_t node_id, znet_node_channel_id_t channelId ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_central_scene_supported_get( node_id, channelId, NULL, NULL, encap );
}

// node cmd central scene nitification /////////////////////////////////////////
/// INFO: Central Scene Command  Class v3
void znet_cc_central_scene_notification( const ZFunction func,
                                         uint8_t node_id,
                                         int cc_data_len,
                                         const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len == sizeof( ZW_CENTRAL_SCENE_V3_NOTIFICATION_FRAME ) );

    const ZW_CENTRAL_SCENE_V3_NOTIFICATION_FRAME* data = (const ZW_CENTRAL_SCENE_V3_NOTIFICATION_FRAME*)cc_data;

    assert( data->cmdClass == ZNET_COMMAND_CLASS_CENTRAL_SCENE );
    assert( data->cmd == CENTRAL_SCENE_NOTIFICATION );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    znet_central_scene_notification_t report = { .sequence_number = data->sequenceNumber,
                                                 .slow_refresh = data->slowRefresh,
                                                 .key_attribute = data->keyAttribute,
                                                 .scene_number = data->sceneNumber };

    if( znet_cb->node_cmd_central_scene_notification )
        znet_cb->node_cmd_central_scene_notification( ZNET_ERR_OK, node_id, &report, znet_cb->arg );
}
////////////////////////////////////////////////////////////////////////
