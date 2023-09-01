/**
 * @file znt_cmd_multichannel_association.c
 * @date 4 Jul 2018
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

// node cmd multichannel association report ////////////////////////////
/// INFO: Multi Channel Association Command Class v2..v3

void znet_cc_multichannel_association_report( const ZFunction func,
                                              uint8_t node_id,
                                              int cc_data_len,
                                              const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 5 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION );
    assert( cc_data[1] == MULTI_CHANNEL_ASSOCIATION_REPORT_V3 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const size_t nsize = cc_data_len - 5;
    size_t ncount = 0;
    int marker = 0;
    for( size_t i = 0; i < nsize; ++i ) {
        if( cc_data[5 + i] == MULTI_CHANNEL_ASSOCIATION_REPORT_MARKER_V3 ) {
            if( marker ) {
                ++i;
            }
            marker = !0;
            continue;
        }
        if( marker ) {
            ++i;
        }
        ++ncount;
    }

    const size_t buff_size = sizeof( znet_multichannel_association_report_t ) + ncount * sizeof( znet_endpoint_t );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_multichannel_association_report_t* report = (znet_multichannel_association_report_t*)&buff;
    report->grouping_id = cc_data[2];
    report->max_end_pints_supported = cc_data[3];
    report->end_pints_count = ncount;
    marker = 0;
    ncount = 0;
    for( size_t i = 0; i < nsize; ++i ) {
        if( cc_data[5 + i] == MULTI_CHANNEL_ASSOCIATION_REPORT_MARKER_V3 ) {
            if( marker ) {
                ++i;
            }
            marker = !0;
            continue;
        }
        report->end_pints[ncount].node_id = cc_data[5 + i];
        if( marker ) {
            ++i;
            report->end_pints[ncount].channel_id = cc_data[5 + i];
        }
        else {
            report->end_pints[ncount].channel_id = ZNET_CHANNEL_ID_INVALID;
        }
        ++ncount;
    }
    uint8_t endpoint = 0;
    if( func ) {
        endpoint = func->_endpoint;
    }
    if( znet_cb->node_cmd_multichannel_association_result )
        znet_cb->node_cmd_multichannel_association_result(
            0, node_id, endpoint, report->grouping_id, report, znet_cb->arg );
}

// node cmd multichannel association get ///////////////////////////////
/// INFO: INFO: Multi Channel Association Command Class v2..v3

static void _znet_node_cmd_multichannel_association_get_cb( ZFunction func,
                                                            void* arg,
                                                            ZFuncFailures_e reason,
                                                            const void* dataPtr,
                                                            size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    union AssociationInfo info;
    info.data = arg;

    if( znet_cb->node_cmd_multichannel_association_result )
        znet_cb->node_cmd_multichannel_association_result(
            znet_get_error( reason ), nodeId, endpoint, info.grouping_id, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_multichannel_association_get( znet_node_id_t node_id,
                                                         znet_node_channel_id_t channel_id,
                                                         znet_grouping_id_t grouping_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union AssociationInfo info;
    info.grouping_id = grouping_id;

    /// TODO: check wait report for node_id

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_multichannel_association_get(
        node_id, channel_id, grouping_id, _znet_node_cmd_multichannel_association_get_cb, info.data, encap );
}

// node cmd multichannel association set ///////////////////////////////
/// INFO: Multi Channel Association Command Class v2..v3

static void _znet_node_cmd_multichannel_association_set_cb( ZFunction func,
                                                            void* arg,
                                                            ZFuncFailures_e reason,
                                                            const void* dataPtr,
                                                            size_t dataSize ) {
    if( znet_cb->node_cmd_multichannel_association_set_done ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union AssociationInfo info;
        info.data = arg;

        znet_error_t err = znet_get_error( reason );
        if( err == ZNET_ERR_REQUEST_SENT_OK ) {
            err = ZNET_ERR_OK;
        }

        znet_cb->node_cmd_multichannel_association_set_done( err, nodeId, endpoint, info.grouping_id, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_multichannel_association_set( znet_node_id_t node_id,
                                                         znet_node_channel_id_t channel_id,
                                                         znet_grouping_id_t grouping_id,
                                                         const znet_endpoint_t* value,
                                                         uint8_t value_count ) {
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

    uint8_t nodes_count = 0;
    uint8_t multi_channel_nodes_count = 0;
    for( size_t i = 0; i < value_count; ++i ) {
        if( value[i].channel_id == ZNET_CHANNEL_ID_INVALID ) {
            ++nodes_count;
        }
        else {
            ++multi_channel_nodes_count;
        }
    }
    uint8_t nodes[nodes_count];
    size_t j = 0;
    uint16_t multi_channel_nodes[multi_channel_nodes_count];
    size_t k = 0;
    for( size_t i = 0; i < value_count; ++i ) {
        if( value[i].channel_id == ZNET_CHANNEL_ID_INVALID ) {
            nodes[j++] = value[i].node_id;
        }
        else {
            multi_channel_nodes[k] = value[i].node_id;
            multi_channel_nodes[k++] |= ( (uint16_t)value[i].channel_id << 8 );
        }
    }

    union AssociationInfo info;
    info.grouping_id = grouping_id;

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_multichannel_association_set( node_id,
                                                 channel_id,
                                                 grouping_id,
                                                 nodes_count,
                                                 &nodes[0],
                                                 multi_channel_nodes_count,
                                                 &multi_channel_nodes[0],
                                                 _znet_node_cmd_multichannel_association_set_cb,
                                                 info.data,
                                                 encap );
}

// node cmd multichannel association remove ////////////////////////////
/// INFO: Multi Channel Association Command Class v2..v3

static void _znet_node_cmd_multichannel_association_remove_cb( ZFunction func,
                                                               void* arg,
                                                               ZFuncFailures_e reason,
                                                               const void* dataPtr,
                                                               size_t dataSize ) {
    if( znet_cb->node_cmd_multichannel_association_remove_done ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        znet_cb->node_cmd_multichannel_association_remove_done(
            znet_get_error( reason ), nodeId, endpoint, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_multichannel_association_remove( znet_node_id_t node_id,
                                                            znet_node_channel_id_t channel_id,
                                                            znet_grouping_id_t grouping_id,
                                                            const znet_endpoint_t* value,
                                                            uint8_t value_count ) {
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

    uint8_t nodes_count = 0;
    uint8_t multi_channel_nodes_count = 0;
    for( size_t i = 0; i < value_count; ++i ) {
        if( value[i].channel_id == ZNET_CHANNEL_ID_INVALID ) {
            ++nodes_count;
        }
        else {
            ++multi_channel_nodes_count;
        }
    }
    uint8_t nodes[nodes_count];
    size_t j = 0;
    uint16_t multi_channel_nodes[multi_channel_nodes_count];
    size_t k = 0;
    for( size_t i = 0; i < value_count; ++i ) {
        if( value[i].channel_id == ZNET_CHANNEL_ID_INVALID ) {
            nodes[j++] = value[i].node_id;
        }
        else {
            multi_channel_nodes[k] = value[i].node_id;
            multi_channel_nodes[k++] |= ( (uint16_t)value[i].channel_id << 8 );
        }
    }

    void* callbackArg = NULL;
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_multichannel_association_remove( node_id,
                                                    channel_id,
                                                    grouping_id,
                                                    nodes_count,
                                                    &nodes[0],
                                                    multi_channel_nodes_count,
                                                    &multi_channel_nodes[0],
                                                    _znet_node_cmd_multichannel_association_remove_cb,
                                                    callbackArg,
                                                    encap );
}

// node cmd multichannel association supported grouping report /////////
/// INFO: Multi Channel Association Command Class v2..v3

void znet_cc_multichannel_association_groupings_report( const ZFunction func,
                                                        uint8_t node_id,
                                                        int cc_data_len,
                                                        const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION );
    assert( cc_data[1] == MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V3 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    uint8_t endpoint = 0;
    if( func ) {
        endpoint = func->_endpoint;
    }
    if( znet_cb->node_cmd_multichannel_association_supported_grouping_result )
        znet_cb->node_cmd_multichannel_association_supported_grouping_result(
            0, node_id, endpoint, cc_data[2], znet_cb->arg );
}

// node cmd multichannel association supported grouping get ////////////
/// INFO: Multi Channel Association Command Class v2..v3

static void _znet_node_cmd_multichannel_association_supported_grouping_get_cb( ZFunction func,
                                                                               void* arg,
                                                                               ZFuncFailures_e reason,
                                                                               const void* dataPtr,
                                                                               size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( znet_cb->node_cmd_multichannel_association_supported_grouping_result )
        znet_cb->node_cmd_multichannel_association_supported_grouping_result(
            znet_get_error( reason ), nodeId, endpoint, 0, znet_cb->arg );
}

znet_error_t znet_node_cmd_multichannel_association_supported_grouping_get( znet_node_id_t node_id,
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

    return znet_cc_multichannel_association_groupings_get(
        node_id, channel_id, _znet_node_cmd_multichannel_association_supported_grouping_get_cb, callbackArg, encap );
}

////////////////////////////////////////////////////////////////////////
