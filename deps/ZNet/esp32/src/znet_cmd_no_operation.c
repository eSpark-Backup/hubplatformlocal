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

// node cmd no operation //////////////////////////////////////////////////
/// INFO: No Operation Command Class v1

static void _znet_node_cmd_no_operation_cb( ZFunction func,
                                            void* arg,
                                            ZFuncFailures_e reason,
                                            const void* dataPtr,
                                            size_t dataSize ) {
    if( znet_cb->node_cmd_no_operation_result ) {
        uint8_t nodeId;
        ZNet_CheckCallback( func, reason, &nodeId, NULL );

        znet_cb->node_cmd_no_operation_result( znet_get_error( reason ), nodeId, arg );
    }
}

znet_error_t znet_node_cmd_no_operation( znet_node_id_t node_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    return znet_cc_no_operation( node_id, _znet_node_cmd_no_operation_cb, znet_cb->arg );
}

////////////////////////////////////////////////////////////////////////
