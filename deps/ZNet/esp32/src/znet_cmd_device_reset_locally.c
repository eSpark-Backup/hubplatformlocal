#include "znet_main.h"
#include "znet_log.h"

#include <znet_lib_cc_management.h>

znet_error_t znet_node_cmd_device_reset_locally_notification( znet_node_id_t node_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_device_reset_localy_notification( node_id, ZNET_CHANNEL_ID_ROOT, NULL, NULL, encap );
}
