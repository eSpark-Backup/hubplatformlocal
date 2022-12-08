/**
 * @file znt_cmd_association_group_info.c
 * @date 12 Jun 2018
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

// node cmd association group name report //////////////////////////////
/// INFO: Association Group Info Command Class v1..v3

void znet_cc_association_group_name_report( const ZFunction func,
                                            uint8_t node_id,
                                            int cc_data_len,
                                            const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO );
    assert( cc_data[1] == ASSOCIATION_GROUP_NAME_REPORT_V3 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const size_t nsize = cc_data[3];

    const size_t buff_size =
        sizeof( znet_association_group_name_report_t ) + nsize * ( sizeof( char ) + 1 /*for NULL*/ );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_association_group_name_report_t* report = (znet_association_group_name_report_t*)&buff;
    report->grouping_id = cc_data[2];
    report->name_size = nsize;
    memcpy( report->name, &cc_data[4], nsize );

    if( znet_cb->node_cmd_association_group_name_result )
        znet_cb->node_cmd_association_group_name_result(
            0, node_id, func->_endpoint, report->grouping_id, report, znet_cb->arg );
}

// node cmd association group name get /////////////////////////////////
/// INFO: INFO: Association Group Info Command Class v1..v3

static void _znet_node_cmd_association_group_name_get_cb( ZFunction func,
                                                          void* arg,
                                                          ZFuncFailures_e reason,
                                                          const void* dataPtr,
                                                          size_t dataSize ) {
    if( znet_cb->node_cmd_association_group_name_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union AssociationInfo info;
        info.data = arg;

        znet_cb->node_cmd_association_group_name_result(
            znet_get_error( reason ), nodeId, endpoint, info.grouping_id, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_association_group_name_get( znet_node_id_t node_id,
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

    return znet_cc_association_group_name_get(
        node_id, channel_id, grouping_id, _znet_node_cmd_association_group_name_get_cb, info.data, encap );
}

// node cmd association group info report //////////////////////////////
/// INFO: Association Group Info Command Class v1..v3

void znet_cc_association_group_info_report( const ZFunction func,
                                            uint8_t node_id,
                                            int cc_data_len,
                                            const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO );
    assert( cc_data[1] == ASSOCIATION_GROUP_INFO_REPORT_V3 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const size_t gcount = cc_data[2] & 0x3F;

    const size_t buff_size = sizeof( znet_association_group_info_report_t ) +
                             gcount * sizeof( ( (znet_association_group_info_report_t*)NULL )->group_infos[0] );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_association_group_info_report_t* report = (znet_association_group_info_report_t*)&buff;
    report->list_mode = ( cc_data[2] & 0x80 ) == 0x80;
    report->dynamic_info = ( cc_data[2] & 0x40 ) == 0x40;
    report->group_info_count = gcount;
    for( size_t i = 0; i < gcount; ++i ) {
        report->group_infos[i].grouping_id = cc_data[7 * i + 3];
        report->group_infos[i].profile = cc_data[7 * i + 6];
        report->group_infos[i].profile |= ( cc_data[7 * i + 5] << 8 );
    }

    if( znet_cb->node_cmd_association_group_info_result )
        znet_cb->node_cmd_association_group_info_result(
            0, node_id, func->_endpoint, report->group_infos[0].grouping_id, report, znet_cb->arg );
}

// node cmd association group info get /////////////////////////////////
/// INFO: Association Group Info Command Class v1..v3

static void _znet_node_cmd_association_group_info_get_cb( ZFunction func,
                                                          void* arg,
                                                          ZFuncFailures_e reason,
                                                          const void* dataPtr,
                                                          size_t dataSize ) {
    if( znet_cb->node_cmd_association_group_info_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union AssociationInfo info;
        info.data = arg;

        znet_cb->node_cmd_association_group_info_result(
            znet_get_error( reason ), nodeId, endpoint, info.grouping_id, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_association_group_info_get( znet_node_id_t node_id,
                                                       znet_node_channel_id_t channel_id,
                                                       znet_grouping_id_t grouping_id,
                                                       int list_mode,
                                                       int refresh_cache ) {
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

    return znet_cc_association_group_info_get( node_id,
                                               channel_id,
                                               list_mode,
                                               refresh_cache,
                                               grouping_id,
                                               _znet_node_cmd_association_group_info_get_cb,
                                               info.data,
                                               encap );
}

// node cmd association group commands list report /////////////////////
/// INFO: Association Command Class v1..v3

void znet_cc_association_group_command_list_report( const ZFunction func,
                                                    uint8_t node_id,
                                                    int cc_data_len,
                                                    const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO );
    assert( cc_data[1] == ASSOCIATION_GROUP_COMMAND_LIST_REPORT_V3 );
    //    assert( cc_data[3] + 4 <= cc_data_len );

    size_t ccount = 0;
    for( size_t i = 0; i < cc_data[3]; ++i ) {
        if( cc_data[4 + i] >= 0xF1 )  // MSB
        {
            /// TODO: implement extended command class (now skiped)
            i += 2;
            continue;
        }
        ++i;
        ++ccount;
    }

    const size_t buff_size = sizeof( znet_association_group_command_list_report_t ) +
                             ccount * sizeof( ( (znet_association_group_command_list_report_t*)NULL )->commands[0] );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_association_group_command_list_report_t* report = (znet_association_group_command_list_report_t*)&buff;
    report->grouping_id = cc_data[2];
    report->commands_count = ccount;
    size_t j = 0;
    for( size_t i = 0; i < cc_data[3]; ++i ) {
        if( cc_data[4 + i] >= 0xF1 )  // MSB
        {
            /// TODO: implement extended command class (now skiped)
            i += 2;
            continue;
        }
        report->commands[j].command = cc_data[4 + i];
        ++i;
        report->commands[j].sub_command = cc_data[4 + i];
        ++j;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_association_group_command_list_result )
        znet_cb->node_cmd_association_group_command_list_result(
            0, node_id, func->_endpoint, report->grouping_id, report, znet_cb->arg );
}

// node cmd association group commands list get ////////////////////////
/// INFO: Association Command Class v1..v3

static void _znet_node_cmd_association_group_command_list_get_cb( ZFunction func,
                                                                  void* arg,
                                                                  ZFuncFailures_e reason,
                                                                  const void* dataPtr,
                                                                  size_t dataSize ) {
    if( znet_cb->node_cmd_association_group_command_list_result ) {
        uint8_t nodeId, endpoint;
        ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

        union AssociationInfo info;
        info.data = arg;

        znet_cb->node_cmd_association_group_command_list_result(
            znet_get_error( reason ), nodeId, endpoint, info.grouping_id, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_association_group_command_list_get( znet_node_id_t node_id,
                                                               znet_node_channel_id_t channel_id,
                                                               znet_grouping_id_t grouping_id,
                                                               int allow_cache ) {
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

    return znet_cc_association_group_command_list_get( node_id,
                                                       channel_id,
                                                       allow_cache,
                                                       grouping_id,
                                                       _znet_node_cmd_association_group_command_list_get_cb,
                                                       info.data,
                                                       encap );
}

////////////////////////////////////////////////////////////////////////
