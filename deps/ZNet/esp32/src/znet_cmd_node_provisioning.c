/**
 * @file znet_cmd_no_operation.c
 * @date 20 June 2018
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
#include <znet_lib_cc_network.h>

// node cmd node provisioning //////////////////////////////////////////////////
//----------------------------------------------------------------------------
// Node Provisioning Set receive  Command
void ICACHE_FLASH_ATTR znet_cc_node_provisioning_set_received( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data ) {
    if( cc_data[0] != COMMAND_CLASS_NODE_PROVISIONING || cc_data[1] != NODE_PROVISION_SET ) {
        return;
    }

    if( ( cc_data[3] & NODE_PROVISION_SET_PROPERTIES1_DSK_LENGTH_MASK ) != 16 ) {
        return;
    }

    if( cc_data_len < 20 ) {
        return;
    }

    znet_node_provisioning_set_t value;
    value.sequence_number = cc_data[2];
    memcpy( value.dsk_key, cc_data + 4, 16 );
    value.meta_data_length = cc_data_len - 20;
    value.meta_data = cc_data + 20;

    if( znet_cb->node_cmd_node_provisioning_set )
        znet_cb->node_cmd_node_provisioning_set( ZNET_ERR_OK, node_id, ZNET_CHANNEL_ID_ROOT, &value, znet_cb->arg );
}

//----------------------------------------------------------------------------
// Node Provisioning Delete receive  Command
void ICACHE_FLASH_ATTR znet_cc_node_provisioning_delete_received( const ZFunction func,
                                                                  uint8_t node_id,
                                                                  int cc_data_len,
                                                                  const uint8_t* cc_data ) {
    if( cc_data[0] != COMMAND_CLASS_NODE_PROVISIONING || cc_data[1] != NODE_PROVISION_DELETE ) {
        return;
    }

    znet_node_provisioning_delete_t value;
    value.sequence_number = cc_data[2];

    uint8_t dsk_length = cc_data[3] & NODE_PROVISION_DELETE_PROPERTIES1_DSK_LENGTH_MASK;
    if( dsk_length == 0 ) {
        value.delete_all = true;
    }
    else if( dsk_length == 16 && cc_data_len == 20 ) {
        value.delete_all = false;
        memcpy( value.dsk_key, cc_data + 4, 16 );
    }
    else {
        return;
    }

    if( znet_cb->node_cmd_node_provisioning_delete )
        znet_cb->node_cmd_node_provisioning_delete( ZNET_ERR_OK, node_id, ZNET_CHANNEL_ID_ROOT, &value, znet_cb->arg );
}

//----------------------------------------------------------------------------
// Node Provisioning List Iteration Get receive  Command
void ICACHE_FLASH_ATTR znet_cc_node_provisioning_list_iteration_get_received( const ZFunction func,
                                                                              uint8_t node_id,
                                                                              int cc_data_len,
                                                                              const uint8_t* cc_data ) {
    if( cc_data[0] != COMMAND_CLASS_NODE_PROVISIONING || cc_data[1] != NODE_PROVISIONING_LIST_ITERATION_GET ) {
        return;
    }

    if( cc_data_len != 4 ) {
        return;
    }

    uint8_t sequence_number = cc_data[2];
    uint8_t remaining_counter = cc_data[3];

    if( znet_cb->node_cmd_node_provisioning_list_iteration_get )
        znet_cb->node_cmd_node_provisioning_list_iteration_get(
            ZNET_ERR_OK, node_id, ZNET_CHANNEL_ID_ROOT, sequence_number, remaining_counter, znet_cb->arg );
}

//----------------------------------------------------------------------------
// Node Provisioning Get receive  Command
void ICACHE_FLASH_ATTR znet_cc_node_provisioning_get_received( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data ) {
    if( cc_data[0] != COMMAND_CLASS_NODE_PROVISIONING || cc_data[1] != NODE_PROVISION_GET ) {
        return;
    }

    if( cc_data_len != 20 ) {
        return;
    }

    uint8_t sequence_number = cc_data[2];
    uint8_t dsk_length = cc_data[3] & NODE_PROVISION_GET_PROPERTIES1_DSK_LENGTH_MASK;
    if( dsk_length != 16 ) {
        return;
    }

    uint8_t dsk_key[16];
    memcpy( dsk_key, cc_data + 4, 16 );

    if( znet_cb->node_cmd_node_provisioning_get )
        znet_cb->node_cmd_node_provisioning_get(
            ZNET_ERR_OK, node_id, ZNET_CHANNEL_ID_ROOT, sequence_number, dsk_key, znet_cb->arg );
}

//----------------------------------------------------------------------------
// Node Provisioning Report Command
znet_error_t znet_node_cmd_node_provisioning_report( znet_node_id_t node_id,
                                                     znet_node_channel_id_t channel_id,
                                                     uint8_t sequence_number,
                                                     uint8_t* dsk,
                                                     uint8_t* meta_data,
                                                     uint8_t metadata_length ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_node_provisioning_report( node_id,
                                             channel_id,
                                             sequence_number,
                                             dsk,
                                             dsk ? 16 : 0,
                                             meta_data,
                                             metadata_length,
                                             NULL,
                                             znet_cb->arg,
                                             encap );
}

//----------------------------------------------------------------------------
// Node Provisioning List Iteration Report Command
znet_error_t znet_node_cmd_node_provisioning_list_iteration_report( znet_node_id_t node_id,
                                                                    znet_node_channel_id_t channel_id,
                                                                    uint8_t sequence_number,
                                                                    uint8_t remaining_count,
                                                                    uint8_t* dsk,
                                                                    uint8_t* meta_data,
                                                                    uint8_t metadata_length ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_node_provisioning_list_iteration_report( node_id,
                                                            channel_id,
                                                            sequence_number,
                                                            remaining_count,
                                                            dsk,
                                                            dsk ? 16 : 0,
                                                            meta_data,
                                                            metadata_length,
                                                            NULL,
                                                            znet_cb->arg,
                                                            encap );
}
