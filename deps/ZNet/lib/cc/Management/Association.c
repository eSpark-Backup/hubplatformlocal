
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "byte_ops.h"

//#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"
//#include "znet_fc_private.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"
#include "znet_store.h"
#include "S2_wrap.h"
#include "logger.h"
#include "ZW_transport_api.h"
/// TODO: investigate (config_app.h missing with BLE_MAX_NODES)
#if( !defined( __linux__ ) && !defined( ESP_PLATFORM ) && !defined( __APPLE__ ) )
#include "config_app.h"
#endif

#define MAX_ASSOCIATION_GROUP 1

const uint8_t max_association_groups = MAX_ASSOCIATION_GROUP;
uint8_t association_grouping_id[MAX_ASSOCIATION_GROUP][ZW_MAX_NODES] = { 0 };

extern void ICACHE_FLASH_ATTR learn_ok( uint8_t node_id, bool multichannel );
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Send Association Get
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: group_id
// @default: 0
// Group Id (from 1 to 255)
// 0 requests all groups
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_get( uint8_t node_id,
                                                        uint8_t channel_id,
                                                        uint8_t grouping_id,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg,
                                                        encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_ASSOCIATION, ASSOCIATION_GET, grouping_id };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send Association Set (Add)
//
// @param: znet
// ZNet object instance
//
// @param: channel_id
// Destination channel Id
//
// @param: node_id
// Destination Node Id
//
// @param: group_id
// Group Id (from 1 to 255)
//
// @param: node_count
// Count of the Nodes to be added to the group
//
// @param: nodes
// Nodes to be added to the group
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_set( uint8_t node_id,
                                                        uint8_t channel_id,
                                                        uint8_t group_id,
                                                        int node_count,
                                                        const uint8_t* nodes,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg,
                                                        encap_type_t encap_type ) {
    Log_Write( "znet_cc_association_set\r\n" );

    uint8_t data_len = 3 * sizeof( uint8_t ) + node_count;
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    data[0] = COMMAND_CLASS_ASSOCIATION;
    data[1] = ASSOCIATION_SET;
    data[2] = group_id;
    if( group_id != 0 )
        group_id--;
    if( node_count > 0 )
        memcpy( &data[3], nodes, node_count );
    for( uint8_t k = 0; k < node_count; k++ ) {
        if( max_association_groups >= data[2] )
            for( uint8_t j = 0; j < ZW_MAX_NODES; j++ ) {
                if( association_grouping_id[group_id][j] == node_id )
                    break;
                if( association_grouping_id[group_id][j] == 0 ) {
                    Log_Write( "add node %d to group %d\r\n", node_id, group_id );
                    association_grouping_id[group_id][j] = node_id;
                    break;
                }
            }
    }

    znet_store_associations_set( group_id, association_grouping_id[group_id] );

    print_hex_buf( association_grouping_id[group_id], ZW_MAX_NODES );
    /// TODO: check channel support
    znet_error_t result = znet_fc_send_data( node_id, channel_id, data_len, data, Callback, callbackArg, encap_type );
    Heap_Free( data );

    return result;
}

//----------------------------------------------------------------------------
// Send Association Remove
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: group_id
// Group Id (from 1 to 255)
//
// @param: node_count
// Count of the Nodes to be removed from the group
//
// @param: nodes
// Nodes to be removed from the group
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_remove( uint8_t node_id,
                                                           uint8_t channel_id,
                                                           uint8_t group_id,
                                                           int node_count,
                                                           const uint8_t* nodes,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg,
                                                           encap_type_t encap_type ) {
    uint8_t data_len = 3 * sizeof( uint8_t ) + node_count;
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    data[0] = COMMAND_CLASS_ASSOCIATION;
    data[1] = ASSOCIATION_REMOVE;
    data[2] = group_id;
    if( node_count > 0 )
        memcpy( &data[3], nodes, node_count );

    /// TODO: check channel support
    znet_error_t result = znet_fc_send_data( node_id, channel_id, data_len, data, Callback, callbackArg, encap_type );
    Heap_Free( data );

    return result;
}

