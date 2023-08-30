
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "byte_ops.h"

#include "heap.h"

//#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"
//#include "znet_fc_private.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"
#include "znet_store.h"
#include "logger.h"
#include "ZW_transport_api.h"
#if( !defined( ESP_PLATFORM ) && !defined( __linux__ ) && !defined( __APPLE__ ) )
#include "config_app.h"
#endif

#define MAX_GROUPS_SUPPORTED 1
#define MAX_NODES_SUPPORTED 5

const uint8_t max_groups_supported = MAX_GROUPS_SUPPORTED;
const uint8_t max_nodes_supported = MAX_NODES_SUPPORTED;

extern uint8_t association_grouping_id[][ZW_MAX_NODES];

VG_MULTI_CHANNEL_ASSOCIATION_SET_V2_VG
multi_ch_association_id_vg[MAX_GROUPS_SUPPORTED][MAX_NODES_SUPPORTED] = {};

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Send MultiChannelAssociation Get
//
// @param: this
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_association_get( uint8_t node_id,
                                                                     uint8_t channel_id,
                                                                     uint8_t group_id,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg,
                                                                     encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3, MULTI_CHANNEL_ASSOCIATION_GET_V3, group_id };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR SendMultichannelAssociationCommand( uint8_t command,
                                                                          uint8_t channel_id,
                                                                          uint8_t node_id,
                                                                          uint8_t group_id,
                                                                          uint8_t nodes_count,
                                                                          const uint8_t* nodes,
                                                                          uint8_t multiChannelNodes_count,
                                                                          const uint16_t* multiChannelNodes,
                                                                          ZFunction_Callback Callback,
                                                                          void* callbackArg,
                                                                          encap_type_t encap_type ) {
    int nodes_size = nodes_count * sizeof( *nodes );
    int multiChannelNodes_size = multiChannelNodes_count * sizeof( *multiChannelNodes );

    //                             cc_id  c_id  group_id               MARKER
    uint8_t data_size =
        ( uint8_t )( 1 + 1 + 1 + nodes_size + ( ( multiChannelNodes_size > 0 ) ? 1 : 0 ) + multiChannelNodes_size );
    uint8_t* data = (uint8_t*)Heap_Alloc( data_size );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    data[0] = COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3;
    data[1] = command;
    data[2] = group_id;
    int data_len = 3;
    if( nodes_size > 0 ) {
        memcpy( &data[3], nodes, nodes_size );
        data_len += nodes_size;
    }
    if( multiChannelNodes_size > 0 ) {
        data[data_len] = 0x00;  /// MARKER
        memcpy( &data[data_len + 1], multiChannelNodes, multiChannelNodes_size );
    }

    /// TODO: check channel support
    znet_error_t result = znet_fc_send_data( node_id, channel_id, data_size, data, Callback, callbackArg, encap_type );

    Heap_Free( data );

    return result;
}

//----------------------------------------------------------------------------
// Send MultiChannelAssociation Set (Add)
//
// @param: this
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_association_set( uint8_t node_id,
                                                                     uint8_t channel_id,
                                                                     uint8_t group_id,
                                                                     uint8_t nodes_count,
                                                                     const uint8_t* nodes,
                                                                     uint8_t multiChannelNodes_count,
                                                                     const uint16_t* multiChannelNodes,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg,
                                                                     encap_type_t encap_type ) {
    return SendMultichannelAssociationCommand( MULTI_CHANNEL_ASSOCIATION_SET_V3,
                                               channel_id,
                                               node_id,
                                               group_id,
                                               nodes_count,
                                               nodes,
                                               multiChannelNodes_count,
                                               multiChannelNodes,
                                               Callback,
                                               callbackArg,
                                               encap_type );
}

