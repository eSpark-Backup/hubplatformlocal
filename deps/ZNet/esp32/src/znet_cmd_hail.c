/**
 * @file znet_cmd_hail.c
 * @date 14 Oct 2018
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
#include <znet_lib_cc_application.h>

/// INFO: Hail Command Class v1

/*********************************************************************************************************************/

void znet_cc_hail_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 2 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_HAIL );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( znet_cb->node_cmd_hail_result )
        znet_cb->node_cmd_hail_result( ZNET_ERR_OK, node_id, znet_cb->arg );
}