//----------------------------------------------------------------------------
// Send Association Groupings Get
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_grouping_get( uint8_t node_id,
                                                                 uint8_t channel_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_ASSOCIATION, ASSOCIATION_GROUPINGS_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send Association Specific Group Get
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: group_id
// Group Id (from 1 to 255)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_specific_group_get( uint8_t node_id,
                                                                       uint8_t channel_id,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_ASSOCIATION, ASSOCIATION_SPECIFIC_GROUP_GET_V2 };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Association Receive
void ICACHE_FLASH_ATTR znet_cc_association_receive( const ZFunction func,
                                                    uint8_t node_id,
                                                    int cc_data_len,
                                                    const uint8_t* cc_data ) {
    Log_Write( "znet_cc_association_receive \r\n" );
    uint8_t group_id;
    if( ( cc_data[2] == 0 ) || ( max_association_groups < cc_data[2] ) )
        group_id = 0;
    else
        group_id = cc_data[2] - 1;

    if( cc_data[1] == ASSOCIATION_SET_V2 ) {
        supervision_status = SUPERVISION_REPORT_SUCCESS;
        if( max_association_groups >= cc_data[2] ) {
            for( uint8_t i = 3; i < cc_data_len; i++ ) {
                for( uint8_t j = 0; j < ZW_MAX_NODES; j++ ) {
                    if( ZW_MAX_NODES < cc_data[i] ) {
                        supervision_status = SUPERVISION_REPORT_FAIL;
                        break;
                    }
                    if( association_grouping_id[group_id][j] == cc_data[i] )
                        break;
                    if( association_grouping_id[group_id][j] == 0 ) {
                        association_grouping_id[group_id][j] = cc_data[i];
                        break;
                    }
                }
            }
            if( cc_data_len - 3 > ZW_MAX_NODES ) {
                supervision_status = SUPERVISION_REPORT_FAIL;
            }
        }
        else
            supervision_status = SUPERVISION_REPORT_FAIL;

        znet_store_associations_set( group_id, association_grouping_id[group_id] );

        learn_ok( node_id, false );
    }
    if( cc_data[1] == ASSOCIATION_GET_V2 ) {
        uint8_t data[ZW_MAX_NODES + 5];
        data[0] = COMMAND_CLASS_ASSOCIATION_V2;
        data[1] = ASSOCIATION_REPORT_V2;
        data[3] = ZW_MAX_NODES;
        data[4] = 0;
        uint8_t size = 5;
        data[2] = group_id + 1;
        for( uint8_t i = 0; i < ZW_MAX_NODES; i++ ) {
            if( association_grouping_id[group_id][i] != 0 ) {
                data[5 + i] = association_grouping_id[group_id][i];
                size++;
            }
        }
        if( size <= sizeof( data ) ) {
            znet_fc_send_data( node_id, func->_endpoint, size, data, NULL, NULL, func->_security );
        }
    }
    if( cc_data[1] == ASSOCIATION_GROUPINGS_GET_V2 ) {
        ZW_ASSOCIATION_GROUPINGS_REPORT_V2_FRAME data;
        data.cmdClass = COMMAND_CLASS_ASSOCIATION_V2;
        data.cmd = ASSOCIATION_GROUPINGS_REPORT_V2;
        data.supportedGroupings = 1;
        znet_fc_send_data( node_id, func->_endpoint, sizeof data, (uint8_t*)&data, NULL, NULL, func->_security );
    }
    if( cc_data[1] == ASSOCIATION_SPECIFIC_GROUP_GET_V2 ) {
        ZW_ASSOCIATION_SPECIFIC_GROUP_REPORT_V2_FRAME data;
        data.cmdClass = COMMAND_CLASS_ASSOCIATION_V2;
        data.cmd = ASSOCIATION_SPECIFIC_GROUP_REPORT_V2;
        data.group = 0;
        znet_fc_send_data( node_id, func->_endpoint, sizeof data, (uint8_t*)&data, NULL, NULL, func->_security );
    }
    if( cc_data[1] == ASSOCIATION_REMOVE_V2 ) {
        Log_Write( "ASSOCIATION_REMOVE_V2\r\n" );
        print_hex_buf( cc_data, cc_data_len );
        supervision_status = SUPERVISION_REPORT_SUCCESS;
        if( cc_data[2] == 0 ) {
            if( cc_data_len == 3 ) {
                // Number of NodeIDs = 0
                for( uint8_t j = 0; j < max_association_groups; j++ )
                    for( uint8_t i = 0; i < ZW_MAX_NODES; i++ )
                        association_grouping_id[j][i] = 0;
            }
            else {
                for( uint8_t j = 0; j < max_association_groups; j++ )
                    for( uint8_t i = 0; i < ZW_MAX_NODES; i++ )
                        if( association_grouping_id[j][i] == cc_data[3] ) {
                            for( uint8_t k = i; k < ( ZW_MAX_NODES - 1 ); k++ )
                                association_grouping_id[j][k] = association_grouping_id[j][k + 1];
                            break;
                        }
            }
        }
        else {
            if( cc_data_len == 3 ) {
                for( uint8_t i = 0; i < ZW_MAX_NODES; i++ )
                    association_grouping_id[cc_data[2] - 1][i] = 0;
            }
            else {
                for( uint8_t i = 3; i < cc_data_len; i++ )
                    for( uint8_t j = 0; j < ZW_MAX_NODES; j++ ) {
                        if( association_grouping_id[cc_data[2] - 1][j] == cc_data[i] ) {
                            for( uint8_t k = j; k < ( ZW_MAX_NODES - 1 ); k++ )
                                association_grouping_id[cc_data[2] - 1][k] =
                                    association_grouping_id[cc_data[2] - 1][k + 1];
                            association_grouping_id[max_association_groups - 1][ZW_MAX_NODES - 1] = 0;
                            break;
                        }
                    }
            }
        }

        znet_store_associations_set( group_id, association_grouping_id[group_id] );
    }
    Log_Write( "supervision_status = %d \r\n", supervision_status );
}

