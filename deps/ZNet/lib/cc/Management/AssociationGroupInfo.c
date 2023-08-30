
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

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Association Group Name Get Command
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
// @param: id
// Grouping Identifier
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_group_name_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   uint8_t id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_ASSOCIATION_GRP_INFO, ASSOCIATION_GROUP_NAME_GET, id };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Association Group Info Get Command
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
// @param: list_mode
// List Mode (1 bits)
//
// @param: refresh_cache
// Refresh cache (1 bits)
//
// @param: id
// Grouping Identifier
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_group_info_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   bool list_mode,
                                                                   bool refresh_cache,
                                                                   uint8_t id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_ASSOCIATION_GRP_INFO,
                       ASSOCIATION_GROUP_INFO_GET,
                       ( refresh_cache ? 0x80 : 0x00 ) | ( list_mode ? 0x40 : 0x00 ),
                       id };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Association Group Command List Get Command
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
// @param: allow_cache
// Refresh cache (1 bits)
//
// @param: id
// Grouping Identifier
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_group_command_list_get( uint8_t node_id,
                                                                           uint8_t channel_id,
                                                                           bool allow_cache,
                                                                           uint8_t id,
                                                                           ZFunction_Callback Callback,
                                                                           void* callbackArg,
                                                                           encap_type_t encap_type ) {
    uint8_t data[] = {
        COMMAND_CLASS_ASSOCIATION_GRP_INFO, ASSOCIATION_GROUP_COMMAND_LIST_GET, ( allow_cache ? 0x80 : 0x00 ), id
    };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Association Group Info Receive
void ICACHE_FLASH_ATTR znet_cc_association_group_info_receive( const ZFunction func,
                                                               uint8_t node_id,
                                                               int UNUSED( cc_data_len ),
                                                               const uint8_t* cc_data ) {
    if( cc_data[1] == ASSOCIATION_GROUP_INFO_GET_V3 ) {
        ZW_ASSOCIATION_GROUP_INFO_REPORT_1BYTE_V3_FRAME data;
        data.cmdClass = COMMAND_CLASS_ASSOCIATION_GRP_INFO_V3;
        data.cmd = ASSOCIATION_GROUP_INFO_REPORT_V3;
        data.properties1 = 1 | ( ( cc_data[2] & 0x40 ) ? 0x80 : 0x00 );  // Group Count + Dynamic info + List mode;
        data.variantgroup1.groupingIdentifier = cc_data[3];
        data.variantgroup1.mode = 0;
        data.variantgroup1.profile1 = ASSOCIATION_GROUP_INFO_REPORT_PROFILE_GENERAL;
        data.variantgroup1.profile2 = ASSOCIATION_GROUP_INFO_REPORT_PROFILE_GENERAL_LIFELINE_V3;
        data.variantgroup1.reserved = 0;
        data.variantgroup1.eventCode1 = 0;
        data.variantgroup1.eventCode2 = 0;
        znet_fc_send_data( node_id, func->_endpoint, sizeof data, (uint8_t*)&data, NULL, NULL, func->_security );
    }
    if( cc_data[1] == ASSOCIATION_GROUP_NAME_GET_V3 ) {
        uint8_t data[] = { COMMAND_CLASS_ASSOCIATION_GRP_INFO_V3,
                           ASSOCIATION_GROUP_NAME_REPORT_V3,
                           cc_data[2],
                           8,
                           'L',
                           'i',
                           'f',
                           'e',
                           'l',
                           'i',
                           'n',
                           'e' };
        znet_fc_send_data( node_id, func->_endpoint, sizeof data, data, NULL, NULL, func->_security );
    }
    if( cc_data[1] == ASSOCIATION_GROUP_COMMAND_LIST_GET_V3 ) {
        ZW_ASSOCIATION_GROUP_COMMAND_LIST_REPORT_2BYTE_V3_FRAME data;
        data.cmdClass = COMMAND_CLASS_ASSOCIATION_GRP_INFO_V3;
        data.cmd = ASSOCIATION_GROUP_COMMAND_LIST_REPORT_V3;
        data.groupingIdentifier = cc_data[3];
        data.listLength = 2;
        data.command1 = COMMAND_CLASS_DEVICE_RESET_LOCALLY;
        data.command2 = DEVICE_RESET_LOCALLY_NOTIFICATION;
        znet_fc_send_data( node_id, func->_endpoint, sizeof data, (uint8_t*)&data, NULL, NULL, func->_security );
    }
}
