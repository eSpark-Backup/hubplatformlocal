/**
 * @file znet_cmd_door_lock.c
 * @date 14 Oct 2018
 * @brief TBD.
 */

/// INFO: crt & system
#include <assert.h>
#include <string.h>

/// INFO: public
#include "znet/znet.h"

/// INFO: private
#include "znet_main.h"
#include "znet_log.h"
#include "znet_store.h"

/// INFO: internal
#include "heap.h"
#include "znet_lib.h"
#include "znet_lib_cc_application.h"

/// INFO: Door Lock Command Class v3

/*********************************************************************************************************************/

void znet_cc_door_lock_configuration_report( const ZFunction func,
                                             uint8_t node_id,
                                             int cc_data_len,
                                             const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 6 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_DOOR_LOCK );
    assert( cc_data[1] == DOOR_LOCK_CONFIGURATION_REPORT_V3 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_door_lock_configuration_result ) {
        znet_door_lock_configuration_t report = {};

        const ZW_DOOR_LOCK_CONFIGURATION_REPORT_V2_FRAME* frame_v2 =
            (const ZW_DOOR_LOCK_CONFIGURATION_REPORT_V2_FRAME*)cc_data;
        report.are_timed_operations_enabled =
            frame_v2->operationType == DOOR_LOCK_CONFIGURATION_REPORT_TIMED_OPERATION &&
            frame_v2->lockTimeoutMinutes != 0xFE && frame_v2->lockTimeoutSeconds != 0xFE;
        report.enabled_inside_handle_ids = frame_v2->insideDoorHandleModes;
        report.enabled_outside_handle_ids = frame_v2->outsideDoorHandleModes;

        report.lock_timeout =
            report.are_timed_operations_enabled ? frame_v2->lockTimeoutMinutes * 60 + frame_v2->lockTimeoutSeconds : 0;

        if( (size_t)cc_data_len >= sizeof( ZW_DOOR_LOCK_OPERATION_REPORT_V2_FRAME ) ) {
            znet_door_lock_extended_configuration_t extended_report = {};

            const ZW_DOOR_LOCK_CONFIGURATION_REPORT_V4_FRAME* frame_v4 =
                (const ZW_DOOR_LOCK_CONFIGURATION_REPORT_V4_FRAME*)cc_data;

            extended_report.auto_relock_timeout =
                ( uint16_t )( frame_v4->autoRelockTimeLsb | ( frame_v4->autoRelockTimeMsb << 8 ) );
            extended_report.hold_and_release_timeout =
                ( uint16_t )( frame_v4->holdAndReleaseTimeLsb | ( frame_v4->holdAndReleaseTimeMsb << 8 ) );

            extended_report.twist_assist_enabled = frame_v4->twistAsisitEnabled;
            extended_report.block_to_block_enabled = frame_v4->blockToBlockEnabled;

            znet_cb->node_cmd_door_lock_configuration_result(
                ZNET_ERR_OK, node_id, func->_endpoint, &report, &extended_report, znet_cb->arg );
        }
        else {
            znet_cb->node_cmd_door_lock_configuration_result(
                ZNET_ERR_OK, node_id, func->_endpoint, &report, NULL, znet_cb->arg );
        }
    }
}

/*********************************************************************************************************************/

