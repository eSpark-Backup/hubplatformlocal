/**
 * @file znet_cmd_scene.c
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

// node cmd scene activation set receive ///////////////////////////////////////////////
/// INFO: Scene Activation Command Class v1

void ICACHE_FLASH_ATTR znet_cc_scene_activation_receive_set( const ZFunction func,
                                                             uint8_t node_id,
                                                             int cc_data_len,
                                                             const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SCENE_ACTIVATION );
    assert( cc_data[1] == SCENE_ACTIVATION_SET );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_scene_activation_set_result )
        znet_cb->node_cmd_scene_activation_set_result( ZNET_ERR_OK, node_id, cc_data[2], znet_cb->arg );
}

// node cmd scene activation set ////////////////////////////////////////////////
/// INFO: Scene Activation Command Class v1

znet_error_t znet_node_cmd_scene_activation_set( znet_node_id_t node_id,
                                                 znet_node_channel_id_t channel_id,
                                                 znet_cmd_scene_id_t scene_id,
                                                 znet_cmd_scene_dimming_duration_t dimming_duration ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( scene_id == 0 ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_scene_activation_set( node_id, channel_id, scene_id, dimming_duration, NULL, NULL, encap );
}

// node cmd scene actuator configuration set ////////////////////////////////////////////////
/// INFO: Scene Actuator Configuration Command Class v1

znet_error_t znet_node_cmd_scene_actuator_configuration_set( znet_node_id_t node_id,
                                                             znet_node_channel_id_t channel_id,
                                                             znet_cmd_scene_id_t scene_id,
                                                             znet_cmd_scene_dimming_duration_t dimming_duration,
                                                             uint8_t is_override,
                                                             znet_cmd_scene_level_t level ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( scene_id == 0 ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_scene_actuator_conf_set(
        node_id, channel_id, scene_id, dimming_duration, ( is_override ? TRUE : FALSE ), level, NULL, NULL, encap );
}

// node cmd scene actuator configuration report ///////////////////////////////////////////////
/// INFO: Scene Actuator Configuration Command Class v1

void znet_cc_scene_actuator_conf_report( const ZFunction func,
                                         znet_node_id_t node_id,
                                         int cc_data_len,
                                         const uint8_t* cc_data ) {
    assert( cc_data );

    if( cc_data_len < 5 ) {
        ZNET_LOGW( "znet_cc_scene_actuator_conf_report: invalid data len %u\n", cc_data_len );
        return;
    }

    assert( cc_data[0] == ZNET_COMMAND_CLASS_SCENE_ACTUATOR_CONF );
    assert( cc_data[1] == SCENE_ACTUATOR_CONF_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    // The value MUST correspond to the format of the Value field when returning a Basic Report Command.
    if( cc_data[3] > ZNET_CMD_BASIC_VALUE_MAX && cc_data[3] != ZNET_CMD_BASIC_VALUE_UNKNOWN &&
        cc_data[3] != ZNET_CMD_BASIC_VALUE_MAX_ABSOLUTE )
        return;

    if( cc_data[4] == 0xFF )
        return;

    znet_scene_actuator_configuration_report_t report = {};
    report.scene_id = cc_data[2];
    if( cc_data[2] ) {
        report.level = cc_data[3];
        report.dimming_duration = cc_data[4];
    }
    if( znet_cb->node_cmd_scene_actuator_configuration_result )
        znet_cb->node_cmd_scene_actuator_configuration_result(
            ZNET_ERR_OK, node_id, func->_endpoint, report.scene_id, &report, znet_cb->arg );
}

union actuator_configuration_param {
    void* data;
    znet_cmd_scene_id_t scene_id;
};

// node cmd scene actuator_configuration get //////////////////////////////////////////////////
/// INFO: Meter Command Class v1
static void _znet_node_cmd_scene_actuator_configuration_get_cb( ZFunction func,
                                                                void* arg,
                                                                ZFuncFailures_e reason,
                                                                const void* dataPtr,
                                                                size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    union actuator_configuration_param param;
    param.data = arg;

    if( znet_cb->node_cmd_scene_actuator_configuration_result )
        znet_cb->node_cmd_scene_actuator_configuration_result(
            znet_get_error( reason ), nodeId, endpoint, param.scene_id, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_scene_actuator_configuration_get( znet_node_id_t node_id,
                                                             znet_node_channel_id_t channel_id,
                                                             znet_cmd_scene_id_t scene_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( scene_id == 0 ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    union actuator_configuration_param param;
    param.data = NULL;
    param.scene_id = scene_id;
    // param.data

    return znet_cc_scene_actuator_conf_get(
        node_id, channel_id, scene_id, _znet_node_cmd_scene_actuator_configuration_get_cb, param.data, encap );
}

// node cmd scene controller configuration report /////////////////////////////////////
/// INFO: Scene Controller Configuration Command Class v1

void znet_cc_scene_controller_conf_report( const ZFunction func,
                                           uint8_t node_id,
                                           int cc_data_len,
                                           const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 5 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SCENE_CONTROLLER_CONF );
    assert( cc_data[1] == SCENE_CONTROLLER_CONF_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( cc_data[2] == 0 || cc_data[3] == 0 )
        return;

    if( cc_data[4] == 0xFF )
        return;

    znet_scene_controller_configuration_report_t report = {};
    report.group_id = cc_data[2];
    report.scene_id = cc_data[3];
    report.dimming_duration = cc_data[4];

    if( znet_cb->node_cmd_scene_controller_configuration_result )
        znet_cb->node_cmd_scene_controller_configuration_result(
            ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
}

// node cmd scene controller configuration get ////////////////////////////////////////
/// INFO: Scene Controller Configuration Command Class v1
static void _znet_node_cmd_scene_controller_configuration_get_cb( ZFunction func,
                                                                  void* arg,
                                                                  ZFuncFailures_e reason,
                                                                  const void* dataPtr,
                                                                  size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_scene_controller_configuration_result )
        znet_cb->node_cmd_scene_controller_configuration_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_scene_controller_configuration_get( znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id,
                                                               znet_cmd_group_id_t group_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_scene_controller_conf_get(
        node_id, channel_id, group_id, _znet_node_cmd_scene_controller_configuration_get_cb, NULL, encap );
}

// node cmd scene controller configuration set ////////////////////////////////////////////////
/// INFO: Scene Controller Configuration Command Class v1
znet_error_t znet_node_cmd_scene_controller_configuration_set( znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id,
                                                               znet_cmd_group_id_t group_id,
                                                               znet_cmd_scene_id_t scene_id,
                                                               znet_cmd_scene_dimming_duration_t dimming_duration ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( group_id == 0 || scene_id == 0 ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_scene_controller_conf_set(
        node_id, channel_id, group_id, scene_id, dimming_duration, NULL, NULL, encap );
}

////////////////////////////////////////////////////////////////////////
