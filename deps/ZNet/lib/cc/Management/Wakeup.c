
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "byte_ops.h"

//#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"
//#include "znet_fc_private.h"
#include "znet_private.h"  //verified by PM
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

//////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR znet_cc_wakeup_notification_lib( const ZFunction func,
                                                        uint8_t node_id,
                                                        int cc_data_len,
                                                        const uint8_t* cc_data ) {
#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
    ZNet_WakingNode( node_id, !0 );
    ZNet_ProcessWaitingCommands( node_id );
#endif
    znet_cc_wakeup_notification( func, node_id, cc_data_len, cc_data );
}

//----------------------------------------------------------------------------
// Send Wakeup Get
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_wakeup_get( uint8_t node_id,
                                                   uint8_t channel_id,
                                                   ZFunction_Callback Callback,
                                                   void* callbackArg,
                                                   encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_WAKE_UP, WAKE_UP_INTERVAL_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send Wakeup Set
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
// @param: interval
// Wakeup interval in seconds
//
// @param: notification_node_id
// Node Id to be notified about wakeup
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_wakeup_set( uint8_t node_id,
                                                   uint8_t channel_id,
                                                   uint32_t interval_sec,
                                                   uint8_t notification_node_id,
                                                   ZFunction_Callback Callback,
                                                   void* callbackArg,
                                                   encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_WAKE_UP, WAKE_UP_INTERVAL_SET, 1, 2, 3, notification_node_id };
    int24_to_bytes( interval_sec, &data[2] );
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send Wakeup NoMoreInformation (Sleep)
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_wakeup_sleep( uint8_t node_id,
                                                     uint8_t channel_id,
                                                     ZFunction_Callback Callback,
                                                     void* callbackArg,
                                                     encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_WAKE_UP, WAKE_UP_NO_MORE_INFORMATION };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Wake Up Notification Command
//
// @param: this
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

/*znet_error_t ICACHE_FLASH_ATTR znet_cc_wakeup_notification( uint8_t node_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg ) {
    uint8_t data[] = { COMMAND_CLASS_WAKE_UP, WAKE_UP_NOTIFICATION };
    return znet_fc_send_data( node_id, sizeof data, data, Callback, callbackArg, Encapsulation_None );
}*/

//----------------------------------------------------------------------------
// Send Wakeup CapabilityGet
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_wakeup_capabilities_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_WAKE_UP, WAKE_UP_INTERVAL_CAPABILITIES_GET_V2 };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Wake Up receive
//
void ICACHE_FLASH_ATTR znet_cc_wakeup_receive( const ZFunction func,
                                               uint8_t node_id,
                                               int UNUSED( cc_data_len ),
                                               const uint8_t* cc_data ) {
    if( WAKE_UP_INTERVAL_CAPABILITIES_GET_V2 == cc_data[1] ) {
        ZW_WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2_FRAME data;
        data.cmdClass = COMMAND_CLASS_WAKE_UP_V2;
        data.cmd = WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2;
        data.minimumWakeupIntervalSeconds1 = 0;
        data.minimumWakeupIntervalSeconds2 = 0;
        data.minimumWakeupIntervalSeconds3 = 0;
        data.maximumWakeupIntervalSeconds1 = 0;
        data.maximumWakeupIntervalSeconds2 = 0;
        data.maximumWakeupIntervalSeconds3 = 0;
        data.defaultWakeupIntervalSeconds1 = 0;
        data.defaultWakeupIntervalSeconds2 = 0;
        data.defaultWakeupIntervalSeconds3 = 0;
        data.wakeUpIntervalStepSeconds1 = 0;
        data.wakeUpIntervalStepSeconds2 = 0;
        data.wakeUpIntervalStepSeconds3 = 0;
        znet_fc_send_data( node_id, func->_endpoint, sizeof data, (uint8_t*)&data, NULL, NULL, func->_security );
    }
}
