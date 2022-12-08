
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "byte_ops.h"

#include "znet.h"

#include "ZPlatform.h"
#include "znet_lib_cc.h"
#include "znet_lib_fc.h"

extern void indicator_identify();

//----------------------------------------------------------------------------
// Indicator Set Receive
void ICACHE_FLASH_ATTR znet_cc_indicator_set_receive( const ZFunction func,
                                                      uint8_t node_id,
                                                      int cc_data_len,
                                                      const uint8_t* cc_data ) {
    if( cc_data_len != 13 ) {
        return;
    }

    if( cc_data[0] != COMMAND_CLASS_INDICATOR || cc_data[1] != INDICATOR_SET_V2 ) {
        return;
    }

    bool isIdentifyReceived = cc_data[2] == 0x00 && cc_data[4] == 0x50 && cc_data[5] == 0x03 && cc_data[6] == 0x08 &&
                              cc_data[7] == 0x50 && cc_data[8] == 0x04 && cc_data[9] == 0x03 && cc_data[10] == 0x50 &&
                              cc_data[11] == 0x05 && cc_data[12] == 0x06;

    if( isIdentifyReceived ) {
        indicator_identify();
    }
}

//----------------------------------------------------------------------------
// Indicator Get Receive
void ICACHE_FLASH_ATTR znet_cc_indicator_get_receive( const ZFunction func,
                                                      uint8_t node_id,
                                                      int cc_data_len,
                                                      const uint8_t* cc_data ) {
    if( cc_data[0] != COMMAND_CLASS_INDICATOR || cc_data[1] != INDICATOR_GET_V2 ) {
        return;
    }

    uint8_t data[7];
    data[0] = COMMAND_CLASS_INDICATOR;
    data[1] = INDICATOR_REPORT_V2;
    data[2] = 0x00;
    data[3] = 0x01;
    data[4] = cc_data[2] == 0x50 ? 0x50 : 0x00;
    data[5] = 0x00;
    data[6] = 0x00;

    znet_fc_send_data( node_id, func->_endpoint, 7, data, NULL, NULL, func->_security );
}

//----------------------------------------------------------------------------
// Indicator Get Receive
void ICACHE_FLASH_ATTR znet_cc_indicator_supported_get_receive( const ZFunction func,
                                                                uint8_t node_id,
                                                                int cc_data_len,
                                                                const uint8_t* cc_data ) {
    if( cc_data[0] != COMMAND_CLASS_INDICATOR || cc_data[1] != INDICATOR_SUPPORTED_GET_V2 ) {
        return;
    }

    uint8_t indicatorId = ( cc_data[2] == 0x50 || cc_data[2] == 0x00 ) ? 0x50 : 0x00;

    uint8_t data[6];
    data[0] = COMMAND_CLASS_INDICATOR;
    data[1] = INDICATOR_SUPPORTED_REPORT_V2;
    data[2] = indicatorId;
    data[3] = 0x00;
    data[4] = indicatorId == 0x50 ? 0x01 : 0x00;
    data[5] = indicatorId == 0x50 ? 0x38 : 0x00;

    znet_fc_send_data( node_id, func->_endpoint, indicatorId == 0x50 ? 6 : 5, data, NULL, NULL, func->_security );
}

//----------------------------------------------------------------------------
// Indicator Get Command
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
// @param: indicator_id
// Indicator Id
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_indicator_get( uint8_t node_id,
                                                      uint8_t channel_id,
                                                      uint8_t id,
                                                      uint8_t version,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg,
                                                      encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_INDICATOR, INDICATOR_GET, id };

    return znet_fc_send_data(
        node_id, channel_id, sizeof data - ( version <= 1 ), data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Indicator Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//

// @param: id
// Indicator ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_indicator_supported_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t id,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_INDICATOR, INDICATOR_SUPPORTED_GET_V2, id };

    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Indicator Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: object_count
// Indicator Object Count (5 bits)
//
// @param: object
// Indicator Object
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_indicator_set( uint8_t node_id,
                                                      uint8_t channel_id,
                                                      uint8_t value,
                                                      uint8_t object_count,
                                                      const uint8_t* object,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg,
                                                      encap_type_t encap_type ) {
    uint8_t data_len = 4 + ( sizeof( uint8_t ) * 3 * object_count );
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    data[0] = COMMAND_CLASS_INDICATOR;
    data[1] = INDICATOR_SET;
    data[2] = value;
    data[3] = object_count;
    if( object_count > 0 ) {
        memcpy( &data[4], object, sizeof( uint8_t ) * 3 * object_count );
    }

    znet_error_t result = znet_fc_send_data( node_id, channel_id, data_len, data, Callback, callbackArg, encap_type );
    Heap_Free( data );

    return result;
}
