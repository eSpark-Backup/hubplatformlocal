/**
 * @file znet_cmd_multichannel.c
 * @date 20 Apr 2018
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
#include <znet_lib_cc_codes.h>
#include <znet_lib_cc_transport.h>

// node cmd multichannel endpoint report ///////////////////////////////
/// INFO: Multi Channel Command Class v3..v4

void znet_cc_multichannel_endpoint_report( const ZFunction func,
                                           uint8_t node_id,
                                           int cc_data_len,
                                           const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_MULTI_CHANNEL );
    assert( cc_data[1] == MULTI_CHANNEL_END_POINT_REPORT_V4 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    znet_multichannel_endpoint_report_t report = {};
    report.dynamic = ( cc_data[2] & 0x80 ) == 0x80;
    report.identical = ( cc_data[2] & 0x40 ) == 0x40;
    report.channel_ids_count = cc_data[3];
    if( cc_data_len >= 5 ) {
        report.agregated_channel_ids_count = cc_data[4];
    }

    if( znet_cb->node_cmd_multichannel_endpoint_result )
        znet_cb->node_cmd_multichannel_endpoint_result( 0, node_id, &report, znet_cb->arg );
}

// node cmd multichannel endpoint get //////////////////////////////////
/// INFO: Multi Channel Command Class v3..v4

static void _znet_node_cmd_multichannel_endpoint_get_cb( ZFunction func,
                                                         void* arg,
                                                         ZFuncFailures_e reason,
                                                         const void* dataPtr,
                                                         size_t dataSize ) {
    uint8_t nodeId;
    ZNet_CheckCallback( func, reason, &nodeId, NULL );

    if( znet_cb->node_cmd_multichannel_endpoint_result )
        znet_cb->node_cmd_multichannel_endpoint_result( znet_get_error( reason ), nodeId, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_multichannel_endpoint_get( znet_node_id_t node_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check in storage node_id
    /// TODO: check wait report for node_id

    /// TODO: investigte, for which the arg is used in
    /// znet_cc_multichannel_endpoint_get
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_multichannel_endpoint_get( node_id, _znet_node_cmd_multichannel_endpoint_get_cb, NULL, encap );
}

// node cmd multichannel capability report /////////////////////////////
/// INFO: Multi Channel Command Class v3..v4

void znet_cc_multichannel_capability_report( const ZFunction func,
                                             uint8_t node_id,
                                             int cc_data_len,
                                             const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 5 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_MULTI_CHANNEL );
    assert( cc_data[1] == MULTI_CHANNEL_CAPABILITY_REPORT_V4 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    znet_multichannel_capability_report_t report = {};
    report.dynamic = ( cc_data[2] & 0x80 ) == 0x80;
    report.channel_id = cc_data[2] & 0x7F;
    report.generic = cc_data[3];
    report.specific = cc_data[4];
    report.commands_count = cc_data_len - 5;
    report.commands = cc_data + 5;

    if( znet_cb->node_cmd_multichannel_capability_result )
        znet_cb->node_cmd_multichannel_capability_result( 0, node_id, report.channel_id, &report, znet_cb->arg );
}

// node cmd multichannel capability get ////////////////////////////////
/// INFO: Multi Channel Command Class v3..v4

union multichannel_capabilities_param {
    void* data;
    uint8_t channel_id;
};

static void _znet_node_cmd_multichannel_capability_get_cb( ZFunction func,
                                                           void* arg,
                                                           ZFuncFailures_e reason,
                                                           const void* dataPtr,
                                                           size_t dataSize ) {
    uint8_t nodeId;
    ZNet_CheckCallback( func, reason, &nodeId, NULL );

    union multichannel_capabilities_param param;
    param.data = arg;

    if( znet_cb->node_cmd_multichannel_capability_result )
        znet_cb->node_cmd_multichannel_capability_result(
            znet_get_error( reason ), nodeId, param.channel_id, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_multichannel_capability_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( channel_id < ZNET_CHANNEL_ID_MIN || channel_id > ZNET_CHANNEL_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check in storage node_id
    /// TODO: check wait report for node_id

    /// TODO: investigte, for which the arg is used in
    /// znet_cc_multichannel_capability_get

    union multichannel_capabilities_param param;
    param.data = NULL;
    param.channel_id = channel_id;

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_multichannel_capability_get(
        node_id, channel_id, _znet_node_cmd_multichannel_capability_get_cb, param.data, encap );
}

// node cmd multichannel endpoint find report //////////////////////////
/// INFO: Multi Channel Command Class v3..v4

void znet_cc_multichannel_endpoint_find_report( const ZFunction func,
                                                uint8_t node_id,
                                                int cc_data_len,
                                                const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 5 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_MULTI_CHANNEL );
    assert( cc_data[1] == MULTI_CHANNEL_END_POINT_FIND_REPORT_V4 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    size_t ecount = cc_data_len - 5;

    const size_t buff_size =
        sizeof( znet_multichannel_endpoint_find_report_t ) + ecount * sizeof( znet_node_channel_id_t );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_multichannel_endpoint_find_report_t* report = (znet_multichannel_endpoint_find_report_t*)&buff;
    report->reports_to_follow = cc_data[2];
    report->generic = cc_data[3];
    report->specific = cc_data[4];
    report->channel_ids_count = ecount;
    for( size_t i = 0; i < ecount; ++i ) {
        report->channel_ids[i] = cc_data[5 + i];
    }

    if( znet_cb->node_cmd_multichannel_endpoint_find_result ) {
        znet_cb->node_cmd_multichannel_endpoint_find_result( 0, node_id, report, znet_cb->arg );
    }
}

// node cmd multichannel endpoint find /////////////////////////////////
/// INFO: Multi Channel Command Class v3..v4

union MiltiChannelEndpointFindInfo {
    struct {
        znet_generic_class_t generic;
        znet_specific_class_t specific;
    };
    void* data;
};

static void _znet_node_cmd_multichannel_endpoint_find_cb( ZFunction func,
                                                          void* arg,
                                                          ZFuncFailures_e reason,
                                                          const void* dataPtr,
                                                          size_t dataSize ) {
    uint8_t nodeId;
    ZNet_CheckCallback( func, reason, &nodeId, NULL );

    union MiltiChannelEndpointFindInfo param;
    param.data = arg;

    if( znet_cb->node_cmd_multichannel_endpoint_find_result )
        znet_cb->node_cmd_multichannel_endpoint_find_result( znet_get_error( reason ), nodeId, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_multichannel_endpoint_find( znet_node_id_t node_id,
                                                       znet_generic_class_t generic,
                                                       znet_specific_class_t specific,
                                                       bool do_request ) {
    if( !do_request ) {
        ZNET_LOGI( "Next packages have to be sent automatically\n" );
        return ZNET_ERR_OK;
    }

    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union MiltiChannelEndpointFindInfo param;
    param.generic = generic;
    param.specific = specific;

    /// TODO: check in storage node_id
    /// TODO: check wait report for node_id

    /// TODO: investigte, for which the arg is used in
    /// znet_cc_multichannel_endpoint_find
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_multichannel_endpoint_find(
        node_id, generic, specific, _znet_node_cmd_multichannel_endpoint_find_cb, param.data, encap );
}

// node cmd multichannel aggregated members report /////////////////////
/// INFO: Multi Channel Command Class v4

void znet_cc_multichannel_aggregated_members_report( const ZFunction func,
                                                     uint8_t node_id,
                                                     int cc_data_len,
                                                     const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_MULTI_CHANNEL );
    assert( cc_data[1] == MULTI_CHANNEL_AGGREGATED_MEMBERS_REPORT_V4 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    size_t bmcount = cc_data[3];
    assert( cc_data_len >= 4 + bmcount );

    size_t acount = 0;
    for( size_t i = 0; i < bmcount; ++i ) {
        for( size_t j = 0; j < 8; ++j ) {
            const uint8_t mask = 0x01 << j;
            if( ( cc_data[4 + i] & mask ) == mask )
                ++acount;
        }
    }

    const size_t buff_size =
        sizeof( znet_multichannel_aggregated_members_report_t ) + acount * sizeof( znet_node_channel_id_t );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_multichannel_aggregated_members_report_t* report = (znet_multichannel_aggregated_members_report_t*)&buff;
    report->channel_id = cc_data[2] & 0x7F;
    report->agregated_channel_ids_count = acount;
    size_t a = 0;
    for( size_t i = 0; i < bmcount; ++i ) {
        for( size_t j = 0; j < 8; ++j ) {
            const uint8_t mask = 0x01 << j;
            if( ( cc_data[4 + i] & mask ) == mask )
                report->agregated_channel_ids[a++] = i * 8 + j + 1;
        }
    }

    if( znet_cb->node_cmd_multichannel_aggregated_members_result )
        znet_cb->node_cmd_multichannel_aggregated_members_result(
            0, node_id, report->channel_id, report, znet_cb->arg );
}

// node cmd multichannel aggregated members get ////////////////////////
/// INFO: Multi Channel Command Class v4

static void _znet_node_cmd_multichannel_aggregated_members_get_cb( ZFunction func,
                                                                   void* arg,
                                                                   ZFuncFailures_e reason,
                                                                   const void* dataPtr,
                                                                   size_t dataSize ) {
    uint8_t nodeId;
    ZNet_CheckCallback( func, reason, &nodeId, NULL );

    union multichannel_capabilities_param param;
    param.data = arg;

    if( znet_cb->node_cmd_multichannel_aggregated_members_result )
        znet_cb->node_cmd_multichannel_aggregated_members_result(
            znet_get_error( reason ), nodeId, param.channel_id, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_multichannel_aggregated_members_get( znet_node_id_t node_id,
                                                                znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( channel_id < ZNET_CHANNEL_ID_MIN || channel_id > ZNET_CHANNEL_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union multichannel_capabilities_param param;
    param.data = NULL;
    param.channel_id = channel_id;

    /// TODO: check in storage node_id
    /// TODO: check wait report for node_id

    /// TODO: investigte, for which the arg is used in
    /// znet_cc_multichannel_aggregated_members_get
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    return znet_cc_multichannel_aggregated_members_get(
        node_id, channel_id, _znet_node_cmd_multichannel_aggregated_members_get_cb, param.data, encap );
}