void znet_cc_door_lock_operation_report( const ZFunction func,
                                         uint8_t node_id,
                                         int cc_data_len,
                                         const uint8_t* cc_data ) {
    assert( cc_data );
    assert( (size_t)cc_data_len >= sizeof( ZW_DOOR_LOCK_OPERATION_REPORT_V2_FRAME ) );

    const ZW_DOOR_LOCK_OPERATION_REPORT_V2_FRAME* frame_v2 = (const ZW_DOOR_LOCK_OPERATION_REPORT_V2_FRAME*)cc_data;

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_door_lock_operation_result ) {
        znet_door_lock_operation_report_t report = {};

        report.mode = frame_v2->doorLockMode;
        report.enabled_outside_handle_ids = frame_v2->outsideDoorHandleModes;
        report.enabled_inside_handle_ids = frame_v2->insideDoorHandleModes;
        report.condition = frame_v2->doorCondition;
        report.is_lock_timeout_activated = frame_v2->lockTimeoutMinutes != 0xFE && frame_v2->lockTimeoutSeconds != 0xFE;
        report.lock_timeout =
            report.is_lock_timeout_activated ? frame_v2->lockTimeoutMinutes * 60 + frame_v2->lockTimeoutSeconds : 0;

        if( (size_t)cc_data_len >= sizeof( ZW_DOOR_LOCK_OPERATION_REPORT_V4_FRAME ) ) {
            const ZW_DOOR_LOCK_OPERATION_REPORT_V4_FRAME* frame_v4 =
                (const ZW_DOOR_LOCK_OPERATION_REPORT_V4_FRAME*)cc_data;
            report.has_target_mode = true;
            report.tagret_mode = frame_v4->targetDoorLockMode;
            report.tagret_mode_timeout = frame_v4->targetDoorLockModeTimeout;
        }
        else {
            report.has_target_mode = false;
        }

        znet_cb->node_cmd_door_lock_operation_result( ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
    }
}

/*********************************************************************************************************************/

void znet_cc_door_lock_capabilities_report( const ZFunction func,
                                            uint8_t node_id,
                                            int cc_data_len,
                                            const uint8_t* cc_data ) {
    assert( cc_data );
    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_door_lock_capabilities_result ) {
        znet_door_lock_capabilities_t report = {};

        {
            const ZW_DOOR_LOCK_CAPABILITIES_REPORT_SUB_FRAME_1* subFrame =
                (const ZW_DOOR_LOCK_CAPABILITIES_REPORT_SUB_FRAME_1*)cc_data;

            report.are_timed_operations_supported =
                subFrame->operationTypesBitmaskLength &&
                ( subFrame->operationTypesBitmask[0] & ( 1 << DOOR_LOCK_CONFIGURATION_REPORT_TIMED_OPERATION ) );

            cc_data += sizeof( ZW_DOOR_LOCK_CAPABILITIES_REPORT_SUB_FRAME_1 ) + subFrame->operationTypesBitmaskLength;
        }

        {
            const ZW_DOOR_LOCK_CAPABILITIES_REPORT_SUB_FRAME_2* subFrame =
                (const ZW_DOOR_LOCK_CAPABILITIES_REPORT_SUB_FRAME_2*)cc_data;

            report.supported_door_lock_modes_count = subFrame->supportedModesLength;
            report.supported_door_lock_modes = subFrame->supportedModes;

            cc_data += sizeof( ZW_DOOR_LOCK_CAPABILITIES_REPORT_SUB_FRAME_2 ) + subFrame->supportedModesLength;
        }

        {
            const ZW_DOOR_LOCK_CAPABILITIES_REPORT_SUB_FRAME_3* subFrame =
                (const ZW_DOOR_LOCK_CAPABILITIES_REPORT_SUB_FRAME_3*)cc_data;

            report.supported_inside_handle_ids = subFrame->supportedInsideDoorHandleModes;
            report.supported_outside_handle_ids = subFrame->supportedOutsideDoorHandleModes;
            report.is_bolt_supported = subFrame->isBoltSupported;
            report.is_door_supported = subFrame->isDoorSupported;
            report.is_latch_supported = subFrame->isLatchSupported;
            report.is_twist_assist_supported = subFrame->isTwistAssistSupported;
            report.is_block_to_block_supported = subFrame->isBlockToBlockSupported;
            report.is_hold_and_release_supported = subFrame->isHoldAndReleaseSupported;
            report.is_auto_relock_supported = subFrame->isAutoRelockSupported;
        }

        znet_cb->node_cmd_door_lock_capabilities_result( ZNET_ERR_OK, node_id, func->_endpoint, &report, znet_cb->arg );
    }
}

/*********************************************************************************************************************/

