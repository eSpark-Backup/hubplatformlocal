
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
#include "ZW_transport_api.h"

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Device Reset Locally Notification Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_device_reset_localy_notification( uint8_t node_id,
                                                                         uint8_t channel_id,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_DEVICE_RESET_LOCALLY, DEVICE_RESET_LOCALLY_NOTIFICATION };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Device Reset Locally Notification Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
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
void ICACHE_FLASH_ATTR znet_cc_device_reset_localy_report_lib( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data ) {
    if( ( node_id != 0 ) && ( node_id <= ZW_MAX_NODES ) ) {
        supervision_status = SUPERVISION_REPORT_SUCCESS;
        znet_cc_device_reset_localy_report( func, node_id, cc_data_len, cc_data );
    }
    else {
        supervision_status = SUPERVISION_REPORT_FAIL;
    }
}
