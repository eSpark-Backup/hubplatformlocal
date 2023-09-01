/// INFO: private
#include "znet_main.h"
#include "znet_log.h"

/*----------------------------------------------------------------------------*/

#define ZNET_CHECK_NODE( node_id )                                   \
    if( !znet_cb ) {                                                 \
        ZNET_LOGE( "Library not initialized!\n" );                   \
        return ZNET_ERR_FAULT;                                       \
    }                                                                \
    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) { \
        ZNET_LOGE( "Invalid node id!\n" );                           \
        return ZNET_ERR_INVAL;                                       \
    }

/*----------------------------------------------------------------------------*/
