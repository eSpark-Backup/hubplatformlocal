#include "typedefs.h"

//#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"
//#include "znet_fc_private.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

static uint8_t basic_value = 0;
//----------------------------------------------------------------------------
// Command Class Basic //

// Send Basic Get
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_basic_get( uint8_t node_id,
                                                  uint8_t channel_id,
                                                  ZFunction_Callback Callback,
                                                  void* callbackArg,
                                                  encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_BASIC, BASIC_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send Basic Set
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: value
// Value
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_basic_set( uint8_t node_id,
                                                  uint8_t channel_id,
                                                  uint8_t value,
                                                  ZFunction_Callback Callback,
                                                  void* callbackArg,
                                                  encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_BASIC, BASIC_SET, value };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Basic Receive
//
void ICACHE_FLASH_ATTR znet_cc_basic_receive( const ZFunction func,
                                              uint8_t node_id,
                                              int cc_data_len,
                                              const uint8_t* cc_data ) {
    if( cc_data[1] == BASIC_GET ) {
        /*
         //TODO add validation for the correctness of the data, they must be
         //either 0 or 0xFF
         ZW_BASIC_REPORT_FRAME data; data.cmdClass =
         COMMAND_CLASS_BASIC; data.cmd = BASIC_REPORT; data.value = basic_value;
                znet_fc_send_data( node_id, sizeof data,
           (uint8_t*)&data, NULL, NULL, func->_security );
        */
    }
    if( cc_data[1] == BASIC_SET ) {
        basic_value = cc_data[2];
        znet_cc_basic_report( func, node_id, cc_data_len, cc_data );
    }
}
