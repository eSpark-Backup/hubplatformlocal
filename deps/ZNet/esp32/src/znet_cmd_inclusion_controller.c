/**
 * @file znet_cmd_inclusion_controller.c
 * @date 15 May 2020
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
#include <znet_lib_cc_network.h>

// node cmd inclusion_initiate /////////////////////////////////////////////
/// INFO: Inclusion Controller Command Class v1

znet_error_t znet_node_inclusion_initiate( inclusion_controller_info_t ic_info,
                                           znet_node_id_t node_number,
                                           znet_inclution_controller_step_id_t step_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    uint8_t node_id = ic_info.id;

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( step_id < ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION ||
        step_id > ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION_REPLACE ) {
        return ZNET_ERR_INVAL;
    }

    return znet_cc_inclusion_initiate( node_id, node_number, step_id, NULL, NULL, ic_info.security );
}

znet_error_t znet_node_inclusion_complete( inclusion_controller_info_t ic_info,
                                           znet_inclution_controller_step_id_t step_id,
                                           znet_inclution_controller_status_t status ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }
    uint8_t node_id = ic_info.id;
    encap_type_t encap = ic_info.security;

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( step_id < ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION ||
        step_id > ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION_REPLACE ) {
        return ZNET_ERR_INVAL;
    }
    znet_set_ic_info( ZNET_NODE_ID_ANY, Encapsulation_None );

    return znet_cc_inclusion_complete( node_id, step_id, status, NULL, NULL, encap );
}