void ICACHE_FLASH_ATTR association_reset( void ) {
    for( uint8_t j = 0; j < max_association_groups; ++j ) {
        memset( association_grouping_id[j], 0, ZW_MAX_NODES );

        znet_store_associations_set( j, association_grouping_id[j] );
    }
}

void ICACHE_FLASH_ATTR association_remove_node( uint8_t node_id ) {
    for( uint8_t j = 0; j < max_association_groups; j++ ) {
        for( uint8_t i = 0; i < ZW_MAX_NODES; i++ )
            if( association_grouping_id[j][i] == node_id ) {
                for( uint8_t k = i; k < ( ZW_MAX_NODES - 1 ); k++ )
                    if( ( association_grouping_id[j][k] = association_grouping_id[j][k + 1] ) == 0 )
                        break;
                break;
            }

        znet_store_associations_set( j, association_grouping_id[j] );
    }
}

size_t ICACHE_FLASH_ATTR association_get_nodes_in_lifeline_group( const uint8_t** nodes ) {
    if( nodes )
        *nodes = association_grouping_id[0];

    size_t i = 0;
    for( ; i != ZW_MAX_NODES; ++i ) {
        if( association_grouping_id[0][i] == 0 )
            break;
    }

    return i;
}

void ICACHE_FLASH_ATTR associations_load( void ) {
    for( uint8_t j = 0; j < max_association_groups; ++j ) {
        znet_store_associations_get( j, association_grouping_id[j] );
    }
}