znet_error_t znet_node_cmd_door_lock_configuration_set(
    znet_node_id_t node_id,
    znet_node_channel_id_t channel_id,
    const znet_door_lock_configuration_t* value,
    const znet_door_lock_extended_configuration_t* extended_configuration ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    assert( value );

    if( value->are_timed_operations_enabled && value->lock_timeout >= 0xFE * 60 ) {
        return ZNET_ERR_INVAL;
    }

    const uint8_t operation_type = value->are_timed_operations_enabled ? DOOR_LOCK_CONFIGURATION_SET_TIMED_OPERATION :
                                                                         DOOR_LOCK_CONFIGURATION_SET_CONSTANT_OPERATION;

    const uint8_t timeout_mins = value->are_timed_operations_enabled ? ( uint8_t )( value->lock_timeout / 60 ) : 0xFE;

    const uint8_t timeout_secs = value->are_timed_operations_enabled ? value->lock_timeout % 60 : 0xFE;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    const znet_door_lock_extended_configuration_t default_extended_configuration = {};
    extended_configuration = extended_configuration ? extended_configuration : &default_extended_configuration;

    return znet_cc_door_lock_configuration_set( node_id,
                                                channel_id,
                                                operation_type,
                                                value->enabled_inside_handle_ids,
                                                value->enabled_outside_handle_ids,
                                                timeout_mins,
                                                timeout_secs,
                                                extended_configuration->auto_relock_timeout,
                                                extended_configuration->hold_and_release_timeout,
                                                extended_configuration->block_to_block_enabled,
                                                extended_configuration->twist_assist_enabled,
                                                NULL,
                                                NULL,
                                                encap );
}

/*********************************************************************************************************************/

znet_error_t znet_node_cmd_door_lock_operation_set( znet_node_id_t node_id,
                                                    znet_node_channel_id_t channel_id,
                                                    znet_cmd_door_lock_operation_set_t value ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    switch( value ) {
    case DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED:
    case DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_WITH_TIMEOUT:
    case DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES:
    case DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES_WITH_TIMEOUT:
    case DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES:
    case DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES_WITH_TIMEOUT:
    case DOOR_LOCK_OPERATION_SET_DOOR_SECURED:
        break;

    default:
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_door_lock_operation_set( node_id, channel_id, value, NULL, NULL, encap );
}

/*********************************************************************************************************************/

static void _znet_node_cmd_door_lock_configuration_get_cb( ZFunction func,
                                                           void* arg,
                                                           ZFuncFailures_e reason,
                                                           const void* dataPtr,
                                                           size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_door_lock_configuration_result )
        znet_cb->node_cmd_door_lock_configuration_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_door_lock_configuration_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_door_lock_configuration_get(
        node_id, channel_id, _znet_node_cmd_door_lock_configuration_get_cb, NULL, encap );
}

/*********************************************************************************************************************/

static void _znet_node_cmd_door_lock_operation_get_cb( ZFunction func,
                                                       void* arg,
                                                       ZFuncFailures_e reason,
                                                       const void* dataPtr,
                                                       size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_door_lock_operation_result )
        znet_cb->node_cmd_door_lock_operation_result( znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
}

/*********************************************************************************************************************/

static void _znet_node_cmd_door_lock_capabilities_get_cb( ZFunction func,
                                                          void* arg,
                                                          ZFuncFailures_e reason,
                                                          const void* dataPtr,
                                                          size_t dataSize ) {
    if( znet_cb->node_cmd_door_lock_capabilities_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_door_lock_capabilities_result(
            znet_get_error( reason ), nodeId, endpoint, NULL, znet_cb->arg );
    }
}

/*********************************************************************************************************************/

znet_error_t znet_node_cmd_door_lock_operation_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_door_lock_operation_get(
        node_id, channel_id, _znet_node_cmd_door_lock_operation_get_cb, NULL, encap );
}

znet_error_t znet_node_cmd_door_lock_capabilities_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_door_lock_capabilities_get(
        node_id, channel_id, _znet_node_cmd_door_lock_capabilities_get_cb, NULL, encap );
}
