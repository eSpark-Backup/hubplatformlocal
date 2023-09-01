/**
 * @file znt_cmd_association.c
 * @date 11 Jun 2018
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

union AssociationInfo {
    void* data;
    znet_grouping_id_t grouping_id;
};

// node cmd association report /////////////////////////////////////////
/// INFO: Association Command Class v1..v2

void znet_cc_association_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 5 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_ASSOCIATION );
    assert( cc_data[1] == ASSOCIATION_REPORT_V2 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const size_t ncount = cc_data_len - 5;

    const size_t buff_size = sizeof( znet_association_report_t ) + ncount * sizeof( znet_node_id_t );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_association_report_t* report = (znet_association_report_t*)&buff;
    report->grouping_id = cc_data[2];
    report->max_nodes_supported = cc_data[3];
    report->node_id_count = ncount;
    for( size_t i = 0; i < ncount; ++i ) {
        report->node_ids[i] = cc_data[5 + i];
    }

    uint8_t endpoint = 0;
    if( func ) {
        endpoint = func->_endpoint;
    }
    if( znet_cb->node_cmd_association_result )
        znet_cb->node_cmd_association_result( 0, node_id, endpoint, report->grouping_id, report, znet_cb->arg );
}

// node cmd association get ////////////////////////////////////////////
/// INFO: INFO: Association Command Class v1..v2

static void _znet_node_cmd_association_get_cb( ZFunction func,
                                               void* arg,
                                               ZFuncFailures_e reason,
                                               const void* dataPtr,
                                               size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    union AssociationInfo info;
    info.data = arg;

    if( znet_cb->node_cmd_association_result )
        znet_cb->node_cmd_association_result(
            znet_get_error( reason ), nodeId, endpoint, info.grouping_id, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_association_get( znet_node_id_t node_id,
                                            znet_node_channel_id_t channel_id,
                                            znet_grouping_id_t grouping_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    union AssociationInfo info;
    info.grouping_id = grouping_id;

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_association_get(
        node_id, channel_id, grouping_id, _znet_node_cmd_association_get_cb, info.data, encap );
}

// node cmd association set ////////////////////////////////////////////
/// INFO: Association Command Class v1..v2

static void _znet_node_cmd_association_set_cb( ZFunction func,
                                               void* arg,
                                               ZFuncFailures_e reason,
                                               const void* dataPtr,
                                               size_t dataSize ) {
    if( znet_cb->node_cmd_association_set_done ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union AssociationInfo info;
        info.data = arg;

        znet_error_t err = znet_get_error( reason );
        if( err == ZNET_ERR_REQUEST_SENT_OK ) {
            err = ZNET_ERR_OK;
        }

        znet_cb->node_cmd_association_set_done( err, nodeId, endpoint, info.grouping_id, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_association_set( znet_node_id_t node_id,
                                            znet_node_channel_id_t channel_id,
                                            znet_grouping_id_t grouping_id,
                                            const znet_node_id_t* value,
                                            size_t value_count ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    /// TODO: investigate, need support report

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( !value ) {
        return ZNET_ERR_INVAL;
    }

    union AssociationInfo info;
    info.grouping_id = grouping_id;

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_association_set(
        node_id, channel_id, grouping_id, value_count, value, _znet_node_cmd_association_set_cb, info.data, encap );
}

// node cmd association remove /////////////////////////////////////////
/// INFO: Association Command Class v1..v2

static void _znet_node_cmd_association_remove_cb( ZFunction func,
                                                  void* arg,
                                                  ZFuncFailures_e reason,
                                                  const void* dataPtr,
                                                  size_t dataSize ) {
    if( znet_cb->node_cmd_association_remove_done ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_association_remove_done( znet_get_error( reason ), nodeId, endpoint, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_association_remove( znet_node_id_t node_id,
                                               znet_node_channel_id_t channel_id,
                                               znet_grouping_id_t grouping_id,
                                               const znet_node_id_t* value,
                                               size_t value_count ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    /// TODO: investigate, need support report

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( !value ) {
        return ZNET_ERR_INVAL;
    }

    void* callbackArg = NULL;
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_association_remove( node_id,
                                       channel_id,
                                       grouping_id,
                                       value_count,
                                       value,
                                       _znet_node_cmd_association_remove_cb,
                                       callbackArg,
                                       encap );
}

// node cmd association supported grouping report //////////////////////
/// INFO: Association Command Class v1..v2

void znet_cc_association_groupings_report( const ZFunction func,
                                           uint8_t node_id,
                                           int cc_data_len,
                                           const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_ASSOCIATION );
    assert( cc_data[1] == ASSOCIATION_GROUPINGS_REPORT_V2 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    uint8_t endpoint = 0;
    if( func ) {
        endpoint = func->_endpoint;
    }

    if( znet_cb->node_cmd_association_supported_grouping_result )
        znet_cb->node_cmd_association_supported_grouping_result( 0, node_id, endpoint, cc_data[2], znet_cb->arg );
}

// node cmd association supported grouping get /////////////////////////
/// INFO: Association Command Class v1..v2

static void _znet_node_cmd_association_supported_grouping_get_cb( ZFunction func,
                                                                  void* arg,
                                                                  ZFuncFailures_e reason,
                                                                  const void* dataPtr,
                                                                  size_t dataSize ) {
    if( znet_cb->node_cmd_association_supported_grouping_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );
        znet_cb->node_cmd_association_supported_grouping_result(
            znet_get_error( reason ), nodeId, endpoint, 0, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_association_supported_grouping_get( znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    void* callbackArg = NULL;
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_association_grouping_get(
        node_id, channel_id, _znet_node_cmd_association_supported_grouping_get_cb, callbackArg, encap );
}

// node cmd association specific group report //////////////////////////
/// INFO: Association Command Class v2

void znet_cc_association_specific_group_report( const ZFunction func,
                                                uint8_t node_id,
                                                int cc_data_len,
                                                const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_ASSOCIATION );
    assert( cc_data[1] == ASSOCIATION_SPECIFIC_GROUP_REPORT_V2 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_association_specific_group_result )
        znet_cb->node_cmd_association_specific_group_result( 0, node_id, func->_endpoint, znet_cb->arg );
}

// node cmd association specific group get /////////////////////////////
/// INFO: Association Command Class v2

static void _znet_node_cmd_association_specific_group_get_cb( ZFunction func,
                                                              void* arg,
                                                              ZFuncFailures_e reason,
                                                              const void* dataPtr,
                                                              size_t dataSize ) {
    if( znet_cb->node_cmd_association_specific_group_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );
        znet_cb->node_cmd_association_specific_group_result( znet_get_error( reason ), nodeId, endpoint, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_association_specific_group_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    void* callbackArg = NULL;
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_association_specific_group_get(
        node_id, channel_id, _znet_node_cmd_association_specific_group_get_cb, callbackArg, encap );
}

////////////////////////////////////////////////////////////////////////
