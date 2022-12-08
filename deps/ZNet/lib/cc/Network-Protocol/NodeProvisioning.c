
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "byte_ops.h"

#include "znet.h"

#include "ZPlatform.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Send Node Provisioning Report
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: sequence_number
// Sequence number
//
// @param: dsk
// DSK key
//
// @param: dsk_length
// DSK key length (0 or 16)
//
// @param: meta_data
// Meta data
//
// @param: metadata_length
// Meta data length
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_provisioning_report( uint8_t node_id,
                                                                 uint8_t channel_id,
                                                                 uint8_t sequence_number,
                                                                 uint8_t* dsk,
                                                                 uint8_t dsk_length,
                                                                 uint8_t* meta_data,
                                                                 uint8_t metadata_length,
                                                                 ZFunction_Callback callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type ) {
    uint8_t data_len = 4 + ( sizeof( uint8_t ) * dsk_length ) + ( sizeof( uint8_t ) * metadata_length );
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    data[0] = COMMAND_CLASS_NODE_PROVISIONING;
    data[1] = NODE_PROVISION_REPORT;
    data[2] = sequence_number;
    data[3] = dsk_length;
    if( dsk_length > 0 ) {
        memcpy( &data[4], dsk, sizeof( uint8_t ) * dsk_length );
    }
    if( metadata_length > 0 ) {
        memcpy( &data[4 + dsk_length], meta_data, sizeof( uint8_t ) * metadata_length );
    }

    znet_error_t result = znet_fc_send_data( node_id, channel_id, data_len, data, callback, callbackArg, encap_type );
    Heap_Free( data );

    return result;
}

//----------------------------------------------------------------------------
// Send Node Provisioning List Iteration Report
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: sequence_number
// Sequence number
//
// @param: remaining_count
// Remaining count
//
// @param: dsk
// DSK key
//
// @param: dsk_length
// DSK key length (0 or 16)
//
// @param: meta_data
// Meta data
//
// @param: metadata_length
// Meta data length
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_provisioning_list_iteration_report( uint8_t node_id,
                                                                                uint8_t channel_id,
                                                                                uint8_t sequence_number,
                                                                                uint8_t remaining_count,
                                                                                uint8_t* dsk,
                                                                                uint8_t dsk_length,
                                                                                uint8_t* meta_data,
                                                                                uint8_t metadata_length,
                                                                                ZFunction_Callback callback,
                                                                                void* callbackArg,
                                                                                encap_type_t encap_type ) {
    uint8_t data_len = 5 + ( sizeof( uint8_t ) * dsk_length ) + ( sizeof( uint8_t ) * metadata_length );
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    data[0] = COMMAND_CLASS_NODE_PROVISIONING;
    data[1] = NODE_PROVISIONING_LIST_ITERATION_REPORT;
    data[2] = sequence_number;
    data[3] = remaining_count;
    data[4] = dsk_length;
    if( dsk_length > 0 ) {
        memcpy( &data[5], dsk, sizeof( uint8_t ) * dsk_length );
    }
    if( metadata_length > 0 ) {
        memcpy( &data[5 + dsk_length], meta_data, sizeof( uint8_t ) * metadata_length );
    }

    znet_error_t result = znet_fc_send_data( node_id, channel_id, data_len, data, callback, callbackArg, encap_type );
    Heap_Free( data );

    return result;
}