//----------------------------------------------------------------------------
// Send MultiChannelAssociation Remove
//
// @param: this
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_association_remove( uint8_t node_id,
                                                                        uint8_t channel_id,
                                                                        uint8_t group_id,
                                                                        uint8_t nodes_count,
                                                                        const uint8_t* nodes,
                                                                        uint8_t multiChannelNodes_count,
                                                                        const uint16_t* multiChannelNodes,
                                                                        ZFunction_Callback Callback,
                                                                        void* callbackArg,
                                                                        encap_type_t encap_type ) {
    return SendMultichannelAssociationCommand( MULTI_CHANNEL_ASSOCIATION_REMOVE_V3,
                                               channel_id,
                                               node_id,
                                               group_id,
                                               nodes_count,
                                               nodes,
                                               multiChannelNodes_count,
                                               multiChannelNodes,
                                               Callback,
                                               callbackArg,
                                               encap_type );
}

//----------------------------------------------------------------------------
// Send MultiChannelAssociation GroupingsGet
//
// @param: this
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_association_groupings_get( uint8_t node_id,
                                                                               uint8_t channel_id,
                                                                               ZFunction_Callback Callback,
                                                                               void* callbackArg,
                                                                               encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3, MULTI_CHANNEL_ASSOCIATION_GROUPINGS_GET_V3 };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR add_node_multich_association( uint8_t group_id, uint8_t node_id ) {
    Log_Write( "add_node_multich_association_vg\r\n" );
    uint8_t j;
    for( j = 0; j < max_nodes_supported; j++ ) {
        if( association_grouping_id[group_id][j] == 0 )
            break;
        if( association_grouping_id[group_id][j] == node_id )
            return true;
    }
    if( j == max_nodes_supported )
        return false;
    association_grouping_id[group_id][j] = node_id;
    Log_Write( " association_grouping_id  j = %d, node_id= %d \r\n", j, association_grouping_id[group_id][j] );

    znet_store_associations_set( group_id, association_grouping_id[group_id] );
    return true;
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR add_node_multich_association_vg( uint8_t group_id, uint8_t node_id, uint8_t properties ) {
    Log_Write( "add_node_multich_association_vg\r\n" );
    uint8_t i;
    for( i = 0; i < max_nodes_supported; i++ ) {
        if( multi_ch_association_id_vg[group_id][i].multiChannelNodeId == 0 )
            break;
        if( ( multi_ch_association_id_vg[group_id][i].multiChannelNodeId == node_id ) &&
            ( multi_ch_association_id_vg[group_id][i].properties1 == properties ) )
            return true;
    }
    if( i == max_nodes_supported )
        return false;
    multi_ch_association_id_vg[group_id][i].multiChannelNodeId = node_id;
    multi_ch_association_id_vg[group_id][i].properties1 = properties;

    znet_store_multichannel_associations_set( group_id, multi_ch_association_id_vg[group_id] );
    return true;
}
//----------------------------------------------------------------------------
uint8_t ICACHE_FLASH_ATTR get_destinations_number( uint8_t group_id ) {
    Log_Write( "get_destination_number\r\n" );
    uint8_t number = 0;
    for( uint8_t i = 0; i < max_nodes_supported; i++ ) {
        if( association_grouping_id[group_id][i] != 0 )
            number++;
        if( multi_ch_association_id_vg[group_id][i].multiChannelNodeId != 0 )
            number++;
    }
    return number;
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR clear_node_multich_association( uint8_t group_id ) {
    Log_Write( "clear_node_multich_association group_id = %d\r\n", group_id );
    uint8_t j;
    for( j = 0; j < max_nodes_supported; j++ ) {
        association_grouping_id[group_id][j] = 0;
        multi_ch_association_id_vg[group_id][j].multiChannelNodeId = 0;
        multi_ch_association_id_vg[group_id][j].properties1 = 0;
    }

    znet_store_associations_set( group_id, association_grouping_id[group_id] );
    znet_store_multichannel_associations_set( group_id, multi_ch_association_id_vg[group_id] );
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR remove_node_multich_association( uint8_t group_id, uint8_t node_id ) {
    Log_Write( "remove_node_multich_association\r\n" );
    uint8_t j;
    uint8_t remove_node = 0;
    for( j = 0; j < ZW_MAX_NODES - 1; j++ ) {
        if( association_grouping_id[group_id][j] == node_id )
            remove_node = 1;
        if( remove_node )
            association_grouping_id[group_id][j] = association_grouping_id[group_id][j + 1];
    }
    if( ( association_grouping_id[group_id][j] == node_id ) || ( remove_node ) )
        association_grouping_id[group_id][j] = 0;
    print_hex_buf( &association_grouping_id[group_id][0], max_nodes_supported );

    znet_store_associations_set( group_id, association_grouping_id[group_id] );
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR remove_node_multich_association_vg( uint8_t group_id, uint8_t node_id, uint8_t properties ) {
    Log_Write( "remove_node_multich_association_vg  node_id = %d, properties = %d\r\n", node_id, properties );
    uint8_t j;
    uint8_t remove_node = 0;
    for( j = 0; j < max_nodes_supported - 1; j++ ) {
        if( ( multi_ch_association_id_vg[group_id][j].multiChannelNodeId == node_id ) &&
            ( multi_ch_association_id_vg[group_id][j].properties1 == properties ) )
            remove_node = 1;
        if( remove_node ) {
            multi_ch_association_id_vg[group_id][j].multiChannelNodeId =
                multi_ch_association_id_vg[group_id][j + 1].multiChannelNodeId;
            multi_ch_association_id_vg[group_id][j].properties1 =
                multi_ch_association_id_vg[group_id][j + 1].properties1;
        }
    }
    if( ( multi_ch_association_id_vg[group_id][j].multiChannelNodeId == node_id ) || ( remove_node ) ) {
        multi_ch_association_id_vg[group_id][j].multiChannelNodeId = 0;
        multi_ch_association_id_vg[group_id][j].properties1 = 0;
    }

    znet_store_multichannel_associations_set( group_id, multi_ch_association_id_vg[group_id] );
}

//----------------------------------------------------------------------------
// Multi Channel Association Receive
//
void ICACHE_FLASH_ATTR znet_cc_multichannel_association_receive( const ZFunction func,
                                                                 uint8_t node_id,
                                                                 int cc_data_len,
                                                                 const uint8_t* cc_data ) {
    uint8_t group_id;
    uint8_t i, j;
    Log_Write( "znet_cc_multichannel_association_receive   cc_data_len = %d\r\n", cc_data_len );
    print_hex_buf( cc_data, cc_data_len );
    if( ( cc_data[2] == 0 ) || ( max_groups_supported < cc_data[2] ) )
        group_id = 0;
    else
        group_id = cc_data[2] - 1;

    if( MULTI_CHANNEL_ASSOCIATION_GET_V3 == cc_data[1] ) {
        //        if( max_groups_supported < cc_data[2] )
        //            return;
        uint8_t data[25];
        data[0] = COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3;
        data[1] = MULTI_CHANNEL_ASSOCIATION_REPORT_V3;
        data[2] = group_id + 1;
        data[3] = max_nodes_supported;
        data[4] = 0;
        i = 5;
        for( j = 0; j < max_nodes_supported; j++ ) {
            if( association_grouping_id[group_id][j] != 0 )
                data[i++] = association_grouping_id[group_id][j];
            else
                break;
        }
        data[i++] = MULTI_CHANNEL_ASSOCIATION_SET_MARKER_V3;
        for( j = 0; j < max_nodes_supported; j++ ) {
            if( multi_ch_association_id_vg[group_id][j].multiChannelNodeId != 0 ) {
                data[i++] = multi_ch_association_id_vg[group_id][j].multiChannelNodeId;
                data[i++] = multi_ch_association_id_vg[group_id][j].properties1;
            }
            else
                break;
        }
        if( i == 6 )  // do not add MULTI_CHANNEL_ASSOCIATION_SET_MARKER_V3 if we don`t have nodes and vg
            i--;
        if( i <= sizeof( data ) ) {
            znet_fc_send_data( node_id, func->_endpoint, i, data, NULL, NULL, func->_security );
        }
    }
    if( MULTI_CHANNEL_ASSOCIATION_SET_V3 == cc_data[1] ) {
        Log_Write( "MULTI_CHANNEL_ASSOCIATION_SET_V3 group_id = %d \r\n", group_id );
        supervision_status = SUPERVISION_REPORT_SUCCESS;
        if( max_groups_supported < cc_data[2] ) {
            supervision_status = SUPERVISION_REPORT_FAIL;
            return;
        }
        if( get_destinations_number( group_id ) >= max_nodes_supported ) {
            supervision_status = SUPERVISION_REPORT_FAIL;
            return;
        }
        for( i = 3; i < cc_data_len; i++ ) {
            if( MULTI_CHANNEL_ASSOCIATION_REPORT_MARKER_V3 == cc_data[i] )
                break;
            if( !add_node_multich_association( group_id, cc_data[i] ) ) {
                supervision_status = SUPERVISION_REPORT_FAIL;
                return;
            }
        }
        i++;
        for( ; i < cc_data_len; i += 2 ) {
            if( !add_node_multich_association_vg( group_id, cc_data[i], cc_data[i + 1] ) ) {
                supervision_status = SUPERVISION_REPORT_FAIL;
                return;
            }
            Log_Write( " multi_ch_association_id_vg  i = %d, node_id= %x, prop "
                       "= %d \r\n",
                       i,
                       multi_ch_association_id_vg[group_id][i].multiChannelNodeId,
                       multi_ch_association_id_vg[group_id][i].properties1 );
        }
        learn_ok( node_id, true );
    }
    if( MULTI_CHANNEL_ASSOCIATION_REMOVE_V3 == cc_data[1] ) {
        supervision_status = SUPERVISION_REPORT_SUCCESS;
        Log_Write( "MULTI_CHANNEL_ASSOCIATION_REMOVE \r\n" );
        if( cc_data_len == 2 ) {
            for( j = 0; j < max_groups_supported; j++ ) {
                clear_node_multich_association( j );
            }
            return;
        }
        if( max_groups_supported < cc_data[2] )
            return;
        if( cc_data_len == 3 ) {
            if( cc_data[2] == 0 )
                for( j = 0; j < max_groups_supported; j++ )
                    clear_node_multich_association( j );
            else
                clear_node_multich_association( group_id );
        }
        else {
            if( ( cc_data[2] == 0 ) && ( cc_data_len == 4 ) ) {
                if( cc_data[3] == MULTI_CHANNEL_ASSOCIATION_REPORT_MARKER_V3 ) {
                    for( j = 0; j < max_groups_supported; j++ ) {
                        clear_node_multich_association( j );
                    }
                    return;
                }
                for( uint8_t i = 0; i < max_groups_supported; i++ )
                    for( j = 3; j < cc_data_len; j++ ) {
                        if( MULTI_CHANNEL_ASSOCIATION_REPORT_MARKER_V3 == cc_data[j] )
                            break;
                        else
                            remove_node_multich_association( i, cc_data[j] );
                    }
            }
            else {
                if( cc_data[3] == MULTI_CHANNEL_ASSOCIATION_REPORT_MARKER_V3 && cc_data_len == 4 ) {
                    clear_node_multich_association( group_id );
                }
                else {
                    for( j = 3; j < cc_data_len; j++ ) {
                        if( MULTI_CHANNEL_ASSOCIATION_REPORT_MARKER_V3 == cc_data[j] )
                            break;
                        else
                            remove_node_multich_association( group_id, cc_data[j] );
                    }
                    j++;
                    if( j < cc_data_len ) {
                        for( ; j < cc_data_len; j += 2 ) {
                            remove_node_multich_association_vg( group_id, cc_data[j], cc_data[j + 1] );
                        }
                    }
                }
            }
        }
    }

    if( MULTI_CHANNEL_ASSOCIATION_GROUPINGS_GET_V3 == cc_data[1] ) {
        ZW_MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V3_FRAME data;
        data.cmdClass = COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3;
        data.cmd = MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V3;
        data.supportedGroupings = max_groups_supported;
        znet_fc_send_data( node_id, func->_endpoint, sizeof data, (uint8_t*)&data, NULL, NULL, func->_security );
    }
}

void ICACHE_FLASH_ATTR multichannel_associations_load( void ) {
    for( uint8_t j = 0; j < max_groups_supported; ++j ) {
        znet_store_multichannel_associations_get( j, multi_ch_association_id_vg[j] );
    }
}
