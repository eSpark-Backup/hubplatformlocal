
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

//#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"
//#include "znet_fc_private.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Send MultiChannel CapabilitiesGet
// Request information about the specified endpoint
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: endpoint
// Endpoint in question
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_capability_get( uint8_t node_id,
                                                                    uint8_t endpoint,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_MULTI_CHANNEL_V4,
                       MULTI_CHANNEL_CAPABILITY_GET_V4,
                       ( endpoint & MULTI_CHANNEL_CAPABILITY_GET_PROPERTIES1_END_POINT_MASK_V2 ) };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR MultichannelEncapsulate( uint8_t node_id,
                                                               uint8_t header_len,
                                                               uint8_t* header,
                                                               uint8_t cmd_data_len,
                                                               const uint8_t* cmd_data,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type ) {
    uint8_t data_len = header_len + cmd_data_len;
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    memcpy( &data[0], header, header_len );
    memcpy( &data[header_len], cmd_data, cmd_data_len );

    /// TODO: check channel support
    znet_error_t result = znet_fc_send_data( node_id, 0, data_len, data, Callback, callbackArg, encap_type );
    Heap_Free( data );
    return result;
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR
znet_cc_multichannel_cmd_encapsulate( uint8_t node_id,
                                      uint8_t src_endpoint,
                                      uint8_t dst_endpoint,
                                      uint8_t cmd_data_length,
                                      const uint8_t* cmd_data,
                                      ZFunction_Callback Callback,
                                      void* callbackArg,
                                      encap_type_t encap_type ) {  // we address only one instance
    uint8_t header[] = { COMMAND_CLASS_MULTI_CHANNEL_V4, MULTI_CHANNEL_CMD_ENCAP_V4, src_endpoint, dst_endpoint };
    return MultichannelEncapsulate(
        node_id, sizeof header, header, cmd_data_length, cmd_data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR znet_cc_multichannel_instance_cmd_encapsulate( uint8_t node_id,
                                                                      uint8_t instance,
                                                                      bool res,
                                                                      uint8_t cmd_data_length,
                                                                      const uint8_t* cmd_data,
                                                                      ZFunction_Callback Callback,
                                                                      void* callbackArg,
                                                                      encap_type_t encap_type ) {
    uint8_t header[] = { COMMAND_CLASS_MULTI_CHANNEL_V4,
                         MULTI_INSTANCE_CMD_ENCAP_V4,
                         ( res ? MULTI_INSTANCE_CMD_ENCAP_PROPERTIES1_RES_BIT_MASK_V4 : 0 ) |
                             ( instance & MULTI_INSTANCE_CMD_ENCAP_PROPERTIES1_INSTANCE_MASK_V4 ) };
    return MultichannelEncapsulate(
        node_id, sizeof header, header, cmd_data_length, cmd_data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send MultiChannel EndpointFind
// Note that MultiChannel EndpointFind Report is not supported as useless. But
// one can still trap the response packet in logs.
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: generic
// Generic type in search
//
// @param: specific
// Specific type in search
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_endpoint_find( uint8_t node_id,
                                                                   uint8_t generic,
                                                                   uint8_t specific,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_MULTI_CHANNEL_V4, MULTI_CHANNEL_END_POINT_FIND_V4, generic, specific };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send MultiChannel EndpointGet
// Get the number of available endpoints
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_endpoint_get( uint8_t node_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_MULTI_CHANNEL_V4, MULTI_CHANNEL_END_POINT_GET_V4 };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send MultiChannel Get (MultiInstance V1 command)
// Reports number of channels supporting a defined Command Class
// Depricated by MutliChannel V2 - needed for old devices only
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: cc_id
// Command Class Id in question
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_instance_get( uint8_t node_id,
                                                                  uint8_t cc_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_MULTI_CHANNEL_V4, MULTI_INSTANCE_GET, cc_id };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, encap_type );
}

void ICACHE_FLASH_ATTR znet_cc_multichannel_instance_report( const ZFunction func,
                                                             uint8_t node_id,
                                                             int cc_data_len,
                                                             const uint8_t* cc_data ) {}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_aggregated_members_get( uint8_t node_id,
                                                                            uint8_t aggregated_endpoint,
                                                                            ZFunction_Callback Callback,
                                                                            void* callbackArg,
                                                                            encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_MULTI_CHANNEL_V4, MULTI_CHANNEL_AGGREGATED_MEMBERS_GET_V4, aggregated_endpoint };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR znet_cc_multichannel_recieve( const ZFunction func,
                                                     uint8_t node_id,
                                                     int cc_data_len,
                                                     const uint8_t* cc_data ) {
    ZNet_ApplicationCommandHandler(
        (uint8_t*)&cc_data[4], cc_data_len - 4, node_id, func->_security, cc_data[2] & 0x7F );
}
